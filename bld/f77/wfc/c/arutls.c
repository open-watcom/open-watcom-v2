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
// ARUTLS    : utilities used in expression handling ( upscan, downscan )
//

#include "ftnstd.h"
#include "global.h"
#include "opr.h"
#include "opn.h"
#include "astype.h"
#include "recog.h"
#include "ferror.h"
#include "utility.h"
#include "arutls.h"


void    ScanExpr(void) {
//==================

// Advance CITNode to the end of the current expression.
//
//  Stops on: level zero comma
//            level zero colon
//            unmatched right parenthesis
//            terminator

    int         level;

    level = 0;
    for( ;; ) {
        if( RecOpenParen() ) {
            level++;
        } else if( RecCloseParen() ) {
            level--;
        }
        if( ( RecComma() || RecColon() ) && ( level == 0 ) )
            break;
        if( level < 0  )
            break;
        if( RecTrmOpr() )
            break;
        AdvanceITPtr();
    }
}

// At a given point in time during the upscan/downscan process, link
// fields from CITNode to the end of the list point forward, and all link
// fields from BkLink back to the start symbol of the expression point
// backward

void    MoveDown(void) {
//==================

// Move down the itlist, reverse the link field to point
// back to the precding itnode, and update BkLink.

    itnode      *nextnode;

    nextnode = CITNode->link;
    CITNode->link = BkLink;
    BkLink = CITNode;
    CITNode = nextnode;
}


void    BackTrack(void) {
//===================

// Move up the itlist, restoring the link field to point forward and
// updating BkLink.

    itnode      *oldcit;

    oldcit = CITNode;
    CITNode = BkLink;
    BkLink  = CITNode->link;
    CITNode->link = oldcit;
}


void    AdvError( int code ) {
//============================

// Advance CITNode, call error routine, reset CITNode.
// Use this routine to get error-pointer ( "carrot" ) in proper place

    MoveDown();
    Error( code );
    BackTrack();
}


void    KillOpnOpr(void) {
//====================

// jnneeded opn followed by unneeded opr in consecutive nodes so
// copy opr from node 1 down to node 2, then release node 1
//
//  CASE1: (not at start-node of expression)
//
//            ---------------               ---------------
//  Before:   | opr1 | opn1 |      After:       released
//            ---------------               ---------------
//     CIT==> | opr2 | opn2 |        CIT==> | opr1 | opn2 |
//            ---------------               ---------------
//
//  CASE2: (start-node of expression)
//         DO NOT release start-node since some unknown itnode points at
//         him
//
//            ---------------               ---------------
//  Before:   | TRM  | opn1 |      After:   | PHI  | PHI  |
//            ---------------               ---------------
//     CIT==> | opr2 | opn2 |        CIT==> | TRM  | opn2 |
//            ---------------               ---------------
//
//  In CASE1, top node is released since there may not be room to copy
//  operand up (variable length), but can easily copy operator-code down

    itnode      *itptr;

    CITNode->opr = BkLink->opr;                 // copy operator information
    CITNode->is_catparen = BkLink->is_catparen; // ...
    CITNode->oprpos = BkLink->oprpos;
    if( BkLink->opr == OPR_TRM ) {
        BkLink->opr = OPR_PHI;
        BkLink->link = CITNode;     // restore link to point forward
        BkLink = NULL;
    } else {
        itptr = BkLink;             // back up to previous entry
        BkLink = itptr->link;       // fix back link
        FreeOneNode( itptr );       // free single itnode
    }
}
