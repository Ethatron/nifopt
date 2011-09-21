

/* this ALWAYS GENERATED file contains the RPC server stubs */


 /* File created by MIDL compiler version 7.00.0555 */
/* at Mon Sep 19 16:48:03 2011
 */
/* Compiler settings for nifopt-hvk.idl:
    Oicf, W1, Zp8, env=Win32 (32b run), target_arch=X86 7.00.0555 
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */

#if !defined(_M_IA64) && !defined(_M_AMD64)


#pragma warning( disable: 4049 )  /* more than 64k source lines */
#if _MSC_VER >= 1200
#pragma warning(push)
#endif

#pragma warning( disable: 4211 )  /* redefine extern to static */
#pragma warning( disable: 4232 )  /* dllimport identity*/
#pragma warning( disable: 4024 )  /* array to pointer mapping*/
#pragma warning( disable: 4100 ) /* unreferenced arguments in x86 call */

#pragma optimize("", off ) 

#include <string.h>
#include "nifopt-hvk.h"

#define TYPE_FORMAT_STRING_SIZE   315                               
#define PROC_FORMAT_STRING_SIZE   991                               
#define EXPR_FORMAT_STRING_SIZE   1                                 
#define TRANSMIT_AS_TABLE_SIZE    0            
#define WIRE_MARSHAL_TABLE_SIZE   0            

typedef struct _nifopt2Dhvk_MIDL_TYPE_FORMAT_STRING
    {
    short          Pad;
    unsigned char  Format[ TYPE_FORMAT_STRING_SIZE ];
    } nifopt2Dhvk_MIDL_TYPE_FORMAT_STRING;

typedef struct _nifopt2Dhvk_MIDL_PROC_FORMAT_STRING
    {
    short          Pad;
    unsigned char  Format[ PROC_FORMAT_STRING_SIZE ];
    } nifopt2Dhvk_MIDL_PROC_FORMAT_STRING;

typedef struct _nifopt2Dhvk_MIDL_EXPR_FORMAT_STRING
    {
    long          Pad;
    unsigned char  Format[ EXPR_FORMAT_STRING_SIZE ];
    } nifopt2Dhvk_MIDL_EXPR_FORMAT_STRING;


static const RPC_SYNTAX_IDENTIFIER  _RpcTransferSyntax = 
{{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}};

extern const nifopt2Dhvk_MIDL_TYPE_FORMAT_STRING nifopt2Dhvk__MIDL_TypeFormatString;
extern const nifopt2Dhvk_MIDL_PROC_FORMAT_STRING nifopt2Dhvk__MIDL_ProcFormatString;
extern const nifopt2Dhvk_MIDL_EXPR_FORMAT_STRING nifopt2Dhvk__MIDL_ExprFormatString;

/* Standard interface: havok, ver. 1.0,
   GUID={0x79eb9777,0x29c9,0x427f,{0x8f,0x2a,0xac,0x64,0xf7,0xa6,0xbf,0x5a}} */


extern const MIDL_SERVER_INFO havok_ServerInfo;

extern const RPC_DISPATCH_TABLE havok_v1_0_DispatchTable;

static const RPC_SERVER_INTERFACE havok___RpcServerInterface =
    {
    sizeof(RPC_SERVER_INTERFACE),
    {{0x79eb9777,0x29c9,0x427f,{0x8f,0x2a,0xac,0x64,0xf7,0xa6,0xbf,0x5a}},{1,0}},
    {{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}},
    (RPC_DISPATCH_TABLE*)&havok_v1_0_DispatchTable,
    0,
    0,
    0,
    &havok_ServerInfo,
    0x04000000
    };
RPC_IF_HANDLE havok_v1_0_s_ifspec = (RPC_IF_HANDLE)& havok___RpcServerInterface;

extern const MIDL_STUB_DESC havok_StubDesc;


#if !defined(__RPC_WIN32__)
#error  Invalid build platform for this stub.
#endif

#if !(TARGET_IS_NT50_OR_LATER)
#error You need Windows 2000 or later to run this stub because it uses these features:
#error   /robust command line switch.
#error However, your C/C++ compilation flags indicate you intend to run this app on earlier systems.
#error This app will fail with the RPC_X_WRONG_STUB_VERSION error.
#endif


