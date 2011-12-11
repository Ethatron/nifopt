
#include <vector>
//#include <map>
#include <set>
//#include <list>
//#include <queue>
//#include <string>
//#include <utility>

using namespace std;

/* ---------------------------------------------------- */

class objFace;
class objVertex : public Vertex3d {
public:
  objVertex() { idx = 0; }

  int idx;

  std::set<unsigned int> o;
  std::vector<class objFace *> f;

  bool operator < (const class objVertex &other) const {
    /* don't include odx/idx */
    return memcmp(this, &other, offsetof(Vertex3d, odx)) < 0;
  }

  void fill();
};

class objFace {
public:
  objFace() { narea = 0.0; v[0] = v[1] = v[2] = NULL; }

  float nx, ny, nz;
  float narea;

  /* just copy it around, don't loose it */
  unsigned short extra;

  class objVertex *v[3];

  void fill();
};

void objVertex::fill() {
#if 0
  struct { float x,y,z; } crosses;
  float crossmag;

  crosses.x = 0.0;
  crosses.y = 0.0;
  crosses.z = 0.0;
  crossmag  = 0.0;

  assert(f.size() != 0);

  vector<class objFace *>::iterator itf;
  for (itf = f.begin(); itf != f.end(); itf++) {
    crosses.x += (*itf)->nx;
    crosses.y += (*itf)->ny;
    crosses.z += (*itf)->nz;
    crossmag  += (*itf)->narea * 2.0f;
    /* faster than normalize(),
     * has no precision problems either
     * parallelizable
     */
  }

  crosses.x /= crossmag;
  crosses.y /= crossmag;
  crosses.z /= crossmag;

  nx = crosses.x;
  ny = crosses.y;
  nz = crosses.z;
#endif
};

void objFace::fill() {
  class objVertex *v1 = v[0];
  class objVertex *v2 = v[1];
  class objVertex *v3 = v[2];

  assert(v1 != v2); assert(v1 != NULL);
  assert(v1 != v3); assert(v2 != NULL);
  assert(v2 != v3); assert(v3 != NULL);

  struct { float x,y,z; } v1v2;
  struct { float x,y,z; } v1v3;

  v1v2.x = v1->x - v2->x;
  v1v2.y = v1->y - v2->y;
  v1v2.z = v1->z - v2->z;

  v1v3.x = v1->x - v3->x;
  v1v3.y = v1->y - v3->y;
  v1v3.z = v1->z - v3->z;

  struct { float x,y,z; } cross;
  float mag;

  cross.x = v1v2.y * v1v3.z - v1v2.z * v1v3.y;
  cross.y = v1v2.z * v1v3.x - v1v2.x * v1v3.z;
  cross.z = v1v2.x * v1v3.y - v1v2.y * v1v3.x;

  mag = sqrtf((cross.x * cross.x) +
	      (cross.y * cross.y) +
	      (cross.z * cross.z));

#if 0
  /* Oscape specific */
  if (cross.z < 0) {
    cross.x = -cross.x;
    cross.y = -cross.y;
    cross.z = -cross.z;
  }
#endif

  nx = cross.x;
  ny = cross.y;
  nz = cross.z;

  narea = 0.5f * fabsf(mag);
};

struct V {
  bool operator()(const class objVertex *s1, const class objVertex *s2) const {
    /* don't include odx/idx */
    return memcmp(s1, s2, offsetof(Vertex3d, odx)) < 0;
  }
};

std::set<class objVertex *, struct V> Vertices;
std::vector<class objFace *> Faces;

/* ---------------------------------------------------- */

int tri_procd = 0;
int tri_opted = 0;

class objFace *RegisterFace(const Vertex3d& _p1,
				     const Vertex3d& _p2,
				     const Vertex3d& _p3) {

    /* orient CCW */
    if (TriArea(_p1, _p2, _p3) < 0.0) {
      return RegisterFace(_p1, _p3, _p2); }

    Vertex3d p1, p2, p3;

    /* allow any position for points */
    p1 = _p1;
    p2 = _p2;
    p3 = _p3;

    class objVertex vp1, vp2, vp3;
    class objVertex *v1, *v2, *v3;

    set<class objVertex *, struct V>::iterator i1;
    set<class objVertex *, struct V>::iterator i2;
    set<class objVertex *, struct V>::iterator i3;

    /* copy the entire vertex-information */
    *((Vertex3d*)&vp1) = p1; v1 = NULL;
    *((Vertex3d*)&vp2) = p2; v2 = NULL;
    *((Vertex3d*)&vp3) = p3; v3 = NULL;

    i1 = Vertices.find(&vp1); if (i1 != Vertices.end()) v1 = *i1;
    i2 = Vertices.find(&vp2); if (i2 != Vertices.end()) v2 = *i2;
    i3 = Vertices.find(&vp3); if (i3 != Vertices.end()) v3 = *i3;

    /* any collapse of old indices must be recorded */
    if (v1) v1->o.insert(p1.odx);
    if (v2) v2->o.insert(p2.odx);
    if (v3) v3->o.insert(p3.odx);

    /* all three vertices exist already, look
     * if they share a common face
     */
    if (v1 && v2 && v3) {
      /* remove degenerate face cases, the surface is a closed
       * mesh, so the adjacent vertices will snap as well and
       * close this "hole"
       */
      if (v1 == v2)
	return NULL;
      if (v1 == v3)
	return NULL;
      if (v2 == v3)
	return NULL;

      vector<class objFace *>::iterator f1, f2, f3;

      /* compare each face for vertex 1 with the faces of vertex 2 */
      for (f1 = v1->f.begin(); f1 != v1->f.end(); f1++) {
	for (f2 = v2->f.begin(); f2 != v2->f.end(); f2++) {
	  /* if they have a shared faced, search that one for vertex 3 */
	  if ((*f1) == (*f2)) {
	    for (f3 = v3->f.begin(); f3 != v3->f.end(); f3++) {
	      /* the face is shared by all three vertices, which means we got a double */
	      if ((*f1) == (*f3))
		return NULL;
	    }
	  }
	}
      }
    }
    /* not all of the vertices have been allocated,
     * still this can be a degenerate case
     */
    else {
      /* remove degenerate face cases, the surface is a closed
       * mesh, so the adjacent vertices will snap as well and
       * close this "hole"
       */
      if (memcmp(&p1, &p2, offsetof(Vertex3d, odx)) == 0)
	return NULL;
      if (memcmp(&p1, &p3, offsetof(Vertex3d, odx)) == 0)
	return NULL;
      if (memcmp(&p2, &p3, offsetof(Vertex3d, odx)) == 0)
	return NULL;
    }

    if (!v1) {
	v1 = new class objVertex;

	/* copy the entire vertex-information */
	*((Vertex3d*)v1) = _p1;
	/* any collapse of old indices must be recorded */
	v1->o.insert(_p1.odx);

	Vertices.insert(v1);
    }

    if (!v2) {
	v2 = new class objVertex;

	/* copy the entire vertex-information */
	*((Vertex3d*)v2) = _p2;
	/* any collapse of old indices must be recorded */
	v2->o.insert(_p2.odx);

	Vertices.insert(v2);
    }

    if (!v3) {
	v3 = new class objVertex;

	/* copy the entire vertex-information */
	*((Vertex3d*)v3) = _p3;
	/* any collapse of old indices must be recorded */
	v3->o.insert(_p3.odx);

	Vertices.insert(v3);
    }

    assert(v1 != v2);
    assert(v1 != v3);
    assert(v2 != v3);

    class objFace *f; f = new class objFace;

    f->v[0] = v1; v1->f.push_back(f);
    f->v[1] = v2; v2->f.push_back(f);
    f->v[2] = v3; v3->f.push_back(f);

    Faces.push_back(f);
    return f;
}

