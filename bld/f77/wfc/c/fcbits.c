/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2025      The Open Watcom Contributors. All Rights Reserved.
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
// FCBITS    : bitwise operations
//

#include "ftnstd.h"
#include "emitobj.h"
#include "fcjmptab.h"
#include "cgdefs.h"
#include "cg.h"
#include "fctypes.h"
#include "fcstack.h"
#include "cgswitch.h"
#include "cgprotos.h"


static  cg_name BitPosition( unsigned_16 typ_info ) {
//===================================================

    cg_type     cgtyp;

    cgtyp = GetCGTypes2( typ_info );
    return( CGBinary( O_LSHIFT, CGInteger( 1, cgtyp ), XPopValue( cgtyp ), cgtyp ) );
}


static  void    BitOperation( cg_op bit_op ) {
//============================================

    cg_name     op;
    cg_type     cgtyp;
    unsigned_16 typ_info;

    typ_info = GetU16();
    cgtyp = GetCGTypes1( typ_info );
    op = XPopValue( cgtyp );
    XPush( CGBinary( bit_op, op, BitPosition( typ_info ), cgtyp ) );
}


void    FCBitTest( void ) {
//===================

    cg_name     op;
    cg_type     cgtyp;
    unsigned_16 typ_info;

    typ_info = GetU16();
    cgtyp = GetCGTypes1( typ_info );
    op = XPopValue( cgtyp );
    XPush( CGCompare( O_NE, CGBinary( O_AND, op, BitPosition( typ_info ), cgtyp ),
                            CGInteger( 0, TY_INTEGER ), cgtyp ) );
}


void    FCBitSet( void ) {
//==================

    BitOperation( O_OR );
}


void    FCBitChange( void ) {
//=====================

    BitOperation( O_XOR );
}


void    FCBitClear( void ) {
//====================

    cg_name     op;
    cg_type     cgtyp;
    unsigned_16 typ_info;

    typ_info = GetU16();
    cgtyp = GetCGTypes1( typ_info );
    op = XPopValue( cgtyp );
    XPush( CGBinary( O_AND, op,
                     CGUnary( O_COMPLEMENT, BitPosition( typ_info ), cgtyp ),
                     cgtyp ) );
}


void    FCBitNot( void ) {
//==================

    cg_type     cgtyp;

    cgtyp = GetCGType( GetU16() );
    XPush( CGUnary( O_COMPLEMENT, XPopValue( cgtyp ), cgtyp ) );
}


static  cg_type BitWise( cg_op op ) {
//===================================

    unsigned_16 typ_info;
    cg_type     cgtyp1;
    cg_type     cgtyp2;
    cg_name     op1;

    typ_info = GetU16();
    cgtyp1 = GetCGTypes1( typ_info );
    cgtyp2 = GetCGTypes2( typ_info );
    op1 = XPopValue( cgtyp1 );
    cgtyp2 = ResCGType( cgtyp1, cgtyp2 );
    XPush( CGBinary( op, op1, XPopValue( cgtyp2 ), cgtyp2 ) );
    return( cgtyp2 );
}


void    FCBitOr( void ) {
//=================

    BitWise( O_OR );
}


void    FCBitAnd( void ) {
//==================

    BitWise( O_AND );
}


void    FCBitExclOr( void ) {
//=====================

    BitWise( O_XOR );
}


void    FCBitEquiv( void ) {
//====================

    cg_type     cgtyp;

    cgtyp = BitWise( O_XOR );
    XPush( CGUnary( O_COMPLEMENT, XPop(), cgtyp ) );
}


void FCBitLShift( void ) {
//==================

    BitWise( O_LSHIFT );
}


void FCBitRShift( void ) {
//==================

    BitWise( O_RSHIFT );
}
