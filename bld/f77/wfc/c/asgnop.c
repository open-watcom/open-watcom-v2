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
// ASGNOP    : assignment code generation routines
//

#include "ftnstd.h"
#include "fcodes.h"
#include "global.h"
#include "opn.h"
#include "optr.h"
#include "emitobj.h"

extern  void            AsgnChar(void);

void    AsgnOp( TYPE typ1, TYPE typ2, OPTR opr ) {
//================================================

// Generate code to perform an assign operation.

    typ1 = typ1; typ2 = typ2; opr = opr;
    if( ResultType == FT_CHAR ) {
        AsgnChar();
    } else {
        EmitOp( FC_POP );
        SymRef( CITNode );
        GenTypes( CITNode, CITNode->link );
        if( CITNode->sym_ptr->ns.u1.s.typ == FT_STRUCTURE ) {
            if( CITNode->opn.us & USOPN_SAFE ) {
                // destination is a sub-field or an array element
                OutU16( 1 );
                if( CITNode->typ == FT_STRUCTURE ) {
                    if( CITNode->opn.us & USOPN_FLD ) {
                        // sub-field is a structure
                        OutPtr( CITNode->value.st.field_id );
                    } else {
                        // array element is a structure
                        OutPtr( NULL );
                    }
                }
            } else {
                // destination is a structure
                OutU16( 0 );
            }
        }
    }
    SetOpn( CITNode, USOPN_SAFE ); // CGAssign() leaves "cg_name" on stack
}
