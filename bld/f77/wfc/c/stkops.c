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
// STKOPS    : stack operation code generation routines
//

#include "ftnstd.h"
#include "global.h"
#include "fcgbls.h"
#include "fcodes.h"
#include "opn.h"
#include "emitobj.h"
#include "types.h"
#include "rstconst.h"


void    PushOpn( itnode *itptr ) {
//================================

// Generate a push of an operand.
// Also called for target of character assignment.

    unsigned_16 flags;
    TYPE        typ;
    USOPN       what;
    USOPN       where;

    where = itptr->opn.us & USOPN_WHERE;
    if( ( itptr->opn.ds != DSOPN_PHI ) && ( where != USOPN_SAFE ) ) {
        typ = itptr->typ;
        flags = itptr->flags;
        what = itptr->opn.us & USOPN_WHAT;
        if( where != 0 ) {
            EmitOp( FC_PUSH );
            SymRef( itptr );
        } else if( itptr->opn.us & USOPN_FLD ) {
            PushConst( itptr->value.intstar4 );
        } else if( ( flags & SY_CLASS ) == SY_SUBPROGRAM ) {
            // 1. it's a statement function
            // 2. it's a subprogram passed as an argument
            EmitOp( FC_PUSH );
            SymRef( itptr );
        } else if( what == USOPN_CON ) {
            if( typ == FT_CHAR ) {
                EmitOp( FC_PUSH_LIT );
                if( itptr->sym_ptr->u.lt.flags & LT_SCB_TMP_REFERENCE ) {
                    itptr->sym_ptr->u.lt.flags |= LT_SCB_REQUIRED;
                } else {
                    // in case string optimizations use value directly,
                    // LT_SCB_TMP_REFERENCE will be turned off
                    itptr->sym_ptr->u.lt.flags |= LT_SCB_TMP_REFERENCE;
                }
            } else {
                EmitOp( FC_PUSH_CONST );
            }
            SymRef( itptr );
        } else {
            EmitOp( FC_PUSH );
            SymRef( itptr );
        }
        SetOpn( itptr, USOPN_SAFE );
    }
}



void    PushSym( sym_id sym ) {
//=============================

    EmitOp( FC_PUSH );
    OutPtr( sym );
}


void        SymRef( itnode *itptr ) {
//=======================================

// Generate symbol table address for given symbol.

    OutPtr( itptr->sym_ptr );
}


void    PushConst( intstar4 val ) {
//=================================

// Push an integer constant.

    EmitOp( FC_PUSH_CONST );
    OutPtr( STConst( &val, FT_INTEGER, TypeSize( FT_INTEGER ) ) );
}


void    GParenExpr( void ) {
//====================

    EmitOp( FC_DONE_PAREN_EXPR );
    GenType( CITNode );
}
