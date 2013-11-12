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
// GSFUNC    : statement function code generation routines
//

#include "ftnstd.h"
#include "global.h"
#include "fcodes.h"
#include "opn.h"
#include "emitobj.h"

extern  sym_id          GTempString(uint);


void    GSFArg( sym_id sym ) {
//============================

// Generate a statement function argument.

    sym = sym;
}


void    GSFCall( itnode *sfunc ) {
//================================

// Generate a statement function call.

    sf_parm     *arg;

    EmitOp( FC_SF_CALL );
    OutPtr( sfunc->sym_ptr );
    arg = sfunc->sym_ptr->u.ns.si.sf.header->parm_list;
    while( arg != NULL ) {
        OutPtr( arg->shadow );
        arg = arg->link;
    }
    OutPtr( NULL );
    if( sfunc->typ == FT_CHAR ) {
        OutPtr( GTempString( sfunc->size ) );
    }
    SetOpn( sfunc, USOPN_SAFE );
}


void    GSFRetVal( itnode *sfunc ) {
//==================================

// Generate a statement function return value.

    sfunc = sfunc;
}


void    AdjSFList( itnode *sfunc ) {
//==================================

// Process statement function argument list.

    sfunc = sfunc;
}
