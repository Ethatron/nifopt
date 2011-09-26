// ***** BEGIN LICENSE BLOCK *****
//
// Copyright (c) 2006-2008, NIF File Format Library and Tools.
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//
//    * Redistributions of source code must retain the above copyright
//      notice, this list of conditions and the following disclaimer.
//
//    * Redistributions in binary form must reproduce the above
//      copyright notice, this list of conditions and the following
//      disclaimer in the documentation and/or other materials provided
//      with the distribution.
//
//    * Neither the name of the NIF File Format Library and Tools
//      project nor the names of its contributors may be used to endorse
//      or promote products derived from this software without specific
//      prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
// FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
// COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
// INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
// BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
// LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
// ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//
// ***** END LICENSE BLOCK *****

struct hvkPoint3
{
	float x, y, z;
	void Set(float x, float y, float z){
		this->x = x; this->y = y; this->z = z;
	}
};

struct hvkPoint4
{
	float x, y, z, w;
	void Set(float x, float y, float z, float w){
		this->x = x; this->y = y; this->z = z; this->w = w;
	}
};

struct hvkTriangle
{
	unsigned short a, b, c;

	unsigned short operator[](size_t i) const {
		switch (i)
		{
		case 0: return a;
		case 1: return b;
		case 2: return c;
		default: throw std::exception("Invalid index");
		}
	}
	unsigned short& operator[](size_t i) {
		switch (i)
		{
		case 0: return a;
		case 1: return b;
		case 2: return c;
		default: throw std::exception("Invalid index");
		}
	}
};

struct hvkTriangle4
{
	unsigned short a, b, c, m_weldingInfo;
};

struct hvkMatrix3
{
	float m[3][3];
};

struct hvkMatrix43
{
	float m[4][3];
};

struct hvkMatrix44
{
	float m[4][4];
};

typedef	unsigned char	hvkByte;
typedef	unsigned short	hvkShort;

/* ############################################################################ */
#ifdef _WIN64
#include <exception>

#include "nifopt-hvk_c.c"
#include "nifopt-hvk-c.c"

HANDLE jobinfo;
STARTUPINFO startinfo;
PROCESS_INFORMATION processinfo;

bool HavokBegin(bool havokserver) {
  memset(&startinfo, 0, sizeof(startinfo));
  startinfo.cb = sizeof(startinfo);
  jobinfo = CreateJobObject(NULL, NULL);

  /* reach out for the server first */
//if (!HavokClientInit()) {
    /* then try to create it */
    CreateProcess(
#ifndef NDEBUG
      "nifopt_x32d.exe",
#else
      "nifopt_x32.exe",
#endif
      " -havokserver",
      NULL,
      NULL,
      TRUE,
      /*CREATE_NO_WINDOW | DEBUG_PROCESS | */
      CREATE_NEW_CONSOLE | INHERIT_PARENT_AFFINITY,
      NULL,
      NULL,
      &startinfo,
      &processinfo
    );

    assert(processinfo.hProcess);
    if (!processinfo.hProcess)
      return false;
    
    // Configure all child processes associated with the job to terminate when the
    JOBOBJECT_EXTENDED_LIMIT_INFORMATION
    jeli = { 0 };
    jeli.BasicLimitInformation.LimitFlags = JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE;

    AssignProcessToJobObject(jobinfo, processinfo.hProcess);
    SetInformationJobObject(jobinfo, JobObjectExtendedLimitInformation, &jeli, sizeof(jeli));

    Sleep(2000);

    return HavokClientInit();
//}
//
//return true;
}

void HavokEnd(bool havokserver) {
  HavokShutdown();
  HavokClientExit();

  Sleep(2000);
  CloseHandle(jobinfo);
}

/* ############################################################################ */
#else
#include <eh.h>
#include <exception>

void catch_func(unsigned int u, EXCEPTION_POINTERS* pExp) {
  throw runtime_error("SEH Exception");
}

#include "nifopt-hvk_s.c"
#include "nifopt-hvk-s.c"

/* ---------------------------------------------------- */

// Math and base include
#include <Common/Base/hkBase.h>
#include <Common/Base/System/hkBaseSystem.h>
#include <Common/Base/Memory/hkThreadMemory.h>
#include <Common/Base/Memory/Memory/Pool/hkPoolMemory.h>
#include <Common/Base/System/Error/hkDefaultError.h>
#include <Common/Base/Monitor/hkMonitorStream.h>

#include <Common/Base/System/Io/FileSystem/hkFileSystem.h>
#include <Common/Base/Container/LocalArray/hkLocalBuffer.h>

//#include <Common/hkAnimationPhysicsPublicInclude.h>
#include <Common/Base/Types/Geometry/hkGeometry.h>
#include <Common/Base/Types/Geometry/hkStridedVertices.h>
#include <Physics/Utilities/Dynamics/Inertia/hkpInertiaTensorComputer.h>
#include <Common/Internal/ConvexHull/hkGeometryUtility.h>
#include <Physics/Collide/Shape/Convex/Box/hkpBoxShape.h>
#include <Physics/Collide/Shape/Convex/ConvexTranslate/hkpConvexTranslateShape.h>
#include <Physics/Collide/Shape/Convex/ConvexTransform/hkpConvexTransformShape.h>
#include <Physics/Collide/Shape/Compound/Collection/SimpleMesh/hkpSimpleMeshShape.h>
#include <Physics/Collide/Shape/Compound/Collection/ExtendedMeshShape/hkpExtendedMeshShape.h>
#include <Physics/Collide/Shape/Deprecated/Mesh/hkpMeshShape.h>
#include <Physics/Collide/Shape/Compound/Collection/List/hkpListShape.h>
#include <Physics/Collide/Shape/Compound/Collection/Mesh/hkpMeshMaterial.h>

#include <Physics/Collide/Shape/Convex/Capsule/hkpCapsuleShape.h>
#include <Physics/Collide/Shape/Compound/Tree/Mopp/hkpMoppBvTreeShape.h>
#include <Physics/Collide/Shape/Compound/Tree/Mopp/hkpMoppUtility.h>
#include <Physics/Internal/Collide/Mopp/Code/hkpMoppCode.h>

#pragma comment(lib, "hkBase.lib")
#pragma comment(lib, "hkSerialize.lib")
#pragma comment(lib, "hkInternal.lib")
#pragma comment(lib, "hkpInternal.lib")
#pragma comment(lib, "hkpUtilities.lib")
#pragma comment(lib, "hkpCollide.lib")
#pragma comment(lib, "hkpConstraintSolver.lib")
#pragma comment(lib, "hkpDynamics.lib")

static hkPoolMemory *memoryManager = NULL;
map<hkThreadMemory *,char *> stackBuffer;
int req = 0;

hkThreadMemory *HavocThreadInit(const char *which) {
  hkThreadMemory *tm = NULL;

  if (havokserver) {
    fprintf(stdout, "Serving RPC-request %d: %s\r", ++req, which);

//  if (!&hkThreadMemory::getInstance()) {
      tm = new hkThreadMemory(memoryManager/*, 16*/);
      hkBaseSystem::initThread(tm);

      // We now initialize the stack area to 100k (fast temporary memory to be used by the engine).
      {
	int stackSize = 0x100000;
	stackBuffer[tm] = hkAllocate<char>(stackSize, HK_MEMORY_CLASS_BASE);
	tm->setStackArea(stackBuffer[tm], stackSize);
      }
//  }
  }

  return tm;
}

void HavocThreadExit(hkThreadMemory *tm) {
  if (havokserver) {
    if (tm) {
      tm->setStackArea(0, 0);
      hkDeallocate(stackBuffer[tm]);
      stackBuffer[tm] = NULL;
      tm->releaseCachedMemory();
      hkBaseSystem::clearThreadResources();
      tm->removeReference();
    }
  }
}

/* ---------------------------------------------------- */

