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
// INTRNSC   : intrinsic functions
//

#include "ftnstd.h"
#include "errcod.h"
#include "global.h"
#include "namecod.h"
#include "recog.h"
#include "types.h"
#include "iflookup.h"
#include "ferror.h"
#include "insert.h"
#include "utility.h"
#include "proctbl.h"
#include "symtab.h"


#define ERR_MASK        (SY_IN_EC | SY_SUB_PARM)


void    CpIntrinsic(void) {
//=====================

// Compile INTRINSIC statement.

//     INTRINSIC FUNC {,FUNC1} . . .

    unsigned_16 flags;
    IFF         func;
    sym_id      sym_ptr;
    TYPE        func_typ;

    for(;;) {
        if( ReqName( NAME_INTRINSIC ) ) {
            func = IFLookUp();
            if( func > 0 ) {
                func_typ = IFType( func );
                sym_ptr = LkSym();
                flags = sym_ptr->u.ns.flags;
                if( ( flags & SY_USAGE ) != 0 ) {
                    if( ( flags & SY_CLASS ) == SY_SUBPROGRAM ) {
                        if( ( flags & SY_INTRINSIC ) != 0 ) {
                            Error( SR_PREV_INTRNSC );
                        } else if( ( flags & SY_EXTERNAL ) != 0 ) {
                            Error( SR_INTRNSC_EXTRN );
                        }
                    } else {
                        IllName( sym_ptr );
                    }
                } else if( flags & ERR_MASK ) {
                    IllName( sym_ptr );
                } else if( ( flags & SY_TYPE ) &&
                           ( sym_ptr->u.ns.u1.s.typ != func_typ ) ) {
                    NameTypeErr( TY_TYP_PREV_DEF, sym_ptr );
                } else {
                    // we must OR the flags since SY_TYPE and/or SY_REFERENCED
                    // bit might already be set in the symbol table
                    sym_ptr->u.ns.flags |= SY_USAGE | SY_SUBPROGRAM |
                                         SY_FUNCTION | SY_INTRINSIC;
                    func_typ = IFType( func );
                    sym_ptr->u.ns.u1.s.typ = func_typ;
                    sym_ptr->u.ns.xt.size = TypeSize( func_typ );
                    sym_ptr->u.ns.si.fi.index = func;
                }
            } else {
                Error( SR_NOT_INTRNSC );
            }
        }
        AdvanceITPtr();
        if( !RecComma() ) {
            break;
        }
    }
    ReqEOS();
}
