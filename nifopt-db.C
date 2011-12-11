#include "sqlite/sqlite3.h"

/*
** Used to prevent warnings about unused parameters
*/
#define UNUSED_PARAMETER(x) (void)(x)

/*
** These are the allowed modes.
*/
#define MODE_Line     0  /* One column per line.  Blank line between records */
#define MODE_Column   1  /* One record per line in neat columns */
#define MODE_List     2  /* One record per line with a separator */
#define MODE_Semi     3  /* Same as MODE_List but append ";" to each line */
#define MODE_Html     4  /* Generate an XHTML table */
#define MODE_Insert   5  /* Generate SQL "insert" statements */
#define MODE_Tcl      6  /* Generate ANSI-C or TCL quoted elements */
#define MODE_Csv      7  /* Quote strings, numbers are plain */
#define MODE_Explain  8  /* Like MODE_Column, but do not truncate data */

struct previous_mode_data {
  int valid;        /* Is there legit data in here? */
  int mode;
  int showHeader;
  int colWidth[100];
};

/*
** An pointer to an instance of this structure is passed from
** the main program to the callback.  This is used to communicate
** state and mode information.
*/
struct callback_data {
  sqlite3 *db;           /* The database */
  int echoOn;            /* True to echo input commands */
  int statsOn;           /* True to display memory stats before each finalize */
  int cnt;               /* Number of records displayed so far */
  FILE *out;             /* Write results here */
  int mode;              /* An output mode setting */
  int writableSchema;    /* True if PRAGMA writable_schema=ON */
  int showHeader;        /* True to show column names in List or Column mode */
  char *zDestTable;      /* Name of destination table when MODE_Insert */
  char separator[20];    /* Separator character for MODE_List */
  int colWidth[100];     /* Requested width of each column when in column mode*/
  int actualWidth[100];  /* Actual width of each column */
  char nullvalue[20];    /* The text to print when a NULL comes back from
                         ** the database */
  struct previous_mode_data explainPrev;
                         /* Holds the mode information just before
                         ** .explain ON */
  char outfile[FILENAME_MAX]; /* Filename for *out */
  const char *zDbFilename;    /* name of the database file */
  const char *zVfs;           /* Name of VFS to use */
  sqlite3_stmt *pStmt;   /* Current statement if any. */
  FILE *pLog;            /* Write log output here */
};

struct callback_data database;

/*
** The following is the open SQLite database.  We make a pointer
** to this database a static variable so that it can be accessed
** by the SIGINT handler to interrupt database processing.
*/
static sqlite3 *db = 0;

/*
** A global char* and an SQL function to access its current value 
** from within an SQL statement. This program used to use the 
** sqlite_exec_printf() API to substitue a string into an SQL statement.
** The correct way to do this with sqlite3 is to use the bind API, but
** since the shell is built around the callback paradigm it would be a lot
** of work. Instead just use this hack, which is quite harmless.
*/
static const char *zShellStatic = 0;
static void shellstaticFunc(
  sqlite3_context *context,
  int argc,
  sqlite3_value **argv
){
  assert( 0==argc );
  assert( zShellStatic );
  UNUSED_PARAMETER(argc);
  UNUSED_PARAMETER(argv);
  sqlite3_result_text(context, zShellStatic, -1, SQLITE_STATIC);
}

/*
** A callback for the sqlite3_log() interface.
*/
void shellLog(void *pArg, int iErrCode, const char *zMsg){
  struct callback_data *p = (struct callback_data*)pArg;
  if( p->pLog==0 ) return;
  fprintf(p->pLog, "(%d) %s\n", iErrCode, zMsg);
  fflush(p->pLog);
}

/*
** Make sure the database is open.  If it is not, then open it.  If
** the database fails to open, print an error message and exit.
*/
static void open_db(struct callback_data *p){
  if( p->db==0 ){
    sqlite3_open(p->zDbFilename, &p->db);
    db = p->db;
    if( db && sqlite3_errcode(db)==SQLITE_OK ){
      sqlite3_create_function(db, "shellstatic", 0, SQLITE_UTF8, 0,
          shellstaticFunc, 0, 0);
    }
    if( db==0 || SQLITE_OK!=sqlite3_errcode(db) ){
      fprintf(stderr,"Error: unable to open database \"%s\": %s\n", 
          p->zDbFilename, sqlite3_errmsg(db));
      exit(1);
    }
#ifndef SQLITE_OMIT_LOAD_EXTENSION
    sqlite3_enable_load_extension(p->db, 1);
#endif
  }
}

void sqlexec(const char *statement);
void sqlexec(string statement);

/*
** Initialize the state information in data
*/
void dbinit(const char *dbname) {
  memset(&database, 0, sizeof(database));
  database.mode = MODE_List;
  memcpy(database.separator,"|", 2);
  database.showHeader = 0;
  database.zDbFilename = dbname;
  sqlite3_config(SQLITE_CONFIG_URI, 1);
  sqlite3_config(SQLITE_CONFIG_LOG, shellLog, &database);
//sqlite3_snprintf(sizeof(mainPrompt), mainPrompt,"sqlite> ");
//sqlite3_snprintf(sizeof(continuePrompt), continuePrompt,"   ...> ");
  sqlite3_config(SQLITE_CONFIG_SINGLETHREAD);
#if defined(SQLITE_ENABLE_MEMSYS3) || defined(SQLITE_ENABLE_MEMSYS5)
  sqlite3_config(SQLITE_CONFIG_HEAP, malloc((int)(1024 * 1024)), (int)(1024 * 1024), 64);
#endif

  /* Go ahead and open the database file if it already exists.  If the
  ** file does not exist, delay opening it.  This prevents empty database
  ** files from being created if a user mistypes the database name argument
  ** to the sqlite command-line tool.
  */
  if (access(database.zDbFilename, 0) == 0) {
    open_db(&database);

    /* speed! */
    sqlexec("PRAGMA synchronous = OFF");
    sqlexec("PRAGMA temp_store = MEMORY");
    sqlexec("PRAGMA journal_mode = MEMORY");
    sqlexec("PRAGMA cache_size = 32768"); // hold 32MB of pages
  }
  else {
    fprintf(stderr, "Databse %s not found!", database.zDbFilename);
    exit(0);
  }
}

void dbexit() {
//set_table_name(&database, 0);
  if( database.zDestTable ){
    free(database.zDestTable);
    database.zDestTable = 0;
  }

  if (database.db) {
    sqlite3_close(database.db);
  }
}

