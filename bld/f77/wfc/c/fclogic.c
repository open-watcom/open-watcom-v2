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
// FCLOGIC   : logical operator F-Code processor
//

#include "ftnstd.h"
#include "cgdefs.h"
#include "cg.h"
#include "emitobj.h"
#include "fctypes.h"
#include "fcjmptab.h"
#include "fcstack.h"
#include "cgswitch.h"
#include "cgprotos.h"


static  void    Equivalent( cg_op op_code ) {
//===========================================


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
    typ1 = CGType( op1 );
    if( typ1 != TY_BOOLEAN ) {
        op1 = CGCompare( O_NE, op1, CGInteger( 0, typ1 ), typ1 );
    }
    typ2 = CGType( op2 );
    if( typ2 != TY_BOOLEAN ) {
        op2 = CGCompare( O_NE, op2, CGInteger( 0, typ2 ), typ2 );
    }
    XPush( CGCompare( op_code, op1, op2, TY_UINT_1 ) );
}


static  void    XLogic( int op_code ) {
//=====================================

// Logical operator F-Code processor.

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
    XPush( CGFlow( op_code, op1, op2 ) );
}


void    FCOr( void ) {
//==============

// Logical .OR. F-Code processor.

    XLogic( O_FLOW_OR );
}


void    FCAnd( void ) {
//===============

// Logical .AND. F-Code processor.

    XLogic( O_FLOW_AND );
}


void    FCNot( void ) {
//===============

// Logical .NOT. F-Code processor.

    XPush( CGFlow( O_FLOW_NOT, GetTypedValue(), NULL ) );
}

void    FCEqv( void ) {
//===============

// Logical .EQV. F-Code processor.

    Equivalent( O_EQ );
}


void    FCNEqv( void ) {
//================

// Logical .NEQV. F-Code processor.

    Equivalent( O_NE );
}


