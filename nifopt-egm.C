
struct EgmMorphHead {
  // Always = FR
  char Identifier[2];
  // EGM
  char FileType[3];
  // 002 for Oblivion
  char Version[3];

  // Number of vertices in the base model + Modifier Vertices.
  int NumVertices;
  // Number of symmetrical morphs. Oblivion = 50.
  int NumSymmetricalMorphs;
  // Number of asymmetrical morphs. Oblivion = 30.
  int NumAsymmetricalMorphs;
  // Time stamp? Always 2001060901 for Oblivon.
  unsigned int TimeDateStamp;
  // Always filled with 0's.
  int Unknown[10];
};

struct EgmMorphVertex {
  short x, y, z;
};

struct EgmMorphRecord {
  // All the following relative vertices are multiplied by this scale.
  float Scale;

  // Amount to alter the base model corresponding vertex by.
  std::vector<EgmMorphVertex> VertexList;
  std::vector<EgmMorphVertex> ModifierList;
};

bool EgmMorphConv = false;
bool EgmMorphProc = false;
struct EgmMorphHead EgmMorphHeader;
std::vector<EgmMorphRecord> EgmMorphRecords;

void ReadEgm(const char *inname) {
#if 0
  // ******** Start of Structure definitions ********

  struct sRelativeVertex {
     short   x;
     short   y;
     short   z;
  };

  struct EgmMorphRecord {
     float         Scale;            // All the following relative vertices are multiplied by this scale.
     sRelativeVertex      VertexList[NumVertices];   // Amount to alter the base model corresponding vertex by.
  };

  /*    Uses the same formula as a morph in a tri file.
     Includes morphs for modifier morphs as defined in the tri file. Each set of morphs contain morphs for the base model
     followed by replacement morphs when a modifier morph is used.
  */

  // ******** End of structure definitions ********

  // ********   Start of file definition ********
  // ******** Start of Header chunk ********

  char      Identifier[2];               // Always = FR
  char       FileType[3];               // EGM
  char      Version[3];               // 002 for Oblivion

  int      NumVertices;               // Number of vertices in the base model + Modifier Vertices.
  int      NumSymmetricalMorphs;            // Number of symmetrical morphs. Oblivion = 50.
  int      NumAsymmetricalMorphs;            // Number of asymmetrical morphs. Oblivion = 30.
  unsigned int   TimeDateStamp;               // Time stamp? Always 2001060901 for Oblivon.
  int      Unknown[10];               // Always filled with 0's.

  // ******** End of Header chunk ********

  // ******** Start of data chunk ********

  EgmMorphRecord   SymMorphList[NumSymetricalMorphs];      // List of symmetrical morphs. Used by symmetrical face sliders.
  EgmMorphRecord   AsymMorphList[NumAsymetricalMorphs];      // List of asymmetrical morphs. Used by asymmetrical face sliders.

  // Please note that the MorphLists above do not have a 1:1 relationship with the face shape sliders and a single
  // slider may effect several morphs at the same time.

  // ******** End of data chunk ********
  // ******** End of file definition ********
#endif

  struct iofile *egm;

  memset(&EgmMorphHeader, 0, sizeof(EgmMorphHeader));

  /* clear all lists that we're going to load */
  EgmMorphRecords.clear();

  /* attempt to load */
  EgmMorphConv = false;
  EgmMorphProc = false;
  if ((egm = ioopenfile(inname, "rb"))) {
    if (ioreadfile(&EgmMorphHeader, sizeof(EgmMorphHeader), egm) == sizeof(EgmMorphHeader)) {
      /* we got a valid TRI-file */
      if ((EgmMorphHeader.Identifier[0] != 'F') ||
	  (EgmMorphHeader.Identifier[1] != 'R'))
	return;
      if ((EgmMorphHeader.FileType[0] != 'E') ||
	  (EgmMorphHeader.FileType[1] != 'G') ||
	  (EgmMorphHeader.FileType[2] != 'M'))
	return;

      /* serialized load all contained information */
      int moffs = 0;

      /* initially load all symmetrical morphs in as base-morphs
       * symmetrical modifier-morphs are separated later
       */
      for (int m = 0; m < EgmMorphHeader.NumSymmetricalMorphs; m++) {
	struct EgmMorphRecord mr;

	int length = EgmMorphHeader.NumVertices * sizeof(EgmMorphVertex);

	mr.VertexList.resize(EgmMorphHeader.NumVertices);

	if (length && (ioreadfile(&mr.Scale, sizeof(mr.Scale), egm) != sizeof(mr.Scale)))
	  return;
	if (length && (ioreadfile(&mr.VertexList[0], length, egm) != length))
	  return;

	/* advance position */
	EgmMorphRecords.push_back(mr); moffs++;
      }

      /* initially load all asymmetrical morphs in as base-morphs
       * asymmetrical modifier-morphs are separated later
       */
      for (int m = 0; m < EgmMorphHeader.NumAsymmetricalMorphs; m++) {
	struct EgmMorphRecord mr;

	int length = EgmMorphHeader.NumVertices * sizeof(EgmMorphVertex);

	mr.VertexList.resize(EgmMorphHeader.NumVertices);

	if (length && (ioreadfile(&mr.Scale, sizeof(mr.Scale), egm) != sizeof(mr.Scale)))
	  return;
	if (length && (ioreadfile(&mr.VertexList[0], length, egm) != length))
	  return;

	/* advance position */
	EgmMorphRecords.push_back(mr); moffs++;
      }
    }

    /* uh, MicroSoft, no signed "ssize_t"? */
    long is;
    if ((is = iotell(egm)))
      processedoubytes += is, deltaiobytes += is;

    /* TODO: nowait asynchronous */
    ioclosefile(egm);
  }
}