/*
** This is the callback routine that the shell
** invokes for each row of a query result.
*/
int last_id = 0;
int shell_callback(void *pArg, int num_columns, char **result_columns, char **column_names){
  struct callback_data *p = (struct callback_data *)pArg;

  if (num_columns > 0) {
    for (int n = 0; n < num_columns; n++) {
      if (!stricmp(column_names[n], "id")) {
	last_id = atoi(result_columns[n]);
	break;
      }
    }
  }

  return 0;
}

void sqlexec(string statement) {
  char *zErrMsg; last_id = 0;
  int rc = sqlite3_exec(database.db, statement.data(), shell_callback, &database, &zErrMsg);
  if (zErrMsg) {
    fprintf(stderr, zErrMsg);
  }
  if (!last_id) last_id = (int)sqlite3_last_insert_rowid(database.db);
}

void sqlexec(const char *statement) {
  char *zErrMsg; last_id = 0;
  int rc = sqlite3_exec(database.db, statement, shell_callback, &database, &zErrMsg);
  if (zErrMsg) {
    fprintf(stderr, zErrMsg);
  }
  if (!last_id) last_id = (int)sqlite3_last_insert_rowid(database.db);
}

/* ---------------------------------------------------------------------------------------------- */

char statement[2048];

const char *tohex(unsigned int num) {
  static char buf[256];
  sprintf(buf, "%08x", num);
  return buf;
}

char lastcsp[1024] = "\0";
unsigned int lastcs = 0U;
unsigned int checksum(const char *pathname) {
  /* LRU of entire path */
  if (!stricmp(lastcsp, pathname))
    return lastcs;

  struct iofile *src = ioopenfile(pathname, "r");
  if (!src)
    return 0;

  size_t cnt = iosize(pathname), blk, rsr;
  void *mem = malloc(1024 * 1024);
  unsigned int adler = adler32(0,0,0);

  /* compare with small buffer */
  do {
    blk = min(cnt, 1024 * 1024);

    if ((rsr = ioreadfile(mem, blk, src)) != blk) {
      break;
      throw runtime_error("Reading failed!");
    }

    adler = adler32(adler, (const Bytef *)mem, (uInt)blk);

    cnt -= blk;
  } while (cnt != 0);

  ioclosefile(src);
  free(mem);

  strcpy(lastcsp, pathname);
    return lastcs = adler;
}

void breakpath(const char *pathname, char *prefix, char *archive, char *path, char *name) {
  char *walk = strdup(pathname);
  char *shrt = walk + strlen(walk);

  prefix[0] = archive[0] = path[0] = name[0] = '\0';

  while (shrt) {
    *shrt = '\0';

    /* BSAs directly reside in "Data/" */
    if (isext(walk, "bsa")) {
      if (strlen(walk) < strlen(pathname)) {
	strcpy(path, pathname + strlen(walk) + 1);
	/* has sub-directories */
	if ((shrt = strrchr(path, '\\'))) {
	  *shrt = '\0';
	  strcpy(name, shrt + 1);
	}
	else {
	  strcpy(name, path);
	  path[0]  = '\0';
	}
      }

      if ((shrt = strrchr(walk, '\\'))) {
	*shrt = '\0';

	strcpy(archive, shrt + 1);
	strcpy(prefix, walk);
      }
      else
	strcpy(archive, walk);

      free(walk);
      return;
    }

    /* resides in "Data/" */
    if (!stricmp(walk + strlen(walk) - 4, "data")) {
      if (strlen(walk) < strlen(pathname)) {
	strcpy(path, pathname + strlen(walk) + 1);
	/* has sub-directories */
	if ((shrt = strrchr(path, '\\'))) {
	  *shrt = '\0';
	  strcpy(name, shrt + 1);
	}
	else {
	  strcpy(name, path);
	  path[0]  = '\0';
	}
      }

      strcpy(prefix, walk);

      free(walk);
      return;
    }

    shrt = strrchr(walk, '\\');
  }

  /* nowhere */
  strcpy(path, pathname);
  /* has sub-directories */
  if ((shrt = strrchr(path, '\\'))) {
    *shrt = '\0';
    strcpy(name, shrt + 1);
  }
  else {
    strcpy(name, path);
    path[0]  = '\0';
  }

  free(walk);
  return;
}


enum {
  FT_NIF = OB_BSAFILE_NIF << 0,
  FT_KF  = OB_BSAFILE_NIF << 1,
  FT_DDS = OB_BSAFILE_DDS << 0,
  FT_XML = OB_BSAFILE_XML << 0,
  FT_WAV = OB_BSAFILE_WAV << 0,
  FT_MP3 = OB_BSAFILE_MP3 << 0,
  FT_TXT = OB_BSAFILE_TXT << 0,
  FT_BAT = OB_BSAFILE_BAT << 0,
  FT_SCC = OB_BSAFILE_SCC << 0,
  FT_SPT = OB_BSAFILE_SPT << 0,
  FT_TEX = OB_BSAFILE_TEX << 0,
  FT_FNT = OB_BSAFILE_FNT << 0,
  FT_CTL = OB_BSAFILE_CTL << 0,
  FT_HTM = OB_BSAFILE_HTML << 0,
  FT_LIP = OB_BSAFILE_LIP << 0,
  FT_BIK = OB_BSAFILE_BIK << 0,
  FT_JPG = OB_BSAFILE_JPG << 0,
  FT_TRI = (OB_BSAFILE_JPG + 1) << 0,
  FT_EGM = (OB_BSAFILE_JPG + 2) << 0,
  FT_EGT = (OB_BSAFILE_JPG + 3) << 0,
  FT_LOD = (OB_BSAFILE_JPG + 4) << 0,
};

int getfiletype(const char *path) {
  char ext[256];
  const char *buf = getext(ext, path);

  /**/ if (!stricmp(buf, "nif")) return FT_NIF;
  else if (!stricmp(buf, "kf" )) return FT_KF;
  else if (!stricmp(buf, "dds")) return FT_DDS;
  else if (!stricmp(buf, "xml")) return FT_XML;
  else if (!stricmp(buf, "wav")) return FT_WAV;
  else if (!stricmp(buf, "mp3")) return FT_MP3;
  else if (!stricmp(buf, "txt")) return FT_TXT;
  else if (!stricmp(buf, "bat")) return FT_BAT;
  else if (!stricmp(buf, "scc")) return FT_SCC;
  else if (!stricmp(buf, "spt")) return FT_SPT;
  else if (!stricmp(buf, "tex")) return FT_TEX;
  else if (!stricmp(buf, "fnt")) return FT_FNT;
  else if (!stricmp(buf, "cnt")) return FT_CTL;

  else if (!stricmp(buf, "html")) return FT_HTM;

  else if (!stricmp(buf, "lip")) return FT_LIP;
  else if (!stricmp(buf, "bik")) return FT_BIK;
  else if (!stricmp(buf, "jpg")) return FT_JPG;

  else if (!stricmp(buf, "tri")) return FT_TRI;
  else if (!stricmp(buf, "egm")) return FT_EGM;
  else if (!stricmp(buf, "egt")) return FT_EGT;
  else if (!stricmp(buf, "lod")) return FT_LOD;

  return 0;
}