static const nifopt2Dhvk_MIDL_PROC_FORMAT_STRING nifopt2Dhvk__MIDL_ProcFormatString =
    {
        0,
        {

	/* Procedure GenerateMoppCode */

			0x32,		/* FC_BIND_PRIMITIVE */
			0x48,		/* Old Flags:  */
/*  2 */	NdrFcLong( 0x0 ),	/* 0 */
/*  6 */	NdrFcShort( 0x0 ),	/* 0 */
/*  8 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 10 */	NdrFcShort( 0x15 ),	/* 21 */
/* 12 */	NdrFcShort( 0x8 ),	/* 8 */
/* 14 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x6,		/* 6 */
/* 16 */	0x8,		/* 8 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 18 */	NdrFcShort( 0x0 ),	/* 0 */
/* 20 */	NdrFcShort( 0x1 ),	/* 1 */
/* 22 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter material */

/* 24 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 26 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 28 */	0x2,		/* FC_CHAR */
			0x0,		/* 0 */

	/* Parameter nVerts */

/* 30 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 32 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 34 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter verts */

/* 36 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 38 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 40 */	NdrFcShort( 0xc ),	/* Type Offset=12 */

	/* Parameter nTris */

/* 42 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 44 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 46 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter tris */

/* 48 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 50 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 52 */	NdrFcShort( 0x2c ),	/* Type Offset=44 */

	/* Return value */

/* 54 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 56 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 58 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GenerateMoppCodeWithSubshapes */

/* 60 */	0x32,		/* FC_BIND_PRIMITIVE */
			0x48,		/* Old Flags:  */
/* 62 */	NdrFcLong( 0x0 ),	/* 0 */
/* 66 */	NdrFcShort( 0x1 ),	/* 1 */
/* 68 */	NdrFcShort( 0x24 ),	/* x86 Stack size/offset = 36 */
/* 70 */	NdrFcShort( 0x20 ),	/* 32 */
/* 72 */	NdrFcShort( 0x8 ),	/* 8 */
/* 74 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x9,		/* 9 */
/* 76 */	0x8,		/* 8 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 78 */	NdrFcShort( 0x0 ),	/* 0 */
/* 80 */	NdrFcShort( 0x1 ),	/* 1 */
/* 82 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter nShapes */

/* 84 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 86 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 88 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter shapeVerts */

/* 90 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 92 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 94 */	NdrFcShort( 0x46 ),	/* Type Offset=70 */

	/* Parameter nMaterials */

/* 96 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 98 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 100 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter materialIDs */

/* 102 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 104 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 106 */	NdrFcShort( 0x56 ),	/* Type Offset=86 */

	/* Parameter nVerts */

/* 108 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 110 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 112 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter verts */

/* 114 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 116 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 118 */	NdrFcShort( 0x66 ),	/* Type Offset=102 */

	/* Parameter nTris */

/* 120 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 122 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 124 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter tris */

/* 126 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 128 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 130 */	NdrFcShort( 0x80 ),	/* Type Offset=128 */

	/* Return value */

/* 132 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 134 */	NdrFcShort( 0x20 ),	/* x86 Stack size/offset = 32 */
/* 136 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure RetrieveMoppCode */

/* 138 */	0x32,		/* FC_BIND_PRIMITIVE */
			0x48,		/* Old Flags:  */
/* 140 */	NdrFcLong( 0x0 ),	/* 0 */
/* 144 */	NdrFcShort( 0x2 ),	/* 2 */
/* 146 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 148 */	NdrFcShort( 0x8 ),	/* 8 */
/* 150 */	NdrFcShort( 0x8 ),	/* 8 */
/* 152 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x3,		/* 3 */
/* 154 */	0x8,		/* 8 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 156 */	NdrFcShort( 0x1 ),	/* 1 */
/* 158 */	NdrFcShort( 0x0 ),	/* 0 */
/* 160 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter nBuffer */

/* 162 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 164 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 166 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter buffer */

/* 168 */	NdrFcShort( 0x113 ),	/* Flags:  must size, must free, out, simple ref, */
/* 170 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 172 */	NdrFcShort( 0x9a ),	/* Type Offset=154 */

	/* Return value */

/* 174 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 176 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 178 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure RetrieveMoppWelding */

/* 180 */	0x32,		/* FC_BIND_PRIMITIVE */
			0x48,		/* Old Flags:  */
/* 182 */	NdrFcLong( 0x0 ),	/* 0 */
/* 186 */	NdrFcShort( 0x3 ),	/* 3 */
/* 188 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 190 */	NdrFcShort( 0x8 ),	/* 8 */
/* 192 */	NdrFcShort( 0x8 ),	/* 8 */
/* 194 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x3,		/* 3 */
/* 196 */	0x8,		/* 8 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 198 */	NdrFcShort( 0x1 ),	/* 1 */
/* 200 */	NdrFcShort( 0x0 ),	/* 0 */
/* 202 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter nBuffer */

/* 204 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 206 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 208 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter buffer */

/* 210 */	NdrFcShort( 0x113 ),	/* Flags:  must size, must free, out, simple ref, */
/* 212 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 214 */	NdrFcShort( 0xaa ),	/* Type Offset=170 */

	/* Return value */

/* 216 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 218 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 220 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure RetrieveMoppScale */

/* 222 */	0x32,		/* FC_BIND_PRIMITIVE */
			0x48,		/* Old Flags:  */
/* 224 */	NdrFcLong( 0x0 ),	/* 0 */
/* 228 */	NdrFcShort( 0x4 ),	/* 4 */
/* 230 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 232 */	NdrFcShort( 0x0 ),	/* 0 */
/* 234 */	NdrFcShort( 0x24 ),	/* 36 */
/* 236 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 238 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 240 */	NdrFcShort( 0x0 ),	/* 0 */
/* 242 */	NdrFcShort( 0x0 ),	/* 0 */
/* 244 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter value */

/* 246 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 248 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 250 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Return value */

/* 252 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 254 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 256 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure RetrieveMoppOrigin */

/* 258 */	0x32,		/* FC_BIND_PRIMITIVE */
			0x48,		/* Old Flags:  */
/* 260 */	NdrFcLong( 0x0 ),	/* 0 */
/* 264 */	NdrFcShort( 0x5 ),	/* 5 */
/* 266 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 268 */	NdrFcShort( 0x0 ),	/* 0 */
/* 270 */	NdrFcShort( 0x24 ),	/* 36 */
/* 272 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 274 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 276 */	NdrFcShort( 0x0 ),	/* 0 */
/* 278 */	NdrFcShort( 0x0 ),	/* 0 */
/* 280 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter value */

/* 282 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 284 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 286 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Return value */

/* 288 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 290 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 292 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure CalcOBBPolyhedron */

/* 294 */	0x32,		/* FC_BIND_PRIMITIVE */
			0x48,		/* Old Flags:  */
/* 296 */	NdrFcLong( 0x0 ),	/* 0 */
/* 300 */	NdrFcShort( 0x6 ),	/* 6 */
/* 302 */	NdrFcShort( 0x28 ),	/* x86 Stack size/offset = 40 */
/* 304 */	NdrFcShort( 0x20 ),	/* 32 */
/* 306 */	NdrFcShort( 0xc6 ),	/* 198 */
/* 308 */	0x42,		/* Oi2 Flags:  clt must size, has ext, */
			0xa,		/* 10 */
/* 310 */	0x8,		/* 8 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 312 */	NdrFcShort( 0x0 ),	/* 0 */
/* 314 */	NdrFcShort( 0x1 ),	/* 1 */
/* 316 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter nVerts */

/* 318 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 320 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 322 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter verts */

/* 324 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 326 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 328 */	NdrFcShort( 0xbe ),	/* Type Offset=190 */

	/* Parameter nTris */

/* 330 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 332 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 334 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter tris */

/* 336 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 338 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 340 */	NdrFcShort( 0xd8 ),	/* Type Offset=216 */

	/* Parameter factor */

/* 342 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 344 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 346 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter tolerance */

/* 348 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 350 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 352 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter boxtransform */

/* 354 */	NdrFcShort( 0x112 ),	/* Flags:  must free, out, simple ref, */
/* 356 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 358 */	NdrFcShort( 0xf2 ),	/* Type Offset=242 */

	/* Parameter identity */

/* 360 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 362 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 364 */	0x2,		/* FC_CHAR */
			0x0,		/* 0 */

	/* Parameter boxhalfextents */

/* 366 */	NdrFcShort( 0x4112 ),	/* Flags:  must free, out, simple ref, srv alloc size=16 */
/* 368 */	NdrFcShort( 0x20 ),	/* x86 Stack size/offset = 32 */
/* 370 */	NdrFcShort( 0x6 ),	/* Type Offset=6 */

	/* Parameter exclusive */

/* 372 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 374 */	NdrFcShort( 0x24 ),	/* x86 Stack size/offset = 36 */
/* 376 */	0x2,		/* FC_CHAR */
			0x0,		/* 0 */

	/* Procedure CalcHullPolyhedron */

/* 378 */	0x32,		/* FC_BIND_PRIMITIVE */
			0x48,		/* Old Flags:  */
/* 380 */	NdrFcLong( 0x0 ),	/* 0 */
/* 384 */	NdrFcShort( 0x7 ),	/* 7 */
/* 386 */	NdrFcShort( 0x24 ),	/* x86 Stack size/offset = 36 */
/* 388 */	NdrFcShort( 0x3c ),	/* 60 */
/* 390 */	NdrFcShort( 0x35 ),	/* 53 */
/* 392 */	0x43,		/* Oi2 Flags:  srv must size, clt must size, has ext, */
			0x9,		/* 9 */
/* 394 */	0x8,		/* 8 */
			0x7,		/* Ext Flags:  new corr desc, clt corr check, srv corr check, */
/* 396 */	NdrFcShort( 0x1 ),	/* 1 */
/* 398 */	NdrFcShort( 0x1 ),	/* 1 */
/* 400 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter nVerts */

/* 402 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 404 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 406 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter verts */

/* 408 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 410 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 412 */	NdrFcShort( 0xbe ),	/* Type Offset=190 */

	/* Parameter nTris */

/* 414 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 416 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 418 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter tris */

/* 420 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 422 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 424 */	NdrFcShort( 0xd8 ),	/* Type Offset=216 */

	/* Parameter factor */

/* 426 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 428 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 430 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter tolerance */

/* 432 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 434 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 436 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter nPlanes */

/* 438 */	NdrFcShort( 0x158 ),	/* Flags:  in, out, base type, simple ref, */
/* 440 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 442 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter planes */

/* 444 */	NdrFcShort( 0x113 ),	/* Flags:  must size, must free, out, simple ref, */
/* 446 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 448 */	NdrFcShort( 0x10e ),	/* Type Offset=270 */

	/* Parameter exclusive */

/* 450 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 452 */	NdrFcShort( 0x20 ),	/* x86 Stack size/offset = 32 */
/* 454 */	0x2,		/* FC_CHAR */
			0x0,		/* 0 */

	/* Procedure CalcMassPropertiesSphere */

/* 456 */	0x32,		/* FC_BIND_PRIMITIVE */
			0x48,		/* Old Flags:  */
/* 458 */	NdrFcLong( 0x0 ),	/* 0 */
/* 462 */	NdrFcShort( 0x8 ),	/* 8 */
/* 464 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 466 */	NdrFcShort( 0x15 ),	/* 21 */
/* 468 */	NdrFcShort( 0xbc ),	/* 188 */
/* 470 */	0x40,		/* Oi2 Flags:  has ext, */
			0x7,		/* 7 */
/* 472 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 474 */	NdrFcShort( 0x0 ),	/* 0 */
/* 476 */	NdrFcShort( 0x0 ),	/* 0 */
/* 478 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter radius */

/* 480 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 482 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 484 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter density */

/* 486 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 488 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 490 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter solid */

/* 492 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 494 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 496 */	0x2,		/* FC_CHAR */
			0x0,		/* 0 */

	/* Parameter mass */

/* 498 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 500 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 502 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter volume */

/* 504 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 506 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 508 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter center */

/* 510 */	NdrFcShort( 0x4112 ),	/* Flags:  must free, out, simple ref, srv alloc size=16 */
/* 512 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 514 */	NdrFcShort( 0x6 ),	/* Type Offset=6 */

	/* Parameter inertia */

/* 516 */	NdrFcShort( 0xc112 ),	/* Flags:  must free, out, simple ref, srv alloc size=48 */
/* 518 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 520 */	NdrFcShort( 0x128 ),	/* Type Offset=296 */

	/* Procedure CalcMassPropertiesBox */

/* 522 */	0x32,		/* FC_BIND_PRIMITIVE */
			0x48,		/* Old Flags:  */
/* 524 */	NdrFcLong( 0x0 ),	/* 0 */
/* 528 */	NdrFcShort( 0x9 ),	/* 9 */
/* 530 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 532 */	NdrFcShort( 0x3d ),	/* 61 */
/* 534 */	NdrFcShort( 0xbc ),	/* 188 */
/* 536 */	0x40,		/* Oi2 Flags:  has ext, */
			0x7,		/* 7 */
/* 538 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 540 */	NdrFcShort( 0x0 ),	/* 0 */
/* 542 */	NdrFcShort( 0x0 ),	/* 0 */
/* 544 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter size */

/* 546 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 548 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 550 */	NdrFcShort( 0x6 ),	/* Type Offset=6 */

	/* Parameter density */

/* 552 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 554 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 556 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter solid */

/* 558 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 560 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 562 */	0x2,		/* FC_CHAR */
			0x0,		/* 0 */

	/* Parameter mass */

/* 564 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 566 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 568 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter volume */

/* 570 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 572 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 574 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter center */

/* 576 */	NdrFcShort( 0x4112 ),	/* Flags:  must free, out, simple ref, srv alloc size=16 */
/* 578 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 580 */	NdrFcShort( 0x6 ),	/* Type Offset=6 */

	/* Parameter inertia */

/* 582 */	NdrFcShort( 0xc112 ),	/* Flags:  must free, out, simple ref, srv alloc size=48 */
/* 584 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 586 */	NdrFcShort( 0x128 ),	/* Type Offset=296 */

	/* Procedure CalcMassPropertiesCylinder */

/* 588 */	0x32,		/* FC_BIND_PRIMITIVE */
			0x48,		/* Old Flags:  */
/* 590 */	NdrFcLong( 0x0 ),	/* 0 */
/* 594 */	NdrFcShort( 0xa ),	/* 10 */
/* 596 */	NdrFcShort( 0x24 ),	/* x86 Stack size/offset = 36 */
/* 598 */	NdrFcShort( 0x75 ),	/* 117 */
/* 600 */	NdrFcShort( 0xbc ),	/* 188 */
/* 602 */	0x40,		/* Oi2 Flags:  has ext, */
			0x9,		/* 9 */
/* 604 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 606 */	NdrFcShort( 0x0 ),	/* 0 */
/* 608 */	NdrFcShort( 0x0 ),	/* 0 */
/* 610 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter startAxis */

/* 612 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 614 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 616 */	NdrFcShort( 0x6 ),	/* Type Offset=6 */

	/* Parameter endAxis */

/* 618 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 620 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 622 */	NdrFcShort( 0x6 ),	/* Type Offset=6 */

	/* Parameter radius */

/* 624 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 626 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 628 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter density */

/* 630 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 632 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 634 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter solid */

/* 636 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 638 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 640 */	0x2,		/* FC_CHAR */
			0x0,		/* 0 */

	/* Parameter mass */

/* 642 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 644 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 646 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter volume */

/* 648 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 650 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 652 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter center */

/* 654 */	NdrFcShort( 0x4112 ),	/* Flags:  must free, out, simple ref, srv alloc size=16 */
/* 656 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 658 */	NdrFcShort( 0x6 ),	/* Type Offset=6 */

	/* Parameter inertia */

/* 660 */	NdrFcShort( 0xc112 ),	/* Flags:  must free, out, simple ref, srv alloc size=48 */
/* 662 */	NdrFcShort( 0x20 ),	/* x86 Stack size/offset = 32 */
/* 664 */	NdrFcShort( 0x128 ),	/* Type Offset=296 */

	/* Procedure CalcMassPropertiesCapsule */

/* 666 */	0x32,		/* FC_BIND_PRIMITIVE */
			0x48,		/* Old Flags:  */
/* 668 */	NdrFcLong( 0x0 ),	/* 0 */
/* 672 */	NdrFcShort( 0xb ),	/* 11 */
/* 674 */	NdrFcShort( 0x24 ),	/* x86 Stack size/offset = 36 */
/* 676 */	NdrFcShort( 0x75 ),	/* 117 */
/* 678 */	NdrFcShort( 0xbc ),	/* 188 */
/* 680 */	0x40,		/* Oi2 Flags:  has ext, */
			0x9,		/* 9 */
/* 682 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 684 */	NdrFcShort( 0x0 ),	/* 0 */
/* 686 */	NdrFcShort( 0x0 ),	/* 0 */
/* 688 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter startAxis */

/* 690 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 692 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 694 */	NdrFcShort( 0x6 ),	/* Type Offset=6 */

	/* Parameter endAxis */

/* 696 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 698 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 700 */	NdrFcShort( 0x6 ),	/* Type Offset=6 */

	/* Parameter radius */

/* 702 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 704 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 706 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter density */

/* 708 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 710 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 712 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter solid */

/* 714 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 716 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 718 */	0x2,		/* FC_CHAR */
			0x0,		/* 0 */

	/* Parameter mass */

/* 720 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 722 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 724 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter volume */

/* 726 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 728 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 730 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter center */

/* 732 */	NdrFcShort( 0x4112 ),	/* Flags:  must free, out, simple ref, srv alloc size=16 */
/* 734 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 736 */	NdrFcShort( 0x6 ),	/* Type Offset=6 */

	/* Parameter inertia */

/* 738 */	NdrFcShort( 0xc112 ),	/* Flags:  must free, out, simple ref, srv alloc size=48 */
/* 740 */	NdrFcShort( 0x20 ),	/* x86 Stack size/offset = 32 */
/* 742 */	NdrFcShort( 0x128 ),	/* Type Offset=296 */

	/* Procedure CalcMassPropertiesPolyhedron */

/* 744 */	0x32,		/* FC_BIND_PRIMITIVE */
			0x48,		/* Old Flags:  */
/* 746 */	NdrFcLong( 0x0 ),	/* 0 */
/* 750 */	NdrFcShort( 0xc ),	/* 12 */
/* 752 */	NdrFcShort( 0x28 ),	/* x86 Stack size/offset = 40 */
/* 754 */	NdrFcShort( 0x1d ),	/* 29 */
/* 756 */	NdrFcShort( 0xbc ),	/* 188 */
/* 758 */	0x42,		/* Oi2 Flags:  clt must size, has ext, */
			0xa,		/* 10 */
/* 760 */	0x8,		/* 8 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 762 */	NdrFcShort( 0x0 ),	/* 0 */
/* 764 */	NdrFcShort( 0x1 ),	/* 1 */
/* 766 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter nVerts */

/* 768 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 770 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 772 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter verts */

/* 774 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 776 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 778 */	NdrFcShort( 0xbe ),	/* Type Offset=190 */

	/* Parameter nTris */

/* 780 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 782 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 784 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter tris */

/* 786 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 788 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 790 */	NdrFcShort( 0xd8 ),	/* Type Offset=216 */

	/* Parameter density */

/* 792 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 794 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 796 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter solid */

/* 798 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 800 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 802 */	0x2,		/* FC_CHAR */
			0x0,		/* 0 */

	/* Parameter mass */

/* 804 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 806 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 808 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter volume */

/* 810 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 812 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 814 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter center */

/* 816 */	NdrFcShort( 0x4112 ),	/* Flags:  must free, out, simple ref, srv alloc size=16 */
/* 818 */	NdrFcShort( 0x20 ),	/* x86 Stack size/offset = 32 */
/* 820 */	NdrFcShort( 0x6 ),	/* Type Offset=6 */

	/* Parameter inertia */

/* 822 */	NdrFcShort( 0xc112 ),	/* Flags:  must free, out, simple ref, srv alloc size=48 */
/* 824 */	NdrFcShort( 0x24 ),	/* x86 Stack size/offset = 36 */
/* 826 */	NdrFcShort( 0x128 ),	/* Type Offset=296 */

	/* Procedure CombineMassProperties */

/* 828 */	0x32,		/* FC_BIND_PRIMITIVE */
			0x48,		/* Old Flags:  */
/* 830 */	NdrFcLong( 0x0 ),	/* 0 */
/* 834 */	NdrFcShort( 0xd ),	/* 13 */
/* 836 */	NdrFcShort( 0x28 ),	/* x86 Stack size/offset = 40 */
/* 838 */	NdrFcShort( 0x128 ),	/* 296 */
/* 840 */	NdrFcShort( 0xbc ),	/* 188 */
/* 842 */	0x40,		/* Oi2 Flags:  has ext, */
			0xa,		/* 10 */
/* 844 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 846 */	NdrFcShort( 0x0 ),	/* 0 */
/* 848 */	NdrFcShort( 0x0 ),	/* 0 */
/* 850 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter nItems */

/* 852 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 854 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 856 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter masses */

/* 858 */	NdrFcShort( 0x148 ),	/* Flags:  in, base type, simple ref, */
/* 860 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 862 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter volumes */

/* 864 */	NdrFcShort( 0x148 ),	/* Flags:  in, base type, simple ref, */
/* 866 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 868 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter centers */

/* 870 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 872 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 874 */	NdrFcShort( 0x6 ),	/* Type Offset=6 */

	/* Parameter inertias */

/* 876 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 878 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 880 */	NdrFcShort( 0x128 ),	/* Type Offset=296 */

	/* Parameter transforms */

/* 882 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 884 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 886 */	NdrFcShort( 0xf2 ),	/* Type Offset=242 */

	/* Parameter mass */

/* 888 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 890 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 892 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter volume */

/* 894 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 896 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 898 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter center */

/* 900 */	NdrFcShort( 0x4112 ),	/* Flags:  must free, out, simple ref, srv alloc size=16 */
/* 902 */	NdrFcShort( 0x20 ),	/* x86 Stack size/offset = 32 */
/* 904 */	NdrFcShort( 0x6 ),	/* Type Offset=6 */

	/* Parameter inertia */

/* 906 */	NdrFcShort( 0xc112 ),	/* Flags:  must free, out, simple ref, srv alloc size=48 */
/* 908 */	NdrFcShort( 0x24 ),	/* x86 Stack size/offset = 36 */
/* 910 */	NdrFcShort( 0x128 ),	/* Type Offset=296 */

	/* Procedure HavokInit */

/* 912 */	0x32,		/* FC_BIND_PRIMITIVE */
			0x48,		/* Old Flags:  */
/* 914 */	NdrFcLong( 0x0 ),	/* 0 */
/* 918 */	NdrFcShort( 0xe ),	/* 14 */
/* 920 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 922 */	NdrFcShort( 0x0 ),	/* 0 */
/* 924 */	NdrFcShort( 0x5 ),	/* 5 */
/* 926 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x1,		/* 1 */
/* 928 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 930 */	NdrFcShort( 0x0 ),	/* 0 */
/* 932 */	NdrFcShort( 0x0 ),	/* 0 */
/* 934 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Return value */

/* 936 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 938 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 940 */	0x2,		/* FC_CHAR */
			0x0,		/* 0 */

	/* Procedure HavokCleanup */

/* 942 */	0x32,		/* FC_BIND_PRIMITIVE */
			0x48,		/* Old Flags:  */
/* 944 */	NdrFcLong( 0x0 ),	/* 0 */
/* 948 */	NdrFcShort( 0xf ),	/* 15 */
/* 950 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 952 */	NdrFcShort( 0x0 ),	/* 0 */
/* 954 */	NdrFcShort( 0x0 ),	/* 0 */
/* 956 */	0x40,		/* Oi2 Flags:  has ext, */
			0x0,		/* 0 */
/* 958 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 960 */	NdrFcShort( 0x0 ),	/* 0 */
/* 962 */	NdrFcShort( 0x0 ),	/* 0 */
/* 964 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Procedure HavokShutdown */

/* 966 */	0x32,		/* FC_BIND_PRIMITIVE */
			0x48,		/* Old Flags:  */
/* 968 */	NdrFcLong( 0x0 ),	/* 0 */
/* 972 */	NdrFcShort( 0x10 ),	/* 16 */
/* 974 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 976 */	NdrFcShort( 0x0 ),	/* 0 */
/* 978 */	NdrFcShort( 0x0 ),	/* 0 */
/* 980 */	0x40,		/* Oi2 Flags:  has ext, */
			0x0,		/* 0 */
/* 982 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 984 */	NdrFcShort( 0x0 ),	/* 0 */
/* 986 */	NdrFcShort( 0x0 ),	/* 0 */
/* 988 */	NdrFcShort( 0x0 ),	/* 0 */

			0x0
        }
    };

