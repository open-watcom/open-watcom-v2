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
* Description:  Compile a FORTRAN statement.
*
****************************************************************************/


#include "ftnstd.h"
#include "progsw.h"
#include "stmtsw.h"
#include "errcod.h"
#include "opr.h"
#include "opn.h"
#include "cpopt.h"
#include "segsw.h"
#include "extnsw.h"
#include "ctrlflgs.h"
#include "global.h"
#include "recog.h"
#include "ferror.h"
#include "insert.h"
#include "frl.h"
#include "inout.h"
#include "utility.h"
#include "compstmt.h"
#include "proctbl.h"
#include "cpsubpgm.h"
#include "csloops.h"
#include "dsname.h"
#include "iodefs.h"
#include "kwlook.h"
#include "stresolv.h"
#include "symtab.h"
#include "gdbginfo.h"
#include "tdinit.h"

#include <string.h>
#include <ctype.h>


extern  char    *StmtKeywords[];
extern  void    (* const __FAR ProcTable[])( void );
extern  const unsigned_16   __FAR CFTable[];

/* forward declarations */
static void     SetCtrlFlgs( void );
static void     GetStmtType( void );
static void     CheckOrder( void );
static void     DefStmtType( void );
static void     RemCheck( void );
static void     CheckDoEnd( void );
static void     FiniDo( void );

static  void    ChkStatementSequence( void )
{
    SetCtrlFlgs();
    if( (StmtSw & SS_HOLLERITH) && ( StmtProc != PR_FMT ) ) {
        Extension( HO_CONST );
    }
    if( SgmtSw & SG_DEFINING_MAP ) {
        if( CtrlFlgOn( CF_NOT_IN_MAP ) ) {
            StmtPtrErr( SP_NOT_IN_STRUCTURE, StmtKeywords[PR_MAP] );
        }
    } else if( SgmtSw & SG_DEFINING_UNION ) {
        if( CtrlFlgOn( CF_NOT_IN_UNION ) ) {
            StmtPtrErr( SP_NOT_IN_STRUCTURE, StmtKeywords[PR_UNION] );
        }
    } else if( SgmtSw & SG_DEFINING_STRUCTURE ) {
        if( CtrlFlgOn( CF_NOT_IN_STRUCTURE ) ) {
            StmtPtrErr( SP_NOT_IN_STRUCTURE, StmtKeywords[PR_STRUCTURE] );
        }
    } else {
        if( StmtProc == PR_MAP ) {
            StmtPtrErr( SP_STATEMENT_REQUIRED, StmtKeywords[PR_UNION] );
        } else if( StmtProc == PR_UNION ) {
            StmtPtrErr( SP_STATEMENT_REQUIRED, StmtKeywords[PR_STRUCTURE] );
        }
    }
    if( ( (SgmtSw & SG_PROLOG_DONE) == 0 ) &&
        (ProgSw & PS_IN_SUBPROGRAM) &&
        ( !CtrlFlgOn( CF_SPECIFICATION ) ) &&
        ( StmtProc != PR_FMT ) && ( StmtProc != PR_INCLUDE ) ) {
        Prologue();
    }
    if( ( (SgmtSw & SG_SYMTAB_RESOLVED) == 0 ) &&
        ( !CtrlFlgOn( CF_SPECIFICATION | CF_SUBPROGRAM ) ) &&
        ( StmtProc != PR_FMT ) && ( StmtProc != PR_INCLUDE ) ) {
        STResolve();
        SgmtSw |= SG_SYMTAB_RESOLVED;
    }
    if( Remember.endstmt || ( (SgmtSw & SG_STMT_PROCESSED) == 0 ) ) {
        if( !CtrlFlgOn( CF_SUBPROGRAM ) && ( StmtProc != PR_INCLUDE ) ) {
            DefProg();
        }
    }
    // Must be done after prologue so that a label can be dumped
    if( !CtrlFlgOn( CF_SUBPROGRAM ) ) {
        CkDefStmtNo();
    }
}


static  void    ProcStmt( void ) {
//================================

    if( AError )
        return;
    if( CpError && (StmtProc == PR_NULL) )
        return;
    ProcTable[ StmtProc ]();
}

static  void    InitStatement( void )
{
    TDStmtInit();
    ChkPntLst();
    StmtSw = SS_SCANNING;
    CpError = false;
    MakeITList();
    StmtSw &= ~SS_SCANNING;
    StmtProc = 0;
}

static void FiniStatement( void )
{
    FreeITNodes( ITHead );
    ITHead = NULL;
}

