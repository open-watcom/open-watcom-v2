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
// SAVE      : SAVE statement processor
//

#include "ftnstd.h"
#include "errcod.h"
#include "ecflags.h"
#include "opr.h"
#include "segsw.h"
#include "namecod.h"
#include "global.h"
#include "recog.h"
#include "ferror.h"
#include "insert.h"
#include "utility.h"
#include "proctbl.h"
#include "symtab.h"


#define SV_ON_OR_OFF    (SY_USAGE | SY_TYPE | SY_IN_DIMEXPR | SY_SUBSCRIPTED | \
                         SY_DATA_INIT | SY_SAVED | SY_IN_EQUIV | SY_REFERENCED)

static  void    Save( sym_id sym_ptr ) {
//======================================

// Check that item has not been saved twice.

    if( ( (sym_ptr->u.ns.flags & SY_SAVED) == 0 ) && ( (SgmtSw & SG_BIG_SAVE) == 0 ) ) {
        sym_ptr->u.ns.flags |= SY_SAVED;
        SgmtSw |= SG_LITTLE_SAVE;
    } else {
        Error( SA_SAVED );
    }
}

void    CpSave( void ) {
//================

// Compile SAVE statement :    SAVE A1 {,A2 ... }
//
//      where Ai is 1. a common block name preceded and followed by a /
//                  2. an array name
//                  3. a variable name

    sym_id      sym_ptr;

    if( RecNOpn() && RecNextOpr( OPR_TRM ) ) {
        if( ( (SgmtSw & SG_LITTLE_SAVE) != 0 ) || ( (SgmtSw & SG_BIG_SAVE) != 0 ) ) {
            Error( SA_SAVED );
        }
        SgmtSw |= SG_BIG_SAVE;
    } else {
        for( ;; ) {
            if( RecNOpn() && RecNextOpr( OPR_DIV ) ) {
                AdvanceITPtr();
                if( ReqName( NAME_COMMON ) ) {
                    sym_ptr = LkCommon();
                    if( sym_ptr->u.ns.flags == 0 ) {
                        sym_ptr->u.ns.flags |= SY_USAGE | SY_COMMON;
                    }
                    Save( sym_ptr );
                }
                AdvanceITPtr();
                ReqDiv();
                ReqNOpn();
            } else if( ReqName( NAME_VAR_OR_ARR ) ) {
                sym_ptr = LkSym();
                if( ( sym_ptr->u.ns.flags & ~SV_ON_OR_OFF ) != SY_VARIABLE ) {
                    IllName( sym_ptr );
                } else {
                    Save( sym_ptr );
                }
            }
            AdvanceITPtr();
            if( !RecComma() ) {
                break;
            }
        }
        ReqEOS();
    }
}

