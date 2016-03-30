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
// CSATEND   : processing for AT END and END AT END
//

#include "ftnstd.h"
#include "errcod.h"
#include "ctrlflgs.h"
#include "stmtsw.h"
#include "opn.h"
#include "rtconst.h"
#include "fcodes.h"
#include "global.h"
#include "recog.h"
#include "utility.h"
#include "compstmt.h"
#include "proctbl.h"
#include "csutls.h"
#include "cgmagic.h"
#include "gio.h"
#include "gflow.h"


void    CpAtEnd(void) {
//=================

// Process an ATEND statement.

    label_id    skip;
    label_id    code;

    CSExtn();
    skip = NextLabel();
    code = NextLabel();
    GBranch( skip );
    GCheckEOF( code );
    if( RecKeyWord( "DO" ) ) {
        AddCSNode( CS_ATEND );
        CSHead->bottom = skip;
        CITNode->opn.ds = DSOPN_PHI;       // not part of the block label
        BlockLabel();
        CtrlFlgs |= CF_BAD_DO_ENDING;
        GLabel( code );
        FreeLabel( code );
    } else {
        AdvanceITPtr();
        ReqComma();
        if( ( StmtSw & SS_COMMA_THEN_EQ ) != 0 ) {
            StmtSw &= ~SS_COMMA_THEN_EQ;  // consider AT END, DATA == 0
            StmtSw |= SS_EQUALS_FOUND;
        }
        GLabel( code );
        Recurse();
        GLabel( skip );
        FreeLabel( code );
        FreeLabel( skip );
    }
}


void    CpEndAtEnd(void) {
//====================

// Process an ENDATEND statement.

    if( CSHead->typ == CS_ATEND ) {
        GLabel( CSHead->bottom );
        FreeLabel( CSHead->bottom );
    } else {
        Match();
    }
    DelCSNode();
    CSNoMore();
}