void CalcOBBPolyhedron(int nVerts, const hvkPoint3 *verts,
		       int nTris, const hvkTriangle *tris,
		       float factor, float tolerance,
		       hvkMatrix44 *boxtransform, bool *identity,
		       hvkPoint3 *boxhalfextents, bool *exclusive) {

    hkThreadMemory *tm = HavocThreadInit("Check orthogonality and calculate OBB");

    /* encapsulate constructors/destructors, as the release
     * of the memory-manager must happen after all destructors
     * have been called
     */
    do {
      hkStridedVertices sv;
      hkVector4 halfExtents;
      hkTransform transform;
  //  hkSphere sphere;

      /* 8 corners + outside */
      bool boxb[8+1] = {false};
      hkVector4 boxv[8+3];

      vector<hvkPoint3> boxverts;
      vector<bool> inu(nVerts);

      for (int ts = 0; ts < nTris; ts++) {
	/* degenerate handling */
	inu[tris[ts].a] =
	inu[tris[ts].b] =
	inu[tris[ts].c] = true;
      }

      for (int vs = 0; vs < nVerts; vs++) {
	/* degenerate handling */
	if (inu[vs])
	  boxverts.push_back(verts[vs]);
      }

      sv.m_vertices = reinterpret_cast<const float *>(&boxverts[0]);
      sv.m_numVertices = (int)boxverts.size();
      sv.m_striding = sizeof(hvkPoint3);

      try {
	_set_se_translator(catch_func);

	/* HK_GEOM_OBB_MODE_BETTER_FIT_BUT_SLOWER */
	hkGeometryUtility::calcObb(sv, halfExtents, transform, HK_GEOM_OBB_MODE_DEFAULT);
  //	sphere = hkGeometryUtility::createMinBoundingSphere(sv, 1e-5f);
      }
      catch(...) {
	/* gracefull abort:
	 *
	 * Oblivion - Meshes\meshes\architecture\basementsections\ungrdexitdoorway.nif
	 * Oblivion - Meshes\meshes\architecture\castle\castlewallbend07.nif
	 * Oblivion - Meshes\meshes\architecture\castle\skingrad\skcastleroof02half02.nif
	 * Oblivion - Meshes\meshes\architecture\castleinterior\castleintbraiser02.nif
	 * Oblivion - Meshes\meshes\architecture\cheydinhal\castlegatecheydinhal01.nif
	 * Oblivion - Meshes\meshes\architecture\chorrol\interior\chorrolmguildwallupper01.nif
	 * Oblivion - Meshes\meshes\architecture\farmhouse\farmhouseconstruction01.nif
	 * Oblivion - Meshes\meshes\architecture\imperialcity\icaugatewall01.nif
	 * Oblivion - Meshes\meshes\architecture\imperialcity\ictalosblock03house01.nif
	 * Oblivion - Meshes\meshes\architecture\imperialcity\ictalosblock04house01.nif
	 * Oblivion - Meshes\meshes\architecture\imperialcity\interior\icwallint01.nif
	 * Oblivion - Meshes\meshes\architecture\leyawiin\exterior\leyawiinfightersguildhouse.nif
	 * Oblivion - Meshes\meshes\architecture\leyawiin\exterior\leyawiinhouselower04.nif
	 * Oblivion - Meshes\meshes\architecture\leyawiin\exterior\leyawiinhouseupper07.nif
	 * Oblivion - Meshes\meshes\architecture\ships\interiorset\wreckedsiwall01.nif
	 * Oblivion - Meshes\meshes\dungeons\ayleidruins\exterior\arringouterwall01.nif
	 * Oblivion - Meshes\meshes\dungeons\fortruins\castle\stonerubble\rfxstonemedh.nif
	 * Oblivion - Meshes\meshes\furniture\middleclass\middlechair04.nif
	 * LetThePeopleDrink\Meshes\Architecture\ImperialCity\Aqueducts+Fountains\AQAqueductArch.nif
	 */
	*exclusive = false;
	break;
      }

      /* copy out transform (row-major!) */
      hkRotation rot = transform.getRotation();
      hkVector4 trns = transform.getTranslation();
      float mat[4][4] = {
	rot(0,0), rot(1,0), rot(2,0), trns(0),
	rot(0,1), rot(1,1), rot(2,1), trns(1),
	rot(0,2), rot(1,2), rot(2,2), trns(2),
	0.0f    , 0.0f    , 0.0f    , 0.0f
      };

      memcpy(boxtransform, &mat, sizeof(mat));

//    transform.get4x4ColumnMajor(reinterpret_cast<hkReal *>(boxtransform));

      /* create the OBB */
      boxv[ 0] = halfExtents; boxv[ 0].mul4(hkVector4( 1, 1, 1, 0));
      boxv[ 1] = halfExtents; boxv[ 1].mul4(hkVector4( 1, 1,-1, 0));
      boxv[ 2] = halfExtents; boxv[ 2].mul4(hkVector4( 1,-1, 1, 0));
      boxv[ 3] = halfExtents; boxv[ 3].mul4(hkVector4( 1,-1,-1, 0));
      boxv[ 4] = halfExtents; boxv[ 4].mul4(hkVector4(-1, 1, 1, 0));
      boxv[ 5] = halfExtents; boxv[ 5].mul4(hkVector4(-1, 1,-1, 0));
      boxv[ 6] = halfExtents; boxv[ 6].mul4(hkVector4(-1,-1, 1, 0));
      boxv[ 7] = halfExtents; boxv[ 7].mul4(hkVector4(-1,-1,-1, 0));
      boxv[ 8] = halfExtents; boxv[ 8].mul4(hkVector4( 1, 0, 0, 0));
      boxv[ 9] = halfExtents; boxv[ 9].mul4(hkVector4( 0, 1, 0, 0));
      boxv[10] = halfExtents; boxv[10].mul4(hkVector4( 0, 0, 1, 0));

      boxhalfextents->x = halfExtents(0);
      boxhalfextents->y = halfExtents(1);
      boxhalfextents->z = halfExtents(2);

      /* check for void transform/rotation */
      hkVector4 eps(1e-6f, 1e-6f, 1e-6f, 1e-6f);

      /* absolute distance from 0 */
      trns.setAbs4(trns);
      *identity = false;
      if (trns.allLessThan3(eps) &&
	  rot.isOrthonormal(1.0f - 1e-6f))
	*identity = true;

      /* match versus an OBB/MVBB  */
      for (int vs = 0; vs < nVerts; vs++) {
	hvkPoint3 vec = verts[vs];
	hkVector4 hvkv(vec.x, vec.y, vec.z, 0.0f), hvka;

	/* degenerate handling */
	if (!inu[vs])
	  continue;

	bool boxm = false;

	/* local coordinates */
	hvka.setTransformedInversePos(transform, hvkv);

	/* match corners */
	for (int c = 0; c < 8; c++) {
	  /* it's a corner */
	  if (hvka.equals3(boxv[c], tolerance)) {
	    boxm = boxb[c] = true;
	    break;
	  }
	}

	/* match planes */
	if (!boxm) {
	  /* absolute local coordinates */
	  hvkv.setAbs4(hvka);

	  /* match sides (one axis is sufficient) */
	  if ((fabs(hvkv(0) - boxv[ 8](0)) < tolerance) ||
	      (fabs(hvkv(1) - boxv[ 9](1)) < tolerance) ||
	      (fabs(hvkv(2) - boxv[10](2)) < tolerance)) {
	    boxm = true;
	    break;
	  }

	  /* test complete axi */
	  for (int c = 8; c < 11; c++) {
	    if (hvkv.equals3(boxv[c], tolerance)) {
	      boxm = true;
	      break;
	    }
	  }
	}

	/* vertex matches the box's surface exactly */
	boxb[8] = boxb[8] || !boxm;
      }

      int boxs = 0;
      for (int c = 0; c <= 8; c++)
	boxs += boxb[c] ? 1 : 0;

      *exclusive = (boxs == 8);
    } while (0);

    HavocThreadExit(tm);
}

