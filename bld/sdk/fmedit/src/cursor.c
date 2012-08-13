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
* Description:  Change cursor shape according to state.
*
****************************************************************************/


#include <windows.h>

#include "fmedit.def"
#include "cursor.def"

enum {
    CURSOR_ARROW,
    CURSOR_CROSS,
    CURSOR_S_UD,
    CURSOR_S_LR,
    CURSOR_S_UR_BL,
    CURSOR_S_UL_BR,
    MAX_CURSORS
};


static HCURSOR Cursors[MAX_CURSORS];

static unsigned char StateCursor[] = {
    CURSOR_ARROW,       /* DORMANT       */
    0,                  /* OVERBOX       */
    CURSOR_CROSS,       /* MOVING        */
    CURSOR_ARROW,       /* EDITING       */
    0,                  /* SIZING        */
    CURSOR_ARROW,       /* CREATING      */
    CURSOR_ARROW,       /* ALIGNING      */
    CURSOR_CROSS,       /* PASTE_PENDING */
    CURSOR_CROSS,       /* PASTEING      */
    CURSOR_CROSS,       /* SELECTING     */
    CURSOR_CROSS        /* MOVE_PENDING  */
};

static unsigned char SizeCursor[] = {
    0,           CURSOR_S_UD,    CURSOR_S_UD,    0,
    CURSOR_S_LR, CURSOR_S_UL_BR, CURSOR_S_UR_BL, 0,
    CURSOR_S_LR, CURSOR_S_UR_BL, CURSOR_S_UL_BR
};


extern void InitCursors( void )
/*****************************/
{
    /* set up the various mouse cursors needed */
    Cursors[CURSOR_ARROW] = LoadCursor( NULL, IDC_ARROW );
    Cursors[CURSOR_CROSS] = LoadCursor( NULL, IDC_CROSS );
    Cursors[CURSOR_S_UD] =  LoadCursor( NULL, IDC_SIZENS );
    Cursors[CURSOR_S_LR] = LoadCursor( NULL, IDC_SIZEWE );
    Cursors[CURSOR_S_UR_BL] = LoadCursor( NULL, IDC_SIZENESW );
    Cursors[CURSOR_S_UL_BR] = LoadCursor( NULL, IDC_SIZENWSE );
}


extern void SetStateCursor( STATE_ID st )
/***************************************/
{
    /* set the cursor depending on the specified state */
    int cursoridx;

    if( st != SIZING && st != OVERBOX ) {
        cursoridx = StateCursor[st];
    } else {
        cursoridx = SizeCursor[GetSizing()];
    }
    SetCursor( Cursors[cursoridx] );
}