void    CompStatement( void )
{
    bool        scan_error;

    InitStatement();
    scan_error = CpError;
    if( ITHead != NULL ) {
        GetStmtType();
        // if it's a bad statement:
        //    - don't set control flags so that we don't get cascading
        //      errors in the following case
        //          INTEGER X
        //          ITNEGER Y
        //          REAL Z
        //    - don't let STResolve() get done so that we have incomplete
        //      data structures as in the following case
        //          A : 2
        //          CHARACTER A, B(79)
        //          EQUIVALENCE (A,B(1))
        //          END
        if( !scan_error && (StmtProc != PR_NULL) ) {
            ChkStatementSequence();
        }
    }
    if( !scan_error ) {
        if( CurrFile->link == NULL ) {  // no line numbering information for
            GSetDbugLine();             // include files
        }
        if( CtrlFlgOn( CF_NEED_SET_LINE ) && (Options & OPT_TRACE) ) {
            GSetSrcLine();
        }
        CheckOrder();
        if( (ProgSw & PS_BLOCK_DATA) && !CtrlFlgOn( CF_BLOCK_DATA ) ) {
            // if statement wasn't decodeable, don't issue an error
            if( StmtProc != PR_NULL ) {
                StmtErr( BD_IN_BLOCK_DATA );
            }
        } else if( CtrlFlgOn( CF_NOT_EXECUTABLE ) ) {
            ProcStmt();
        } else {
            RemCheck();
            ClearRem();
            ProcStmt();
        }
    }
    // Must come before CheckDoEnd(). Consider:
    //           DO I=1,10
    //   10      M = M + 1
    // We must terminate the statement containing the DO label before we
    // terminate the DO-loop.
    TDStmtFini();
    if( StmtNo != 0 ) {
        Update( StmtNo );
        CheckDoEnd();
    }
    // The following must include a check for scanning errors since it is
    // possible to get a scanning error and "StmtProc != PR_NULL" as in the
    // following example:       integer*2 fwinmain( hInstance
    // If the check for a scanning error is not performed, then SubProgId
    // will not be properly set when we compile the RETURN statement in the
    // following example:       integer*2 fwinmain( hInstance
    //                          return
    if( !scan_error && (StmtProc != PR_NULL) && (StmtProc != PR_INCLUDE) ) {
        SgmtSw |= SG_STMT_PROCESSED;
    }
    FiniStatement();
}


void CkDefStmtNo( void )
{
    if( StmtNo != 0 ) {
        DefStmtNo( StmtNo );
    }
}


void Recurse( void )
{
// Compile a statement after a logical IF or AT END.
// Do not be alarmed by the name of this routine. The recursion is
// controlled so that it may only happen once due to the fact that
// another IF or AT END cannot follow the first one.

    unsigned_16 ctrlflgs;
    STMT        proc;

    CITNode->opr = OPR_TRM;
    ctrlflgs = CtrlFlgs;
    proc = StmtProc;
    SPtr1 = SPtr2;
    GetStmtType();
    SetCtrlFlgs();
    if( CtrlFlgOn( CF_NOT_SIMPLE_STMT ) ) { // controls recursion
        StmtPtrErr( ST_NOT_ALLOWED, StmtKeywords[ proc ] );
    } else {
        ProcStmt();
        ClearRem();
        TDStmtFini();
        if( CtrlFlgOn( CF_NOT_SIMPLE_STMT | CF_NOT_EXECUTABLE ) ) {
            StmtPtrErr( ST_NOT_ALLOWED, StmtKeywords[ proc ] );
        }
    }
    StmtProc = proc;
    CtrlFlgs = ctrlflgs;
}


static bool CharSubStrung( void )
{
    bool        substrung;
    itnode      *cit;

    cit = CITNode;
    AdvanceITPtr();                     // step up to the OPR_LBR
    substrung = SubStrung();
    CITNode = cit;
    return( substrung );
}

