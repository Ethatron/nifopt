

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


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

#pragma warning( disable: 4049 )  /* more than 64k source lines */


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 475
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__


#ifndef __nifopt2Dhvk_h__
#define __nifopt2Dhvk_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */

#ifdef __cplusplus
extern "C"{
#endif


#ifndef __havok_INTERFACE_DEFINED__
#define __havok_INTERFACE_DEFINED__

/* interface havok */
/* [implicit_handle][version][uuid] */

#if 0
typedef unsigned char bool;

typedef unsigned char hvkByte;

typedef unsigned short hvkShort;

typedef unsigned short hvkTriangle[ 3 ];

typedef float hvkPoint3[ 3 ];

typedef float hvkPoint4[ 4 ];

typedef float hvkMatrix43[ 12 ];

typedef float hvkMatrix44[ 16 ];
#endif

int GenerateMoppCode(
    /* [in] */ hvkByte material,
    /* [in] */ int nVerts,
    /* [size_is][ref][in] */ const hvkPoint3 *verts,
    /* [in] */ int nTris,
    /* [size_is][ref][in] */ const hvkTriangle *tris);

int GenerateMoppCodeWithSubshapes(
    /* [in] */ int nShapes,
    /* [size_is][ref][in] */ const int *shapeVerts,
    /* [in] */ int nMaterials,
    /* [size_is][ref][in] */ const hvkByte *materialIDs,
    /* [in] */ int nVerts,
    /* [size_is][ref][in] */ const hvkPoint3 *verts,
    /* [in] */ int nTris,
    /* [size_is][ref][in] */ const hvkTriangle *tris);

int RetrieveMoppCode(
    /* [in] */ int nBuffer,
    /* [size_is][ref][out] */ hvkByte *buffer);

int RetrieveMoppWelding(
    /* [in] */ int nBuffer,
    /* [size_is][ref][out] */ hvkShort *buffer);

int RetrieveMoppScale(
    /* [out] */ float *value);

int RetrieveMoppOrigin(
    /* [out] */ float *value);

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
    /* [ref][out] */ bool *exclusive);

void CalcHullPolyhedron(
    /* [in] */ int nVerts,
    /* [size_is][ref][in] */ const hvkPoint3 *verts,
    /* [in] */ int nTris,
    /* [size_is][ref][in] */ const hvkTriangle *tris,
    /* [in] */ float factor,
    /* [in] */ float tolerance,
    /* [ref][out][in] */ int *nPlanes,
    /* [size_is][ref][out] */ hvkPoint4 *planes,
    /* [ref][out] */ bool *exclusive);

void CalcMassPropertiesSphere(
    /* [in] */ float radius,
    /* [in] */ float density,
    /* [in] */ bool solid,
    /* [ref][out] */ float *mass,
    /* [ref][out] */ float *volume,
    /* [ref][out] */ hvkPoint3 *center,
    /* [ref][out] */ hvkMatrix43 *inertia);

void CalcMassPropertiesBox(
    /* [ref][in] */ const hvkPoint3 *size,
    /* [in] */ float density,
    /* [in] */ bool solid,
    /* [ref][out] */ float *mass,
    /* [ref][out] */ float *volume,
    /* [ref][out] */ hvkPoint3 *center,
    /* [ref][out] */ hvkMatrix43 *inertia);

void CalcMassPropertiesCylinder(
    /* [ref][in] */ const hvkPoint3 *startAxis,
    /* [ref][in] */ const hvkPoint3 *endAxis,
    /* [in] */ float radius,
    /* [in] */ float density,
    /* [in] */ bool solid,
    /* [ref][out] */ float *mass,
    /* [ref][out] */ float *volume,
    /* [ref][out] */ hvkPoint3 *center,
    /* [ref][out] */ hvkMatrix43 *inertia);

void CalcMassPropertiesCapsule(
    /* [ref][in] */ const hvkPoint3 *startAxis,
    /* [ref][in] */ const hvkPoint3 *endAxis,
    /* [in] */ float radius,
    /* [in] */ float density,
    /* [in] */ bool solid,
    /* [ref][out] */ float *mass,
    /* [ref][out] */ float *volume,
    /* [ref][out] */ hvkPoint3 *center,
    /* [ref][out] */ hvkMatrix43 *inertia);

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
    /* [ref][out] */ hvkMatrix43 *inertia);

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
    /* [ref][out] */ hvkMatrix43 *inertia);

bool HavokInit( void);

void HavokCleanup( void);

void HavokShutdown( void);


extern handle_t havok_IfHandle;


extern RPC_IF_HANDLE havok_v1_0_c_ifspec;
extern RPC_IF_HANDLE havok_v1_0_s_ifspec;
#endif /* __havok_INTERFACE_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