static const nifopt2Dhvk_MIDL_TYPE_FORMAT_STRING nifopt2Dhvk__MIDL_TypeFormatString =
    {
        0,
        {
			NdrFcShort( 0x0 ),	/* 0 */
/*  2 */	
			0x11, 0x0,	/* FC_RP */
/*  4 */	NdrFcShort( 0x8 ),	/* Offset= 8 (12) */
/*  6 */	
			0x1d,		/* FC_SMFARRAY */
			0x3,		/* 3 */
/*  8 */	NdrFcShort( 0xc ),	/* 12 */
/* 10 */	0xa,		/* FC_FLOAT */
			0x5b,		/* FC_END */
/* 12 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 14 */	NdrFcShort( 0x0 ),	/* 0 */
/* 16 */	0x28,		/* Corr desc:  parameter, FC_LONG */
			0x0,		/*  */
/* 18 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 20 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 22 */	NdrFcLong( 0xffffffff ),	/* -1 */
/* 26 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 28 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 30 */	NdrFcShort( 0xffe8 ),	/* Offset= -24 (6) */
/* 32 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 34 */	
			0x11, 0x0,	/* FC_RP */
/* 36 */	NdrFcShort( 0x8 ),	/* Offset= 8 (44) */
/* 38 */	
			0x1d,		/* FC_SMFARRAY */
			0x1,		/* 1 */
/* 40 */	NdrFcShort( 0x6 ),	/* 6 */
/* 42 */	0x6,		/* FC_SHORT */
			0x5b,		/* FC_END */
/* 44 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x1,		/* 1 */
/* 46 */	NdrFcShort( 0x0 ),	/* 0 */
/* 48 */	0x28,		/* Corr desc:  parameter, FC_LONG */
			0x0,		/*  */
/* 50 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 52 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 54 */	NdrFcLong( 0xffffffff ),	/* -1 */
/* 58 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 60 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 62 */	NdrFcShort( 0xffe8 ),	/* Offset= -24 (38) */
/* 64 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 66 */	
			0x11, 0x0,	/* FC_RP */
/* 68 */	NdrFcShort( 0x2 ),	/* Offset= 2 (70) */
/* 70 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 72 */	NdrFcShort( 0x4 ),	/* 4 */
/* 74 */	0x28,		/* Corr desc:  parameter, FC_LONG */
			0x0,		/*  */
/* 76 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 78 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 80 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 82 */	
			0x11, 0x0,	/* FC_RP */
/* 84 */	NdrFcShort( 0x2 ),	/* Offset= 2 (86) */
/* 86 */	
			0x1b,		/* FC_CARRAY */
			0x0,		/* 0 */
/* 88 */	NdrFcShort( 0x1 ),	/* 1 */
/* 90 */	0x28,		/* Corr desc:  parameter, FC_LONG */
			0x0,		/*  */
/* 92 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 94 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 96 */	0x2,		/* FC_CHAR */
			0x5b,		/* FC_END */
/* 98 */	
			0x11, 0x0,	/* FC_RP */
/* 100 */	NdrFcShort( 0x2 ),	/* Offset= 2 (102) */
/* 102 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 104 */	NdrFcShort( 0x0 ),	/* 0 */
/* 106 */	0x28,		/* Corr desc:  parameter, FC_LONG */
			0x0,		/*  */
/* 108 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 110 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 112 */	NdrFcLong( 0xffffffff ),	/* -1 */
/* 116 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 118 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 120 */	NdrFcShort( 0xff8e ),	/* Offset= -114 (6) */
/* 122 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 124 */	
			0x11, 0x0,	/* FC_RP */
/* 126 */	NdrFcShort( 0x2 ),	/* Offset= 2 (128) */
/* 128 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x1,		/* 1 */
/* 130 */	NdrFcShort( 0x0 ),	/* 0 */
/* 132 */	0x28,		/* Corr desc:  parameter, FC_LONG */
			0x0,		/*  */
/* 134 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 136 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 138 */	NdrFcLong( 0xffffffff ),	/* -1 */
/* 142 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 144 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 146 */	NdrFcShort( 0xff94 ),	/* Offset= -108 (38) */
/* 148 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 150 */	
			0x11, 0x0,	/* FC_RP */
/* 152 */	NdrFcShort( 0x2 ),	/* Offset= 2 (154) */
/* 154 */	
			0x1b,		/* FC_CARRAY */
			0x0,		/* 0 */
/* 156 */	NdrFcShort( 0x1 ),	/* 1 */
/* 158 */	0x28,		/* Corr desc:  parameter, FC_LONG */
			0x0,		/*  */
/* 160 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 162 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 164 */	0x2,		/* FC_CHAR */
			0x5b,		/* FC_END */
/* 166 */	
			0x11, 0x0,	/* FC_RP */
/* 168 */	NdrFcShort( 0x2 ),	/* Offset= 2 (170) */
/* 170 */	
			0x1b,		/* FC_CARRAY */
			0x1,		/* 1 */
/* 172 */	NdrFcShort( 0x2 ),	/* 2 */
/* 174 */	0x28,		/* Corr desc:  parameter, FC_LONG */
			0x0,		/*  */
/* 176 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 178 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 180 */	0x6,		/* FC_SHORT */
			0x5b,		/* FC_END */
/* 182 */	
			0x11, 0xc,	/* FC_RP [alloced_on_stack] [simple_pointer] */
/* 184 */	0xa,		/* FC_FLOAT */
			0x5c,		/* FC_PAD */
/* 186 */	
			0x11, 0x0,	/* FC_RP */
/* 188 */	NdrFcShort( 0x2 ),	/* Offset= 2 (190) */
/* 190 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 192 */	NdrFcShort( 0x0 ),	/* 0 */
/* 194 */	0x28,		/* Corr desc:  parameter, FC_LONG */
			0x0,		/*  */
/* 196 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 198 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 200 */	NdrFcLong( 0xffffffff ),	/* -1 */
/* 204 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 206 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 208 */	NdrFcShort( 0xff36 ),	/* Offset= -202 (6) */
/* 210 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 212 */	
			0x11, 0x0,	/* FC_RP */
/* 214 */	NdrFcShort( 0x2 ),	/* Offset= 2 (216) */
/* 216 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x1,		/* 1 */
/* 218 */	NdrFcShort( 0x0 ),	/* 0 */
/* 220 */	0x28,		/* Corr desc:  parameter, FC_LONG */
			0x0,		/*  */
/* 222 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 224 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 226 */	NdrFcLong( 0xffffffff ),	/* -1 */
/* 230 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 232 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 234 */	NdrFcShort( 0xff3c ),	/* Offset= -196 (38) */
/* 236 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 238 */	
			0x11, 0x0,	/* FC_RP */
/* 240 */	NdrFcShort( 0x2 ),	/* Offset= 2 (242) */
/* 242 */	
			0x1d,		/* FC_SMFARRAY */
			0x3,		/* 3 */
/* 244 */	NdrFcShort( 0x40 ),	/* 64 */
/* 246 */	0xa,		/* FC_FLOAT */
			0x5b,		/* FC_END */
/* 248 */	
			0x11, 0xc,	/* FC_RP [alloced_on_stack] [simple_pointer] */
/* 250 */	0x2,		/* FC_CHAR */
			0x5c,		/* FC_PAD */
/* 252 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 254 */	NdrFcShort( 0xff08 ),	/* Offset= -248 (6) */
/* 256 */	
			0x11, 0x8,	/* FC_RP [simple_pointer] */
/* 258 */	0x8,		/* FC_LONG */
			0x5c,		/* FC_PAD */
/* 260 */	
			0x11, 0x0,	/* FC_RP */
/* 262 */	NdrFcShort( 0x8 ),	/* Offset= 8 (270) */
/* 264 */	
			0x1d,		/* FC_SMFARRAY */
			0x3,		/* 3 */
/* 266 */	NdrFcShort( 0x10 ),	/* 16 */
/* 268 */	0xa,		/* FC_FLOAT */
			0x5b,		/* FC_END */
/* 270 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 272 */	NdrFcShort( 0x0 ),	/* 0 */
/* 274 */	0x28,		/* Corr desc:  parameter, FC_LONG */
			0x0,		/*  */
/* 276 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 278 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 280 */	NdrFcLong( 0xffffffff ),	/* -1 */
/* 284 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 286 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 288 */	NdrFcShort( 0xffe8 ),	/* Offset= -24 (264) */
/* 290 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 292 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 294 */	NdrFcShort( 0x2 ),	/* Offset= 2 (296) */
/* 296 */	
			0x1d,		/* FC_SMFARRAY */
			0x3,		/* 3 */
/* 298 */	NdrFcShort( 0x30 ),	/* 48 */
/* 300 */	0xa,		/* FC_FLOAT */
			0x5b,		/* FC_END */
/* 302 */	
			0x11, 0x0,	/* FC_RP */
/* 304 */	NdrFcShort( 0xfed6 ),	/* Offset= -298 (6) */
/* 306 */	
			0x11, 0x8,	/* FC_RP [simple_pointer] */
/* 308 */	0xa,		/* FC_FLOAT */
			0x5c,		/* FC_PAD */
/* 310 */	
			0x11, 0x0,	/* FC_RP */
/* 312 */	NdrFcShort( 0xfff0 ),	/* Offset= -16 (296) */

			0x0
        }
    };

