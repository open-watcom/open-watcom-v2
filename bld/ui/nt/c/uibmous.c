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


#include "uidef.h"
#include "uimouse.h"


#define OFF_SCREEN      200

extern MOUSEORD MouseRow, MouseCol;

static MOUSEORD oldMouseRow, oldMouseCol = OFF_SCREEN;
static bool     mouseOn = FALSE;
#if 0
static ATTR     OldAttr;

static LP_STRING RegenPos( unsigned row, unsigned col )
/*****************************************************/
{
    LP_STRING   pos;

    pos = (LP_STRING)UIData->screen.origin
          + (row*UIData->screen.increment+col)*sizeof(PIXEL) + 1;
    vertretrace();
    return( pos );
}
#endif

static void uisetmouseoff( void )
/*******************************/
{
//    LP_STRING   old;

    if( mouseOn ) {
#if 0
        old = RegenPos( oldMouseRow, oldMouseCol );
        *old = OldAttr;
        {
            SAREA       area;
            area.row = oldMouseRow;
            area.col = oldMouseCol;
            area.height = 1;
            area.width = 1;
            physupdate( &area );
        }
#endif
    }
}

static void uisetmouseon( MOUSEORD row, MOUSEORD col )
/****************************************************/
{
//    LP_STRING   new;

    if( mouseOn ){
#if 0
        new = RegenPos( row, col );
        OldAttr = *new;
        if( UIData->colour == M_MONO ){
            *new = (OldAttr & 0x79) ^ 0x71;
        } else {
            *new = (OldAttr & 0x7f) ^ 0x77;
        }
        {
            SAREA       area;
            area.row = row;
            area.col = col;
            area.height = 1;
            area.width = 1;
            physupdate( &area );
        }
#endif
        oldMouseRow = row;
        oldMouseCol = col;
    }
}



void UIAPI uisetmouse( MOUSEORD row, MOUSEORD col )
/**************************************************/
{
    if( oldMouseRow == row && oldMouseCol == col ) return;
    uisetmouseoff();
    uisetmouseon( row, col );
}


void UIAPI uimouse( int func )
/*****************************/
{
    if( func == MOUSE_ON ) {
        mouseOn = TRUE;
        uisetmouseon( oldMouseRow, oldMouseCol );
    } else {
        uisetmouseoff();
        mouseOn = FALSE;
    }
}