class objFace *SplitFace(const Vertex3d& tp1, const Vertex3d& tp2, const Vertex3d& tp3) {
    return RegisterFace(tp1, tp2, tp3);
}

void CollectFace(Triangle3d *t) {
    const Vertex3d& p1 = t->point1();
    const Vertex3d& p2 = t->point2();
    const Vertex3d& p3 = t->point3();
    class objFace *f;

    if ((f = SplitFace(p1, p2, p3)))
      f->extra = t->extra;

    tri_procd++;
    if ((tri_procd & 0xFF) == 0)
      fprintf(stderr, "faces processed %d\r", tri_procd);
}

void CalculateGeometryNormals() {
    set<class objVertex *, struct V>::iterator itv;
    vector<class objFace *>::iterator itf;

    for (itf = Faces.begin(); itf != Faces.end(); itf++)
      (*itf)->fill();
    for (itv = Vertices.begin(); itv != Vertices.end(); itv++)
      (*itv)->fill();
}

/* ---------------------------------------------------- */

std::set<class objVertex *, struct V> SectorVertices;
std::vector<class objFace *> SectorFaces;
std::vector<int> SectorRemap;

void IndexGeometry(int maxv) {
    set<class objVertex *, struct V>::iterator itv;
    vector<class objFace *>::iterator itf;

    for (itf = Faces.begin(); itf != Faces.end(); itf++) {
      /* copy the entire vertex-information */
      class objVertex vo1 = *((*itf)->v[0]), *v1;
      class objVertex vo2 = *((*itf)->v[1]), *v2;
      class objVertex vo3 = *((*itf)->v[2]), *v3;

      /* --------------------------------------------- */
      set<class objVertex *, struct V>::iterator i1;
      set<class objVertex *, struct V>::iterator i2;
      set<class objVertex *, struct V>::iterator i3;

      class objFace *f; f = new class objFace;

      v1 = NULL; i1 = SectorVertices.find(&vo1); if (i1 != SectorVertices.end()) v1 = *i1;
      v2 = NULL; i2 = SectorVertices.find(&vo2); if (i2 != SectorVertices.end()) v2 = *i2;
      v3 = NULL; i3 = SectorVertices.find(&vo3); if (i3 != SectorVertices.end()) v3 = *i3;

      if (!v1) {
	  v1 = new class objVertex;

	  /* copy the entire vertex-information */
	  *((Vertex3d*)v1) = *((Vertex3d*)&vo1);
	  /* any collapse of old indices must be recorded */
	  v1->o = vo1.o;

      	  SectorVertices.insert(v1);
      }

      if (!v2) {
	  v2 = new class objVertex;

	  /* copy the entire vertex-information */
	  *((Vertex3d*)v2) = *((Vertex3d*)&vo2);
	  /* any collapse of old indices must be recorded */
	  v2->o = vo2.o;

      	  SectorVertices.insert(v2);
      }

      if (!v3) {
	  v3 = new class objVertex;

	  /* copy the entire vertex-information */
	  *((Vertex3d*)v3) = *((Vertex3d*)&vo3);
	  /* any collapse of old indices must be recorded */
	  v3->o = vo3.o;

      	  SectorVertices.insert(v3);
      }

      f->v[0] = v1; v1->f.push_back(f);
      f->v[1] = v2; v2->f.push_back(f);
      f->v[2] = v3; v3->f.push_back(f);
      f->extra = (*itf)->extra;
      f->fill();

      SectorFaces.push_back(f);
    }

    /* record all eliminations (non-mappings) */
    SectorRemap.resize(maxv);
    SectorRemap.assign(maxv, -1);

    int idx = 0; for (itv = SectorVertices.begin(); itv != SectorVertices.end(); itv++) {
      /* assign index the moment of writing it out */
      (*itv)->idx = idx++;

      /* must be 1 at least: odx -> idx */
      assert((*itv)->o.size() > 0);

      set<unsigned int>::iterator iti;
      for (iti = (*itv)->o.begin(); iti != (*itv)->o.end(); iti++)
	/* remember mapping of original index to new index */
	SectorRemap[(*iti)] = (*itv)->idx;
    }
}

std::vector<class objVertex *> SectorVerticeO;
std::vector<class objFace *> SectorFaceO;
std::vector<unsigned int> SectorRemapO;

typedef std::vector<class objVertex *>::iterator viterator;
typedef std::vector<class objFace *>::iterator fiterator;

#define _USE_MATH_DEFINES
#include <math.h>
#include <vector>
#include <list>
#include <string.h>
#include <string>
#include <iostream>
#include <fstream>
#include <cassert>
#include "../AMDTootle/include/tootlelib.h"

#if   _MSC_VER == 1500
/* static lib */
#define LIBFILE	"Static_2k8_"
#ifndef	NDEBUG
#define LIBSUFX	"MT_d"
#else
#define LIBSUFX	"MT"
#endif
#elif _MSC_VER == 1600
/* dynamic lib */
#define LIBFILE	"DLL"
#define LIBSUFX	""
#endif

#ifndef	NDEBUG
#pragma comment(lib,"d3d9.lib")
#pragma comment(lib,"d3dx9.lib")
#ifdef	_WIN64
#pragma comment(lib,"../../../AMDTootle/lib/Tootle" LIBFILE LIBSUFX "64")
#else
#pragma comment(lib,"../../../AMDTootle/lib/Tootle" LIBFILE LIBSUFX)
#endif
#else
#pragma comment(lib,"d3d9.lib")
#pragma comment(lib,"d3dx9.lib")
#ifdef	_WIN64
#pragma comment(lib,"../../../AMDTootle/lib/Tootle" LIBFILE LIBSUFX "64")
#else
#pragma comment(lib,"../../../AMDTootle/lib/Tootle" LIBFILE LIBSUFX)
#endif
#endif

#include "../AMDTootle/samples/Timer.cpp"
#include "../AMDTootle/samples/Timer.h"

