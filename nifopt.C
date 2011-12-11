
#define	_CRT_SECURE_NO_WARNINGS
#define	_CRT_NONSTDC_NO_DEPRECATE

#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdarg.h>
#include <stdio.h>
#include <ctype.h>
#include <time.h>
#include <math.h>
#include <limits.h>
#include <sys/types.h>

//#define	D3D_DEBUG_INFO
#include <d3d9.h>
#include <d3dx9.h>

bool geotoprimitive = false;
bool stripstolists = false;
bool optimizehavok = false;
bool optimizesmmopp = false;
bool optimizemmmopp = false;
bool optimizelists = false;
bool optimizeparts = false;
bool optimizekeys = false;
bool optimizequick = false;
bool optimizetexts = false;
bool optimizefar = false;
bool barestripfar = false;
bool parallaxmapping = false;
bool texturepaths = true;
bool reattachnodes = true;
bool droptrimeshes = false;
bool dropextras = false;
bool skiphashcheck = false;
bool skipmodels = false;
bool skipimages = false;
bool skipsounds = false;
bool skipexisting = false;
bool skipnewer = false;
bool skipprocessing = false;
bool processhidden = false;
bool passthrough = false;
bool simulation = false;
bool datamining = false;
bool verbose = false;
bool critical = false;

#if 0
void barprintf(FILE *f, char fill, const char *format, ...) {
  va_list args;
  va_start(args, format);

  if (!critical) {
    char buf[1024];
    char fll[1024];
    size_t len;

    memset(fll, fill, 1024);
    sprintf(buf, format, args);
    len = strlen(buf);
    fll[(80 - 4) - len] = '\0';
    fprintf(f, "\n%c %s %s\n", fill, buf, fll);

    fflush(stdout);
  }
}

void nfoprintf(FILE *f, const char *format, ...) {
  va_list args;
  va_start(args, format);

  if (!critical) {
    char buf[1024];
    sprintf(buf, format, args);
    fprintf(f, format, args);
  }
}

void errprintf(FILE *f, const char *format, ...) {
  va_list args;
  va_start(args, format);

  if (1) {
    char buf[1024];
    sprintf(buf, format, args);
    fprintf(f, format, args);
  }
}
#else
#define barprintf(f, fill, format, ...) {		\
  if (!critical) {					\
    char buf[1024];					\
    char fll[1024];					\
    size_t len;						\
							\
    memset(fll, fill, 1024);				\
    sprintf(buf, format, ## __VA_ARGS__);		\
    len = strlen(buf);					\
    fll[(80 - 4) - len] = '\0';				\
    fprintf(f, "\n%c %s %s\n", fill, buf, fll);		\
							\
    fflush(stdout);					\
  }							\
}

#define nfoprintf(f, format, ...) { if (!critical) { fprintf(f, format, ## __VA_ARGS__); } }
#define errprintf(f, format, ...) { if (1        ) { fprintf(f, format, ## __VA_ARGS__); } }
#endif

/*
 * Find the first occurrence of find in s, ignore case.
 */
char *stristr(const char *s, const char *find) {
  char c, sc;
  size_t len;

  if ((c = *find++) != 0) {
    c = tolower((unsigned char)c);
    len = strlen(find);
    do {
      do {
	if ((sc = *s++) == 0)
	  return (NULL);
      } while ((char)tolower((unsigned char)sc) != c);
    } while (strnicmp(s, find, len) != 0);

    s--;
  }

  return ((char *)s);
}

char tempbuf[1024];

#define addnote(format, ...) sprintf(tempbuf, format, ## __VA_ARGS__), notes.push_back(tempbuf)
#define adderrr(format, ...) sprintf(tempbuf, format, ## __VA_ARGS__), errrs.push_back(tempbuf)

#include <string>
#include <vector>
#include <map>

using namespace std;

vector<string> notes;
vector<string> errrs;
vector<string> orphegms;
vector<string> orphtris;
vector<string> texpths;
vector<string> zerobytes;
map<string, string> duplicates;
map<string, int> texmode;

/* ---------------------------------------------------- */

size_t processedinbytes = 0;
size_t processedoubytes = 0;
size_t compressedinbytes = 0;
size_t compressedoubytes = 0;
size_t convertedinbytes = 0;
size_t convertedoubytes = 0;
ptrdiff_t deltaiobytes = 0;
int virtualbsafiles = 0;
int virtualbsabytes = 0;

#include "nifopt-io.C"

/* ---------------------------------------------------------------------------------------------- */

bool compressimages = false;
bool leavehdrtexts = false;
bool normalmapts = false;
bool colormapgamma = true;
int normalsteepness = 1;

int fixedtexts = 0;
int modifiedtexts = 0;
int planartexts = 0;
int changedformats = 0;

#include "nifopt-tex.cpp"

#define	TEXTUREOPT
#ifdef	TEXTUREOPT
bool ProcessDDS(const char *inname, const char *ouname, const char *rep) {
    /* read the DDS */
    void *inmem = NULL, *oumem = NULL;
    UINT insize =    0, ousize =    0;
    LPD3DXBUFFER oubuf = NULL;
    char buf[1024], *newsuf = NULL;

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

      UINT rdsze = (UINT)
      ioreadfile(inmem, insize, file);
      ioclosefile(file);

      if (rdsze != insize) {
	errprintf(stderr, "can't read the DDS\n");
	return false;
      }
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
      if (((DDS_HEADER *)inmem)->dwMagicNumber == MAKEFOURCC('D', 'D', 'S', ' '))
	mipl = ((DDS_HEADER *)inmem)->dwMipMapCount;
      if (!mipl)
	mipl = 1;

      /* with depth: cube or volume */
      if (((DDS_HEADER *)inmem)->dwHeaderFlags & 0x00800000U)
	;//addnote(" Texture has unsupported format (cubic or volumetric).\n");
      else if ((res = D3DXCreateTextureFromFileInMemoryEx(
	pD3DDevice, inmem, insize,
	D3DX_DEFAULT, D3DX_DEFAULT, mipl/*D3DX_DEFAULT*/,
	0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_FILTER_NONE/*D3DX_DEFAULT*/,
	D3DX_FILTER_NONE/*D3DX_DEFAULT*/, 0, NULL, NULL,
	&base
      )) == D3D_OK) {

//    if ((res = D3DXCreateTextureFromFileInMemory(
//	pD3DDevice, inmem, insize, &base
//    )) == D3D_OK) {

	barprintf(stdout, '#', "%s", rep);
	if (verbose)
	  nfoprintf(stdout, "%s\n", inname);

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
	if (!cangamma)
	  addnote(" Turned gamma off for face-modifier texture.\n");

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

	/**/ if (base->GetType() == D3DRTYPE_VOLUMETEXTURE)
	  unsupported = true, levels = levelc;
	else if (base->GetType() == D3DRTYPE_CUBETEXTURE)
	  unsupported = true, levels = levelc;
	else if (base->GetType() != D3DRTYPE_TEXTURE)
	  unsupported = true;

	/* file-size sanity check */
	if (unsupported)
	  addnote(" Texture has unsupported format.\n");
	else if (levelc > 1) {
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
	    addnote(" Truncated texture found, fixed!\n"), fixedtexts++;
	}

	/* choose what to do */
	int relevel = 0;
	if (levels > levelc)
	  relevel = levelc + 1;
	else if (levels < levelc)
	  relevel = 0 - levels;
	else if (levels == 1)
	  relevel = 0 - levels;

	/* remip it just all again */
	bool remip = false;
	if (optimizetexts || (compressimages && uncompressed))
	  remip = true, relevel = min(0, relevel);
	if (leavehdrtexts && ((depth < -8) || (depth > 8)))
	  remip = false;
	if (unsupported)
	  remip = false;

	/* go ahead, compress */
	bool success = false;
	if (remip) {
	  bool unknown = false;
	  bool dooptcomp = compressimages || !uncompressed;

	  nfoprintf(stdout, "processing:\n");

	  if (dooptcomp) {
	    /* normal maps */
	    if (issuf(inname, "_n") ||
		issuf(inname, "_fn") ||
		issuf(inname, "_xyz") ||
		issuf(inname, "_xyzd")) {
	      /* only known object-space normal-maps */
	      bool ts = normalmapts;
	      if (stristr(inname, "landscapelod"))
		ts = false;
	      if (relevel > 0)
		ts = false;

	      /**/ if (issuf(inname, "_xyzd")) {
		if (ts) success = TextureCompressXYZD (&base, relevel);
		else    success = TextureCompress_XYZD(&base, relevel);
	      }
	      else if (issuf(inname, "_xyz")) {
		if (ts) success = TextureCompressXYZ  (&base, relevel);
		else    success = TextureCompress_XYZ (&base, relevel);
	      }
	      else if (channels == 4) {
		if (ts) success = TextureCompressXYZD (&base, relevel);
		else    success = TextureCompress_XYZD(&base, relevel);
	      }
	      else if (channels == 3) {
		if (ts) success = TextureCompressXYZ  (&base, relevel);
		else    success = TextureCompress_XYZ (&base, relevel);
	      }
	      else if (channels == 2) {
		if (ts) success = TextureCompressXY   (&base, relevel);
		else    success = TextureCompressXY   (&base, relevel);
	      }
/*	      else if (channels == 1) {
		if (ts) success = TextureCompressZ    (&base, relevel);
		else    success = TextureCompressZ    (&base, relevel);
	      } */
	      else
		unknown = true;

	      newsuf = "_n";
	      if (issuf(inname, "_fn"))
		newsuf = "_fn";
	    }
	    /* glow maps (l/rgb-only) */
	    else if (issuf(inname, "_g")) {
	      /**/ if (channels == 4)
		success = TextureCompressRGBH(&base, relevel, false);
	      else if (channels == 3)
		success = TextureCompressRGB (&base, relevel, false);
//	      else if (channels == 2)
//	        success = TextureCompressLA  (&base, relevel);
//	      else if (channels == 1)
//	        success = TextureCompressL   (&base, relevel
	      else
		unknown = true;
	    }
	    /* hair gloss maps (l/rgb-only, greyscale) */
	    else if (issuf(inname, "_hh")) {
	      bool ga = cangamma;

  	      /**/ if (channels == 4)
  		success = TextureCompressRGB (&base, relevel, ga);
  	      else if (channels == 3)
		success = TextureCompressRGB (&base, relevel, ga);
	      else
		unknown = true;
	    }
	    /* hair detail maps (rgba) */
	    else if (issuf(inname, "_hl")) {
	      bool ga = cangamma;

	      /**/ if (channels == 4)
		success = TextureCompressRGBA(&base, relevel, ga);
	      else if (channels == 3)
		success = TextureCompressRGB (&base, relevel, ga);
	      else
		unknown = true;
	    }
	    /* color maps */
	    else if (issuf(inname, "_rgb") ||
		     issuf(inname, "_rgba")) {
	      bool ga = cangamma;

	      /**/ if (issuf(inname, "_rgba"))
		success = TextureCompressRGBA(&base, relevel, ga);
	      else if (issuf(inname, "_rgb"))
		success = TextureCompressRGB (&base, relevel, ga);
	      else
		unknown = true;

	      newsuf = "";
	    }
	    /* color maps */
	    else {
	      bool ga = cangamma;

	      /**/ if (channels == 4 && istransp)
		success = TextureCompressRGBA(&base, relevel, ga);
	      else if (channels == 4)
		success = TextureCompressRGBH(&base, relevel, ga);
	      else if (channels == 3)
		success = TextureCompressRGB (&base, relevel, ga);
//	      else if (channels == 2)
//	        success = TextureCompressLA  (&base, relevel);
//	      else if (channels == 1)
//	        success = TextureCompressL   (&base, relevel);
	      else
		unknown = true;
	    }

	    if (success)
	      modifiedtexts++;
	  }

	  if (!dooptcomp || (!success && unknown)) {
	    /* normal maps */
	    if (issuf(inname, "_n") ||
		issuf(inname, "_fn") ||
		issuf(inname, "_xyz") ||
		issuf(inname, "_xyzd")) {
	      /* only known object-space normal-maps */
	      bool ts = normalmapts;
	      if (stristr(inname, "landscapelod"))
		ts = false;
	      if (relevel > 0)
		ts = false;

	      /**/ if (issuf(inname, "_xyzd")) {
		if (ts) success = TextureConvertXYZD (&base, relevel);
		else    success = TextureConvert_XYZD(&base, relevel);
	      }
	      else if (issuf(inname, "_xyz")) {
		if (ts) success = TextureConvertXYZ  (&base, relevel);
		else    success = TextureConvert_XYZ (&base, relevel);
	      }
	      else if (channels == 4) {
		if (ts) success = TextureConvertXYZD (&base, relevel);
		else    success = TextureConvert_XYZD(&base, relevel);
	      }
	      else if (channels == 3) {
		if (ts) success = TextureConvertXYZ  (&base, relevel);
		else    success = TextureConvert_XYZ (&base, relevel);
	      }
	      else if (channels == 2) {
		if (ts) success = TextureConvertXY   (&base, relevel);
		else    success = TextureConvertXY   (&base, relevel);
	      }
/*	      else if (channels == 1) {
		if (ts) success = TextureConvertZ    (&base, relevel);
		else    success = TextureConvertZ    (&base, relevel);
	      } */
	      else
		unknown = true;

	      newsuf = "_n";
	      if (issuf(inname, "_fn"))
		newsuf = "_fn";
	    }
	    /* glow maps (l/rgb-only) */
	    else if (issuf(inname, "_g")) {
	      /**/ if (channels == 4)
		success = TextureConvertRGBH(&base, relevel, false);
	      else if (channels == 3)
		success = TextureConvertRGB (&base, relevel, false);
	      else if (channels == 2)
		success = TextureConvertLA  (&base, relevel);
	      else if (channels == 1)
		success = TextureConvertL   (&base, relevel);
	      else
		unknown = true;
	    }
	    /* hair gloss maps (l/rgb-only, greyscale) */
	    else if (issuf(inname, "_hh")) {
	      bool ga = cangamma;

	      /**/ if (channels == 4)
		success = TextureConvertRGB (&base, relevel, ga);
	      else if (channels == 3)
		success = TextureConvertRGB (&base, relevel, ga);
	      else if (channels == 2)
		success = TextureConvertLA  (&base, relevel);
	      else if (channels == 1)
		success = TextureConvertL   (&base, relevel);
	      else
		unknown = true;
	    }
	    /* hair detail maps (rgba) */
	    else if (issuf(inname, "_hl")) {
	      bool ga = cangamma;

	      /**/ if (channels == 4)
		success = TextureConvertRGBA(&base, relevel, ga);
	      else if (channels == 3)
		success = TextureConvertRGB (&base, relevel, ga);
	      else
		unknown = true;
	    }
	    /* color maps */
	    else if (issuf(inname, "_rgb") ||
		     issuf(inname, "_rgba")) {
	      bool ga = cangamma;

	      /**/ if (issuf(inname, "_rgba"))
		success = TextureConvertRGBA(&base, relevel, ga);
	      else if (issuf(inname, "_rgb"))
		success = TextureConvertRGB (&base, relevel, ga);
	      else
		unknown = true;

	      newsuf = "";
	    }
	    /* color maps */
	    else {
	      bool ga = cangamma;

	      /**/ if (channels == 4 && istransp)
		success = TextureConvertRGBA(&base, relevel, ga);
	      else if (channels == 4)
		success = TextureConvertRGBH(&base, relevel, ga);
	      else if (channels == 3)
		success = TextureConvertRGB (&base, relevel, ga);
	      else if (channels == 2)
		success = TextureConvertLA  (&base, relevel);
	      else if (channels == 1)
		success = TextureConvertL   (&base, relevel);
	    }

	    if (success)
	      modifiedtexts++;
	  }

	  if (!success && !unknown)
	    errprintf(stderr, "failed to regenerate DDS\n");
	}
	/* do the mip-levels */
	else if (levels != levelc) {
	  LPDIRECT3DTEXTURE9 replct;
	  LPDIRECT3DSURFACE9 stex, srep;
	  DWORD filter = D3DX_FILTER_BOX;
	  HRESULT res;

	  nfoprintf(stdout, "processing:\n");

	  /* normal maps */
	  if (issuf(inname, "_n") || issuf(inname, "_fn"))
	    ;
	  /* glow maps */
	  else if (issuf(inname, "_g"))
	    ;
	  /* color maps */
	  else if (cangamma)
	    filter |= D3DX_FILTER_DITHER | D3DX_FILTER_SRGB;
	  else
	    filter |= D3DX_FILTER_DITHER;

	  if ((res = pD3DDevice->CreateTexture(based.Width, based.Height, levels, 0, based.Format, D3DPOOL_MANAGED, &replct, NULL)) == D3D_OK) {
	    base->GetSurfaceLevel(0, &stex);
	    replct->GetSurfaceLevel(0, &srep);

	    res = D3DXLoadSurfaceFromSurface(srep, NULL, NULL, stex, NULL, NULL, filter, 0);

	    stex->Release();
	    srep->Release();

	    if (res == D3D_OK) {
	      base->Release();
	      base = replct;

	      res = D3DXFilterTexture(base, NULL, 0, filter);
	    }
	    else {
	      replct->Release();
	      replct = base;
	    }
	  }

	  if (!(success = (res == D3D_OK)))
	    errprintf(stderr, "failed to regenerate DDS\n");

	  if (success)
	    modifiedtexts++;
	}

	if (success && (res = D3DXSaveTextureToFileInMemory(&oubuf, D3DXIFF_DDS, base, NULL)) == D3D_OK) {
	  D3DFORMAT oldFormat = based.Format;

	  /* read changed information in */
	  base->GetLevelDesc(0, &based);

	  /* recalculate levels after conversion */
	  int levels = 1; {
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

	  if (((based.Format != D3DFMT_DXT1) &&
	       (based.Format != D3DFMT_DXT2) &&
	       (based.Format != D3DFMT_DXT3) &&
	       (based.Format != D3DFMT_DXT4) &&
	       (based.Format != D3DFMT_DXT5) &&
	       (based.Format != D3DFMT_ATI1) &&
	       (based.Format != D3DFMT_ATI2)) && !uncompressed)
	    addnote(" Texture was compressed.\n");
	  if (((based.Format == D3DFMT_DXT1) ||
	       (based.Format == D3DFMT_DXT2) ||
	       (based.Format == D3DFMT_DXT3) ||
	       (based.Format == D3DFMT_DXT4) ||
	       (based.Format == D3DFMT_DXT5) ||
	       (based.Format == D3DFMT_ATI1) ||
	       (based.Format == D3DFMT_ATI2)) && uncompressed)
	    addnote(" Texture was uncompressed.\n");

	  if (oldFormat != based.Format)
	    changedformats++;
	  if ((based.Width == 1) && (based.Height == 1))
	    planartexts++;

	  nfoprintf(stdout, " Format           : %s to %s\n", from, findFormat(based.Format));
	  nfoprintf(stdout, " Dimensions       : %dx%d - %d to %d levels\n", based.Width, based.Height, levelc, levels);

	  if (notes.size() > 0) {
	    nfoprintf(stdout, "\nnotes:\n");
	    for (size_t n = 0; n < notes.size(); n++)
	      nfoprintf(stdout, notes[n].data());
	  }

	  oumem  = oubuf->GetBufferPointer();
	  ousize = oubuf->GetBufferSize();

	  /* mark the file to have been optimized */
	  ((DDS_HEADER *)oumem)->dwReserved1[0] = MAKEFOURCC('N', 'O', '0', '1');
	}

	notes.clear();
	base->Release();
      }
      else
	errprintf(stderr, "can't read the DDS\n");
    }

    /* dummy content (copy over) */
    bool success = true;
    if (!oumem)
      oumem = inmem, ousize = insize;

    if (oumem) {
      /* take the ouname if we have it */
      if (ouname)
	strcpy(buf, ouname);
      else
	strcpy(buf, inname);

      /* we do never overwrite files! append suffix */
      putext(buf, buf, "dds");
      if (newsuf)
	setsuf(buf, buf, newsuf);
      if (!stricmp(inname, buf))
	putsuf(buf, buf, "_opt");
      assert(isext(buf, "dds"));

      /* put out the file-contents */
      if (!simulation) {
	struct iofile *file;

	if ((file = ioopenfile(buf, "wb")))
	  ioflushtofile(oumem, ousize, file);
	else {
	  errprintf(stderr, "can't write the DDS\n"); success = false; }
      }

      /* uh, MicroSoft, no signed "ssize_t"? */
      if (insize)
	processedinbytes += insize, convertedinbytes += insize, deltaiobytes += insize;
      if (ousize)
	processedoubytes += ousize, convertedoubytes += ousize, deltaiobytes -= ousize;
   }

   if (inmem) free(inmem);
   if (oubuf) oubuf->Release();

   return success;
}
#endif

/* ---------------------------------------------------------------------------------------------- */

bool compresssounds = false;

#undef	SOUNDOPT

/* ---------------------------------------------------- */

/* Oblivion knows these:
 *
 * bhkAabbPhantom
 * bhkAngularDashpotAction
 * bhkBallAndSocketConstraint
 * bhkBlendCollisionObject
 * bhkBlendController
 * bhkBoxShape
 * bhkBreakableConstraint
 * bhkCachingShapePhantom
 * bhkCapsuleShape
 * bhkCollisionObject
 * bhkConvexSweepShape
 * bhkConvexTransformShape
 * bhkConvexVerticesShape
 * bhkCylinderShape
 * bhkDashpotAction
 * bhkExtraData
 * bhkFixedConstraint
 * bhkHingeConstraint
 * bhkLimitedHingeConstraint
 * bhkListShape
 * bhkMalleableConstraint
 * bhkMeshShape
 * bhkMoppBvTreeShape
 * bhkMotorAction
 * bhkMouseSpringAction
 * bhkMultiSphereShape
 * bhkNiTriStripsShape
 * bhkPCollisionObject
 * bhkPackedNiTriStripsShape
 * bhkPlaneShape
 * bhkPrismaticConstraint
 * bhkRagdollConstraint
 * bhkRigidBodyT
 * bhkRigidBody
 * bhkSPCollisionObject
 * bhkSimpleShapePhantom
 * bhkSphereShape
 * bhkSpringAction
 * bhkStiffSpringConstraint
 * bhkTransformShape
 * bhkTriangleShape
 * bhkWheelConstraint
 * hkPackedNiTriStripsData
 */

#define	HAVOKMOPP
#ifdef	HAVOKMOPP
bool havokserver = false;
#include "nifopt-hvk.C"
#endif

/* ---------------------------------------------------- */

struct Sort3d {
  /* material/unique identifier (primary sort-key) */
  signed short groupid;
};

struct Point3d : Sort3d {
  float x, y, z;
};

struct Vertex3d : Point3d {
  struct { float u, v; } uv[16];

  float nx, ny, nz;
  float r, g, b, a;

  /* original vertex index/indices */
  unsigned int odx;

  /* these are functions of uv[0] & normal
   * there may be rounding errors in it, so
   * don't consider them inherently unique
   */
  float tx, ty, tz;
  float bx, by, bz;
};

struct Triangle3d {
  Vertex3d p1, p2, p3;

  /* just copy it around, don't loose it */
  unsigned short extra;

  Vertex3d& point1() { return p1; };
  Vertex3d& point2() { return p2; };
  Vertex3d& point3() { return p3; };
};

/* ---------------------------------------------------- */

#define TriArea	narea

float narea(const Point3d& v1, const Point3d& v2, const Point3d& v3) {
  struct { float x,y,z; } v1v2;
  struct { float x,y,z; } v1v3;

  v1v2.x = v1.x - v2.x;
  v1v2.y = v1.y - v2.y;
  v1v2.z = v1.z - v2.z;

  v1v3.x = v1.x - v3.x;
  v1v3.y = v1.y - v3.y;
  v1v3.z = v1.z - v3.z;

  struct { float x,y,z; } cross;
  float mag;

  cross.x = v1v2.y * v1v3.z - v1v2.z * v1v3.y;
  cross.y = v1v2.z * v1v3.x - v1v2.x * v1v3.z;
  cross.z = v1v2.x * v1v3.y - v1v2.y * v1v3.x;

  mag = sqrtf(
    (cross.x * cross.x) +
    (cross.y * cross.y) +
    (cross.z * cross.z)
  );

  return 0.5f * fabsf(mag);
}

float narea(const Point3d& v1, const Point3d& v2, const Point3d& v3, Point3d& n) {
  struct { float x,y,z; } v1v2;
  struct { float x,y,z; } v1v3;

  v1v2.x = v1.x - v2.x;
  v1v2.y = v1.y - v2.y;
  v1v2.z = v1.z - v2.z;

  v1v3.x = v1.x - v3.x;
  v1v3.y = v1.y - v3.y;
  v1v3.z = v1.z - v3.z;

  struct { float x,y,z; } cross;
  float mag;

  cross.x = v1v2.y * v1v3.z - v1v2.z * v1v3.y;
  cross.y = v1v2.z * v1v3.x - v1v2.x * v1v3.z;
  cross.z = v1v2.x * v1v3.y - v1v2.y * v1v3.x;

  mag = sqrtf(
    (cross.x * cross.x) +
    (cross.y * cross.y) +
    (cross.z * cross.z)
  );

  n.x = cross.x;
  n.y = cross.y;
  n.z = cross.z;

  return 0.5f * fabsf(mag);
}

/* ---------------------------------------------------- */

#define	NIFLIB_STATIC_LINK

#include "../NIFtools/trunk/niflib/include/niflib.h"

#include "../NIFtools/trunk/niflib/include/obj/NiNode.h"
#include "../NIFtools/trunk/niflib/include/obj/NiTriStrips.h"
#include "../NIFtools/trunk/niflib/include/obj/NiTriStripsData.h"
#include "../NIFtools/trunk/niflib/include/obj/NiTriShape.h"
#include "../NIFtools/trunk/niflib/include/obj/NiTriShapeData.h"
#include "../NIFtools/trunk/niflib/include/obj/BSSegmentedTriShape.h"
#include "../NIFtools/trunk/niflib/include/obj/NiEnvMappedTriShape.h"
#include "../NIFtools/trunk/niflib/include/obj/NiEnvMappedTriShapeData.h"

#include "../NIFtools/trunk/niflib/include/obj/NiProperty.h"
#include "../NIFtools/trunk/niflib/include/obj/NiExtraData.h"
#include "../NIFtools/trunk/niflib/include/obj/NiBinaryExtraData.h"
#include "../NIFtools/trunk/niflib/include/obj/NiStringExtraData.h"
#include "../NIFtools/trunk/niflib/include/obj/NiIntegerExtraData.h"

