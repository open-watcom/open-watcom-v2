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
// FCRELOPS  : arithmetic relations F-Code processor
//

#include "ftnstd.h"
#include "symbol.h"
#include "wf77defs.h"
#include "cg.h"
#include "rtconst.h"
#include "emitobj.h"
#include "fctypes.h"
#include "cgswitch.h"
#include "cgprotos.h"


extern  void            XPush(cg_name);
extern  cg_name         XPop(void);
extern  cg_name         XPopValue(cg_type);
extern  call_handle     InitCall(RTCODE);
extern  cg_name         SCBPointer(cg_name);
extern  cg_name         IntegerConstant(ftn_type *,uint);


static  void    XCompare( cg_op op_code ) {
//=========================================

// Arithmetic comparision.

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
    XPush( CGCompare( op_code, op1, op2, ResCGType( typ1, typ2 ) ) );
}


void    FCCmpEQ( void ) {
//=================

// Arithmetic '==' ( equals ) F-Code processor

    XCompare( O_EQ );
}


void    FCCmpLE( void ) {
//=================

// Arithmetic '<=' ( less than or equal ) F-Code processor

    XCompare( O_LE );
}


void    FCCmpLT( void ) {
//=================

// Arithmetic '<' ( less than ) F-Code processor

    XCompare( O_LT );
}


void    FCCmpNE( void ) {
//=================

// Arithmetic '!=' ( not equal to ) F-Code processor

    XCompare( O_NE );
}


void    FCCmpGT( void ) {
//=================

// Arithmetic '>' ( greater than ) F-Code processor

    XCompare( O_GT );
}


void    FCCmpGE( void ) {
//=================

// Arithmetic '>=' ( greater than or equal to ) F-Code processor

    XCompare( O_GE );
}

cg_name GetChOp( cg_type ch_type ) {
//==================================

// Get character operand.

    sym_id      lit;

    lit = GetPtr();
    if( lit != NULL ) {
        if( lit->lt.flags & (LT_SCB_REQUIRED | LT_SCB_TMP_REFERENCE) ) {
            CGTrash( XPop() );
        }
        return( IntegerConstant( (ftn_type *)(&lit->lt.value), lit->lt.length));
    } else {
        return( CGUnary( O_POINTS, SCBPointer( XPop() ), ch_type ) );
    }
}


static  void    XChar1Compare( cg_op op ) {
//=========================================

// Compare single characters.

    cg_name     op_1;
    cg_name     op_2;
    cg_type     ch_type;

    ch_type = GetType( GetU16() );
    op_1 = GetChOp( ch_type );
    op_2 = GetChOp( ch_type );
    XPush( CGCompare( op, op_1, op_2, ch_type ) );
}

static  void    XCharCompare( cg_op op ) {
//========================================

// Character comparision.

    call_handle call;
    cg_name     name1;
    cg_name     name2;

    name1 = XPop();
    name2 = XPop();
    call = InitCall( RT_LEXCMP );
    CGAddParm( call, name2, TY_LOCAL_POINTER );
    CGAddParm( call, name1, TY_LOCAL_POINTER );
    XPush( CGCompare( op, CGUnary( O_POINTS, CGCall( call ), TY_INTEGER ),
                      CGInteger( 0, TY_INTEGER ), TY_INTEGER ) );
}


void    FCCharCmpEQ( void ) {
//=====================

// Character '==' ( equals ) F-Code processor.

    XCharCompare( O_EQ );
}


void    FCCharCmpLE( void ) {
//=====================

// Character '<=' ( less than or equal ) F-Code processor

    XCharCompare( O_LE );
}


void    FCCharCmpLT( void ) {
//=====================

// Character '<' ( less than ) F-Code processor

    XCharCompare( O_LT );
}


void    FCCharCmpNE( void ) {
//=====================

// Character '!=' ( not equal to ) F-Code processor

    XCharCompare( O_NE );
}


void    FCCharCmpGT( void ) {
//=====================

// Character '>' ( greater than ) F-Code processor

    XCharCompare( O_GT );
}


void    FCCharCmpGE( void ) {
//=====================

// Character '>=' ( greater than or equal to ) F-Code processor

    XCharCompare( O_GE );
}


void    FCChar1CmpEQ( void ) {
//======================

// Compare single characters.

    XChar1Compare( O_EQ );
}


void    FCChar1CmpLE( void ) {
//======================

// Compare single characters.

    XChar1Compare( O_LE );
}


void    FCChar1CmpLT( void ) {
//======================

// Compare single characters.

    XChar1Compare( O_LT );
}


void    FCChar1CmpNE( void ) {
//======================

// Compare single characters.

    XChar1Compare( O_NE );
}


void    FCChar1CmpGE( void ) {
//======================

// Compare single characters.

    XChar1Compare( O_GE );
}


void    FCChar1CmpGT( void ) {
//======================

// Compare single characters.

    XChar1Compare( O_GT );
}