static const unsigned short havok_FormatStringOffsetTable[] =
    {
    0,
    60,
    138,
    180,
    222,
    258,
    294,
    378,
    456,
    522,
    588,
    666,
    744,
    828,
    912,
    942,
    966
    };


static const MIDL_STUB_DESC havok_StubDesc = 
    {
    (void *)& havok___RpcServerInterface,
    MIDL_user_allocate,
    MIDL_user_free,
    0,
    0,
    0,
    0,
    0,
    nifopt2Dhvk__MIDL_TypeFormatString.Format,
    1, /* -error bounds_check flag */
    0x50002, /* Ndr library version */
    0,
    0x700022b, /* MIDL Version 7.0.555 */
    0,
    0,
    0,  /* notify & notify_flag routine table */
    0x1, /* MIDL flag */
    0, /* cs routines */
    0,   /* proxy/server info */
    0
    };

static const RPC_DISPATCH_FUNCTION havok_table[] =
    {
    NdrServerCall2,
    NdrServerCall2,
    NdrServerCall2,
    NdrServerCall2,
    NdrServerCall2,
    NdrServerCall2,
    NdrServerCall2,
    NdrServerCall2,
    NdrServerCall2,
    NdrServerCall2,
    NdrServerCall2,
    NdrServerCall2,
    NdrServerCall2,
    NdrServerCall2,
    NdrServerCall2,
    NdrServerCall2,
    NdrServerCall2,
    0
    };
