
struct TriMorphHead {
  // Always FR
  char Identifier[2];
  // Always TRI
  char FileType[3];
  // 002 for Oblivion
  char Version[3];

  // number of NumVertices
  int NumVertices;
  // number of tri faces
  int NumPolyTris;
  // number of quad faces
  int NumPolyQuads;

  int unknown2;
  int unknown3;

  // number of uv map NumVertices
  int NumUVVertices;
  // Flags?
  int Flags;
  // bit 0 must be 1 or crashes
  // bit 1 true=don't display face
  // bit 2 no effect?
  // number of morphs
  int NumMorphRecords;
  // number of modifiers
  int NumModifiers;
  // number of vertices to replace by modifiers
  int NumModifierVertices;

  int unknown7;
  int unknown8;
  int unknown9;
  int unknown10;
};

struct TriMorph_fCOORDS {
  float x,y,z;
};

struct TriMorph_triCOORDS {
  int a, b, c;
};

struct TriMorph_quadCOORDS {
  int a, b, c, d;
};

struct TriMorph_uvCOORDS {
  float u, v;
};

struct TriMorphRecord {
  // = obj v indexed from 0
  std::vector<TriMorph_fCOORDS> VertexList;//[header.NumVertices];
  // modifier NumVertices
  std::vector<TriMorph_fCOORDS> ModifierVertexList;//[header.NumModifierVertices];
  // = obj f a,b,c (indexed from 0)
  std::vector<TriMorph_triCOORDS> FaceList;//[header.NumPolyTris];
  // = obj f a,b,c,d (indexed from 0)
  std::vector<TriMorph_quadCOORDS> QuadList;//[header.NumPolyQuads];

  // = obj vt
  std::vector<TriMorph_uvCOORDS> UVVertexList;//[header.NumUVVertices];
  // = obj ?/a,?/b,?/c
  std::vector<TriMorph_triCOORDS> UVFaceList;//[header.NumPolyTris];
  // = obj ?/a,?/b,?/c,?/d
  std::vector<TriMorph_quadCOORDS> UVQuadList;//[header.NumPolyQuads];
};

struct TriMorphVertex {
  short x, y, z;
};

struct TriMorphData {
  int namesize;
  char name[256];//[namesize];

  char unknown1; // next 4 bytes to do with scaling
  char unknown2; // haven't worked out yet. Uses
  char unknown3; // individual scaling for x,y,z axis.
  char unknown4;

  std::vector<TriMorphVertex> VertexList;//[header.NumVertices];
};

struct TriMorphModifier {
  int namesize;
  char name[256];//[namesize];

  int datasize;
  int *miscdata;//[datasize];

  /* for later EGM processing */
  int dataskps;
  bool *skpdata;//[datasize];
};

bool TriMorphConv = false;
bool TriMorphProc = false;
struct TriMorphHead TriMorphHeader;
TriMorphRecord TriMorphRecords;
std::vector<TriMorphData> TriMorphDatas;//[header.NumMorphRecords];
std::vector<TriMorphModifier> TriMorphModifiers;//[header.NumModifiers];