#include "../NIFtools/trunk/niflib/include/obj/NiSkinInstance.h"
#include "../NIFtools/trunk/niflib/include/obj/NiSkinData.h"
#include "../NIFtools/trunk/niflib/include/obj/NiSkinPartition.h"
#include "../NIFtools/trunk/niflib/include/obj/NiMorphData.h"

#include "../NIFtools/trunk/niflib/include/obj/NiAlphaProperty.h"
#include "../NIFtools/trunk/niflib/include/obj/NiMaterialProperty.h"
#include "../NIFtools/trunk/niflib/include/obj/NiTexturingProperty.h"
#include "../NIFtools/trunk/niflib/include/obj/NiSourceTexture.h"

#include "../NIFtools/trunk/niflib/include/obj/NiCollisionObject.h"
#include "../NIFtools/trunk/niflib/include/obj/NiControllerSequence.h"
#include "../NIFtools/trunk/niflib/include/obj/NiBoneLODController.h"
#include "../NIFtools/trunk/niflib/include/obj/NiTimeController.h"
#include "../NIFtools/trunk/niflib/include/obj/NiGeomMorpherController.h"
#include "../NIFtools/trunk/niflib/include/obj/NiMultiTargetTransformController.h"

#include "../NIFtools/trunk/niflib/include/obj/NiInterpolator.h"
#include "../NIFtools/trunk/niflib/include/obj/NiStringPalette.h"

#include "../NIFtools/trunk/niflib/include/obj/NiBoolData.h"
#include "../NIFtools/trunk/niflib/include/obj/NiTextKeyExtraData.h"
#include "../NIFtools/trunk/niflib/include/obj/NiVisData.h"
#include "../NIFtools/trunk/niflib/include/obj/NiPosData.h"
#include "../NIFtools/trunk/niflib/include/obj/NiFloatData.h"
#include "../NIFtools/trunk/niflib/include/obj/NiColorData.h"
#include "../NIFtools/trunk/niflib/include/obj/NiKeyframeData.h"

#include "../NIFtools/trunk/niflib/include/obj/NiPSysMeshEmitter.h"

#include "../NIFtools/trunk/niflib/include/obj/NiDefaultAVObjectPalette.h"

#include "../NIFtools/trunk/niflib/include/obj/BSXFlags.h"

#include "../NIFtools/trunk/niflib/include/obj/bhkCollisionObject.h"
#include "../NIFtools/trunk/niflib/include/obj/bhkWorldObject.h"
#include "../NIFtools/trunk/niflib/include/obj/bhkRigidBody.h"
#include "../NIFtools/trunk/niflib/include/obj/bhkRigidBodyT.h"
#include "../NIFtools/trunk/niflib/include/obj/bhkSerializable.h"
#include "../NIFtools/trunk/niflib/include/obj/bhkListShape.h"
#include "../NIFtools/trunk/niflib/include/obj/bhkBoxShape.h"
#include "../NIFtools/trunk/niflib/include/obj/bhkConvexListShape.h"
#include "../NIFtools/trunk/niflib/include/obj/bhkConvexVerticesShape.h"
#include "../NIFtools/trunk/niflib/include/obj/bhkConvexTransformShape.h"
#include "../NIFtools/trunk/niflib/include/obj/bhkNiTriStripsShape.h"
#include "../NIFtools/trunk/niflib/include/obj/bhkPackedNiTriStripsShape.h"
#include "../NIFtools/trunk/niflib/include/obj/hkPackedNiTriStripsData.h"
#include "../NIFtools/trunk/niflib/include/obj/bhkMoppBvTreeShape.h"

using namespace Niflib;

/* ---------------------------------------------------- */

const char *KeyName[] = {
  "",
  "Linear",
  "Quadric",
  "Tension Bias Continuity",
  "XYZ",
  ""
};

/*! The motion system. */
const char *MotionName[] = {
  /*MO_SYS_INVALID = 0		*/ "Invalid",
  /*MO_SYS_DYNAMIC = 1		*/ "Dynamic",
  /*MO_SYS_SPHERE = 2		*/ "Sphere",
  /*MO_SYS_SPHERE_INERTIA = 3	*/ "Sphere Inertia",
  /*MO_SYS_BOX = 4		*/ "Box",
  /*MO_SYS_BOX_STABILIZED = 5	*/ "Box Inertia",
  /*MO_SYS_KEYFRAMED = 6	*/ "Box Inertia",
  /*MO_SYS_FIXED = 7		*/ "Fixed",
  /*MO_SYS_THIN_BOX = 8		*/ "Thin Box",
  /*MO_SYS_CHARACTER = 9	*/ "Character"
};

/*! A material, used by havok shape objects. */
const char *MaterialName[] = {
  /*HAV_MAT_STONE = 0		*/ "Stone",
  /*HAV_MAT_CLOTH = 1		*/ "Cloth",
  /*HAV_MAT_DIRT = 2		*/ "Dirt",
  /*HAV_MAT_GLASS = 3		*/ "Glass",
  /*HAV_MAT_GRASS = 4		*/ "Grass",
  /*HAV_MAT_METAL = 5		*/ "Metal",
  /*HAV_MAT_ORGANIC = 6		*/ "Organic",
  /*HAV_MAT_SKIN = 7		*/ "Skin",
  /*HAV_MAT_WATER = 8		*/ "Water",
  /*HAV_MAT_WOOD = 9		*/ "Wood",
  /*HAV_MAT_HEAVY_STONE		*/ "Heavy Stone",
  /*HAV_MAT_HEAVY_METAL		*/ "Heavy Metal",
  /*HAV_MAT_HEAVY_WOOD		*/ "Heavy Wood",
  /*HAV_MAT_CHAIN		*/ "Chain",
  /*HAV_MAT_SNOW		*/ "Snow",
  /*HAV_MAT_STONE_STAIRS	*/ "Stone Stairs",
  /*HAV_MAT_CLOTH_STAIRS	*/ "Cloth Stairs",
  /*HAV_MAT_DIRT_STAIRS		*/ "Dirt Stairs",
  /*HAV_MAT_GLASS_STAIRS	*/ "Glass Stairs",
  /*HAV_MAT_GRASS_STAIRS	*/ "Grass Stairs",
  /*HAV_MAT_METAL_STAIRS	*/ "Metal Stairs",
  /*HAV_MAT_ORGANIC_STAIRS	*/ "Organic Stairs",
  /*HAV_MAT_SKIN_STAIRS		*/ "Skin Stairs",
  /*HAV_MAT_WATER_STAIRS	*/ "Water Stairs",
  /*HAV_MAT_WOOD_STAIRS		*/ "Wood Stairs",
  /*HAV_MAT_HEAVY_STONE_STAIRS	*/ "Heavy Stone Stairs",
  /*HAV_MAT_HEAVY_METAL_STAIRS	*/ "Heavy Metal Stairs",
  /*HAV_MAT_HEAVY_WOOD_STAIRS	*/ "Heavy Wood Stairs",
  /*HAV_MAT_CHAIN_STAIRS	*/ "Chain Stairs",
  /*HAV_MAT_SNOW_STAIRS		*/ "Snow Stairs",
  /*HAV_MAT_ELEVATOR		*/ "Elevator"
};

NiObjectRef FindRoot( vector<NiObjectRef> const & objects ) {
  //--Look for a NiNode that has no parents--//

  //Find the first NiObjectNET derived object
  NiAVObjectRef root;
  for (unsigned int i = 0; i < objects.size(); ++i) {
    root = DynamicCast<NiAVObject>(objects[i]);
    if ( root != NULL ) {
      break;
    }
  }

  //Make sure a node was found, if not return first node
  if ( root == NULL )
    return objects[0];

  //Move up the chain to the root node
  while ( root->GetParent() != NULL ) {
    root = StaticCast<NiAVObject>(root->GetParent());
  }

  return StaticCast<NiObject>(root);
}

#if   _MSC_VER == 1500
#define LIBDIR	"vc9"
#elif _MSC_VER == 1600
#define LIBDIR	"vc10"
#endif

#ifndef	NDEBUG
#ifdef	_WIN64
#pragma comment(lib,"../../../NIFtools/trunk/niflib/" LIBDIR "/lib/niflib_static_debug_x64")
#else
#pragma comment(lib,"../../../NIFtools/trunk/niflib/" LIBDIR "/lib/niflib_static_debug")
#endif
#else
#ifdef	_WIN64
#pragma comment(lib,"../../../NIFtools/trunk/niflib/" LIBDIR "/lib/niflib_static_x64")
#else
#pragma comment(lib,"../../../NIFtools/trunk/niflib/" LIBDIR "/lib/niflib_static")
#endif
#endif

int brokenfiles = 0;
int fixedfiles = 0;
int brokennodes = 0;
int meshdamages = 0;
int fixedmeshes = 0;
int fixedpaths = 0;
int modifiedmeshes = 0;
int modifiedtlines = 0;
int processedfiles = 0;
int orphanegmfiles = 0;
int orphantrifiles = 0;

vector<NiObjectRef> utilization;
vector<NiObjectRef> globalnlist;
vector<NiObjectRef> masterlist;
NiObjectRef masterroot;
NifInfo masterinfo;

string FindController(NiInterpolatorRef interp) {
  vector<NiObjectRef>::iterator walk;

  for (walk = masterlist.begin(); walk != masterlist.end(); walk++) {
    NiControllerSequenceRef ctrls = DynamicCast<NiControllerSequence>(*walk);
    if (ctrls) {
      /* uh, not pointer, intensive! */
      vector<ControllerLink> links = ctrls->GetControllerData();
      vector<ControllerLink>::iterator lnk;

      for (lnk = links.begin(); lnk != links.end(); lnk++) {
	if ((*lnk).interpolator == interp) {
	  NiStringPaletteRef spal = ctrls->GetStringPalette();
	  if (spal) {
	    string all, frg;

	    if ((*lnk).nodeNameOffset >= 0) {
	      frg = spal->GetSubStr((*lnk).nodeNameOffset);

	      if (frg != "") {
		all += " ";
		all += "\"";
		all += frg;
		all += "\"";
	      }
	    }

	    if ((*lnk).propertyTypeOffset >= 0) {
	      frg = spal->GetSubStr((*lnk).propertyTypeOffset);

	      if (frg != "") {
		all += ".";
		all += frg;
	      }
	    }

	    return all;
	  }

	  return "";
	}
      }
    }
  }

  return "";
}

void SubstituteRefs(NiObjectRef newref, NiObjectRef oldref) {
  vector<NiObjectRef>::iterator walk;

  /* substitute geometry-references */
  NiTriBasedGeomRef newgeo = DynamicCast<NiTriBasedGeom>(newref);
  NiTriBasedGeomRef oldgeo = DynamicCast<NiTriBasedGeom>(oldref);
  if (newgeo && oldgeo) {
    for (walk = masterlist.begin(); walk != masterlist.end(); walk++) {
      NiBoneLODControllerRef blod = DynamicCast<NiBoneLODController>(*walk);
      if (blod) {
	blod->ReplaceShapeInGroup(newgeo, oldgeo);
	continue;
      }

      NiPSysMeshEmitterRef meshem = DynamicCast<NiPSysMeshEmitter>(*walk);
      if (meshem) {
	meshem->ReplaceEmitterMesh(newgeo, oldgeo);
	continue;
      }
    }
  }

  /* substitute AVObject-references */
  NiAVObjectRef newav = DynamicCast<NiAVObject>(newref);
  NiAVObjectRef oldav = DynamicCast<NiAVObject>(oldref);
  if (newav && oldav) {
    for (walk = masterlist.begin(); walk != masterlist.end(); walk++) {
      NiDefaultAVObjectPaletteRef dav = DynamicCast<NiDefaultAVObjectPalette>(*walk);
      if (dav) {
	dav->ReplaceObj(newav, oldav);
	continue;
      }

      NiMultiTargetTransformControllerRef ctrl = DynamicCast<NiMultiTargetTransformController>(*walk);
      if (ctrl) {
	ctrl->ReplaceExtraTarget(newav, oldav);
	continue;
      }
    }
  }

  /* substitute Shape-references */
  NiSkinInstanceRef newsi = DynamicCast<NiSkinInstance>(newref);
  NiSkinInstanceRef oldsi = DynamicCast<NiSkinInstance>(oldref);
  if (newsi && oldsi) {
    for (walk = masterlist.begin(); walk != masterlist.end(); walk++) {
      NiGeometryRef geo = DynamicCast<NiGeometry>(*walk);
      if (geo) {
	if (geo->GetSkinInstance() == oldsi)
	  geo->SetSkinInstance(newsi);
	continue;
      }
    }
  }

  /* substitute Shape-references */
  bhkShapeRef newbs = DynamicCast<bhkShape>(newref);
  bhkShapeRef oldbs = DynamicCast<bhkShape>(oldref);
  if (newbs && oldbs) {
    for (walk = masterlist.begin(); walk != masterlist.end(); walk++) {
      bhkWorldObjectRef wld = DynamicCast<bhkWorldObject>(*walk);
      if (wld) {
	if (wld->GetShape() == oldbs)
	  wld->SetShape(newbs);
	continue;
      }

      bhkTransformShapeRef trn = DynamicCast<bhkTransformShape>(*walk);
      if (trn) {
	if (trn->GetShape() == oldbs)
	  trn->SetShape(newbs);
	continue;
      }

      bhkMoppBvTreeShapeRef mts = DynamicCast<bhkMoppBvTreeShape>(*walk);
      if (mts) {
	bhkPackedNiTriStripsShapeRef newpck = DynamicCast<bhkPackedNiTriStripsShape>(newbs);
	bhkPackedNiTriStripsShapeRef oldpck = DynamicCast<bhkPackedNiTriStripsShape>(oldbs);

	if (!newpck) {
	  /* replace the entire MOPP-Node by this shape
	   * this is some primitive now
	   */
	}
	else if (oldpck) {
	  /* recalculate MOPP-Code */
	}

	/* TODO: recalculate MOPP-Code */
//	if (mts->GetShape() == oldbs)
//	  mts->SetShape(newbs);
//	continue;
      }
    }
  }

  /* remove from the global list */
  for (walk = masterlist.begin(); walk != masterlist.end(); walk++) {
    if (*walk == oldref)
      *walk = newref;
  }
}

/* ---------------------------------------------------- */

#include "nifopt-geometry.C"
//#include "nifopt-nmbaker.C"
#include "nifopt-tri.C"
#include "nifopt-egm.C"

/* ---------------------------------------------------- */

char nodebarchar = '-';
string nodebarref;
MotionSystem nodemotion;
OblivionLayer nodelayer;
CycleType nodecycle;
bool nodemopped = false;

/* BoxShape is AABB, Axis Aligned Bounding Box */
/* TransformShape is a orientation-wrapper, can make BoxShape an OBB, Oriented Bounding Box */
/* ConvexVerticesShape can be calculated by QHull */
/* MoppBvTreeShape is Memory Optimized Partial Polytope bounding volume tree shape */

//bhkShape.bhkSphereRepShape.bhkConvexShape.bhkBoxShape
//bhkShape.bhkSphereRepShape.bhkConvexShape.bhkCapsuleShape
//bhkShape.bhkSphereRepShape.bhkConvexShape.bhkConvexVerticesShape
//bhkShape.bhkSphereRepShape.bhkConvexShape.bhkSphereShape
//bhkShape.bhkSphereRepShape.bhkMultiSphereShape
//bhkShape.bhkConvexListShape (collection)
//bhkShape.bhkShapeCollection.bhkListShape (collection)
//bhkShape.bhkTransformShape.bhkConvexTransformShape
//bhkShape.bhkBvTreeShape.bhkMoppBvTreeShape

string ConvertNode(bhkNiTriStripsShapeRef node, NiNodeRef parent, NiObjectRef &subst) {
#define TRISTRIPSSHAPE_FACTOR	7.0f
#define BOXSHAPE_TOLERANCE	0.20f	// must be way up sadly, was 0.05f
#define HULLSHAPE_TOLERANCE	0.20f	// must be way up sadly, was 0.05f
  int sd = node->GetNumStripsData();
  /* no need for collision-node */
  if (!sd)
    throw runtime_error("No Strips found.");

  vector<bhkShapeRef> nnodes;
  vector<bhkConvexShapeRef> cnodes;
  vector<NiTriStripsDataRef> nstrips;

  bool bar = false;
  for (int ss = 0; ss < sd; ss++) {
    NiGeometryDataRef raw = node->GetStripsData(ss);
    NiTriStripsDataRef data = DynamicCast<NiTriStripsData>(raw);

    /* convert strips to primitives automatically */
    int vc = data->GetVertexCount();
    unsigned short sc = data->GetStripCount();

    vector<Niflib::Vector3> v = data->GetVertices();
    vector<Niflib::Triangle> f = data->GetTriangles();

    int fc = (int)f.size();

    size_t pts = 0;
    for (unsigned short sl = 0; sl < sc; sl++)
      pts += data->GetStrip(sl).size();

    /* all points of the corners found && no points inside the box */
    Niflib::Matrix44 boxtransform;
    Niflib::Vector3 boxhalfextends;
    bool identity = false, isbox = false, isconvex = false;

    /* there can't be more planes than faces */
    vector<Niflib::Vector4> p(fc);
    int pc = fc;

    CalcHullPolyhedron(vc, reinterpret_cast<hvkPoint3 *>(&v[0]),
		       fc, reinterpret_cast<hvkTriangle *>(&f[0]),
		       TRISTRIPSSHAPE_FACTOR, HULLSHAPE_TOLERANCE,
		       &pc, reinterpret_cast<hvkPoint4 *>(&p[0]), &isconvex);

    CalcOBBPolyhedron(vc, reinterpret_cast<hvkPoint3 *>(&v[0]),
		      fc, reinterpret_cast<hvkTriangle *>(&f[0]),
		      TRISTRIPSSHAPE_FACTOR, BOXSHAPE_TOLERANCE,
		      reinterpret_cast<hvkMatrix44 *>(&boxtransform), &identity,
		      reinterpret_cast<hvkPoint3 *>(&boxhalfextends), &isbox);

    /* the box can also be produced by this case:
     * G:\Oblivion\Data-Unpacked\Oblivion - Meshes\meshes\architecture\anvil\anvilaltar01.nif
     *
     * all points lie on the box, so the convexity check has to be made as well
     */
    if (isconvex) {
      if (isbox) {
	bhkBoxShapeRef box = new bhkBoxShape;

	/* spit out some info */
	if (!bar) {
	  barprintf(stdout, nodebarchar, "%s collider", nodebarref.data());
	  nfoprintf(stdout, "converting:\n"); nodebarchar = '-';
	  nfoprintf(stdout, " Type             : %s\n", nodemotion == MotionSystem::MO_SYS_FIXED ? "Static" : "Dynamic");
	  nfoprintf(stdout, " Material         : %s\n", MaterialName[node->GetMaterial()]);

	  bar = true;
	}

	/* rescale extends and translation */
	if (TRISTRIPSSHAPE_FACTOR != 1.0f) {
	  boxhalfextends.x = boxhalfextends.x / TRISTRIPSSHAPE_FACTOR;
	  boxhalfextends.y = boxhalfextends.y / TRISTRIPSSHAPE_FACTOR;
	  boxhalfextends.z = boxhalfextends.z / TRISTRIPSSHAPE_FACTOR;

	  /* row-major */
	  boxtransform[0][3] = boxtransform[0][3] / TRISTRIPSSHAPE_FACTOR;
	  boxtransform[1][3] = boxtransform[1][3] / TRISTRIPSSHAPE_FACTOR;
	  boxtransform[2][3] = boxtransform[2][3] / TRISTRIPSSHAPE_FACTOR;
	  boxtransform[3][3] = boxtransform[3][3] / TRISTRIPSSHAPE_FACTOR;
	}

	box->SetDimensions(boxhalfextends);
	box->SetMaterial(node->GetMaterial());
	box->SetRadius(0.1f);

	/* no transform/rotation */
	if (identity) {
	  nfoprintf(stdout, " Collision Shape %d: %s to %s\n", ss, "Strip(s)", "AABB");

	  /* register box */
	  nnodes.push_back(DynamicCast<bhkShape>(box));
	  cnodes.push_back(DynamicCast<bhkConvexShape>(box));
	}
	else {
	  bhkConvexTransformShapeRef trs = new bhkConvexTransformShape;

	  if ((boxtransform[0][0] == 1.0f) &&
	      (boxtransform[1][1] == 1.0f) &&
	      (boxtransform[2][2] == 1.0f)) {
	    nfoprintf(stdout, " Collision Shape %d: %s to %s\n", ss, "Strip(s)", "AABB"); }
	  else {
	    nfoprintf(stdout, " Collision Shape %d: %s to %s\n", ss, "Strip(s)", "OBB"); }

//	  trs->AddChild(DynamicCast<NiAVObject>(box));
	  trs->SetShape(box);
	  trs->SetMaterial(box->GetMaterial());
	  trs->SetTransform(boxtransform);

	  /* register transform+box */
	  nnodes.push_back(DynamicCast<bhkShape>(trs));
	  cnodes.push_back(DynamicCast<bhkConvexShape>(trs));
	}

	nfoprintf(stdout, "  Strips          : %6d to %6d (%.4f%%)\n", sc, 0, 0.0f);
	nfoprintf(stdout, "  Points          : %6d to %6d (%.4f%%), space\n", (pts         ), 8, 100.0f * 8 / (pts         ));
	nfoprintf(stdout, "  Faces           : %6d to %6d (%.4f%%), perf.\n", (pts - sc * 2), 6, 100.0f * 6 / (pts - sc * 2));
      }
      /* existing example: meshes\armor\blades\akavarishield.nif
       * correct exampe: MidasSpells\meshes\architecture\midasastralplane\midasastralplatform01.nif
       * incorrect exampe: MidasSpells\meshes\architecture\midasastralplane\midasastraltilesteps01.nif
       *
       * the last one has all points on the hull, but not all edges on hull-edges
       */
      else {
	/* TODO: sphere-check:
	 * a) we got the OBB, it's center is the convex-hull-center
	 * b) we need the plane-equations with the OBB-center as origin
	 * c) if all points are within a sphere-radius tolerance it's a sphere
	 * d) at what resolution do we decide a polyhedron is perfectly spherical?
	 * e) if all plane-normal anyway meet in the origin it's another kind of
	 *    platonic primitive (cylinder fe.)
	 */
	bhkConvexVerticesShapeRef hull = new bhkConvexVerticesShape;

	/* spit out some info */
	if (!bar) {
	  barprintf(stdout, nodebarchar, "%s collider", nodebarref.data());
	  nfoprintf(stdout, "converting:\n"); nodebarchar = '-';
	  nfoprintf(stdout, " Type             : %s\n", nodemotion == MotionSystem::MO_SYS_FIXED ? "Static" : "Dynamic");
	  nfoprintf(stdout, " Material         : %s\n", MaterialName[node->GetMaterial()]);

	  bar = true;
	}

	/* shorten to real size */
	p.resize(pc);

	/* rescale convex shape */
	if (TRISTRIPSSHAPE_FACTOR != 1.0f) {
	  vector<Niflib::Vector3>::iterator itv;
	  for (itv = v.begin(); itv != v.end(); itv++) {
	    (*itv).x = (*itv).x / TRISTRIPSSHAPE_FACTOR;
	    (*itv).y = (*itv).y / TRISTRIPSSHAPE_FACTOR;
	    (*itv).z = (*itv).z / TRISTRIPSSHAPE_FACTOR;
	  }

	  vector<Niflib::Vector4>::iterator itp;
	  for (itp = p.begin(); itp != p.end(); itp++) {
	    (*itp).w = (*itp).w / TRISTRIPSSHAPE_FACTOR;
	  }
	}

//	hull->SetDimensions(boxhalfextends);
	hull->SetMaterial(node->GetMaterial());
	hull->SetRadius(0.1f);
	hull->SetVertices(v);
	hull->SetNormalsAndDist(p);

	/* no transform/rotation */
	if (1/*identity*/) {
	  nfoprintf(stdout, " Collision Shape %d: %s to %s\n", ss, "Strip", "Hull");

	  /* register box */
	  nnodes.push_back(DynamicCast<bhkShape>(hull));
	  cnodes.push_back(DynamicCast<bhkConvexShape>(hull));
	}
	else {
	  bhkConvexTransformShapeRef trs = new bhkConvexTransformShape;

	  nfoprintf(stdout, " Collision Shape %d: %s to %s\n", ss, "Strip", "Hull");

//	  trs->AddChild(DynamicCast<NiAVObject>(box));
	  trs->SetShape(hull);
	  trs->SetMaterial(hull->GetMaterial());
	  trs->SetTransform(boxtransform);

	  /* register transform+box */
	  nnodes.push_back(DynamicCast<bhkShape>(trs));
	  cnodes.push_back(DynamicCast<bhkConvexShape>(trs));
	}

	nfoprintf(stdout, "  Strips          : %6d to %6d (%.4f%%)\n", sc, 0, 0.0f);
	nfoprintf(stdout, "  Points          : %6d to %6d (%.4f%%), space\n", (pts         ), vc, 100.0f * vc / (pts         ));
	nfoprintf(stdout, "  Faces to Planes : %6d to %6d (%.4f%%), perf.\n", (pts - sc * 2), pc, 100.0f * pc / (pts - sc * 2));
      }
    }
    else
      nstrips.push_back(data);
  }

  if (!nnodes.size()) {
  }
  else if ((nnodes.size() + nstrips.size()) > 1) {
    /* can be bhkConvexListShape as well */
    bhkShapeRef lnode = NULL;

    /* original node becomes part of the list */
    if (nstrips.size()) {
      bhkListShapeRef nnode = new bhkListShape;
      nfoprintf(stdout, " Collision Shapes : %s to %s\n", "Strips", "List");

      node->SetNumStripsData((int)nstrips.size());
      for (int s = 0; s < (int)nstrips.size(); s++)
        node->SetStripsData(s, nstrips[s]);
      nnodes.push_back(DynamicCast<bhkShape>(node));

      nnode->SetSubShapes(nnodes);
      nnode->SetMaterial(node->GetMaterial());

      lnode = nnode;
    }
    else {
      bhkConvexListShapeRef nnode = new bhkConvexListShape;
      nfoprintf(stdout, " Collision Shapes : %s to %s\n", "Strips", "Primitive List");

      nnode->SetSubShapes(cnodes);
      nnode->SetMaterial(node->GetMaterial());

      lnode = nnode;
    }

    /* replace references everywhere */
    SubstituteRefs(DynamicCast<NiObject>(lnode), DynamicCast<NiObject>(node));

    /* replace node (without having the ability directly) */
    subst = DynamicCast<NiObject>(lnode);

    return lnode->GetType().GetTypeName();
  }
  else if (nnodes.size()) {
    bhkShapeRef nnode = nnodes[0];

    nfoprintf(stdout, " Collision Shapes : %s to %s\n", "Strips", "Primitive");

    /* replace references everywhere */
    SubstituteRefs(DynamicCast<NiObject>(nnode), DynamicCast<NiObject>(node));

    /* replace node (without having the ability directly) */
    subst = DynamicCast<NiObject>(nnode);

    return nnode->GetType().GetTypeName();
  }

  /* check supported version-number */
  if ((masterinfo.userVersion >= 10) &&
      (masterinfo.userVersion <= 11) &&
      ((masterinfo.version == 0x14000004) ||
       (masterinfo.version == 0x14000005) ||
       (masterinfo.version == 0x14000007))) {

    /* not a box, not a convex hull, make it a MOPP
     * we don't need to know this wasn't a MOPP in the first
     * place ...
     */
    if (!nodemopped)
    if (((nodemotion == MO_SYS_FIXED    ) && (nodelayer != OL_CLUTTER    )) ||
	((nodemotion == MO_SYS_KEYFRAMED) && (nodelayer == OL_ANIM_STATIC))) {  // still applies?
      bhkMoppBvTreeShapeRef nnode = new bhkMoppBvTreeShape;
      bhkPackedNiTriStripsShapeRef pnode = new bhkPackedNiTriStripsShape;
      hkPackedNiTriStripsDataRef pdata = new hkPackedNiTriStripsData;
      vector<Niflib::OblivionSubShape> os(1);

      vector<Niflib::Vector3> _v;
      vector<Niflib::hkTriangle> _w;
      vector<Niflib::Triangle> _f;

      /* spit out some info */
      if (!bar) {
	barprintf(stdout, nodebarchar, "%s collider", nodebarref.data());
	nfoprintf(stdout, "converting:\n"); nodebarchar = '-';
	nfoprintf(stdout, " Type             : %s\n", nodemotion == MotionSystem::MO_SYS_FIXED ? "Static" : "Dynamic");
	nfoprintf(stdout, " Material         : %s\n", MaterialName[node->GetMaterial()]);

	bar = true;
      }

      int offset = 0;
      for (int ss = 0; ss < sd; ss++) {
	NiGeometryDataRef raw = node->GetStripsData(ss);
	NiTriStripsDataRef data = DynamicCast<NiTriStripsData>(raw);

	/* convert multiple strip-datas to one consecusive array */
	int vc = data->GetVertexCount();
  //    unsigned short sc = data->GetStripCount();

	vector<Niflib::Vector3> v = data->GetVertices();
	vector<Niflib::Triangle> f = data->GetTriangles();

	int fc = (int)f.size();

	vector<Niflib::Vector3>::iterator itv;
	for (itv = v.begin(); itv != v.end(); itv++)
	  _v.push_back((*itv) / TRISTRIPSSHAPE_FACTOR);

	vector<Niflib::Triangle>::iterator itf;
	for (itf = f.begin(); itf != f.end(); itf++) {
	  Point3d v1; v1.x = v[(*itf).v1].x; v1.y = v[(*itf).v1].y; v1.z = v[(*itf).v1].z;
	  Point3d v2; v2.x = v[(*itf).v2].x; v2.y = v[(*itf).v2].y; v2.z = v[(*itf).v2].z;
	  Point3d v3; v3.x = v[(*itf).v3].x; v3.y = v[(*itf).v3].y; v3.z = v[(*itf).v3].z;
	  Point3d n;
	  float l = 2.0f * narea(v1, v2, v3, n);
	  assert(l >= 0.0f); /* ensure CCW */

	  Niflib::hkTriangle hkt;

	  hkt.triangle.v1 = (*itf).v1 + offset;
	  hkt.triangle.v2 = (*itf).v2 + offset;
	  hkt.triangle.v3 = (*itf).v3 + offset;

	  hkt.normal.x = n.x / l;
	  hkt.normal.y = n.y / l;
	  hkt.normal.z = n.z / l;

	  assert((hkt.triangle.v1 >= offset) && (hkt.triangle.v1 < (offset + vc)));
	  assert((hkt.triangle.v2 >= offset) && (hkt.triangle.v2 < (offset + vc)));
	  assert((hkt.triangle.v3 >= offset) && (hkt.triangle.v3 < (offset + vc)));

	  _w.push_back(hkt);
	  _f.push_back(hkt.triangle);
	}

	offset += vc;
      }

      /* single sub-shape MOPP */
      os[0].layer = OL_STATIC;//node->GetOblivionLayer(0);
      os[0].colFilter = 0;//node->GetOblivionFilter(0);
      os[0].unknownShort = 0;
      os[0].numVertices = offset;
      os[0].material = node->GetMaterial();

      vector<int> vb(os.size());
      vector<Niflib::byte> vm(os.size());
      vector<short> vf(os.size());
      int sc = (int)os.size();
      for (int ss = 0; ss < sc; ss++) {
	vb[ss] = os[ss].numVertices;
	vm[ss] = os[ss].material;
	vf[ss] = os[ss].colFilter;  // may be used ...
      }

      int vc = (int)_v.size();
      int wc = (int)_w.size();
      int fc = (int)_f.size();

      vector<short> wcode;
      int wlen = 0;

      vector<Niflib::byte> ncode;
      Vector3 norigin;
      float nscale;
      int nlen = 0;

      /* MOPP may need recalculation:
       * removal of faces:    no, MOPP takes care of degenerate faces
       * removal of vertices: no, MOPP takes care of identical vertices
       * rem/add of shapes:   yes, the only case where it'd be needed
       */
      wlen = wc;
      nlen = GenerateMoppCodeWithSubshapes(
	sc, &vb[0],
	sc, reinterpret_cast<hvkByte *>(&vm[0]),
	vc, reinterpret_cast<hvkPoint3 *>(&_v[0]),
	fc, reinterpret_cast<hvkTriangle *>(&_f[0])
      );

      if (nlen > 0) {
	wcode.resize(wlen);
	ncode.resize(nlen);

	if (RetrieveMoppCode(nlen, reinterpret_cast<hvkByte *>(&ncode[0])) > 0) {
	  if (RetrieveMoppWelding(wlen, reinterpret_cast<hvkShort *>(&wcode[0])) > 0) {
	    /* transfer welding-info */
	    vector<Niflib::hkTriangle>::iterator itw; int pos;
	    for (pos = 0, itw = _w.begin(); itw != _w.end(); itw++, pos++)
	      (*itw).weldingInfo = wcode[pos];
	  }
	  else {
	    errprintf(stdout, "failed to generate MOPP\n");
	    return node->GetType().GetTypeName();
	  }

	  RetrieveMoppScale (&nscale);
	  RetrieveMoppOrigin(&norigin[0]);
	}
	else {
	  errprintf(stdout, "failed to generate MOPP\n");
	  return node->GetType().GetTypeName();
	}

	nnode->SetMoppCode(ncode);
	nnode->SetMoppScale(nscale);
	nnode->SetMoppOrigin(norigin);
      }
      else {
	errprintf(stdout, "failed to generate MOPP\n");
	return node->GetType().GetTypeName();
      }

      nfoprintf(stdout, " Collision Shapes : %s to %s\n", "Strips", "PackedStrips + MOPP");
      nfoprintf(stdout, " MOPP Code Size   : %6d\n", nlen);

      // 0x14000007
      if (masterinfo.version == 0x14000007)
	pdata->SetSubShapes(os);
      pdata->SetVertices(_v);
      pdata->SetTriangles(_w);

      // 0x14000004, 0x14000005
      if ((masterinfo.version == 0x14000004) ||
	  (masterinfo.version == 0x14000005))
	pnode->SetSubShapes(os);
      pnode->SetData(pdata);
      pnode->SetScale(node->GetScale());

      nnode->SetMaterial(node->GetMaterial());
      nnode->SetShape(pnode);

      /* replace references everywhere */
      SubstituteRefs(DynamicCast<NiObject>(nnode), DynamicCast<NiObject>(node));

      /* replace node (without having the ability directly) */
      subst = DynamicCast<NiObject>(nnode);

      return nnode->GetType().GetTypeName();
    }
  }

  return node->GetType().GetTypeName();
}