void CalcHullPolyhedron(int nVerts, const hvkPoint3 *verts,
			int nTris, const hvkTriangle *tris,
			float factor, float tolerance,
			int *nPlanes, hvkPoint4 *planes,
			bool *exclusive) {

    hkThreadMemory *tm = HavocThreadInit("Check convexity and calculate convex shape");

    /* encapsulate constructors/destructors, as the release
     * of the memory-manager must happen after all destructors
     * have been called
     */
    do {
      /* convert every triangle into plane-equation form
       * Ax + By + Cz + D = 0
       */
      class hkVector4e : public hkVector4 {
      public:
	bool operator < (const class hkVector4e &other) const {
	  /* esp for the normal has to be low, eps for
	   * the distance has higher magnitude
	   *
	   * you can check the behavior on:
	   * MidasSpells\meshes\architecture\midasastralplane\midasastralplatform01.nif
	   */
	  const hkVector4 eps(1e-6f, 1e-6f, 1e-6f, 1e-3f);
	  hkVector4 s3, s4;

	  /* equal-check with tolerance */
	  s3.setSub4(*this, other);
	  s4.setAbs4(s3);
	  if (s4.allLessThan4(eps))
	    return 0;

	  const hkReal *t = &(*this)(0);
	  const hkReal *o = &(other)(0);

	  /* regular sorting (endianess-aware, so no memcmp) */
	  return (t[0] != o[0]) ? t[0] < o[0] /* sign(s3(0)) */
	       : (t[1] != o[1]) ? t[1] < o[1] /* sign(s3(1)) */
	       : (t[2] != o[2]) ? t[2] < o[2] /* sign(s3(2)) */
	       : (t[3] != o[3]) ? t[3] < o[3] /* sign(s3(3)) */
	       : 0;

	  /* regular sorting
	  return memcmp(&t, &o, sizeof(hkVector4)) < 0; */
	}
      };

      const hkVector4 eps(1e-6f, 1e-6f, 1e-6f, 1e-6f);
      set<hkVector4e> planeEquationsRaw;
      set<hkVector4e>::iterator per;
      vector<bool> inu(nVerts);

      for (int ts = 0; ts < nTris; ts++) {
	const hvkPoint3 *anchor  = verts + tris[ts].a;
	const hvkPoint3 *target1 = verts + tris[ts].b;
	const hvkPoint3 *target2 = verts + tris[ts].c;

	/* degenerate handling */
	inu[tris[ts].a] =
	inu[tris[ts].b] =
	inu[tris[ts].c] = true;

	/* degenerate handling */
	if ((anchor->x == target1->x) &&
	    (anchor->y == target1->y) &&
	    (anchor->z == target1->z)) continue;
	if ((anchor->x == target2->x) &&
	    (anchor->y == target2->y) &&
	    (anchor->z == target2->z)) continue;
	if ((target1->x == target2->x) &&
	    (target1->y == target2->y) &&
	    (target1->z == target2->z)) continue;

	hkVector4e pe;
	hkVector4 _anchor ( anchor->x             , anchor->y             , anchor->z             );
	hkVector4 _target1( anchor->x - target1->x, anchor->y - target1->y, anchor->z - target1->z);
	hkVector4 _target2( anchor->x - target2->x, anchor->y - target2->y, anchor->z - target2->z);

	/* vector perpendicular to t1 and t2 */
	pe.setCross(_target1, _target2);

	/* snap the normal */
	if (fabsf(pe(0)) < 0.002f) pe(0) = 0.0f;
	if (fabsf(pe(1)) < 0.002f) pe(1) = 0.0f;
	if (fabsf(pe(2)) < 0.002f) pe(2) = 0.0f;

	/* all vertices on a line handling */
	hkVector4 z; z.setAbs4(pe);
	if (z.allLessThan3(eps))
	   continue;

	/* distance to (0,0) diagonal */
	hkReal dlength = pe.dot3fpu(_anchor);

	/* inverse length of normal */
	hkReal length = pe.length3();
	hkReal ilength = 1.0f / length;

	/* distance to (0,0) perpendicular, inverse */
	pe(3) = -dlength;
//	pe.mul4(ilength);
	pe(0) = pe(0) / length;
	pe(1) = pe(1) / length;
	pe(2) = pe(2) / length;
	pe(3) = pe(3) / length;

	/* register, with tolerance */
	planeEquationsRaw.insert(pe);
      }

      /* put the unique set of plane equations into the comparison-vector (sorted) */
      hkArray<hkVector4> planeEquations;
      for (per = planeEquationsRaw.begin(); per != planeEquationsRaw.end(); per++)
	planeEquations.pushBack(*per);
      planeEquationsRaw.clear();

      /* number of vertices on/inside/outside the hull */
      int onh = 0, inh = 0, ouh = 0;

      /* check all vertices versus all planes [-1,0,+1] */
      int nPlns = (int)planeEquations.getSize();
      for (int vs = 0; vs < nVerts; vs++) {
	hvkPoint3 vec = verts[vs];
	hkVector4 hvkv(vec.x, vec.y, vec.z, 0.0f);

	/* degenerate handling */
	if (!inu[vs])
	  continue;

	bool onp = false,
	     ins = false,
	     ous = false;

	for (int ps = 0; ps < nPlns; ps++) {
	  /* Ax + By + Cz + D = 0 */
	  hkVector4 plane = planeEquations[ps];
	  hkReal dist, side;
#ifndef	NDEBUG
	  hkReal A = plane(0);
	  hkReal B = plane(1);
	  hkReal C = plane(2);
	  hkReal D = plane(3);
	  hkReal x = hvkv(0);
	  hkReal y = hvkv(1);
	  hkReal z = hvkv(2);
#endif
  	
	  dist = hvkv.dot3fpu(plane);
	  side = dist + plane(3);

	  /* it's at least on one plane */
	  if (fabsf(side) < tolerance)
	    onp = onp || true;
	  else if (side <= 0.0f)
	    ins = ins || true;
	  else
	    ous = ous || true;
	}

	/* it's on one plane an inside all others */
	if (!ous) {
	  if (onp)
	    onh++;
	  else if (ins)
	    inh++;
	}
	else
	  ouh++;
      }

      /* number of unshared edges */
      int uns = 0;

      /* it's convex, but is it also closed? */
      if (!inh && !ouh) {
	/* you can proof closeness by checking if all edges
	 * are shared by two triangles, one forward, one backward
	 * (because of CCWity)
	 */
	struct Edge {
	  hvkPoint3 p1, p2;
	  
	  mutable bool f, b;
	  mutable unsigned short n;

	  bool operator < (const struct Edge &other) const {
	    /* regular sorting */
	    return memcmp(this, &other, sizeof(hvkPoint3) * 2) < 0; 
	  }
	};

	set<struct Edge> edges;
	set<struct Edge>::iterator etr;

	for (int ts = 0; ts < nTris; ts++) {
	  const hvkPoint3 *p1 = verts + tris[ts].a;
	  const hvkPoint3 *p2 = verts + tris[ts].b;
	  const hvkPoint3 *p3 = verts + tris[ts].c;

	  /* degenerate handling */
	  if ((p1->x == p2->x) &&
	      (p1->y == p2->y) &&
	      (p1->z == p2->z)) continue;
	  if ((p1->x == p3->x) &&
	      (p1->y == p3->y) &&
	      (p1->z == p3->z)) continue;
	  if ((p2->x == p3->x) &&
	      (p2->y == p3->y) &&
	      (p2->z == p3->z)) continue;

	  /* assign CCW-edge */
	  struct Edge e1; memset(&e1, 0, sizeof(struct Edge)); e1.p1 = *p1; e1.p2 = *p2;
	  struct Edge e2; memset(&e2, 0, sizeof(struct Edge)); e2.p1 = *p2; e2.p2 = *p3;
	  struct Edge e3; memset(&e3, 0, sizeof(struct Edge)); e3.p1 = *p3; e3.p2 = *p1;

	  /* assign sorted edge (p1 always < p2) */
	  if (memcmp(&e1.p1, &e1.p2, sizeof(hvkPoint3)) < 0) e1.p1 = *p2, e1.p2 = *p1;
	  if (memcmp(&e2.p1, &e2.p2, sizeof(hvkPoint3)) < 0) e2.p1 = *p3, e2.p2 = *p2;
	  if (memcmp(&e3.p1, &e3.p2, sizeof(hvkPoint3)) < 0) e3.p1 = *p1, e3.p2 = *p3;

	  /* register and get reference */
	  set<struct Edge>::iterator i1;
	  set<struct Edge>::iterator i2;
	  set<struct Edge>::iterator i3;

	  edges.insert(e1); i1 = edges.find(e1); i1->n++;
	  edges.insert(e2); i2 = edges.find(e2); i2->n++;
	  edges.insert(e3); i3 = edges.find(e3); i3->n++;

	  /* check if we have the CCW or the CW case and mark it */
	  if (!memcmp(&i1->p1, p1, sizeof(hvkPoint3))) i1->f = true;
	  if (!memcmp(&i1->p1, p2, sizeof(hvkPoint3))) i1->b = true;

	  if (!memcmp(&i2->p1, p2, sizeof(hvkPoint3))) i2->f = true;
	  if (!memcmp(&i2->p1, p3, sizeof(hvkPoint3))) i2->b = true;

	  if (!memcmp(&i3->p1, p3, sizeof(hvkPoint3))) i3->f = true;
	  if (!memcmp(&i3->p1, p1, sizeof(hvkPoint3))) i3->b = true;
	}

	for (etr = edges.begin(); etr != edges.end(); etr++) {
	  struct Edge e = *etr;
	  if (!e.f || !e.b)
	    uns++;
	}

	if (uns && verbose)
	  addnote(" Open convex shape, see if you can close it for better performance.\n");
      }

      try {
	_set_se_translator(catch_func);

	/* create a convex hull in any case as we but it into
	 * a havok element, so it needs to be compatible (even
	 * if the equations are roughly the same)
	 *
	 * besides, we need a convex hull (the above code doesn't
	 * do that, it just makes a equation/check per triangle)
	 */
	hkArray<hkVector4> planeEquationsHvk;
	hkStridedVertices sv;
	hkGeometry geom;

	sv.m_vertices = reinterpret_cast<const float *>(&verts[0]);
	sv.m_numVertices = nVerts;
	sv.m_striding = sizeof(hvkPoint3);

	hkGeometryUtility::createConvexGeometry(sv, geom, planeEquationsHvk);

	/* it is convex, it is closed */
	if (!inh && !ouh && !uns) {
	  /* and our equations are bigger ... */
//	  if (planeEquations.getSize() > planeEquationsHvk.getSize()) {
	    planeEquations = planeEquationsHvk;
//	  }
	}
	/* it's not convex really ... */
	else {
	  planeEquations = planeEquationsHvk;
	}
      }
      catch(...) {
	/* we do have our own valid plane-equations, maybe
	 * not so good ones, but nevertheless
	 */
//	*exclusive = false;
//	break;
      }

      /* copy plane-equations out (either way) */
      *nPlanes = planeEquations.getSize();
      if (*nPlanes > nTris)
	*nPlanes = 0;
      else
#if 1
	planeEquations.copy(
	  reinterpret_cast<hkVector4 *>(planes),
	  planeEquations.begin(),
	  planeEquations.getSize()
	);
#else
	memcpy(planes, planeEquations.begin(), *nPlanes * sizeof(hkVector4));
#endif

      *exclusive = false;
      if (!inh && !ouh && !uns && *nPlanes)
	*exclusive = true;
    } while (0);

    HavocThreadExit(tm);
}

