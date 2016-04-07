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
// FCIFNS    : in-line intrinsic functions
//

#include "ftnstd.h"
#include "wf77defs.h"
#include "cg.h"
#include "tmpdefs.h"
#include "emitobj.h"
#include "fctypes.h"
#include "fcstring.h"
#include "fctemp.h"
#include "fcdata.h"
#include "fcjmptab.h"
#include "fcstack.h"
#include "cgswitch.h"
#include "cgprotos.h"


/* Forward declarations */
static  void    MinMax( cg_op cmp );
static  void    GBinaryMath( uint ifn );
static  void    GUnaryMath( uint ifn );



void    FCCharLen( void ) {
//===================

    XPush( SCBLength( XPop() ) );
}


void    FCMax( void ) {
//===============

    MinMax( O_GT );
}


void    FCMin( void ) {
//===============

    MinMax( O_LT );
}


static  void    MinMax( cg_op cmp ) {
//===================================

    unsigned_16 typ_info;
    cg_type     typ1;
    cg_type     typ2;
    cg_type     func_typ;
    cg_name     op1_1;
    cg_name     op1_2;
    cg_name     op2_1;
    cg_name     op2_2;

    typ_info = GetU16();
    typ1 = GetType1( typ_info );
    typ2 = GetType2( typ_info );
    func_typ = GetType( GetU16() );
    CloneCGName( XPopValue( typ1 ), &op1_1, &op1_2 );
    CloneCGName( XPopValue( typ2 ), &op2_1, &op2_2 );
    XPush( CGChoose( CGCompare( cmp, op1_1, op2_1, ResCGType( typ1, typ2 ) ),
                     op1_2, op2_2, func_typ ) );
}


void    FCDProd( void ) {
//=================

    cg_name     op1;
    cg_name     op2;

    op1 = CGUnary( O_CONVERT, XPopValue( TY_SINGLE ), TY_DOUBLE );
    op2 = CGUnary( O_CONVERT, XPopValue( TY_SINGLE ), TY_DOUBLE );
    XPush( CGBinary( O_TIMES, op1, op2, TY_DOUBLE ) );
}

void    FCXProd( void ) {
//=================

    cg_name     op1;
    cg_name     op2;

    op1 = CGUnary( O_CONVERT, XPopValue( TY_DOUBLE ), TY_LONGDOUBLE );
    op2 = CGUnary( O_CONVERT, XPopValue( TY_DOUBLE ), TY_LONGDOUBLE );
    XPush( CGBinary( O_TIMES, op1, op2, TY_LONGDOUBLE ) );
}



void            FCMod( void ) {
//=======================

    GBinaryMath( O_FMOD );
}


void            FCAbs( void ) {
//=======================

    unsigned_16 typ_info;
    cg_type     typ;
    cg_name     op;
    cg_name     op_1;
    cg_name     op_2;

    typ_info = GetU16();
    typ = GetType( typ_info );
    op = XPopValue( typ );
    if( IntType( typ_info ) ) {
        CloneCGName( op, &op, &op_1 );
        CloneCGName( op, &op, &op_2 );
        XPush( CGChoose( CGCompare( O_LT, op, CGInteger( 0, typ ), typ ),
                         CGUnary( O_UMINUS, op_1, typ ),
                         op_2, typ ) );
    } else {
        XPush( CGUnary( O_FABS, op, typ ) );
    }
}


void            FCASin( void ) {
//========================

    GUnaryMath( O_ASIN );
}


void            FCACos( void ) {
//========================

    GUnaryMath( O_ACOS );
}


void            FCATan( void ) {
//========================

    GUnaryMath( O_ATAN );
}


void            FCATan2( void ) {
//=========================

    GBinaryMath( O_ATAN2 );
}


void            FCLog( void ) {
//=======================

    GUnaryMath( O_LOG );
}


void            FCLog10( void ) {
//=========================

    GUnaryMath( O_LOG10 );
}


void            FCCos( void ) {
//=======================

    GUnaryMath( O_COS );
}


void            FCSin( void ) {
//=======================

    GUnaryMath( O_SIN );
}


void            FCTan( void ) {
//=======================

    GUnaryMath( O_TAN );
}


void            FCSinh( void ) {
//========================

    GUnaryMath( O_SINH );
}


void            FCCosh( void ) {
//========================

    GUnaryMath( O_COSH );
}


void            FCTanh( void ) {
//========================

    GUnaryMath( O_TANH );
}


void            FCSqrt( void ) {
//========================

    GUnaryMath( O_SQRT );
}


void            FCExp( void ) {
//=======================

    GUnaryMath( O_EXP );
}


static  void    GUnaryMath( uint ifn ) {
//======================================

    cg_type     typ;

    typ = GetType( GetU16() );
    XPush( CGUnary( ifn, XPopValue( typ ), typ ) );
}


static  void    GBinaryMath( uint ifn ) {
//=======================================

    cg_type     typ;
    cg_name     op1;

    typ = GetType( GetU16() );
    op1 = XPopValue( typ );
    XPush( CGBinary( ifn, op1, XPopValue( typ ), typ ) );
}


void            FCLoc( void ) {
//=======================

    PTYPE       typ_info;
    cg_type     cg_typ;
    cg_name     arg;

    typ_info = GetU16();
    arg = XPop();
    if( typ_info == PT_CHAR ) {
        arg = SCBPointer( arg );
    } else {
        cg_typ = CGType( arg );
        if( !TypePointer( cg_typ ) ) {
            arg = TmpPtr( MkTmp( arg, cg_typ ), cg_typ );
        }
    }
#if _CPU == 8086
    arg = CGUnary( O_CONVERT, arg, TY_LONG_POINTER );
#endif
    XPush( CGUnary( O_CONVERT, arg, TY_UINT_4 ) );
}


void            FCVolatile( void ) {
//============================

    cg_name     arg;

    arg = XPop();
    if( TypePointer( CGType( arg ) ) ) {
        arg = CGVolatile( arg );
    }
    XPush( arg );
}