string ConvertNode(bhkPackedNiTriStripsShapeRef node, NiNodeRef parent, NiObjectRef &subst) {
#define PACKEDTRISTRIPSSHAPE_FACTOR 1.0f
  /* check supported version-number */
  if ((masterinfo.userVersion >= 10) &&
      (masterinfo.userVersion <= 11) &&
      ((masterinfo.version == 0x14000004) ||
       (masterinfo.version == 0x14000005) ||
       (masterinfo.version == 0x14000007))) {
  }
  else {
  }

  hkPackedNiTriStripsDataRef data = node->GetData();
  vector<Niflib::OblivionSubShape> os; bool osdta;

  // 0x14000007
  os = data->GetSubShapes(), osdta = true;
  if (!os.size())
    // 0x14000004, 0x14000005
    os = node->GetSubShapes(), osdta = false;

  int sc = (int)os.size();
  int vs = 0;

  /* distinct number of materials */
  set<Niflib::HavokMaterial> om;
  for (int ss = 0; ss < sc; ss++) {
    om.insert(os[ss].material);
    vs += os[ss].numVertices;
  }

  int mc = (int)om.size();
  /* no need for collision-node */
  if (!sc || !mc)
    throw runtime_error("No SubShapes found.");

  /* collapse PackedTriStrips into TriStrips */
  if ((sc == 1) && (mc == 1)) {
    bhkNiTriStripsShapeRef nnode = new bhkNiTriStripsShape;
    NiTriStripsDataRef ndata = new NiTriStripsData;

    /* spit out some info */
//  barprintf(stdout, nodebarchar, "%s collider", nodebarref.data());
//  nfoprintf(stdout, "converting:\n"); nodebarchar = '-';

    /* transfer everything over */
    nnode->SetScale(node->GetScale());
    nnode->SetMaterial(os[0].material);
    nnode->SetNumDataLayers(sc);
    for (int ss = 0; ss < sc; ss++) {
      nnode->SetOblivionFilter(ss, os[ss].colFilter);
      nnode->SetOblivionLayer(ss, os[ss].layer);
    }

    int vc = data->GetVertexCount();

    vector<Niflib::Vector3> v = data->GetVertices();
    vector<Niflib::Triangle> f = data->GetTriangles();
//  vector<Niflib::Vector3> n = data->GetNormals();
    /* face-normals! */

    /* weird factor */
    for (int vs = 0; vs < vc; vs++)
      v[vs] *= TRISTRIPSSHAPE_FACTOR / PACKEDTRISTRIPSSHAPE_FACTOR;

    ndata->SetVertices(v);
    ndata->SetTriangles(f);
//  ndata->SetNormals(n);
    /* vertex-normals! */

    nnode->SetNumStripsData(1);
    nnode->SetStripsData(0, ndata);

    /* convert ONLY if the shape is a primitive or forms a convex hull
     * and then to a ConvexVerticesShape
     */
    NiObjectRef nsubst = DynamicCast<NiObject>(nnode);
    string type = ConvertNode(nnode, NULL, nsubst);
    if ((type != "bhkNiTriStripsShape") &&
        (type != "bhkPackedNiTriStripsShape") &&
        (type != "bhkMoppBvTreeShape")) {
//    nfoprintf(stdout, " Collision Shapes : %s to %s\n", "PackedStrips", "Strips");

      /* replace references everywhere */
      SubstituteRefs(DynamicCast<NiObject>(nsubst), DynamicCast<NiObject>(node));

      /* replace node (without having the ability directly) */
      subst = DynamicCast<NiObject>(nsubst);

      return type;
    }
    /* revert the change and put the original object back */
    else {
      SubstituteRefs(DynamicCast<NiObject>(node), DynamicCast<NiObject>(nsubst));
    }

    /* failure, Packed inside a MOPP is always faster */
  }
  /* TODO: verify it's not multiple "objects" */
  else if (mc == 1) {
  }

  return node->GetType().GetTypeName();
}

string ConvertNode(bhkMoppBvTreeShapeRef node, NiNodeRef parent, NiObjectRef &subst) {
  assert((nodemotion == MO_SYS_FIXED) || (nodemotion == MO_SYS_KEYFRAMED));
  nodemopped = true;

  /* check supported version-number */
  if ((masterinfo.userVersion >= 10) &&
      (masterinfo.userVersion <= 11) &&
      ((masterinfo.version == 0x14000004) ||
       (masterinfo.version == 0x14000005) ||
       (masterinfo.version == 0x14000007))) {
    bhkShapeRef raw = node->GetShape();
    bhkPackedNiTriStripsShapeRef pack = DynamicCast<bhkPackedNiTriStripsShape>(raw);
    bhkNiTriStripsShapeRef unpack = DynamicCast<bhkNiTriStripsShape>(raw);
    NiObjectRef sobj = DynamicCast<NiObject>(raw);

    /* NOTE: a MOPP is a "bounding volume tree", it's basically a hierarchy
     *       of convex hulls, so whenever we have a concave hull MOPP is
     *       speeding up the collisions
     * NOTE: MOPPs are exclusively for STATIC objects: "large static geometry"
     *       Static objects are in bhkRigidBodyT nodes and identified by
     *       motionSystem == MO_SYS_FIXED
     */

    /* bhkNiTriStripsShape in bhkMoppBvTreeShape:
     *
     *  DLCShiveringIsles - Meshes\meshes\dungeons\root\interior\misc\gnarlspawner.nif
     */
    if (unpack) {
      /* MOPP is gone */
      string type = ConvertNode(unpack, NULL, sobj);
      if (type != "bhkNiTriStripsShape") {
	addnote(" Dropped MOPP-Node\n");

	/* replace references everywhere */
	SubstituteRefs(DynamicCast<NiObject>(sobj), DynamicCast<NiObject>(node));

	/* replace node (without having the ability directly) */
	subst = DynamicCast<NiObject>(sobj);

	nodemopped = false;
	return sobj->GetType().GetTypeName();
      }
    }
    else if (pack) {
      /* MOPP is gone */
      string type = ConvertNode(pack, NULL, sobj);
      if (type != "bhkPackedNiTriStripsShape") {
	addnote(" Dropped MOPP-Node\n");

	/* replace references everywhere */
	SubstituteRefs(DynamicCast<NiObject>(sobj), DynamicCast<NiObject>(node));

	/* replace node (without having the ability directly) */
	subst = DynamicCast<NiObject>(sobj);

	nodemopped = false;
	return sobj->GetType().GetTypeName();
      }
    }
  }

  nodemopped = false;
  return node->GetType().GetTypeName();
}

string ConvertNode(NiTriStripsRef node, NiNodeRef parent, NiObjectRef &subst) {
  NiGeometryDataRef raw = node->GetData();
  NiTriStripsDataRef data = DynamicCast<NiTriStripsData>(raw);

  /* doesn't mean it's not there, just broken */
  if (!data) {
    brokennodes++;
    if (!raw)
      addnote(" No mesh attached, the file needs some revision.\n");
    else
      addnote(" The attached mesh is of some illegal type, the file needs some revision!\n");

    return "";
  }

  /* spit out some info */
  barprintf(stdout, nodebarchar, DynamicCast<NiObjectNET>(node)->GetName().data());
  nfoprintf(stdout, "converting:\n"); nodebarchar = '-';

  NiTriShapeRef nnode = new NiTriShape;
  NiTriShapeDataRef ndata = new NiTriShapeData;

  NiNodeRef prnt = node->GetParent();

  /* copy over all things */
  nnode->SetActiveMaterial(node->GetActiveMaterial());
//nnode->SetBoneWeights(node->GetBoneWeights());
  if (node->HasBoundingBox())
    nnode->SetBoundingBox(node->GetBoundingBox());
  nnode->SetFlags(node->GetFlags());
  nnode->SetLocalRotation(node->GetLocalRotation());
  nnode->SetLocalScale(node->GetLocalScale());
  nnode->SetLocalTransform(node->GetLocalTransform());
  nnode->SetName(node->GetName());
  nnode->SetShader(node->GetShader());
  nnode->SetVelocity(node->GetVelocity());
  nnode->SetVisibility(node->GetVisibility());

  vector< Ref<NiProperty> > prps = node->GetProperties();
  vector< Ref<NiProperty> >::iterator pi;
  for (pi = prps.begin(); pi != prps.end(); pi++) {
    /* prevent double deallocation */
    node->RemoveProperty(*pi);
    nnode->AddProperty(*pi);
  }

//ndata->SetBound(data->GetBound());
  ndata->SetConsistencyFlags(data->GetConsistencyFlags());
  ndata->SetHasNormals(data->GetHasNormals());
  ndata->SetTspaceFlag(data->GetTspaceFlag());
  ndata->SetUVSetCount(data->GetUVSetCount());

  /* convert strips to lists automatically */
  int vc = data->GetVertexCount();
  unsigned short sc = data->GetStripCount();

  vector<Niflib::Vector3> v = data->GetVertices();
  vector<Niflib::Color4> c = data->GetColors();
  vector<Niflib::Triangle> f = data->GetTriangles();

  ndata->SetVertices(v);
  ndata->SetVertexColors(c);
  ndata->SetTriangles(f);

  /* walk all sets, UV before tangent-space! */
  short uvc = data->GetUVSetCount();
  ndata->SetUVSetCount(uvc);
  for (int uvs = 0; uvs < uvc; uvs++)
    ndata->SetUVSet(uvs, data->GetUVSet(uvs));

  /* inaccessible map<>, we can rev-eng. it this way: */
  map<int, int> im; int mp, cn = 0;
  while ((mp = data->GetUVSetIndex(cn)) != -1)
    im[cn] = mp;
  ndata->SetUVSetMap(im);

  /* set TNB last */
  ndata->SetNormals(data->GetNormals());
  ndata->SetTangents(data->GetTangents());
  ndata->SetBinormals(data->GetBinormals());

  /* look if we also can convert a possible tri/egm now
   *
   * tri/egm files contain the faces exactly the same
   * way as the nif, if they are TriStrips, then there
   * is a complete face for each face in the strips in
   * the tri/egm
   *
   * if we collapse a strip to a (indexed) triangle-list
   * we have to mirror that in the tri/egm
   */
  size_t pts = 0;
  size_t idx = f.size();
  vector< vector<unsigned short> > s(sc);
  for (unsigned short ss = 0; ss < sc; ss++) {
    s[ss] = data->GetStrip(ss);
    pts += s[ss].size();
  }

  nfoprintf(stdout, " Strips           : %6d to %6d (%.4f%%)\n", sc, 0, 0.0f);
  nfoprintf(stdout, " Points           : %6d to %6d (%.4f%%), space\n", (pts         ), (idx * 3), 100.0f * (idx * 3) / (pts         ));
  nfoprintf(stdout, " Faces            : %6d to %6d (%.4f%%), perf.\n", (pts - sc * 2), (idx * 1), 100.0f * (idx * 1) / (pts - sc * 2));

  convert_tri(vc, sc, v, s);
  convert_egm(vc, sc, v, s);

  /* finally set the data */
  nnode->SetData(ndata);

  /* transfer extra data */
  list< Ref<NiExtraData> > x = node->GetExtraData();
  list< Ref<NiExtraData> >::iterator xd;
  for (xd = x.begin(); xd != x.end(); xd++) {
    /* prevent double deallocation */
    node->RemoveExtraData(*xd);
    nnode->AddExtraData(*xd);
  }

  /* transfer controllers */
  list< Ref<NiTimeController> > o = node->GetControllers();
  list< Ref<NiTimeController> >::iterator oi;
  for (oi = o.begin(); oi != o.end(); oi++) {
    /* prevent double deallocation */
    nnode->RemoveController(*oi);
    nnode->AddController(*oi);
  }

  /* we have to add this as a child before doing the skin-binding
   * to have it have the same ancestor as the bone nodes!
   */
  if (prnt)
    prnt->AddChild(DynamicCast<NiAVObject>(nnode));

  /* is this a bug???
   * "Oblivion - Meshes - BSA/meshes/creatures/willothewisp/skeleton.nif"
   */
  else if (parent)
    parent->AddChild(DynamicCast<NiAVObject>(nnode));

  /* move over bone weights */
  NiSkinInstanceRef skn = node->GetSkinInstance();
  if (skn) {
    unsigned int bc = skn->GetBoneCount();
    vector< Ref<NiNode> > bn = skn->GetBones();

    nnode->BindSkin(bn);

    /* transfer everything else */
    NiSkinInstanceRef nskn = nnode->GetSkinInstance();

//  nskn->SetSkeletonRoot(skn->GetSkeletonRoot());
    nskn->SetSkinData(skn->GetSkinData());

    NiSkinPartitionRef nprt = skn->GetSkinPartition();
    if (nprt) {
      int pc = nprt->GetNumPartitions();
      for (int ps = 0; ps < pc; ps++) {
	/* convert strips to lists automatically */
	unsigned short vc = nprt->GetNumVertices(ps);
	unsigned short sc = nprt->GetStripCount(ps);

	if (sc > 0) {
	  /* spit out some info */
	  barprintf(stdout, '.', "%s %d", "skinpartition", ps);
	  nfoprintf(stdout, "converting:\n");

	  /* convert strips to lists automatically */
	  vector<Niflib::Triangle> pt = nprt->GetTriangles(ps);

	  size_t pts = 0;
	  size_t idx = pt.size();
	  vector< vector<unsigned short> > s(sc);
	  for (unsigned short ss = 0; ss < sc; ss++) {
	    s[ss] = nprt->GetStrip(ps, ss);
	    pts += s[ss].size();
	  }

	  nfoprintf(stdout, " Strips           : %6d to %6d (%.4f%%)\n", sc, 0, 0.0f);
	  nfoprintf(stdout, " Points           : %6d to %6d (%.4f%%), space\n", (pts         ), (idx * 3), 100.0f * (idx * 3) / (pts         ));
	  nfoprintf(stdout, " Faces            : %6d to %6d (%.4f%%), perf.\n", (pts - sc * 2), (idx * 1), 100.0f * (idx * 1) / (pts - sc * 2));

	  /* remove strips, add lists */
	  nprt->SetStripCount(ps, 0);
	  nprt->SetTriangles(ps, pt);
	}
      }

      /* prevent double deallocation */
      nskn->SetSkinPartition(nprt);
      skn->SetSkinPartition(NULL);
    }

    /* prevent double deallocation */
    skn->SetSkinData(NULL);

    /* replace references everywhere */
    SubstituteRefs(DynamicCast<NiObject>(nskn), DynamicCast<NiObject>(skn));
  }

  nnode->SetCollisionMode(node->GetCollisionMode());
  NiCollisionObjectRef col = node->GetCollisionObject();
  if (col) {
    /* prevent double deallocation */
    node->SetCollisionObject(NULL);
    nnode->SetCollisionObject(col);
  }

  /* after that (preserving ancestors) we can remove the old
   * node
   */
  if (prnt)
    prnt->RemoveChild(DynamicCast<NiAVObject>(node));

  /* is this a bug???
   * "Oblivion - Meshes - BSA/meshes/creatures/willothewisp/skeleton.nif"
   */
  else if (parent)
    parent->RemoveChild(DynamicCast<NiAVObject>(node));

  if (!prnt && !parent) {
    /* there can't be any parent */
    if (node == masterroot)
      masterroot = nnode;
    /* there should be a parent! */
    else {
      brokennodes++;
      if (!prnt)
	addnote(" The child-parent relationship is broken, file needs revision.\n");
    }
  }

  /* replace references everywhere */
  SubstituteRefs(DynamicCast<NiObject>(nnode), DynamicCast<NiObject>(node));

  /* replace node (without having the ability directly) */
  subst = DynamicCast<NiObject>(nnode);

  return nnode->GetType().GetTypeName();
}