void ReadTri(const char *inname) {
#if 0
  //--- 010 Editor v2.0.2 Binary Template
  //
  // File: Tri file template
  // Author: Scanti
  // Revision: v1.0
  // Purpose: Tamplate for facegen geometry and morphtarget container
  //--------------------------------------
  struct FILE {
    struct HEADER{
      char Identifier[2]; // Always FR
      char FileType[3]; // Always TRI
      char Version[3];
      int NumVertices; // number of NumVertices
      int NumPolyTris; // number of tri faces
      int NumPolyQuads; // number of quad faces
      int unknown2;
      int unknown3;
      int NumUVVertices; // number of uv map NumVertices
      int Flags; // Flags?
      // bit 0 must be 1 or crashes
      // bit 1 true=don't display face
      // bit 2 no effect?
      int NumMorphRecords; // number of morphs
      int NumModifiers; // number of modifiers
      int NumModifierVertices; // number of NumVertices to replace by modifiers
      int unknown7;
      int unknown8;
      int unknown9;
      int unknown10;
    }header;

    struct fCOORDS {
      float x,y,z;
    };

    struct triCOORDS {
      int a,b,c;
    };

    struct quadCOORDS {
      int a,b,c,d;
    };

    struct uvCOORDS {
      float u,v;
    };

    fCOORDS NumVertices[header.NumVertices]; // = obj v indexed from 0
    fCOORDS NumModifierVertices[header.NumModifierVertices]; // modifier NumVertices
    triCOORDS trifaces[header.NumPolyTris]; // = obj f a,b,c (indexed from 0)
    quadCOORDS quadfaces[header.NumPolyQuads]; // = obj f a,b,c,d (indexed from 0)

    uvCOORDS NumUVVertices[header.NumUVVertices]; // = obj vt
    triCOORDS uvtrifaces[header.NumPolyTris]; // = obj ?/a,?/b,?/c
    quadCOORDS uvquadfaces[header.NumPolyQuads]; // = obj ?/a,?/b,?/c,?/d

    struct morphNumVertices {
      short x,y,z;
    };

    struct morphdata {
      int namesize;
      char name[namesize];
      byte unknown1; // next 4 bytes to do with scaling
      byte unknown2; // haven't worked out yet. Uses
      byte unknown3; // individual scaling for x,y,z axis.
      byte unknown4;
      morphNumVertices VertexList[header.NumVertices];
    };

    morphdata morphs[header.NumMorphRecords] <optimize=false>;

    struct modifierdata{
      int namesize;
      char name[namesize];
      int datasize;
      int miscdata[datasize];
    };

    modifierdata modifiers[header.NumModifiers] <optimize=false>;

  } file;
#endif

  struct iofile *tri;

  memset(&TriMorphHeader, 0, sizeof(TriMorphHeader));

  /* clear all lists that we're going to load */
  TriMorphDatas.clear();
  TriMorphModifiers.clear();
  TriMorphRecords.VertexList.clear();
  TriMorphRecords.ModifierVertexList.clear();
  TriMorphRecords.FaceList.clear();
  TriMorphRecords.QuadList.clear();
  TriMorphRecords.UVVertexList.clear();
  TriMorphRecords.UVFaceList.clear();
  TriMorphRecords.UVQuadList.clear();

  /* attempt to load */
  TriMorphConv = false;
  TriMorphProc = false;
  if ((tri = ioopenfile(inname, "rb"))) {
    if (ioreadfile(&TriMorphHeader, sizeof(TriMorphHeader), tri) == sizeof(TriMorphHeader)) {
      /* we got a valid TRI-file */
      if ((TriMorphHeader.Identifier[0] != 'F') ||
	  (TriMorphHeader.Identifier[1] != 'R'))
	return;
      if ((TriMorphHeader.FileType[0] != 'T') ||
	  (TriMorphHeader.FileType[1] != 'R') ||
	  (TriMorphHeader.FileType[2] != 'I'))
	return;

      /* serialized load all contained information */
      if (TriMorphHeader.NumVertices) {
	int length = TriMorphHeader.NumVertices * sizeof(TriMorph_fCOORDS);
	TriMorphRecords.VertexList.resize(TriMorphHeader.NumVertices);

	if (ioreadfile(&TriMorphRecords.VertexList[0], length, tri) != length)
	  return;
      }

      if (TriMorphHeader.NumModifierVertices) {
	int length = TriMorphHeader.NumModifierVertices * sizeof(TriMorph_fCOORDS);
	TriMorphRecords.ModifierVertexList.resize(TriMorphHeader.NumModifierVertices);

	if (ioreadfile(&TriMorphRecords.ModifierVertexList[0], length, tri) != length)
	  return;
      }

      if (TriMorphHeader.NumPolyTris) {
	int length = TriMorphHeader.NumPolyTris * sizeof(TriMorph_triCOORDS);
	TriMorphRecords.FaceList.resize(TriMorphHeader.NumPolyTris);

	if (ioreadfile(&TriMorphRecords.FaceList[0], length, tri) != length)
	  return;
      }

      if (TriMorphHeader.NumPolyQuads) {
	int length = TriMorphHeader.NumPolyQuads * sizeof(TriMorph_quadCOORDS);
	TriMorphRecords.QuadList.resize(TriMorphHeader.NumPolyQuads);

	if (ioreadfile(&TriMorphRecords.QuadList[0], length, tri) != length)
	  return;
      }

      if (TriMorphHeader.NumUVVertices) {
	int length = TriMorphHeader.NumUVVertices * sizeof(TriMorph_uvCOORDS);
	TriMorphRecords.UVVertexList.resize(TriMorphHeader.NumUVVertices);

	if (ioreadfile(&TriMorphRecords.UVVertexList[0], length, tri) != length)
	  return;
      }

      if (TriMorphHeader.NumUVVertices && TriMorphHeader.NumPolyTris) {
	int length = TriMorphHeader.NumPolyTris * sizeof(TriMorph_triCOORDS);
	TriMorphRecords.UVFaceList.resize(TriMorphHeader.NumPolyTris);

	if (ioreadfile(&TriMorphRecords.UVFaceList[0], length, tri) != length)
	  return;
      }

      if (TriMorphHeader.NumUVVertices && TriMorphHeader.NumPolyQuads) {
	int length = TriMorphHeader.NumPolyQuads * sizeof(TriMorph_quadCOORDS);
	TriMorphRecords.UVQuadList.resize(TriMorphHeader.NumPolyQuads);

	if (ioreadfile(&TriMorphRecords.UVQuadList[0], length, tri) != length)
	  return;
      }

      for (int m = 0; m < TriMorphHeader.NumMorphRecords; m++) {
	struct TriMorphData mr;

	int length = TriMorphHeader.NumVertices * sizeof(TriMorphVertex);
	mr.VertexList.resize(TriMorphHeader.NumVertices);

	if (ioreadfile(&mr.namesize, sizeof(mr.namesize), tri) != sizeof(mr.namesize))
	  return;
	if (ioreadfile(&mr.name, mr.namesize, tri) != mr.namesize)
	  return;
	if (ioreadfile(&mr.unknown1, (sizeof(mr.unknown1) * 4), tri) != (sizeof(mr.unknown1) * 4))
	  return;
	if (ioreadfile(&mr.VertexList[0], length, tri) != length)
	  return;

	assert(mr.namesize < 256);
	mr.name[mr.namesize] = '\0';

	/* register new morpher */
	TriMorphDatas.push_back(mr);
      }

      /* the accumulation of all modifier-groups
       * is the number of modifier vertices, it's
       * just a concatenation of all modifier-vertices
       *
       * the meaning of the modifier-groups is written
       * down in the name, normally those are Phonems
       * (face-expressions for specific spoken louds)
       *
       * I assume they are also used to mix'n'match the
       * face-sliders in the facegen
       */
      int mdatasize = 0;
      for (int m = 0; m < TriMorphHeader.NumModifiers; m++) {
	struct TriMorphModifier mr;

	if (ioreadfile(&mr.namesize, sizeof(mr.namesize), tri) != sizeof(mr.namesize))
	  return;
	if (ioreadfile(&mr.name, mr.namesize, tri) != mr.namesize)
	  return;
	if (ioreadfile(&mr.datasize, sizeof(mr.datasize), tri) != sizeof(mr.datasize))
	  return;

	mdatasize += mr.datasize;
	mr.miscdata = new int[mr.datasize];
	if (ioreadfile(mr.miscdata, (mr.datasize * sizeof(int)), tri) != (mr.datasize * sizeof(int)))
	  return;

	/* create an array holding booleans to indicate for the
	 * accompanying EGM if the vertex has been removed or not
	 */
	mr.dataskps = mr.datasize;
	mr.skpdata = new bool[mr.dataskps];
	memset(mr.skpdata, false, mr.dataskps * sizeof(bool));

	assert(mr.namesize < 256);
	mr.name[mr.namesize] = '\0';

	/* register new modifier */
	TriMorphModifiers.push_back(mr);
      }

      assert(TriMorphHeader.NumModifierVertices == mdatasize);
    }

    /* uh, MicroSoft, no signed "ssize_t"? */
    long is;
    if ((is = iotell(tri)))
      processedinbytes += is, deltaiobytes += is;

    ioclosefile(tri);
  }
}

