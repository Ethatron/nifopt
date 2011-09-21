

/* this ALWAYS GENERATED file contains the RPC client stubs */


 /* File created by MIDL compiler version 7.00.0555 */
/* at Mon Sep 19 16:48:04 2011
 */
/* Compiler settings for nifopt-hvk.idl:
    Oicf, W1, Zp8, env=Win64 (32b run), target_arch=AMD64 7.00.0555 
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */

#if defined(_M_AMD64)


#pragma warning( disable: 4049 )  /* more than 64k source lines */
#if _MSC_VER >= 1200
#pragma warning(push)
#endif

#pragma warning( disable: 4211 )  /* redefine extern to static */
#pragma warning( disable: 4232 )  /* dllimport identity*/
#pragma warning( disable: 4024 )  /* array to pointer mapping*/

#include <string.h>

#include "nifopt-hvk.h"

#define TYPE_FORMAT_STRING_SIZE   315                               
#define PROC_FORMAT_STRING_SIZE   1025                              
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

#define GENERIC_BINDING_TABLE_SIZE   0            


/* Standard interface: havok, ver. 1.0,
   GUID={0x79eb9777,0x29c9,0x427f,{0x8f,0x2a,0xac,0x64,0xf7,0xa6,0xbf,0x5a}} */

handle_t havok_IfHandle;


static const RPC_CLIENT_INTERFACE havok___RpcClientInterface =
    {
    sizeof(RPC_CLIENT_INTERFACE),
    {{0x79eb9777,0x29c9,0x427f,{0x8f,0x2a,0xac,0x64,0xf7,0xa6,0xbf,0x5a}},{1,0}},
    {{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}},
    0,
    0,
    0,
    0,
    0,
    0x00000000
    };
RPC_IF_HANDLE havok_v1_0_c_ifspec = (RPC_IF_HANDLE)& havok___RpcClientInterface;

extern const MIDL_STUB_DESC havok_StubDesc;

static RPC_BINDING_HANDLE havok__MIDL_AutoBindHandle;


int GenerateMoppCode( 
    /* [in] */ hvkByte material,
    /* [in] */ int nVerts,
    /* [size_is][ref][in] */ const hvkPoint3 *verts,
    /* [in] */ int nTris,
    /* [size_is][ref][in] */ const hvkTriangle *tris)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&havok_StubDesc,
                  (PFORMAT_STRING) &nifopt2Dhvk__MIDL_ProcFormatString.Format[0],
                  material,
                  nVerts,
                  verts,
                  nTris,
                  tris);
    return ( int  )_RetVal.Simple;
    
}


int GenerateMoppCodeWithSubshapes( 
    /* [in] */ int nShapes,
    /* [size_is][ref][in] */ const int *shapeVerts,
    /* [in] */ int nMaterials,
    /* [size_is][ref][in] */ const hvkByte *materialIDs,
    /* [in] */ int nVerts,
    /* [size_is][ref][in] */ const hvkPoint3 *verts,
    /* [in] */ int nTris,
    /* [size_is][ref][in] */ const hvkTriangle *tris)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&havok_StubDesc,
                  (PFORMAT_STRING) &nifopt2Dhvk__MIDL_ProcFormatString.Format[62],
                  nShapes,
                  shapeVerts,
                  nMaterials,
                  materialIDs,
                  nVerts,
                  verts,
                  nTris,
                  tris);
    return ( int  )_RetVal.Simple;
    
}


int RetrieveMoppCode( 
    /* [in] */ int nBuffer,
    /* [size_is][ref][out] */ hvkByte *buffer)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&havok_StubDesc,
                  (PFORMAT_STRING) &nifopt2Dhvk__MIDL_ProcFormatString.Format[142],
                  nBuffer,
                  buffer);
    return ( int  )_RetVal.Simple;
    
}


int RetrieveMoppWelding( 
    /* [in] */ int nBuffer,
    /* [size_is][ref][out] */ hvkShort *buffer)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&havok_StubDesc,
                  (PFORMAT_STRING) &nifopt2Dhvk__MIDL_ProcFormatString.Format[186],
                  nBuffer,
                  buffer);
    return ( int  )_RetVal.Simple;
    
}


int RetrieveMoppScale( 
    /* [out] */ float *value)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&havok_StubDesc,
                  (PFORMAT_STRING) &nifopt2Dhvk__MIDL_ProcFormatString.Format[230],
                  value);
    return ( int  )_RetVal.Simple;
    
}


int RetrieveMoppOrigin( 
    /* [out] */ float *value)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&havok_StubDesc,
                  (PFORMAT_STRING) &nifopt2Dhvk__MIDL_ProcFormatString.Format[268],
                  value);
    return ( int  )_RetVal.Simple;
    
}


void CalcOBBPolyhedron( 
    /* [in] */ int nVerts,
    /* [size_is][ref][in] */ const hvkPoint3 *verts,
    /* [in] */ int nTris,
    /* [size_is][ref][in] */ const hvkTriangle *tris,
    /* [in] */ float factor,
    /* [in] */ float tolerance,
    /* [ref][out] */ hvkMatrix44 *boxtransform,
    /* [ref][out] */ bool *identity,
    /* [ref][out] */ hvkPoint3 *boxhalfextents,
    /* [ref][out] */ bool *exclusive)
{

    NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&havok_StubDesc,
                  (PFORMAT_STRING) &nifopt2Dhvk__MIDL_ProcFormatString.Format[306],
                  nVerts,
                  verts,
                  nTris,
                  tris,
                  factor,
                  tolerance,
                  boxtransform,
                  identity,
                  boxhalfextents,
                  exclusive);
    
}


