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
// PROCLIST  : process argument/subscript/substring lists
//

#include "ftnstd.h"
#include "opn.h"
#include "errcod.h"
#include "opr.h"
#include "global.h"
#include "stmtsw.h"
#include "recog.h"
#include "ferror.h"
#include "insert.h"
#include "utility.h"
#include "optr.h"
#include "proclist.h"
#include "fieldops.h"
#include "gsfunc.h"
#include "gsubprog.h"
#include "gsubscr.h"
#include "gsubstr.h"



static  void    SubStrArgs( itnode *sym_node ) {
//==============================================

    itnode      *ss_node;

    ss_node = CITNode;
    GSubStr( sym_node );
    AdvanceITPtr();
    GSubStr( sym_node );
    GFiniSS( sym_node, ss_node );
}


static  void    PrSFList( void ) {
//==========================

    int         num_parms;
    unsigned_16 flags;
    itnode      *sfunc_node;
    bool        done_list;
    sf_parm     *parm;
    sym_id      sf_sym;
    sym_id      sym_ptr;
    USOPN       what;
    USOPN       where;

    StmtSw |= SS_SF_REFERENCED;
    sf_sym = CITNode->sym_ptr;
    sf_sym->u.ns.si.sf.header->ref_count++;
    sfunc_node = CITNode;
    CITNode = CITNode->list;
    AdjSFList( CITNode );
    parm = sf_sym->u.ns.si.sf.header->parm_list;
    if( RecNOpn() ) {
        AdvanceITPtr();
        done_list = RecCloseParen() || RecColon();
    } else {
        done_list = FALSE;
        num_parms = 0;
        for(;;) {
            if( done_list ) break;
            if( parm == NULL ) break;
            num_parms++;
            sym_ptr = parm->shadow;
            if( sym_ptr->u.ns.u1.s.typ != CITNode->typ ) {
                TypeTypeErr( SF_PARM_TYPE_MISMATCH, CITNode->typ,
                             sym_ptr->u.ns.u1.s.typ );
            } else {
                what = CITNode->opn.us & USOPN_WHAT;
                where = CITNode->opn.us & USOPN_WHERE;
                if( ( what == USOPN_NWL ) || ( where == USOPN_SAFE ) ||
                    ( where == USOPN_TMP ) ) {
                    GSFArg( sym_ptr );
                } else {
                    flags = CITNode->flags;
                    if( ( ( flags & SY_CLASS ) == SY_VARIABLE ) ||
                        ( ( flags & SY_CLASS ) == SY_PARAMETER ) ) {
                        if( ( what == USOPN_ASS ) ||
                            ( ( flags & SY_SUBSCRIPTED ) == 0 ) ) {
                            GSFArg( sym_ptr );
                        } else {
                            ClassNameErr( SF_ILL_PARM_PASSED,
                                          CITNode->sym_ptr );
                        }
                    } else {
                        ClassNameErr( SF_ILL_PARM_PASSED, CITNode->sym_ptr );
                    }
                }
            }
            AdvanceITPtr();
            parm = parm->link;
            done_list = RecCloseParen() || RecColon();
        }
    }
    if( ( parm != NULL ) || ( done_list == FALSE ) ) {
        while( parm != NULL ) {
            num_parms++;
            parm = parm->link;
        }
        NameErr( SF_WRONG_NUM_PARMS, sf_sym );
    }
    GSFCall( sfunc_node );
    GSFRetVal( sfunc_node );
    if( RecColon() ) {
        GBegSFSS( sfunc_node );
        SubStrArgs( sfunc_node );
    }
}


static  void    PrSubList( itnode *array_node ) {
//===============================================

    GBegSubScr( array_node );
    for(;;) {
        GSubScr();
        AdvanceITPtr();
        if( RecCloseParen() || RecColon() ) break;
    }
    GEndSubScr( array_node );
    if( RecColon() ) {
        GBegSSStr( array_node );
        SubStrArgs( array_node );
    } else {
        if( array_node->opn.us & USOPN_FLD ) {
            if( array_node->sym_ptr->u.fd.typ == FT_CHAR ) {
                if( !(StmtSw & SS_DATA_INIT) )
                    GFieldSCB( array_node->sym_ptr->u.fd.xt.size );
            }
        }
    }
}


static  void    PrSStr( itnode *var_node ) {
//==========================================

    GInitSS( var_node );
    SubStrArgs( var_node );
}


static  void    PrCallList( itnode *subpgm_node ) {
//=================================================

    GBegCall( subpgm_node );
    GEndCall( subpgm_node, GParms( subpgm_node ) );
    if( RecColon() ) {
        GBegFSS( subpgm_node );
        SubStrArgs( subpgm_node );
    }
}

void    ProcList( itnode *cit ) {
//===============================

    itnode      *save_citnode;
    unsigned_16 cit_flags;

    save_citnode = CITNode;
    cit_flags = cit->flags;
    CITNode = cit->list;
    if( ( cit_flags & SY_CLASS ) == SY_SUBPROGRAM ) {
        if( ( cit_flags & SY_SUBPROG_TYPE ) == SY_STMT_FUNC ) {
            CITNode = cit;  // PrSFList has to look up the symbol
            PrSFList();
        } else {
            if( ( cit->typ == FT_CHAR ) && ( cit->size == 0 ) ) {
                CITNode = cit;    // For OpndErr().
                OpndErr( SR_ILL_CHARFUNC );
            } else {
                PrCallList( cit );
            }
        }
    } else if( ( ( cit_flags & SY_CLASS ) == SY_VARIABLE ) &&
            ( cit_flags & SY_SUBSCRIPTED ) ) {
        PrSubList( cit );
    } else {
        PrSStr( cit );
    }
    FreeITNodes( cit->list );
    cit->list = NULL;
    CITNode = save_citnode;
}