static const RPC_DISPATCH_TABLE havok_v1_0_DispatchTable = 
    {
    17,
    (RPC_DISPATCH_FUNCTION*)havok_table
    };

static const SERVER_ROUTINE havok_ServerRoutineTable[] = 
    {
    (SERVER_ROUTINE)GenerateMoppCode,
    (SERVER_ROUTINE)GenerateMoppCodeWithSubshapes,
    (SERVER_ROUTINE)RetrieveMoppCode,
    (SERVER_ROUTINE)RetrieveMoppWelding,
    (SERVER_ROUTINE)RetrieveMoppScale,
    (SERVER_ROUTINE)RetrieveMoppOrigin,
    (SERVER_ROUTINE)CalcOBBPolyhedron,
    (SERVER_ROUTINE)CalcHullPolyhedron,
    (SERVER_ROUTINE)CalcMassPropertiesSphere,
    (SERVER_ROUTINE)CalcMassPropertiesBox,
    (SERVER_ROUTINE)CalcMassPropertiesCylinder,
    (SERVER_ROUTINE)CalcMassPropertiesCapsule,
    (SERVER_ROUTINE)CalcMassPropertiesPolyhedron,
    (SERVER_ROUTINE)CombineMassProperties,
    (SERVER_ROUTINE)HavokInit,
    (SERVER_ROUTINE)HavokCleanup,
    (SERVER_ROUTINE)HavokShutdown
    };

static const MIDL_SERVER_INFO havok_ServerInfo = 
    {
    &havok_StubDesc,
    havok_ServerRoutineTable,
    nifopt2Dhvk__MIDL_ProcFormatString.Format,
    havok_FormatStringOffsetTable,
    0,
    0,
    0,
    0};
#pragma optimize("", on )
#if _MSC_VER >= 1200
#pragma warning(pop)
#endif


#endif /* !defined(_M_IA64) && !defined(_M_AMD64)*/