void CalcHullPolyhedron( 
    /* [in] */ int nVerts,
    /* [size_is][ref][in] */ const hvkPoint3 *verts,
    /* [in] */ int nTris,
    /* [size_is][ref][in] */ const hvkTriangle *tris,
    /* [in] */ float factor,
    /* [in] */ float tolerance,
    /* [ref][out][in] */ int *nPlanes,
    /* [size_is][ref][out] */ hvkPoint4 *planes,
    /* [ref][out] */ bool *exclusive)
{

    NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&havok_StubDesc,
                  (PFORMAT_STRING) &nifopt2Dhvk__MIDL_ProcFormatString.Format[392],
                  nVerts,
                  verts,
                  nTris,
                  tris,
                  factor,
                  tolerance,
                  nPlanes,
                  planes,
                  exclusive);
    
}


void CalcMassPropertiesSphere( 
    /* [in] */ float radius,
    /* [in] */ float density,
    /* [in] */ bool solid,
    /* [ref][out] */ float *mass,
    /* [ref][out] */ float *volume,
    /* [ref][out] */ hvkPoint3 *center,
    /* [ref][out] */ hvkMatrix43 *inertia)
{

    NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&havok_StubDesc,
                  (PFORMAT_STRING) &nifopt2Dhvk__MIDL_ProcFormatString.Format[472],
                  radius,
                  density,
                  solid,
                  mass,
                  volume,
                  center,
                  inertia);
    
}


void CalcMassPropertiesBox( 
    /* [ref][in] */ const hvkPoint3 *size,
    /* [in] */ float density,
    /* [in] */ bool solid,
    /* [ref][out] */ float *mass,
    /* [ref][out] */ float *volume,
    /* [ref][out] */ hvkPoint3 *center,
    /* [ref][out] */ hvkMatrix43 *inertia)
{

    NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&havok_StubDesc,
                  (PFORMAT_STRING) &nifopt2Dhvk__MIDL_ProcFormatString.Format[540],
                  size,
                  density,
                  solid,
                  mass,
                  volume,
                  center,
                  inertia);
    
}


void CalcMassPropertiesCylinder( 
    /* [ref][in] */ const hvkPoint3 *startAxis,
    /* [ref][in] */ const hvkPoint3 *endAxis,
    /* [in] */ float radius,
    /* [in] */ float density,
    /* [in] */ bool solid,
    /* [ref][out] */ float *mass,
    /* [ref][out] */ float *volume,
    /* [ref][out] */ hvkPoint3 *center,
    /* [ref][out] */ hvkMatrix43 *inertia)
{

    NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&havok_StubDesc,
                  (PFORMAT_STRING) &nifopt2Dhvk__MIDL_ProcFormatString.Format[608],
                  startAxis,
                  endAxis,
                  radius,
                  density,
                  solid,
                  mass,
                  volume,
                  center,
                  inertia);
    
}


void CalcMassPropertiesCapsule( 
    /* [ref][in] */ const hvkPoint3 *startAxis,
    /* [ref][in] */ const hvkPoint3 *endAxis,
    /* [in] */ float radius,
    /* [in] */ float density,
    /* [in] */ bool solid,
    /* [ref][out] */ float *mass,
    /* [ref][out] */ float *volume,
    /* [ref][out] */ hvkPoint3 *center,
    /* [ref][out] */ hvkMatrix43 *inertia)
{

    NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&havok_StubDesc,
                  (PFORMAT_STRING) &nifopt2Dhvk__MIDL_ProcFormatString.Format[688],
                  startAxis,
                  endAxis,
                  radius,
                  density,
                  solid,
                  mass,
                  volume,
                  center,
                  inertia);
    
}


void CalcMassPropertiesPolyhedron( 
    /* [in] */ int nVerts,
    /* [size_is][ref][in] */ const hvkPoint3 *verts,
    /* [in] */ int nTris,
    /* [size_is][ref][in] */ const hvkTriangle *tris,
    /* [in] */ float density,
    /* [in] */ bool solid,
    /* [ref][out] */ float *mass,
    /* [ref][out] */ float *volume,
    /* [ref][out] */ hvkPoint3 *center,
    /* [ref][out] */ hvkMatrix43 *inertia)
{

    NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&havok_StubDesc,
                  (PFORMAT_STRING) &nifopt2Dhvk__MIDL_ProcFormatString.Format[768],
                  nVerts,
                  verts,
                  nTris,
                  tris,
                  density,
                  solid,
                  mass,
                  volume,
                  center,
                  inertia);
    
}


void CombineMassProperties( 
    /* [in] */ int nItems,
    /* [ref][in] */ const float *masses,
    /* [ref][in] */ const float *volumes,
    /* [ref][in] */ const hvkPoint3 *centers,
    /* [ref][in] */ const hvkMatrix43 *inertias,
    /* [ref][in] */ const hvkMatrix44 *transforms,
    /* [ref][out] */ float *mass,
    /* [ref][out] */ float *volume,
    /* [ref][out] */ hvkPoint3 *center,
    /* [ref][out] */ hvkMatrix43 *inertia)
{

    NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&havok_StubDesc,
                  (PFORMAT_STRING) &nifopt2Dhvk__MIDL_ProcFormatString.Format[854],
                  nItems,
                  masses,
                  volumes,
                  centers,
                  inertias,
                  transforms,
                  mass,
                  volume,
                  center,
                  inertia);
    
}