void WriteTri(const char *ouname) {
  struct iofile *tri;

  if ((TriMorphHeader.Identifier[0] != 'F') ||
      (TriMorphHeader.Identifier[1] != 'R'))
    return;
  if ((TriMorphHeader.FileType[0] != 'T') ||
      (TriMorphHeader.FileType[1] != 'R') ||
      (TriMorphHeader.FileType[2] != 'I'))
    return;

  /* prevent a false positive */
  if (!TriMorphProc && optimizelists) {
    errprintf(stdout, "\nwarning:\n");
    errprintf(stdout, " Writing TRI without modification, no matching mesh with %d vertices found!\n", TriMorphHeader.NumVertices);
  }

  /* don't create output in simulation-mode */
  if (!simulation && (tri = ioopenfile(ouname, "wb"))) {
    if (iowritefile(&TriMorphHeader, sizeof(TriMorphHeader), tri) == sizeof(TriMorphHeader)) {
      if (TriMorphHeader.NumVertices) {
	int length = TriMorphHeader.NumVertices * sizeof(TriMorph_fCOORDS);
	TriMorphRecords.VertexList.resize(TriMorphHeader.NumVertices);

	if (iowritefile(&TriMorphRecords.VertexList[0], length, tri) != length)
	  return;
      }

      if (TriMorphHeader.NumModifierVertices) {
	int length = TriMorphHeader.NumModifierVertices * sizeof(TriMorph_fCOORDS);
	TriMorphRecords.ModifierVertexList.resize(TriMorphHeader.NumModifierVertices);

	if (iowritefile(&TriMorphRecords.ModifierVertexList[0], length, tri) != length)
	  return;
      }

      if (TriMorphHeader.NumPolyTris) {
	int length = TriMorphHeader.NumPolyTris * sizeof(TriMorph_triCOORDS);
	TriMorphRecords.FaceList.resize(TriMorphHeader.NumPolyTris);

	if (iowritefile(&TriMorphRecords.FaceList[0], length, tri) != length)
	  return;
      }

      if (TriMorphHeader.NumPolyQuads) {
	int length = TriMorphHeader.NumPolyQuads * sizeof(TriMorph_quadCOORDS);
	TriMorphRecords.QuadList.resize(TriMorphHeader.NumPolyQuads);

	if (iowritefile(&TriMorphRecords.QuadList[0], length, tri) != length)
	  return;
      }

      if (TriMorphHeader.NumUVVertices) {
	int length = TriMorphHeader.NumUVVertices * sizeof(TriMorph_uvCOORDS);
	TriMorphRecords.UVVertexList.resize(TriMorphHeader.NumUVVertices);

	if (iowritefile(&TriMorphRecords.UVVertexList[0], length, tri) != length)
	  return;
      }

      if (TriMorphHeader.NumUVVertices && TriMorphHeader.NumPolyTris) {
	int length = TriMorphHeader.NumPolyTris * sizeof(TriMorph_triCOORDS);
	TriMorphRecords.UVFaceList.resize(TriMorphHeader.NumPolyTris);

	if (iowritefile(&TriMorphRecords.UVFaceList[0], length, tri) != length)
	  return;
      }

      if (TriMorphHeader.NumUVVertices && TriMorphHeader.NumPolyQuads) {
	int length = TriMorphHeader.NumPolyQuads * sizeof(TriMorph_quadCOORDS);
	TriMorphRecords.UVQuadList.resize(TriMorphHeader.NumPolyQuads);

	if (iowritefile(&TriMorphRecords.UVQuadList[0], length, tri) != length)
	  return;
      }

      for (int m = 0; m < TriMorphHeader.NumMorphRecords; m++) {
	struct TriMorphData &mr = TriMorphDatas[m];

	int length = TriMorphHeader.NumVertices * sizeof(TriMorphVertex);
	mr.VertexList.resize(TriMorphHeader.NumVertices);

	if (iowritefile(&mr.namesize, sizeof(mr.namesize), tri) != sizeof(mr.namesize))
	  return;
	if (iowritefile(&mr.name, mr.namesize, tri) != mr.namesize)
	  return;
	if (iowritefile(&mr.unknown1, (sizeof(mr.unknown1) * 4), tri) != (sizeof(mr.unknown1) * 4))
	  return;
	if (iowritefile(&mr.VertexList[0], length, tri) != length)
	  return;
      }

      for (int m = 0; m < TriMorphHeader.NumModifiers; m++) {
	struct TriMorphModifier &mr = TriMorphModifiers[m];

	if (iowritefile(&mr.namesize, sizeof(mr.namesize), tri) != sizeof(mr.namesize))
	  return;
	if (iowritefile(&mr.name, mr.namesize, tri) != mr.namesize)
	  return;
	if (iowritefile(&mr.datasize, sizeof(mr.datasize), tri) != sizeof(mr.datasize))
	  return;

	if (iowritefile(mr.miscdata, (mr.datasize * sizeof(int)), tri) != (mr.datasize * sizeof(int)))
	  return;
	delete[] mr.miscdata;
	delete[] mr.skpdata;
	mr.miscdata = NULL;
	mr.skpdata = NULL;
      }
    }

    /* uh, MicroSoft, no signed "ssize_t"? */
    long os;
    if ((os = iotell(tri)))
      processedoubytes += os, deltaiobytes -= os;

    /* TODO: nowait asynchronous */
    ioclosefile(tri);
  }

  memset(&TriMorphHeader, 0, sizeof(TriMorphHeader));

  TriMorphDatas.clear();
  TriMorphModifiers.clear();
  TriMorphRecords.VertexList.clear();
  TriMorphRecords.ModifierVertexList.clear();
  TriMorphRecords.FaceList.clear();
  TriMorphRecords.QuadList.clear();
  TriMorphRecords.UVVertexList.clear();
  TriMorphRecords.UVFaceList.clear();
  TriMorphRecords.UVQuadList.clear();

  TriMorphConv = false;
  TriMorphProc = false;
}

