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
// GSUBSTR   : substringing code generation routines
//

#include "ftnstd.h"
#include "global.h"
#include "opn.h"
#include "fcodes.h"
#include "stmtsw.h"
#include "emitobj.h"
#include "gstring.h"
#include "gsubstr.h"


void    GBegSSStr( itnode *array_node ) {
//=====================================

// Start a substring operation on an array element.

    array_node = array_node;
}


void    GBegFSS( itnode *func_node ) {
//====================================

// Start a substring operation on a function return value.

    func_node = func_node;
}


void    GBegSFSS( itnode *sf_node ) {
//===================================

// Start a substring operation on a statement function.

    sf_node = sf_node;
}


void    GInitSS( itnode *itptr ) {
//================================

// Start a substring operation.

    PushOpn( itptr );
}


void    GSubStr( itnode *char_node ) {
//====================================

// Generate a substring element.

    char_node = char_node;
}


void    GFiniSS( itnode *sym_node, itnode *ss_node ) {
//====================================================

// Finish a substring operation.

    if( sym_node->opn.us & USOPN_FLD ) {
        PushOpn( sym_node );
        EmitOp( FC_FIELD_SUBSTRING );
        OutPtr( sym_node->sym_ptr );
        if( sym_node->opn.us & USOPN_SS1 ) { // length known at compile-time
            OutInt( sym_node->value.st.ss_size );
        } else {
            OutInt( 0 ); // we don't know the length
        }
    } else {
        EmitOp( FC_SUBSTRING );
        if( sym_node->opn.us & USOPN_SS1 ) { // length known at compile-time
            OutPtr( NULL );
        } else {
            SymRef( sym_node ); // in case we need the length of SCB if
        }                       // character*(*) and no upper bound specified
    }
    GenTypes( ss_node, ss_node->link );
    if( (sym_node->opn.us & USOPN_FLD) == 0 ) {
        if( sym_node->opn.us & USOPN_SS1 ) { // length known at compile-time
            OutInt( sym_node->value.st.ss_size );
        }
        if( (StmtSw & SS_DATA_INIT) == 0 ) {
            sym_node->value.st.ss_id = sym_node->sym_ptr;
            sym_node->sym_ptr = GTempString( 0 );
            OutPtr( sym_node->sym_ptr );
            sym_node->opn.us |= USOPN_ASY;
        }
    }
}
