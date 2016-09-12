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
// COMMON    : compile COMMON statement
//

#include "ftnstd.h"
#include "global.h"
#include "ecflags.h"
#include "opr.h"
#include "errcod.h"
#include "namecod.h"
#include "recog.h"
#include "ferror.h"
#include "insert.h"
#include "utility.h"
#include "proctbl.h"
#include "rstalloc.h"
#include "symtab.h"
#include "declare.h"


void    CpCommon(void) {
//==================

// Compile COMMON statement:

//    COMMON {/{CB}/}A1,...,An {{,}/{CB}/B1,...,Bm} ...

    unsigned_16 flags;
    sym_id      com;
    sym_id      sym;
    sym_id      last;
    com_eq      *com_ext;

    sym = NULL;
    if( RecNOpn() ) {
        AdvanceITPtr();
        if( !RecDiv() ) {
            if( !RecCat() ) {
                Error( SX_NO_SLASH );
            }
        }
    }
    for( ;; ) {
        if( RecDiv() ) {
            if( ReqName( NAME_COMMON ) ) {
                com = LkCommon();
            } else {
                com = LkBCommon();
            }
            AdvanceITPtr();
            ReqDiv();
        } else {
            com = LkBCommon();
        }
        for( ;; ) {
            if( ReqName( NAME_VAR_OR_ARR ) ) {
                sym = LkSym();
                flags = sym->u.ns.flags;
                CpError = false;
                if( ( flags & SY_CLASS ) != SY_VARIABLE ) {
                    IllName( sym );
                } else if( flags & SY_IN_COMMON ) {
                    NameErr( CM_ALREADY_IN_COM, sym );
                } else if( flags & SY_SUB_PARM ) {
                    IllName( sym );
                } else if( flags & SY_SAVED ) {
                    Error( SA_SAVED );
                } else if( flags & SY_DATA_INIT ) {
                    NameErr( ST_DATA_ALREADY, sym );
                } else if( ( flags & SY_SUBSCRIPTED ) && _Allocatable( sym ) ) {
                    IllName( sym );
                }
            }
            AdvanceITPtr();
            if( !CpError ) {
                if( RecOpenParen() ) {
                    ArrayDecl( sym );
                    if( _Allocatable( sym ) ) {
                        IllName( sym );
                    }
                }
                // check if we dimensioned an allocatable array
                if( !CpError ) {
                    sym->u.ns.flags |= SY_IN_COMMON;
                    com_ext = sym->u.ns.si.va.vi.ec_ext;
                    // Since "ec_ext" is used for common and equivalence,
                    // the "ec_ext" may have already been allocated as a
                    // result of appearing in an EQUIVALENCE statement.
                    if( com_ext == NULL ) {
                        com_ext = STComEq();
                        sym->u.ns.si.va.vi.ec_ext = com_ext;
                    }
                    com_ext->ec_flags |= LAST_IN_COMMON;
                    com_ext->com_blk = com;
                    last = com->u.ns.si.cb.first;
                    if( last == NULL ) {    // if empty common block
                        com->u.ns.si.cb.first = sym;
                    } else {
                        for( ;; ) {
                            com_ext = last->u.ns.si.va.vi.ec_ext;
                            if( com_ext->link_com == NULL )
                                break;
                            last = com_ext->link_com;
                        }
                        com_ext->link_com = sym;
                        com_ext->ec_flags &= ~LAST_IN_COMMON;
                    }
                }
            }
            if( RecComma() && RecNOpn() ) {
                AdvanceITPtr();
            }
            if( !RecComma() ) {
                break;
            }
        }
        if( !RecDiv() && !RecCat() ) {
            break;
        }
    }
    ReqEOS();
}
