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
// FCBINARY  : binary operator F-Code processor
//

#include "ftnstd.h"
#include "cgdefs.h"
#include "wf77cg.h"
#include "global.h"
#include "cpopt.h"
#include "types.h"
#include "tmpdefs.h"
#include "emitobj.h"
#include "fctypes.h"
#include "fcjmptab.h"
#include "fctemp.h"
#include "fccmplx.h"
#include "fcstack.h"
#include "cgswitch.h"
#include "cgprotos.h"



static  void    XBinary( int op_code ) {
//======================================

// Binary operator F-Code processor.

    cg_name     op1;
    cg_name     op2;
    unsigned_16 typ_info;
    cg_type     typ1;
    cg_type     typ2;

    typ_info = GetU16();
    typ1 = GetType1( typ_info );
    typ2 = GetType2( typ_info );
    op1 = XPopValue( typ1 );
    op2 = XPopValue( typ2 );
    XPush( CGBinary( op_code, op1, op2, ResCGType( typ1, typ2 ) ) );
}


void            FCFlip( void ) {
//========================

// Flip cg-names.

    cg_name     op1;
    cg_name     op2;

    op1 = XPop();
    op2 = XPop();
    XPush( op1 );
    XPush( op2 );
}


void    FCAdd( void ) {
//===============

// Binary addition (+) F-Code processor.

    XBinary( O_PLUS );
}


void    FCSub( void ) {
//===============

// Binary subtraction (-) F-Code processor.

    XBinary( O_MINUS );
}


void    FCMul( void ) {
//===============

// Binary multiplication (*) F-Code processor.

    XBinary( O_TIMES );
}


void    FCDiv( void ) {
//===============

// Binary divide (/) F-Code processor.

    XBinary( O_DIV );
}


void    FCModulus( void ) {
//===================

// Binary mod F-Code processor.

    XBinary( O_MOD );
}


void    FCSign( void ) {
//================

// SIGN F-Code processor.

    unsigned_16 typ_info;
    cg_type     typ1;
    cg_type     typ2;
    cg_name     op;
    cg_name     op1;
    cg_name     op2;

    typ_info = GetU16();
    typ1 = GetType1( typ_info );
    typ2 = GetType2( typ_info );
    op = XPopValue( typ1 );

    if( TypeCGInteger( typ1 ) ) {
        CloneCGName( op, &op, &op1 );
        CloneCGName( op, &op, &op2 );
        op1 = CGChoose( CGCompare( O_LT, op, CGInteger( 0, typ1 ), typ1 ),
                         CGUnary( O_UMINUS, op1, typ1 ),
                         op2, typ1 );
        op = CGCompare( O_LT, XPopValue( typ2 ), CGInteger( 0, typ2 ), typ2 );
    } else {
        op1 = CGUnary( O_FABS, op, typ1 );
        op = CGCompare( O_LT, XPopValue(typ2), CGFloat( "0.0", typ2 ), typ2 );
    }
    CloneCGName( op1, &op1, &op2 );
    XPush( CGChoose( op, CGUnary( O_UMINUS, op1, typ1 ), op2, typ1 ) );
}


void    FCUnaryMul( void ) {
//====================

// Unary multiplication (x * x );

    cg_type     typ;
    cg_name     base;
    cg_name     base_2;
    cg_name     result;
    int         power;

    typ = GetType( GetU16() );
    power = GetU16();
    base = XPopValue( typ );
    result = CGInteger( 1, typ );
    while( power > 0 ) {
        CloneCGName( base, &base, &base_2 );
        if( power & 1 ) {
            result = CGBinary( O_TIMES, result, base_2, typ );
            --power;
        } else {
            base = CGBinary( O_TIMES, base, base_2, typ );
            power >>= 1;
        }
    }
    CGTrash( base );
    XPush( result );
}


void    FCUMinus( void ) {
//==================

// Unary minus (-) F-Code processor.

    cg_type     typ;

    typ = GetType( GetU16() );
    XPush( CGUnary( O_UMINUS, XPopValue( typ ), typ ) );
}


void    FCPow( void ) {
//===============

// Exponentiation.

    cg_type     power_typ;
    cg_type     base_typ;
    cg_name     base;
    cg_name     power;
    unsigned_16 typ_info;

    typ_info = GetU16();
    base_typ = GetType1( typ_info );
    power_typ = GetType2( typ_info );
    base = XPopValue( base_typ );
    power = XPopValue( power_typ );
    base_typ = ResCGType( base_typ, power_typ );
    if( ( base_typ == TY_INT_1 ) || ( base_typ == TY_INT_2 ) ) {
        base_typ = TY_INT_4;
    }
    XPush( CGBinary( O_POW, base, power, base_typ ) );
}


void    FCDoneParenExpr( void ) {
//=========================

    PTYPE       typ_info;
    cg_name     val;
    cg_cmplx    z;

    typ_info = GetU16();
    if( (typ_info == PT_CPLX_8)
      || (typ_info == PT_CPLX_16)
      || (typ_info == PT_CPLX_32) ) {
        XPopCmplx( &z, GetType( typ_info ) );
        val = z.imagpart;
        XPush( CGUnary( O_PARENTHESIS, z.imagpart, CGType( val ) ) );
        val = z.realpart;
    } else if( typ_info == PT_CHAR ) {
        val = XPop();
    } else {
        val = XPopValue( GetType( typ_info ) );
    }
    XPush( CGUnary( O_PARENTHESIS, val, CGType( val ) ) );
}
