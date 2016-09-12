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
// VOLATILE   : VOLATILE statement
//

#include "ftnstd.h"
#include "errcod.h"
#include "global.h"
#include "namecod.h"
#include "recog.h"
#include "ferror.h"
#include "insert.h"
#include "utility.h"
#include "symtab.h"
#include "proctbl.h"


#define ERR_MASK    (SY_USAGE | SY_TYPE | SY_IN_DIMEXPR | SY_SUBSCRIPTED | \
                     SY_DATA_INIT | SY_SAVED | SY_IN_EC | SY_REFERENCED)


void    CpVolatile( void ) {
//====================

// Compile VOLATILE statement.

//     VOLATILE VAR {,VAR1} . . .

    sym_id      sym;

    for( ;; ) {
        if( ReqName( NAME_VAR_OR_ARR ) ) {
            sym = LkSym();
            if( (sym->u.ns.flags & ~ERR_MASK ) != SY_VARIABLE ) {
                IllName( sym );
            } else {
                sym->u.ns.u1.s.xflags |= SY_VOLATILE;
            }
        }
        AdvanceITPtr();
        if( !RecComma() ) {
            break;
        }
    }
    ReqEOS();
}
