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
// CSGUESS   : processing for GUESS, ADMIT, and ENDGUESS
//

#include "ftnstd.h"
#include "errcod.h"
#include "global.h"
#include "proctbl.h"
#include "csutls.h"
#include "cgmagic.h"
#include "fcodes.h"
#include "gflow.h"


void    CpGuess(void) {
//=================

// Process a GUESS statement.

    CSExtn();
    AddCSNode( CS_GUESS );
    CSHead->branch = NextLabel();
    CSHead->bottom = NextLabel();
    BlockLabel();
}


void    CpAdmit(void) {
//=================

// Process an ADMIT statement.

    if( ( CSHead->typ == CS_GUESS ) | ( CSHead->typ == CS_ADMIT ) ) {
        GBranch( CSHead->bottom );
        GLabel( CSHead->branch );
        FreeLabel( CSHead->branch );
        CSHead->typ = CS_ADMIT;
        CSHead->branch = NextLabel();
        CSHead->block = ++BlockNum;
    } else {
        Match();
    }
    CSNoMore();
}


void    CpEndGuess(void) {
//====================

// Process an ENDGUESS statement.

    if( ( CSHead->typ == CS_GUESS ) | ( CSHead->typ == CS_ADMIT ) ) {
        GLabel( CSHead->branch );
        GLabel( CSHead->bottom );
        FreeLabel( CSHead->branch );
        FreeLabel( CSHead->bottom );
    } else {
        Match();
    }
    DelCSNode();
    CSNoMore();
}