bool HavokInit( void)
{

    CLIENT_CALL_RETURN _RetVal;

    _RetVal = NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&havok_StubDesc,
                  (PFORMAT_STRING) &nifopt2Dhvk__MIDL_ProcFormatString.Format[940],
                  0);
    return ( bool  )_RetVal.Simple;
    
}


void HavokCleanup( void)
{

    NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&havok_StubDesc,
                  (PFORMAT_STRING) &nifopt2Dhvk__MIDL_ProcFormatString.Format[972],
                  0);
    
}


void HavokShutdown( void)
{

    NdrClientCall2(
                  ( PMIDL_STUB_DESC  )&havok_StubDesc,
                  (PFORMAT_STRING) &nifopt2Dhvk__MIDL_ProcFormatString.Format[998],
                  0);
    
}


#if !defined(__RPC_WIN64__)
#error  Invalid build platform for this stub.
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
/*  8 */	NdrFcShort( 0x30 ),	/* X64 Stack size/offset = 48 */
/* 10 */	NdrFcShort( 0x15 ),	/* 21 */
/* 12 */	NdrFcShort( 0x8 ),	/* 8 */
/* 14 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x6,		/* 6 */
/* 16 */	0xa,		/* 10 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 18 */	NdrFcShort( 0x0 ),	/* 0 */
/* 20 */	NdrFcShort( 0x1 ),	/* 1 */
/* 22 */	NdrFcShort( 0x0 ),	/* 0 */
/* 24 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter material */

/* 26 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 28 */	NdrFcShort( 0x0 ),	/* X64 Stack size/offset = 0 */
/* 30 */	0x2,		/* FC_CHAR */
			0x0,		/* 0 */

	/* Parameter nVerts */

/* 32 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 34 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 36 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter verts */

/* 38 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 40 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 42 */	NdrFcShort( 0xc ),	/* Type Offset=12 */

	/* Parameter nTris */

/* 44 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 46 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 48 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter tris */

/* 50 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 52 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 54 */	NdrFcShort( 0x2c ),	/* Type Offset=44 */

	/* Return value */

/* 56 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 58 */	NdrFcShort( 0x28 ),	/* X64 Stack size/offset = 40 */
/* 60 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GenerateMoppCodeWithSubshapes */

/* 62 */	0x32,		/* FC_BIND_PRIMITIVE */
			0x48,		/* Old Flags:  */
/* 64 */	NdrFcLong( 0x0 ),	/* 0 */
/* 68 */	NdrFcShort( 0x1 ),	/* 1 */
/* 70 */	NdrFcShort( 0x48 ),	/* X64 Stack size/offset = 72 */
/* 72 */	NdrFcShort( 0x20 ),	/* 32 */
/* 74 */	NdrFcShort( 0x8 ),	/* 8 */
/* 76 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x9,		/* 9 */
/* 78 */	0xa,		/* 10 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 80 */	NdrFcShort( 0x0 ),	/* 0 */
/* 82 */	NdrFcShort( 0x1 ),	/* 1 */
/* 84 */	NdrFcShort( 0x0 ),	/* 0 */
/* 86 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter nShapes */

/* 88 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 90 */	NdrFcShort( 0x0 ),	/* X64 Stack size/offset = 0 */
/* 92 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter shapeVerts */

/* 94 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 96 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 98 */	NdrFcShort( 0x46 ),	/* Type Offset=70 */

	/* Parameter nMaterials */

/* 100 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 102 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 104 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter materialIDs */

/* 106 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 108 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 110 */	NdrFcShort( 0x56 ),	/* Type Offset=86 */

	/* Parameter nVerts */

/* 112 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 114 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 116 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter verts */

/* 118 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 120 */	NdrFcShort( 0x28 ),	/* X64 Stack size/offset = 40 */
/* 122 */	NdrFcShort( 0x66 ),	/* Type Offset=102 */

	/* Parameter nTris */

/* 124 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 126 */	NdrFcShort( 0x30 ),	/* X64 Stack size/offset = 48 */
/* 128 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter tris */

/* 130 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 132 */	NdrFcShort( 0x38 ),	/* X64 Stack size/offset = 56 */
/* 134 */	NdrFcShort( 0x80 ),	/* Type Offset=128 */

	/* Return value */

/* 136 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 138 */	NdrFcShort( 0x40 ),	/* X64 Stack size/offset = 64 */
/* 140 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure RetrieveMoppCode */

/* 142 */	0x32,		/* FC_BIND_PRIMITIVE */
			0x48,		/* Old Flags:  */
/* 144 */	NdrFcLong( 0x0 ),	/* 0 */
/* 148 */	NdrFcShort( 0x2 ),	/* 2 */
/* 150 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 152 */	NdrFcShort( 0x8 ),	/* 8 */
/* 154 */	NdrFcShort( 0x8 ),	/* 8 */
/* 156 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x3,		/* 3 */
/* 158 */	0xa,		/* 10 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 160 */	NdrFcShort( 0x1 ),	/* 1 */
/* 162 */	NdrFcShort( 0x0 ),	/* 0 */
/* 164 */	NdrFcShort( 0x0 ),	/* 0 */
/* 166 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter nBuffer */

/* 168 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 170 */	NdrFcShort( 0x0 ),	/* X64 Stack size/offset = 0 */
/* 172 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter buffer */

/* 174 */	NdrFcShort( 0x113 ),	/* Flags:  must size, must free, out, simple ref, */
/* 176 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 178 */	NdrFcShort( 0x9a ),	/* Type Offset=154 */

	/* Return value */

/* 180 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 182 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 184 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure RetrieveMoppWelding */

/* 186 */	0x32,		/* FC_BIND_PRIMITIVE */
			0x48,		/* Old Flags:  */
