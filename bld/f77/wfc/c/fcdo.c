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
// FCDO     : DO loop F-Code processor
//

#include "ftnstd.h"
#include "wf77defs.h"
#include "cg.h"
#include "tmpdefs.h"
#include "symbol.h"
#include "emitobj.h"
#include "fctypes.h"
#include "cgswitch.h"
#include "cgprotos.h"


extern  cg_name         GetTypedValue(void);
extern  cg_name         SymValue(sym_id);
extern  cg_name         SymAddr(sym_id);
extern  label_handle    GetLabel(label_id);


void    FCDoBegin( void ) {
//===================

// Initialize DO loop processing.

    cg_name             e1;
    cg_name             e2;
    cg_name             e3;
    sym_id              do_var;
    sym_id              increment;
    sym_id              loop_ctrl;
    label_handle        top;
    label_handle        bottom;
    cg_type             do_type;
    intstar4            incr_value;
    int                 cmp_op;
    cg_name             tmp;

    do_var = GetPtr();
    increment = GetPtr();
    if( increment == NULL ) {
        incr_value = GetConst32();
        loop_ctrl = GetPtr();
    } else {
        loop_ctrl = GetPtr();
        e3 = CGEval( GetTypedValue() );
    }
    e2 = CGEval( GetTypedValue() );
    e1 = GetTypedValue();
    top = GetLabel( GetU16() );
    bottom = GetLabel( GetU16() );
    do_type = F772CGType( do_var );
    CGTrash( CGAssign( SymAddr( do_var ), e1, do_type ) );
    if( increment == NULL ) {  // INTEGER DO variable/constant increment
        if( incr_value < 0 ) {
            cmp_op = O_GE;
        } else {
            cmp_op = O_LE;
        }
        CGDone( CGAssign( SymAddr( loop_ctrl ), e2, do_type ) );
        CGControl( O_LABEL, NULL, top );
        CGControl( O_IF_FALSE,
                   CGCompare( cmp_op, SymValue( do_var ), SymValue( loop_ctrl ),
                              do_type ),
                   bottom );
    } else {
        // compute:  e2 - e1
        tmp = CGBinary( O_MINUS, e2, SymValue( do_var ), do_type ); // e2 - e1
        // compute:  e2 - e1 + e3
        tmp = CGBinary( O_PLUS, tmp, CGAssign( SymAddr( increment ),
                                                e3, do_type ), do_type );
        // compute:  ( e2 - e1 + e3 ) / e3
        tmp = CGBinary( O_DIV, tmp, SymValue( increment ), do_type );
        // set type of interation count
        if( !_IsTypeInteger( do_var->ns.typ ) ) {
            do_type = TY_INT_4;
        }
        // save interation count
        tmp = CGAssign( SymAddr( loop_ctrl ), tmp, do_type );
        // goto bottom of loop if iteration count <= 0
        tmp = CGCompare( O_GT, tmp, CGInteger( 0, TY_INTEGER ), do_type );
        CGControl( O_IF_FALSE, tmp, bottom );
        CGControl( O_LABEL, NULL, top );
    }
}


void    FCDoEnd( void ) {
//=================

// Terminal DO loop processing.

    sym_id              do_var;
    sym_id              increment;
    sym_id              iteration;
    label_handle        top;
    cg_type             do_type;
    intstar4            incr_value;
    cg_name             tmp;

    do_var = GetPtr();
    increment = GetPtr();
    if( increment == NULL ) {
        incr_value = GetConst32();
    } else {
        iteration = GetPtr();
    }
    top = GetLabel( GetU16() );
    do_type = F772CGType( do_var );
    if( increment == NULL ) {  // INTEGER DO variable with constant increment
        CGDone( CGAssign( SymAddr( do_var ),
                          CGBinary( O_PLUS, SymValue( do_var ),
                                    CGInteger( incr_value, do_type ),
                                    do_type ),
                          do_type ) );
        CGControl( O_GOTO, NULL, top );
    } else {
        // compute:  do_var += increment
        tmp = CGBinary( O_PLUS, SymValue( do_var ), SymValue( increment ),
                         do_type );
        CGDone( CGAssign( SymAddr( do_var ), tmp, do_type ) );
        // set type of iteration
        if( !_IsTypeInteger( do_var->ns.typ ) ) {
            do_type = TY_INT_4;
        }
        // compute:  --iteration
        tmp = CGBinary( O_MINUS, SymValue( iteration ),
                         CGInteger( 1, TY_INTEGER ), do_type );
        tmp = CGAssign( SymAddr( iteration ), tmp, do_type );
        // goto top of loop if iteration != 0
        tmp = CGCompare( O_EQ, tmp, CGInteger( 0, TY_INTEGER ), do_type );
        CGControl( O_IF_FALSE, tmp, top );
    }
}