/*! Return mass and inertia matrix for a sphere of given radius and
*	density.
*/
void CalcMassPropertiesSphere(float radius, float density, bool solid,
			      float *mass, float *volume, hvkPoint3 *center,
			      hvkMatrix43 *inertia) {

    hkThreadMemory *tm = HavocThreadInit("Calculate mass properties of a sphere");

    /* encapsulate constructors/destructors, as the release
     * of the memory-manager must happen after all destructors
     * have been called
     */ {
      bool scaleDensity = (*mass == 0.0f);
      hkpMassProperties massProperties;
      hkReal hkRadius = radius;
      hkReal hkMass = scaleDensity ? 1.0f : *mass;

      try {
	_set_se_translator(catch_func);

	if (solid)
	  hkpInertiaTensorComputer::computeSphereVolumeMassProperties(hkRadius, hkMass, massProperties);
	else
	  hkpInertiaTensorComputer::computeSphereSurfaceMassProperties(hkRadius, hkMass, FLT_EPSILON, massProperties);
      }
      catch(...) {
      }

      if (scaleDensity)
	massProperties.scaleToDensity(density);

      *mass = massProperties.m_mass;
      *volume = massProperties.m_volume;

      center->Set(
	massProperties.m_centerOfMass(0),
	massProperties.m_centerOfMass(1),
	massProperties.m_centerOfMass(2)
      );

      memcpy(inertia, &massProperties.m_inertiaTensor, sizeof(*inertia));
    }

    HavocThreadExit(tm);
}

/*! Return mass and inertia matrix for a box of given size and
*   density.
*/
void CalcMassPropertiesBox(const hvkPoint3 *size, float density, bool solid,
			   float *mass, float *volume, hvkPoint3 *center,
			   hvkMatrix43 *inertia) {

    hkThreadMemory *tm = HavocThreadInit("Calculate mass properties of a box");
  
    /* encapsulate constructors/destructors, as the release
     * of the memory-manager must happen after all destructors
     * have been called
     */ {
      bool scaleDensity = (*mass == 0.0f);
      hkpMassProperties massProperties;
      hkVector4 boxSize(size->x, size->y, size->z);
      hkReal hkMass = scaleDensity ? 1.0f : *mass;

      try {
	_set_se_translator(catch_func);

	if (solid)
	  hkpInertiaTensorComputer::computeBoxVolumeMassProperties(boxSize, hkMass, massProperties);
	else
	  hkpInertiaTensorComputer::computeBoxSurfaceMassProperties(boxSize, hkMass, FLT_EPSILON, massProperties);
      }
      catch(...) {
      }

      if (scaleDensity)
	massProperties.scaleToDensity(density);

      *mass = massProperties.m_mass;
      *volume = massProperties.m_volume;

      center->Set(
	massProperties.m_centerOfMass(0),
	massProperties.m_centerOfMass(1),
	massProperties.m_centerOfMass(2)
      );

      memcpy(inertia, &massProperties.m_inertiaTensor, sizeof(*inertia));
    }

    HavocThreadExit(tm);
}

/*! Return mass and inertia matrix for a cylinder of given radius,
*   height and density.
*/
void CalcMassPropertiesCylinder(const hvkPoint3 *startAxis, const hvkPoint3 *endAxis,
				float radius, float density, bool solid,
				float *mass, float *volume, hvkPoint3 *center,
				hvkMatrix43 *inertia) {

    hkThreadMemory *tm = HavocThreadInit("Calculate mass properties of a cylinder");
  
    /* encapsulate constructors/destructors, as the release
     * of the memory-manager must happen after all destructors
     * have been called
     */ {
      bool scaleDensity = (*mass == 0.0f);
      hkpMassProperties massProperties;
      hkReal hkRadius = radius;
      hkReal hkMass = scaleDensity ? 1.0f : *mass;
      hkVector4 hkStart(startAxis->x, startAxis->y, startAxis->z),
		hkEnd(endAxis->x, endAxis->y, endAxis->z);

      try {
	_set_se_translator(catch_func);

	if (solid)
	  hkpInertiaTensorComputer::computeCylinderVolumeMassProperties(hkStart, hkEnd, hkRadius, hkMass, massProperties);
	else
	  hkpInertiaTensorComputer::computeCylinderVolumeMassProperties(hkStart, hkEnd, hkMass, FLT_EPSILON, massProperties);
      }
      catch(...) {
      }

      if (scaleDensity)
	massProperties.scaleToDensity(density);

      *mass = massProperties.m_mass;
      *volume = massProperties.m_volume;

      center->Set(
	massProperties.m_centerOfMass(0),
	massProperties.m_centerOfMass(1),
	massProperties.m_centerOfMass(2)
      );

      memcpy(inertia, &massProperties.m_inertiaTensor, sizeof(*inertia));
    }

    HavocThreadExit(tm);
}

/*! Return mass and inertia matrix for a capsule of given radius,
*	height and density.
*/
void CalcMassPropertiesCapsule(const hvkPoint3 *startAxis, const hvkPoint3 *endAxis,
			       float radius, float density, bool solid,
			       float *mass, float *volume, hvkPoint3 *center,
			       hvkMatrix43 *inertia) {

    hkThreadMemory *tm = HavocThreadInit("Calculate mass properties of a capsule");
  
    /* encapsulate constructors/destructors, as the release
     * of the memory-manager must happen after all destructors
     * have been called
     */ {
      bool scaleDensity = (*mass == 0.0f);
      hkpMassProperties massProperties;
      hkReal hkRadius = radius;
      hkReal hkMass = scaleDensity ? 1.0f : *mass;
      hkVector4 hkStart(startAxis->x, startAxis->y, startAxis->z),
		hkEnd(endAxis->x, endAxis->y, endAxis->z);

      try {
	_set_se_translator(catch_func);

	if (solid)
	  hkpInertiaTensorComputer::computeCapsuleVolumeMassProperties(hkStart, hkEnd, hkRadius, hkMass, massProperties);
	else
	  hkpInertiaTensorComputer::computeCapsuleVolumeMassProperties(hkStart, hkEnd, hkRadius, hkMass, massProperties);
      }
      catch(...) {
      }

      if (scaleDensity)
	massProperties.scaleToDensity(density);

      *mass = massProperties.m_mass;
      *volume = massProperties.m_volume;

      center->Set(
	massProperties.m_centerOfMass(0),
	massProperties.m_centerOfMass(1),
	massProperties.m_centerOfMass(2)
      );

      memcpy(inertia, &massProperties.m_inertiaTensor, sizeof(*inertia));
    }

    HavocThreadExit(tm);
}

/*! Return mass and inertia matrix for a capsule of given radius,
*	height and density.
*/
void CalcMassPropertiesPolyhedron(int nVerts, const hvkPoint3 *verts,
				  int nTris, const hvkTriangle *tris,
				  float density, bool solid,
				  float *mass, float *volume, hvkPoint3 *center,
				  hvkMatrix43 *inertia) {

    hkThreadMemory *tm = HavocThreadInit("Calculate mass properties of a polyhedron");

    /* encapsulate constructors/destructors, as the release
     * of the memory-manager must happen after all destructors
     * have been called
     */ {
      //hkGeometry geom;
      //for (int i=0;i<nVerts; ++i)
      //	geom.m_vertices.pushBack( hkVector4(verts[i].x, verts[i].y, verts[i].z) );
      //for (int i=0;i<nTris; ++i) {
      //	hkGeometry::hvkTriangle t; t.set(tris[i].a, tris[i].b, tris[i].c);
      //	geom.m_triangles.pushBack( t );
      //}

      hkStridedVertices vertsIn;
      vertsIn.m_vertices = reinterpret_cast<const float *>(&verts[0]);
      vertsIn.m_numVertices = nVerts;
      vertsIn.m_striding = sizeof(hvkPoint3);
      hkGeometry* geom = new hkGeometry();
      hkInplaceArrayAligned16<hkVector4,32> transformedPlanes;
      hkGeometryUtility::createConvexGeometry(vertsIn, *geom, transformedPlanes);

      bool scaleDensity = (*mass == 0.0f);
      hkpMassProperties massProperties;
      hkReal hkMass = scaleDensity ? 1.0f : *mass;

      try {
	_set_se_translator(catch_func);

	if (solid)
	  hkpInertiaTensorComputer::computeGeometryVolumeMassPropertiesChecked(geom, hkMass, massProperties);
	else
	  hkpInertiaTensorComputer::computeGeometrySurfaceMassProperties      (geom, FLT_EPSILON, true, hkMass, massProperties);
      }
      catch(...) {
      }

      delete geom;

      if (scaleDensity)
	massProperties.scaleToDensity(density);

      *mass = massProperties.m_mass;
      *volume = massProperties.m_volume;

      center->Set(
	massProperties.m_centerOfMass(0),
	massProperties.m_centerOfMass(1),
	massProperties.m_centerOfMass(2)
      );

      memcpy(inertia, &massProperties.m_inertiaTensor, sizeof(*inertia));
    }

    HavocThreadExit(tm);
}