/* 188 */	NdrFcLong( 0x0 ),	/* 0 */
/* 192 */	NdrFcShort( 0x3 ),	/* 3 */
/* 194 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 196 */	NdrFcShort( 0x8 ),	/* 8 */
/* 198 */	NdrFcShort( 0x8 ),	/* 8 */
/* 200 */	0x45,		/* Oi2 Flags:  srv must size, has return, has ext, */
			0x3,		/* 3 */
/* 202 */	0xa,		/* 10 */
			0x3,		/* Ext Flags:  new corr desc, clt corr check, */
/* 204 */	NdrFcShort( 0x1 ),	/* 1 */
/* 206 */	NdrFcShort( 0x0 ),	/* 0 */
/* 208 */	NdrFcShort( 0x0 ),	/* 0 */
/* 210 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter nBuffer */

/* 212 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 214 */	NdrFcShort( 0x0 ),	/* X64 Stack size/offset = 0 */
/* 216 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter buffer */

/* 218 */	NdrFcShort( 0x113 ),	/* Flags:  must size, must free, out, simple ref, */
/* 220 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 222 */	NdrFcShort( 0xaa ),	/* Type Offset=170 */

	/* Return value */

/* 224 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 226 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 228 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure RetrieveMoppScale */

/* 230 */	0x32,		/* FC_BIND_PRIMITIVE */
			0x48,		/* Old Flags:  */
/* 232 */	NdrFcLong( 0x0 ),	/* 0 */
/* 236 */	NdrFcShort( 0x4 ),	/* 4 */
/* 238 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 240 */	NdrFcShort( 0x0 ),	/* 0 */
/* 242 */	NdrFcShort( 0x24 ),	/* 36 */
/* 244 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 246 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 248 */	NdrFcShort( 0x0 ),	/* 0 */
/* 250 */	NdrFcShort( 0x0 ),	/* 0 */
/* 252 */	NdrFcShort( 0x0 ),	/* 0 */
/* 254 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter value */

/* 256 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 258 */	NdrFcShort( 0x0 ),	/* X64 Stack size/offset = 0 */
/* 260 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Return value */

/* 262 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 264 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 266 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure RetrieveMoppOrigin */

/* 268 */	0x32,		/* FC_BIND_PRIMITIVE */
			0x48,		/* Old Flags:  */
/* 270 */	NdrFcLong( 0x0 ),	/* 0 */
/* 274 */	NdrFcShort( 0x5 ),	/* 5 */
/* 276 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 278 */	NdrFcShort( 0x0 ),	/* 0 */
/* 280 */	NdrFcShort( 0x24 ),	/* 36 */
/* 282 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 284 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 286 */	NdrFcShort( 0x0 ),	/* 0 */
/* 288 */	NdrFcShort( 0x0 ),	/* 0 */
/* 290 */	NdrFcShort( 0x0 ),	/* 0 */
/* 292 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter value */

/* 294 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 296 */	NdrFcShort( 0x0 ),	/* X64 Stack size/offset = 0 */
/* 298 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Return value */

/* 300 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 302 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 304 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure CalcOBBPolyhedron */

/* 306 */	0x32,		/* FC_BIND_PRIMITIVE */
			0x48,		/* Old Flags:  */
/* 308 */	NdrFcLong( 0x0 ),	/* 0 */
/* 312 */	NdrFcShort( 0x6 ),	/* 6 */
/* 314 */	NdrFcShort( 0x50 ),	/* X64 Stack size/offset = 80 */
/* 316 */	NdrFcShort( 0x20 ),	/* 32 */
/* 318 */	NdrFcShort( 0xc6 ),	/* 198 */
/* 320 */	0x42,		/* Oi2 Flags:  clt must size, has ext, */
			0xa,		/* 10 */
/* 322 */	0xa,		/* 10 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 324 */	NdrFcShort( 0x0 ),	/* 0 */
/* 326 */	NdrFcShort( 0x1 ),	/* 1 */
/* 328 */	NdrFcShort( 0x0 ),	/* 0 */
/* 330 */	NdrFcShort( 0x500 ),	/* 1280 */

	/* Parameter nVerts */

/* 332 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 334 */	NdrFcShort( 0x0 ),	/* X64 Stack size/offset = 0 */
/* 336 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter verts */

/* 338 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 340 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 342 */	NdrFcShort( 0xbe ),	/* Type Offset=190 */

	/* Parameter nTris */

/* 344 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 346 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 348 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter tris */

/* 350 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 352 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 354 */	NdrFcShort( 0xd8 ),	/* Type Offset=216 */

	/* Parameter factor */

/* 356 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 358 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 360 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter tolerance */

/* 362 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 364 */	NdrFcShort( 0x28 ),	/* X64 Stack size/offset = 40 */
/* 366 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter boxtransform */

/* 368 */	NdrFcShort( 0x112 ),	/* Flags:  must free, out, simple ref, */
/* 370 */	NdrFcShort( 0x30 ),	/* X64 Stack size/offset = 48 */
/* 372 */	NdrFcShort( 0xf2 ),	/* Type Offset=242 */

	/* Parameter identity */

/* 374 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 376 */	NdrFcShort( 0x38 ),	/* X64 Stack size/offset = 56 */
/* 378 */	0x2,		/* FC_CHAR */
			0x0,		/* 0 */

	/* Parameter boxhalfextents */

