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
int shell_callback(void *pArg, int nArg, char **azArg, char **azCol){
  struct callback_data *p = (struct callback_data*)pArg;

  return 0;
}

void sqlexec(string statement) {
  char *zErrMsg;
  int rc = sqlite3_exec(database.db, statement.data(), shell_callback, &database, &zErrMsg);
}

/* ---------------------------------------------------------------------------------------------- */

#ifdef	TEXTUREOPT
bool analyze_dds(const char *inname, const char *rep) {
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
	  addnote(" Missing %d mip-level(s) complemented.\n", levels - levelc);
	else if (levels < levelc)
	  addnote(" Excess of %d mip-level(s) removed.\n", levelc - levels);

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
	    addnote(" Truncated texture found, fixed!\n");
	}

	/* choose what to do */
	int relevel = 0;
	if (levels > levelc)
	  relevel = levelc + 1;
	else if (levels < levelc)
	  relevel = 0 - levels;
	else if (levels == 1)
	  relevel = 0 - levels;

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
bool analyze_wav(const char *inname, const char *rep) {
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

void step_nif(NiObjectRef root, NiNodeRef parent, NiObjectRef pref = NULL) {
  string type = root->GetType().GetTypeName();
  bhkCollisionObjectRef co = DynamicCast<bhkCollisionObject>(root);
  NiControllerSequenceRef cs = DynamicCast<NiControllerSequence>(root);
  NiAVObjectRef av = DynamicCast<NiAVObject>(root);
  NiObjectRef ob = DynamicCast<NiObject>(root);
  NiNodeRef nd = DynamicCast<NiNode>(root);

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
  sanitize_node(av, nd, ob);

  if (0) {
  }
  else {
    list<NiObjectRef> links = root->GetRefs();
    for (list<NiObjectRef>::iterator it = links.begin(); it != links.end(); ++it) {
      step_nif(*it, nd, root);
    }
  }
}

/* ---------------------------------------------------------------------------------------------- */

bool analyze_nif(const char *inname, const char *rep) {
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
      step_nif(masterroot, NULL, NULL);
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

bool analyze_kf(const char *inname, const char *rep) {
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
      step_nif(masterroot, NULL, NULL);
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
	    analyze_nif(inname, fle);
	    /* okay, done with */
	  }
	}

	if (isext(inname, "kf")) {
	  if (1) {
	    fprintf(stderr, "analyzing \"%s\"\n", fle);
	    analyze_kf(inname, fle);
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
	    analyze_dds(inname, fle);
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
	    analyze_wav(inname, fle);
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
    }
  }
}
