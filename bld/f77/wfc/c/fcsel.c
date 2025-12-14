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
// FCSEL  : Select statement F-Code processor
//

#include "ftnstd.h"
#include "i64.h"
#include "symbol.h"
#include "cgdefs.h"
#include "cg.h"
#include "fcodes.h"
#include "emitobj.h"
#include "fcflow.h"
#include "fcjmptab.h"
#include "fcsel.h"
#include "fcstring.h"
#include "fcstack.h"
#include "cgswitch.h"
#include "cgprotos.h"


void    DoSelect( FCODE kind ) {
//==============================

// Select processing for SELECT and computed GOTO.

    sel_handle          s;
    int                 cases;
    int                 stmts;
    label_handle        label;
    sym_id              sel_sym;
    cg_name             sel_expr;
    obj_ptr             curr_obj;
    signed_64           lo;
    signed_64           hi;

    U64High( lo ) = 0;
    U64High( hi ) = 0;
    s = CGSelInit();
    cases = GetU16();
    stmts = cases;
    CGSelOther( s, GetLabel( GetU16() ) );
    curr_obj = FCodeTell( 0 );
    while( cases-- > 0 ) {
        if( kind == FC_COMPUTED_GOTO ) {
            label = GetStmtLabel( GetPtr() );
        } else {
            label = GetLabel( GetU16() );
        }
        U64Low( hi ) = GetU32();
        U64Low( lo ) = GetU32();
        CGSelRange( s, lo, hi, label );
    }
    sel_sym = GetPtr();
    if( sel_sym->u.ns.u1.s.typ == FT_CHAR ) {
        sel_expr = CGUnary( O_POINTS,
                            SCBPointer( SymAddr( sel_sym ) ),
                            TY_UINT_1 );
    } else {
        sel_expr = SymValue( sel_sym );
    }
    CGSelect( s, sel_expr );
    if( kind == FC_COMPUTED_GOTO ) {
        FCodeSeek( curr_obj );
        for( ; stmts != 0; stmts-- ) {
            RefStmtLabel( GetPtr() );
            GetU32();
            GetU32();
        }
        GetPtr(); // skip select variable
    }
}


void    FCSelect( void ) {
//========================

// Perform SELECT statement.

    DoSelect( FC_SELECT );
}