/* 380 */	NdrFcShort( 0x4112 ),	/* Flags:  must free, out, simple ref, srv alloc size=16 */
/* 382 */	NdrFcShort( 0x40 ),	/* X64 Stack size/offset = 64 */
/* 384 */	NdrFcShort( 0x6 ),	/* Type Offset=6 */

	/* Parameter exclusive */

/* 386 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 388 */	NdrFcShort( 0x48 ),	/* X64 Stack size/offset = 72 */
/* 390 */	0x2,		/* FC_CHAR */
			0x0,		/* 0 */

	/* Procedure CalcHullPolyhedron */

/* 392 */	0x32,		/* FC_BIND_PRIMITIVE */
			0x48,		/* Old Flags:  */
/* 394 */	NdrFcLong( 0x0 ),	/* 0 */
/* 398 */	NdrFcShort( 0x7 ),	/* 7 */
/* 400 */	NdrFcShort( 0x48 ),	/* X64 Stack size/offset = 72 */
/* 402 */	NdrFcShort( 0x3c ),	/* 60 */
/* 404 */	NdrFcShort( 0x35 ),	/* 53 */
/* 406 */	0x43,		/* Oi2 Flags:  srv must size, clt must size, has ext, */
			0x9,		/* 9 */
/* 408 */	0xa,		/* 10 */
			0x7,		/* Ext Flags:  new corr desc, clt corr check, srv corr check, */
/* 410 */	NdrFcShort( 0x1 ),	/* 1 */
/* 412 */	NdrFcShort( 0x1 ),	/* 1 */
/* 414 */	NdrFcShort( 0x0 ),	/* 0 */
/* 416 */	NdrFcShort( 0x500 ),	/* 1280 */

	/* Parameter nVerts */

/* 418 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 420 */	NdrFcShort( 0x0 ),	/* X64 Stack size/offset = 0 */
/* 422 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter verts */

/* 424 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 426 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 428 */	NdrFcShort( 0xbe ),	/* Type Offset=190 */

	/* Parameter nTris */

/* 430 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 432 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 434 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter tris */

/* 436 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 438 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 440 */	NdrFcShort( 0xd8 ),	/* Type Offset=216 */

	/* Parameter factor */

/* 442 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 444 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 446 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter tolerance */

/* 448 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 450 */	NdrFcShort( 0x28 ),	/* X64 Stack size/offset = 40 */
/* 452 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter nPlanes */

/* 454 */	NdrFcShort( 0x158 ),	/* Flags:  in, out, base type, simple ref, */
/* 456 */	NdrFcShort( 0x30 ),	/* X64 Stack size/offset = 48 */
/* 458 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter planes */

/* 460 */	NdrFcShort( 0x113 ),	/* Flags:  must size, must free, out, simple ref, */
/* 462 */	NdrFcShort( 0x38 ),	/* X64 Stack size/offset = 56 */
/* 464 */	NdrFcShort( 0x10e ),	/* Type Offset=270 */

	/* Parameter exclusive */

/* 466 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 468 */	NdrFcShort( 0x40 ),	/* X64 Stack size/offset = 64 */
/* 470 */	0x2,		/* FC_CHAR */
			0x0,		/* 0 */

	/* Procedure CalcMassPropertiesSphere */

/* 472 */	0x32,		/* FC_BIND_PRIMITIVE */
			0x48,		/* Old Flags:  */
/* 474 */	NdrFcLong( 0x0 ),	/* 0 */
/* 478 */	NdrFcShort( 0x8 ),	/* 8 */
/* 480 */	NdrFcShort( 0x38 ),	/* X64 Stack size/offset = 56 */
/* 482 */	NdrFcShort( 0x15 ),	/* 21 */
/* 484 */	NdrFcShort( 0xbc ),	/* 188 */
/* 486 */	0x40,		/* Oi2 Flags:  has ext, */
			0x7,		/* 7 */
/* 488 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 490 */	NdrFcShort( 0x0 ),	/* 0 */
/* 492 */	NdrFcShort( 0x0 ),	/* 0 */
/* 494 */	NdrFcShort( 0x0 ),	/* 0 */
/* 496 */	NdrFcShort( 0x5 ),	/* 5 */

	/* Parameter radius */

/* 498 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 500 */	NdrFcShort( 0x0 ),	/* X64 Stack size/offset = 0 */
/* 502 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter density */

/* 504 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 506 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 508 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter solid */

/* 510 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 512 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 514 */	0x2,		/* FC_CHAR */
			0x0,		/* 0 */

	/* Parameter mass */

/* 516 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 518 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 520 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter volume */

/* 522 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 524 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 526 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter center */

/* 528 */	NdrFcShort( 0x4112 ),	/* Flags:  must free, out, simple ref, srv alloc size=16 */
/* 530 */	NdrFcShort( 0x28 ),	/* X64 Stack size/offset = 40 */
/* 532 */	NdrFcShort( 0x6 ),	/* Type Offset=6 */

	/* Parameter inertia */

/* 534 */	NdrFcShort( 0xc112 ),	/* Flags:  must free, out, simple ref, srv alloc size=48 */
/* 536 */	NdrFcShort( 0x30 ),	/* X64 Stack size/offset = 48 */
/* 538 */	NdrFcShort( 0x128 ),	/* Type Offset=296 */

	/* Procedure CalcMassPropertiesBox */

/* 540 */	0x32,		/* FC_BIND_PRIMITIVE */
			0x48,		/* Old Flags:  */