void CombineMassProperties(int nItems,
			   const float *masses, const float *volumes, const hvkPoint3 *centers,
			   const hvkMatrix43 *inertias, const hvkMatrix44 *transforms,
			   float *mass, float *volume, hvkPoint3 *center,
			   hvkMatrix43 *inertia) {

    hkThreadMemory *tm = HavocThreadInit("Combine mass properties");
  
    /* encapsulate constructors/destructors, as the release
     * of the memory-manager must happen after all destructors
     * have been called
     */ {
      bool scaleDensity = (*mass == 0.0f);
      hkpMassProperties massProperties;
      hkReal hkMass = scaleDensity ? 1.0f : *mass;

      hkArray<hkpMassElement> elements;

      for (int i = 0; i < nItems; ++i){
	hkpMassElement elem;

	elem.m_transform.set4x4ColumnMajor(reinterpret_cast<const hkReal *>(transforms + i));
	elem.m_properties.m_mass = masses[i];
	elem.m_properties.m_volume = volumes[i];
	elem.m_properties.m_centerOfMass.set(centers[i].x, centers[i].y, centers[i].z);

	memcpy(&elem.m_properties.m_inertiaTensor, inertias + i, sizeof(elem.m_properties.m_inertiaTensor));

	elements.pushBack(elem);
      }

      try {
	_set_se_translator(catch_func);

	hkpInertiaTensorComputer::combineMassProperties(elements, massProperties);
      }
      catch(...) {
      }

      *mass = massProperties.m_mass;
      *volume = massProperties.m_volume;

      center->Set(
	massProperties.m_centerOfMass(0),
	massProperties.m_centerOfMass(1),
	massProperties.m_centerOfMass(2)
      );

      memcpy(inertia, &massProperties.m_inertiaTensor, sizeof(*inertia));
    }

    HavocThreadExit(tm);
}

/* ---------------------------------------------------- */

#undef	BRUTEFORCE_MATCHHAVOK
#ifdef	BRUTEFORCE_MATCHHAVOK
unsigned short *global_owcode;
int global_owlen;
unsigned char *global_ocode;
int global_olen;

int sspbits_loop;
float sradius_loop;
float bradius_loop;
float aradius_loop;
float iradius_loop;
hkUint8 material_loop;

#define CONVEXSHAPEBITS		sspbits_loop
#define CONVEXSHAPERADIUS	sradius_loop  //0.1945f * 0.95f
#define	BASEFACTOR		bradius_loop  //0.1945f * 1.35f
#define	AATRFACTOR		aradius_loop  //0.1945f * 1.35f
#define	INTLFACTOR		iradius_loop  //0.1945f
#define	MATERIALIDX		material_loop
#else
#define CONVEXSHAPEBITS		8
#define CONVEXSHAPERADIUS	0.1000f
#define	BASEFACTOR		0.3000f
#define	AATRFACTOR		0.3000f
#define	INTLFACTOR		0.5000f
#define	MATERIALIDX		0 // material
#endif

static hkpSimpleMeshShape *ConstructHKMesh(hvkByte material,
					   int nVerts, const hvkPoint3 *verts,
					   int nTris, const hvkTriangle *tris,
					   int vertOffset = 0) {

	hkpSimpleMeshShape *storageMeshShape = new hkpSimpleMeshShape(CONVEXSHAPERADIUS);
	hkArray<hkVector4> &vertices = storageMeshShape->m_vertices;
	hkArray<hkpSimpleMeshShape::Triangle> &triangles = storageMeshShape->m_triangles;
	hkArray<hkUint8> &materials = storageMeshShape->m_materialIndices;

	triangles.setSize(0);
	materials.setSize(0);
	for (int i = 0; i < nTris; ++i) {
		hvkTriangle const &tri = tris[i];
		hkpSimpleMeshShape::Triangle hktri;

		hktri.m_a = tri[0] - vertOffset;
		hktri.m_b = tri[1] - vertOffset;
		hktri.m_c = tri[2] - vertOffset;

		assert((hktri.m_a >= 0) && (hktri.m_a < nVerts));
		assert((hktri.m_b >= 0) && (hktri.m_b < nVerts));
		assert((hktri.m_c >= 0) && (hktri.m_c < nVerts));

		/* the ID of the material is assumed to be the index
		 * into the material-table
		 */
		triangles.pushBack(hktri);
		materials.pushBack(MATERIALIDX);
	}

	vertices.setSize(0);
	for (int i = 0;i < nVerts; ++i) {
		hvkPoint3 const &vert = verts[i];

		vertices.pushBack(hkVector4(vert.x, vert.y, vert.z));
	}

	return storageMeshShape;
}

#define hkpUtilizedMeshShape	hkpExtendedMeshShape
#define UtilizedSubpart		TrianglesSubpart
#define addSubpart_		addTrianglesSubpart
#define m_numTriangles_		m_numTriangleShapes
//#define hkpUtilizedMeshShape	hkpMeshShape
//#define UtilizedSubpart		Subpart
//#define addSubpart_		addSubpart
//#define m_numTriangles_		m_numTriangles

static hkpUtilizedMeshShape *ConstructHKMesh(int nShapes, const int *shapes,
					     int nMaterials, const hvkByte *mats,
					     int nVerts, const hvkPoint3 *verts,
					     int nTris, const hvkTriangle *tris,
					     int vertOffset = 0) {

	hkpUtilizedMeshShape *storageMeshShape = new hkpUtilizedMeshShape(CONVEXSHAPERADIUS, CONVEXSHAPEBITS);

#if 0
	/* the complete material table, we assume Oblivions materials
	 * aka "HavokMaterial" is really an index, so we create an empty
	 * table which has the maximum amount of materials possible by index
	 */
	vector<hkpMeshMaterial> materials;
	for (int m = 0; m < nMaterials; ++m)
	  materials.resize(mats[m] + 1);
#endif

	/* for reference look at:
	 * Demo\Demos\Physics\Api\Collide\Shapes\MeshMaterial\MeshMaterialDemo.cpp
	 */
	int startv = 0;
	for (int s = 0; s < nShapes; ++s) {
	  int endv = startv + (shapes[s] * 1);

	  hkpUtilizedMeshShape::UtilizedSubpart part;

	  /* it will not copy the data out, need to allocate things */
#ifdef	LOCAL_SUBSHAPES
	  part.m_numVertices = shapes[s];
	  part.m_vertexBase = &verts[startv].x;
#else
	  part.m_numVertices = nVerts;
	  part.m_vertexBase = &verts[0].x;
#endif
	  part.m_vertexStriding = sizeof(hvkPoint3);

	  /* assume triangles are unordered, we pick them out one by one
	  vector<hvkTriangle> triangles;
	  for (int t = 0; t < nTris; ++t) {
	    if ((tris[t].a >= startv) && (tris[t].a < endv))
	      triangles.push_back(tris[t]);
	  } */

	  /* assume triangles are ordered, now triangles have local indices
	   * and we can not use them to distinguish by shape, but the
	   * material-index is the shape-index, so we just look at all
	   * triangles with the correct material-index
	   */
	  int nt = 0, nb = 0;
	  for (int t = 0; t < nTris; ++t) {
	    if (mats[t] == s) {
	      if (!nt++)
		nb = t;

#ifdef	LOCAL_SUBSHAPES
	      assert((tris[t].a >= 0) && (tris[t].a < (endv - startv)));
	      assert((tris[t].b >= 0) && (tris[t].b < (endv - startv)));
	      assert((tris[t].c >= 0) && (tris[t].c < (endv - startv)));
#endif
	    }
	  }

	  /* it will not copy the data out, need to allocate things */
//	  part.m_numTriangleShapes = (int)triangles.size();
	  part.m_indexBase = &tris[nb];
	  part.m_numTriangles_ = nt;
	  // using "0" is the only way to get the triangle-id into the sub-part
	  part.m_triangleOffset = -1/*nb*/;
	  part.m_indexStriding = sizeof(hvkTriangle);
	  part.m_stridingType = hkpUtilizedMeshShape::INDICES_INT16;

	  /* the material-indices are only for querying and won't end
	   * up in the MOPP-code: getShapeKey() returns shape-id
	   * which is used to lookup the material directly
	   */
	  part.m_materialIndexBase = &mats[nb];
	  part.m_materialIndexStriding = sizeof(hvkByte);
	  part.m_materialIndexStridingType = hkpUtilizedMeshShape::MATERIAL_INDICES_INT8;

#if 0
	  /* this might not be necessary ... for the MOPP-code the
	   * contents of the material-database are irrelevant, though
	   * the indices into the material-database are what matters
	   */
	  part.m_numMaterials = (hkUint16)materials.size();
	  part.m_materialBase = &materials[0];
	  part.m_materialStriding = sizeof(hkpMeshMaterial);
//	  part.m_materialClass = ???;
#endif

	  storageMeshShape->addSubpart_(part);

	  startv = endv;
	}

	return storageMeshShape;
}

/* ---------------------------------------------------- */

/* process-global storage! */
int moppTris = 0;
int moppSize = 0;
hvkByte *moppBuffer = NULL;
hvkShort *moppWelding = NULL;
float moppScale;
float moppOrigin[3];

