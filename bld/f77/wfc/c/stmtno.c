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
// STMTNO    : routines to handle all aspects of statement numbers
//

#include "ftnstd.h"
#include "errcod.h"
#include "ctrlflgs.h"
#include "global.h"
#include "intcnv.h"
#include "cpopt.h"
#include "ferror.h"
#include "insert.h"
#include "rststmt.h"
#include "cgmagic.h"
#include "fcodes.h"
#include "gflow.h"
#include "fmtcnvt.h"


extern  bool            RecNumber(void);

//  NOTE : These modules will check for errors and give any error
//         messages relating to statement numbers.
//         If you wish to have a label to branch to, use LkUpStmtNo().
//         If you need the label of a format statement, use LkUpFormat().
//
//  Explanation :
//     The method used to detect branches into or between control
//     structures is as follows:
//        assigned a block number. the number is incremented every time,
//        therefore each block has a unique number
//      - when a statement number is first encountered the block number
//        it occurred in (at head of CS list) is remembered on the
//        statement number tree
//      - if the definition occurs before the branch ( i.e. DefStmtNo is
//        invoked before LkStmtNo ) the block the definition occurred in
//        must still be unfinished ( i.e. be in the CS list ) when the
//        branch is encountered
//      - if the branch occurs before the definition the block number
//        the definition occurs in must less than or equal to the block
//        number where the branch occurred ( i.e. the current block had
//        begun before the branch occurred )
//


intstar4        GetStmtNo( void ) {
//===========================

// Create statement number from operand field of CITNode must be before
// routines who call this routine since it returns unsigned_32.

    intstar4    num;

    if( RecNumber() ) {
        if( ( FmtS2I( CITNode->opnd, CITNode->opnd_size, false, &num, false, NULL ) == INT_OK ) && ( num <= 99999 ) ) {
            if( num == 0 ) {
                Error( ST_NUM_ZERO );
            }
        } else {
            Error( ST_NUM_TOO_BIG );
            num = 0;
        }
    } else {
        Error( ST_NO_STMTNO );
        num = 0;
    }
    return( num );
}


sym_id  LookUp( unsigned_32 num ) {
//=================================

    sym_id      sym_ptr;

    sym_ptr = NULL;
    if( num != 0 ) {
        sym_ptr = STStmtNo( num );
        if( ( sym_ptr->u.st.flags & SN_INIT_MASK ) == SN_INIT ) {
            sym_ptr->u.st.line = SrcRecNum;
        }
    }
    return( sym_ptr );
}


void    Err( int errcod, sym_id sym_ptr ) {
//=========================================

    Error( errcod, GetStmtNum( sym_ptr ), sym_ptr->u.st.line );
}


sym_id  LkUpStmtNo( void ) {
//====================

// Return the branch label of a statement number.

    sym_id      sym_ptr;
    unsigned_16 flags;
    csnode      *csptr;

    sym_ptr = LookUp( GetStmtNo() );
    if( sym_ptr != NULL ) {
        sym_ptr->u.st.ref_count++;
        flags = sym_ptr->u.st.flags;
        if( GetStmtNum( sym_ptr ) == StmtNo ) {
            Warning( ST_TO_SELF );     // this is only a warning
        }                              // eg. 10 IF( FNX( Y ) )10,20,30
                                       // will not necessarily be infinite
        if( ( flags & SN_DEFINED ) != 0 ) {
            if( ( flags & SN_BAD_BRANCH ) != 0 ) {
                Err( ST_CANNOT_BRANCH, sym_ptr );
            }
            csptr = CSHead;
            for(;;) {
                if( sym_ptr->u.st.block == csptr->block ) break;
                csptr = csptr->link;
                if( csptr == NULL ) break;
            }
            if( csptr == NULL ) {
                if( !(Options & OPT_WILD) ) {
                    Err( SP_INTO_STRUCTURE, sym_ptr );
                }
            }
            if( StNumbers.in_remote && ( ( flags & SN_IN_REMOTE ) == 0 ) ) {
                Err( SP_OUT_OF_BLOCK, sym_ptr );
            }
        } else {
            if( ( sym_ptr->u.st.block > CSHead->block ) ||
                ( ( flags & SN_BRANCHED_TO ) == 0 ) ||
                // Consider:      DO 10 I==1,2
                //                  GOTO 10
                //                  DO 10 J==1,2
                //           10  CONTINUE
                // When we compile "GOTO 10", 10 is no longer only
                // a DO terminal statement number.
                ( flags & SN_ONLY_DO_TERM ) ) {
                sym_ptr->u.st.flags &= ~SN_ONLY_DO_TERM;
                sym_ptr->u.st.block = CSHead->block;
                sym_ptr->u.st.line = SrcRecNum;
            }
            if( StNumbers.in_remote ) {
                sym_ptr->u.st.flags |= SN_IN_REMOTE;
            }
        }
        sym_ptr->u.st.flags |= SN_BRANCHED_TO;
    }
    return( sym_ptr );
}


sym_id  LkUpFormat( void ) {
//====================

// Return the label of a format statement number.

    sym_id      sym_ptr;

    sym_ptr = LookUp( GetStmtNo() );
    if( sym_ptr != NULL ) {
        if( ( sym_ptr->u.st.flags & SN_DEFINED ) != 0 ) {
            if( ( sym_ptr->u.st.flags & SN_FORMAT ) == 0 ) {
                Err( ST_NOT_FORMAT, sym_ptr );
            }
        } else {
            sym_ptr->u.st.flags |= ( SN_FORMAT | SN_BAD_BRANCH );
        }
    }
    return( sym_ptr );
}