/* ---------------------------------------------------------------------------------------------- */

int sqlite3_selectid(sqlite3_stmt *prepared) {
  last_id = 0;

  int res = sqlite3_step(prepared);
  if (res == SQLITE_ROW) {
    last_id = (int)sqlite3_column_int64(prepared, 0);
  }
  else if (res == SQLITE_ERROR) {
    fprintf(stderr, sqlite3_errmsg(database.db));
  }

  sqlite3_clear_bindings(prepared);
  sqlite3_reset(prepared);

  return last_id;
}

int sqlite3_selectid_tstamp(sqlite3_stmt *prepared, int *tstamp) {
  last_id = 0;

  int res = sqlite3_step(prepared);
  if (res == SQLITE_ROW) {
    last_id = (int)sqlite3_column_int64(prepared, 0);
    *tstamp = (int)sqlite3_column_int(prepared, 1);
  }
  else if (res == SQLITE_ERROR) {
    fprintf(stderr, sqlite3_errmsg(database.db));
  }

  sqlite3_clear_bindings(prepared);
  sqlite3_reset(prepared);

  return last_id;
}

int sqlite3_insertid(sqlite3_stmt *prepared) {
  last_id = 0;

  int res = sqlite3_step(prepared);
  if ((res == SQLITE_OK) || (res == SQLITE_DONE)) {
    last_id = (int)sqlite3_last_insert_rowid(database.db);
  }
  else if (res == SQLITE_ERROR) {
    fprintf(stderr, sqlite3_errmsg(database.db));
  }

  sqlite3_clear_bindings(prepared);
  sqlite3_reset(prepared);

  return last_id;
}

void sqlite3_delete(sqlite3_stmt *prepared) {
  int res = sqlite3_step(prepared);
  if ((res == SQLITE_OK) || (res == SQLITE_DONE)) {
    last_id = 0;
  }
  else if (res == SQLITE_ERROR) {
    fprintf(stderr, sqlite3_errmsg(database.db));
  }

  sqlite3_clear_bindings(prepared);
  sqlite3_reset(prepared);
}

/* ---------------------------------------------------------------------------------------------- */

int ddsformat(DWORD FMT) {
  static const char *sstatement = "SELECT id FROM ddsformats WHERE id = @ID;";
  static const char *istatement = "INSERT INTO ddsformats (id, name) VALUES (@ID, @NM);";
  static sqlite3_stmt *sprepared = NULL;
  static sqlite3_stmt *iprepared = NULL;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if (!sprepared) sqlite3_prepare_v2(database.db, sstatement, 512, &sprepared, NULL);

  sqlite3_bind_int(sprepared, 1, FMT);
  if ((last_id = sqlite3_selectid(sprepared)))
    return FMT;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if (!iprepared) sqlite3_prepare_v2(database.db, istatement, 512, &iprepared, NULL);

  sqlite3_bind_int (iprepared, 1, FMT);
  sqlite3_bind_text(iprepared, 2, findFormat((D3DFORMAT)FMT), -1, SQLITE_TRANSIENT);
  if ((last_id = sqlite3_insertid(iprepared)))
    return FMT;

  return 0;
}

/* ---------------------------------------------------------------------------------------------- */

int nifblock(const char *name) {
  static const char *sstatement = "SELECT id FROM nifblocks WHERE name = @NM;";
  static const char *istatement = "INSERT INTO nifblocks (name) VALUES (@NM);";
  static sqlite3_stmt *sprepared = NULL;
  static sqlite3_stmt *iprepared = NULL;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if (!sprepared) sqlite3_prepare_v2(database.db, sstatement, 512, &sprepared, NULL);

  sqlite3_bind_text(sprepared, 1, name, -1, SQLITE_TRANSIENT);
  if ((last_id = sqlite3_selectid(sprepared)))
    return last_id;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if (!iprepared) sqlite3_prepare_v2(database.db, istatement, 512, &iprepared, NULL);

  sqlite3_bind_text(iprepared, 1, name, -1, SQLITE_TRANSIENT);
  if ((last_id = sqlite3_insertid(iprepared)))
    return last_id;

  return 0;
}

/* ---------------------------------------------------------------------------------------------- */

char lastpp[1024] = "\0"; int lastp = 0;

int getpath(const char *path) {
  /* LRU of entire path */
  if (!stricmp(lastpp, path))
    return lastp;

  static const char *sstatement = "SELECT id FROM paths WHERE path = @PTH;";
  static const char *istatement = "INSERT INTO paths (path) VALUES (@PTH);";
  static sqlite3_stmt *sprepared = NULL;
  static sqlite3_stmt *iprepared = NULL;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if (!sprepared) sqlite3_prepare_v2(database.db, sstatement, 512, &sprepared, NULL);

  sqlite3_bind_text(sprepared, 1, path, -1, SQLITE_TRANSIENT);
  if ((last_id = sqlite3_selectid(sprepared))) {
    strcpy(lastpp, path);
    return lastp = last_id;
  }

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if (!iprepared) sqlite3_prepare_v2(database.db, istatement, 512, &iprepared, NULL);

  sqlite3_bind_text(iprepared, 1, path, -1, SQLITE_TRANSIENT);
  if ((last_id = sqlite3_insertid(iprepared))) {
    strcpy(lastpp, path);
    return lastp = last_id;
  }

  return 0;
}

/* ---------------------------------------------------------------------------------------------- */

int getpackage(const char *name, int v, int sv1, int sv2) {
  static const char *sstatement = "SELECT id FROM packages WHERE name = @NM AND version = @V AND subversion_major = @SV1 AND subversion_minor = @SV2;";
  static const char *istatement = "INSERT INTO packages (name, version, subversion_major, subversion_minor) VALUES (@NM, @V, @SV1, @SV2);";
  static sqlite3_stmt *sprepared = NULL;
  static sqlite3_stmt *iprepared = NULL;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if (!sprepared) sqlite3_prepare_v2(database.db, sstatement, 512, &sprepared, NULL);

  sqlite3_bind_text(sprepared, 1, name, -1, SQLITE_TRANSIENT);
  sqlite3_bind_int (sprepared, 2, v);
  sqlite3_bind_int (sprepared, 3, sv1);
  sqlite3_bind_int (sprepared, 4, sv2);
  if ((last_id = sqlite3_selectid(sprepared)))
    return last_id;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if (!iprepared) sqlite3_prepare_v2(database.db, istatement, 512, &iprepared, NULL);

  sqlite3_bind_text(iprepared, 1, name, -1, SQLITE_TRANSIENT);
  sqlite3_bind_int (iprepared, 2, v);
  sqlite3_bind_int (iprepared, 3, sv1);
  sqlite3_bind_int (iprepared, 4, sv2);
  if ((last_id = sqlite3_insertid(iprepared)))
    return last_id;

  return 0;
}