static int InternalGenerateCode(hvkByte material,
				int nVerts, const hvkPoint3 *verts,
				int nTris, const hvkTriangle *tris) {
#ifdef	BRUTEFORCE_MATCHHAVOK
/*WELDING_TYPE_ANTICLOCKWISE = 0,
  WELDING_TYPE_CLOCKWISE = 4,
  WELDING_TYPE_TWO_SIDED = 5,
  WELDING_TYPE_NONE = 6*/

  vector<float> best_sradius_loop;
  vector<float> best_bradius_loop;
  vector<float> best_aradius_loop;
  vector<float> best_iradius_loop;
  vector<int>   best_material_loop;

  int best_diff = global_olen;

  sradius_loop = 0.1945f;
  bradius_loop = 0.1945f;
  aradius_loop = 0.1945f;
  iradius_loop = 0.5f;

  for (sradius_loop = 0.095f; sradius_loop <= 0.105f; sradius_loop += 0.00005f)
  for (bradius_loop = 0.01f; bradius_loop <= 0.4f; bradius_loop += 0.001f)
//for (aradius_loop = (bradius_loop - 0.01); aradius_loop <= (bradius_loop + 0.01); aradius_loop += 0.001f)
//for (iradius_loop = 0.40f; iradius_loop <= 0.60f; iradius_loop += 0.05f)
  for (material_loop = 0; material_loop <= material; material_loop += max(1, material)) {
      aradius_loop = bradius_loop;

    // edgeNormalSnapDeltaAngle & triangleNormalSnapDeltaAngle -> hkpWorld
//  hkpWeldingUtility::initWeldingTable(15.1f, 15.1f);
#endif

	int retCode = 0;
        hkpMoppCode* k_phkpMoppCode = NULL;

//	hkpShapeCollection *list = ConstructHKMesh(nVerts, verts, nTris, tris);
//	hkpExtendedMeshShape* list = ConstructHKMesh(1, &nVerts, 1, &material, nVerts, verts, nTris, tris);
	hkpSimpleMeshShape* list = ConstructHKMesh(material, nVerts, verts, nTris, tris);
	hkpMoppCompilerInput mfr;

	mfr.setAbsoluteFitToleranceOfTriangles(BASEFACTOR * 1.0f);
	mfr.setAbsoluteFitToleranceOfAxisAlignedTriangles(hkVector4(AATRFACTOR * 1.0f, AATRFACTOR * 1.0f, AATRFACTOR * 1.0f));
//	mfr.setAbsoluteFitToleranceOfInternalNodes(INTLFACTOR * 3.0f);
	mfr.setRelativeFitToleranceOfInternalNodes(INTLFACTOR * 1.0f);
	mfr.m_useShapeKeys = false;		// much better
	mfr.m_enablePrimitiveSplitting = false;
	mfr.m_enableChunkSubdivision = false;	// confirmed
	mfr.m_cachePrimitiveExtents = true;	// just faster
//	m_enablePrimitiveSplitting
//	m_enableChunkSubdivision
//	m_enableInterleavedBuilding
//	m_cachePrimitiveExtents

	moppTris = 0;
	moppSize = 0;

	k_phkpMoppCode = hkpMoppUtility::buildCode(list, mfr);
	if (k_phkpMoppCode) {
	  hkpMoppBvTreeShape bvtree(list, k_phkpMoppCode);

	  list->computeWeldingInfo(&bvtree, hkpWeldingUtility::WELDING_TYPE_ANTICLOCKWISE);

	  moppTris      = nTris;
	  moppSize      = k_phkpMoppCode->m_data.getSize();
	  moppScale     = k_phkpMoppCode->m_info.getScale();
	  moppOrigin[0] = k_phkpMoppCode->m_info.m_offset(0);
	  moppOrigin[1] = k_phkpMoppCode->m_info.m_offset(1);
	  moppOrigin[2] = k_phkpMoppCode->m_info.m_offset(2);
	  moppBuffer    = (hvkByte *)realloc(moppBuffer, moppSize);
	  moppWelding   = (hvkShort *)realloc(moppWelding, nTris * sizeof(hvkShort));

	  /* MOPP Code */
	  memcpy(moppBuffer, &k_phkpMoppCode->m_data[0], moppSize);

	  /* Welding info */
	  hkArray<hkpSimpleMeshShape::Triangle> &triangles = list->m_triangles;
	  for (int i = 0;i < nTris; ++i)
	    moppWelding[i] = triangles[i].m_weldingInfo;

	  k_phkpMoppCode->removeReference();
	  k_phkpMoppCode = NULL;
	}

	list->removeReference();

#ifdef	BRUTEFORCE_MATCHHAVOK
	fprintf(stderr, "%f %f %f %f %d: %d %d\r", sradius_loop, bradius_loop, aradius_loop, iradius_loop, material_loop, moppSize, moppTris);

	/*if (moppSize == global_olen)*/ {
	  int diffs = 0;
	  for (int n = 0; n < moppSize; n++) {
	    if (moppBuffer[n] != global_ocode[n])
	      diffs++;
	  }

	  if (best_diff == diffs) {
	    best_sradius_loop.push_back(sradius_loop);
	    best_bradius_loop.push_back(bradius_loop);
	    best_aradius_loop.push_back(aradius_loop);
	    best_iradius_loop.push_back(iradius_loop);
	    best_material_loop.push_back(material_loop);
	    best_diff = diffs;
	  }
	  else if (best_diff > diffs) {
	    fprintf(stderr, "new best %d:\n", best_diff);

	    best_sradius_loop.clear();
	    best_bradius_loop.clear();
	    best_aradius_loop.clear();
	    best_iradius_loop.clear();
	    best_material_loop.clear();
	    best_sradius_loop.push_back(sradius_loop);
	    best_bradius_loop.push_back(bradius_loop);
	    best_aradius_loop.push_back(aradius_loop);
	    best_iradius_loop.push_back(iradius_loop);
	    best_material_loop.push_back(material_loop);
	    best_diff = diffs;

	    fprintf(stdout, "MOPP-Code %d:\n", moppSize);
	    for (int n = 0; n < moppSize; n++) {
	      fprintf(stdout, " %02x ", moppBuffer[n]);
	      if ((n & 15) == 15)
		fprintf(stdout, "\n");
	    }
	    fprintf(stdout, "\n");
	    fprintf(stdout, "Diffs: %d\n", diffs);

	    fprintf(stdout, "Welding %d:\n", moppTris);
	    for (int n = 0; n < moppTris; n++) {
	      fprintf(stdout, " %04x ", moppWelding[n]);
	      if ((n & 7) == 7)
		fprintf(stdout, "\n");
	    }
	    fprintf(stdout, "\n");
	  }
	}
  }

  int candidates = (int)best_sradius_loop.size();
  fprintf(stdout, "Best diff: %d different bytes\n", best_diff);
  fprintf(stdout, "Candidates: %d combinations\n", candidates);
  for (int n = 0; n < candidates; n++)
    fprintf(stdout, "%f %f %f %f %d\n", best_sradius_loop[n], best_bradius_loop[n], best_aradius_loop[n], best_iradius_loop[n], best_material_loop[n]);
  fprintf(stdout, "\n");
#endif

	return moppSize;
}

