/****************************************************************************
*
*                            Open Watcom Project
*
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
// IFDUMP    : intrinsic function processing
//

#include "ftnstd.h"
#include "global.h"
#include "fcodes.h"
#include "emitobj.h"
#include "types.h"
#include "utility.h"


static  void    MakeComplex( TYPE if_type ) {
//==========================================

// Common routines for above 2 routines.

    GenType( CITNode );
    CITNode->typ = if_type;
    CITNode->size = TypeSize( if_type );
    AdvanceITPtr();
}


static  void    G2Math( FCODE ifn ) {
//===================================

    EmitOp( ifn );
    GenTypes( CITNode, CITNode->link );
    AdvanceITPtr();
}


static  void    GMath( FCODE ifn ) {
//==================================

    EmitOp( ifn );
    GenType( CITNode );
    AdvanceITPtr();
}


static  void    GBitFunction( FCODE fn ) {
//========================================

    EmitOp( fn );
    GenTypes( CITNode, CITNode->link );
    AdvanceITPtr();
}


void    GMakeCplx( void ) {
//===================

// Convert 2 arguments (       a1  ,       a2   )
// into complex number ( real( a1 ), real( a2 ) )

    EmitOp( FC_MAKE_COMPLEX );
    MakeComplex( FT_COMPLEX );
}


void    GMakeDCplx( void ) {
//====================

// Convert 2 arguments (       a1  ,       a2   )
// into double complex number ( double( a1 ), double( a2 ) )

    EmitOp( FC_MAKE_DCOMPLEX );
    MakeComplex( FT_DCOMPLEX );
}


void    GMakeXCplx( void ) {
//====================

// Convert 2 arguments (       a1  ,       a2   )
// into double complex number ( double( a1 ), double( a2 ) )

    EmitOp( FC_MAKE_XCOMPLEX );
    MakeComplex( FT_XCOMPLEX );
}


void    GModulus( void ) {
//==================

    G2Math( FC_MODULUS );
}


void    GSign( void ) {
//===============

    G2Math( FC_SIGN );
}


void    GCharLen( void ) {
//==================

    EmitOp( FC_CHAR_LEN );
    AdvanceITPtr();
}


void    GImag( void ) {
//===============

    GMath( FC_IMAG );
}


void    GConjg( void ) {
//================

    GMath( FC_CONJG );
}


void    GDProd( void ) {
//================

    EmitOp( FC_DPROD );
    AdvanceITPtr();
}


void    GXProd( void ) {
//================

    EmitOp( FC_XPROD );
    AdvanceITPtr();
}


void    GMin( TYPE func_type ) {
//=============================

    EmitOp( FC_MIN );
    GenTypes( CITNode, CITNode->link );
    DumpType( func_type, TypeSize( func_type ) );
    AdvanceITPtr();
}


void    GMax( TYPE func_type ) {
//=============================

    EmitOp( FC_MAX );
    GenTypes( CITNode, CITNode->link );
    DumpType( func_type, TypeSize( func_type ) );
    AdvanceITPtr();
}


void    GBitTest( void ) {
//==================

    GBitFunction( FC_BIT_TEST );
}


void    GBitSet( void ) {
//=================

    GBitFunction( FC_BIT_SET );
}


void    GBitClear( void ) {
//===================

    GBitFunction( FC_BIT_CLEAR );
}


void    GBitOr( void ) {
//================

    GBitFunction( FC_BIT_OR );
}


void    GBitAnd( void ) {
//=================

    GBitFunction( FC_BIT_AND );
}


void    GBitNot( void ) {
//=================

    EmitOp( FC_BIT_NOT );
    GenType( CITNode );
    AdvanceITPtr();
}


void    GBitExclOr( void ) {
//====================

    GBitFunction( FC_BIT_EXCL_OR );
}


void    GBitChange( void ) {
//====================

    GBitFunction( FC_BIT_CHANGE );
}


void    GBitLShift( void ) {
//====================

    GBitFunction( FC_LEFT_SHIFT );
}


void    GBitRShift( void ) {
//====================

    GBitFunction( FC_RIGHT_SHIFT );
}


void            GMod( void ) {
//======================

    GMath( FC_MATH_MOD );
}


void            GAbs( void ) {
//======================

    GMath( FC_MATH_ABS );
}


void            GASin( void ) {
//=======================

    GMath( FC_MATH_ASIN );
}


void            GACos( void ) {
//=======================

    GMath( FC_MATH_ACOS );
}


void            GATan( void ) {
//=======================

    GMath( FC_MATH_ATAN );
}


void            GATan2( void ) {
//========================

    GMath( FC_MATH_ATAN2 );
}


void            GLog( void ) {
//======================

    GMath( FC_MATH_LOG );
}


void            GLog10( void ) {
//========================

    GMath( FC_MATH_LOG10 );
}


void            GCos( void ) {
//======================

    GMath( FC_MATH_COS );
}


void            GSin( void ) {
//======================

    GMath( FC_MATH_SIN );
}


void            GTan( void ) {
//======================

    GMath( FC_MATH_TAN );
}


void            GSinh( void ) {
//=======================

    GMath( FC_MATH_SINH );
}


void            GCosh( void ) {
//=======================

    GMath( FC_MATH_COSH );
}


void            GTanh( void ) {
//=======================

    GMath( FC_MATH_TANH );
}


void            GSqrt( void ) {
//=======================

    GMath( FC_MATH_SQRT );
}


void            GExp( void ) {
//======================

    GMath( FC_MATH_EXP );
}


void    GLoc( void ) {
//==============

    PushOpn( CITNode );
    EmitOp( FC_LOC );
    GenType( CITNode );
    AdvanceITPtr();
}


void    GVolatile( void ) {
//===================

    PushOpn( CITNode );
    EmitOp( FC_VOLATILE );
    AdvanceITPtr();
}
