#include <d3d9.h>
#include <d3dx9.h>

extern IDirect3D9 *pD3D;
extern IDirect3DDevice9 *pD3DDevice;

#define D3DFMT_ATI1 (D3DFORMAT)MAKEFOURCC('A','T','I','1')
#define D3DFMT_ATI2 (D3DFORMAT)MAKEFOURCC('A','T','I','2')

bool TextureInit();
void TextureCleanup();

bool TextureConvertRGBH(LPDIRECT3DTEXTURE9 *base, int minlevel, bool gama);
bool TextureConvertRGB(LPDIRECT3DTEXTURE9 *base, int minlevel, bool gama);
bool TextureConvertA(LPDIRECT3DTEXTURE9 *alpha, int minlevel);
bool TextureConvertXYZD(LPDIRECT3DTEXTURE9 *norm, int minlevel);
bool TextureConvertXY_Z(LPDIRECT3DTEXTURE9 *norm, LPDIRECT3DTEXTURE9 *z, int minlevel);
bool TextureConvertXYZ(LPDIRECT3DTEXTURE9 *norm, int minlevel);
bool TextureConvertXY(LPDIRECT3DTEXTURE9 *norm, int minlevel);
bool TextureConvertLA(LPDIRECT3DTEXTURE9 *base, int minlevel);
bool TextureConvertA(LPDIRECT3DTEXTURE9 *alpha, int minlevel);
bool TextureConvertL(LPDIRECT3DTEXTURE9 *lumi, int minlevel);

bool TextureCompressRGBH(LPDIRECT3DTEXTURE9 *base, int minlevel, bool gama);
bool TextureCompressRGB(LPDIRECT3DTEXTURE9 *base, int minlevel, bool gama);
bool TextureCompressA(LPDIRECT3DTEXTURE9 *alpha, int minlevel);
bool TextureCompressXYZD(LPDIRECT3DTEXTURE9 *norm, int minlevel);
bool TextureCompressXY_Z(LPDIRECT3DTEXTURE9 *norm, LPDIRECT3DTEXTURE9 *z, int minlevel);
bool TextureCompressXYZ(LPDIRECT3DTEXTURE9 *norm, int minlevel);
bool TextureCompressXY(LPDIRECT3DTEXTURE9 *norm, int minlevel);
bool TextureCompressPM(LPDIRECT3DTEXTURE9 *base, LPDIRECT3DTEXTURE9 *norm, int minlevel, bool gama);

bool TextureCompressQDM(LPDIRECT3DTEXTURE9 *base, LPDIRECT3DTEXTURE9 *norm, int minlevel, bool gama, bool LODed);

const char *findFormat(D3DFORMAT fmt);