/* ---------------------------------------------------------------------------------------------- */

char lastap[1024] = "\0"; int lastapc = 0; int lasta = 0;

int getarchive(int package, const char *path, int *cstamp, bool create = true) {
  /* break into parts */
  char prefix[256], archive[256], subpath[256], name[256];
  breakpath(path, prefix, archive, subpath, name);

  if (archive[0]) {
    /* LRU of prefix of path */
    if ((lastapc == package) && lastap[0] && (path == stristr(path, lastap)))
      return lasta;

    static const char *sstatement = "SELECT id, cstamp FROM archives WHERE package = @PCK AND path = @NM;";
    static const char *istatement = "INSERT INTO archives (package, path, cstamp) VALUES (@PCK, @NM, @CS);";
    static sqlite3_stmt *sprepared = NULL;
    static sqlite3_stmt *iprepared = NULL;

    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
    if (!sprepared) sqlite3_prepare_v2(database.db, sstatement, 512, &sprepared, NULL);

    sqlite3_bind_int (sprepared, 1, package);
    sqlite3_bind_text(sprepared, 2, archive, -1, SQLITE_TRANSIENT);
    if ((last_id = sqlite3_selectid_tstamp(sprepared, cstamp))) {
      lastapc = package; strcpy(lastap, path);
      return lasta = last_id;
    }

    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
    if (!create)
      return 0;

    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
    if (!iprepared) sqlite3_prepare_v2(database.db, istatement, 512, &iprepared, NULL);

    sqlite3_bind_int (iprepared, 1, package);
    sqlite3_bind_text(iprepared, 2, archive, -1, SQLITE_TRANSIENT);
    sqlite3_bind_int (iprepared, 3, *cstamp); *cstamp = 0;
    if ((last_id = sqlite3_insertid(iprepared))) {
      lastapc = package; strcpy(lastap, path);
      return lasta = last_id;
    }
  }

  return 0;
}

/* ---------------------------------------------------------------------------------------------- */

char lastfp[1024] = "\0"; int lastfpc = 0; int lastf = 0;