static void GetStmtType( void )
{
    char        *curr_opnd;
    OPR         opr;

    curr_opnd = CITNode->opnd;
    if( CITNode->opn.ds != DSOPN_NAM ) {
        Error( ST_WANT_NAME );
        StmtProc = PR_NULL;
    } else if( ( *curr_opnd == 'D' ) && ( *(curr_opnd + 1) == 'O' ) &&
               (StmtSw & (SS_EQ_THEN_COMMA | SS_COMMA_THEN_EQ)) ) {
        StmtProc = PR_DO;
        RemKeyword( CITNode, 2 );
    } else if( ( *curr_opnd == 'I' ) && ( *(curr_opnd+1) == 'F' ) &&
               ( CITNode->link->opr == OPR_LBR ) && ( SPtr1 != NULL ) &&
               ( ( SPtr1->opn.ds == DSOPN_NAM ) || ( SPtr1->opn.ds == DSOPN_INT ) ) ) {
        if( SPtr1->opn.ds == DSOPN_INT ) {
            StmtProc = PR_ARIF;
        } else {
            StmtProc = PR_IF;
        }
        RemKeyword( CITNode, 2 );
    } else if( ( (StmtSw & SS_COMMA_THEN_EQ) == 0 ) && (StmtSw & SS_EQUALS_FOUND) ) {
        StmtProc = PR_ASNMNT;
        if( RecName() && RecNextOpr( OPR_LBR ) ) {
            if( SPtr1->opn.ds != DSOPN_PHI ) {
                DefStmtType();
            } else {
                opr = SPtr1->link->opr;
                if( opr != OPR_LBR && opr != OPR_DPT && opr != OPR_FLD ) {
                    if( opr != OPR_EQU ) {
                        DefStmtType();
                    } else {
                        LkSym();
                        if( !BitOn( SY_USAGE | SY_SUB_PARM ) &&
                            !CharSubStrung() ) {
                            StmtProc = PR_STMTFUNC;
                        }
                    }
                }
            }
        }
    } else {
        DefStmtType();
    }
}


static void SetCtrlFlgs( void )
{
    CtrlFlgs = CFTable[ StmtProc ];
}

static void DefStmtType( void )
{
    StmtProc = RecStmtKW();      // look up keyword, strip off if found
    if( StmtProc != 0 ) {
        RemKeyword( CITNode, strlen( StmtKeywords[ StmtProc ] ) );
    }
}

static void RemCheck( void )
{
// Check for errors or warnings concerning the dependencies
// of this statement on the last one. eg. READ before ATEND.

    if( StmtProc == PR_ATEND) {
        if( Remember.read ) {
            if( Remember.end_equals ) {
                Error( SP_ATEND_AND_ENDEQUALS );
            }
        } else {
            Error( SP_READ_NO_ATEND );
        }
    }
    if( Remember.transfer && ( StmtNo == 0 ) &&
        !CtrlFlgOn( CF_BAD_BRANCH_OBJECT | CF_IMPLICIT_LABEL ) &&
        ( !Remember.stop_or_return || ( StmtProc != PR_END ) ) ) {
        Warning( ST_NO_EXEC );
    }
    if( Remember.slct && ( StmtProc != PR_CASE ) ) {
        Error( SP_SELECT_THEN_CASE );
    }
}

void ClearRem( void )
{
// Clear the Remember flags for a new statement.

    Remember.read           = false;
    Remember.end_equals     = false;
    Remember.transfer       = false;
    Remember.slct           = false;
    Remember.endstmt        = false;
    Remember.stop_or_return = false;
}


static void CheckOrder( void )
{
    AError = false;
    if( (StmtProc == PR_IMP) && (SgmtSw & SG_NO_MORE_IMPLICIT) ) {
        Error( ST_IMPLICIT_LATE );
    } else if( CtrlFlgOn( CF_SPECIFICATION ) && (SgmtSw & SG_NO_MORE_SPECS) ) {
        // ENTRY and DATA statements are special specification statements
        if( (StmtProc != PR_ENTRY) && (StmtProc != PR_DATA) ) {
            Error( ST_SPEC_LATE );
        }
    } else if( CtrlFlgOn( CF_SPECIFICATION ) && (SgmtSw & SG_SEEN_DATA) ) {
        if( ( StmtProc != PR_DATA ) && (ExtnSw & XS_DATA_STMT_ORDER) == 0 ) {
            Extension( ST_DATA_TOO_EARLY );
            ExtnSw |= XS_DATA_STMT_ORDER;
        }
    } else if( (StmtProc == PR_STMTFUNC) && (SgmtSw & SG_NO_MORE_SF) ) {
        Error( ST_ASF_LATE );
        AError = false; // so we still process statement function
    }
    if( !CtrlFlgOn( CF_SUBPROGRAM ) && (StmtProc != PR_FMT) &&
        (StmtProc != PR_INCLUDE) && (StmtProc != PR_ENTRY) &&
        (StmtProc != PR_PARM) && (StmtProc != PR_DATA) &&
        (StmtProc != PR_IMP) ) {
        SgmtSw |= SG_NO_MORE_IMPLICIT;
        if( !CtrlFlgOn( CF_SPECIFICATION ) ) {
            SgmtSw |= SG_NO_MORE_SPECS;
            if( SgmtSw & SG_DEFINING_STRUCTURE ) {
                Error( SP_UNFINISHED, StmtKeywords[ PR_STRUCTURE ] );
                SgmtSw &= ~SG_DEFINING_STRUCTURE;
                AError = false; // so we still process the statement
            }
            if( StmtProc != PR_STMTFUNC ) {
                SgmtSw |= SG_NO_MORE_SF;
            }
        }
    }
}