//=================================================================================================================================
/// Enumeration for the choice of test cases for tootle.
//=================================================================================================================================
enum TootleAlgorithm
{
   NA_TOOTLE_ALGORITHM,                // Default invalid choice.
   TOOTLE_VCACHE_ONLY,                 // Only perform vertex cache optimization.
   TOOTLE_CLUSTER_VCACHE_OVERDRAW,     // Call the clustering, optimize vertex cache and overdraw individually.
   TOOTLE_FAST_VCACHECLUSTER_OVERDRAW, // Call the functions to optimize vertex cache and overdraw individually.  This is using
                                       //  the algorithm from SIGGRAPH 2007.
   TOOTLE_OPTIMIZE,                    // Call a single function to optimize vertex cache, cluster and overdraw.
   TOOTLE_FAST_OPTIMIZE                // Call a single function to optimize vertex cache, cluster and overdraw using
                                       //  a fast algorithm from SIGGRAPH 2007.
};

//=================================================================================================================================
/// A simple structure to store the settings for this sample app
//=================================================================================================================================
struct TootleSettings
{
   const char*           pMeshName;
   const char*           pViewpointName;
   unsigned int          nClustering;
   unsigned int          nCacheSize;
   TootleFaceWinding     eWinding;
   TootleAlgorithm       algorithmChoice;         // five different types of algorithm to test Tootle
   TootleOverdrawOptimizer eODrawOptimizer;
   TootleVCacheOptimizer eVCacheOptimizer;        // the choice for vertex cache optimization algorithm, it can be either
                                                  //  TOOTLE_VCACHE_AUTO, TOOTLE_VCACHE_LSTRIPS, TOOTLE_VCACHE_DIRECT3D or
                                                  //  TOOTLE_VCACHE_TIPSY.
   bool                  bOptimizeVertexMemory;   // true if you want to optimize vertex memory location, false to skip
   bool                  bMeasureOverdraw;        // true if you want to measure overdraw, false to skip
};

//=================================================================================================================================
/// A simple structure to hold Tootle statistics
//=================================================================================================================================
struct TootleStats
{
   unsigned int nClusters;
   float        fVCacheIn;
   float        fVCacheOut;
   float        fOverdrawIn;
   float        fOverdrawOut;
   float        fMaxOverdrawIn;
   float        fMaxOverdrawOut;
   double       fOptimizeVCacheTime;
   double       fClusterMeshTime;
   double       fOptimizeOverdrawTime;
   double       fVCacheClustersTime;
   double       fOptimizeVCacheAndClusterMeshTime;
   double       fTootleOptimizeTime;
   double       fTootleFastOptimizeTime;
   double       fMeasureOverdrawTime;
   double       fOptimizeVertexMemoryTime;
};

const float INVALID_TIME = -1;

//=================================================================================================================================
/// A helper function to print formatted TOOTLE statistics
/// \param f      A file to print the statistics to
/// \param pStats The statistics to be printed
//=================================================================================================================================
void PrintStats( FILE *fp, TootleStats* pStats )
{
   assert( fp );
   assert( pStats );

   fprintf( fp, " Clusters         : %u\n"
                " CacheIn/Out      : %.3fx (%.3f/%.3f)\n",
              pStats->nClusters,
              pStats->fVCacheIn/pStats->fVCacheOut,
              pStats->fVCacheIn,
              pStats->fVCacheOut );

   if ( pStats->fMeasureOverdrawTime >= 0 )
   {
      fprintf( fp, " OverdrawIn/Out   : %.3fx (%.3f/%.3f)\n"
                   " OverdrawMaxIn/Out: %.3fx (%.3f/%.3f)\n",
               pStats->fOverdrawIn/pStats->fOverdrawOut,
               pStats->fOverdrawIn,
               pStats->fOverdrawOut,
               pStats->fMaxOverdrawIn/pStats->fMaxOverdrawOut,
               pStats->fMaxOverdrawIn,
               pStats->fMaxOverdrawOut );
   }

   // print out the timing result if appropriate.
   if ( pStats->fOptimizeVCacheTime >= 0 )
   {
      fprintf( fp, " OptimizeVCache               = %.4lf seconds\n", pStats->fOptimizeVCacheTime );
   }

   if ( pStats->fClusterMeshTime >= 0 )
   {
      fprintf( fp, " ClusterMesh                  = %.4lf seconds\n", pStats->fClusterMeshTime );
   }

   if ( pStats->fVCacheClustersTime >= 0 )
   {
      fprintf( fp, " VCacheClusters               = %.4lf seconds\n", pStats->fVCacheClustersTime );
   }

   if ( pStats->fOptimizeVCacheAndClusterMeshTime >= 0 )
   {
      fprintf( fp, " OptimizeVCacheAndClusterMesh = %.4lf seconds\n", pStats->fOptimizeVCacheAndClusterMeshTime );
   }

   if ( pStats->fOptimizeOverdrawTime >= 0 )
   {
      fprintf( fp, " OptimizeOverdraw             = %.4lf seconds\n", pStats->fOptimizeOverdrawTime );
   }

   if ( pStats->fTootleOptimizeTime >= 0 )
   {
      fprintf( fp, " TootleOptimize               = %.4lf seconds\n", pStats->fTootleOptimizeTime );
   }

   if ( pStats->fTootleFastOptimizeTime >= 0 )
   {
      fprintf( fp, " TootleFastOptimize           = %.4lf seconds\n", pStats->fTootleFastOptimizeTime );
   }

   if ( pStats->fMeasureOverdrawTime >= 0 )
   {
      fprintf( fp, " MeasureOverdraw              = %.4lf seconds\n", pStats->fMeasureOverdrawTime );
   }

   if ( pStats->fOptimizeVertexMemoryTime >= 0 )
   {
      fprintf( fp, " OptimizeVertexMemory         = %.4lf seconds\n", pStats->fOptimizeVertexMemoryTime );
   }
}

// 3D Vector ( for posisiton and normals )
struct Vertex3D
{
   float x;
   float y;
   float z;
}; // End of ObjVertex3D