static int InternalGenerateCodeWithSubshapes(int nShapes, const int *subShapes,
					     int nMaterials, const hvkByte *materialIDs,
					     int nVerts, const hvkPoint3 *verts,
					     int nTris, const hvkTriangle *tris) {
#ifdef	BRUTEFORCE_MATCHHAVOK
/* Original:

  File:
    "Oblivion - Meshes\meshes\architecture\anvil\anvilstreetlamp01.nif"
  MOPP-Code 436:
    28  00  ff  27  00  29  25  42  40  00  12  00  00  27  08  21
    26  10  5b  0b  01  00  00  08  23  2c  17  00  91  00  f3  01
			^^  ^^  ^^ triangle offset 24bit
		    ^^  subpart num 8bit
    00  00  00  26  00  57  12  57  53  3f  17  2d  27  1c  18  81
    7e  0c  26  00  0b  13  30  27  01  31  27  07  51  30  27  00
    0b  16  81  7a  04  26  00  4f  36  37  15  31  2a  04  27  46
    51  33  26  08  57  18  84  81  04  27  46  51  32  26  4c  57
    14  81  77  04  27  00  4a  34  35  27  07  4a  26  08  4f  0b
    01  00  00  26  17  2c  27  1c  18  82  7e  0c  26  08  23  14
    5b  56  04  27  07  33  31  30  27  07  21  15  53  50  01  36
    26  20  4f  37  18  84  80  0c  27  30  4a  16  5b  58  04  26
    08  37  33  32  26  33  4f  13  52  4d  01  34  27  1d  4a  35
    27  0e  1a  10  1c  17  3b  09  16  11  1a  16  2f  18  81  7f
    14  26  10  1a  10  13  11  06  11  1a  10  01  30  31  28  fc
    ff  27  10  19  40  27  0e  19  11  12  10  09  12  ff  40  01
    36  26  10  1a  37  28  fc  ff  26  11  1a  41  10  1c  11  01
    32  33  12  ff  b8  0c  26  18  1c  17  19  14  04  27  0e  19
    4b  4a  03  17  11  b8  28  03  1a  27  04  33  17  59  17  01
    34  35  01  17  08  b4  16  8e  89  36  28  08  6d  27  13  1f
    11  1f  1c  26  16  8e  7a  11  15  79  35  04  27  13  16  36
    28  69  6d  10  87  00  01  30  31  1a  66  63  04  27  13  16
    37  26  26  87  10  87  26  01  32  33  16  8e  7a  01  39  38
    28  00  4d  26  4c  80  18  a8  a3  2d  11  32  1d  23  16  a7
    a4  1b  01  4c  01  00  26  00  42  10  2c  25  09  17  33  25
    01  3b  27  23  61  3a  28  93  9a  27  23  3e  42  28  00  03
    45  10  80  5e  01  3d  3c  10  80  69  20  14  82  7f  18  01
    4c  01  00  27  01  3e  11  2a  23  06  10  42  00  01  40  41
    28  93  9a  26  25  42  43  28  00  03  44  17  46  3f  04  27
    01  20  3e  3f
  Welding 48:
    0128  bd6e  3f7c  bf15  be59  bf79  bd74  be66
    bf40  3f65  3d6e  3f7c  bf15  3e59  3f48  bd02
    be4c  bf40  3ef3  3e16  3f7c  bdae  3f09  3f48
    bd23  3d80  bf79  bde9  bf70  3f7c  3cf2  3f1c
    3f48  3ddd  3d27  bf7f  bf00  bf1c  be9a  3e05
    3de0  3f48  3ef3  00d9  bf7f  0000  3f7f  a3f6
*/

/* Best match:

#define CONVEXSHAPEBITS		8
#define CONVEXSHAPERADIUS	0.0820f
#define	BASEFACTOR		0.2000f
#define	AATRFACTOR		0.2000f
#define	INTLFACTOR		0.2000f

  File:
    "Oblivion - Meshes\meshes\architecture\anvil\anvilstreetlamp01.nif"
  MOPP-Code 436:
   28  00  ff  27  00  29  25  42  40  00  12  00  00  27  08  21
   26  10  5b  0b  01  00  00  00  23  2c  17  00  91  00  f0  01
		       ^^  ^^  ^^ triangle offset 24bit
		   ^^  subpart num 8bit
   00  00  00  26  00  57  12  57  54  3f  17  2c  27  1c  18  81
   7e  0c  26  00  0b  13  30  27  01  31  27  07  51  30  27  00
   0a  16  81  7a  04  26  00  4f  36  37  15  30  2a  04  27  46
   51  33  26  08  57  18  84  81  04  27  46  51  32  26  4c  57
   14  81  78  04  27  00  49  34  35  27  07  49  26  08  4f  0b
   01  00  00  1e  17  2c  27  1c  18  82  7f  0c  26  08  23  14
   5b  56  04  27  07  33  31  30  27  07  21  15  53  50  01  36
   26  20  4f  37  18  83  80  0c  27  30  49  16  5b  58  04  26
   08  36  33  32  26  33  4f  13  52  4d  01  34  27  1e  49  35
   27  0f  1a  10  1b  17  38  09  16  18  81  7f  1e  10  13  10
   06  11  1a  10  01  30  31  27  10  1a  11  18  16  07  28  fc
   ff  26  11  1a  40  10  1b  11  01  32  33  27  0f  18  11  12
   10  06  10  1a  10  01  37  36  28  fc  ff  26  11  1a  41  12
   ff  b8  0c  26  18  1b  17  19  14  04  27  0f  18  4b  4a  03
   17  11  b8  28  05  19  27  04  32  13  1f  0b  01  34  35  01
   17  08  b4  16  8e  89  39  28  09  6d  27  13  1f  11  1f  1c
   29  16  8e  7a  11  15  79  35  04  27  13  16  36  28  69  6d
   10  87  00  01  30  31  1a  66  63  04  27  13  16  37  28  0c
   6d  26  26  87  10  87  26  01  32  33  16  8e  7a  01  39  38
   28  00  4d  26  4c  80  18  a8  a4  2d  11  31  1d  23  16  a7
   a4  1b  01  4c  01  00  26  00  41  10  2b  26  09  17  32  25
   01  3b  27  24  60  3a  28  94  9a  27  24  3d  42  28  00  03
   45  10  80  5f  01  3d  3c  10  80  6a  20  14  81  7f  18  01
   4c  01  00  27  01  3d  11  29  24  06  10  41  00  01  40  41
   28  94  9a  26  26  41  43  28  00  03  44  17  46  3f  04  27
   01  20  3e  3f
  Welding 48:
   41f6  010f  41f6  010f  41f6  010f  41f6  010f
   59fa  200f  59f2  208f  59e0  218f  59e0  210f
   59f6  010f  55f6  1d0f  55f6  1d0f  55f6  1d0f
   55f6  1d0f  55f5  252f  5df7  1cef  35f6  210f
   35f6  210f  35f6  210f  35f6  210f  55f0  262f
   55f0  262f  55f0  262f  55f0  262f  59f6  210f
*/

/*WELDING_TYPE_ANTICLOCKWISE = 0,
  WELDING_TYPE_CLOCKWISE = 4,
  WELDING_TYPE_TWO_SIDED = 5,
  WELDING_TYPE_NONE = 6*/

  vector<int>   best_sspbits_loop;
  vector<float> best_sradius_loop;
  vector<float> best_bradius_loop;
  vector<float> best_aradius_loop;
  vector<float> best_iradius_loop;
  vector<int>   best_material_loop;

  int best_diff = global_olen;
  int best_num = 0;

  sspbits_loop = 8;
  sradius_loop = 0.1945f;
  bradius_loop = 0.1945f;
  aradius_loop = 0.1945f;
  iradius_loop = 0.5f;

  int lg2 = 3;
  while (nShapes > (1 << lg2))
    lg2++;

//for (sspbits_loop = 4; sspbits_loop <= 16; sspbits_loop *= 2)
  for (sradius_loop = 0.095f; sradius_loop <= 0.105f; sradius_loop += 0.00005f)
  for (bradius_loop = 0.01f; bradius_loop <= 0.4f; bradius_loop += 0.001f)
//for (aradius_loop = (bradius_loop - 0.01); aradius_loop <= (bradius_loop + 0.01); aradius_loop += 0.001f)
//for (iradius_loop = 0.10f; iradius_loop <= 1.00f; iradius_loop += 0.10f)
/*for (material_loop = 0; material_loop <= material; material_loop += max(1, material))*/ {
      aradius_loop = bradius_loop;

    // edgeNormalSnapDeltaAngle & triangleNormalSnapDeltaAngle -> hkpWorld
//  hkpWeldingUtility::initWeldingTable(15.1f, 15.1f);
#endif

	int retCode = 0;
	hkpMoppCode* k_phkpMoppCode = NULL;

	vector<hvkByte> matls(nTris);
	vector<hvkTriangle> trils(nTris);
	hvkByte *lmats = &matls[0];
	hvkTriangle *ltris = &trils[0];
	int startv = 0;
	for (int s = 0; s < nShapes; ++s) {
	  int endv = startv + (subShapes[s] * 1);

	  /* assign material to each triangle and make
	   * all "global" indices "shape-local" indices
	   */
	  for (int t = 0; t < nTris; ++t) {
	    if ((tris[t].a >= startv) && (tris[t].a < endv)) {
	      assert((tris[t].a >= startv) && (tris[t].a < endv));
	      assert((tris[t].b >= startv) && (tris[t].b < endv));
	      assert((tris[t].c >= startv) && (tris[t].c < endv));

	      /* material-index is completely irrelevant
	       * Oblivion does SubShapes[GetSubpart()].material
	       */
	      matls[t] = s;

	      /* we can do them local or global, isn't really any different */
#ifdef	LOCAL_SUBSHAPES
	      trils[t].a = tris[t].a   - startv  ;
	      trils[t].b = tris[t].b   - startv  ;
	      trils[t].c = tris[t].c   - startv  ;
#else
	      trils[t].a = tris[t].a /*- startv*/;
	      trils[t].b = tris[t].b /*- startv*/;
	      trils[t].c = tris[t].c /*- startv*/;
#endif
	    }
	  }

	  startv = endv;
	}

	/* Oblivion uses hkpMeshShape, which is deprecated at least in Havok 5.5
	 * (and up) and has been replaced (with identical functionality) by
	 * hkpExtendedMeshShape
	 */
//	hkpMeshShape* list = ConstructHKMesh(nShapes, subShapes, nMaterials, lmats, nVerts, verts, nTris, ltris);
	hkpUtilizedMeshShape* list = ConstructHKMesh(nShapes, subShapes, nMaterials, lmats, nVerts, verts, nTris, ltris);

	hkpMoppCompilerInput mfr;

	mfr.setAbsoluteFitToleranceOfTriangles(BASEFACTOR * 1.0f);
	mfr.setAbsoluteFitToleranceOfAxisAlignedTriangles(hkVector4(AATRFACTOR * 1.0f, AATRFACTOR * 1.0f, AATRFACTOR * 1.0f));
//	mfr.setAbsoluteFitToleranceOfInternalNodes(INTLFACTOR * 3.0f);
	mfr.setRelativeFitToleranceOfInternalNodes(INTLFACTOR * 1.0f);
	mfr.m_useShapeKeys = false;		// much better
	mfr.m_enablePrimitiveSplitting = false;
	mfr.m_enableChunkSubdivision = false;	// confirmed
	mfr.m_cachePrimitiveExtents = true;	// just faster
//	m_enablePrimitiveSplitting
//	m_enableChunkSubdivision
//	m_enableInterleavedBuilding
//	m_cachePrimitiveExtents

	moppTris = 0;
	moppSize = 0;

	k_phkpMoppCode = hkpMoppUtility::buildCode(list, mfr);
	if (k_phkpMoppCode) {
	  hkpMoppBvTreeShape bvtree(list, k_phkpMoppCode);

	  list->computeWeldingInfo(&bvtree, hkpWeldingUtility::WELDING_TYPE_ANTICLOCKWISE);

	  moppTris      = nTris;
	  moppSize      = k_phkpMoppCode->m_data.getSize();
	  moppScale     = k_phkpMoppCode->m_info.getScale();
	  moppOrigin[0] = k_phkpMoppCode->m_info.m_offset(0);
	  moppOrigin[1] = k_phkpMoppCode->m_info.m_offset(1);
	  moppOrigin[2] = k_phkpMoppCode->m_info.m_offset(2);
	  moppBuffer    = (hvkByte *)realloc(moppBuffer, moppSize);
	  moppWelding   = (hvkShort *)realloc(moppWelding, nTris * sizeof(hvkShort));

	  memcpy(moppBuffer, &k_phkpMoppCode->m_data[0], moppSize);

	  /* Welding info */
	  hkArray<hkUint16> &triangles = list->m_weldingInfo;
	  for (int i = 0;i < triangles.getSize(); ++i)
	    moppWelding[i] = triangles[i];

	  k_phkpMoppCode->removeReference();
	  k_phkpMoppCode = NULL;
	}

	list->removeReference();

#ifdef	BRUTEFORCE_MATCHHAVOK
	fprintf(stderr, "%d: %f %f %f %f %d: %d %d\r", sspbits_loop, sradius_loop, bradius_loop, aradius_loop, iradius_loop, material_loop, moppSize, moppTris);

	int num = 0;
	for (num = 0; num < moppSize; num++) {
	  if (moppBuffer[num] != global_ocode[num])
	    break;
	}

	if (best_num < num) {
	  best_num = num;
	}

	/*if (moppSize == global_olen)*/ {
	  int diffs = 0;
	  for (int n = 0; n < moppSize; n++) {
	    if (moppBuffer[n] != global_ocode[n])
	      diffs++;
	  }

	  if (best_diff == diffs) {
	    best_sspbits_loop.push_back(sspbits_loop);
	    best_sradius_loop.push_back(sradius_loop);
	    best_bradius_loop.push_back(bradius_loop);
	    best_aradius_loop.push_back(aradius_loop);
	    best_iradius_loop.push_back(iradius_loop);
	    best_material_loop.push_back(material_loop);
	    best_diff = diffs;
	  }
	  else if (best_diff > diffs) {
	    fprintf(stderr, "new best %d:\n", best_diff);

	    best_sspbits_loop.clear();
	    best_sradius_loop.clear();
	    best_bradius_loop.clear();
	    best_aradius_loop.clear();
	    best_iradius_loop.clear();
	    best_material_loop.clear();
	    best_sspbits_loop.push_back(sspbits_loop);
	    best_sradius_loop.push_back(sradius_loop);
	    best_bradius_loop.push_back(bradius_loop);
	    best_aradius_loop.push_back(aradius_loop);
	    best_iradius_loop.push_back(iradius_loop);
	    best_material_loop.push_back(material_loop);
	    best_diff = diffs;

	    fprintf(stdout, "MOPP-Code %d:\n", moppSize);
	    for (int n = 0; n < moppSize; n++) {
	      fprintf(stdout, " %02x ", moppBuffer[n]);
	      if ((n & 15) == 15)
		fprintf(stdout, "\n");
	    }
	    fprintf(stdout, "\n");
	    fprintf(stdout, "Diffs: %d\n", diffs);

	    fprintf(stdout, "Welding %d:\n", moppTris);
	    for (int n = 0; n < moppTris; n++) {
	      fprintf(stdout, " %04x ", moppWelding[n]);
	      if ((n & 7) == 7)
		fprintf(stdout, "\n");
	    }
	    fprintf(stdout, "\n");
	  }
	}
  }

  int candidates = (int)best_sradius_loop.size();
  fprintf(stdout, "Best len: %d bytes\n", best_num);
  fprintf(stdout, "Best diff: %d different bytes\n", best_diff);
  fprintf(stdout, "Candidates: %d combinations\n", candidates);
  for (int n = 0; n < candidates; n++)
    fprintf(stdout, "%d: %f %f %f %f %d\n", best_sspbits_loop[n], best_sradius_loop[n], best_bradius_loop[n], best_aradius_loop[n], best_iradius_loop[n], best_material_loop[n]);
  fprintf(stdout, "\n");
#endif

	return moppSize;
}