void WriteEgm(const char *ouname) {
  struct iofile *egm;

  if ((EgmMorphHeader.Identifier[0] != 'F') ||
      (EgmMorphHeader.Identifier[1] != 'R'))
    return;
  if ((EgmMorphHeader.FileType[0] != 'E') ||
      (EgmMorphHeader.FileType[1] != 'G') ||
      (EgmMorphHeader.FileType[2] != 'M'))
    return;

  /* prevent a false positive */
  if (!EgmMorphProc && optimizelists) {
    errprintf(stdout, "\nwarning:\n");
    errprintf(stdout, " Writing EGM without modification, no matching mesh with %d vertices found!\n", EgmMorphHeader.NumVertices);
  }

  /* don't create output in simulation-mode */
  if (!simulation && (egm = ioopenfile(ouname, "wb"))) {
    if (iowritefile(&EgmMorphHeader, sizeof(EgmMorphHeader), egm) == sizeof(EgmMorphHeader)) {
      int moffs = 0;

      for (int m = 0; m < EgmMorphHeader.NumSymmetricalMorphs; m++) {
	struct EgmMorphRecord mr = EgmMorphRecords[moffs]; moffs++;

	size_t length = EgmMorphHeader.NumVertices * sizeof(EgmMorphVertex);
	size_t lengthv = mr.VertexList.size() * sizeof(EgmMorphVertex);
	size_t lengthm = mr.ModifierList.size() * sizeof(EgmMorphVertex);

	assert(length == (lengthv + lengthm));

	if (length  && (iowritefile(&mr.Scale, sizeof(mr.Scale), egm) != sizeof(mr.Scale)))
	  return;
	if (lengthv && (iowritefile(&mr.VertexList[0], lengthv, egm) != lengthv))
	  return;
	if (lengthm && (iowritefile(&mr.ModifierList[0], lengthm, egm) != lengthm))
	  return;
      }

      for (int m = 0; m < EgmMorphHeader.NumAsymmetricalMorphs; m++) {
	struct EgmMorphRecord mr = EgmMorphRecords[moffs]; moffs++;

	size_t length = EgmMorphHeader.NumVertices * sizeof(EgmMorphVertex);
	size_t lengthv = mr.VertexList.size() * sizeof(EgmMorphVertex);
	size_t lengthm = mr.ModifierList.size() * sizeof(EgmMorphVertex);

	assert(length == (lengthv + lengthm));

	if (length  && (iowritefile(&mr.Scale, sizeof(mr.Scale), egm) != sizeof(mr.Scale)))
	  return;
	if (lengthv && (iowritefile(&mr.VertexList[0], lengthv, egm) != lengthv))
	  return;
	if (lengthm && (iowritefile(&mr.ModifierList[0], lengthm, egm) != lengthm))
	  return;
      }
    }

    /* uh, MicroSoft, no signed "ssize_t"? */
    long os;
    if ((os = iotell(egm)))
      processedoubytes += os, deltaiobytes -= os;

    ioclosefile(egm);
  }

  memset(&EgmMorphHeader, 0, sizeof(EgmMorphHeader));

  EgmMorphRecords.clear();

  EgmMorphConv = false;
  EgmMorphProc = false;
}

