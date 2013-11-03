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
// CSGOTO    : process GOTOs , arithmetic IF , and ASSIGN
//

#include "ftnstd.h"
#include "opr.h"
#include "opn.h"
#include "errcod.h"
#include "ctrlflgs.h"
#include "global.h"
#include "recog.h"
#include "ferror.h"
#include "insert.h"
#include "utility.h"

#include <ctype.h>

extern  sym_id          CkAssignOk( void );
extern  void            CkSize4( void );
extern  void            ScanExpr( void );
extern  label_id        NextLabel( void );
extern  void            CkTypeDeclared( void );
extern  sym_id          LkUpAssign( void );
extern  sym_id          LkUpStmtNo( void );
extern  void            AddCSNode( byte );
extern  void            DelCSNode( void );
extern  case_entry      *NewCase( void );
extern  void            InitSelect( void );
extern  void            FiniSelect( void );
extern  void            GAsgnGoTo( bool );
extern  void            GAssign( sym_id );
extern  void            GLabel( label_id );
extern  void            G3WayBranch( sym_id, sym_id, sym_id );
extern  void            GBranch( label_id );
extern  void            GStmtBr( sym_id );
extern  void            FreeLabel( label_id );
extern  void            GStmtAddr( sym_id );
extern  void            GEndBrTab( void );
extern  void            BIOutSymbol( sym_id );


static void UnCondGoto( void )
{
// Process an unconditional GOTO statement.

    GStmtBr( LkUpStmtNo() );
    AdvanceITPtr();
}

static void CompGoto( void )
{
// Process a computed GOTO statement.

    case_entry  *caseptr;
    int         i;

    AddCSNode( CS_COMPUTED_GOTO );
    CSHead->branch = NextLabel();
    CSHead->cs_info.cases = NewCase();
    caseptr = CSHead->cs_info.cases;
    caseptr->label.g_label = NextLabel(); // if expr is out of range 1-n
    AdvanceITPtr();
    ReqOpenParen();
    i = 1;
    for(;;) {
        caseptr->link = NewCase();
        caseptr = caseptr->link;
        caseptr->label.st_label = LkUpStmtNo();
        caseptr->low = i;
        caseptr->high = i;
        AdvanceITPtr();
        i++;
        if( RecComma() == FALSE ) break;
    }
    ReqCloseParen();
    if( !RecEOS() ) {
        if( RecNOpn() && RecNextOpr( OPR_COM ) ) {
            AdvanceITPtr();
        }
        if( !RecEOS() ) {
            IntegerExpr();
            InitSelect();
            FiniSelect();
            GLabel( CSHead->cs_info.cases->label.g_label );
            AdvanceITPtr();
        } else {
            TypeErr( SX_WRONG_TYPE, FT_INTEGER );
        }
    } else {
        TypeErr( SX_WRONG_TYPE, FT_INTEGER );
    }
    FreeLabel( CSHead->branch );
    FreeLabel( CSHead->cs_info.cases->label.g_label );
    DelCSNode();
}

void CkVarRef( void )
{
    BIOutSymbol( CITNode->sym_ptr );
    CITNode->sym_ptr->ns.flags |= SY_REFERENCED;
    CkTypeDeclared();
}

static void AsgnGoto( void )
{
// Process an assigned GOTO statement.

    if( ReqIntVar() ) {
        CkVarRef();
    }
    if( RecNextOpr( OPR_TRM ) ) {
        StNumbers.wild_goto = TRUE;
        GAsgnGoTo( FALSE );
        AdvanceITPtr();
    } else {
        GAsgnGoTo( TRUE );
        AdvanceITPtr();
        if( RecComma() && RecNOpn() ) {
            AdvanceITPtr();
        }
        ReqOpenParen();
        for(;;) {
            GStmtAddr( LkUpStmtNo() );
            AdvanceITPtr();
            if( RecComma() == FALSE ) break;
        }
        GEndBrTab();
        ReqCloseParen();
        if( RecNOpn() ) {
            AdvanceITPtr();
        }
    }
}

void CpGoto( void )
{
// Process a GOTO statement.

    if( RecNumber() ) {
        Remember.transfer = TRUE;
        UnCondGoto();
    } else if( RecNOpn() ) {
        CompGoto();
        CtrlFlgs &= ~CF_BAD_DO_ENDING;
    } else {
        Remember.transfer = TRUE;
        AsgnGoto();
    }
    ReqEOS();
}

void CpArithIf( void )
{
// Process an arithmetic IF.

    sym_id      lt_label = NULL;
    sym_id      eq_label = NULL;
    sym_id      gt_label = NULL;
    itnode      *cit;

    Remember.transfer = TRUE;
    if( RecNOpn() ) {
        AdvanceITPtr();
    }
    cit = CITNode;
    if( ReqOpenParen() ) {
        cit->opr = OPR_TRM; // so ScanExpr stops on ')'
        AdvanceITPtr();
        ScanExpr();  // scan ahead to line numbers
    }
    if( ReqCloseParen() ) {
        lt_label = LkUpStmtNo();
        AdvanceITPtr();
    }
    if( ReqComma() ) {
        eq_label = LkUpStmtNo();
        AdvanceITPtr();
    }
    if( ReqComma() ) {
        gt_label = LkUpStmtNo();
        AdvanceITPtr();
        ReqEOS();
    }
    CITNode = cit;
    G3WayBranch( lt_label, eq_label, gt_label );
}

void CpAssign( void )
{
// Process an ASSIGN statement ( eg. ASSIGN 10 TO I ).

    char        *opnd;
    sym_id      label;

    if( RecNOpn() && RecNextOpr( OPR_PHI ) ) {
        AdvanceITPtr();
    }
    label = LkUpAssign();
    AdvanceITPtr();
    opnd = CITNode->opnd;
    if( RecNOpr() && RecName() && ( CITNode->opnd_size >= 2 ) &&
        ( opnd[ 0 ] == 'T' ) && ( opnd[ 1 ] == 'O' ) ) {
        CITNode->opnd += 2;
        CITNode->opnd_size -= 2;
        CITNode->opnpos += 2;
        if( ( CITNode->opnd_size == 0 ) && isupper( *(CITNode->opnd) ) ) {
            CITNode->opn.ds = DSOPN_PHI;
        }
    } else {
        Error( GO_NO_TO );
    }
    if( ReqIntVar() ) {
        CkVarRef();
        CkSize4();
        CkAssignOk();
        GAssign( label );
    }
    AdvanceITPtr();
    ReqEOS();
}