void CpBadStmt( void )
{
    // consider:
    //          suborutine x() ! misspelled subroutine
    //          integer z
    //          ....
    //          end
    // since the first statement is undecodable, SubProgId will never be
    // set.  This causes problems when generating browsing information and
    // in the code generation routines in the load'n go compiler.
    // If the first statement is garbage, we call DefProd() to set SubProgId
    // to the default program name.
    if( SubProgId == NULL ) {
        DefProg();
    }
    OpndErr( ST_UNKNOWN_STMT );
}

static void CheckDoEnd( void )
{
    csnode      *cs_node;

    for(;;) {
        if( CSHead->typ == CS_DO ) {
            if( CSHead->cs_info.do_parms->do_term != StmtNo ) break;
        } else if( CSHead->typ == CS_DO_WHILE ) {
            if( CSHead->cs_info.do_term != StmtNo ) break;
        } else {
            break;
        }
        FiniDo();
    }
    cs_node = CSHead;
    while( cs_node != NULL ) {
        if( cs_node->typ == CS_DO ) {
            if( cs_node->cs_info.do_parms->do_term == StmtNo ) {
                Error( DO_NESTING_BAD );
            }
        } else if( cs_node->typ == CS_DO_WHILE ) {
            if( cs_node->cs_info.do_term == StmtNo ) {
                Error( DO_NESTING_BAD );
            }
        }
        cs_node = cs_node->link;
    }
}


static void FiniDo( void )
{
    if( ( StmtProc != 0 ) && CtrlFlgOn( CF_BAD_DO_ENDING ) ) {
        StmtErr( DO_ENDING_BAD );
    }
    if( CSHead->typ == CS_DO ) {
        TermDo();
    } else {
        TermDoWhile();
    }
}

void RemKeyword( itnode *itptr, int remove_len )
{
    char        *curr_char;
    itnode      *new_it_node;
    int         curr_size;

    itptr->oprpos += remove_len;
    itptr->opnpos = itptr->oprpos;
    if( itptr->opnd_size <= remove_len ) {
        new_it_node = itptr->link;
        if( new_it_node->opr == OPR_PHI ) {
            itptr->link = new_it_node->link;
            itptr->opn = new_it_node->opn;
            itptr->opnpos = new_it_node->opnpos;
            itptr->opnd_size = new_it_node->opnd_size;
            itptr->opnd = new_it_node->opnd;
            FreeOneNode( new_it_node );
        } else {
            itptr->opnd_size = 0;
            itptr->opn.ds = DSOPN_PHI;
        }
    } else {
        itptr->opnd += remove_len;
        itptr->opnd_size -= remove_len;
        curr_char = itptr->opnd;
        curr_size = itptr->opnd_size;
        for(;;) {
            if( curr_char == itptr->opnd + curr_size ) break;
            if( isdigit( *curr_char ) == 0 ) break;
            curr_char++;
        }
        if( curr_char != itptr->opnd ) {
            itptr->opn.ds = DSOPN_INT;
            if( curr_char - itptr->opnd != curr_size ) {
                remove_len = curr_size - ( curr_char - itptr->opnd );
                itptr->opnd_size = ( curr_char - itptr->opnd );
                new_it_node = FrlAlloc( &ITPool, sizeof( itnode ) );
                new_it_node->opnd_size = remove_len;
                new_it_node->opnd = itptr->opnd + itptr->opnd_size;
                new_it_node->opn.ds = DSOPN_NAM;
                new_it_node->opr = OPR_PHI;
                new_it_node->oprpos = itptr->opnpos + itptr->opnd_size;
                new_it_node->opnpos = new_it_node->oprpos;
                new_it_node->flags = 0;
                new_it_node->list = itptr->list;
                new_it_node->link = itptr->link;
                new_it_node->typ = FT_NO_TYPE;
                itptr->link = new_it_node;
            }
        }
    }
}

