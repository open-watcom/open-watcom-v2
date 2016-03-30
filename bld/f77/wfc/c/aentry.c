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
* Description:  routines that call the expression handler
*
****************************************************************************/


#include "ftnstd.h"
#include "opr.h"
#include "opn.h"
#include "astype.h"
#include "errcod.h"
#include "namecod.h"
#include "global.h"
#include "recog.h"
#include "ferror.h"
#include "insert.h"
#include "utility.h"
#include "aentry.h"
#include "proctbl.h"
#include "arutls.h"
#include "downscan.h"
#include "stmtfunc.h"
#include "symtab.h"
#include "upscan.h"


#define SF_MASK (SY_SUB_PARM|SY_IN_EC|SY_DATA_INIT|SY_SAVED)


static  void    Arith(void) {
//=======================

// The expression handler.

    DownScan();
    UpScan();
}


void    EatExpr(void) {
//=================

// Scan ahead, get an expression, and send it to expression handler.

    OPR         opr;
    itnode      *cit;

    cit = CITNode;
    BkLink = CITNode;
    AdvanceITPtr();
    ScanExpr();                  // scan ahead to end of expr
    opr = CITNode->opr;         // remembers operator at end
    CITNode->opr = OPR_TRM;     // marks end of expr
    CITNode = cit;              // back to start of expr
    CITNode->opr = OPR_TRM;     // marks start of expr
    Arith();
    CITNode->link->opr = opr;   // restores operator at end
}


void    CpAsgnmt(void) {
//==================

    ASType = AST_EOK;                          // equal sign ok
    Arith();
}


void    CpStmtFunc(void) {
//====================

    unsigned_16 flags;

    flags = CITNode->sym_ptr->u.ns.flags;
    if( ( ( flags & SY_CLASS ) == SY_VARIABLE ) &&
        ( ( flags & SF_MASK ) == 0 ) ) {
        ASType = AST_EOK | AST_ASF;
        SFPrologue();
        Arith();
    } else {
        IllName( CITNode->sym_ptr );
    }
}

static  void    ArithNewSubr(void) {
//==============================

    if( CITNode->flags & SY_TYPE ) {
        Extension( SR_FUNC_AS_SUB );
        CITNode->sym_ptr->u.ns.flags |= SY_USAGE | SY_SUBPROGRAM | SY_FUNCTION;
    } else {
        ASType |= AST_ALT;
        CITNode->sym_ptr->u.ns.flags |= SY_USAGE | SY_SUBPROGRAM |
                                      SY_SUBROUTINE;
    }
    Arith();
}

void    CpCall(void) {
//================

    itnode      *next;
    unsigned_16 sp_type;

    next = CITNode->link;
    if( next->opr == OPR_TRM ) {
        ASType = AST_CNA;       // call with no parameter list
    } else if( ( next->opr == OPR_LBR ) &&
               ( next->opn.ds == DSOPN_PHI ) &&
               ( next->link->opr == OPR_RBR ) &&
               ( next->link->opn.ds == DSOPN_PHI ) &&
               ( next->link->link->opr == OPR_TRM ) ) {
        next->opr = OPR_TRM;    // make CALL SAM() same as CALL SAM
        ASType = AST_CNA;       // call with no parameter list
    } else {
        ReqNextOpr( OPR_LBR, SX_SURP_OPR );
        if( ( SPtr1 != NULL ) && ( SPtr1->link->opr != OPR_TRM ) ) {
            Error( SX_JUNK_AFTER_RBR ); // ignore anything after
            SPtr1->link->opr = OPR_TRM; // closing parenthesis
        }
        ASType = AST_OFF;
    }
    if( ReqName( NAME_SUBROUTINE ) ) {
        LkSym();
        if( ClassIs( SY_SUBPROGRAM ) ) {
            sp_type = CITNode->flags & SY_SUBPROG_TYPE;
            if( sp_type == SY_SUBROUTINE ) {
                ASType |= AST_ALT;
                Arith();
            } else if( sp_type == SY_FUNCTION ) {
                Extension( SR_FUNC_AS_SUB );
                Arith();
            } else if( sp_type == 0 ) {
                ArithNewSubr();
            } else {
                Error( SR_NO_SUBRTN_NAME );
            }
        } else {
            if( CITNode->flags & SY_USAGE ) {
                Error( SR_NO_SUBRTN_NAME );
            } else if( CITNode->flags & SY_SAVED ) {
                Error( SA_SAVED );
            } else {
                ArithNewSubr();
            }
        }
    }
}