int GenerateMoppCodeWithSubshapes(int nShapes, const int *shapeVerts,
				  int nMaterials, const hvkByte *materialIDs,
				  int nVerts, const hvkPoint3 *verts,
				  int nTris, const hvkTriangle *tris) {

	hkThreadMemory *tm = HavocThreadInit("Generate MOPP-code");
	int retcode = 0;

	try {
	  _set_se_translator(catch_func);

	  if ((nShapes <= 1) && (nMaterials <= 1))
	    retcode = InternalGenerateCode             (                                 materialIDs[0], nVerts, verts, nTris, tris);
	  else
	    retcode = InternalGenerateCodeWithSubshapes(nShapes, shapeVerts, nMaterials, materialIDs   , nVerts, verts, nTris, tris);
	}
	catch(...) {
	  retcode = -1;
	}

	HavocThreadExit(tm);

	return retcode;
}

int GenerateMoppCode(hvkByte material,
		     int nVerts, const hvkPoint3 *verts,
		     int nTris, const hvkTriangle *tris) {

	hkThreadMemory *tm = HavocThreadInit("Generate MOPP-code");
	int retcode = 0;

	try {
	  _set_se_translator(catch_func);

	  retcode = InternalGenerateCode(material, nVerts, verts, nTris, tris);
	}
	catch(...) {
	  retcode = -1;
	}

	HavocThreadExit(tm);

	return retcode;
}

int RetrieveMoppCode(int nBuffer, hvkByte *buffer) {
	if (moppBuffer == NULL)
		return 0;
	if (moppSize != nBuffer)
		return -moppSize;

	memcpy(buffer, moppBuffer, nBuffer);

	return moppSize;
}

int RetrieveMoppWelding(int nBuffer, hvkShort *buffer) {
	if (moppWelding == NULL)
		return 0;
	if (moppTris != nBuffer)
		return -moppTris;

	memcpy(buffer, moppWelding, moppTris * sizeof(hvkShort));

	return moppTris;
}

int RetrieveMoppScale(float *value) {
	if (moppBuffer == NULL)
		return 0;
	if (value == NULL)
		return 0;

	*value = moppScale;

	return 1;
}

int RetrieveMoppOrigin(float *value) {
	if (moppBuffer == NULL)
		return 0;
	if (value == NULL)
		return 0;

	value[0] = moppOrigin[0];
	value[1] = moppOrigin[1];
	value[2] = moppOrigin[2];

	return 1;
}

/* ---------------------------------------------------- */

static void HK_CALL errorReport(const char* msg, void*)
{
	//printf("%s", msg);
}

bool HavokInit() {
  // Initialize the base system including our memory system
  if (!memoryManager)
    memoryManager = new hkPoolMemory();

  hkThreadMemory *tm;
  if (!(tm = &hkThreadMemory::getInstance())) {
    tm = new hkThreadMemory(memoryManager/*, 16*/);
    hkBaseSystem::init(memoryManager, tm, errorReport);
  }

  if (memoryManager)
    memoryManager->removeReference();

  // We now initialize the stack area to 100k (fast temporary memory to be used by the engine).
  {
    int stackSize = 0x100000;
    char *stackBuffer = hkAllocate<char>(stackSize, HK_MEMORY_CLASS_BASE);
    tm->setStackArea(stackBuffer, stackSize);
  }

  return true;
}

void HavokCleanup() {
  if (moppBuffer ) free(moppBuffer );
  if (moppWelding) free(moppWelding);

  hkThreadMemory *tm;
  if ((tm = &hkThreadMemory::getInstance())) {
    tm->setStackArea(0, 0);
    tm->releaseCachedMemory();
//  hkBaseSystem::clearThreadResources(); // not in main-thread
    tm->removeReference();
  }

  // Quit base system
  hkBaseSystem::quit();
}

bool HavokBegin(bool havokserver) {
  HavokInit();

  if (havokserver)
    HavokServer();

  return true;
}

void HavokEnd(bool havokserver) {
  HavokCleanup();

  if (havokserver)
    ;
}

#endif