/* ---------------------------------------------------- */

void convert_egm(int nv, int ns, vector<Niflib::Vector3> &_v, vector< vector<unsigned short> > &_s) {
  if ((EgmMorphHeader.Identifier[0] != 'F') ||
      (EgmMorphHeader.Identifier[1] != 'R'))
    return;
  if ((EgmMorphHeader.FileType[0] != 'E') ||
      (EgmMorphHeader.FileType[1] != 'G') ||
      (EgmMorphHeader.FileType[2] != 'M'))
    return;

  {
    EgmMorphConv = true;
  }
}

/* ---------------------------------------------------- */

void optimize_egm(vector<Niflib::Vector3> & v, 
		  vector<Niflib::Vector3> &_v) {
  if ((EgmMorphHeader.Identifier[0] != 'F') ||
      (EgmMorphHeader.Identifier[1] != 'R'))
    return;
  if ((EgmMorphHeader.FileType[0] != 'E') ||
      (EgmMorphHeader.FileType[1] != 'G') ||
      (EgmMorphHeader.FileType[2] != 'M'))
    return;

  int nv = (int)v.size();
  int mv = 0, _m = 0;
  if (TriMorphProc) {
    /* calculate number of original modifier-vertices (before reduction) */
    for (int m = 0; m < TriMorphHeader.NumModifiers; m++)
      _m += TriMorphModifiers[m].datasize,
      mv += TriMorphModifiers[m].dataskps;

    /* split-array sizes */
    if (mv && ((nv + mv) == EgmMorphHeader.NumVertices)) {
      int moffs = 0;

      /* divide Symmetrical Morphs */
      for (int m = 0; m < EgmMorphHeader.NumSymmetricalMorphs; m++) {
        assert(EgmMorphRecords[moffs].VertexList.size() == (nv + mv));

        std::vector<EgmMorphVertex>::iterator f = EgmMorphRecords[moffs].VertexList.begin() + nv;
        std::vector<EgmMorphVertex>::iterator l = EgmMorphRecords[moffs].VertexList.end();

        EgmMorphRecords[moffs].ModifierList.assign(f, l);
        EgmMorphRecords[moffs].VertexList.erase(f, l);

	moffs++;
      }

      /* divide Asymmetrical Morphs */
      for (int m = 0; m < EgmMorphHeader.NumAsymmetricalMorphs; m++) {
        assert(EgmMorphRecords[moffs].VertexList.size() == (nv + mv));

        std::vector<EgmMorphVertex>::iterator f = EgmMorphRecords[moffs].VertexList.begin() + nv;
        std::vector<EgmMorphVertex>::iterator l = EgmMorphRecords[moffs].VertexList.end();

        EgmMorphRecords[moffs].ModifierList.assign(f, l);
	EgmMorphRecords[moffs].VertexList.erase(f, l);

	moffs++;
      }
    }
  }

  if ((nv + mv) == EgmMorphHeader.NumVertices) {
    int moffs = 0;

    /* spit out some info */
//  barprintf(stdout, '.', "accompanying egm");
    nfoprintf(stdout, "processing accompanying egm:\n");

    /* process symmetrical morph-vertices */
    for (int m = 0; m < EgmMorphHeader.NumSymmetricalMorphs; m++) {
      struct EgmMorphRecord mr = EgmMorphRecords[moffs];
      struct EgmMorphRecord mn;

      /* the VertexList mirrors the order of vertices in the base-mesh (NIF) */
      assert(mr.VertexList.size() == nv);

      /* old to new to opt */
      int mc = (int)mr.VertexList.size();
      for (int ms = 0; ms < mc; ms++) {
	/* skip eliminated indices */
	int oidx = ms;
	int nidx = SectorRemap[oidx];
	if (nidx < 0) continue;
	int pidx = SectorRemapO[nidx];

	int stz = (int)mn.VertexList.size();
	if (stz <= pidx)
	  mn.VertexList.resize(pidx + 1);

	mn.VertexList[pidx] = mr.VertexList[oidx];
      }

      assert(mn.VertexList.size() == _v.size());

      /* the ModifierList mirrors the list of indices in the modifiers (TRI) */
      assert(mr.ModifierList.size() == mv);

      /* old to new to opt */
      int ml = (int)mr.ModifierList.size();
      int mm = 0, mo = 0;
      for (int ms = 0; ms < ml; ms++) {
	/* percolated walk over all modifier list of indices */
	bool skip = TriMorphModifiers[mm].skpdata[mo];
	if (++mo == TriMorphModifiers[mm].dataskps)
	  mm++, mo = 0;

	/* skip eliminated indices */
	if (skip) continue;

	mn.ModifierList.push_back(mr.ModifierList[ms]);
      }

      assert(mn.ModifierList.size() == _m);

      /* update morph-record */
      EgmMorphRecords[moffs] = mn; moffs++;
    }

    /* process asymmetrical morph-vertices */
    for (int m = 0; m < EgmMorphHeader.NumAsymmetricalMorphs; m++) {
      struct EgmMorphRecord mr = EgmMorphRecords[moffs];
      struct EgmMorphRecord mn;

      /* the VertexList mirrors the order of vertices in the base-mesh (NIF) */
      assert(mr.VertexList.size() == nv);

      /* old to new to opt */
      int mc = (int)mr.VertexList.size();
      for (int ms = 0; ms < mc; ms++) {
	/* skip eliminated indices */
	int oidx = ms;
	int nidx = SectorRemap[oidx];
	if (nidx < 0) continue;
	int pidx = SectorRemapO[nidx];

	int stz = (int)mn.VertexList.size();
	if (stz <= pidx)
	  mn.VertexList.resize(pidx + 1);

	mn.VertexList[pidx] = mr.VertexList[oidx];
      }

      assert(mn.VertexList.size() == _v.size());

      /* the ModifierList mirrors the list of indices in the modifiers (TRI) */
      assert(mr.ModifierList.size() == mv);

      /* old to new to opt */
      int ml = (int)mr.ModifierList.size();
      int mm = 0, mo = 0;
      for (int ms = 0; ms < ml; ms++) {
	/* percolated walk over all modifier list of indices */
	bool skip = TriMorphModifiers[mm].skpdata[mo];
	if (++mo == TriMorphModifiers[mm].dataskps)
	  mm++, mo = 0;

	/* skip eliminated indices */
	if (skip) continue;

	mn.ModifierList.push_back(mr.ModifierList[ms]);
      }

      assert(mn.ModifierList.size() == _m);

      /* update morph-record */
      EgmMorphRecords[moffs] = mn; moffs++;
    }

    /* set new amount of vertices */
    int _nv = (int)_v.size();
    int _mv = _m;

    EgmMorphHeader.NumVertices = _nv + _mv;

    nfoprintf(stdout, " Symmetricals     : %d\n", EgmMorphHeader.NumSymmetricalMorphs);
    nfoprintf(stdout, " Asymmetricals    : %d\n", EgmMorphHeader.NumAsymmetricalMorphs);
    nfoprintf(stdout, " Vertices         : %6d to %6d (%.4f%%)\n", nv, _nv, 100.0f * _nv / nv);
    nfoprintf(stdout, " Modifiers        : %6d to %6d (%.4f%%)\n", mv, _mv, 100.0f * _mv / mv);

    EgmMorphProc = true;
  }
}