/* 542 */	NdrFcLong( 0x0 ),	/* 0 */
/* 546 */	NdrFcShort( 0x9 ),	/* 9 */
/* 548 */	NdrFcShort( 0x38 ),	/* X64 Stack size/offset = 56 */
/* 550 */	NdrFcShort( 0x3d ),	/* 61 */
/* 552 */	NdrFcShort( 0xbc ),	/* 188 */
/* 554 */	0x40,		/* Oi2 Flags:  has ext, */
			0x7,		/* 7 */
/* 556 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 558 */	NdrFcShort( 0x0 ),	/* 0 */
/* 560 */	NdrFcShort( 0x0 ),	/* 0 */
/* 562 */	NdrFcShort( 0x0 ),	/* 0 */
/* 564 */	NdrFcShort( 0x4 ),	/* 4 */

	/* Parameter size */

/* 566 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 568 */	NdrFcShort( 0x0 ),	/* X64 Stack size/offset = 0 */
/* 570 */	NdrFcShort( 0x6 ),	/* Type Offset=6 */

	/* Parameter density */

/* 572 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 574 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 576 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter solid */

/* 578 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 580 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 582 */	0x2,		/* FC_CHAR */
			0x0,		/* 0 */

	/* Parameter mass */

/* 584 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 586 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 588 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter volume */

/* 590 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 592 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 594 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter center */

/* 596 */	NdrFcShort( 0x4112 ),	/* Flags:  must free, out, simple ref, srv alloc size=16 */
/* 598 */	NdrFcShort( 0x28 ),	/* X64 Stack size/offset = 40 */
/* 600 */	NdrFcShort( 0x6 ),	/* Type Offset=6 */

	/* Parameter inertia */

/* 602 */	NdrFcShort( 0xc112 ),	/* Flags:  must free, out, simple ref, srv alloc size=48 */
/* 604 */	NdrFcShort( 0x30 ),	/* X64 Stack size/offset = 48 */
/* 606 */	NdrFcShort( 0x128 ),	/* Type Offset=296 */

	/* Procedure CalcMassPropertiesCylinder */

/* 608 */	0x32,		/* FC_BIND_PRIMITIVE */
			0x48,		/* Old Flags:  */
/* 610 */	NdrFcLong( 0x0 ),	/* 0 */
/* 614 */	NdrFcShort( 0xa ),	/* 10 */
/* 616 */	NdrFcShort( 0x48 ),	/* X64 Stack size/offset = 72 */
/* 618 */	NdrFcShort( 0x75 ),	/* 117 */
/* 620 */	NdrFcShort( 0xbc ),	/* 188 */
/* 622 */	0x40,		/* Oi2 Flags:  has ext, */
			0x9,		/* 9 */
/* 624 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 626 */	NdrFcShort( 0x0 ),	/* 0 */
/* 628 */	NdrFcShort( 0x0 ),	/* 0 */
/* 630 */	NdrFcShort( 0x0 ),	/* 0 */
/* 632 */	NdrFcShort( 0x50 ),	/* 80 */

	/* Parameter startAxis */

/* 634 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 636 */	NdrFcShort( 0x0 ),	/* X64 Stack size/offset = 0 */
/* 638 */	NdrFcShort( 0x6 ),	/* Type Offset=6 */

	/* Parameter endAxis */

/* 640 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 642 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 644 */	NdrFcShort( 0x6 ),	/* Type Offset=6 */

	/* Parameter radius */

/* 646 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 648 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 650 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter density */

/* 652 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 654 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 656 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter solid */

/* 658 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 660 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 662 */	0x2,		/* FC_CHAR */
			0x0,		/* 0 */

	/* Parameter mass */

/* 664 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 666 */	NdrFcShort( 0x28 ),	/* X64 Stack size/offset = 40 */
/* 668 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter volume */

/* 670 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 672 */	NdrFcShort( 0x30 ),	/* X64 Stack size/offset = 48 */
/* 674 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter center */

/* 676 */	NdrFcShort( 0x4112 ),	/* Flags:  must free, out, simple ref, srv alloc size=16 */
/* 678 */	NdrFcShort( 0x38 ),	/* X64 Stack size/offset = 56 */
/* 680 */	NdrFcShort( 0x6 ),	/* Type Offset=6 */

	/* Parameter inertia */

/* 682 */	NdrFcShort( 0xc112 ),	/* Flags:  must free, out, simple ref, srv alloc size=48 */
/* 684 */	NdrFcShort( 0x40 ),	/* X64 Stack size/offset = 64 */
/* 686 */	NdrFcShort( 0x128 ),	/* Type Offset=296 */

	/* Procedure CalcMassPropertiesCapsule */

/* 688 */	0x32,		/* FC_BIND_PRIMITIVE */
			0x48,		/* Old Flags:  */
/* 690 */	NdrFcLong( 0x0 ),	/* 0 */
/* 694 */	NdrFcShort( 0xb ),	/* 11 */
/* 696 */	NdrFcShort( 0x48 ),	/* X64 Stack size/offset = 72 */
/* 698 */	NdrFcShort( 0x75 ),	/* 117 */
/* 700 */	NdrFcShort( 0xbc ),	/* 188 */
/* 702 */	0x40,		/* Oi2 Flags:  has ext, */
			0x9,		/* 9 */
/* 704 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 706 */	NdrFcShort( 0x0 ),	/* 0 */
/* 708 */	NdrFcShort( 0x0 ),	/* 0 */
/* 710 */	NdrFcShort( 0x0 ),	/* 0 */
/* 712 */	NdrFcShort( 0x50 ),	/* 80 */

	/* Parameter startAxis */

/* 714 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 716 */	NdrFcShort( 0x0 ),	/* X64 Stack size/offset = 0 */
/* 718 */	NdrFcShort( 0x6 ),	/* Type Offset=6 */

	/* Parameter endAxis */