/* ---------------------------------------------------- */

void convert_tri(int nv, int ns, vector<Niflib::Vector3> &_v, vector< vector<unsigned short> > &_s) {
  if ((TriMorphHeader.Identifier[0] != 'F') ||
      (TriMorphHeader.Identifier[1] != 'R'))
    return;
  if ((TriMorphHeader.FileType[0] != 'T') ||
      (TriMorphHeader.FileType[1] != 'R') ||
      (TriMorphHeader.FileType[2] != 'I'))
    return;

  int nf = 0;
  for (unsigned short ss = 0; ss < ns; ss++)
    nf += (int)_s[ss].size() - 2;

  if ((nv == TriMorphHeader.NumVertices) &&
      (nf == TriMorphHeader.NumPolyTris)) {
    int mv = TriMorphHeader.NumModifierVertices;
    int ierr = 0;

    /* spit out some info */
    nfoprintf(stdout, "converting accompanying tri:\n");

    if (TriMorphHeader.NumPolyTris) {
      std::vector<TriMorph_triCOORDS> mr = TriMorphRecords.FaceList;
      std::vector<TriMorph_triCOORDS> mn;

      assert(mr.size() == nf);

      // Cycle through all strips
      int ms = 0; // Current triangle
      vector< vector<unsigned short> >::const_iterator it;
      for (it = _s.begin(); it != _s.end(); ++it) {
	struct TriMorph_triCOORDS tri;

	// The first three values in the strip are the first triangle
	tri.a = (*it)[0];
	tri.b = (*it)[1];
	tri.c = (*it)[2];

	int fa = mr[ms].a; if (fa != tri.a) ierr++;
	int fb = mr[ms].b; if (fb != tri.b) ierr++;
	int fc = mr[ms].c; if (fc != tri.c) ierr++;

	// Only add triangles to the list if none of the vertices match
	if ((tri.a != tri.b) &&
	    (tri.a != tri.c) &&
	    (tri.b != tri.c)) {
	  mn.push_back(tri);
	}

	// Move to the next triangle
	ms++;

	// The remaining triangles use the previous two indices as their first two indices.
	for (unsigned int i = 3; i < it->size(); ++i) {
	  // Odd numbered triangles need to be reversed to keep the vertices in counter-clockwise order
	  if ((i % 2) == 0) {
	    tri.a = (*it)[i - 2];
	    tri.b = (*it)[i - 1];
	    tri.c = (*it)[i - 0];
	  }
	  else {
	    tri.a = (*it)[i - 0];
	    tri.b = (*it)[i - 1];
	    tri.c = (*it)[i - 2];
	  }

	  fa = mr[ms].a; if (fa != tri.a) ierr++;
	  fb = mr[ms].b; if (fb != tri.b) ierr++;
	  fc = mr[ms].c; if (fc != tri.c) ierr++;

	  // Only add triangles to the list if none of the vertices match
	  if ((tri.a != tri.b) &&
	      (tri.a != tri.c) &&
	      (tri.b != tri.c)) {
	    mn.push_back(tri);
	  }

	  // Move to the next triangle
	  ms++;
	}
      }

      /* set new amount of NumPolyTris */
      TriMorphRecords.FaceList = mn;
      TriMorphHeader.NumPolyTris = (int)mn.size();
    }

    if (TriMorphHeader.NumUVVertices && TriMorphHeader.NumPolyTris) {
      std::vector<TriMorph_triCOORDS> mr = TriMorphRecords.UVFaceList;
      std::vector<TriMorph_triCOORDS> mn;

      assert(mr.size() == nf);

      // Cycle through all strips
      int ms = 0; // Current triangle
      vector< vector<unsigned short> >::const_iterator it;
      for (it = _s.begin(); it != _s.end(); ++it) {
	struct TriMorph_triCOORDS tri;

	// The first three values in the strip are the first triangle
	tri.a = (*it)[0];
	tri.b = (*it)[1];
	tri.c = (*it)[2];

	int fa = mr[ms].a; if (fa != tri.a) ierr++;
	int fb = mr[ms].b; if (fb != tri.b) ierr++;
	int fc = mr[ms].c; if (fc != tri.c) ierr++;

	// Only add triangles to the list if none of the vertices match
	if ((tri.a != tri.b) &&
	    (tri.a != tri.c) &&
	    (tri.b != tri.c)) {
	  mn.push_back(tri);
	}

	// Move to the next triangle
	ms++;

	// The remaining triangles use the previous two indices as their first two indices.
	for (unsigned int i = 3; i < it->size(); ++i) {
	  // Odd numbered triangles need to be reversed to keep the vertices in counter-clockwise order
	  if ((i % 2) == 0) {
	    tri.a = (*it)[i - 2];
	    tri.b = (*it)[i - 1];
	    tri.c = (*it)[i - 0];
	  }
	  else {
	    tri.a = (*it)[i - 0];
	    tri.b = (*it)[i - 1];
	    tri.c = (*it)[i - 2];
	  }

	  fa = mr[ms].a; if (fa != tri.a) ierr++;
	  fb = mr[ms].b; if (fb != tri.b) ierr++;
	  fc = mr[ms].c; if (fc != tri.c) ierr++;

	  // Only add triangles to the list if none of the vertices match
	  if ((tri.a != tri.b) &&
	      (tri.a != tri.c) &&
	      (tri.b != tri.c)) {
	    mn.push_back(tri);
	  }

	  // Move to the next triangle
	  ms++;
	}
      }

      /* set new amount of NumPolyTris */
      TriMorphRecords.UVFaceList = mn;
      TriMorphHeader.NumPolyTris = (int)mn.size();
    }

    nfoprintf(stdout, " Vertices         : %6d\n", nv);
    nfoprintf(stdout, " Faces            : %6d to %6d (%.4f%%)\n", nf, TriMorphHeader.NumPolyTris, 100.0f * TriMorphHeader.NumPolyTris / nf);

    if (ierr)
      addnote(" Strips in NIF and TRI mismatched, corrected!\n");

    TriMorphConv = true;
  }
}

