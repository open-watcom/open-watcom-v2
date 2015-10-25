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
// CSIF      : processing for the block and logical IF
//

#include "ftnstd.h"
#include "errcod.h"
#include "ctrlflgs.h"
#include "opn.h"
#include "opr.h"
#include "global.h"
#include "recog.h"
#include "ferror.h"
#include "utility.h"
#include "compstmt.h"
#include "proctbl.h"
#include "csutls.h"
#include "fcodes.h"
#include "gflow.h"
#include "cgmagic.h"



void    CpLogIf(void) {
//=================

// Process a logical IF statement.

    label_id    if_skip;

    if_skip = NextLabel();
    CSCond( if_skip );
    if( RecKeyWord( "THEN" ) &&
        ( RecNextOpr( OPR_TRM ) || RecNextOpr( OPR_COL ) ) ) {
        AddCSNode( CS_IF );
        CSHead->branch = if_skip;
        CSHead->bottom = NextLabel();
        CITNode->opn.ds = DSOPN_PHI;       // not part of the block label
        BlockLabel();
        CtrlFlgs |= CF_BAD_DO_ENDING;
    } else {
        Recurse();
        GLabel( if_skip );
        FreeLabel( if_skip );
    }
}


void    CpElseIf(void) {
//==================

// Process an ELSEIF statement.

    if( ( CSHead->typ == CS_IF ) || ( CSHead->typ == CS_ELSEIF ) ) {
        GBranch( CSHead->bottom );
        GLabel( CSHead->branch );
        FreeLabel( CSHead->branch );
        CSHead->typ = CS_ELSEIF;
        CSHead->branch = NextLabel();
        CSHead->block = ++BlockNum;
    } else if( CSHead->typ == CS_ELSE ) {
        Error( IF_ELSE_LAST );
    } else {
        Match();
    }
    CSCond( CSHead->branch );
    if( RecKeyWord( "THEN" ) ) {
        AdvanceITPtr();
        ReqEOS();
    } else {
        Error( IF_NO_THEN );
    }
}


void    CpElse(void) {
//================

// Process an ELSE statement.

    if( ( CSHead->typ == CS_IF ) || ( CSHead->typ == CS_ELSEIF ) ) {
        GBranch( CSHead->bottom );
        GLabel( CSHead->branch );
        FreeLabel( CSHead->branch );
        CSHead->typ = CS_ELSE;
        CSHead->block = ++BlockNum;
    } else if( CSHead->typ == CS_ELSE ) {
        Error( IF_ELSE_LAST );
    } else {
        Match();
    }
    CSNoMore();
}


void    CpEndif(void) {
//=================

// Process an ENDIF statement.

    if( ( CSHead->typ == CS_IF ) || ( CSHead->typ == CS_ELSEIF ) ) {
        GLabel( CSHead->branch );
        FreeLabel( CSHead->branch );
        GLabel( CSHead->bottom );
        FreeLabel( CSHead->bottom );
    } else if( CSHead->typ == CS_ELSE ) {
        GLabel( CSHead->bottom );
        FreeLabel( CSHead->bottom );
    } else {
        Match();
    }
    DelCSNode();
    CSNoMore();
}