/* 720 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 722 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 724 */	NdrFcShort( 0x6 ),	/* Type Offset=6 */

	/* Parameter radius */

/* 726 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 728 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 730 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter density */

/* 732 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 734 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 736 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter solid */

/* 738 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 740 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 742 */	0x2,		/* FC_CHAR */
			0x0,		/* 0 */

	/* Parameter mass */

/* 744 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 746 */	NdrFcShort( 0x28 ),	/* X64 Stack size/offset = 40 */
/* 748 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter volume */

/* 750 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 752 */	NdrFcShort( 0x30 ),	/* X64 Stack size/offset = 48 */
/* 754 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter center */

/* 756 */	NdrFcShort( 0x4112 ),	/* Flags:  must free, out, simple ref, srv alloc size=16 */
/* 758 */	NdrFcShort( 0x38 ),	/* X64 Stack size/offset = 56 */
/* 760 */	NdrFcShort( 0x6 ),	/* Type Offset=6 */

	/* Parameter inertia */

/* 762 */	NdrFcShort( 0xc112 ),	/* Flags:  must free, out, simple ref, srv alloc size=48 */
/* 764 */	NdrFcShort( 0x40 ),	/* X64 Stack size/offset = 64 */
/* 766 */	NdrFcShort( 0x128 ),	/* Type Offset=296 */

	/* Procedure CalcMassPropertiesPolyhedron */

/* 768 */	0x32,		/* FC_BIND_PRIMITIVE */
			0x48,		/* Old Flags:  */
/* 770 */	NdrFcLong( 0x0 ),	/* 0 */
/* 774 */	NdrFcShort( 0xc ),	/* 12 */
/* 776 */	NdrFcShort( 0x50 ),	/* X64 Stack size/offset = 80 */
/* 778 */	NdrFcShort( 0x1d ),	/* 29 */
/* 780 */	NdrFcShort( 0xbc ),	/* 188 */
/* 782 */	0x42,		/* Oi2 Flags:  clt must size, has ext, */
			0xa,		/* 10 */
/* 784 */	0xa,		/* 10 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 786 */	NdrFcShort( 0x0 ),	/* 0 */
/* 788 */	NdrFcShort( 0x1 ),	/* 1 */
/* 790 */	NdrFcShort( 0x0 ),	/* 0 */
/* 792 */	NdrFcShort( 0x100 ),	/* 256 */

	/* Parameter nVerts */

/* 794 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 796 */	NdrFcShort( 0x0 ),	/* X64 Stack size/offset = 0 */
/* 798 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter verts */

/* 800 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 802 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 804 */	NdrFcShort( 0xbe ),	/* Type Offset=190 */

	/* Parameter nTris */

/* 806 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 808 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 810 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter tris */

/* 812 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 814 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 816 */	NdrFcShort( 0xd8 ),	/* Type Offset=216 */

	/* Parameter density */

/* 818 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 820 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 822 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter solid */

/* 824 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 826 */	NdrFcShort( 0x28 ),	/* X64 Stack size/offset = 40 */
/* 828 */	0x2,		/* FC_CHAR */
			0x0,		/* 0 */

	/* Parameter mass */

/* 830 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 832 */	NdrFcShort( 0x30 ),	/* X64 Stack size/offset = 48 */
/* 834 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter volume */

/* 836 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 838 */	NdrFcShort( 0x38 ),	/* X64 Stack size/offset = 56 */
/* 840 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter center */

/* 842 */	NdrFcShort( 0x4112 ),	/* Flags:  must free, out, simple ref, srv alloc size=16 */
/* 844 */	NdrFcShort( 0x40 ),	/* X64 Stack size/offset = 64 */
/* 846 */	NdrFcShort( 0x6 ),	/* Type Offset=6 */

	/* Parameter inertia */

/* 848 */	NdrFcShort( 0xc112 ),	/* Flags:  must free, out, simple ref, srv alloc size=48 */
/* 850 */	NdrFcShort( 0x48 ),	/* X64 Stack size/offset = 72 */
/* 852 */	NdrFcShort( 0x128 ),	/* Type Offset=296 */

	/* Procedure CombineMassProperties */

/* 854 */	0x32,		/* FC_BIND_PRIMITIVE */
			0x48,		/* Old Flags:  */
/* 856 */	NdrFcLong( 0x0 ),	/* 0 */
/* 860 */	NdrFcShort( 0xd ),	/* 13 */
/* 862 */	NdrFcShort( 0x50 ),	/* X64 Stack size/offset = 80 */
/* 864 */	NdrFcShort( 0x128 ),	/* 296 */
/* 866 */	NdrFcShort( 0xbc ),	/* 188 */
/* 868 */	0x40,		/* Oi2 Flags:  has ext, */
			0xa,		/* 10 */
/* 870 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 872 */	NdrFcShort( 0x0 ),	/* 0 */
/* 874 */	NdrFcShort( 0x0 ),	/* 0 */
/* 876 */	NdrFcShort( 0x0 ),	/* 0 */
/* 878 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter nItems */

/* 880 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 882 */	NdrFcShort( 0x0 ),	/* X64 Stack size/offset = 0 */
/* 884 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter masses */

/* 886 */	NdrFcShort( 0x148 ),	/* Flags:  in, base type, simple ref, */
/* 888 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 890 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter volumes */

/* 892 */	NdrFcShort( 0x148 ),	/* Flags:  in, base type, simple ref, */
/* 894 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
/* 896 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter centers */