/* ---------------------------------------------------- */

void optimize_tri(vector<Niflib::Vector3> & v, vector<Niflib::Triangle> & f, vector<Niflib::TexCoord> & uv,
		  vector<Niflib::Vector3> &_v, vector<Niflib::Triangle> &_f, vector<Niflib::TexCoord> &_uv) {
  if ((TriMorphHeader.Identifier[0] != 'F') ||
      (TriMorphHeader.Identifier[1] != 'R'))
    return;
  if ((TriMorphHeader.FileType[0] != 'T') ||
      (TriMorphHeader.FileType[1] != 'R') ||
      (TriMorphHeader.FileType[2] != 'I'))
    return;

  int nv = (int)v.size();
  int nf = (int)f.size();
  if ((nv == TriMorphHeader.NumVertices) &&
      (nf == TriMorphHeader.NumPolyTris)) {
    int mv = TriMorphHeader.NumModifierVertices;

    /* spit out some info */
//  barprintf(stdout, '.', "accompanying tri");
    nfoprintf(stdout, "processing accompanying tri:\n");

    if (TriMorphHeader.NumVertices) {
      std::vector<TriMorph_fCOORDS> mr = TriMorphRecords.VertexList;
      std::vector<TriMorph_fCOORDS> mn;

      assert(mr.size() == nv);

      /* check for identity */
      bool equal = true;
      int mc = (int)mr.size();
      for (int ms = 0; ms < mc; ms++) {
	/* soft check */
	if ((fabsf(mr[ms].x - v[ms].x) > 1e-5) ||
	    (fabsf(mr[ms].y - v[ms].y) > 1e-5) ||
	    (fabsf(mr[ms].z - v[ms].z) > 1e-5)) {
	  equal = false;
	  break;
	}
      }

      if (!equal) {
	if (droptrimeshes)
	  addnote(" Vertices in NIF and TRI mismatch, fixed!\n");
	else
	  addnote(" Vertices in NIF and TRI mismatch, file may have problems!\n");
      }

      /* identical */
      if (equal || droptrimeshes) {
//     !memcmp(&mr[0], & v[0],  v.size() * sizeof(TriMorph_fCOORDS))
	mn.resize(_v.size());
	memcpy(&mn[0], &_v[0], _v.size() * sizeof(TriMorph_fCOORDS));
      }
      /* different */
      else {
	/* old to new to opt */
	int mc = (int)mr.size();
	for (int ms = 0; ms < mc; ms++) {
	  /* skip eliminated indices */
	  int oidx = ms;
	  int nidx = SectorRemap[oidx];
	  if (nidx < 0) continue;
	  int pidx = SectorRemapO[nidx];

	  int stz = (int)mn.size();
	  if (stz <= pidx)
	    mn.resize(pidx + 1);

	  mn[pidx] = mr[oidx];
	}
      }

      assert(mn.size() == _v.size());

      TriMorphRecords.VertexList = mn;
      /* set new amount of NumVertices */
      TriMorphHeader.NumVertices = (int)_v.size();
    }

    if (TriMorphHeader.NumPolyTris) {
      std::vector<TriMorph_triCOORDS> mr = TriMorphRecords.FaceList;
      std::vector<TriMorph_triCOORDS> mn;

      assert(mr.size() == nf);

      /* check for identity */
      bool equal = true;
      int mc = (int)mr.size();
      for (int ms = 0; ms < mc; ms++) {
	TriMorph_triCOORDS &src = mr[ms];
	Niflib::Triangle   &dst =  f[ms];

	/* soft check */
	if ((src.a != dst.v1) ||
	    (src.b != dst.v2) ||
	    (src.c != dst.v3)) {
	  equal = false;
	  break;
	}
      }

      if (!equal) {
	if (droptrimeshes)
	  addnote(" Faces in NIF and TRI mismatch, fixed!\n");
	else
	  addnote(" Faces in NIF and TRI mismatch, file may have problems!\n");
      }

      /* identical */
      if (equal || droptrimeshes) {
//     !memcmp(&mr[0], & f[0],  f.size() * sizeof(TriMorph_triCOORDS))) {
	mn.resize(_f.size());
//	memcpy(&mn[0], &_f[0], _f.size() * sizeof(TriMorph_triCOORDS));

	int mc = (int)mn.size();
	for (int ms = 0; ms < mc; ms++) {
	  mn[ms].a = _f[ms].v1;
	  mn[ms].b = _f[ms].v2;
	  mn[ms].c = _f[ms].v3;
	}
      }
      /* different */
      else {
	/* old to new to opt */
	int mc = (int)mr.size();
	for (int ms = 0; ms < mc; ms++) {
	  /* skip eliminated indices */
	  int oidx1 = mr[ms].a;
	  int oidx2 = mr[ms].b;
	  int oidx3 = mr[ms].c;
	  int nidx1 = SectorRemap[oidx1];
	  int nidx2 = SectorRemap[oidx2];
	  int nidx3 = SectorRemap[oidx3];
	  if (nidx1 < 0) continue;
	  if (nidx2 < 0) continue;
	  if (nidx3 < 0) continue;
	  int pidx1 = SectorRemapO[nidx1];
	  int pidx2 = SectorRemapO[nidx2];
	  int pidx3 = SectorRemapO[nidx3];
	  if (pidx1 == pidx2) continue;
	  if (pidx2 == pidx3) continue;
	  if (pidx3 == pidx1) continue;

	  TriMorph_fCOORDS f1 = TriMorphRecords.VertexList[pidx1];
	  TriMorph_fCOORDS f2 = TriMorphRecords.VertexList[pidx2];
	  TriMorph_fCOORDS f3 = TriMorphRecords.VertexList[pidx3];
	  if ((f1.x == f2.x) && (f1.y == f2.y) && (f1.z == f2.z)) continue;
	  if ((f2.x == f3.x) && (f2.y == f3.y) && (f2.z == f3.z)) continue;
	  if ((f3.x == f1.x) && (f3.y == f1.y) && (f3.z == f1.z)) continue;

	  struct TriMorph_triCOORDS tri;

	  tri.a = pidx1;
	  tri.b = pidx2;
	  tri.c = pidx3;

	  mn.push_back(tri);
	}
      }

      assert(mn.size() == _f.size());

      /* set new amount of NumPolyTris */
      TriMorphRecords.FaceList = mn;
      TriMorphHeader.NumPolyTris = (int)_f.size();
    }

    if (TriMorphHeader.NumUVVertices) {
      std::vector<TriMorph_uvCOORDS> mr = TriMorphRecords.UVVertexList;
      std::vector<TriMorph_uvCOORDS> mn;

      assert(mr.size() == nv);

      /* check for identity */
      bool equal = true;
      int mc = (int)mr.size();
      for (int ms = 0; ms < mc; ms++) {
	/* soft check */
	if ((fabsf(mr[ms].u - uv[ms].u) > 1e-5) ||
	    (fabsf(mr[ms].v - uv[ms].v) > 1e-5)) {
	  equal = false;
	  break;
	}
      }

      if (!equal) {
	if (droptrimeshes)
	  addnote(" UV-Coords in NIF and TRI mismatch, fixed!\n");
	else
	  addnote(" UV-Coords in NIF and TRI mismatch, file may have problems!\n");
      }

      /* identical */
      if (equal || droptrimeshes) {
//     !memcmp(&mr[0], & uv[0],  uv.size() * sizeof(TriMorph_uvCOORDS))
	mn.resize(_uv.size());
	memcpy(&mn[0], &_uv[0], _uv.size() * sizeof(TriMorph_uvCOORDS));
      }
      /* different */
      else {
	/* old to new to opt */
	int mc = (int)mr.size();
	for (int ms = 0; ms < mc; ms++) {
	  /* skip eliminated indices */
	  int oidx = ms;
	  int nidx = SectorRemap[oidx];
	  if (nidx < 0) continue;
	  int pidx = SectorRemapO[nidx];

	  int stz = (int)mn.size();
	  if (stz <= pidx)
	    mn.resize(pidx + 1);

	  mn[pidx] = mr[oidx];
	}
      }

      assert(mn.size() == _v.size());

      /* set new amount of NumUVVertices */
      TriMorphRecords.UVVertexList = mn;
      TriMorphHeader.NumUVVertices = (int)_v.size();
    }

    if (TriMorphHeader.NumUVVertices && TriMorphHeader.NumPolyTris) {
      std::vector<TriMorph_triCOORDS> mr = TriMorphRecords.UVFaceList;
      std::vector<TriMorph_triCOORDS> mn;

      assert(mr.size() == nf);

      /* check for identity */
      bool equal = true;
      int mc = (int)mr.size();
      for (int ms = 0; ms < mc; ms++) {
	TriMorph_triCOORDS &src = mr[ms];
	Niflib::Triangle   &dst =  f[ms];

	/* soft check */
	if ((src.a != dst.v1) ||
	    (src.b != dst.v2) ||
	    (src.c != dst.v3)) {
	  equal = false;
	  break;
	}
      }

      if (!equal) {
	if (droptrimeshes)
	  addnote(" UV-Faces in NIF and TRI mismatch, fixed!\n");
	else
	  addnote(" UV-Faces in NIF and TRI mismatch, file may have problems!\n");
      }

      /* identical */
      if (equal || droptrimeshes) {
//     !memcmp(&mr[0], & f[0],  f.size() * sizeof(TriMorph_triCOORDS))) {
	mn.resize(_f.size());
//	memcpy(&mn[0], &_f[0], _f.size() * sizeof(TriMorph_triCOORDS));

	int mc = (int)mn.size();
	for (int ms = 0; ms < mc; ms++) {
	  mn[ms].a = _f[ms].v1;
	  mn[ms].b = _f[ms].v2;
	  mn[ms].c = _f[ms].v3;
	}
      }
      /* different */
      else {
	/* old to new to opt */
	int mc = (int)mr.size();
	for (int ms = 0; ms < mc; ms++) {
	  /* skip eliminated indices */
	  int oidx1 = mr[ms].a;
	  int oidx2 = mr[ms].b;
	  int oidx3 = mr[ms].c;
	  int nidx1 = SectorRemap[oidx1];
	  int nidx2 = SectorRemap[oidx2];
	  int nidx3 = SectorRemap[oidx3];
	  if (nidx1 < 0) continue;
	  if (nidx2 < 0) continue;
	  if (nidx3 < 0) continue;
	  int pidx1 = SectorRemapO[nidx1];
	  int pidx2 = SectorRemapO[nidx2];
	  int pidx3 = SectorRemapO[nidx3];
	  if (pidx1 == pidx2) continue;
	  if (pidx2 == pidx3) continue;
	  if (pidx3 == pidx1) continue;

	  TriMorph_uvCOORDS f1 = TriMorphRecords.UVVertexList[pidx1];
	  TriMorph_uvCOORDS f2 = TriMorphRecords.UVVertexList[pidx2];
	  TriMorph_uvCOORDS f3 = TriMorphRecords.UVVertexList[pidx3];
	  if ((f1.u == f2.u) && (f1.v == f2.v)) continue;
	  if ((f2.u == f3.u) && (f2.v == f3.v)) continue;
	  if ((f3.u == f1.u) && (f3.v == f1.v)) continue;

	  struct TriMorph_triCOORDS tri;

	  tri.a = pidx1;
	  tri.b = pidx2;
	  tri.c = pidx3;

	  mn.push_back(tri);
	}
      }

      assert(mn.size() == _f.size());

      /* set new amount of NumPolyTris */
      TriMorphRecords.UVFaceList = mn;
      TriMorphHeader.NumPolyTris = (int)_f.size();
    }

    if (TriMorphHeader.NumVertices) {
      std::vector<TriMorph_fCOORDS> mr = TriMorphRecords.VertexList;
      std::vector<TriMorph_fCOORDS> mn;

      assert(mr.size() == nv);

      /* old to new to opt */
      int mc = (int)mr.size();
      for (int ms = 0; ms < mc; ms++) {
	/* skip eliminated indices */
	int oidx = ms;
	int nidx = SectorRemap[oidx];
	if (nidx < 0) continue;
	int pidx = SectorRemapO[nidx];

	int stz = (int)mn.size();
	if (stz <= pidx)
	  mn.resize(pidx + 1);

	mn[pidx] = mr[oidx];
      }

      assert(mn.size() == _v.size());

      /* set new amount of NumVertices */
      TriMorphRecords.VertexList = mn;
      TriMorphHeader.NumVertices = (int)_v.size();
    }

    for (int m = 0; m < TriMorphHeader.NumMorphRecords; m++) {
      std::vector<TriMorphVertex> mr = TriMorphDatas[m].VertexList;
      std::vector<TriMorphVertex> mn;

      assert(mr.size() == nv);

      /* old to new to opt */
      int mc = (int)mr.size();
      for (int ms = 0; ms < mc; ms++) {
	/* skip eliminated indices */
	int oidx = ms;
	int nidx = SectorRemap[oidx];
	if (nidx < 0) continue;
	int pidx = SectorRemapO[nidx];

	int stz = (int)mn.size();
	if (stz <= pidx)
	  mn.resize(pidx + 1);

	mn[pidx] = mr[oidx];
      }

      assert(mn.size() == _v.size());

      /* update morph-data */
      TriMorphDatas[m].VertexList = mn;
    }

    for (int m = 0; m < TriMorphHeader.NumModifiers; m++) {
      int   mc = TriMorphModifiers[m].datasize;
      int  *mr = TriMorphModifiers[m].miscdata;
      bool *sr = TriMorphModifiers[m].skpdata;
      int  *mn = new int[mc];
      int  *mp = mn;

      /* old to new to opt */
      for (int ms = 0; ms < mc; ms++) {
	/* skip eliminated indices */
	int oidx = mr[ms];
	int nidx = SectorRemap[oidx];
	if ((*sr++ = (nidx < 0))) continue;
	int pidx = SectorRemapO[nidx];

	*mp++ = pidx;
      }

      /* update modifier list of indices */
      TriMorphModifiers[m].datasize = (int)(mp - mn);
      TriMorphModifiers[m].miscdata = mn;

      /* set new amount of NumModifierVertices */
      TriMorphHeader.NumModifierVertices -= mc - (int)(mp - mn);

      delete[] mr;
    }

    nfoprintf(stdout, " Vertices         : %6d to %6d (%.4f%%)\n", nv, TriMorphHeader.NumVertices, 100.0f * TriMorphHeader.NumVertices / nv);
    nfoprintf(stdout, " Faces            : %6d to %6d (%.4f%%)\n", nf, TriMorphHeader.NumPolyTris, 100.0f * TriMorphHeader.NumPolyTris / nf);

    nfoprintf(stdout, " Morph Records    : %d\n", TriMorphHeader.NumMorphRecords);

    /* explicitly mention all morph-names */
    if (verbose && TriMorphHeader.NumMorphRecords) {
      for (int m = 0; m < TriMorphHeader.NumMorphRecords; m++) {
	struct TriMorphData &mr = TriMorphDatas[m];

	if (!m) {
	  nfoprintf(stdout, " Morph Groups     : %6d to %6d %s\n", nv, mr.VertexList.size(), mr.name); }
	else {
	  nfoprintf(stdout, "                    %6d to %6d %s\n", nv, mr.VertexList.size(), mr.name); }
      }
    }

    nfoprintf(stdout, " Modifiers        : %d\n", TriMorphHeader.NumModifiers); if (TriMorphHeader.NumModifiers)
    nfoprintf(stdout, " Modifier Vertices: %6d to %6d (%.4f%%)\n", mv, TriMorphHeader.NumModifierVertices, 100.0f * TriMorphHeader.NumModifierVertices / mv);

    /* explicitly mention all modifier-names */
    if (verbose && TriMorphHeader.NumModifiers) {
      for (int m = 0; m < TriMorphHeader.NumModifiers; m++) {
	struct TriMorphModifier &mr = TriMorphModifiers[m];

	if (!m) {
	  nfoprintf(stdout, " Modifier Groups  : %6d to %6d %s\n", mr.dataskps, mr.datasize, mr.name); }
	else {
	  nfoprintf(stdout, "                    %6d to %6d %s\n", mr.dataskps, mr.datasize, mr.name); }
      }
    }

    TriMorphProc = true;
  }
}