void OptimizeGeometry() {
    set<class objVertex *, struct V>::iterator itv;
    vector<class objFace *>::iterator itf;
    vector<int>::iterator iti;

    // initialize settings to defaults
    TootleSettings settings;

    settings.pMeshName             = NULL;
    settings.pViewpointName        = NULL;
    settings.nClustering           = 0;
    settings.nCacheSize            = TOOTLE_DEFAULT_VCACHE_SIZE;
    settings.eWinding              = TOOTLE_CCW;
#ifndef	NDEBUG
    settings.algorithmChoice       = (1 ? TOOTLE_FAST_OPTIMIZE : TOOTLE_OPTIMIZE);
    settings.eODrawOptimizer	   = (1 ? TOOTLE_OVERDRAW_FAST : TOOTLE_OVERDRAW_RAYTRACE);
#else
    settings.algorithmChoice       = (optimizequick ? TOOTLE_FAST_OPTIMIZE : TOOTLE_OPTIMIZE);
    settings.eODrawOptimizer	   = (optimizequick ? TOOTLE_OVERDRAW_FAST : TOOTLE_OVERDRAW_RAYTRACE);
#endif
    settings.eVCacheOptimizer      = TOOTLE_VCACHE_TIPSY;            // the auto selection as the default to optimize vertex cache
    settings.bOptimizeVertexMemory = true;                           // default value is to optimize the vertex memory
    settings.bMeasureOverdraw      = true;                           // default is to measure overdraw

    if ((SectorVertices.size() > 0) &&
        (SectorFaces.size() > 0)) {
      /* VBs starts on index 0 */
      unsigned int i = 0, idx = 0;

      fprintf(stderr, "optimizing %d faces ...\r", SectorFaces.size());
      fprintf(stdout, "optimizing:\n");
//    fprintf(stdout, " Vertices         : %d\n", SectorVertices.size());
//    fprintf(stdout, " Faces            : %d\n", SectorFaces.size());
      fflush(stderr);
      fflush(stdout);

      // build buffers containing only the vertex positions and indices, since this is what Tootle requires
      std::vector<Vertex3D> vertices;
      vertices.resize(SectorVertices.size());

      SectorVerticeO.resize(SectorVertices.size());
      i = 0; idx = 0; for (itv = SectorVertices.begin(); itv != SectorVertices.end(); itv++) {
        SectorVerticeO[i] = (*itv);

      	/* assign index the moment of writing it out */
	(*itv)->idx = idx++;

        vertices[i].x = (float)((*itv)->x);
        vertices[i].y = (float)((*itv)->y);
        vertices[i].z = (float)((*itv)->z); i++;
      }

      std::vector<unsigned int> indices;
      indices.resize(SectorFaces.size() * 3);

      SectorFaceO.resize(SectorFaces.size());
      i = 0; idx = 0; for (itf = SectorFaces.begin(); itf != SectorFaces.end(); itf++) {
        SectorFaceO[idx] = (*itf);

	/* assign index the moment of writing it out */
	idx++;

        indices[i] = (*itf)->v[0]->idx; i++;
        indices[i] = (*itf)->v[1]->idx; i++;
        indices[i] = (*itf)->v[2]->idx; i++;
      }

      // ******************************************
      //    Load viewpoints if necessary
      // ******************************************

      // read viewpoints if needed
      std::vector<Vertex3D> viewpoints;
#if 0
      viewpoints.resize(4 * 16);

      /* http://en.wikibooks.org/wiki/Mathematica/Uniform_Spherical_Distribution:
       * 
       * (* Uniform on [0,Pi], longitude, only goes halfway round*)
       * theta := RandomReal[ {0, Pi} ];
       * (* Colatitude, from pole to pole *)
       * phi   := 2 ArcSin[Sqrt[Random[ {0, 1} ]]];
       */

      /* cameras on a cylinder around the terrain-patch */
      for (float theta = 0; theta < M_2PI; theta++)
      for (float phi   = 0; phi   < M_PI2; phi++  ) {
	float x = sin(phi) * cos(theta);
	float y = sin(phi) * sin(theta);
	float z = cos(phi);
	Real n = sqrt(x * x + y * y + z * z);
	struct Vertex3D v = {(float)(x / n), (float)(y / n), (float)(z / n)};

	viewpoints.push_back(v);
      }
#endif

      // if we didn't get any viewpoints, then use a NULL array
      const float* pViewpoints = NULL;
      unsigned int nViewpoints = (unsigned int)viewpoints.size();
      if (viewpoints.size() > 0)
	pViewpoints = (const float *)&viewpoints[0];

      // *****************************************************************
      //   Prepare the mesh and initialize stats variables
      // *****************************************************************

      unsigned int  nFaces    = (unsigned int)  indices.size() / 3;
      unsigned int  nVertices = (unsigned int)  vertices.size();
      float*        pfVB      = (float *)       &vertices[0];
      unsigned int* pnIB      = (unsigned int *)&indices[0];
      unsigned int  nStride   = 3 * sizeof(float);

      TootleStats stats;

      // initialize the timing variables
      stats.fOptimizeVCacheTime               = INVALID_TIME;
      stats.fClusterMeshTime                  = INVALID_TIME;
      stats.fVCacheClustersTime               = INVALID_TIME;
      stats.fOptimizeVCacheAndClusterMeshTime = INVALID_TIME;
      stats.fOptimizeOverdrawTime             = INVALID_TIME;
      stats.fTootleOptimizeTime               = INVALID_TIME;
      stats.fTootleFastOptimizeTime           = INVALID_TIME;
      stats.fMeasureOverdrawTime              = INVALID_TIME;
      stats.fOptimizeVertexMemoryTime         = INVALID_TIME;

      TootleResult result;

      // measure input VCache efficiency
      if (TootleMeasureCacheEfficiency(pnIB, nFaces, settings.nCacheSize, &stats.fVCacheIn) != TOOTLE_OK)
      	return;

      if (settings.bMeasureOverdraw) {
        // measure input overdraw.  Note that we assume counter-clockwise vertex winding.
        TootleMeasureOverdraw(pfVB, pnIB, nVertices, nFaces, nStride, pViewpoints, nViewpoints, settings.eWinding,
                                      &stats.fOverdrawIn, &stats.fMaxOverdrawIn);
      }

      // allocate an array to hold the cluster ID for each face
      std::vector<unsigned int> faceClusters;
      faceClusters.resize(nFaces + 1);
      unsigned int nNumClusters;

      Timer timer;
      timer.Reset();

      // **********************************************************************************************************************
      //   Optimize the mesh:
      //
      // The following cases show five examples for developers on how to use the library functions in Tootle.
      // 1. If you are interested in optimizing vertex cache only, see the TOOTLE_VCACHE_ONLY case.
      // 2. If you are interested to optimize vertex cache and overdraw, see either TOOTLE_CLUSTER_VCACHE_OVERDRAW
      //     or TOOTLE_OPTIMIZE cases.  The former uses three separate function calls while the latter uses a single
      //     utility function.
      // 3. To use the algorithm from SIGGRAPH 2007 (v2.0), see TOOTLE_FAST_VCACHECLUSTER_OVERDRAW or TOOTLE_FAST_OPTIMIZE
      //     cases.  The former uses two separate function calls while the latter uses a single utility function.
      //
      // Note the algorithm from SIGGRAPH 2007 (v2.0) is very fast but produces less quality results especially for the
      //  overdraw optimization.  During our experiments with some medium size models, we saw an improvement of 1000x in
      //  running time (from 20+ minutes to less than 1 second) for using v2.0 calls vs v1.2 calls.  The resulting vertex
      //  cache optimization is very similar while the overdraw optimization drops from 3.8x better to 2.5x improvement over
      //  the input mesh.
      //  Developers should always run the overdraw optimization using the fast algorithm from SIGGRAPH initially.
      //  If they require a better result, then re-run the overdraw optimization using the old v1.2 path (TOOTLE_OVERDRAW_AUTO).
      //  Passing TOOTLE_OVERDRAW_AUTO to the algorithm will let the algorithm choose between Direct3D or raytracing path
      //  depending on the total number of clusters (less than 200 clusters, it will use Direct3D path otherwise it will
      //  use raytracing path since the raytracing path will be faster than the Direct3D path at that point).
      //
      // Tips: use v2.0 for fast optimization, and v1.2 to further improve the result by mix-matching the calls.
      // **********************************************************************************************************************

      result = TOOTLE_OK;
      switch (settings.algorithmChoice) {
        case TOOTLE_VCACHE_ONLY:
          // *******************************************************************************************************************
          // Perform Vertex Cache Optimization ONLY
          // *******************************************************************************************************************
          stats.nClusters = 1;

          // Optimize vertex cache
          if ((result = TootleOptimizeVCache(pnIB, nFaces, nVertices, settings.nCacheSize,
                                   pnIB, NULL, settings.eVCacheOptimizer)) != TOOTLE_OK)
            break;

          stats.fOptimizeVCacheTime = timer.GetElapsed();
          break;

        case TOOTLE_CLUSTER_VCACHE_OVERDRAW:
          // *******************************************************************************************************************
          // An example of calling clustermesh, vcacheclusters and optimize overdraw individually.
          // This case demonstrate mix-matching v1.2 clustering with v2.0 overdraw optimization.
          // *******************************************************************************************************************

          // Cluster the mesh, and sort faces by cluster.
          if ((result = TootleClusterMesh(pfVB, pnIB, nVertices, nFaces, nStride,
          	 settings.nClustering, pnIB, &faceClusters[0], NULL)) != TOOTLE_OK)
             break;

          stats.fClusterMeshTime = timer.GetElapsed();
          timer.Reset();

          // The last entry of of faceClusters store the total number of clusters.
          stats.nClusters = faceClusters[nFaces];

          // Perform vertex cache optimization on the clustered mesh.
          if ((result = TootleVCacheClusters(pnIB, nFaces, nVertices,
          	settings.nCacheSize, &faceClusters[0],
                 pnIB, NULL, settings.eVCacheOptimizer)) != TOOTLE_OK)
             break;

          stats.fVCacheClustersTime = timer.GetElapsed();
          timer.Reset();

          // Optimize the draw order (using v1.2 path: TOOTLE_OVERDRAW_AUTO, the default path is from v2.0--SIGGRAPH version).
          if ((result = TootleOptimizeOverdraw(pfVB, pnIB, nVertices, nFaces, nStride, pViewpoints, nViewpoints,
          	settings.eWinding, &faceClusters[0], pnIB, NULL, settings.eODrawOptimizer)) != TOOTLE_OK)
             break;

          stats.fOptimizeOverdrawTime = timer.GetElapsed();
          break;

        case TOOTLE_FAST_VCACHECLUSTER_OVERDRAW:
          // *******************************************************************************************************************
          // An example of calling v2.0 optimize vertex cache and clustering mesh with v1.2 overdraw optimization.
          // *******************************************************************************************************************

          // Optimize vertex cache and create cluster
          // The algorithm from SIGGRAPH combine the vertex cache optimization and clustering mesh into a single step
          if ((result = TootleFastOptimizeVCacheAndClusterMesh(pnIB, nFaces, nVertices,
          	settings.nCacheSize, pnIB, &faceClusters[0], &nNumClusters, TOOTLE_DEFAULT_ALPHA)) != TOOTLE_OK)
             break;

          stats.fOptimizeVCacheAndClusterMeshTime = timer.GetElapsed();
          timer.Reset();

          stats.nClusters = nNumClusters;

          // In this example, we use TOOTLE_OVERDRAW_AUTO to show that we can mix-match the clustering and
          //  vcache computation from the new library with the overdraw optimization from the old library.
          //  TOOTLE_OVERDRAW_AUTO will choose between using Direct3D or CPU raytracing path.  This path is
          //  much slower than TOOTLE_OVERDRAW_FAST but usually produce 2x better results.
          if ((result = TootleOptimizeOverdraw(pfVB, pnIB, nVertices, nFaces, nStride, NULL, 0,
          	settings.eWinding, &faceClusters[0], pnIB, NULL, settings.eODrawOptimizer)) != TOOTLE_OK)
             break;

          stats.fOptimizeOverdrawTime = timer.GetElapsed();
          break;

        case TOOTLE_OPTIMIZE:
          // *******************************************************************************************************************
          // An example of using a single utility function to perform v1.2 optimizations.
          // *******************************************************************************************************************

          // This function will compute the entire optimization (cluster mesh, vcache per cluster, and optimize overdraw).
          // It will use TOOTLE_OVERDRAW_FAST as the default overdraw optimization
          if ((result = TootleOptimize(pfVB, pnIB, nVertices, nFaces, nStride,
          	settings.nCacheSize, pViewpoints, nViewpoints, settings.eWinding,
          	pnIB, &nNumClusters, settings.eVCacheOptimizer)) != TOOTLE_OK)
             break;

          stats.fTootleOptimizeTime = timer.GetElapsed();
          stats.nClusters = nNumClusters;
          break;

        case TOOTLE_FAST_OPTIMIZE:
          // *******************************************************************************************************************
          // An example of using a single utility function to perform v2.0 optimizations.
          // *******************************************************************************************************************

          // This function will compute the entire optimization (optimize vertex cache, cluster mesh, and optimize overdraw).
          // It will use TOOTLE_OVERDRAW_FAST as the default overdraw optimization
          if ((result = TootleFastOptimize(pfVB, pnIB, nVertices, nFaces, nStride,
          	settings.nCacheSize, settings.eWinding, pnIB, &nNumClusters, TOOTLE_DEFAULT_ALPHA)) != TOOTLE_OK)
            break;

          stats.fTootleFastOptimizeTime = timer.GetElapsed();
          stats.nClusters = nNumClusters;
          break;

        default:
          // wrong algorithm choice
          break;
      }

      if (result != TOOTLE_OK)
        return;

      // measure output VCache efficiency
      if (TootleMeasureCacheEfficiency(pnIB, nFaces, settings.nCacheSize, &stats.fVCacheOut) != TOOTLE_OK)
        return;

      if (settings.bMeasureOverdraw) {
        // measure output overdraw
        timer.Reset();

        TootleMeasureOverdraw(pfVB, pnIB, nVertices, nFaces, nStride, pViewpoints, nViewpoints, settings.eWinding,
                                        &stats.fOverdrawOut, &stats.fMaxOverdrawOut);

        stats.fMeasureOverdrawTime = timer.GetElapsed();
      }

      //-----------------------------------------------------------------------------------------------------
      // recreate the faces based on the reordered index-buffer

      for (unsigned int ii = 0, iif = 0; ii < indices.size(); ii += 3, iif += 1) {
        class objFace *f = SectorFaceO[iif];

        f->v[0]->idx = indices[ii + 0];
        f->v[1]->idx = indices[ii + 1];
        f->v[2]->idx = indices[ii + 2];

      	/* recreate normal (doesn't change by reordering) */
      	f->fill();
      }

      //-----------------------------------------------------------------------------------------------------
      // PERFORM VERTEX MEMORY OPTIMIZATION (rearrange memory layout for vertices based on the final indices
      //  to exploit vertex cache prefetch).
      //  We want to optimize the vertex memory locations based on the final optimized index buffer that will
      //  be in the output file.
      //  Thus, in this sample code, we recompute a copy of the indices that point to the original vertices
      //  (pnIBTmp) to be passed into the function TootleOptimizeVertexMemory.  If we use the array pnIB, we
      //  will optimize for the wrong result since the array pnIB is based on the rehashed vertex location created
      //  by the function ObjLoader.
      //-----------------------------------------------------------------------------------------------------
      timer.Reset();

      // The actual total number of vertices referenced by the indices
      if (settings.bOptimizeVertexMemory) {
	SectorRemapO.resize(nVertices);
	unsigned int* pfRM = (unsigned int *)&SectorRemapO[0];

        // For this sample code, we are just going to use vertexRemapping array result.  This is to support general obj
        //  file input and output.
        //  In fact, we are sending the wrong vertex buffer here (it should be based on the original file instead of the
        //  rehashed vertices).  But, it is ok because we do not request the reordered vertex buffer as an output.
        if (TootleOptimizeVertexMemory(pfVB, pnIB, nVertices, nFaces, nStride,
        			       pfVB, pnIB, pfRM) != TOOTLE_OK)
          return;

	/// \param pnVertexRemapOut     An array that will receive a vertex re-mapping.  May be NULL if the output is not requested.
	///                              If not NULL, must be an array of size nVertices.  The i'th element of the output array contains
	///                               the position of the input vertex i in the new vertex re-ordering.

	//TootleResult TOOTLE_DLL TootleOptimizeVertexMemory( const void*         pVB,
	//                                                    const unsigned int* pnIB,
	//                                                    unsigned int        nVertices,
	//                                                    unsigned int        nFaces,
	//                                                    unsigned int        nVBStride,
	//                                                    void*               pVBOut,
	//                                                    unsigned int*       pnIBOut,
	//                                                    unsigned int*       pnVertexRemapOut );

        //-----------------------------------------------------------------------------------------------------
        // recreate the vertices/faces based on the reordered vertex/index-buffer

	i = 0; idx = 0; for (itv = SectorVertices.begin(); itv != SectorVertices.end(); itv++) {
	  /* move from position i to idx */ 
	  idx = SectorRemapO[i++];

	  SectorVerticeO[idx] = (*itv);

	  /* normal is still valid this way */
	  (*itv)->idx = idx;
	}

	//-----------------------------------------------------------------------------------------------------
	// recreate the faces based on the reordered index-buffer

	for (unsigned int ii = 0, iif = 0; ii < indices.size(); ii += 3, iif += 1) {
	  class objFace *f = SectorFaceO[iif];

	  f->v[0]->idx = indices[ii + 0];
	  f->v[1]->idx = indices[ii + 1];
	  f->v[2]->idx = indices[ii + 2];

	  /* recreate normal (doesn't change by reordering) */
	  f->fill();
	}

        stats.fOptimizeVertexMemoryTime = timer.GetElapsed();
      }

      tri_opted += nFaces;
      fprintf(stderr, "faces optimized %d\r", tri_opted);
      fflush(stderr);

      PrintStats(stdout, &stats);
    }
}