sym_id  FmtPointer( void ) {
//====================

// Return the label of the current statement number.

    sym_id      sym_ptr;

    sym_ptr = LookUp( StmtNo );
    if( sym_ptr == NULL ) {
        Warning( FM_NO_STMTNO );
    }
    return( sym_ptr );
}


sym_id  LkUpAssign( void ) {
//====================

// Return label for ASSIGN 10 TO I.

    sym_id      sym_ptr;

    sym_ptr = LookUp( GetStmtNo() );
    if( sym_ptr != NULL ) {
        sym_ptr->u.st.ref_count++;
        sym_ptr->u.st.flags |= SN_ASSIGNED;
        if( ( sym_ptr->u.st.flags & SN_DEFINED ) &&
            ( ( sym_ptr->u.st.flags & SN_FORMAT ) == 0 ) &&
            ( sym_ptr->u.st.flags & SN_BAD_BRANCH ) ) {
            Err( GO_CANNOT_ASSIGN, sym_ptr );
            sym_ptr->u.st.flags &= ~SN_ASSIGNED;
        }
    }
    return( sym_ptr );
}


unsigned_32     LkUpDoTerm( void ) {
//============================

// Look up a statement numbers for a do terminator.

    sym_id      sym_ptr;
    unsigned_16 flags;
    unsigned_32 num;

    num = GetStmtNo();
    if( num != 0 ) {
        sym_ptr = LookUp( num );
        flags = sym_ptr->u.st.flags;
        if( ( flags & SN_DEFINED ) != 0 ) {
            Err( DO_BACKWARDS_DO, sym_ptr );
        }
        if( ( flags & SN_ONLY_DO_TERM ) != 0 ) {
            sym_ptr->u.st.block = CSHead->block;
            sym_ptr->u.st.line = SrcRecNum;
        } else if( ( flags & (SN_INIT_MASK & ~SN_ASSIGNED) ) == SN_INIT ) {
            // Assert: SN_ASSIGNED is only bit that might be set

            // Consider: ASSIGN 10 to I
            //           DO 10
            //      10   CONTINUE
            // update the block number so we don't issue an SP_FROM_OUTSIDE
            // in Update() when we compile the CONTINUE statement
            if( flags & SN_ASSIGNED ) {
                sym_ptr->u.st.block = CSHead->block;
            }

            sym_ptr->u.st.flags |= SN_ONLY_DO_TERM;
        }
        sym_ptr->u.st.flags |= SN_BRANCHED_TO;
    }
    return( num );
}


void    DefStmtNo( unsigned_32 num ) {
//====================================

// Generate label for statement number.

    sym_id      sym_ptr;

    sym_ptr = LookUp( num );
    sym_ptr->u.st.ref_count++;
    if( ( sym_ptr->u.st.flags & SN_DEFINED ) != 0 ) {
        Err( ST_ALREADY, sym_ptr );
    } else {
        StNumbers.blk_before = CSHead->block;
        if( Remember.transfer ) {
            sym_ptr->u.st.flags |= SN_AFTR_BRANCH;
        }
        if( StmtProc == PR_FMT ) {
            sym_ptr->u.st.flags |= SN_FORMAT;
        } else if( !CtrlFlgOn( CF_BAD_BRANCH_OBJECT ) ) {
            GStmtLabel( sym_ptr );
        }
        if( StNumbers.in_remote ) {
            sym_ptr->u.st.flags |= SN_IN_REMOTE;
        }
    }
}



void    Update( unsigned_32 num ) {
//=================================

// Update the statement number after compiling the statement.

    sym_id      sym;
    unsigned_16 block;

    sym = LookUp( num );
    if( ( sym->u.st.flags & SN_DEFINED ) == 0 ) {
        sym->u.st.flags |= SN_DEFINED;
        block = StNumbers.blk_before; // allow branch to start of block
        if( block > CSHead->block ) { // end of structure occurred
            block = CSHead->block; // allow branch to end of block
        }
        if( ( sym->u.st.flags & SN_BRANCHED_TO ) != 0 ) {
            if( sym->u.st.block < block ) {
                if( !(Options & OPT_WILD) ) {
                    Err( SP_FROM_OUTSIDE, sym );
                }
            }
            if( !StNumbers.in_remote &&
                ( ( sym->u.st.flags & SN_IN_REMOTE ) != 0 ) ) {
                Err( SP_OUT_OF_BLOCK, sym );
            }
        }
        sym->u.st.block = block;
    }
    if( CtrlFlgOn( CF_BAD_BRANCH_OBJECT ) ) {
        sym->u.st.flags |= SN_BAD_BRANCH;
        if( ( sym->u.st.flags & SN_BRANCHED_TO ) != 0 ) {
            StmtIntErr( ST_BAD_BRANCHED, sym->u.st.line );
        }
        sym->u.st.flags &= ~SN_AFTR_BRANCH;
        if( ( sym->u.st.flags & SN_ASSIGNED ) && ( StmtProc != PR_FMT ) ) {
            StmtIntErr( GO_ASSIGNED_BAD, sym->u.st.line );
        }
    }
    if( !CtrlFlgOn( CF_NOT_EXECUTABLE ) ) {
        sym->u.st.flags |= SN_EXECUTABLE;
    }
    if( StmtProc != PR_FMT ) {
        if( ( sym->u.st.flags & SN_FORMAT ) != 0 ) {
            Err( ST_EXPECT_FORMAT, sym );
        }
    }
    sym->u.st.line = SrcRecNum;
}


void    InitStNumbers( void ) {
//=======================

// Intitialize statement number processing.

    StNumbers.wild_goto  = false;
    StNumbers.var_format = false;
    StNumbers.in_remote  = false;
    StNumbers.branches   = NextLabel();
}
