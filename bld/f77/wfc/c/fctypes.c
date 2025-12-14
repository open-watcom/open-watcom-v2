/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2025 The Open Watcom Contributors. All Rights Reserved.
*    Portions Copyright (c) 1983-2002 Sybase, Inc. All Rights Reserved.
*
*  ========================================================================
*
*    This file contains Original Code and/or Modifications of Original
*    Code as defined in and that are subject to the Sybase Open Watcom
*    Public License version 1.0 (the 'License'). You may not use this file
*    except in compliance with the License. BY USING THIS FILE YOU AGREE TO
*    ALL TERMS AND CONDITIONS OF THE LICENSE. A copy of the License is
*    provided with the Original Code and Modifications, and is also
*    available at www.sybase.com/developer/opensource.
*
*    The Original Code and all software distributed under the License are
*    distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
*    EXPRESS OR IMPLIED, AND SYBASE AND ALL CONTRIBUTORS HEREBY DISCLAIM
*    ALL SUCH WARRANTIES, INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF
*    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR
*    NON-INFRINGEMENT. Please see the License for the specific language
*    governing rights and limitations under the License.
*
*  ========================================================================
*
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


//
// FCTYPES :    Get typing information
//

#include "ftnstd.h"
#include "symtypes.h"
#include "types.h"
#include "symbol.h"
#include "emitobj.h"
#include "fctypes.h"


#define NUM_TYPES       12
#define L1              TY_UINT_1
#define L4              TY_UINT_4
#define I1              TY_INT_1
#define I2              TY_INT_2
#define I4              TY_INT_4
#define R4              TY_SINGLE
#define R8              TY_DOUBLE
#define R10             TY_LONGDOUBLE
#define C8              TY_COMPLEX
#define C16             TY_DCOMPLEX
#define C20             TY_XCOMPLEX
#define CH              TY_CHAR

static  byte            MapCGTypes[] = {

// L1   L4  I1   I2   I4   R4   R8   R10  C8   C16  C20  CH

   L1,  L4, 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   // L1
   L4,  L4, 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   // L4
   0,   0,  I1,  I2,  I4,  R4,  R8,  R10, C8,  C16, C20, 0,   // I1
   0,   0,  I2,  I2,  I4,  R4,  R8,  R10, C8,  C16, C20, 0,   // I2
   0,   0,  I4,  I4,  I4,  R4,  R8,  R10, C8,  C16, C20, 0,   // I4
   0,   0,  R4,  R4,  R4,  R4,  R8,  R10, C8,  C16, C20, 0,   // R4
   0,   0,  R8,  R8,  R8,  R8,  R8,  R10, C16, C16, C20, 0,   // R8
   0,   0,  R10, R10, R10, R10, R10, R10, C20, C20, C20, 0,   // R10
   0,   0,  C8,  C8,  C8,  C8,  C16, C20, C8,  C16, C20, 0,   // C8
   0,   0,  C16, C16, C16, C16, C16, C20, C16, C16, C20, 0,   // C16
   0,   0,  C20, C20, C20, C20, C20, C20, C20, C20, C20, 0,   // C20
   0,   0,  0,   0,   0,   0,   0,   0,   0,   0,   0,   CH   // CH
};

static  cg_type         CGTypesMap[] = {
    #define pick(id,type,dbgtype,cgtype,inpfun,outfun,typnam) cgtype,
    #include "ptypdefn.h"
    #undef pick
};

cg_type MkCGType( PTYPE ptyp )
//===========================
// Map FORTRAN parameter type to CG-type.
{
    return( CGTypesMap[ptyp] );
}


cg_type GetCGType( unsigned_16 typ_info )
//=======================================
{
    return( MkCGType( _GetArgInfoPtyp( typ_info ) ) );
}


cg_type GetCGTypes1( unsigned_16 typ_info )
//=========================================
{
    return( MkCGType( _GetArgInfoPtypes1( typ_info ) ) );
}


cg_type GetCGTypes2( unsigned_16 typ_info )
//=========================================
{
    return( MkCGType( _GetArgInfoPtypes2( typ_info ) ) );
}


cg_type         F77ToCGType( sym_id sym )
//========================================
// Map a WATFOR-77 type to a CG type.
{
    if( sym->u.ns.u1.s.typ == FT_STRUCTURE )
        return( sym->u.ns.xt.record->cgtyp );
    return( MkCGType( ParmType( sym->u.ns.u1.s.typ, sym->u.ns.xt.size ) ) );
}


static int  CGIndex( cg_type cgtyp )
//==================================
// Return index for a CG-type.
{
    if( cgtyp == TY_UINT_1 )
        return( 0 );
    if( cgtyp == TY_UINT_2 )
        return( 1 );
    if( cgtyp == TY_INT_1 )
        return( 2 );
    if( cgtyp == TY_INT_2 )
        return( 3 );
    if( cgtyp == TY_INT_4 )
        return( 4 );
    if( cgtyp == TY_SINGLE )
        return( 5 );
    if( cgtyp == TY_DOUBLE )
        return( 6 );
    if( cgtyp == TY_LONGDOUBLE )
        return( 7 );
    if( cgtyp == TY_COMPLEX )
        return( 8 );
    if( cgtyp == TY_DCOMPLEX )
        return( 9 );
    if( cgtyp == TY_XCOMPLEX )
        return( 10 );
    return( 11 ); // cgtyp == TY_CHAR
}


cg_type ResCGType( cg_type cgtyp1, cg_type cgtyp2 )
//=================================================
// For a binary operation of two operands of the given CG-types, return
// the result CG-type.
{
    return( MapCGTypes[CGIndex( cgtyp1 ) * NUM_TYPES + CGIndex( cgtyp2 )] );
}


bool    IsCGCodePointer( cg_type cgtyp )
//======================================
// Is CG-type a code pointer?
{
    switch( cgtyp ) {
    case TY_CODE_PTR:
    case TY_LONG_CODE_PTR:
    case TY_NEAR_CODE_PTR:
        return( true );
    }
    return( false );
}


bool    IsCGInteger( cg_type cgtyp )
//==================================
// Is CG-type an integer?
{
    switch( cgtyp ) {
    case TY_UINT_1:
    case TY_INT_1:
    case TY_UINT_2:
    case TY_INT_2:
    case TY_UINT_4:
    case TY_INT_4:
    case TY_UINT_8:
    case TY_INT_8:
    case TY_INTEGER:
        return( true );
    }
    return( false );
}


bool    IsCGPointer( cg_type cgtyp )
//==================================
// Is CG-type a pointer?
{
    switch( cgtyp ) {
    default:
        if( cgtyp < TY_USER_DEFINED )
            break;
        /* fall through */
    case TY_NEAR_POINTER:
    case TY_LONG_POINTER:
    case TY_HUGE_POINTER:
    case TY_LOCAL_POINTER:
    case TY_COMPLEX:
    case TY_DCOMPLEX:
    case TY_XCOMPLEX:
    case TY_CHAR:
    case TY_CODE_PTR:
    case TY_LONG_CODE_PTR:
    case TY_NEAR_CODE_PTR:
        return( true );
    }
    return( false );
}


cg_type PromoteCGToBaseType( cg_type cgtyp )
//==========================================
// if type is integer TY_INT_1, TY_INT_2 under the _AXP or _PPC,
// we must promote it in order to make a call
{
#if _RISC_CPU
    if( ( cgtyp == TY_INT_1 )
      || ( cgtyp == TY_INT_2 )  ) {
        cgtyp = TY_INT_4;
    }
#endif
    return( cgtyp );
}