void OptimizeGeometry(vector<Vertex3D> &vertices,
		     vector<unsigned int> &indices,
		     vector<unsigned int> &remap) {

    // initialize settings to defaults
    TootleSettings settings;

    settings.pMeshName             = NULL;
    settings.pViewpointName        = NULL;
    settings.nClustering           = 0;
    settings.nCacheSize            = TOOTLE_DEFAULT_VCACHE_SIZE;
    settings.eWinding              = TOOTLE_CCW;
#ifndef	NDEBUG
    settings.algorithmChoice       = (1 ? TOOTLE_FAST_OPTIMIZE : TOOTLE_OPTIMIZE);
    settings.eODrawOptimizer	   = (1 ? TOOTLE_OVERDRAW_FAST : TOOTLE_OVERDRAW_RAYTRACE);
#else
    settings.algorithmChoice       = (optimizequick ? TOOTLE_FAST_OPTIMIZE : TOOTLE_OPTIMIZE);
    settings.eODrawOptimizer	   = (optimizequick ? TOOTLE_OVERDRAW_FAST : TOOTLE_OVERDRAW_RAYTRACE);
#endif
    settings.eVCacheOptimizer      = TOOTLE_VCACHE_TIPSY;            // the auto selection as the default to optimize vertex cache
    settings.bOptimizeVertexMemory = true;                           // default value is to optimize the vertex memory
    settings.bMeasureOverdraw      = true;                           // default is to measure overdraw

    {
      /* VBs starts on index 0 */
      unsigned int i = 0;

      fprintf(stdout, "optimizing:\n");
//    fprintf(stdout, " Vertices         : %d\n", LocalVertices.size());
//    fprintf(stdout, " Faces            : %d\n", LocalFaces.size());

      // ******************************************
      //    Load viewpoints if necessary
      // ******************************************

      // read viewpoints if needed
      std::vector<Vertex3D> viewpoints;
#if 0
      viewpoints.resize(4 * 16);

      /* http://en.wikibooks.org/wiki/Mathematica/Uniform_Spherical_Distribution:
       * 
       * (* Uniform on [0,Pi], longitude, only goes halfway round*)
       * theta := RandomReal[ {0, Pi} ];
       * (* Colatitude, from pole to pole *)
       * phi   := 2 ArcSin[Sqrt[Random[ {0, 1} ]]];
       */

      /* cameras on a cylinder around the terrain-patch */
      for (float theta = 0; theta < M_2PI; theta++)
      for (float phi   = 0; phi   < M_PI2; phi++  ) {
	float x = sin(phi) * cos(theta);
	float y = sin(phi) * sin(theta);
	float z = cos(phi);
	Real n = sqrt(x * x + y * y + z * z);
	struct Vertex3D v = {(float)(x / n), (float)(y / n), (float)(z / n)};

	viewpoints.push_back(v);
      }
#endif

      // if we didn't get any viewpoints, then use a NULL array
      const float* pViewpoints = NULL;
      unsigned int nViewpoints = (unsigned int)viewpoints.size();
      if (viewpoints.size() > 0)
	pViewpoints = (const float *)&viewpoints[0];

      // *****************************************************************
      //   Prepare the mesh and initialize stats variables
      // *****************************************************************

      unsigned int  nFaces    = (unsigned int)  indices.size() / 3;
      unsigned int  nVertices = (unsigned int)  vertices.size();
      float*        pfVB      = (float *)       &vertices[0];
      unsigned int* pnIB      = (unsigned int *)&indices[0];
      unsigned int  nStride   = 3 * sizeof(float);

      TootleStats stats;

      // initialize the timing variables
      stats.fOptimizeVCacheTime               = INVALID_TIME;
      stats.fClusterMeshTime                  = INVALID_TIME;
      stats.fVCacheClustersTime               = INVALID_TIME;
      stats.fOptimizeVCacheAndClusterMeshTime = INVALID_TIME;
      stats.fOptimizeOverdrawTime             = INVALID_TIME;
      stats.fTootleOptimizeTime               = INVALID_TIME;
      stats.fTootleFastOptimizeTime           = INVALID_TIME;
      stats.fMeasureOverdrawTime              = INVALID_TIME;
      stats.fOptimizeVertexMemoryTime         = INVALID_TIME;

      TootleResult result;

      // measure input VCache efficiency
      if (TootleMeasureCacheEfficiency(pnIB, nFaces, settings.nCacheSize, &stats.fVCacheIn) != TOOTLE_OK)
      	return;

      if (settings.bMeasureOverdraw) {
        // measure input overdraw.  Note that we assume counter-clockwise vertex winding.
        TootleMeasureOverdraw(pfVB, pnIB, nVertices, nFaces, nStride, pViewpoints, nViewpoints, settings.eWinding,
                                      &stats.fOverdrawIn, &stats.fMaxOverdrawIn);
      }

      // allocate an array to hold the cluster ID for each face
      std::vector<unsigned int> faceClusters;
      faceClusters.resize(nFaces + 1);
      unsigned int nNumClusters;

      Timer timer;
      timer.Reset();

      // **********************************************************************************************************************
      //   Optimize the mesh:
      //
      // The following cases show five examples for developers on how to use the library functions in Tootle.
      // 1. If you are interested in optimizing vertex cache only, see the TOOTLE_VCACHE_ONLY case.
      // 2. If you are interested to optimize vertex cache and overdraw, see either TOOTLE_CLUSTER_VCACHE_OVERDRAW
      //     or TOOTLE_OPTIMIZE cases.  The former uses three separate function calls while the latter uses a single
      //     utility function.
      // 3. To use the algorithm from SIGGRAPH 2007 (v2.0), see TOOTLE_FAST_VCACHECLUSTER_OVERDRAW or TOOTLE_FAST_OPTIMIZE
      //     cases.  The former uses two separate function calls while the latter uses a single utility function.
      //
      // Note the algorithm from SIGGRAPH 2007 (v2.0) is very fast but produces less quality results especially for the
      //  overdraw optimization.  During our experiments with some medium size models, we saw an improvement of 1000x in
      //  running time (from 20+ minutes to less than 1 second) for using v2.0 calls vs v1.2 calls.  The resulting vertex
      //  cache optimization is very similar while the overdraw optimization drops from 3.8x better to 2.5x improvement over
      //  the input mesh.
      //  Developers should always run the overdraw optimization using the fast algorithm from SIGGRAPH initially.
      //  If they require a better result, then re-run the overdraw optimization using the old v1.2 path (TOOTLE_OVERDRAW_AUTO).
      //  Passing TOOTLE_OVERDRAW_AUTO to the algorithm will let the algorithm choose between Direct3D or raytracing path
      //  depending on the total number of clusters (less than 200 clusters, it will use Direct3D path otherwise it will
      //  use raytracing path since the raytracing path will be faster than the Direct3D path at that point).
      //
      // Tips: use v2.0 for fast optimization, and v1.2 to further improve the result by mix-matching the calls.
      // **********************************************************************************************************************

      result = TOOTLE_OK;
      switch (settings.algorithmChoice) {
        case TOOTLE_VCACHE_ONLY:
          // *******************************************************************************************************************
          // Perform Vertex Cache Optimization ONLY
          // *******************************************************************************************************************
          stats.nClusters = 1;

          // Optimize vertex cache
          if ((result = TootleOptimizeVCache(pnIB, nFaces, nVertices, settings.nCacheSize,
                                   pnIB, NULL, settings.eVCacheOptimizer)) != TOOTLE_OK)
            break;

          stats.fOptimizeVCacheTime = timer.GetElapsed();
          break;

        case TOOTLE_CLUSTER_VCACHE_OVERDRAW:
          // *******************************************************************************************************************
          // An example of calling clustermesh, vcacheclusters and optimize overdraw individually.
          // This case demonstrate mix-matching v1.2 clustering with v2.0 overdraw optimization.
          // *******************************************************************************************************************

          // Cluster the mesh, and sort faces by cluster.
          if ((result = TootleClusterMesh(pfVB, pnIB, nVertices, nFaces, nStride,
          	 settings.nClustering, pnIB, &faceClusters[0], NULL)) != TOOTLE_OK)
             break;

          stats.fClusterMeshTime = timer.GetElapsed();
          timer.Reset();

          // The last entry of of faceClusters store the total number of clusters.
          stats.nClusters = faceClusters[nFaces];

          // Perform vertex cache optimization on the clustered mesh.
          if ((result = TootleVCacheClusters(pnIB, nFaces, nVertices,
          	settings.nCacheSize, &faceClusters[0],
                 pnIB, NULL, settings.eVCacheOptimizer)) != TOOTLE_OK)
             break;

          stats.fVCacheClustersTime = timer.GetElapsed();
          timer.Reset();

          // Optimize the draw order (using v1.2 path: TOOTLE_OVERDRAW_AUTO, the default path is from v2.0--SIGGRAPH version).
          if ((result = TootleOptimizeOverdraw(pfVB, pnIB, nVertices, nFaces, nStride, pViewpoints, nViewpoints,
          	settings.eWinding, &faceClusters[0], pnIB, NULL, settings.eODrawOptimizer)) != TOOTLE_OK)
             break;

          stats.fOptimizeOverdrawTime = timer.GetElapsed();
          break;

        case TOOTLE_FAST_VCACHECLUSTER_OVERDRAW:
          // *******************************************************************************************************************
          // An example of calling v2.0 optimize vertex cache and clustering mesh with v1.2 overdraw optimization.
          // *******************************************************************************************************************

          // Optimize vertex cache and create cluster
          // The algorithm from SIGGRAPH combine the vertex cache optimization and clustering mesh into a single step
          if ((result = TootleFastOptimizeVCacheAndClusterMesh(pnIB, nFaces, nVertices,
          	settings.nCacheSize, pnIB, &faceClusters[0], &nNumClusters, TOOTLE_DEFAULT_ALPHA)) != TOOTLE_OK)
             break;

          stats.fOptimizeVCacheAndClusterMeshTime = timer.GetElapsed();
          timer.Reset();

          stats.nClusters = nNumClusters;

          // In this example, we use TOOTLE_OVERDRAW_AUTO to show that we can mix-match the clustering and
          //  vcache computation from the new library with the overdraw optimization from the old library.
          //  TOOTLE_OVERDRAW_AUTO will choose between using Direct3D or CPU raytracing path.  This path is
          //  much slower than TOOTLE_OVERDRAW_FAST but usually produce 2x better results.
          if ((result = TootleOptimizeOverdraw(pfVB, pnIB, nVertices, nFaces, nStride, NULL, 0,
          	settings.eWinding, &faceClusters[0], pnIB, NULL, settings.eODrawOptimizer)) != TOOTLE_OK)
             break;

          stats.fOptimizeOverdrawTime = timer.GetElapsed();
          break;

        case TOOTLE_OPTIMIZE:
          // *******************************************************************************************************************
          // An example of using a single utility function to perform v1.2 optimizations.
          // *******************************************************************************************************************

          // This function will compute the entire optimization (cluster mesh, vcache per cluster, and optimize overdraw).
          // It will use TOOTLE_OVERDRAW_FAST as the default overdraw optimization
          if ((result = TootleOptimize(pfVB, pnIB, nVertices, nFaces, nStride,
          	settings.nCacheSize, pViewpoints, nViewpoints, settings.eWinding,
          	pnIB, &nNumClusters, settings.eVCacheOptimizer)) != TOOTLE_OK)
             break;

          stats.fTootleOptimizeTime = timer.GetElapsed();
          stats.nClusters = nNumClusters;
          break;

        case TOOTLE_FAST_OPTIMIZE:
          // *******************************************************************************************************************
          // An example of using a single utility function to perform v2.0 optimizations.
          // *******************************************************************************************************************

          // This function will compute the entire optimization (optimize vertex cache, cluster mesh, and optimize overdraw).
          // It will use TOOTLE_OVERDRAW_FAST as the default overdraw optimization
          if ((result = TootleFastOptimize(pfVB, pnIB, nVertices, nFaces, nStride,
          	settings.nCacheSize, settings.eWinding, pnIB, &nNumClusters, TOOTLE_DEFAULT_ALPHA)) != TOOTLE_OK)
            break;

          stats.fTootleFastOptimizeTime = timer.GetElapsed();
          stats.nClusters = nNumClusters;
          break;

        default:
          // wrong algorithm choice
          break;
      }

      if (result != TOOTLE_OK)
        return;

      // measure output VCache efficiency
      if (TootleMeasureCacheEfficiency(pnIB, nFaces, settings.nCacheSize, &stats.fVCacheOut) != TOOTLE_OK)
        return;

      if (settings.bMeasureOverdraw) {
        // measure output overdraw
        timer.Reset();

        TootleMeasureOverdraw(pfVB, pnIB, nVertices, nFaces, nStride, pViewpoints, nViewpoints, settings.eWinding,
                                        &stats.fOverdrawOut, &stats.fMaxOverdrawOut);

        stats.fMeasureOverdrawTime = timer.GetElapsed();
      }

      //-----------------------------------------------------------------------------------------------------
      // PERFORM VERTEX MEMORY OPTIMIZATION (rearrange memory layout for vertices based on the final indices
      //  to exploit vertex cache prefetch).
      //  We want to optimize the vertex memory locations based on the final optimized index buffer that will
      //  be in the output file.
      //  Thus, in this sample code, we recompute a copy of the indices that point to the original vertices
      //  (pnIBTmp) to be passed into the function TootleOptimizeVertexMemory.  If we use the array pnIB, we
      //  will optimize for the wrong result since the array pnIB is based on the rehashed vertex location created
      //  by the function ObjLoader.
      //-----------------------------------------------------------------------------------------------------
      timer.Reset();

#if 0
      // The actual total number of vertices referenced by the indices
      if (settings.bOptimizeVertexMemory) {
	remap.resize(nVertices);
	unsigned int* pfRM = (unsigned int *)&remap[0];

	// For this sample code, we are just going to use vertexRemapping array result.  This is to support general obj
	//  file input and output.
	//  In fact, we are sending the wrong vertex buffer here (it should be based on the original file instead of the
	//  rehashed vertices).  But, it is ok because we do not request the reordered vertex buffer as an output.
	if (TootleOptimizeVertexMemory(pfVB, pnIB, nVertices, nFaces, nStride,
	  pfVB, pnIB, pfRM) != TOOTLE_OK)
	  return;

	/// \param pnVertexRemapOut     An array that will receive a vertex re-mapping.  May be NULL if the output is not requested.
	///                              If not NULL, must be an array of size nVertices.  The i'th element of the output array contains
	///                               the position of the input vertex i in the new vertex re-ordering.

	//TootleResult TOOTLE_DLL TootleOptimizeVertexMemory( const void*         pVB,
	//                                                    const unsigned int* pnIB,
	//                                                    unsigned int        nVertices,
	//                                                    unsigned int        nFaces,
	//                                                    unsigned int        nVBStride,
	//                                                    void*               pVBOut,
	//                                                    unsigned int*       pnIBOut,
	//                                                    unsigned int*       pnVertexRemapOut );

	stats.fOptimizeVertexMemoryTime = timer.GetElapsed();
      }
#endif

      tri_opted += nFaces;
      fprintf(stderr, "faces optimized %d\r", tri_opted);
      fflush(stderr);

      PrintStats(stdout, &stats);
    }
}

void FreeGeometry() {
  set<class objVertex *, struct V>::iterator itv;
  vector<class objFace *>::iterator itf;

  for (itf = Faces.begin(); itf != Faces.end(); itf++)
    delete (*itf);
  for (itv = Vertices.begin(); itv != Vertices.end(); itv++)
    delete (*itv);

  for (itf = SectorFaces.begin(); itf != SectorFaces.end(); itf++)
    delete (*itf);
  for (itv = SectorVertices.begin(); itv != SectorVertices.end(); itv++)
    delete (*itv);

  /* raw containers */
  Faces.clear();
  Vertices.clear();

  /* unoptimized containers */
  SectorFaces.clear();
  SectorVertices.clear();
  SectorRemap.clear();	// old index to new index

  /* optimized containers */
  SectorFaceO.clear();
  SectorVerticeO.clear();
  SectorRemapO.clear();	// new index to opt index
}
