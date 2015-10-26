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
// FIELDOPS     : field selection operator generation routines
//

#include "ftnstd.h"
#include "global.h"
#include "opn.h"
#include "fcodes.h"
#include "stmtsw.h"
#include "optr.h"
#include "emitobj.h"
#include "types.h"

extern  sym_id          GTempString(uint);


void    GFieldSCB( inttarg size ) {
//=================================

// Setup an SCB for a character field.

    PushConst( size );
    EmitOp( FC_FLIP );
}


void    FieldOp( TYPE typ1, TYPE typ2, OPTR op ) {
//================================================

// Generate code for a field selection operator.

    typ1 = typ1; op = op;
    PushOpn( CITNode->link );
    PushOpn( CITNode );
    if( CITNode->opn.us & USOPN_FLD ) {
        // sub-field reference
        EmitOp( FC_ADD );
        DumpTypes( FT_INTEGER, TypeSize( FT_INTEGER ), FT_INTEGER, TypeSize( FT_INTEGER ) );
    } else {
        EmitOp( FC_FIELD_OP );
        OutPtr( CITNode->sym_ptr );
        if( ( StmtSw & SS_DATA_INIT ) == 0 ) {
            if( typ2 == FT_CHAR ) {
                if( ( CITNode->link->opn.us & USOPN_WHAT ) != USOPN_ARR ) {
                    if( ( ( CITNode->link->opn.us & USOPN_WHAT ) != USOPN_NWL ) &&
                        ( ( CITNode->link->opn.us & USOPN_WHAT ) != USOPN_ASS ) ) {
                        GFieldSCB( CITNode->link->size );
                    }
                    EmitOp( FC_MAKE_SCB );
                    OutPtr( GTempString( 0 ) );
                }
            }
        } else {
            OutPtr( CITNode->link->sym_ptr );
        }
    }
}