string ConvertNode(NiTriShapeRef node, NiNodeRef parent, NiObjectRef &subst) {
  NiGeometryDataRef raw = node->GetData();
  NiTriShapeDataRef data = DynamicCast<NiTriShapeData>(raw);

  /* move over bone weights */
  NiSkinInstanceRef skn = node->GetSkinInstance();
  if (skn) {
    NiSkinPartitionRef prt = skn->GetSkinPartition();
    if (prt) {
      int pc = prt->GetNumPartitions();
      for (int ps = 0; ps < pc; ps++) {
	if (prt->GetStripCount(ps)) {
	  barprintf(stdout, nodebarchar, DynamicCast<NiObjectNET>(node)->GetName().data());
	  nodebarchar = '-'; break;
	}
      }

      for (int ps = 0; ps < pc; ps++) {
	/* convert strips to lists automatically */
	unsigned short vc = prt->GetNumVertices(ps);
	unsigned short sc = prt->GetStripCount(ps);

	if (sc > 0) {
	  /* spit out some info */
	  barprintf(stdout, '.', "%s %d", "skinpartition", ps);
	  nfoprintf(stdout, "converting:\n");

	  /* convert strips to lists automatically */
	  vector<Niflib::Triangle> pt = prt->GetTriangles(ps);

	  size_t pts = 0;
	  size_t idx = pt.size();
	  vector< vector<unsigned short> > s(sc);
	  for (unsigned short ss = 0; ss < sc; ss++) {
	    s[ss] = prt->GetStrip(ps, ss);
	    pts += s[ss].size();
	  }

	  nfoprintf(stdout, " Strips           : %6d to %6d (%.4f%%)\n", sc, 0, 0.0f);
	  nfoprintf(stdout, " Points           : %6d to %6d (%.4f%%), space\n", (pts         ), (idx * 3), 100.0f * (idx * 3) / (pts         ));
	  nfoprintf(stdout, " Faces            : %6d to %6d (%.4f%%), perf.\n", (pts - sc * 2), (idx * 1), 100.0f * (idx * 1) / (pts - sc * 2));

	  /* remove strips, add lists */
	  prt->SetStripCount(ps, 0);
	  prt->SetTriangles(ps, pt);
	}
      }
    }
  }

  return node->GetType().GetTypeName();
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - -  */
#if 0
LINEAR_KEY = 1, /*!< Use linear interpolation. */
QUADRATIC_KEY = 2, /*!< Use quadratic interpolation.  Forward and back tangents will be stored. */
TBC_KEY = 3, /*!< Use Tension Bias Continuity interpolation.  Tension, bias, and continuity will be stored. */
XYZ_ROTATION_KEY = 4, /*!< For use only with rotation data.  Separate X, Y, and Z keys will be stored instead of using quaternions. */
UNKNOWN_KEY = 5, /*!< Unknown.  Step function? */
#endif

bool isequal(unsigned char  a, unsigned char  b) { return a == b; }
bool isequal(unsigned short a, unsigned short b) { return a == b; }
bool isequal(unsigned int   a, unsigned int   b) { return a == b; }
bool isequal(unsigned long  a, unsigned long  b) { return a == b; }

bool isequal(float  a, float  b) { return fabsf(a - b) < 1e-5; }
bool isequal(double a, double b) { return fabs (a - b) < 1e-5; }

bool isequal(Color4 a, Color4 b) {
  return (fabsf(a.r - b.r) < 1e-4) &&
	 (fabsf(a.g - b.g) < 1e-4) &&
	 (fabsf(a.b - b.b) < 1e-4) &&
	 (fabsf(a.a - b.a) < 1e-4);
}

bool isequal(Vector3 a, Vector3 b) {
  return (fabsf(a.x - b.x) < 1e-5) &&
	 (fabsf(a.y - b.y) < 1e-5) &&
	 (fabsf(a.z - b.z) < 1e-5);
}

bool isequal(Quaternion a, Quaternion b) {
  return (fabsf(a.x - b.x) < 1e-6) &&
	 (fabsf(a.y - b.y) < 1e-6) &&
	 (fabsf(a.z - b.z) < 1e-6) &&
	 (fabsf(a.w - b.w) < 1e-6);
}

/* linear interpolation:
 *  intp = (kp->data *           wp   ) + (kn->data * (1.0 -    wp  ));
 *  intp = (kp->data * (1.0 -    wn  )) + (kn->data *           wn   );
 * these two are equivalent
 *
 * quadric interpolation:
 *  intp = (kp->data *        wp * wp ) + (kn->data * (1.0 - wp * wp));
 *  intp = (kp->data * (1.0 - wn * wn)) + (kn->data *        wn * wn );
 * here the two forms are accelerating/deccalerating
 *
 * quadric bézier-curves:
 *  ...
 * tcb bézier-curves:
 *  ...
 *
 * http://www.cubic.org/docs/hermite.htm
 */

/* Scalar/Vector linear interpolation */
template<typename K>
K lerp(float t, K p, K q) {
  float wp, wq;

  wq =         t ;
  wp = (1.0f - t);

  return (p * wp) + (q * wq);
}

template<typename K>
K lerp(float t, Key<K> p, Key<K> q) {
  return lerp(1.0 - ((t - p.time) / (q.time - p.time)), p.data, q.data);
}

/* Scalar/Vector quadratic interpolation */
template<typename K>
K quad(float t, K p, K a, K b, K q) {
  K po1 = (q - p) * 3.0f - (a * 2.0f + b);
  K po2 = (p - q) * 2.0f + (a        + b);

  K po3 = (po2 * t) + po1;
  K po4 = (po3 * t) + a;
  K po5 = (po4 * t) + p;

  return po5;
}

template<typename K>
K quad(float t, Key<K> p, Key<K> q) {
  return quad(1.0 - ((t - p.time) / (q.time - p.time)), p.data, p.forward_tangent, q.backward_tangent, q.data);
}

/* Quaternion linear interpolation */
#define Q Niflib::Quaternion
template<>
Q lerp(float t, Q p, Q q) {
  float cosTheta = p.Dot(q);
  float theta    = acosf(cosTheta);
  float sinTheta = sinf(theta);
  float wp, wq;

  if (sinTheta > 0.001f) {
    wq = sinf(        t  * theta) / sinTheta;
    wp = sinf((1.0f - t) * theta) / sinTheta;
  }
  else {
    wq =         t ;
    wp = (1.0f - t);
  }

  return (p * wp) + (q * wq);
}

template<>
Q lerp(float t, Key<Q> p, Key<Q> q) {
  return lerp(1.0f - ((t - p.time) / (q.time - p.time)), p.data, q.data);
}
#undef Q

/* Quaternion quadratic interpolation */
#define Q Niflib::Quaternion
template<>
Q quad(float t, Q p, Q a, Q b, Q q) {
  Q sq1 = lerp(t, p, q);
  Q sq2 = lerp(t, a, b);

  return lerp(2.0f * t * (1.0f - t), sq1, sq2);
}

template<>
Q quad(float t, Key<Q> p, Key<Q> q) {
  return quad(1.0f - ((t - p.time) / (q.time - p.time)), p.data, p.forward_tangent, q.backward_tangent, q.data);
}
#undef Q

/* Scalar/Vector TCB factors (for quad) */
template<typename K>
void tcb(Key<K> o, K &b, Key<K> p, K &a, Key<K> q) {
  float wo, wq;
  float fo, fq;
  float m1, m2;
  K ko, kq;

  // Time deltas
  wo = p.time - o.time;
  wq = q.time - p.time;

  // Value deltas
  ko = p.value - o.value;
  kq = q.value - p.value;

  // Speed Control
  fo = (wo + wo) / (wo + wq);
  fq = (wq + wq) / (wo + wq);

  // The "incoming" (backward) Tangent equation:
  m1 = (1.0f - p.tension) * (1.0f - p.continuity) * (1.0f + p.bias) * 0.5f;
  // The "outgoing" (forward) Tangent equation:
  m2 = (1.0f - p.tension) * (1.0f + p.continuity) * (1.0f - p.bias) * 0.5f;

  // Factors for quad()
  a = (m1 * ko + m2 * kq) * fo;
  b = (m2 * ko + m1 * kq) * fq;
}

template<typename K>
K tcb(float t, Key<K> oo, Key<K> o, Key<K> p, Key<K> q, Key<K> qq) {
  float a1, a0, b0, b1;

  /**/ if (t <= p.time) {
    t = 1.0 - ((t - o.time) / (p.time - o.time));

    tcb(oo, a1, o, a0, p);
    tcb(o , b0, p, b1, q);

    return quad(t, o, a0, b0, p);
  }
  else if (t >= p.time) {
    t = 1.0 - ((t - p.time) / (q.time - p.time));

    tcb(o, a1, p, a0, q );
    tcb(p, b0, q, b1, qq);

    return quad(t, p, a0, b0, q);
  }
}

/* . . . . . . . . . . . . . . . . . . . . . . . . . .  */

template<typename K>
int OptimizeKeys(KeyType ip, vector< Key<K> > &keys);
template<typename K>
int OptimizeKeys(KeyType ip, vector< Key<K> > &keys, float round) {
  int kc = (int)keys.size();

  /* linear interpolator */
  if (ip == LINEAR_KEY) {
    if (kc < 3)
      return 0;

    vector< Key<K> >::iterator k = keys.begin() + 1;
    do {
      vector< Key<K> >::iterator kp = k - 1, del = k, kn = k + 1;

      /* linear interpolator */
      float wd = kn->time - kp->time; K dd = kn->data - kp->data;
      float wn = k ->time - kp->time; K dn = k ->data - kp->data;
      float wp = kn->time - k ->time; K dp = kn->data - k ->data;

      /* problems with rounding 0.5 + 0.5 == 0Ul */
//    K intpp = kp->data * wp;
//    K intpn = kn->data * wn;

      /* delta */
//    K intpp = dn / wn;		      // delta prev to now
//    K intpn = dp / wp;		      // delta now  to next

      /* no problems with rounding */
      K intpp = dn;			      // real      relative value
      K intpn = (K)((dd * wn + round) / wd);  // predicted relative value

      K diff = (intpp - intpn);

      /* remove and step back */
      if (isequal(intpp, intpn)) {
	k--; keys.erase(del);
      }
    } while (++k != (keys.end() - 1));
  }
  /* Quadratic Bézier curves */
  else if (ip == QUADRATIC_KEY) {
  }

  return (int)(kc - keys.size());
}

#define Q Niflib::Quaternion
template<>
int OptimizeKeys<Q>(KeyType ip, vector< Key<Q> > &keys, float round) {
  int kc = (int)keys.size();

  /* linear interpolator */
  if (ip == LINEAR_KEY) {
    if (kc < 3)
      return 0;

    vector< Key<Q> >::iterator k = keys.begin() + 1;
    do {
      vector< Key<Q> >::iterator kp = k - 1, del = k, kn = k + 1;

      /* spherical linear interpolator */
      float wd = kn->time - kp->time;
      float wn = k ->time - kp->time; wn /= wd;
      float wp = kn->time - k ->time; wp /= wd;

      /* we could also do normalizing linear interpolation
       * but I think spherical is more appropriate for
       * rotations, which seems the purpose of quaternions
       * in the first place
       */
      float cosTheta = kp->data.Dot(kn->data);
      float theta    = acosf(cosTheta);
      float sinTheta = sinf(theta);

      if (sinTheta > 0.001f) {
	wn = sinf(wn * theta) / sinTheta;
	wp = sinf(wp * theta) / sinTheta;
      }
      else {
	wn = wn;
	wp = wp;
      }

      /* no problems with rounding */
      Q intp = (kp->data * wp) + (kn->data * wn);

      /* remove and step back */
      if (isequal(intp, k->data)) {
	k--; keys.erase(del);
      }
    } while (++k != (keys.end() - 1));
  }
  /* Quadratic Bézier curves */
  else if (ip == QUADRATIC_KEY) {
  }

  return (int)(kc - keys.size());
}
#undef	Q

#define B unsigned char
template<>
int OptimizeKeys<B>(KeyType ip, vector< Key<B> > &keys) {
  int kc = (int)keys.size();
  if (kc < 3)
    return 0;

  vector< Key<B> >::iterator k = keys.begin() + 1;
  do {
    vector< Key<B> >::iterator kp = k - 1, del = k, kn = k + 1;

    /* step interpolator */
    float wd = kn->time - kp->time;
    float wn = k ->time - kp->time;
    float wp = kn->time - k ->time;

    /* there is no smooth interpolation-property for
     * booleans, it's a step-function
     */
    B intp = kp->data;

    /* remove and step back */
    if (intp == k->data) {
      k--; keys.erase(del);
    }
  } while (++k != (keys.end() - 1));

  return (int)(kc - keys.size());
}
#undef	B

#define S string
template<>
int OptimizeKeys<S>(KeyType ip, vector< Key<S> > &keys) {
  int kc = (int)keys.size();
  if (kc < 2)
    return 0;

  vector< Key<S> >::iterator k = keys.begin() + 1;
  do {
    vector< Key<S> >::iterator kp = k - 1, del = k, kn = k + 1;

    /* step interpolator
    float wd = kn->time - kp->time;
    float wn = k ->time - kp->time;
    float wp = kn->time - k ->time;
     */

    /* there is no smooth interpolation-property for
     * strings, it's a step-function
     */
    S intp = kp->data;

    /* remove and step back */
    if (intp == k->data) {
      k--; keys.erase(del);
    }
  } while (++k != (keys.end() - 0));

  return (int)(kc - keys.size());
}
#undef	S

string OptimizeNode(NiTextKeyExtraDataRef node) {
  vector< Key<string> > keys;
  int kd;

  /* spit out some info */
  barprintf(stdout, nodebarchar, "%s keyframes", nodebarref.data());
  nfoprintf(stdout, "processing:\n"); nodebarchar = '-';

  keys = node->GetKeys();

  nfoprintf(stdout, " Type             : %s\n", "Text");
  nfoprintf(stdout, " Interpolation    : %s\n", "Step");

  if ((kd = OptimizeKeys(LINEAR_KEY, keys)))
    node->SetKeys(keys), modifiedtlines++;

  nfoprintf(stdout, " Keys             : %6d to %6d (%.4f%%)\n", (int)keys.size() + kd, (int)keys.size(), 100.0f * keys.size() / ((int)keys.size() + kd));

  return node->GetType().GetTypeName();
}

string OptimizeNode(NiBoolDataRef node) {
  vector< Key<unsigned char> > keys;
  KeyType ip;
  int kd;

  /* spit out some info */
  barprintf(stdout, nodebarchar, "%s keyframes", nodebarref.data());
  nfoprintf(stdout, "processing:\n"); nodebarchar = '-';

  ip = node->GetKeyType();
  keys = node->GetKeys();

  nfoprintf(stdout, " Type             : %s\n", "Boolean");
  nfoprintf(stdout, " Interpolation    : %s\n", KeyName[ip]);

  if ((kd = OptimizeKeys(ip, keys)))
    node->SetKeys(keys), modifiedtlines++;

  nfoprintf(stdout, " Keys             : %6d to %6d (%.4f%%)\n", (int)keys.size() + kd, (int)keys.size(), 100.0f * keys.size() / ((int)keys.size() + kd));

  return node->GetType().GetTypeName();
}

string OptimizeNode(NiVisDataRef node) {
  vector< Key<unsigned char> > keys;
  int kd;

  /* spit out some info */
  barprintf(stdout, nodebarchar, "%s keyframes", nodebarref.data());
  nfoprintf(stdout, "processing:\n"); nodebarchar = '-';

  keys = node->GetKeys();

  nfoprintf(stdout, " Type             : %s\n", "Vis");
  nfoprintf(stdout, " Interpolation    : %s\n", KeyName[LINEAR_KEY]);

  if ((kd = OptimizeKeys(LINEAR_KEY, keys, 0.5f)))
    node->SetKeys(keys), modifiedtlines++;

  nfoprintf(stdout, " Keys             : %6d to %6d (%.4f%%)\n", (int)keys.size() + kd, (int)keys.size(), 100.0f * keys.size() / ((int)keys.size() + kd));

  return node->GetType().GetTypeName();
}

string OptimizeNode(NiMorphDataRef node) {
  vector< Key<float> > keys;
  KeyType ip;
  int kd, mc, m;

  /* spit out some info */
  barprintf(stdout, nodebarchar, "%s keyframes", nodebarref.data());
  nfoprintf(stdout, "processing:\n"); nodebarchar = '-';
  nfoprintf(stdout, " Type             : %s\n", "Morph");

  mc = node->GetMorphCount();
  for (m = 0; m < mc; m++) {
    ip = node->GetMorphKeyType(m);
    keys = node->GetMorphKeys(m);

    nfoprintf(stdout, " Interpolation %2d : %s\n", m, KeyName[ip]);

    if ((kd = OptimizeKeys(ip, keys, 0.0f)))
      node->SetMorphKeys(m, keys), modifiedtlines++;

    nfoprintf(stdout, " Keys %2d          : %6d to %6d (%.4f%%)\n", (int)keys.size() + kd, (int)keys.size(), 100.0f * keys.size() / ((int)keys.size() + kd));
  }

  return node->GetType().GetTypeName();
}

string OptimizeNode(NiColorDataRef node) {
  vector< Key<Color4> > keys;
  KeyType ip;
  size_t kd;

  /* spit out some info */
  barprintf(stdout, nodebarchar, "%s keyframes", nodebarref.data());
  nfoprintf(stdout, "processing:\n"); nodebarchar = '-';

  ip = node->GetKeyType();
  keys = node->GetKeys();

  nfoprintf(stdout, " Type             : %s\n", "Color");
  nfoprintf(stdout, " Interpolation    : %s\n", KeyName[ip]);

  if ((kd = OptimizeKeys(ip, keys, 0.0f)))
    node->SetKeys(keys), modifiedtlines++;

  nfoprintf(stdout, " Keys             : %6d to %6d (%.4f%%)\n", (int)keys.size() + kd, (int)keys.size(), 100.0f * keys.size() / ((int)keys.size() + kd));

  return node->GetType().GetTypeName();
}

string OptimizeNode(NiFloatDataRef node) {
  vector< Key<float> > keys;
  KeyType ip;
  size_t kd;

  /* spit out some info */
  barprintf(stdout, nodebarchar, "%s keyframes", nodebarref.data());
  nfoprintf(stdout, "processing:\n"); nodebarchar = '-';

  ip = node->GetKeyType();
  keys = node->GetKeys();

  nfoprintf(stdout, " Type             : %s\n", "Float");
  nfoprintf(stdout, " Interpolation    : %s\n", KeyName[ip]);

  if ((kd = OptimizeKeys(ip, keys, 0.0f)))
    node->SetKeys(keys), modifiedtlines++;

  nfoprintf(stdout, " Keys             : %6d to %6d (%.4f%%)\n", (int)keys.size() + kd, (int)keys.size(), 100.0f * keys.size() / ((int)keys.size() + kd));

  return node->GetType().GetTypeName();
}

string OptimizeNode(NiPosDataRef node) {
  vector< Key<Vector3> > keys;
  KeyType ip;
  int kd;

  /* spit out some info */
  barprintf(stdout, nodebarchar, "%s keyframes", nodebarref.data());
  nfoprintf(stdout, "processing:\n"); nodebarchar = '-';

  ip = node->GetKeyType();
  keys = node->GetKeys();

  nfoprintf(stdout, " Type             : %s\n", "Position");
  nfoprintf(stdout, " Interpolation    : %s\n", KeyName[ip]);

  if ((kd = OptimizeKeys(ip, keys, 0.0f)))
    node->SetKeys(keys), modifiedtlines++;

  nfoprintf(stdout, " Keys             : %6d to %6d (%.4f%%)\n", (int)keys.size() + kd, (int)keys.size(), 100.0f * keys.size() / ((int)keys.size() + kd));

  return node->GetType().GetTypeName();
}

string OptimizeNode(NiKeyframeDataRef node) {
  vector< Key<Quaternion> > keysq;
  vector< Key<float> > keysx;
  vector< Key<float> > keysy;
  vector< Key<float> > keysz;
  vector< Key<Vector3> > keyst;
  vector< Key<float> > keyss;
  KeyType ipq;
  KeyType ipx;
  KeyType ipy;
  KeyType ipz;
  KeyType ipt;
  KeyType ips;
  size_t kd;

  /* spit out some info */
  barprintf(stdout, nodebarchar, "%s keyframes", nodebarref.data());
  nfoprintf(stdout, "processing:\n"); nodebarchar = '-';

  ipq = node->GetRotateType();
  keysq = node->GetQuatRotateKeys();
  if (keysq.size()) {
    nfoprintf(stdout, " Type             : %s\n", "Quaternion-Rotation");
    nfoprintf(stdout, " Interpolation    : %s\n", KeyName[ipq]);

    if ((kd = OptimizeKeys(ipq, keysq, 0.0f)))
      node->SetQuatRotateKeys(keysq), modifiedtlines++;

    nfoprintf(stdout, " Keys             : %6d to %6d (%.4f%%)\n", (int)keysq.size() + kd, (int)keysq.size(), 100.0f * keysq.size() / ((int)keysq.size() + kd));
  }

  ipx = node->GetXRotateType();
  keysx = node->GetXRotateKeys();
  if (keysx.size()) {
    nfoprintf(stdout, " Type             : %s\n", "Euler-Rotation X");
    nfoprintf(stdout, " Interpolation    : %s\n", KeyName[ipx]);

    if ((kd = OptimizeKeys(ipx, keysx, 0.0f)))
      node->SetXRotateKeys(keysx), modifiedtlines++;

    nfoprintf(stdout, " Keys             : %6d to %6d (%.4f%%)\n", (int)keysx.size() + kd, (int)keysx.size(), 100.0f * keysx.size() / ((int)keysx.size() + kd));
  }

  ipy = node->GetYRotateType();
  keysy = node->GetYRotateKeys();
  if (keysy.size()) {
    nfoprintf(stdout, " Type             : %s\n", "Euler-Rotation Y");
    nfoprintf(stdout, " Interpolation    : %s\n", KeyName[ipy]);

    if ((kd = OptimizeKeys(ipy, keysy, 0.0f)))
      node->SetYRotateKeys(keysy), modifiedtlines++;

    nfoprintf(stdout, " Keys             : %6d to %6d (%.4f%%)\n", (int)keysy.size() + kd, (int)keysy.size(), 100.0f * keysy.size() / ((int)keysy.size() + kd));
  }

  ipz = node->GetZRotateType();
  keysz = node->GetZRotateKeys();
  if (keysz.size()) {
    nfoprintf(stdout, " Type             : %s\n", "Euler-Rotation Z");
    nfoprintf(stdout, " Interpolation    : %s\n", KeyName[ipz]);

    if ((kd = OptimizeKeys(ipz, keysz, 0.0f)))
      node->SetZRotateKeys(keysz), modifiedtlines++;

    nfoprintf(stdout, " Keys             : %6d to %6d (%.4f%%)\n", (int)keysz.size() + kd, (int)keysz.size(), 100.0f * keysz.size() / ((int)keysz.size() + kd));
  }

  ipt = node->GetTranslateType();
  keyst = node->GetTranslateKeys();
  if (keyst.size()) {
    nfoprintf(stdout, " Type             : %s\n", "Translation");
    nfoprintf(stdout, " Interpolation    : %s\n", KeyName[ipt]);

    if ((kd = OptimizeKeys(ipt, keyst, 0.0f)))
      node->SetTranslateKeys(keyst), modifiedtlines++;

    nfoprintf(stdout, " Keys             : %6d to %6d (%.4f%%)\n", (int)keyst.size() + kd, (int)keyst.size(), 100.0f * keyst.size() / ((int)keyst.size() + kd));
  }

  ips = node->GetScaleType();
  keyss = node->GetScaleKeys();
  if (keyss.size()) {
    nfoprintf(stdout, " Type             : %s\n", "Scale");
    nfoprintf(stdout, " Interpolation    : %s\n", KeyName[ips]);

    if ((kd = OptimizeKeys(ips, keyss, 0.0f)))
      node->SetScaleKeys(keyss), modifiedtlines++;

    nfoprintf(stdout, " Keys             : %6d to %6d (%.4f%%)\n", (int)keyss.size() + kd, (int)keyss.size(), 100.0f * keyss.size() / ((int)keyss.size() + kd));
  }

  return node->GetType().GetTypeName();
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - -  */

string OptimizeNode(bhkNiTriStripsShapeRef node) {
  int sd = node->GetNumStripsData();
  /* no need for collision-node */
  if (!sd)
    throw runtime_error("No Strips found.");

  /* spit out some info */
  barprintf(stdout, nodebarchar, "%s collider", nodebarref.data());
  nfoprintf(stdout, "processing:\n"); nodebarchar = '-';
  nfoprintf(stdout, " Type             : %s\n", nodemotion == MotionSystem::MO_SYS_FIXED ? "Static" : "Dynamic");
  nfoprintf(stdout, " Material         : %s\n", MaterialName[node->GetMaterial()]);

  bool degeneratevertx = false,
       degeneratefaces = false;

  for (int ss = 0; ss < sd; ss++) {
    NiGeometryDataRef raw = node->GetStripsData(ss);
    NiTriStripsDataRef data = DynamicCast<NiTriStripsData>(raw);

    /* convert strips to primitives automatically */
    int vc = data->GetVertexCount();
    unsigned short sc = data->GetStripCount();

    vector<Niflib::Vector3> v = data->GetVertices();
    vector<Niflib::Triangle> f = data->GetTriangles();

    bool degeneratevertx = false,
	 degeneratefaces = false;

    int fc = (int)f.size();

    /* raw containers */
    Faces.clear();
    Vertices.clear();

    /* unoptimized containers */
    SectorFaces.clear();
    SectorVertices.clear();
    SectorRemap.clear();	// old index to new index

    {
      vector<Niflib::Triangle>::iterator itf;
      for (itf = f.begin(); itf != f.end(); itf++) {
	struct Triangle3d tri;

	memset(&tri, 0, sizeof(tri));

	/* the original vertex indices (for remapping skin weights)
	 */
	tri.p1.odx = (*itf).v1;
	tri.p2.odx = (*itf).v2;
	tri.p3.odx = (*itf).v3;

	if (((int)tri.p1.odx > vc) ||
	    ((int)tri.p2.odx > vc) ||
	    ((int)tri.p3.odx > vc))
	  throw runtime_error("Face-index out-of-bounds. Corrupt face, NifSkope will crash opening this file.");

	/* --------------------------------------- */
	tri.p1.x = v[(*itf).v1].x;
	tri.p1.y = v[(*itf).v1].y;
	tri.p1.z = v[(*itf).v1].z;

	/* --------------------------------------- */
	tri.p2.x = v[(*itf).v2].x;
	tri.p2.y = v[(*itf).v2].y;
	tri.p2.z = v[(*itf).v2].z;

	/* --------------------------------------- */
	tri.p3.x = v[(*itf).v3].x;
	tri.p3.y = v[(*itf).v3].y;
	tri.p3.z = v[(*itf).v3].z;

	/* --------------------------------------- *
	if (n.size()) {
	  tri.nx = n[itf - f.begin()].x;
	  tri.ny = n[itf - f.begin()].y;
	  tri.nz = n[itf - f.begin()].z;
	}					   */

	/* the collector considers the entire data-set of a vertex
	 * to determine "unique" vertices
	 */
	CollectFace(&tri);
      }
    }

    CalculateGeometryNormals();
    IndexGeometry((int)v.size());

    {
      set<class objVertex *, struct V>::iterator itv;
      std::vector<class objFace *>::iterator itf;
      int idx = 0;

      vector<Niflib::Vector3> _v;
      vector<Niflib::Vector3> _n;
      vector<Niflib::Triangle> _f;

      for (itv = SectorVertices.begin(); itv != SectorVertices.end(); itv++) {
	/* assign index the moment of writing it out */
	(*itv)->idx = idx++;

	_v.push_back(Niflib::Vector3(
	  (float)(*itv)->x,
	  (float)(*itv)->y,
	  (float)(*itv)->z)
	);
      }

      for (itf = SectorFaces.begin(); itf != SectorFaces.end(); itf++) {
	_f.push_back(Niflib::Triangle(
	  (*itf)->v[0]->idx,
	  (*itf)->v[1]->idx,
	  (*itf)->v[2]->idx
	));
      }

      data->SetVertices(_v);
      data->SetTriangles(_f);
      data->SetNormals(_n);

      /* register what happened */
      degeneratevertx = degeneratevertx || (v.size() != _v.size());
      degeneratefaces = degeneratefaces || (f.size() != _f.size());

      nfoprintf(stdout, " Vertices %2d      : %6d to %6d (%.4f%%)\n", ss, v.size(), _v.size(), 100.0f * _v.size() / v.size());
      nfoprintf(stdout, " Faces %2d         : %6d to %6d (%.4f%%)\n", ss, f.size(), _f.size(), 100.0f * _f.size() / f.size());
    }

    FreeGeometry();
  }

  if (degeneratevertx || degeneratefaces) {
    if (degeneratevertx && !degeneratefaces)
      addnote(" Removed degenerate vertices (vertex-duplications).\n"), modifiedmeshes++;
    else if (!degeneratevertx && degeneratefaces)
      addnote(" Removed degenerate faces (faces without area or duplicate faces).\n"), modifiedmeshes++;
    else if (degeneratevertx && degeneratefaces)
      addnote(" Removed identical vertices and their faces.\n"), modifiedmeshes++;
  }

  return node->GetType().GetTypeName();
}

string OptimizeNode(bhkPackedNiTriStripsShapeRef node) {
  /* check supported version-number */
  if ((masterinfo.userVersion >= 10) &&
      (masterinfo.userVersion <= 11) &&
      ((masterinfo.version == 0x14000004) ||
       (masterinfo.version == 0x14000005) ||
       (masterinfo.version == 0x14000007))) {
  }
  else {
  }

  hkPackedNiTriStripsDataRef data = node->GetData();
  vector<Niflib::OblivionSubShape> os; bool osdta;

  // 0x14000007
  os = data->GetSubShapes(), osdta = true;
  if (!os.size())
    // 0x14000004, 0x14000005
    os = node->GetSubShapes(), osdta = false;

  vector<int> vb(os.size()), va(os.size());
  vector<int> fb(os.size()), fa(os.size());
  int sc = (int)os.size();
  int vs = 0;

  /* spit out some info */
  barprintf(stdout, nodebarchar, "%s collider", nodebarref.data());
  nfoprintf(stdout, "processing:\n"); nodebarchar = '-';
  nfoprintf(stdout, " Type             : %s\n", nodemotion == MotionSystem::MO_SYS_FIXED ? "Static" : "Dynamic");
//nfoprintf(stdout, " Material         : %s\n", MaterialName[node->GetMaterial()]);

  bool degeneratevertx = false,
       degeneratefaces = false;

  /* distinct number of materials */
  set<Niflib::HavokMaterial> om;
  for (int ss = 0; ss < sc; ss++) {
    om.insert(os[ss].material);
    vs    += os[ss].numVertices;
    vb[ss] = os[ss].numVertices;
  }
  size_t mc = om.size();

  /* no need for collision-node */
  if (!sc || !mc)
    throw runtime_error("No SubShapes found.");

  int vc = data->GetVertexCount();
  if (vs != vc) {
    if (sc == 1) {
      vb[0] = vc;
      vs    = vc;

      addnote(" Number of vertices doesn't equal number of sum, fixed.\n");
    }
    else {
      vb[sc - 1] += vc - vs;
      vs          = vc;

      addnote(" Number of vertices doesn't equal number of sum, probably broken collider!\n");
    }
  }

  vector<Niflib::Vector3> v = data->GetVertices();
  vector<Niflib::hkTriangle> f = data->GetTrianglesFull();

  int fc = (int)f.size();

  /* no need for collision-node */
  if ((vc < 3) || (fc < 1))
    throw runtime_error("No Vertices & Faces found.");

  /* raw containers */
  Faces.clear();
  Vertices.clear();

  /* unoptimized containers */
  SectorFaces.clear();
  SectorVertices.clear();
  SectorRemap.clear();	// old index to new index

  {
    vector<Niflib::hkTriangle>::iterator itf;
    for (itf = f.begin(); itf != f.end(); itf++) {
      struct Triangle3d tri;

      memset(&tri, 0, sizeof(tri));

      /* the original vertex indices
       * (for remapping skin weights)
       */
      tri.p1.odx = (*itf).triangle.v1;
      tri.p2.odx = (*itf).triangle.v2;
      tri.p3.odx = (*itf).triangle.v3;

      if (((int)tri.p1.odx > vc) ||
	  ((int)tri.p2.odx > vc) ||
	  ((int)tri.p3.odx > vc))
	throw runtime_error("Face-index out-of-bounds. Corrupt face, NifSkope will crash opening this file.");

      /* sorting key */
      tri.p1.groupid = -1;
      tri.p2.groupid = -2;
      tri.p3.groupid = -3;

      unsigned int start = 0;
      for (unsigned int ss = 0; ss < (unsigned int)sc; ss++) {
	unsigned int end = start + vb[ss];

	if ((tri.p1.odx >= start) && (tri.p1.odx < end))
	  tri.p1.groupid = (signed short)ss;
	if ((tri.p2.odx >= start) && (tri.p2.odx < end))
	  tri.p2.groupid = (signed short)ss;
	if ((tri.p3.odx >= start) && (tri.p3.odx < end))
	  tri.p3.groupid = (signed short)ss;

	start = end;
      }

      assert((tri.p1.groupid >= 0) && (tri.p2.groupid >= 0) && (tri.p3.groupid >= 0));
      assert((tri.p1.groupid == tri.p2.groupid) && (tri.p2.groupid == tri.p3.groupid));

      /* --------------------------------------- */
      tri.p1.x = v[(*itf).triangle.v1].x;
      tri.p1.y = v[(*itf).triangle.v1].y;
      tri.p1.z = v[(*itf).triangle.v1].z;

      /* --------------------------------------- */
      tri.p2.x = v[(*itf).triangle.v2].x;
      tri.p2.y = v[(*itf).triangle.v2].y;
      tri.p2.z = v[(*itf).triangle.v2].z;

      /* --------------------------------------- */
      tri.p3.x = v[(*itf).triangle.v3].x;
      tri.p3.y = v[(*itf).triangle.v3].y;
      tri.p3.z = v[(*itf).triangle.v3].z;

      /* --------------------------------------- *
      tri.p1.nx = (*itf).normal.x;
      tri.p2.ny = (*itf).normal.y;
      tri.p3.nz = (*itf).normal.z;		 */

      /* --------------------------------------- */
      tri.extra = (*itf).weldingInfo;

      /* the collector considers the entire data-set of a vertex
       * to determine "unique" vertices
       */
      CollectFace(&tri);

      /* count cluster-faces */
      fb[tri.p1.groupid]++;
    }
  }

  CalculateGeometryNormals();
  IndexGeometry((int)v.size());

  {
    set<class objVertex *, struct V>::iterator itv;
    std::vector<class objFace *>::iterator itf;
    int idx = 0;

    vector<Niflib::Vector3> _v;
    vector<Niflib::hkTriangle> _f;

    int lastid = 0, lastidx = 0;
    for (itv = SectorVertices.begin(); itv != SectorVertices.end(); itv++) {
      /* record border-trespassing (the set is guaranteed to be sorted by groupid) */
      int thisid = (*itv)->groupid;
      assert(thisid >= lastid);
      if (thisid != lastid) {
	os[lastid].numVertices =

	idx - lastidx;
	lastidx = idx;
      }

      /* assign index the moment of writing it out */
      (*itv)->idx = idx++;

      _v.push_back(Niflib::Vector3(
	(float)(*itv)->x,
	(float)(*itv)->y,
	(float)(*itv)->z)
      );

      lastid = thisid;
    }

    /* final group */
    os[lastid].numVertices = idx - lastidx;

    for (itf = SectorFaces.begin(); itf != SectorFaces.end(); itf++) {
      /* record border-trespassing (the set is guaranteed to be sorted by groupid) */
      int thisid = (*itf)->v[0]->groupid;

      assert((*itf)->v[0]->groupid == thisid);
      assert((*itf)->v[1]->groupid == thisid);
      assert((*itf)->v[2]->groupid == thisid);

      Niflib::hkTriangle hkt;

      /* set face-indices */
      hkt.triangle.v1 = (*itf)->v[0]->idx;
      hkt.triangle.v2 = (*itf)->v[1]->idx;
      hkt.triangle.v3 = (*itf)->v[2]->idx;

      /* set face-normal */
      float len = (*itf)->narea * 2.0f;
      assert(len >= 0.0f); /* ensure CCW */
      hkt.normal.x = (*itf)->nx / len;
      hkt.normal.y = (*itf)->ny / len;
      hkt.normal.z = (*itf)->nz / len;

      /* restore welding info (even if it is a bit useless ...) */
      hkt.weldingInfo = (*itf)->extra;

      _f.push_back(hkt);

      /* count cluster-faces */
      fa[thisid]++;
    }

    data->SetVertices(_v);
    data->SetTriangles(_f);
  }

  /* distinct number of materials */
  for (int ss = 0; ss < sc; ss++) {
    va[ss] = os[ss].numVertices;
    /* TODO: remove */
    if (os[ss].numVertices == 0)
      addnote(" Shape %d is empty now TODO remove subshape/material\n", ss);
  }

  /* stats */
  for (int ss = 0; ss < sc; ss++) {
    /* register what happened */
    degeneratevertx = degeneratevertx || (vb[ss] != va[ss]);
    degeneratefaces = degeneratefaces || (fb[ss] != fa[ss]);

    nfoprintf(stdout, " Material %2d      : %s\n", ss, MaterialName[os[ss].material]);
    nfoprintf(stdout, " Vertices %2d      : %6d to %6d (%.4f%%)\n", ss, vb[ss], va[ss], 100.0f * va[ss] / vb[ss]);
    nfoprintf(stdout, " Faces %2d         : %6d to %6d (%.4f%%)\n", ss, fb[ss], fa[ss], 100.0f * fa[ss] / fb[ss]);
  }

  if (osdta)
    // 0x14000007
    data->SetSubShapes(os);
  else
    // 0x14000004, 0x14000005
    node->SetSubShapes(os);

  FreeGeometry();

  if (degeneratevertx || degeneratefaces) {
    if (degeneratevertx && !degeneratefaces)
      addnote(" Removed degenerate vertices (vertex-duplications).\n"), modifiedmeshes++;
    else if (!degeneratevertx && degeneratefaces)
      addnote(" Removed degenerate faces (faces without area or duplicate faces).\n"), modifiedmeshes++;
    else if (degeneratevertx && degeneratefaces)
      addnote(" Removed identical vertices and their faces.\n"), modifiedmeshes++;
  }

  /* if inside MOPP-shape, the below function embeds this function
   * and the MOPP-code is recalculated
   */

  /* TODO: if not in MOPP and STATIC, create MOPP! */

  return node->GetType().GetTypeName();
}

string OptimizeNode(bhkMoppBvTreeShapeRef node) {
  assert((nodemotion == MO_SYS_FIXED) || (nodemotion == MO_SYS_KEYFRAMED));
  nodemopped = true;

  /* check supported version-number */
  if ((masterinfo.userVersion >= 10) &&
      (masterinfo.userVersion <= 11) &&
      ((masterinfo.version == 0x14000004) ||
       (masterinfo.version == 0x14000005) ||
       (masterinfo.version == 0x14000007))) {
    bhkShapeRef raw = node->GetShape();
    bhkPackedNiTriStripsShapeRef pack = DynamicCast<bhkPackedNiTriStripsShape>(raw);
    bhkNiTriStripsShapeRef unpack = DynamicCast<bhkNiTriStripsShape>(raw);

    /* bhkNiTriStripsShape in bhkMoppBvTreeShape:
     *
     *  DLCShiveringIsles - Meshes\meshes\dungeons\root\interior\misc\gnarlspawner.nif
     */
    if (unpack) {
      /* MOPP is gone */
      string type = OptimizeNode(unpack);
      if (type != "bhkNiTriStripsShape")
	abort();

      /* skip optimizing MOPP */
      if (!optimizesmmopp)
	return node->GetType().GetTypeName();

      int sd = unpack->GetNumStripsData();

      vector<Niflib::Vector3> _v;
      vector<Niflib::Triangle> _f;
      vector<Niflib::Vector3> _n;

      int offset = 0;
      for (int ss = 0; ss < sd; ss++) {
	NiGeometryDataRef raw = unpack->GetStripsData(ss);
	NiTriStripsDataRef data = DynamicCast<NiTriStripsData>(raw);

	/* convert multiple strip-datas to one consecusive array */
	vector<Niflib::Vector3> v = data->GetVertices();
	vector<Niflib::Triangle> f = data->GetTriangles();
	vector<Niflib::Vector3> n = data->GetNormals();

	vector<Niflib::Vector3>::iterator itv;
	for (itv = v.begin(); itv != v.end(); itv++)
	  _v.push_back(*itv);

	vector<Niflib::Triangle>::iterator itf;
	for (itf = f.begin(); itf != f.end(); itf++)
	  _f.push_back(*itf);

	vector<Niflib::Vector3>::iterator itn;
	for (itn = n.begin(); itn != n.end(); itn++)
	  _n.push_back(*itn);

	offset += (int)_v.size();
      }

      int vc = (int)_v.size();
      int wc = (int)_f.size();
      int fc = (int)_f.size();

      /* single sub-shape MOPP */
      vector<int> vb(1);
      vector<Niflib::byte> vm(1);
      vector<short> vf(1);
      int sc = 1;
      {
	vb[0] = vc;
	vm[0] = unpack->GetMaterial();
	vf[0] = 0;  // may be used ...
      }

      vector<Niflib::byte> ocode = node->GetMoppCode();
      Vector3 oorigin = node->GetMoppOrigin();
      float oscale = node->GetMoppScale();
      int olen = (int)ocode.size();

      vector<short> wcode;
      int wlen = 0;

      vector<Niflib::byte> ncode;
      Vector3 norigin;
      float nscale;
      int nlen = 0;

      /* MOPP may need recalculation:
       *
       * removal of faces:    no, MOPP takes care of degenerate faces
       * removal of vertices: no, MOPP takes care of identical vertices
       * rem/add of shapes:   yes, the only case where it'd be needed
       *
       * which means even if we optimize the data, the MOPP stays valid
       * and the re-calculation can fail without problems
       */
      wlen = wc;
      nlen = GenerateMoppCodeWithSubshapes(
	sc, &vb[0],
	sc, reinterpret_cast<hvkByte *>(&vm[0]),
	vc, reinterpret_cast<hvkPoint3 *>(&_v[0]),
	fc, reinterpret_cast<hvkTriangle *>(&_f[0])
      );

      if (nlen > 0) {
	wcode.resize(wlen);
	ncode.resize(nlen);

	if (RetrieveMoppCode(nlen, reinterpret_cast<hvkByte *>(&ncode[0])) > 0) {
	  RetrieveMoppScale (&nscale);
	  RetrieveMoppOrigin(&norigin[0]);
	}

	node->SetMoppCode(ncode);
	node->SetMoppScale(nscale);
	node->SetMoppOrigin(norigin);
      }

      nfoprintf(stdout, " MOPP Code Size   : %6d to %6d (%.4f%%)\n", olen, nlen, nlen * 100.0f / olen);
    }
    else if (pack) {
      hkPackedNiTriStripsDataRef data = pack->GetData();

      int _vc = data->GetVertexCount();
      int _fc = data->GetNumFace();

      /* MOPP is gone */
      string type = OptimizeNode(pack);
      if (type != "bhkPackedNiTriStripsShape")
	abort();

      data = pack->GetData();
      vector<Niflib::OblivionSubShape> os; bool osdta;

      // 0x14000007
      os = data->GetSubShapes(), osdta = true;
      if (!os.size())
	// 0x14000004, 0x14000005
	os = pack->GetSubShapes(), osdta = false;

      vector<int> vb(os.size());
      vector<Niflib::byte> vm(os.size());
      vector<short> vf(os.size());
      int sc = (int)os.size();
      for (int ss = 0; ss < sc; ss++) {
	vb[ss] = os[ss].numVertices;
	vm[ss] = os[ss].material;
	vf[ss] = os[ss].colFilter;  // may be used ...
      }

      /* skip optimizing MOPP */
      if ((sc <= 1) && !optimizesmmopp)
	return node->GetType().GetTypeName();
      if ((sc >  1) && !optimizemmmopp)
	return node->GetType().GetTypeName();

      vector<Niflib::Vector3> v = data->GetVertices();
      vector<Niflib::hkTriangle> w = data->GetTrianglesFull();
      vector<Niflib::Triangle> f = data->GetTriangles();

      int vc = data->GetVertexCount();
      int wc = (int)w.size();
      int fc = (int)f.size();

      vector<Niflib::byte> ocode = node->GetMoppCode();
      Vector3 oorigin = node->GetMoppOrigin();
      float oscale = node->GetMoppScale();
      int olen = (int)ocode.size();

      vector<unsigned short> wcode;
      int wlen = 0;

      vector<Niflib::byte> ncode;
      Vector3 norigin;
      float nscale;
      int nlen = 0;

#ifdef	BRUTEFORCE_MATCHHAVOK
      vector<unsigned short> owcode = data->GetWeldings();
      int owlen = owcode.size();

      global_owcode = (unsigned short *)&owcode[0];
      global_owlen = owlen;
      global_ocode = (unsigned char *)&ocode[0];
      global_olen = olen;

      fprintf(stdout, "MOPP-Code %d:\n", olen);
      for (int n = 0; n < olen; n++) {
	fprintf(stdout, " %02x ", ocode[n]);
	if ((n & 15) == 15)
	  fprintf(stdout, "\n");
      }
      fprintf(stdout, "\n");

      fprintf(stdout, "Welding %d:\n", owlen);
      for (int n = 0; n < owlen; n++) {
	fprintf(stdout, " %04x ", owcode[n]);
	if ((n & 7) == 7)
	  fprintf(stdout, "\n");
      }
      fprintf(stdout, "\n");
#endif

      /* MOPP may need recalculation:
       *
       * removal of faces:    no, MOPP takes care of degenerate faces
       * removal of vertices: no, MOPP takes care of identical vertices
       * rem/add of shapes:   yes, the only case where it'd be needed
       *
       * which means even if we optimize the data, the MOPP stays valid
       * and the re-calculation can fail without problems
       */
      wlen = wc;
      nlen = GenerateMoppCodeWithSubshapes(
	sc, &vb[0],
	sc, reinterpret_cast<hvkByte *>(&vm[0]),
	vc, reinterpret_cast<hvkPoint3 *>(&v[0]),
	fc, reinterpret_cast<hvkTriangle *>(&f[0])
      );

      if (nlen > 0) {
	wcode.resize(wlen);
	ncode.resize(nlen);

	if (RetrieveMoppCode(nlen, reinterpret_cast<hvkByte *>(&ncode[0])) > 0) {
	  if (RetrieveMoppWelding(wlen, reinterpret_cast<hvkShort *>(&wcode[0])) > 0) {
	    /* transfer welding-info */
	    vector<Niflib::hkTriangle>::iterator itw; int pos;
	    for (pos = 0, itw = w.begin(); itw != w.end(); itw++, pos++)
	      (*itw).weldingInfo = wcode[pos];

	    /* and put it into the list */
	    data->SetTriangles(w);
	  }

	  RetrieveMoppScale (&nscale);
	  RetrieveMoppOrigin(&norigin[0]);
	}

	node->SetMoppCode(ncode);
	node->SetMoppScale(nscale);
	node->SetMoppOrigin(norigin);
      }

      nfoprintf(stdout, " MOPP Code Size   : %6d to %6d (%.4f%%)\n", olen, nlen, nlen * 100.0f / olen);
    }
  }

  nodemopped = false;
  return node->GetType().GetTypeName();
}

string OptimizeNode(BSSegmentedTriShapeRef node) {
  return node->GetType().GetTypeName();
}

string OptimizeNode(NiEnvMappedTriShapeRef node) {
  return node->GetType().GetTypeName();
}

/* corresponds to: SpellOptimizeGeometry */
string OptimizeNode(NiTriShapeRef node) {
  NiGeometryDataRef raw = node->GetData();
  NiTriShapeDataRef data = DynamicCast<NiTriShapeData>(raw);

  /* doesn't mean it's not there, just broken */
  if (!data) {
    brokennodes++;
    if (!raw)
      addnote(" No mesh attached, the file needs some revision.\n");
    else
      addnote(" The attached mesh is of some illegal type, the file needs some revision!\n");

    return "";
  }

  /* spit out some info */
  barprintf(stdout, nodebarchar, DynamicCast<NiObjectNET>(node)->GetName().data());
  nfoprintf(stdout, "processing:\n"); nodebarchar = '-';

  /* partition had another number of vertices as the main body! */
  bool repairpartition = false, repairbnweights = false, repairmorphvrtx = false;
  bool brokenpartition = false, brokenbnweights = false, brokenmorphvrtx = false;
  bool degeneratevertx = false, degeneratefaces = false, brokentangentsp = false;

  int vc = data->GetVertexCount();
  short uvc = data->GetUVSetCount();
  int vic = data->GetVertexIndexCount();

  Niflib::byte tf = data->GetTspaceFlag();
  bool nf = data->GetHasNormals();

  vector<Niflib::Vector3> v = data->GetVertices();
  vector<Niflib::Color4> c = data->GetColors();
  vector<Niflib::TexCoord> uv[16];
  for (int uvs = 0; uvs < uvc; uvs++)
    uv[uvs] = data->GetUVSet(uvs);
  vector<Niflib::Vector3> t = data->GetTangents();
  vector<Niflib::Vector3> n = data->GetNormals();
  vector<Niflib::Vector3> b = data->GetBinormals();
  vector<Niflib::Triangle> f = data->GetTriangles();

  int fc = (int)f.size();

  bool xtnb = false; size_t tnbs = 0;
  NiBinaryExtraDataRef bx = NULL;
  vector<Niflib::byte> bd;
  Niflib::Vector3 *xt = NULL;
  Niflib::Vector3 *xb = NULL;

  /* find out if we have TB-vectors in the extra data */
  list< Ref<NiExtraData> > x = node->GetExtraData();
  list< Ref<NiExtraData> >::iterator xd;
  for (xd = x.begin(); xd != x.end(); xd++) {
    if (!strcmp("Tangent space (binormal & tangent vectors)", (*xd)->GetName().data())) {
      if ((bx = DynamicCast<NiBinaryExtraData>(*xd))) {
	/* this is a copy? */
	bd = bx->GetData();

	tnbs = bd.size() / (sizeof(Niflib::Vector3) * 2);
	if (tnbs == v.size()) {
	  /* inplace-modification possible */
	  xt = (Niflib::Vector3 *)&bd[0];
	  xb = (Niflib::Vector3 *)&bd[bd.size() >> 1];
	}
	else
	  xtnb = brokentangentsp = true;
      }

      break;
    }
  }

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

  {
    vector<Niflib::Triangle>::iterator itf;
    for (itf = f.begin(); itf != f.end(); itf++) {
      struct Triangle3d tri;

      memset(&tri, 0, sizeof(tri));

      /* the original vertex indices (for remapping skin weights)
       */
      tri.p1.odx = (*itf).v1;
      tri.p2.odx = (*itf).v2;
      tri.p3.odx = (*itf).v3;

      /* fe.: OWC - ND\Meshes\Furniture\UpperClass\UpperDoubleBed06.NIF */
      if (((int)tri.p1.odx > vc) ||
	  ((int)tri.p2.odx > vc) ||
	  ((int)tri.p3.odx > vc))
	throw runtime_error("Face-index out-of-bounds. Corrupt face, NifSkope will crash opening this file.");

      /* --------------------------------------- */
      tri.p1.x = v[(*itf).v1].x;
      tri.p1.y = v[(*itf).v1].y;
      tri.p1.z = v[(*itf).v1].z;

      if (c.size()) {
	tri.p1.r = c[(*itf).v1].r;
	tri.p1.g = c[(*itf).v1].g;
	tri.p1.b = c[(*itf).v1].b;
	tri.p1.a = c[(*itf).v1].a;
      }

      if (n.size()) {
	tri.p1.nx = n[(*itf).v1].x;
	tri.p1.ny = n[(*itf).v1].y;
	tri.p1.nz = n[(*itf).v1].z;
      }

      if (t.size()) {
	tri.p1.tx = t[(*itf).v1].x;
	tri.p1.ty = t[(*itf).v1].y;
	tri.p1.tz = t[(*itf).v1].z;
      }

      if (xt) {
	tri.p1.tx = xt[(*itf).v1].x;
	tri.p1.ty = xt[(*itf).v1].y;
	tri.p1.tz = xt[(*itf).v1].z;
      }

      if (b.size()) {
	tri.p1.bx = b[(*itf).v1].x;
	tri.p1.by = b[(*itf).v1].y;
	tri.p1.bz = b[(*itf).v1].z;
      }

      if (xb) {
	tri.p1.bx = xb[(*itf).v1].x;
	tri.p1.by = xb[(*itf).v1].y;
	tri.p1.bz = xb[(*itf).v1].z;
      }

      for (int uvs = 0; uvs < uvc; uvs++) {
	tri.p1.uv[uvs].u = uv[uvs][(*itf).v1].u;
	tri.p1.uv[uvs].v = uv[uvs][(*itf).v1].v;
      }

      /* --------------------------------------- */
      tri.p2.x = v[(*itf).v2].x;
      tri.p2.y = v[(*itf).v2].y;
      tri.p2.z = v[(*itf).v2].z;

      if (c.size()) {
	tri.p2.r = c[(*itf).v2].r;
	tri.p2.g = c[(*itf).v2].g;
	tri.p2.b = c[(*itf).v2].b;
	tri.p2.a = c[(*itf).v2].a;
      }

      if (n.size()) {
	tri.p2.nx = n[(*itf).v2].x;
	tri.p2.ny = n[(*itf).v2].y;
	tri.p2.nz = n[(*itf).v2].z;
      }

      if (t.size()) {
	tri.p2.tx = t[(*itf).v2].x;
	tri.p2.ty = t[(*itf).v2].y;
	tri.p2.tz = t[(*itf).v2].z;
      }

      if (xt) {
	tri.p2.tx = xt[(*itf).v2].x;
	tri.p2.ty = xt[(*itf).v2].y;
	tri.p2.tz = xt[(*itf).v2].z;
      }

      if (b.size()) {
	tri.p2.bx = b[(*itf).v2].x;
	tri.p2.by = b[(*itf).v2].y;
	tri.p2.bz = b[(*itf).v2].z;
      }

      if (xb) {
	tri.p2.bx = xb[(*itf).v2].x;
	tri.p2.by = xb[(*itf).v2].y;
	tri.p2.bz = xb[(*itf).v2].z;
      }

      for (int uvs = 0; uvs < uvc; uvs++) {
	tri.p2.uv[uvs].u = uv[uvs][(*itf).v2].u;
	tri.p2.uv[uvs].v = uv[uvs][(*itf).v2].v;
      }

      /* --------------------------------------- */
      tri.p3.x = v[(*itf).v3].x;
      tri.p3.y = v[(*itf).v3].y;
      tri.p3.z = v[(*itf).v3].z;

      if (c.size()) {
	tri.p3.r = c[(*itf).v3].r;
	tri.p3.g = c[(*itf).v3].g;
	tri.p3.b = c[(*itf).v3].b;
	tri.p3.a = c[(*itf).v3].a;
      }

      if (n.size()) {
	tri.p3.nx = n[(*itf).v3].x;
	tri.p3.ny = n[(*itf).v3].y;
	tri.p3.nz = n[(*itf).v3].z;
      }

      if (t.size()) {
	tri.p3.tx = t[(*itf).v3].x;
	tri.p3.ty = t[(*itf).v3].y;
	tri.p3.tz = t[(*itf).v3].z;
      }

      if (xt) {
	tri.p3.tx = xt[(*itf).v3].x;
	tri.p3.ty = xt[(*itf).v3].y;
	tri.p3.tz = xt[(*itf).v3].z;
      }

      if (b.size()) {
	tri.p3.bx = b[(*itf).v3].x;
	tri.p3.by = b[(*itf).v3].y;
	tri.p3.bz = b[(*itf).v3].z;
      }

      if (xb) {
	tri.p3.bx = xb[(*itf).v3].x;
	tri.p3.by = xb[(*itf).v3].y;
	tri.p3.bz = xb[(*itf).v3].z;
      }

      for (int uvs = 0; uvs < uvc; uvs++) {
	tri.p3.uv[uvs].u = uv[uvs][(*itf).v3].u;
	tri.p3.uv[uvs].v = uv[uvs][(*itf).v3].v;
      }

      /* the collector considers the entire data-set of a vertex
       * to determine "unique" vertices
       */
      CollectFace(&tri);
    }

    /* register what happened */
    degeneratevertx = degeneratevertx || (v.size() != Vertices.size());
    degeneratefaces = degeneratefaces || (f.size() != Faces.size()   );

    nfoprintf(stdout, " Vertices         : %6d to %6d (%.4f%%)\n", v.size(), Vertices.size(), 100.0f * Vertices.size() / v.size());
    nfoprintf(stdout, " Faces            : %6d to %6d (%.4f%%)\n", f.size(), Faces.size()   , 100.0f * Faces.size()    / f.size());
  }

  CalculateGeometryNormals();
  IndexGeometry((int)v.size());
  OptimizeGeometry();

  if (bx) {
    /* this is a copy? */
    bd.resize(SectorVerticeO.size() * sizeof(Niflib::Vector3) * 2);

    /* inplace-modification possible */
    xt = (Niflib::Vector3 *)&bd[0];
    xb = (Niflib::Vector3 *)&bd[bd.size() >> 1];
  }

  {
    viterator itv;
    fiterator itf;
    int idx = 0;

    vector<Niflib::Vector3> _v;
    vector<Niflib::Color4> _c;
    vector<Niflib::TexCoord> _uv[16];
    vector<Niflib::Vector3> _t;
    vector<Niflib::Vector3> _n;
    vector<Niflib::Vector3> _b;
    vector<Niflib::Triangle> _f;

    for (itv = SectorVerticeO.begin(); itv != SectorVerticeO.end(); itv++) {
      /* assign index the moment of writing it out */
      (*itv)->idx = idx++;

      _v.push_back(Niflib::Vector3(
	(float)(*itv)->x,
	(float)(*itv)->y,
	(float)(*itv)->z)
      );

      if (c.size())
	_c.push_back(Niflib::Color4(
	  (float)(*itv)->r,
	  (float)(*itv)->g,
	  (float)(*itv)->b,
	  (float)(*itv)->a)
	);

      for (int uvs = 0; uvs < uvc; uvs++) {
	_uv[uvs].push_back(Niflib::TexCoord(
	  (float)(*itv)->uv[uvs].u,
	  (float)(*itv)->uv[uvs].v)
	);
      }

      if (n.size())
	_n.push_back(Niflib::Vector3(
	  (float)(*itv)->nx,
	  (float)(*itv)->ny,
	  (float)(*itv)->nz)
	);

      if (t.size())
	_t.push_back(Niflib::Vector3(
	  (float)(*itv)->tx,
	  (float)(*itv)->ty,
	  (float)(*itv)->tz)
	);

      if (xt)
	xt[(*itv)->idx].x = (float)(*itv)->tx,
	xt[(*itv)->idx].y = (float)(*itv)->ty,
	xt[(*itv)->idx].z = (float)(*itv)->tz;

      if (b.size())
	_b.push_back(Niflib::Vector3(
	  (float)(*itv)->bx,
	  (float)(*itv)->by,
	  (float)(*itv)->bz)
	);

      if (xb)
	xb[(*itv)->idx].x = (float)(*itv)->bx,
	xb[(*itv)->idx].y = (float)(*itv)->by,
	xb[(*itv)->idx].z = (float)(*itv)->bz;
    }

    for (itf = SectorFaceO.begin(); itf != SectorFaceO.end(); itf++) {
      _f.push_back(Niflib::Triangle(
	(*itf)->v[0]->idx,
	(*itf)->v[1]->idx,
	(*itf)->v[2]->idx
      ));
    }

//  data->SetVertexCount(vc);
//  data->SetUVSetCount(uvc);
//  data->SetVertexIndexCount(vic);

    data->SetTspaceFlag(tf);
    data->SetHasNormals(nf);

    data->SetVertices(_v);
    data->SetTriangles(_f);
    data->SetVertexColors(_c);

    /* UV before tangent-space! */
    for (int uvs = 0; uvs < uvc; uvs++)
      data->SetUVSet(uvs, _uv[uvs]);

    /* set TNB last */
    data->SetNormals(_n);
    data->SetTangents(_t);
    data->SetBinormals(_b);

    /* if it's in extra-data do it there */
    if (bx)
      bx->SetData(bd);
    /* if it's in extra-data do it there */
    if (xtnb)
      node->UpdateTangentSpace();

    /* remap the morph vertices */
    list< Ref<NiTimeController> > o = node->GetControllers();
    list< Ref<NiTimeController> >::iterator oi;
    for (oi = o.begin(); oi != o.end(); oi++) {
      NiGeomMorpherControllerRef gmc;
      if ((gmc = DynamicCast<NiGeomMorpherController>(*oi))) {
	NiMorphDataRef md;
	if ((md = gmc->GetData())) {
	  /* walk over every morpher */
	  int mtc = md->GetMorphCount();
	  if (mtc > 0) {
	    /* copy them out first because we have to
	     * change the size for all at once
	     */
	    vector<Vector3> mt[256];
	    for (int mts = 0; mts < mtc; mts++)
	      mt[mts] = md->GetMorphVerts(mts);

	    /* register what we did */
	    brokenmorphvrtx = brokenmorphvrtx || (md->GetVertexCount() != v.size());

	    /* set the new size */
	    md->SetVertexCount((int)_v.size());

	    for (int mts = 0; mts < mtc; mts++) {
	      vector<Vector3> mc;

	      /* contains degenerate vertices as well */
	      if (md->GetVertexCount() == v.size()) {
		/* old to new to opt */
		int vc = (int)v.size();
		for (int vs = 0; vs < vc; vs++) {
		  /* skip eliminated indices (count inclusive) */
		  int oidx = vs;
		  int nidx = SectorRemap[oidx];
		  if (nidx < 0) continue;
		  int pidx = SectorRemapO[nidx];

		  int stz = (int)mc.size();
		  if (stz <= pidx)
		    mc.resize(pidx + 1);

		  mc[pidx] = mt[mts][oidx];
		}
	      }
	      /* does not contain degenerate vertices */
	      else if (md->GetVertexCount() == _v.size()) {
		/* fe. (unbelievable, a original mesh):
		 *  Oblivion - Meshes\meshes\creatures\ghost\lefthandemissive.nif
		 *  Oblivion WarCry\meshes\OE\Creatures\Skeleton Variants\Leoric Skull.nif
		 */

		/* old to new to opt */
		int vc = (int)v.size();
		for (int vs = 0, vh = 0; vs < vc; vs++) {
		  /* skip eliminated indices (count exclusive) */
		  int sidx = vs;
		  int nidx = SectorRemap[sidx];
		  if (nidx < 0) continue;
		  int oidx = vh++;
		  int pidx = SectorRemapO[nidx];

		  int stz = (int)mc.size();
		  if (stz <= pidx)
		    mc.resize(pidx + 1);

		  mc[pidx] = mt[mts][oidx];
		}
	      }
	      /* kaputt */
	      else
		continue;

	      assert(mc.size() == _v.size());

	      /* register what we did (repaired it) */
	      repairmorphvrtx = repairmorphvrtx || (brokenmorphvrtx && (mc.size() == _v.size()));

	      md->SetMorphVerts(mts, mc);
	    }
	  }
	}
      }
    }

    /* look if we also can process a possible tri/egm now */
    optimize_tri(v, f, uv[0], _v, _f, _uv[0]);
    optimize_egm(v          , _v            );

    /* remap the vertex weights and the vertex map */
    NiSkinInstanceRef skn = node->GetSkinInstance();
    if (skn) {
      NiSkinDataRef wgt = skn->GetSkinData();
      unsigned int bc = wgt->GetBoneCount();

      for (unsigned int bs = 0; bs < bc; bs++) {
	vector<SkinWeight> vw = wgt->GetBoneWeights(bs);
	vector<SkinWeight> cw;
	vector<SkinWeight>::iterator vwi;

	/* register what we did */
	brokenbnweights = brokenbnweights || (vw.size() > v.size());

	/* old to new to opt */
	for (vwi = vw.begin(); vwi != vw.end(); vwi++) {
	  /* skip eliminated indices */
	  int oidx = (*vwi).index;
	  int nidx = SectorRemap[oidx];
	  if (nidx < 0) continue;
	  int pidx = SectorRemapO[nidx];

	  /* */ {
	    (*vwi).index = pidx;

	    /* we always need to dynamically grow to support skips */
	    cw.push_back(*vwi);
	  }
	}

	assert(cw.size() <= v.size());

	/* remove duplicates (index:weight pairs are freely sortable) */
	std::sort(cw.begin(), cw.end());
	cw.erase(std::unique(cw.begin(), cw.end()), cw.end());

	assert(cw.size() <= _v.size());

	/* register what we did (repaired it) */
	repairbnweights = repairbnweights || (brokenbnweights && (cw.size() <= _v.size()));

	if (cw.size() > 0)
	  /* via node, so we don't calc center/radius */
//	  node->SetBoneWeights(bs, cw);
	  /* via wgt, so we don't modify center/radius */
	  wgt->SetBoneWeights(bs, cw);
      }

      NiSkinPartitionRef prt = skn->GetSkinPartition();
      if (prt) {
	int pc = prt->GetNumPartitions();
	for (int ps = 0; ps < pc; ps++) {
	  struct PartitionData {
	    unsigned short index;
	    vector<float> weights;
	    vector<unsigned short> bones;

	    PartitionData() {};

	    bool operator < (const struct PartitionData &other) const {
	      return index < other.index;
	    }

	    bool operator == (const struct PartitionData &other) const {
	      return index == other.index;
	    }
	  };

	  /* copy vertex mapping */
	  vector<unsigned short> vm = prt->GetVertexMap(ps);
	  vector<unsigned short>::iterator vmi;

	  /* define triangles in terms of global indices */
	  vector<Niflib::Triangle> vf = prt->GetTriangles(ps);
	  vector<Niflib::Triangle>::iterator vfi;

	  /* log conjunct data acording to the base mesh */
	  vector<PartitionData> pd;
	  vector<PartitionData>::iterator pdi;
	  int pos = 0;
	  int ovm = (int)vm.size();
	  int ovf = (int)vf.size();

	  /* register what we did */
	  brokenpartition = brokenpartition || ((pc == 1) && (ovm != v.size()));

	  /* this doesn't go! */
	  if (vm.size() == 0)
	    throw runtime_error("VertexMap must contain entries.");

	  /* old to new to opt */
	  for (vfi = vf.begin(); vfi != vf.end(); vfi++) {
	    /* "triangle-index" points to "local-index" maps to "global-index" */
	    int oidx1 = vm[(*vfi).v1];
	    int oidx2 = vm[(*vfi).v2];
	    int oidx3 = vm[(*vfi).v3];
	    int nidx1 = SectorRemap[oidx1];
	    int nidx2 = SectorRemap[oidx2];
	    int nidx3 = SectorRemap[oidx3];
	    /* skip eliminated indices */
	    int pidx1 = (nidx1 < 0 ? -1 : SectorRemapO[nidx1]);
	    int pidx2 = (nidx2 < 0 ? -1 : SectorRemapO[nidx2]);
	    int pidx3 = (nidx3 < 0 ? -1 : SectorRemapO[nidx3]);

	    /* we can't cast -1 to 65535, so make it
	     * degenerate instead
	     */
	    if ((pidx1 < 0) || (pidx2 < 0) || (pidx3 < 0))
	      pidx1 = pidx2 = pidx3 = 0;

	    assert(pidx1 < (int)SectorVerticeO.size());
	    assert(pidx2 < (int)SectorVerticeO.size());
	    assert(pidx3 < (int)SectorVerticeO.size());

	    (*vfi).v1 = pidx1;
	    (*vfi).v2 = pidx2;
	    (*vfi).v3 = pidx3;
	  }

	  /* old to new to opt */
	  for (pos = 0, vmi = vm.begin(); vmi != vm.end(); vmi++, pos++) {
	    /* skip eliminated indices */
	    int oidx = (*vmi);
	    int nidx = SectorRemap[oidx];
	    if (nidx < 0) continue;
	    int pidx = SectorRemapO[nidx];

	    assert(pidx < (int)SectorVerticeO.size());

	    /* */ {
	      struct PartitionData pdd;

	      /* get the aux data */
	      pdd.index = pidx;
	      if (prt->HasVertexWeights(ps))
		pdd.weights = prt->GetVertexWeights(ps, pos);
	      if (prt->HasVertexBoneIndices(ps))
		pdd.bones   = prt->GetVertexBoneIndices(ps, pos);

	      /* we always need to dynamically grow to support skips */
	      pd.push_back(pdd);
	    }
	  }

	  assert(pd.size() <= vm.size());

	  /* remove duplicates (index:weight pairs are freely sortable) */
	  std::sort(pd.begin(), pd.end());
	  pd.erase(std::unique(pd.begin(), pd.end()), pd.end());
	  vm.resize(pd.size());

	  assert(pd.size() <= _v.size());

	  /* reconfigure data */
	  prt->SetNumVertices(ps, (unsigned short)pd.size());
	  prt->EnableVertexWeights(ps, prt->HasVertexWeights(ps));
	  prt->EnableVertexBoneIndices(ps, prt->HasVertexBoneIndices(ps));

	  /* reinstanciate the triangles from global to local indices */
	  vector<unsigned short> map;

	  /* move the stuff back into the partition */
	  for (pos = 0, pdi = pd.begin(); pdi != pd.end(); pdi++, pos++) {
	    unsigned short pidx = (*pdi).index;

	    /* local to global map */
	    vm[pos] = pidx;

	    if (prt->HasVertexWeights(ps))
	      prt->SetVertexWeights(ps, pos, (*pdi).weights);
	    if (prt->HasVertexBoneIndices(ps))
	      prt->SetVertexBoneIndices(ps, pos, (*pdi).bones);

	    /* global to local map */
	    int stz = (int)map.size();
	    if (stz <= pidx)
	      map.resize(pidx + 1);
	    map[pidx] = pos;
	  }

	  /* register what we did (repaired it) */
	  repairpartition = repairpartition || (brokenpartition && (vm.size() == _v.size()));

	  /* redefine triangles in terms of local indices */
	  vector<Niflib::Triangle> lvf;
	  vector<Niflib::Triangle>::iterator lvfi;

	  /* optimize */
	  if (optimizeparts) {
	    /* spit out some info */
	    barprintf(stdout, '.', "%s %d", "skinpartition", ps);
	    nfoprintf(stdout, "processing:\n");

	    /* reinstanciate the triangles from global to local indices */
	    vector<Vertex3D> lv;
	    vector<Vertex3D>::iterator lvi;

	    /* move the stuff back into the partition */
	    for (pdi = pd.begin(); pdi != pd.end(); pdi++) {
	      /* global index of local vertex */
	      unsigned short pidx = (*pdi).index;
	      struct Vertex3D v;

	      /* global position of local vertex */
	      v.x = (float)SectorVerticeO[pidx]->x;
	      v.y = (float)SectorVerticeO[pidx]->y;
	      v.z = (float)SectorVerticeO[pidx]->z;

	      /* append to vertex array */
	      lv.push_back(v);
	    }

	    /* remapped local indices */
	    vector<unsigned int> lm;
	    vector<unsigned int>::iterator lmi;

	    /* reinstanciate the triangles from global to local indices */
	    vector<unsigned int> lf;
	    vector<unsigned int>::iterator lfi;

	    /* old to new to opt */
	    for (vfi = vf.begin(); vfi != vf.end(); vfi++) {
	      /* "triangle-index" points to "global-index" maps to "local-index" */
	      int v1 = (*vfi).v1; v1 = map[v1];
	      int v2 = (*vfi).v2; v2 = map[v2];
	      int v3 = (*vfi).v3; v3 = map[v3];

	      /* remove degenerate face cases, the surface is a closed
	       * mesh, so the adjacent vertices will snap as well and
	       * close this "hole"
	       */
	      if ((v1 == v2) || (v1 < 0))
		continue;
	      if ((v1 == v3) || (v2 < 0))
		continue;
	      if ((v2 == v3) || (v3 < 0))
		continue;

	      /* very expensive search without sets */
	      if (f.size() != _f.size()) {
		unsigned int f1 = (*vfi).v1;
		unsigned int f2 = (*vfi).v2;
		unsigned int f3 = (*vfi).v3, fs, ss;
		bool matched = false;

		if (f2 < f1) { fs = f1; f1 = f2; f2 = fs; }
		if (f3 < f1) { fs = f1; f1 = f3; f3 = fs; }
		if (f3 < f2) { fs = f2; f2 = f3; f3 = fs; }

		/* this is a replica of the face-rejector for different
		 * percolations used in "register_checked_face"
		 */
		vector<Niflib::Triangle>::iterator sfi;
		for (sfi = vf.begin(); sfi != vfi; sfi++) {
		  {
		    unsigned int s1 = (*sfi).v1;
		    unsigned int s2 = (*sfi).v2;
		    unsigned int s3 = (*sfi).v3;

		    if (s2 < s1) { ss = s1; s1 = s2; s2 = ss; }
		    if (s3 < s1) { ss = s1; s1 = s3; s3 = ss; }
		    if (s3 < s2) { ss = s2; s2 = s3; s3 = ss; }

		    /* compare each face for vertex 1 with the faces of vertex 2 */
		    if ((f1 == s1) && (f2 == s2) && (f3 == s3)) {
		      matched = true; break;
		    }
		  }
		}

		if (matched)
		  continue;
	      }

	      lf.push_back(v1);
	      lf.push_back(v2);
	      lf.push_back(v3);
	    }

	    assert((lv.size() / 1) <= _v.size());
	    assert((lf.size() / 3) <= _f.size());

	    nfoprintf(stdout, " Vertices         : %6d to %6d (%.4f%%)\n", ovm      , lv.size() / 1, 100.0f * (lv.size() / 1) / ovm      );
	    nfoprintf(stdout, " Faces            : %6d to %6d (%.4f%%)\n", vf.size(), lf.size() / 3, 100.0f * (lf.size() / 3) / vf.size());

	    /* we actually can reorder the vertices! */
	    OptimizeGeometry(lv, lf, lm);

#if 0
	    /* TODO: make this work */
	    assert(lm.size() == vm.size());
	    vector<unsigned short> vo = vm;
	    for (pos = 0, lmi = lm.begin(); lmi != lm.end(); lmi++, pos++) {
	      /* "local-index" points to "global-index" */
	      vm[*lmi] = vo[pos];
	    }
#endif

	    lfi = lf.begin();
	    while (lfi != lf.end()) {
	      /* "triangle-index" points to "local-index" */
	      unsigned int v1 = (*lfi); lfi++;
	      unsigned int v2 = (*lfi); lfi++;
	      unsigned int v3 = (*lfi); lfi++;

	      lvf.push_back(Niflib::Triangle(v1, v2, v3));
	    }
	  }
	  else {
	    /* define triangles in terms of global indices */
	    vector<Niflib::Triangle> vf = prt->GetTriangles(ps);
	    vector<Niflib::Triangle>::iterator vfi;

	    /* old to new to opt */
	    for (vfi = vf.begin(); vfi != vf.end(); vfi++) {
	      /* "triangle-index" points to "global-index" maps to "local-index" */
	      int v1 = (*vfi).v1; v1 = map[v1];
	      int v2 = (*vfi).v2; v2 = map[v2];
	      int v3 = (*vfi).v3; v3 = map[v3];

	      /* remove degenerate face cases, the surface is a closed
	       * mesh, so the adjacent vertices will snap as well and
	       * close this "hole"
	       */
	      if ((v1 == v2) || (v1 < 0))
		continue;
	      if ((v1 == v3) || (v2 < 0))
		continue;
	      if ((v2 == v3) || (v3 < 0))
		continue;

	      lvf.push_back(Niflib::Triangle(v1, v2, v3));
	    }
	  }

	  /* set sorted or remapped vertex-map */
	  prt->SetVertexMap(ps, vm);

	  /* remove strips, add lists */
	  prt->SetStripCount(ps, 0);
	  prt->SetTriangles(ps, lvf);

	  /* register what happened */
	  degeneratevertx = degeneratevertx || ( vm.size() != ovm);
	  degeneratefaces = degeneratefaces || (lvf.size() != ovf);
	}
      }
    }
  }

  /* throw match-groups out */
  if (data->HasMatchData()) {
    data->RemoveMatchDetection();

    if (verbose)
      addnote(" Removed optional match-groups\n");
  }

  FreeGeometry();

  if (brokenmorphvrtx || brokenbnweights || brokenpartition ||
      degeneratevertx || degeneratefaces || brokentangentsp) {
    if (brokentangentsp)
      addnote(" Tangent-space was broken (%d vectors), fixed!\n", tnbs), fixedmeshes++;

    if (degeneratevertx && !degeneratefaces)
      addnote(" Removed degenerate vertices (vertices without faces).\n"), modifiedmeshes++;
    else if (!degeneratevertx && degeneratefaces)
      addnote(" Removed degenerate faces (faces without area or duplicate faces).\n"), modifiedmeshes++;
    else if (degeneratevertx && degeneratefaces)
      addnote(" Removed identical vertices and their faces.\n"), modifiedmeshes++;

    if (brokenmorphvrtx && repairmorphvrtx)
      addnote(" Morph had different number of indices to base-mesh, fixed!\n"), fixedmeshes++;
    else if (brokenmorphvrtx)
      addnote(" Morph has different number of indices to base-mesh!\n"), meshdamages++;

    if (brokenbnweights && repairbnweights)
      addnote(" Bone had different number of weights to base-mesh, fixed!\n"), fixedmeshes++;
    else if (brokenbnweights)
      addnote(" Bone has different number of weights to base-mesh!\n"), meshdamages++;

    if (brokenpartition && repairpartition)
      addnote(" SkinPartition had different number of vertices to base-mesh, fixed!\n"), fixedmeshes++;
    else if (brokenpartition)
      addnote(" SkinPartition has different number of vertices to base-mesh!\n"), meshdamages++;
  }

  return node->GetType().GetTypeName();
}

void StripNode(NiAVObjectRef node) {
  vector< Ref<NiProperty> > prps = node->GetProperties();
  vector< Ref<NiProperty> >::iterator pi;
  for (pi = prps.begin(); pi != prps.end(); pi++) {
    string pt = (*pi)->GetType().GetTypeName();

    if ((pt == "NiVertexColorProperty") ||
	(pt == "NiAlphaProperty") ||
	(pt == "NiSpecularProperty"))
      node->RemoveProperty(*pi);
    /* PM-removal only if barestrip */
    else if ((pt == "NiTexturingProperty") && barestripfar) {
      NiTexturingPropertyRef tx = DynamicCast<NiTexturingProperty>(*pi);

      if (tx->GetApplyMode() == APPLY_HILIGHT2)
	tx->SetApplyMode(APPLY_MODULATE);
    }
  }

  /* kill TB, kill other extra data */
  list< Ref<NiExtraData> > x = node->GetExtraData();
  list< Ref<NiExtraData> >::iterator xd;
  for (xd = x.begin(); xd != x.end(); xd++) {
    /* TB-removal only if barestrip */
    if (DynamicCast<NiBinaryExtraData>(*xd) && barestripfar) {
      if (!strcmp("Tangent space (binormal & tangent vectors)", (*xd)->GetName().data()))
	node->RemoveExtraData(*xd);
    }
    else if (DynamicCast<NiStringExtraData>(*xd)) {
      node->RemoveExtraData(*xd);
    }
    else if (DynamicCast<NiIntegerExtraData>(*xd)) {
      if (DynamicCast<BSXFlags>(*xd))
	node->RemoveExtraData(*xd);
    }
  }

  /* kill controllers */
  list< Ref<NiTimeController> > o = node->GetControllers();
  list< Ref<NiTimeController> >::iterator oi;
  for (oi = o.begin(); oi != o.end(); oi++) {
    node->RemoveController(*oi);
  }

  /* kill the collision object */
  NiCollisionObjectRef col = node->GetCollisionObject();
  if (col) {
    node->SetCollisionMode(NiAVObject::CollisionType::CT_NONE);
    node->SetCollisionObject(NULL);
  }
}

void ModifyNode(NiAVObjectRef node) {
  vector< Ref<NiProperty> > prps = node->GetProperties();
  vector< Ref<NiProperty> >::iterator pi;
  for (pi = prps.begin(); pi != prps.end(); pi++) {
    Type py = (*pi)->GetType();
    string pt = py.GetTypeName();

    if ((pt == "NiAlphaProperty")) {
      NiAlphaPropertyRef tr = DynamicCast<NiAlphaProperty>(*pi);

      /* collect alpha-infos for textures */
//    texmode[] = true;
    }
    else if ((pt == "NiMaterialProperty")) {
    }
    else if ((pt == "NiTexturingProperty")) {
      NiTexturingPropertyRef tx = DynamicCast<NiTexturingProperty>(*pi);

      switch (tx->GetApplyMode()) {
	/* (eradicate all underlying blended textures?) */
	case APPLY_REPLACE: break;
	/* decal surface (must-have alpha, no illumination) */
	case APPLY_DECAL: break;
	/* regular surface (can-have alpha) */
	case APPLY_MODULATE: break;
	/* reflective surface (no diffuse, no alpha) */
	case APPLY_HILIGHT: break;
	/* parallax surface (no alpha but height-map) */
	case APPLY_HILIGHT2: break;
      }

      /* turn on parallax mapping */
      if (parallaxmapping) {
	if (tx->GetApplyMode() == APPLY_MODULATE)
	  tx->SetApplyMode(APPLY_HILIGHT2);
      }

      /* make texture-paths sane */
      if (texturepaths) {
	int txc = tx->GetTextureCount();
	for (int txs = 0; txs < txc; txs++) {
	  TexDesc txd = tx->GetTexture(txs);
	  NiSourceTextureRef src = txd.source;

	  if (src && src->IsTextureExternal()) {
	    string path = src->GetTextureFileName();
	    string pcpy = path;

	    if (path.length() == 0) {
//	      addnote(" Texture-path is empty, the file needs some revision.!\n");
	      tx->ClearTexture(txs);
	      continue;
	    }

	    std::replace(path.begin(), path.end(), '/', '\\');
	    while (path[0] == '\\')
	      path = path.substr(1);

	    string srch = path;
	    std::transform(srch.begin(), srch.end(), srch.begin(), ::tolower);
	    size_t found = srch.find("data\\");
	    if (found != string::npos)
	      path = path.substr(found + 5);
	    if (compressimages)
	      putext(path, path, "dds");

	    if (path.length() == 0)
	      addnote(" Texture-path becomes empty: \"%s\".\n", pcpy.data());
	    else if (path != pcpy) {
	      src->SetExternalTexture(path);
	      texpths.push_back(pcpy), fixedpaths++;

	      if (verbose)
		addnote(" Fixed texture-path: \"%s\".\n", pcpy.data());
	    }
	  }
	}

	bool tset = false;
	for (int txs = 0; txs < txc; txs++)
	  tset = tset || tx->HasTexture(txs);

#if 0
	int stc = tx->GetShaderTextureCount();
	for (int sts = 0; sts < stc; sts++) {
	  TexDesc std = tx->GetShaderTexture(sts);
	  NiSourceTextureRef src = std.source;

	  if (src && src->IsTextureExternal()) {
	    string path = src->GetTextureFileName();
	    string pcpy = path;

	    if (path.length() == 0) {
//	      addnote(" Texture-path is empty, the file needs some revision.!\n");
	      tx->ClearShaderTexture(sts);
	      continue;
	    }

	    std::replace(path.begin(), path.end(), '/', '\\');
	    while (path[0] == '\\')
	      path = path.substr(1);

	    string srch = path;
	    std::transform(srch.begin(), srch.end(), srch.begin(), ::tolower);
	    size_t found = srch.find("data\\");
	    if (found != string::npos)
	      path = path.substr(found + 5);
	    if (compressimages)
	      putext(path, path, "dds");

	    if (path.length() == 0)
	      addnote(" Texture-path becomes empty: \"%s\".\n", pcpy.data());
	    else if (path != pcpy) {
	      src->SetExternalTexture(path);
	      texpths.push_back(pcpy), fixedpaths++;

	      if (verbose)
		addnote(" Fixed texture-path: \"%s\".\n", pcpy.data());
	    }
	  }
	}

	bool sset = false;
	for (int txs = 0; txs < txc; txs++)
	  sset = sset || tx->HasShaderTexture(txs);
#endif
	bool sset = !!tx->GetShaderTextureCount();

	/* kill empty TexturingProperties */
	if (!tset && !sset) {
	  node->RemoveProperty(tx);
	  addnote(" Removed empty texturing-property\n");
	}
      }
    }
  }
}

/* corresponds to: SpellCleanRefLists */
void SanitizeNode(NiAVObjectRef av, NiNodeRef nd, NiObjectRef ob) {
  if (av) {
    /* kill empty extra-data */
    av->RemoveExtraData(NULL);
    /* kill empty controllers */
    av->RemoveController(NULL);
    /* kill empty properties */
    av->RemoveProperty(NULL);

    {
      vector< Ref<NiProperty> > prps = av->GetProperties();
      vector< Ref<NiProperty> >::iterator pi;
      for (pi = prps.begin(); pi != prps.end(); pi++) {
	Type py = (*pi)->GetType();
	string pt = py.GetTypeName();

	if ((pt == "NiMaterialProperty")) {
	  NiMaterialPropertyRef mt = DynamicCast<NiMaterialProperty>(*pi);

	  /* make alpha-values sane (though in reality these could
	   * be use to nice effect, it's just math in the end)
	   */
	  float alpha = mt->GetTransparency();

	  /**/ if (alpha < 0.0) alpha = 0.0, (verbose ? addnote(" Fixed too small transparency.\n") : 0);
	  else if (alpha > 1.0) alpha = 1.0, (verbose ? addnote(" Fixed too big transparency.\n") : 0);

	  mt->SetTransparency(alpha);
	}
      }
    }

    {
      NiTriBasedGeomRef geo = DynamicCast<NiTriBasedGeom>(av);
      NiGeometryDataRef raw = (geo ? geo->GetData() : NULL);

      if (raw) {
	bool xtnb = false; size_t tnbs = 0;
	NiBinaryExtraDataRef bx = NULL;
	NiStringExtraDataRef sx = NULL;
	NiTriShapeDataRef    tr = NULL;

	/* find out if we have TB-vectors in the extra data */
	list< Ref<NiExtraData> > x = geo->GetExtraData();
	list< Ref<NiExtraData> >::iterator xd;
	for (xd = x.begin(); xd != x.end(); xd++) {
	  /* check TNB */
	  if (!optimizelists) {
	    if (!strcmp("Tangent space (binormal & tangent vectors)", (*xd)->GetName().data())) {
	      if ((bx = DynamicCast<NiBinaryExtraData>(*xd))) {
		tnbs = bx->GetData().size() / (sizeof(Niflib::Vector3) * 2);
		if (tnbs != raw->GetVertexCount())
		  xtnb = true;
	      }
	    }
	  }

	  /* remove empty extra-datas */
	  if ((bx = DynamicCast<NiBinaryExtraData>(*xd))) {
	    if (bx->GetData().size() == 0)
	      av->RemoveExtraData(*xd);
	  }
	  else if ((sx = DynamicCast<NiStringExtraData>(*xd))) {
	    if (sx->GetData().size() == 0)
	      av->RemoveExtraData(*xd);
	  }
	}

	if (xtnb) {
	  adderrr("Tangent-space is broken (%d vectors don't match %d vertices)!\n", tnbs, raw->GetVertexCount()), fixedmeshes++;
	  geo->UpdateTangentSpace();
	}
      }
    }
  }

  if (nd) {
    /* kill empty children */
    nd->RemoveChild(NULL);

    /* remove double children */
    vector<NiAVObjectRef> chlds = nd->GetChildren();
    vector<NiAVObjectRef>::iterator lastu = std::unique(chlds.begin(), chlds.end());

    /* there is some duplicate in there */
    if (lastu != chlds.end()) {
      /* we can't directly remove doubles ... this approach is without side-effects */
      nd->ClearChildren();
      for (vector<NiAVObjectRef>::iterator it = chlds.begin(); it != lastu; ++it)
	nd->AddChild(*it);

      if (verbose)
	addnote(" Removed children which occured multiple times\n");
    }
  }

  if (ob) {
    {
      bhkMoppBvTreeShapeRef bhk = DynamicCast<bhkMoppBvTreeShape>(ob);
      bhkShapeRef shp = (bhk ? bhk->GetShape() : NULL);
      bhkPackedNiTriStripsShapeRef pck = (shp ? DynamicCast<bhkPackedNiTriStripsShape>(shp) : NULL);
      bhkNiTriStripsShapeRef tri = (shp ? DynamicCast<bhkNiTriStripsShape>(shp) : NULL);

      if (bhk) {
	HavokMaterial mat = bhk->GetMaterial();
	bool broken = false;

	if (pck) {
	  vector<Niflib::OblivionSubShape> os;

	  /* reverse direction: lowest material highest priority */
	  os = pck->GetSubShapes();
	  int ss = (int)os.size() - 1;
	  while (ss >= 0) {
	    /* broken material */
	    if ((os[ss].material < HAV_MAT_STONE) ||
	        (os[ss].material > HAV_MAT_ELEVATOR)) {
	      os[ss].material = HAV_MAT_WOOD;
	      broken = true;
	    }

	    mat = os[ss].material;
	    ss--;
	  }

	  if (broken)
	    pck->SetSubShapes(os);
	}
	else if (tri) {
	  mat = tri->GetMaterial();

	  /* broken material */
	  if ((mat < HAV_MAT_STONE) || (mat > HAV_MAT_ELEVATOR)) {
	    tri->SetMaterial(mat = HAV_MAT_WOOD);
	    broken = true;
	  }
	}

	{
	  HavokMaterial bmt = bhk->GetMaterial();

	  /* broken material */
	  if ((bmt < HAV_MAT_STONE) || (bmt > HAV_MAT_ELEVATOR)) {
	    if ((mat < HAV_MAT_STONE) || (mat > HAV_MAT_ELEVATOR))
	      bhk->SetMaterial(bmt = HAV_MAT_WOOD);
	    else
	      bhk->SetMaterial(bmt = mat);

	    if (verbose)
	      addnote(" Invalid parent-material replaced by child-material.\n");
	  }
	}

	if (verbose && broken)
	  addnote(" Invalid material replaced by Wood.\n");
      }
    }

    {
      bhkNiTriStripsShapeRef bhk = DynamicCast<bhkNiTriStripsShape>(ob);

      if (bhk) {
	HavokMaterial mat = bhk->GetMaterial();
	/* broken material */
	if ((mat < HAV_MAT_STONE) || (mat > HAV_MAT_ELEVATOR)) {
	  /* fe.: Better Cities - Meshes\meshes\bettercitiesresources\3dmaps\hbcyrodiilframe01.nif */
	  bhk->SetMaterial(HAV_MAT_WOOD);

	  if (verbose)
	    addnote(" Invalid material replaced by Wood.\n");
	}
      }
    }

    {
      NiMultiTargetTransformControllerRef ctrl = DynamicCast<NiMultiTargetTransformController>(ob);

      if (ctrl) {
	vector< Ref<NiAVObject> > trgt = ctrl->GetExtraTargets();
	vector< Ref<NiAVObject> >::iterator ti;
	for (ti = trgt.begin(); ti != trgt.end(); ti++) {
	  if (!*ti) {
	    ;
	  }
	}
      }
    }
  }
}

bool KillcheckNode(NiAVObjectRef node) {
  NiGeometryRef geo = DynamicCast<NiGeometry>(node);
  NiGeometryDataRef dat = (geo ? geo->GetData() : NULL);

  if (dat && (dat->GetVertexCount() < 3)) {
    nfoprintf(stdout, "\n %s had no contents, removed\n", node->GetName().data());

    return true;
  }

  /* TODO: add the collision-nodes too */

  return false;
}

void WalkNIF(NiObjectRef root, NiNodeRef parent, NiObjectRef pref = NULL, bool farnif = false) {
  string type = root->GetType().GetTypeName();
  bhkCollisionObjectRef co = DynamicCast<bhkCollisionObject>(root);
  NiControllerSequenceRef cs = DynamicCast<NiControllerSequence>(root);
  NiAVObjectRef av = DynamicCast<NiAVObject>(root);
  NiObjectRef ob = DynamicCast<NiObject>(root);
  NiNodeRef nd = DynamicCast<NiNode>(root);

  /* start the node-bar with this */
  nodebarchar = '+';

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
  SanitizeNode(av, nd, ob);

  /* do some preset stuff */
  if ((barestripfar || optimizefar) && farnif) {
    if (av) StripNode(av);
  }
  else {
    if (av) ModifyNode(av);
  }

  /* optimize keys */
  if ((type == "NiVisData") ||
      (type == "NiPosData") ||
      (type == "NiMorphData") ||
      (type == "NiBoolData") ||
      (type == "NiColorData") ||
      (type == "NiFloatData") ||
      (type == "NiKeyframeData") ||
      (type == "NiTransformData") ||
      (type == "NiTextKeyExtraData")) {
    if (optimizekeys) {
      string nodebarbak = nodebarref, wh;

      if (DynamicCast<NiInterpolator>(pref))
	wh = FindController(DynamicCast<NiInterpolator>(pref));
      if (wh != "") {
	nodebarref += ":";
	nodebarref += wh;
      }

      /**/ if (type == "NiBoolData")
	type = OptimizeNode(DynamicCast<NiBoolData>(root));
      else if (type == "NiTextKeyExtraData")
	type = OptimizeNode(DynamicCast<NiTextKeyExtraData>(root));
      else if (type == "NiVisData")
	type = OptimizeNode(DynamicCast<NiVisData>(root));
      else if (type == "NiMorphData")
	type = OptimizeNode(DynamicCast<NiMorphData>(root));
      else if (type == "NiColorData")
	type = OptimizeNode(DynamicCast<NiColorData>(root));
      else if (type == "NiFloatData")
	type = OptimizeNode(DynamicCast<NiFloatData>(root));
      else if (type == "NiPosData")
	type = OptimizeNode(DynamicCast<NiPosData>(root));
      else if (type == "NiKeyframeData")
	type = OptimizeNode(DynamicCast<NiKeyframeData>(root));
      else if (type == "NiTransformData")
	type = OptimizeNode(DynamicCast<NiKeyframeData>(root));

      nodebarref = nodebarbak;
    }
  }

  /* optimize geometry */
  if ((type == "NiTriStrips") ||
      (type == "NiTriShape") ||
      (type == "NiEnvMappedTriShape") ||
      (type == "BSSegmentedTriShape") ||
      (type == "bhkNiTriStripsShape") ||
      (type == "bhkPackedNiTriStripsShape") ||
      (type == "bhkMoppBvTreeShape")) {

    if (geotoprimitive) {
      /**/ if (type == "bhkMoppBvTreeShape")
	type = ConvertNode(DynamicCast<bhkMoppBvTreeShape>(root), parent, root);
      else if (type == "bhkNiTriStripsShape")
	type = ConvertNode(DynamicCast<bhkNiTriStripsShape>(root), parent, root);
      else if (type == "bhkPackedNiTriStripsShape")
	type = ConvertNode(DynamicCast<bhkPackedNiTriStripsShape>(root), parent, root);
    }

    if (stripstolists) {
      /**/ if (type == "bhkNiTriStripsShape")
	type = type; /* doesn't have list-form */
      else if (type == "bhkPackedNiTriStripsShape")
	type = type; /* doesn't have list-form */
      else if (type == "NiTriStrips")
	type = ConvertNode(DynamicCast<NiTriStrips>(root), parent, root);
      else if (type == "NiTriShape")
	type = ConvertNode(DynamicCast<NiTriShape>(root), parent, root);
    }

    if (optimizehavok) {
      /**/ if (type == "bhkMoppBvTreeShape")
	type = OptimizeNode(DynamicCast<bhkMoppBvTreeShape>(root));
      else if (type == "bhkNiTriStripsShape")
	type = OptimizeNode(DynamicCast<bhkNiTriStripsShape>(root));
      else if (type == "bhkPackedNiTriStripsShape")
	type = OptimizeNode(DynamicCast<bhkPackedNiTriStripsShape>(root));
    }

    if (optimizelists) {
      /**/ if (type == "BSSegmentedTriShape")
	type = OptimizeNode(DynamicCast<BSSegmentedTriShape>(root));
      else if (type == "NiEnvMappedTriShape")
	type = OptimizeNode(DynamicCast<NiEnvMappedTriShape>(root));
      else if (type == "NiTriShape")
	type = OptimizeNode(DynamicCast<NiTriShape>(root));
    }
  }
  else {
    list<NiObjectRef> links = root->GetRefs();
    for (list<NiObjectRef>::iterator it = links.begin(); it != links.end(); ++it) {
      WalkNIF(*it, nd, root, farnif);

      NiAVObjectRef ch = DynamicCast<NiAVObject>(*it);
      if (nd && ch && KillcheckNode(ch))
	nd->RemoveChild(ch);
    }
  }

  if (notes.size() > 0) {
    nfoprintf(stdout, "\nnotes:\n");
    for (size_t n = 0; n < notes.size(); n++)
      nfoprintf(stdout, notes[n].data());

    notes.clear();
  }
}

void MarkNIF(NiObjectRef root, NiNodeRef parent, bool inclusive) {
  NiNodeRef nd = DynamicCast<NiNode>(root);

  /* clear entry */
  if (inclusive) {
    vector<NiObjectRef>::iterator it;
    it = find(utilization.begin(), utilization.end(), root);
    if (it != utilization.end()) (*it) = NULL;
  }

  list<NiObjectRef> links = root->GetRefs();
  for (list<NiObjectRef>::iterator it = links.begin(); it != links.end(); ++it) {
    MarkNIF(*it, nd, true);
  }
}

bool RepairNIF(NiObjectRef root) {
  /* find first non-NULL entry */
  vector<NiObjectRef>::iterator first, last, walk, next;
  for (first = utilization.begin(); first != utilization.end(); first++)
    if (*first)
      break;
  /* none found */
  if (first == utilization.end())
    return false;

  /* get the type */
  Type typc = (*first)->GetType();
  string type = typc.GetTypeName();

  if (verbose)
    adderrr("Disconnected %s found! Attempting repair.\n", type.data());

  /* let's take a bit care here */
  do {
    /* the cases I know:
     * - all lost geometry starts with ExtraData
     * - and ends with ShapeData or StripData
     */
    for (last = first; last != utilization.end(); last++) {
      /* there may exist holes from unattached sub-nodes */
      if (*last) {
	/* don't overwrite "type" if we don't catch it */
	string stype = (*last)->GetType().GetTypeName();
	if (stype == "NiTriShapeData") {
	  type = stype; break; }
	if (stype == "NiTriStripsData") {
	  type = stype; break; }
      }

      /* TODO: check for double-occurances which also could mark
       * borders of lost blocks:
       * "NiSpecularProperty" belongs to some inner block
       * "NiSpecularProperty" belongs to the block below
       * "NiTriShapeData"
       */
    }

    for (next = last; next != utilization.end(); next++) {
      /* there may exist holes from unattached sub-nodes */
      if (*next) {
	/* don't overwrite "type" if we don't catch it */
	string stype = (*next)->GetType().GetTypeName();
	if (stype == "NiSkinInstance") {
	  last = next; break; }
      }
    }

    /* there are no shape-data here, continue with properties */
    if (last == utilization.end())
      break;

    /* the rescued node */
    NiGeometryRef ts;

    /* create a TriShape and attach it */
    if (type == "NiTriShapeData")
      ts = new NiTriShape();
    /* create a TriStrips and attach it */
    if (type == "NiTriStripsData")
      ts = new NiTriStrips();

    /* make it identifiable */
    if (ts)
      ts->SetName("Recovered");

    for (walk = first; walk != last + 1; walk++) {
      /* there may exist holes from unattached sub-nodes */
      if (*walk) {
	NiExtraDataRef ex = DynamicCast<NiExtraData>(*walk);
	NiPropertyRef pt = DynamicCast<NiProperty>(*walk);
	NiGeometryDataRef gy = DynamicCast<NiGeometryData>(*walk);
	NiTimeControllerRef tc = DynamicCast<NiTimeController>(*walk);
	NiSkinInstanceRef sk = DynamicCast<NiSkinInstance>(*walk);

	/**/ if (ex) { ts->AddExtraData(ex); *walk = NULL; }
	else if (pt) { ts->AddProperty(pt); *walk = NULL; }
	else if (gy) { ts->SetData(gy); *walk = NULL; }
	else if (tc) { ts->AddController(tc); *walk = NULL; }
	else if (sk) { break; }
	else
	  throw runtime_error("Damaged NIF contains untreatable additional data.");
      }
    }

    /* attach to root */
    NiNodeRef node = DynamicCast<NiNode>(root);
    if (!node)
      throw runtime_error("Root isn't a node, can't re-attach geometry.");
    node->AddChild(DynamicCast<NiAVObject>(ts));

    /* we had to attach before we transfer the skin */
    if (walk != utilization.end()) {
      /* there can be an additional trailing SkinInstance */
      if (*walk) {
	NiSkinInstanceRef skn = DynamicCast<NiSkinInstance>(*walk);
	if (skn) {
//	  ts->SetSkinInstance(skn);

	  /* move over bone weights */
	  unsigned int bc = skn->GetBoneCount();
	  vector< Ref<NiNode> > bn = skn->GetBones();

	  ts->BindSkin(bn);

	  /* transfer everything else */
	  NiSkinInstanceRef nskn = ts->GetSkinInstance();

//	  nskn->SetSkeletonRoot(skn->GetSkeletonRoot());
	  nskn->SetSkinData(skn->GetSkinData());
	  nskn->SetSkinPartition(skn->GetSkinPartition());

	  /* prevent double deallocation */
//	  skn->SetSkeletonRoot(NULL);
	  skn->SetSkinData(NULL);
	  skn->SetSkinPartition(NULL);

	  /* replace references everywhere */
	  SubstituteRefs(DynamicCast<NiObject>(nskn), DynamicCast<NiObject>(skn));

	  *walk = NULL;
	}
      }
    }

    return true;
  } while (0);

  /* let's take a bit care here */
  if ((type == "NiTriStrips") ||
      (type == "NiTriShape") ||
      (type == "NiNode")) {
    /* attach to root */
    NiNodeRef node = DynamicCast<NiNode>(root);
    NiAVObjectRef av = DynamicCast<NiAVObject>(*first);
    if (!node)
      throw runtime_error("Root isn't a node, can't re-attach geometry.");
    NiNodeRef prnt = av->GetParent();
    node->AddChild(av); *first = NULL;

    return true;
  }

  /* let's take a bit care here */
  if (type == "bhkNiTriStripsShape") {
    for (walk = globalnlist.begin(); walk != globalnlist.end(); walk++) {
      bhkRigidBodyRef rig = DynamicCast<bhkRigidBody>(*walk);
      if (rig) {
	/* attach to rigid body */
	rig->AddConstraint(DynamicCast<bhkSerializable>(*first)); *first = NULL;

	return true;
      }
    }

    if (verbose)
      adderrr("No appropriate location found, node has been thrown away.\n");

    *first = NULL;
    return true;
  }

  /* let's take a bit care here */
  if ((type == "NiAlphaProperty") ||
      (type == "NiSpecularProperty") ||
      (type == "NiMaterialProperty") ||
      (type == "NiTexturingProperty") ||
      (type == "NiZBufferProperty")) {
    for (walk = globalnlist.begin(); walk != globalnlist.end(); walk++) {
      NiGeometryRef geo = DynamicCast<NiGeometry>(*walk);
      if (geo) {
	/* this node got this type allready */

	/* BUG: Doesn't work, types are all NiProperty
	NiPropertyRef pt = geo->GetPropertyByType(typc);
	if (pt)
	  continue; */

	vector< Ref<NiProperty> > prps = geo->GetProperties();
	vector< Ref<NiProperty> >::iterator pi;
	for (pi = prps.begin(); pi != prps.end(); pi++)
	  if ((*pi) && ((*pi)->GetType().GetTypeName() == type))
	    break;

	/* we reached the end, and no match found */
	if (pi == prps.end()) {
	  /* attach the property to this node */
	  geo->AddProperty(DynamicCast<NiProperty>(*first)); *first = NULL;
	  return true;
	}
      }
    }

    if (verbose)
      adderrr("No appropriate location found, node has been thrown away.\n");

    *first = NULL;
    return true;
  }

  /* let's take a bit care here */
  if (type == "NiBinaryExtraData") {
    NiBinaryExtraDataRef bx = DynamicCast<NiBinaryExtraData>(*first);
    vector<Niflib::byte> bd = bx->GetData();

    if (bx && !strcmp("Tangent space (binormal & tangent vectors)", bx->GetName().data())) {
      for (walk = globalnlist.begin(); walk != globalnlist.end(); walk++) {
	NiGeometryRef geo = DynamicCast<NiGeometry>(*walk);
	if (geo) {
	  /* find out if we have TB-vectors in the extra data */
	  list< Ref<NiExtraData> > x = geo->GetExtraData();
	  list< Ref<NiExtraData> >::iterator xd;
	  for (xd = x.begin(); xd != x.end(); xd++) {
	    if ((*xd) && !strcmp("Tangent space (binormal & tangent vectors)", (*xd)->GetName().data())) {
	      break;
	    }
	  }

	  /* this node got TNB allready */
	  if (xd != x.end())
	    continue;

	  NiGeometryDataRef raw = geo->GetData();
	  if (raw) {
	    int vc = raw->GetVertexCount();

	    /* found a match */
	    if ((bd.size() / (2 * sizeof(Niflib::Vector3))) == vc) {
	      geo->AddExtraData(DynamicCast<NiExtraData>(*first)); *first = NULL;
	      return true;
	    }
	  }
	}
      }
    }

    if (verbose)
      adderrr("No appropriate location found, node has been thrown away.\n");

    *first = NULL;
    return true;
  }

  /* Apachii_Goddess_Store/meshes/clutter/apachii/anubisguard.nif */
  if ((type == "bhkRigidBodyT") ||
      /* Sheogorad\meshes\sheogorad\velothi\interior\2waypitkillroomgate.nif */
      (type == "bhkBoxShape")) {
    /* TODO: If no shape, throw away */
    if (verbose)
    adderrr("No predictable solution available, node has been thrown away.\n");

    *first = NULL;
    return true;
  }

  /* let's take a bit care here */
  if ((type == "NiPixelData") ||
      (type == "NiMorphData") ||
      (type == "NiFloatInterpolator") ||
      /* Orden des Drachen\meshes\odd\static\furniture\curtainbarakanmini.nif */
      (type == "NiAlphaController") ||
      /* Sheogorad\meshes\sheogorad\dungeoneffects\fxclouddust.nif */
      (type == "NiMultiTargetTransformController") ||
      /* Apachii_Goddess_Store/meshes/clothes/apachii/hgec/boobsvalkyriadiamond.nif */
      (type == "NiSkinPartition")) {
    if (verbose)
      adderrr("No predictable solution available, node has been thrown away.\n");

    *first = NULL;
    return true;
  }

  errprintf(stdout, "Disconnected %s found! Attempting repair.\n", type.data());
  fflush(stdout);
  throw runtime_error("Damaged NIF contains unpredictable structural damage.");
}

/* ---------------------------------------------------------------------------------------------- */

bool ProcessNIF(const char *inname, const char *ouname, const char *rep) {
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
      /* mark the file to have been optimized (change version when adding more features) */
      masterinfo.creator = "NO01";

      barprintf(stdout, '#', "%s", rep);
      if (verbose)
	nfoprintf(stdout, "%s\n", inname);

      /* find the root */
      masterroot = FindRoot(masterlist);

      if (reattachnodes) {
	/* this copy raises all reference counts */
	globalnlist = masterlist;
	utilization = masterlist;

	/* remove root-attached entries from utilization */
	MarkNIF(masterroot, NULL, true);
	/* remove attached to unattached list-entries */
	for (walk = masterlist.begin(); walk != masterlist.end(); walk++)
	  if (*walk)
	    MarkNIF(*walk, NULL, false);

	/* repair untill nothing left */
	int fixednodes = 0;
	while (RepairNIF(masterroot))
	  fixednodes++;

	if (verbose) {
	  if (errrs.size() > 0) {
//	    errprintf(stdout, "errors:\n");
	    for (size_t n = 0; n < errrs.size(); n++)
	      errprintf(stdout, errrs[n].data());

	    errrs.clear();
	  }
	}

	if (fixednodes) {
	  fixedfiles++;
	  brokenfiles++;

	  errprintf(stdout, "Severely damaged NIF, reattached %d nodes (properties or geometry).\n", fixednodes);
	  errprintf(stdout, "You must check the file in NifSkope for correctness!\n");
	}

	/* let's release all reference counts earlier than later */
	globalnlist.clear();
	utilization.clear();
      }

      notes.clear();
      errrs.clear();

      if (masterroot) {
	processedfiles++;

	/* try to load the accompanying TRI/EGM */
	ReadTri(putext(buf, inname, "tri"));
	ReadEgm(putext(buf, inname, "egm"));

	/* check if it's a far NIF */
	bool farnif = issuf(buf, "_far");

	/* take the ouname if we have it */
	if (ouname)
	  strcpy(buf, ouname);
	else
	  strcpy(buf, inname);

	/* we do never overwrite files! append suffix */
	if (!stricmp(inname, buf))
	  putsuf(buf, buf, "_opt");

	/* go and optimize */
	WalkNIF(masterroot, NULL, NULL, farnif);

	/* write the NIF back to disk */
	if (!simulation) {
	  ostream *ost;
	  if ((ost = ioopenostream(buf))) {
	    WriteNifTree(*ost, masterroot, masterinfo);

	    /* uh, MicroSoft, no signed "ssize_t"? */
	    size_t is, os;
	    if ((is = iotellistream(ist)))
	      processedinbytes += is, deltaiobytes += is;
	    if ((os = iotellostream(ost)))
	      processedoubytes += os, deltaiobytes -= os;

	    iocloseostream(ost);
	  }
	  else
	    errprintf(stderr, "can't write the NIF\n");
	}
	else {
	  ostringstream *ost;
	  if ((ost = new ostringstream())) {
	    WriteNifTree(*ost, masterroot, masterinfo);

	    /* uh, MicroSoft, no signed "ssize_t"? */
	    size_t is, os;
	    if ((is = iotellistream(ist)))
	      processedinbytes += is, deltaiobytes += is;
	    if ((os = ost->str().length()))
	      processedoubytes += os, deltaiobytes -= os;

	    delete ost;
	  }
	  else
	    errprintf(stderr, "can't write the NIF\n");
	}

	/* try to save the accompanying TRI/EGM */
	WriteTri(putext(buf, buf, "tri"));
	WriteEgm(putext(buf, buf, "egm"));
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

/* ---------------------------------------------------------------------------------------------- */

bool ProcessKF(const char *inname, const char *ouname, const char *rep) {
  vector<NiObjectRef>::iterator walk;
  char buf[1024];

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
    barprintf(stdout, '#', "%s", rep);
    if (verbose)
      nfoprintf(stdout, "%s\n", inname);

    /* find the root */
    masterroot = FindRoot(masterlist);

    notes.clear();
    errrs.clear();

    if (masterroot) {
      processedfiles++;

      /* check if it's a far KF */
      bool farnif = false;

      /* take the ouname if we have it */
      if (ouname)
	strcpy(buf, ouname);
      else
	strcpy(buf, inname);

      /* we do never overwrite files! append suffix */
      if (!stricmp(inname, buf))
	putsuf(buf, buf, "_opt");

      /* go and optimize */
      WalkNIF(masterroot, NULL, NULL, farnif);

      /* write the NIF back to disk */
      if (!simulation) {
	ostream *ost;
	if ((ost = ioopenostream(buf))) {
	  WriteNifTree(*ost, masterroot, masterinfo);

	  /* uh, MicroSoft, no signed "ssize_t"? */
	  size_t is, os;
	  if ((is = iotellistream(ist)))
	    processedinbytes += is, deltaiobytes += is;
	  if ((os = iotellostream(ost)))
	    processedoubytes += os, deltaiobytes -= os;

	  iocloseostream(ost);
	}
	else
	  errprintf(stderr, "can't write the KF\n");
      }
      else {
	ostringstream *ost;
	if ((ost = new ostringstream())) {
	  WriteNifTree(*ost, masterroot, masterinfo);

	  /* uh, MicroSoft, no signed "ssize_t"? */
	  size_t is, os;
	  if ((is = iotellistream(ist)))
	    processedinbytes += is, deltaiobytes += is;
	  if ((os = ost->str().length()))
	    processedoubytes += os, deltaiobytes -= os;

	  delete ost;
	}
	else
	  errprintf(stderr, "can't write the KF\n");
      }
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

void process(const char *inname, const char *ouname) {
  size_t ilen = (inname ? strlen(inname) : 0);
  size_t olen = (ouname ? strlen(ouname) : ilen);

  bool rm = false;
  struct ioinfo iinfo, oinfo;
  if (!iostat(inname, &iinfo)) {
    /* input: directory */
    if (iinfo.io_type & IO_DIRECTORY) {
      /* don't create output in simulation-mode */
      if (ouname && !simulation && iostat(ouname, &oinfo)) {
	rm = true;

	if (iomkdir(ouname)) {
	  fprintf(stderr, "can't create the directory \"%s\"\n", ouname);
	  return;
	}

	if (iostat(ouname, &oinfo)) {
	  fprintf(stderr, "can't find the directory \"%s\"\n", ouname);
	  return;
	}
      }

      if ((oinfo.io_type & IO_DIRECTORY) || simulation) {
	struct iodir *dir;

	if ((dir = ioopendir(inname))) {
	  struct iodirent *et;

	  while ((et = ioreaddir(dir))) {
	    if (!strcmp(et->name, ".") ||
		!strcmp(et->name, ".."))
	      continue;

	    char *ninname = (char *)malloc(ilen + 1 + et->namelength + 1);
	    char *nouname = (char *)malloc(olen + 1 + et->namelength + 1);

	    strcpy(ninname, inname);
	    strcpy(nouname, ouname ? ouname : inname);

	    strcat(ninname, "\\");
	    strcat(nouname, "\\");

	    strcat(ninname, et->name);
	    strcat(nouname, et->name);

	    process(ninname, nouname);

	    free(ninname);
	    free(nouname);
	  }

	  ioclosedir(dir);

	  /* don't create output in simulation-mode */
	  if (ouname && !simulation && (dir = ioopendir(ouname))) {
	    struct iodirent *et;
	    int num = 0;

	    while ((et = ioreaddir(dir))) {
	      if (!strcmp(et->name, ".") ||
		  !strcmp(et->name, ".."))
		continue;

	      /* ioopen/fopen is synchronous, only close is
	       * asynchronous, which means even non-yet-written
	       * files exist in the folder and we don't nee to wait for
	       * completition to check if a folder is empty/non-empty
	       *
	       * no lock/synchronization necessary
	       */
	      num++;
	    }

	    ioclosedir(dir);

	    if (!num && rm)
	      iormdir(ouname);
	  }
	}
	else
	  fprintf(stderr, "can't open the directory \"%s\"\n", inname);
      }
      else
	fprintf(stderr, "destination isn't a directory \"%s\"\n", ouname);
    }
    /* input: file */
    else {
      const char *fle;
      struct iodir *dir;
      bool dealloc = false;
      bool docopy = true;
      bool iszero = !iinfo.io_size;

      /* strip trailing slash */
      if (!(fle = strrchr(inname, '/')))
	if (!(fle = strrchr(inname, '\\')))
	  fle = inname - 1;

      fle += 1;
      if (stristr(fle, "thumbs.db"))
	return;
      if (dropextras) {
	if (isext(fle, "psd"))
	  docopy = false;

	if (!docopy)
	  fprintf(stderr, "filtered \"%s\"\n", fle);
      }

      /* if there is no destination, clone the source */
      if (ouname && !iostat(ouname, &oinfo)) {
	if (oinfo.io_type & IO_DIRECTORY) {
	  char *nouname = (char *)malloc(olen + 1 + strlen(fle) + 1);

	  /* if we push a single specific file into a BSA
	   * we have to open/close it to flush the BSA to disk
	   */
	  dir = ioopendir(ouname);

	  strcpy(nouname, ouname);
	  strcat(nouname, "\\");
	  strcat(nouname, fle);

	  ouname = nouname;
	  dealloc = true;
	}
	else if (skipexisting)
	  return;
	else if (skipnewer)
	  if (iinfo.io_time <= oinfo.io_time)
	    return;
      }

      /* global statistics (even for multiple files in one pass) */
      deltaiobytes = 0;

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
	  /* done via conversion */
	  else if (!skipprocessing)
	    docopy = false;
	}

	/* try to save the accompanying TRI */
	if (isext(inname, "tri")) {
	  char companynif[1024];
	  putext(companynif, inname, "nif");

	  if (iostat(companynif, &iinfo)) {
	    orphtris.push_back(inname);
	    orphantrifiles++;
	  }
	  /* done via conversion */
	  else if (!skipprocessing)
	    docopy = false;
	}

	if (isext(inname, "nif")) {
	  if (skipmodels)
	    docopy = false;
	  else if (!skipprocessing) {
	    fprintf(stderr, "processing \"%s\"\n", fle);
	    docopy = !ProcessNIF(inname, ouname, fle);
	    /* okay, done with */
	  }
	}

	if (isext(inname, "kf")) {
	  if (skipmodels)
	    docopy = false;
	  else if (!skipprocessing) {
	    fprintf(stderr, "processing \"%s\"\n", fle);
	    docopy = !ProcessKF(inname, ouname, fle);
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
	    docopy = false;
	  else if (!skipprocessing && stristr(inname, "textures\\")) {
	    fprintf(stderr, "processing \"%s\"\n", fle);
	    docopy = !ProcessDDS(inname, ouname, fle);
	    /* okay, done with */
	  }
	}
#endif

#ifdef	SOUNDOPT
	if (isext(inname, "wav")) {
	  if (skipsounds)
	    docopy = false;
	  else if (!skipprocessing) {
	    fprintf(stderr, "processing \"%s\"\n", fle);
	    docopy = !ProcessWave(inname, ouname, fle);
	    /* okay, done with */
	  }
	}

	if (isext(inname, "wav") ||
	  ((isext(inname, "mp3") ||
	    isext(inname, "lip")) && (compresssounds || skipsounds))) {
	  if (skipsounds)
	    docopy = false;
	}
#endif
#ifdef	NDEBUG
      }
      catch(exception &e) {
	errprintf(stderr, "error processing \"%s\": %s\n", fle, e.what());
	errprintf(stdout, "error processing \"%s\": %s\n", fle, e.what());
	brokenfiles++;
      }
//      }
//      __except (EXCEPTION_EXECUTE_HANDLER) {
//	errprintf(stderr, "Fatal abortion while reading the NIF\n");
//	errprintf(stdout, "Fatal abortion while reading the NIF\n");
//      }
#endif

      /* passthrough-mode, don't create output in simulation-mode */
      if (docopy && passthrough && ouname && !simulation) {
	/* action required */
	if (stricmp(inname, ouname)) {
	  fprintf(stderr, "copying \"%s\"\n", fle);

	  /* TODO: nowait asynchronous */
	  iocp(inname, ouname);
	}

	if (iszero)
	  zerobytes.push_back(inname);
      }
      else {
	if (deltaiobytes)
	  nfoprintf(stdout, "delta:            %d bytes %s\n", (deltaiobytes > 0) ? deltaiobytes : -deltaiobytes, (deltaiobytes > 0) ? "less" : "more");
/*	else
	  nfoprintf(stdout, "delta:            unchanged\n");	*/
      }

      if (dealloc) {
	ioclosedir(dir);
	free((void *)ouname);
      }
    }
  }
}

/* ---------------------------------------------------------------------------------------------- */

#define	DATAMINING
#ifdef	DATAMINING
#include "nifopt-db.C"
#else
void analyze(const char *inname) {}
#endif

#define TASKGUI
#ifdef	TASKGUI
//nclude "nifopt-gui.cpp"
#endif

extern bool parse_gui();

void prolog() {
  ioinit();

#ifdef	DATAMINING
  if (datamining)
    dbinit("nifopt.sqlite");
#endif

  // initialize Tootle
  if (!skipprocessing || havokserver) {
    if ((geotoprimitive || optimizehavok || havokserver) && (HavokBegin(havokserver) != true))
      exit(0);
    if (optimizelists && (TootleInit() != TOOTLE_OK))
      exit(0);
    if ((TextureInit() != true))
      exit(0);
  }
}

void epilog() {
  // clean up tootle
  if (!skipprocessing) {
    if ((geotoprimitive || optimizehavok || havokserver))
      HavokEnd(havokserver);
    if (optimizelists)
      TootleCleanup();
    if (!skipprocessing)
      TextureCleanup();
  }

#ifdef	DATAMINING
  if (datamining)
    dbexit();
#endif

  ioexit();
}

/* ---------------------------------------------------------------------------------------------- */

char *infile = NULL;
char *outfile = NULL;

static char option_usage[] = "Options: \n\
 -havokgeotoprim   convert Havok geometry to primitives (everywhere)\n\
 -stripstolists    convert NiTriStrips to NiTriShapes (everywhere)\n\
 -optimizefar      optimize \"_far\" NIFs\n\
 -optimizelists    optimize NiTriShapes for vertex-cache and overdraw\n\
 -optimizeparts    optimize NiSkinPartitions for vertex-cache and overdraw\n\
 -optimizekeys     optimize keyframes in timelines\n\
 -optimizehavok    optimize Havok NiTriShapes\n\
 -optimizesmmopp   regenerate single-material MOPP-codes (stable)\n\
 -optimizemmmopp   regenerate multi-material MOPP-codes (experimental)\n\
 -optimizetexts    optimize DDS quality, re-mip as well\n\
 -optimizequick    optimize faster but not so well, applies to NIFs and DDSs\n\
 -optimizenothing  don't optimize, just do conversions\n\
 -barestripfar     \"_far\" NIFs will be stripped bare to the bone\n\
 -mipgamma         treat color-maps as in gamma-space and filter them special\n\
 -tsnormals        treat normal-maps as in tangent-space and code them special\n\
 -steepnormals     steepen the normal-map every mip-level, reduces smoothing\n\
\n\
 -parallaxmapping  turn on parallax mapping\n\
 -leavetexpaths    leave the texture-paths as-is, otherwise make meshes BSAable\n\
 -leavehdrtexts    leave the hdr-textures as-is, otherwise compress them\n\
 -dropextras       drop extra files from the hierarchy (thumbs.db, .psd, etc.)\n\
 -dropunattached   don't repair unattached nodes, throw them away (dangerous)\n\
 -droptrimeshes    don't reuse non-matching TRI-data, replace by NIF-data\n\
 -skiphashcheck    don't verify BSA hashes, recover contents\n\
\n\
 -compress <level> when wring BSA, compression-strength +- <0-9,10>\n\
 -compressimages   any image will be converted to DXT-DDSs, tex-paths adjusted\n\
 -compresssounds   compress sounds to MS ADPCM (unsupported by Oblivion)\n\
 -skipexisting     skip if the destination file exist\n\
 -skipnewer        skip if the destination file (or BSA) is newer\n\
 -skipmodels       skip processing of model-files\n\
 -skipimages       skip processing of image-files\n\
 -skipsounds       skip processing of sound-files\n\
 -skipprocessing   skip any of the NIF i/o, just copy and report\n\
 -processhidden    process hidden files and directories instead of skipping\n\
 -passthrough      copy broken and every file which is not a NIF as well\n\
 -game <ob|sk>     indicate the gameversion being Oblivion (ob) or Skyrim (sk)\n\
 -simulate         operate in read-only mode, nothing will be written\n\
 -verbose          output more information\n\
 -criticals	   output only critical error related information\n\
\n\
 -copy             copy only preset, never change or compress anything\n\
 -deployment       deployment preset, everything maxed out\n\
\n\
Some explanations:\n\
 \n\
 When run without parameters it doesn't do much. Removing void entries\n\
 and fixing texture-paths basically.\n\
 \n\
 If the support-library NIFlib can not load a NIF it'll bail out with an error\n\
 message and the destination is created without changes.\n\
 \n\
 The tool runs on files, archives and directories, with and without outputs. It\n\
 will not overwrite it's own input, so if there is no distinct destination\n\
 given it'll append \"_opt\" to the NIF/DDS.\n\
 \n\
 It can distinguish the purposes of the texture-files by their suffix, \"_n\"\n\
 is a normal-map f.e. and it uses appropriate routines for each different\n\
 type. When obvious it can identify the alpha-channel as transparency-carrier.\n\
 If you want to mark specific files to have optimal specific treatment you can\n\
 further add suffixes, like \"_xyz\"/\"_rgb\" which will drop alpha-channels,\n\
 or \"_rgba\" which will specifically utilize the transparency-optimizations.\n\
 Informational suffices will be dropped, or converted into \"_n\". Converted\n\
 image-formats other than DDS will receive the DDS-extension and texture-paths\n\
 will be changed to it as well.\n\
 \n\
 When creating or modifying archives it will utilize the given strength of\n\
 compression ranking from 0-9 as for the equivalent option in the zlib. You can\n\
 also utilize 10 in which case a 7z-based deflater brute-force searches the\n\
 parameters for best compression. The tool normally re-considers compressing\n\
 files like mp3s and videos if they are not really well compressed. If you\n\
 supply negative levels (-1 to -10) you can force it to compress always.\n\
";

static void usage(char *progname) {
  cerr << "Usage:" << endl;
  cerr << progname << " [options] in [out]" << endl;
  cerr << option_usage << endl;
  exit(1);
}

void parseCommandline(int argc, char *argv[]) {
  if (argc < 2)
    usage(argv[0]);

  int i;

  infile  = NULL;
  outfile = NULL;

  for(i = 1; i < argc; i++) {
    /**/ if (!strcmp(argv[i], "-gui"))
      parse_gui();
    else if (!strcmp(argv[i], "-havokgeotoprim"))
      geotoprimitive = true;
    else if (!strcmp(argv[i], "-stripstolists"))
      stripstolists = true;
    else if (!strcmp(argv[i], "-optimizefar"))
      optimizefar = true;
    else if (!strcmp(argv[i], "-optimizelists"))
      optimizelists = optimizeparts = true;
    else if (!strcmp(argv[i], "-optimizeparts"))
      optimizeparts = true;
    else if (!strcmp(argv[i], "-optimizekeys"))
      optimizekeys = true;
    else if (!strcmp(argv[i], "-optimizehavok"))
      optimizehavok = true;
    else if (!strcmp(argv[i], "-optimizesmmopp"))
      optimizesmmopp = true;
    else if (!strcmp(argv[i], "-optimizemmmopp"))
      optimizemmmopp = true;
    else if (!strcmp(argv[i], "-optimizequick"))
      optimizequick = true;
    else if (!strcmp(argv[i], "-optimizetexts"))
      optimizetexts = true;
    else if (!strcmp(argv[i], "-optimizenothing"))
      optimizelists = optimizeparts = optimizetexts = false;
    else if (!strcmp(argv[i], "-parallaxmapping"))
      parallaxmapping = true;
    else if (!strcmp(argv[i], "-leavetexpaths"))
      texturepaths = false;
    else if (!strcmp(argv[i], "-leavehdrtexts"))
      leavehdrtexts = true;
    else if (!strcmp(argv[i], "-dropunattached"))
      reattachnodes = false;
    else if (!strcmp(argv[i], "-droptrimeshes"))
      droptrimeshes = false;
    else if (!strcmp(argv[i], "-dropextras"))
      dropextras = true;
    else if (!strcmp(argv[i], "-skiphashcheck"))
      skiphashcheck = true;
    else if (!strcmp(argv[i], "-barestripfar"))
      barestripfar = true;
    else if (!strcmp(argv[i], "-tsnormals"))
      normalmapts = true;
    else if (!strcmp(argv[i], "-steepnormals"))
      normalsteepness = 2;
    else if (!strcmp(argv[i], "-mipgamma"))
      colormapgamma = true;
    else if (!strcmp(argv[i], "-skipmodels"))
      skipmodels = true;
    else if (!strcmp(argv[i], "-skipimages"))
      skipimages = true;
    else if (!strcmp(argv[i], "-skipsounds"))
      skipsounds = true;
    else if (!strcmp(argv[i], "-skipexisting"))
      skipexisting = true;
    else if (!strcmp(argv[i], "-skipnewer"))
      skipnewer = true;
    else if (!strcmp(argv[i], "-skipprocessing"))
      skipprocessing = true;
    else if (!strcmp(argv[i], "-processhidden"))
      processhidden = true;
    else if (!strcmp(argv[i], "-passthrough"))
      passthrough = true;
    else if (!strcmp(argv[i], "-verbose"))
      verbose = true;
    else if (!strcmp(argv[i], "-criticals"))
      critical = true;
    else if (!strcmp(argv[i], "-compressimages"))
      compressimages = true;
    else if (!strcmp(argv[i], "-compresssounds"))
      compresssounds = true;
    else if (!strcmp(argv[i], "-compress"))
      compresslevel = atoi(argv[++i]);
    else if (!strcmp(argv[i], "-simulate"))
      simulation = true;
    else if (!strcmp(argv[i], "-analyze"))
      datamining = true;
    else if (!strcmp(argv[i], "-game")) {
      ++i;
      if (!strcmp(argv[i], "ob"))
	gameversion = OB_BSAHEADER_VERSION;
      else if (!strcmp(argv[i], "sk"))
	gameversion = SK_BSAHEADER_VERSION, skipmodels = true;
    }
    else if (!strcmp(argv[i], "-deployment"))
      geotoprimitive = true,
      stripstolists = true,
      optimizehavok = true,
      optimizesmmopp = true,
      optimizemmmopp = false,
      optimizefar = true,
      optimizelists = true,
      optimizeparts = true,
      optimizekeys = true,
      optimizequick = false,
      optimizetexts = true,
      reattachnodes = false,
      skiphashcheck = true,
      barestripfar = false,
      normalmapts = false,
      normalsteepness = 2,
      colormapgamma = true,
      passthrough = true,
      verbose = true,
      skipprocessing = false,
      compressimages = true,
      compresslevel = 10;
    else if (!strcmp(argv[i], "-copy"))
      geotoprimitive = false,
      stripstolists = false,
      optimizehavok = false,
      optimizesmmopp = false,
      optimizemmmopp = false,
      optimizefar = false,
      optimizelists = false,
      optimizeparts = false,
      optimizekeys = false,
      optimizequick = false,
      optimizetexts = false,
      reattachnodes = false,
      skiphashcheck = false,
      barestripfar = false,
      normalmapts = false,
      normalsteepness = 1,
      colormapgamma = false,
      passthrough = true,
      verbose = true,
      skipprocessing = true,
      compressimages = false,
      compresslevel = 0;
    else if (!strcmp(argv[i], "-havokserver"))
      havokserver = true;
    else if (!infile)
      infile = argv[i];
    else if (!outfile)
      outfile = argv[i];
    else if (!strcmp(argv[i], "-h") || !strcmp(argv[i], "-help"))
      usage(argv[0]);
    else
      usage(argv[0]);
  }

  if (compresslevel < 0)
    compresslevel = -compresslevel, thresholdbsa = false;
  if (compresslevel == 0)
    compressbsa = false;
  if (compresslevel == (Z_BEST_COMPRESSION + 1))
    compresslevel = 9, srchbestbsa = true;

  if (!infile && !havokserver)
    usage(argv[0]);

  /* normalize */
  char *rpl;

  if (infile) {
    while ((rpl = strchr(infile, '/')))
      *rpl = '\\';
    rpl = infile + strlen(infile);
    while (*--rpl == '\\')
      *rpl = '\0';
  }

  if (outfile) {
    while ((rpl = strchr(outfile, '/')))
      *rpl = '\\';
    rpl = outfile + strlen(outfile);
    while (*--rpl == '\\')
      *rpl = '\0';
  }
}

void summary(FILE *out) {
  if (1) {
    fprintf(out, "\n\nFinal report \"%s\":\n", infile);
    fprintf(out, " processed files: %d\n", processedfiles);
    fprintf(out, " processed faces: %d\n", tri_procd);
    fprintf(out, " modified textures: %d\n", modifiedtexts);
    fprintf(out, " modified meshes: %d\n", modifiedmeshes);
    fprintf(out, " modified timelines: %d\n", modifiedtlines);
    fprintf(out, " damaged meshes: %d\n", meshdamages);
    fprintf(out, " fixed textures: %d\n", fixedtexts);
    fprintf(out, " fixed meshes: %d\n", fixedmeshes);
    fprintf(out, " fixed files: %d\n", fixedfiles);
    fprintf(out, " fixed paths: %d\n", fixedpaths);
    fprintf(out, " broken hierarchy: %d\n", brokennodes);
    fprintf(out, " broken files: %d (%d without fixed ones)\n", brokenfiles, brokenfiles - fixedfiles);
    fprintf(out, " planar (1x1) textures: %d\n", planartexts);
    fprintf(out, " changed texture formats: %d\n", changedformats);

    if (processedoubytes || processedinbytes)
      fprintf(out, " i/o delta: %d bytes\n", processedoubytes - processedinbytes);
    if (convertedinbytes || convertedoubytes)
      fprintf(out, " tex delta: %d bytes\n", convertedoubytes - convertedinbytes);
    if (compressedinbytes || compressedoubytes)
      fprintf(out, " z delta: %d bytes\n", compressedoubytes - compressedinbytes);
    if (virtualbsabytes || virtualbsafiles)
      fprintf(out, " bsa virtuals: -%d bytes (%d files)\n", virtualbsabytes, virtualbsafiles);
  }

  if (orphegms.size() > 0) {
    fprintf(out, "\norphaned EGMs:\n");
    for (size_t n = 0; n < orphegms.size(); n++)
      fprintf(out, "%s\n", orphegms[n].data());
  }

  if (orphtris.size() > 0) {
    fprintf(out, "\norphaned TRIs:\n");
    for (size_t n = 0; n < orphtris.size(); n++)
      fprintf(out, "%s\n", orphtris[n].data());
  }

  if (verbose) {
    if (texpths.size() > 0) {
      fprintf(out, "\nfixed texture-paths:\n");
      for (size_t n = 0; n < texpths.size(); n++)
	fprintf(out, "%s\n", texpths[n].data());
    }

    if (zerobytes.size() > 0) {
      fprintf(out, "\nzero byte files:\n");
      for (size_t n = 0; n < zerobytes.size(); n++)
	fprintf(out, "%s\n", zerobytes[n].data());
    }

    if (duplicates.size() > 0) {
      fprintf(out, "\nduplicates files:\n");
      map<string, string>::iterator n;
      for (n = duplicates.begin(); n != duplicates.end(); n++)
	fprintf(out, "%s == %s\n", n->first.data(), n->second.data());
    }
  }

  fflush(out);
}

int main(int argc,char **argv) {
    parseCommandline(argc, argv);

    prolog();

    if (infile) {
#ifdef	NDEBUG
      try {
#endif
	if (datamining)
	  analyze(infile);
	else
	  process(infile, outfile);

	fflush(stderr);
	fflush(stdout);

	/* summary */
	summary(stderr);
	summary(stdout);
#ifdef	NDEBUG
      }
      catch(exception &e) {
	fprintf(stderr, "fatal error: %s\n", e.what());
      }
#endif
    }

    epilog();

    return 0;
}