int getfile(int package, const char *path, int *tstamp, int filetype, int archive) {
  int cstamp = 0;

  /* break into parts */
  char prefix[256], _archive[256], subpath[256], name[256];
  breakpath(path, prefix, _archive, subpath, name);

  if (archive == -1)
    archive = getarchive(package, path, &cstamp, false);

  if (name[0]) {
    /* LRU of entire path */
    if ((lastfpc == package) && lastfp[0] && !stricmp(lastfp, path))
      return lastf;

    static const char *sstatement = "SELECT id, tstamp FROM files WHERE package = @PCK AND archive = @ARC AND path = @PTH AND name = @NM;";
    static const char *istatement = "INSERT INTO files (package, archive, path, name, checksum, tstamp, filesize, filetype) VALUES (@PCK, @ARC, @PTH, @NM, @CS, @TS, @SZ, @FT);";
    static sqlite3_stmt *sprepared = NULL;
    static sqlite3_stmt *iprepared = NULL;

    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
    if (!sprepared) sqlite3_prepare_v2(database.db, sstatement, 512, &sprepared, NULL);

    sqlite3_bind_int (sprepared, 1, package);
    sqlite3_bind_int (sprepared, 2, archive);
    sqlite3_bind_int (sprepared, 3, getpath(subpath));
    sqlite3_bind_text(sprepared, 4, name, -1, SQLITE_TRANSIENT);
    if ((last_id = sqlite3_selectid_tstamp(sprepared, tstamp))) {
      lastfpc = package; strcpy(lastfp, path);
      return lastf = last_id;
    }

    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
    if (!iprepared) sqlite3_prepare_v2(database.db, istatement, 512, &iprepared, NULL);

    sqlite3_bind_int (iprepared, 1, package);
    sqlite3_bind_int (iprepared, 2, archive);
    sqlite3_bind_int (iprepared, 3, getpath(subpath));
    sqlite3_bind_text(iprepared, 4, name, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(iprepared, 5, tohex(checksum(path)), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int (iprepared, 6, *tstamp); *tstamp = 0;
    sqlite3_bind_int (iprepared, 7, iosize(path));
    sqlite3_bind_int (iprepared, 8, filetype);
    if ((last_id = sqlite3_insertid(iprepared))) {
      lastfpc = package; strcpy(lastfp, path);
      return lastf = last_id;
    }
  }

  return 0;
}

int getfile(int package, const char *path, int *tstamp, int filetype) {
  return getfile(package, path, tstamp, filetype, -1);
}

int getfile(int package, const char *path, int *tstamp) {
  return getfile(package, path, tstamp, getfiletype(path));
}

/* ---------------------------------------------------------------------------------------------- */

void markblock(int nif, const char *block) {
  static const char *istatement = "INSERT INTO file_blocks (file, block) VALUES (@FL, @BL);";
  static sqlite3_stmt *iprepared = NULL;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if (!iprepared) sqlite3_prepare_v2(database.db, istatement, 512, &iprepared, NULL);

  sqlite3_bind_int (iprepared, 1, nif);
  sqlite3_bind_int (iprepared, 2, nifblock(block));
  sqlite3_insertid(iprepared);
}

void marktexture(int nif, const char *path, bool trans, bool spec, bool bump, int slot, int mode, int aformat, int mips) {
  /* break into parts */
  char prefix[256], archive[256], subpath[256], name[256];
  breakpath(path, prefix, archive, subpath, name);

  if (name[0]) {
    static const char *istatement = "INSERT INTO file_texture (model, texturepath, texturename, transparent, specular, heightfield, applymode, slot, aformat, mips) VALUES (@MDL, @PTH, @NM, @TR, @SP, @HM, @AM, @SL, @AF, @MIP);";
    static sqlite3_stmt *iprepared = NULL;

    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
    if (!iprepared) sqlite3_prepare_v2(database.db, istatement, 512, &iprepared, NULL);

    sqlite3_bind_int (iprepared, 1, nif);
    sqlite3_bind_int (iprepared, 2, getpath(subpath));
    sqlite3_bind_text(iprepared, 3, name, -1, SQLITE_TRANSIENT);
    sqlite3_bind_int (iprepared, 4, trans);
    sqlite3_bind_int (iprepared, 5, spec);
    sqlite3_bind_int (iprepared, 6, bump);
    sqlite3_bind_int (iprepared, 7, mode);
    sqlite3_bind_int (iprepared, 8, slot);
    sqlite3_bind_int (iprepared, 9, aformat);
    sqlite3_bind_int (iprepared, 10, mips);
    sqlite3_insertid(iprepared);
  }
}

#define ALPHA_NO	0
#define ALPHA_WHITE	1
#define ALPHA_BLACK	2
#define ALPHA_GREY	3

void markformat(int tex, DWORD format, int width, int height, bool alpha, bool alphawhite, bool alphablack) {
  static const char *istatement = "INSERT INTO file_format (file, format, width, height, alpha) VALUES (@FLE, @FMT, @W, @H, @AC);";
  static sqlite3_stmt *iprepared = NULL;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if (!iprepared) sqlite3_prepare_v2(database.db, istatement, 512, &iprepared, NULL);

  sqlite3_bind_int (iprepared, 1, tex);
  sqlite3_bind_int (iprepared, 2, ddsformat(format));
  sqlite3_bind_int (iprepared, 3, width);
  sqlite3_bind_int (iprepared, 4, height);
  sqlite3_bind_int (iprepared, 5, (!alpha ? ALPHA_NO : (alphawhite ? ALPHA_WHITE : (alphablack ? ALPHA_BLACK : ALPHA_GREY))));
  sqlite3_insertid(iprepared);
}

/* ---------------------------------------------------------------------------------------------- */

/* auto-propagation (via foreign keys) */
void flusharchive(int package, int archive, const char *path) {
  /* flush only if the archive-file is hit directly */
  if (archive) {
    static const char *dstatement = "DELETE FROM archives WHERE WHERE id = @ARC;";
    static sqlite3_stmt *dprepared = NULL;

    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
    if (!dprepared) sqlite3_prepare_v2(database.db, dstatement, 512, &dprepared, NULL);

    sqlite3_bind_int (dprepared, 1, archive);
    sqlite3_delete(dprepared);
  }
}

/* auto-propagation (via foreign keys) */
void flusharchive(int package, const char *path) {
  /* break into parts */
  char prefix[256], archive[256], subpath[256], name[256];
  breakpath(path, prefix, archive, subpath, name);

  /* flush only if the archive-file is hit directly */
  if (archive[0] && !subpath[0] && !name[0]) {
    static const char *dstatement = "DELETE FROM archives WHERE id = @ARC;";
    static sqlite3_stmt *dprepared = NULL;

    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
    if (!dprepared) sqlite3_prepare_v2(database.db, dstatement, 512, &dprepared, NULL);

    sqlite3_bind_int (dprepared, 1, getarchive(package, archive, false));
    sqlite3_delete(dprepared);
  }
}

/* auto-propagation (via foreign keys) */
void flushfile(int package, int archive, const char *path) {
  /* break into parts */
  char prefix[256], _archive[256], subpath[256], name[256];
  breakpath(path, prefix, _archive, subpath, name);

  if (name[0]) {
    if (archive == -1)
      archive = getarchive(package, _archive, false);

    static const char *dstatement = "DELETE FROM files WHERE package = @PCK AND archive = @ARC AND path = @PTH AND name = @NM;";
    static sqlite3_stmt *dprepared = NULL;

    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
    if (!dprepared) sqlite3_prepare_v2(database.db, dstatement, 512, &dprepared, NULL);

    sqlite3_bind_int (dprepared, 1, package);
    sqlite3_bind_int (dprepared, 2, archive);
    sqlite3_bind_int (dprepared, 3, getpath(subpath));
    sqlite3_bind_text(dprepared, 4, name, -1, SQLITE_TRANSIENT);
    sqlite3_delete(dprepared);
  }
}

/* auto-propagation (via foreign keys) */
void flushfile(int package, const char *path) {
  flushfile(package, -1, path);
}

/* ---------------------------------------------------------------------------------------------- */

int package = 0, dbarchive = 0, dbfile = 0;
int nif = 0, dds = 0;
int ft = 0;

/* ---------------------------------------------------------------------------------------------- */

#ifdef	TEXTUREOPT
bool AnalyzeDDS(const char *inname, const char *rep) {
    /* read the DDS */
    void *inmem = NULL;
    UINT insize =    0;

    /* look what we'v got */
    struct ioinfo info;
    if (iostat(inname, &info)) {
      errprintf(stderr, "can't read the DDS\n");
      return false;
    }

    /* read in the file-contents */
    struct iofile *file;
    if ((file = ioopenfile(inname, "rb"))) {
      inmem = malloc(insize = (UINT)info.io_size);

      ioreadfile(inmem, insize, file);
      ioclosefile(file);
    }
    else {
      errprintf(stderr, "can't read the DDS\n");
      return false;
    }

    /* only if we got something */
    if (inmem && insize) {
      LPDIRECT3DTEXTURE9 base = NULL;
      HRESULT res;

      /* extract levels the hard way (if there is 0 in the file, make it 1) */
      int mipl = 1;
      if (((DWORD *)inmem)[0] == MAKEFOURCC('D', 'D', 'S', ' '))
	mipl = ((DWORD *)inmem)[7];
      if (!mipl)
	mipl = 1;

      if ((res = D3DXCreateTextureFromFileInMemoryEx(
	pD3DDevice, inmem, insize,
	D3DX_DEFAULT, D3DX_DEFAULT, mipl/*D3DX_DEFAULT*/,
	0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_FILTER_NONE/*D3DX_DEFAULT*/,
	D3DX_FILTER_NONE/*D3DX_DEFAULT*/, 0, NULL, NULL,
	&base
      )) == D3D_OK) {

//    if ((res = D3DXCreateTextureFromFileInMemory(
//	pD3DDevice, inmem, insize, &base
//    )) == D3D_OK) {

	/* the lowest mip-level contains a row or a column of 4x4 blocks
	 * we won't generate mip-levels for mips smaller than the BTC-area
	 */
	D3DSURFACE_DESC based;
	base->GetLevelDesc(0, &based);
	int levelc = base->GetLevelCount();
	short depth = findFormatDepth(based.Format);
	char channels = findFormatChannels(based.Format);
	const char *from = findFormat(based.Format);
	bool uncompressed = false, unsupported = false;
	bool istransp = false;
	bool cangamma = !stristr(inname, "faces\\");

	/* exclude menus (and recognize they are alphas) */
	int levels = 1;
	if (stristr(inname, "\\menus"))
	  istransp = true;
	else {
	  int ww = based.Width;
	  int hh = based.Height;
	  while ((ww > 1) && (hh > 1)) {
	    ww = (ww + 1) >> 1;
	    hh = (hh + 1) >> 1;

	    levels++;
	  }
	}

	if (levels > levelc)
	  addnote(" Missing %d mip-level(s).\n", levels - levelc);
	else if (levels < levelc)
	  addnote(" Excess of %d mip-level(s).\n", levelc - levels);

	if ((based.Format != D3DFMT_DXT1) &&
	    (based.Format != D3DFMT_DXT2) &&
	    (based.Format != D3DFMT_DXT3) &&
	    (based.Format != D3DFMT_DXT4) &&
	    (based.Format != D3DFMT_DXT5) &&
	    (based.Format != D3DFMT_ATI1) &&
	    (based.Format != D3DFMT_ATI2))
	  uncompressed = true;
	if ((based.Format == D3DFMT_V8U8) ||
	    (based.Format == D3DFMT_Q8W8V8U8) ||
	    (based.Format == D3DFMT_V16U16)  ||
	    (based.Format == D3DFMT_Q16W16V16U16) ||
	    (based.Format == D3DFMT_CxV8U8))
	  unsupported = true;

	if (unsupported)
	  addnote(" Texture has unsupported format.\n"), fixedtexts++;

	/* file-size sanity check */
	if (levelc > 1) {
	  unsigned int bsize = 0;
	  switch (based.Format) {
	    case D3DFMT_DXT5:
	    case D3DFMT_DXT4:
	    case D3DFMT_DXT3:
	    case D3DFMT_DXT2:
	    case D3DFMT_ATI2:
	      bsize = 0;
	      for (int l = 0; l < levelc; l++)
		bsize += ((((based.Width  + (1 << l) - 1) >> l) + 3) / 4) *
			 ((((based.Height + (1 << l) - 1) >> l) + 3) / 4) * 16;
	      break;
	    case D3DFMT_DXT1:
	    case D3DFMT_ATI1:
	      bsize = 0;
	      for (int l = 0; l < levelc; l++)
		bsize += ((((based.Width  + (1 << l) - 1) >> l) + 3) / 4) *
			 ((((based.Height + (1 << l) - 1) >> l) + 3) / 4) * 8;
	      break;
	    case D3DFMT_A4L4:
	    case D3DFMT_A4R4G4B4:
	      bsize = 0;
	      for (int l = 0; l < levelc; l++)
		bsize += ((((based.Width  + (1 << l) - 1) >> l) + 0) / 1) *
			 ((((based.Height + (1 << l) - 1) >> l) + 0) / 1) * channels >> 1;
	      break;
	    default:
	      bsize = 0;
	      for (int l = 0; l < levelc; l++)
		bsize += ((((based.Width  + (1 << l) - 1) >> l) + 0) / 1) *
			 ((((based.Height + (1 << l) - 1) >> l) + 0) / 1) * channels;
	      break;
	  }

	  if (bsize > insize)
	    addnote(" Truncated texture found!\n");
	}

	/* choose what to do */
	int relevel = 0;
	if (levels > levelc)
	  relevel = levelc + 1;
	else if (levels < levelc)
	  relevel = 0 - levels;
	else if (levels == 1)
	  relevel = 0 - levels;

	bool alphawhite = true;
	bool alphablack = true;

	/* normal maps */
	bool defblack = false;
	if (issuf(inname, "_n") ||
	    issuf(inname, "_fn") ||
	    issuf(inname, "_xyz") ||
	    issuf(inname, "_xyzd"))
	  defblack = true;

	if (TextureConvert(based, &base, defblack)) {
	  D3DLOCKED_RECT texs;
	  base->LockRect(0, &texs, NULL, 0);
	  ULONG *sTex = (ULONG *)texs.pBits;

	  for (int y = 0; y < (int)based.Height; y += 1) {
	  for (int x = 0; x < (int)based.Width ; x += 1) {
	    ULONG t = sTex[(y * based.Width) + x];
	    ULONG a = (t >> 24) & 0xFF; /*a*/
	    ULONG r = (t >> 16) & 0xFF; /*a*/
	    ULONG g = (t >>  8) & 0xFF; /*a*/
	    ULONG b = (t >>  0) & 0xFF; /*a*/

	    if (a != 0x00) alphablack = false;
	    if (a != 0xFF) alphawhite = false;
	  }
	  }
	}

	/* register the node-type */
	markformat(dbfile, based.Format, based.Width, based.Height, findAlpha(based.Format), alphawhite, alphablack);

#if 0
	  nfoprintf(stdout, " Format           : %s to %s\n", from, findFormat(based.Format));
	  nfoprintf(stdout, " Dimensions       : %dx%d - %d to %d levels\n", based.Width, based.Height, levelc, levels);
#endif

	base->Release();
      }
      else
	errprintf(stderr, "can't read the DDS\n");
    }

    /* dummy content (copy over) */
    bool success = true;

    if (inmem) free(inmem);

    return success;
}
#endif

/* ---------------------------------------------------------------------------------------------- */

#ifdef	SOUNDOPT
bool AnalyzeWave(const char *inname, const char *rep) {
  st_signalinfo_t filein; ft_t descin;

  memset(&filein, 0, sizeof(filein)); descin = NULL;

  filein.rate     =  0;
  filein.size     = -1;
  filein.encoding = -1;
  filein.channels = -1;

  bool success = false;
  if ((descin = st_open_read(inname, &filein, "wav"))) {
    wav_t wavin = (wav_t)descin->priv;

    if (filein.rate     ==  0) filein.rate     = descin->info.rate;
    if (filein.size     == -1) filein.size     = descin->info.size;
    if (filein.encoding == -1) filein.encoding = descin->info.encoding;
    if (filein.channels == -1) filein.channels = descin->info.channels;

    /* copy loop info, resizing appropriately
     * it's in samples, so # channels don't matter
     * FIXME: This doesn't work for multi-file processing or
     * effects that change file length.
     */
    double factor = (double)filein.rate / (double)filein.rate;
    st_loopinfo_t loops[ST_MAX_NLOOPS];
    for (int i = 0; i < ST_MAX_NLOOPS; i++) {
      loops[i].start  = (st_size_t)(descin->loops[i].start * factor);
      loops[i].length = (st_size_t)(descin->loops[i].length * factor);
      loops[i].count  = descin->loops[i].count;
      loops[i].type   = descin->loops[i].type;
    }

    double time = (double)wavin->numSamples / filein.rate;
    int hours, minutes, seconds, millis;
    hours   = (int)floor(time / (60 * 60)); time -= hours * 60 * 60;
    minutes = (int)floor(time / (     60)); time -= minutes * 60;
    seconds = (int)floor(time            ); time -= seconds;
    millis  = (int)floor(time * 1000     );

#if 0
    nfoprintf(stdout, " Format           : \"%s\" to \"%s\"\n", wav_format_str(wavin->formatTag), wav_format_str(wavou->formatTag));
    nfoprintf(stdout, " Channels         : %d, %s\n", filein.channels, filein.channels == 1 ? "Mono" : "Stereo");
    nfoprintf(stdout, " Bits / Sample    : %d bits\n", filein.size * 8);
    nfoprintf(stdout, " Length           : %d:%02d:%02d.%03d\n", hours, minutes, seconds, millis);
    nfoprintf(stdout, " Samples / s      : %d Hz\n", filein.rate);
#endif

    success = true;
    st_close(descin);
  }

  return success;
}
#endif

/* ---------------------------------------------------------------------------------------------- */

void StepProperties(NiAVObjectRef node) {
  vector< Ref<NiProperty> > prps = node->GetProperties();
  vector< Ref<NiProperty> >::iterator pi;

  /* default "missing" texture channel values */
  Niflib::Color3 emc(1.0, 1.0, 1.0);
  Niflib::Color3 spc(1.0, 1.0, 1.0);
  float gls = 1.0;

  for (pi = prps.begin(); pi != prps.end(); pi++) {
    string pt = (*pi)->GetType().GetTypeName();

    /**/ if (pt == "NiVertexColorProperty")
      ;
    else if (pt == "NiAlphaProperty")
      ;
    else if (pt == "NiSpecularProperty")
      ;
    else if (pt == "NiMaterialProperty") {
      NiMaterialPropertyRef mt = DynamicCast<NiMaterialProperty>(*pi);

      emc = mt->GetEmissiveColor();
      spc = mt->GetSpecularColor();
      gls = mt->GetGlossiness();
    }
  }

  for (pi = prps.begin(); pi != prps.end(); pi++) {
    string pt = (*pi)->GetType().GetTypeName();

    /**/ if (pt == "NiTexturingProperty") {
      NiTexturingPropertyRef tx = DynamicCast<NiTexturingProperty>(*pi);

      bool trans = false;
      bool spec = false;
      bool bump = false;

      ApplyMode mode;
      switch ((mode = tx->GetApplyMode())) {
	/* (eradicate all underlying blended textures?) */
	case APPLY_REPLACE:  trans = true;  bump = false; spec = false; break;
	/* decal surface (must-have alpha, no illumination) */
	case APPLY_DECAL:    trans = true;  bump = false; spec = false; break;
	/* regular surface (can-have alpha) */
	case APPLY_MODULATE: trans = true;  bump = false; spec = false; break;
	/* reflective surface (no diffuse, no alpha) */
	case APPLY_HILIGHT:  trans = false; bump = false; spec = true; break;
	/* parallax surface (no alpha but height-map) */
	case APPLY_HILIGHT2: trans = false; bump = true;  spec = true; break;
      }

      int txc = tx->GetTextureCount();
      for (int txs = 0; txs < txc; txs++) {
	TexDesc txd = tx->GetTexture(txs);
	NiSourceTextureRef src = txd.source;

	if (src && src->IsTextureExternal()) {
	  string path = src->GetTextureFileName();

	  switch (txs) {
	    case BASE_MAP:
	    case DARK_MAP:
	    case DETAIL_MAP:
	    case GLOSS_MAP:
	    case GLOW_MAP:
	    case BUMP_MAP:
	    case NORMAL_MAP:
	    case UNKNOWN2_MAP:
	    case DECAL_0_MAP:
	    case DECAL_1_MAP:
	    case DECAL_2_MAP:
	    case DECAL_3_MAP:
	      break;
	  }

	  AlphaFormat afmt = src->GetAlphaFormat();
	  MipMapFormat mfmt = src->GetMipMapFormat();

	  marktexture(dbfile, path.data(), trans, spec, bump, txs, mode, afmt, mfmt);
	}
      }
    }
  }
}

void StepNIF(NiObjectRef root, NiNodeRef parent, NiObjectRef pref = NULL) {
  string type = root->GetType().GetTypeName();
  bhkCollisionObjectRef co = DynamicCast<bhkCollisionObject>(root);
  NiControllerSequenceRef cs = DynamicCast<NiControllerSequence>(root);
  NiAVObjectRef av = DynamicCast<NiAVObject>(root);
  NiObjectRef ob = DynamicCast<NiObject>(root);
  NiNodeRef nd = DynamicCast<NiNode>(root);

  /* register the node-type */
  markblock(dbfile, type.data());

  /* informations */
  if (co && co->GetTarget())
    nodebarref = co->GetTarget()->GetName();
  else if (cs)
    nodebarref = cs->GetName();
  else if (av)
    nodebarref = av->GetName();

  /* switches */
  if (co && DynamicCast<bhkRigidBody>(co->GetBody())) {
    nodemotion = DynamicCast<bhkRigidBody>(co->GetBody())->GetMotionSystem();
    nodelayer = DynamicCast<bhkRigidBody>(co->GetBody())->GetLayer();
  }
  else if (cs) {
    nodecycle = cs->GetCycleType();
  }

  /* sanitize before */
  parallaxmapping = false;
  texturepaths = true;

  SanitizeNode(av, nd, ob);
  if (av) {
    /* clean texture-paths */
    ModifyNode(av);
    StepProperties(av);
  }

  if (0) {
  }
  else {
    list<NiObjectRef> links = root->GetRefs();
    for (list<NiObjectRef>::iterator it = links.begin(); it != links.end(); ++it) {
      StepNIF(*it, nd, root);
    }
  }
}

/* ---------------------------------------------------------------------------------------------- */

bool AnalyzeNIF(const char *inname, const char *rep) {
  vector<NiObjectRef>::iterator walk;
  char buf[1024];

  istream *ist;
  if (!(ist = ioopenistream(inname))) {
    errprintf(stderr, "can't open the NIF\n");
    return false;
  }

  /* clear first */
  memset(&masterinfo, 0, sizeof(masterinfo));

  /* read the NIF */
  masterlist = ReadNifList(*ist, &masterinfo);
  if (masterlist.size()) {
    /* find the root */
    masterroot = FindRoot(masterlist);
    if (masterroot) {
      processedfiles++;

      /* try to load the accompanying TRI/EGM */
      ReadTri(putext(buf, inname, "tri"));
      ReadEgm(putext(buf, inname, "egm"));

      /* go and optimize */
      StepNIF(masterroot, NULL, NULL);
    }
    else
      errprintf(stderr, "can't find the NIF-root\n");
  }
  else
    errprintf(stderr, "can't read the NIF\n");

  /* dereference */
  masterroot = NULL;
  masterlist.clear();

  iocloseistream(ist);

  return true;
}

bool AnalyzeKF(const char *inname, const char *rep) {
  vector<NiObjectRef>::iterator walk;

  istream *ist;
  if (!(ist = ioopenistream(inname))) {
    errprintf(stderr, "can't open the KF\n");
    return false;
  }

  /* clear first */
  memset(&masterinfo, 0, sizeof(masterinfo));

  /* read the KF */
  masterlist = ReadNifList(*ist, &masterinfo);
  if (masterlist.size()) {
    /* find the root */
    masterroot = FindRoot(masterlist);

    if (masterroot) {
      processedfiles++;

      /* go and optimize */
      StepNIF(masterroot, NULL, NULL);
    }
    else
      errprintf(stderr, "can't find the KF-root\n");
  }
  else
    errprintf(stderr, "can't read the KF\n");

  /* dereference */
  masterroot = NULL;
  masterlist.clear();

  iocloseistream(ist);

  return true;
}

/* ---------------------------------------------------------------------------------------------- */

void analyze(const char *inname) {
  size_t ilen = (inname ? strlen(inname) : 0);

  bool rm = false;
  struct ioinfo iinfo;
  if (!iostat(inname, &iinfo)) {
    /* input: directory */
    if (iinfo.io_type & IO_DIRECTORY) {
      if (1) {
	struct iodir *dir;

	if ((dir = ioopendir(inname))) {
	  struct iodirent *et;

	  if (isarchive(inname)) {
	    int dbstamp = (int)iinfo.io_time;

	    /* get/put from/to the database */
	    if ((dbarchive = getarchive(package, inname, &dbstamp))) {
	    /* already in the database */
	    if (dbstamp != iinfo.io_time) {
	      /* when entering an old archive, flush the whole thing */
	      if (dbstamp != 0)
		flusharchive(package, inname);
	    }
	    }
	  }

	  while ((et = ioreaddir(dir))) {
	    if (!strcmp(et->name, ".") ||
		!strcmp(et->name, ".."))
	      continue;

	    char *ninname = (char *)malloc(ilen + 1 + et->namelength + 1);

	    strcpy(ninname, inname);
	    strcat(ninname, "\\");
	    strcat(ninname, et->name);

	    analyze(ninname);

	    free(ninname);
	  }

	  ioclosedir(dir);
	}
	else
	  fprintf(stderr, "can't open the directory \"%s\"\n", inname);
      }
    }
    /* input: file */
    else {
      const char *fle;

      /* strip trailing slash */
      if (!(fle = strrchr(inname, '/')))
	if (!(fle = strrchr(inname, '\\')))
	  fle = inname - 1;

      fle += 1;

      /* register the file */
      if (iostat(inname, &iinfo))
	return;
      if (!(ft = getfiletype(inname)))
	return;

      /* if it interrupts, stay sane (make getfile() part of the transaction) */
      sqlexec("BEGIN TRANSACTION");
      bool skipped = true;

      int dbstamp = (int)iinfo.io_time;

      /* get/put from/to the database */
      if ((dbfile = getfile(package, inname, &dbstamp, ft))) {
      /* already in the database */
      if (dbstamp != iinfo.io_time) {
	/* when entering an old file, flush the whole thing */
	if (dbstamp != 0)
	  flushfile(package, inname);

#ifdef	NDEBUG
//      __try {
      try {
#endif
	/* try to save the accompanying EGM */
	if (isext(inname, "egm")) {
	  char companynif[1024];
	  putext(companynif, inname, "nif");

	  if (iostat(companynif, &iinfo)) {
	    orphegms.push_back(inname);
	    orphanegmfiles++;
	  }
	}

	/* try to save the accompanying TRI */
	if (isext(inname, "tri")) {
	  char companynif[1024];
	  putext(companynif, inname, "nif");

	  if (iostat(companynif, &iinfo)) {
	    orphtris.push_back(inname);
	    orphantrifiles++;
	  }
	}

	if (isext(inname, "nif")) {
	  if (1) {
	    fprintf(stderr, "analyzing \"%s\"\n", fle);
	    AnalyzeNIF(inname, fle);
	    /* okay, done with */
	  }
	}

	if (isext(inname, "kf")) {
	  if (1) {
	    fprintf(stderr, "analyzing \"%s\"\n", fle);
	    AnalyzeKF(inname, fle);
	    /* okay, done with */
	  }
	}

#ifdef	TEXTUREOPT
	if (isext(inname, "dds") ||
	  ((isext(inname, "png") ||
	    isext(inname, "bmp") ||
	    isext(inname, "dib") ||
	    isext(inname, "tga") ||
	    isext(inname, "ppm") ||
	    isext(inname, "pfm") ||
	    isext(inname, "hdr")) && (compressimages || skipimages))) {
	  if (skipimages)
	    ;
	  else if (stristr(inname, "textures\\")) {
	    fprintf(stderr, "analyzing \"%s\"\n", fle);
	    AnalyzeDDS(inname, fle);
	    /* okay, done with */
	  }
	}
#endif

#ifdef	SOUNDOPT
	if (isext(inname, "wav")) {
	  if (skipsounds)
	    ;
	  else if (1) {
	    fprintf(stderr, "analyzing \"%s\"\n", fle);
	    AnalyzeWave(inname, fle);
	    /* okay, done with */
	  }
	}

	if (isext(inname, "wav") ||
	  ((isext(inname, "mp3") ||
	    isext(inname, "lip")) && skipsounds)) {
	  if (skipsounds)
	    ;
	}
#endif
#ifdef	NDEBUG
      }
      catch(exception &e) {
	errprintf(stderr, "error analyzing \"%s\": %s\n", fle, e.what());
	errprintf(stdout, "error analyzing \"%s\": %s\n", fle, e.what());
	brokenfiles++;
      }
//      }
//      __except (EXCEPTION_EXECUTE_HANDLER) {
//	errprintf(stderr, "Fatal abortion while reading the NIF\n");
//	errprintf(stdout, "Fatal abortion while reading the NIF\n");
//      }
#endif

	skipped = false;
      }}

      sqlexec("END TRANSACTION");
      if (verbose && skipped)
	fprintf(stderr, "skipped \"%s\"\r", fle);
    }
  }
}