/* 898 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 900 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
/* 902 */	NdrFcShort( 0x6 ),	/* Type Offset=6 */

	/* Parameter inertias */

/* 904 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 906 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
/* 908 */	NdrFcShort( 0x128 ),	/* Type Offset=296 */

	/* Parameter transforms */

/* 910 */	NdrFcShort( 0x10a ),	/* Flags:  must free, in, simple ref, */
/* 912 */	NdrFcShort( 0x28 ),	/* X64 Stack size/offset = 40 */
/* 914 */	NdrFcShort( 0xf2 ),	/* Type Offset=242 */

	/* Parameter mass */

/* 916 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 918 */	NdrFcShort( 0x30 ),	/* X64 Stack size/offset = 48 */
/* 920 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter volume */

/* 922 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 924 */	NdrFcShort( 0x38 ),	/* X64 Stack size/offset = 56 */
/* 926 */	0xa,		/* FC_FLOAT */
			0x0,		/* 0 */

	/* Parameter center */

/* 928 */	NdrFcShort( 0x4112 ),	/* Flags:  must free, out, simple ref, srv alloc size=16 */
/* 930 */	NdrFcShort( 0x40 ),	/* X64 Stack size/offset = 64 */
/* 932 */	NdrFcShort( 0x6 ),	/* Type Offset=6 */

	/* Parameter inertia */

/* 934 */	NdrFcShort( 0xc112 ),	/* Flags:  must free, out, simple ref, srv alloc size=48 */
/* 936 */	NdrFcShort( 0x48 ),	/* X64 Stack size/offset = 72 */
/* 938 */	NdrFcShort( 0x128 ),	/* Type Offset=296 */

	/* Procedure HavokInit */

/* 940 */	0x32,		/* FC_BIND_PRIMITIVE */
			0x48,		/* Old Flags:  */
/* 942 */	NdrFcLong( 0x0 ),	/* 0 */
/* 946 */	NdrFcShort( 0xe ),	/* 14 */
/* 948 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
/* 950 */	NdrFcShort( 0x0 ),	/* 0 */
/* 952 */	NdrFcShort( 0x5 ),	/* 5 */
/* 954 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x1,		/* 1 */
/* 956 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 958 */	NdrFcShort( 0x0 ),	/* 0 */
/* 960 */	NdrFcShort( 0x0 ),	/* 0 */
/* 962 */	NdrFcShort( 0x0 ),	/* 0 */
/* 964 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Return value */

/* 966 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 968 */	NdrFcShort( 0x0 ),	/* X64 Stack size/offset = 0 */
/* 970 */	0x2,		/* FC_CHAR */
			0x0,		/* 0 */

	/* Procedure HavokCleanup */

/* 972 */	0x32,		/* FC_BIND_PRIMITIVE */
			0x48,		/* Old Flags:  */
/* 974 */	NdrFcLong( 0x0 ),	/* 0 */
/* 978 */	NdrFcShort( 0xf ),	/* 15 */
/* 980 */	NdrFcShort( 0x0 ),	/* X64 Stack size/offset = 0 */
/* 982 */	NdrFcShort( 0x0 ),	/* 0 */
/* 984 */	NdrFcShort( 0x0 ),	/* 0 */
/* 986 */	0x40,		/* Oi2 Flags:  has ext, */
			0x0,		/* 0 */
/* 988 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 990 */	NdrFcShort( 0x0 ),	/* 0 */
/* 992 */	NdrFcShort( 0x0 ),	/* 0 */
/* 994 */	NdrFcShort( 0x0 ),	/* 0 */
/* 996 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Procedure HavokShutdown */

/* 998 */	0x32,		/* FC_BIND_PRIMITIVE */
			0x48,		/* Old Flags:  */
/* 1000 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1004 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1006 */	NdrFcShort( 0x0 ),	/* X64 Stack size/offset = 0 */
/* 1008 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1010 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1012 */	0x40,		/* Oi2 Flags:  has ext, */
			0x0,		/* 0 */
/* 1014 */	0xa,		/* 10 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1016 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1018 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1020 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1022 */	NdrFcShort( 0x0 ),	/* 0 */

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
/* 18 */	NdrFcShort( 0x8 ),	/* X64 Stack size/offset = 8 */
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
/* 50 */	NdrFcShort( 0x18 ),	/* X64 Stack size/offset = 24 */
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
/* 76 */	NdrFcShort( 0x0 ),	/* X64 Stack size/offset = 0 */
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
/* 92 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
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
/* 108 */	NdrFcShort( 0x20 ),	/* X64 Stack size/offset = 32 */
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
/* 134 */	NdrFcShort( 0x30 ),	/* X64 Stack size/offset = 48 */
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
/* 160 */	NdrFcShort( 0x0 ),	/* X64 Stack size/offset = 0 */
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
/* 176 */	NdrFcShort( 0x0 ),	/* X64 Stack size/offset = 0 */
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
/* 196 */	NdrFcShort( 0x0 ),	/* X64 Stack size/offset = 0 */
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
/* 222 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
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
/* 276 */	NdrFcShort( 0x10 ),	/* X64 Stack size/offset = 16 */
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
    62,
    142,
    186,
    230,
    268,
    306,
    392,
    472,
    540,
    608,
    688,
    768,
    854,
    940,
    972,
    998
    };


static const MIDL_STUB_DESC havok_StubDesc = 
    {
    (void *)& havok___RpcClientInterface,
    MIDL_user_allocate,
    MIDL_user_free,
    &havok_IfHandle,
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
#if _MSC_VER >= 1200
#pragma warning(pop)
#endif


#endif /* defined(_M_AMD64)*/

