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
// DETACH    : detach argument/subscript/substring lists
//

#include "ftnstd.h"
#include "opn.h"
#include "errcod.h"
#include "opr.h"
#include "global.h"
#include "recog.h"
#include "ferror.h"
#include "insert.h"
#include "utility.h"

extern  bool    OptimalChSize(uint);

/* Forward declarations */
static  void    SubStrArgs( itnode *cit );
static  void    Detach( itnode *cit );
static  void    CkScrStr(void);


static  bool    CheckColon(void) {
//============================

    if( RecNextOpr( OPR_COL ) ) {
        CITNode->opr = OPR_COL;
        return( TRUE );
    }
    return( FALSE );
}


void    SetDefinedStatus(void) {
//==========================

    switch( CITNode->opn.us & USOPN_WHAT ) {
    case USOPN_NNL:
    case USOPN_ASS:
    case USOPN_NWL:
    case USOPN_ARR:
        if( ClassIs( SY_VARIABLE ) ) {
            CITNode->sym_ptr->ns.u1.s.xflags |= SY_DEFINED;
        }
    }
}


void    DetCallList(void) {
//=====================

    itnode      *cit;

    cit = CITNode;
    AdvanceITPtr();
    if( RecNOpn() ) {
        AdvanceITPtr();
    } else {
        SetDefinedStatus();
        AdvanceITPtr();
        while( RecComma() ) {
            if( CheckColon() ) {
                Extension( SS_FUNCTION_VALUE );
                SubStrArgs( cit );
                Detach( cit );
                return;
            }
            if( RecNOpn() ) break;
            SetDefinedStatus();
            AdvanceITPtr();
        }
    }
    if( !RecCloseParen() ) {
        Error( PC_NO_CLOSEPAREN );
    }
    Detach( cit );
}


void    DetSubList(void) {
//====================

    itnode      *cit;
    int         count;
    byte        no_subs;
    itnode      *save_cit;
    uint        ch_size;

    if( CITNode->opn.us & USOPN_FLD ) {
        no_subs = _DimCount( CITNode->sym_ptr->fd.dim_ext->dim_flags );
    } else {
        no_subs = _DimCount( CITNode->sym_ptr->ns.si.va.u.dim_ext->dim_flags );
    }
    count = 0;
    cit = CITNode;
    AdvanceITPtr();
    while( RecComma() || RecFBr() ) {
        if( CheckColon() ) {
            if( count == 0 ) {
                save_cit = CITNode;
                CITNode = cit;
                OpndErr( SV_TRIED_SSTR );
                CITNode = save_cit;
            } else if( count != no_subs ) {
                Error( SV_INV_SSCR );
            }
            SubStrArgs( cit );
            cit->opn.us &= ~USOPN_WHAT;
            cit->opn.us |= USOPN_ASS;
            Detach( cit );
            return;
        }
        if( RecNOpn() ) break;
        ++count;
        CkScrStr();
        AdvanceITPtr();
    }
    if( !RecCloseParen() ) {
        Error( PC_NO_CLOSEPAREN );
    }
    if( count != no_subs ) {
        Error( SV_INV_SSCR );
    }
    // we must make sure the array isn't substrung before we can set OPN_SS1
    if( !( cit->opn.us & USOPN_FLD ) && ( cit->sym_ptr->ns.u1.s.typ == FT_CHAR ) ) {
        ch_size = cit->sym_ptr->ns.xt.size;
        if( ch_size > 0 ) {
            cit->opn.us |= USOPN_SS1;
            cit->value.st.ss_size = ch_size;
        }
    }
    Detach( cit );
}


void    DetSStr( void ) {
//=================

    itnode      *cit;

    cit = CITNode;
    AdvanceITPtr();
    SubStrArgs( cit );
    Detach( cit );
}


static  void    SubStrArgs( itnode *cit ) {
//=========================================

    int         count;

    if( cit->typ != FT_CHAR ) {
        Error( SS_ONLY_IF_CHAR );
    }
    count = 1;
    for(;;) {
        if( CITNode->opn.us & USOPN_SS1 ) {
            cit->opn.us |= USOPN_SS1;
            cit->value.st.ss_size = CITNode->value.st.ss_size;
        }
        CkScrStr();
        AdvanceITPtr();
        if( !RecColon() ) break;
        ++count;
    }
}


static  void    CkScrStr( void ) {
//==========================

    USOPN        opn;

    ChkType( FT_INTEGER );
    opn = CITNode->opn.us;
    if( (opn & USOPN_WHAT) != USOPN_ARR ) return;
    ClassErr( SV_NO_LIST, CITNode->sym_ptr );
}


static  void    Detach( itnode *cit ) {
//=====================================

    cit->list = cit->link;
    cit->link = CITNode->link;
    if( !RecNOpn() ) {
        Error( SX_NO_OPR );
    }
    CITNode->link = NULL;
    CITNode = cit;
}
