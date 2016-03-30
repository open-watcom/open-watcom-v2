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
// EXTERNAL  : EXTERNAL statement processor
//

#include "ftnstd.h"
#include "global.h"
#include "errcod.h"
#include "namecod.h"
#include "cpopt.h"
#include "recog.h"
#include "ferror.h"
#include "insert.h"
#include "utility.h"
#include "proctbl.h"
#include "symtab.h"


void    CpExternal(void) {
//====================

// Compile EXTERNAL statement.

//     EXTERNAL PROC {,PROC1} . . .

    sym_id      sym;
    unsigned_16 flags;

    for(;;) {
        if( ReqName( NAME_EXT_PROC ) ) {
            sym = LkSym();
            flags = sym->u.ns.flags;
            if( ( flags & SY_CLASS ) == SY_SUBPROGRAM ) {
                if( ( flags & SY_EXTERNAL ) != 0 ) {
                    Error( SR_EXTRNED_TWICE );
                } else if( ( flags & SY_INTRINSIC ) != 0 ) {
                    Error( SR_INTRNSC_EXTRN );
                }
            } else if( ( flags & SY_USAGE ) != 0 ) {
                IllName( sym );
            } else {
                flags = SY_USAGE | SY_SUBPROGRAM | SY_EXTERNAL;
                if( ( sym->u.ns.flags & SY_TYPE ) != 0 ) {
                    flags |= SY_FUNCTION;
                }
                sym->u.ns.flags |= flags;
                if( ( Options & OPT_REFERENCE ) == 0 ) {
                    sym->u.ns.flags |= SY_RELAX_EXTERN;
                }
            }
        }
        AdvanceITPtr();
        if( !RecComma() ) break;
    }
    ReqEOS();
}
