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


global  void (intern *DrawCursor)(void) = NULL;
global  void (intern *EraseCursor)(void) = NULL;


#define OFF_SCREEN      200

extern MOUSEORD MouseRow, MouseCol;

static MOUSEORD OldMouseRow, OldMouseCol = OFF_SCREEN;
static bool     MouseOn = FALSE;
static ATTR     OldAttr;
static int      ColAdjust;

static char __FAR *RegenPos( unsigned row, unsigned col )
/*******************************************************/
{
    char        __FAR *pos;
    char        __FAR *col0;

    col0 = (char __FAR *)UIData->screen.origin
          + (row*UIData->screen.increment)*sizeof(PIXEL);
    pos = col0 + col*sizeof( PIXEL );
    while( col0 < pos ) {
        col0 += uicharlen( *col0 ) * sizeof( PIXEL );
    }
    if( col0 != pos ) {
        ColAdjust = -1;
        pos -= sizeof( PIXEL ); // put on char boundary
    } else {
        ColAdjust = 0;
    }
    vertretrace();
    return( pos );
}

static void uisetmouseoff()
/*************************/
{
    char                __FAR *old;
    SAREA               area;

    if( MouseOn ) {

        if( EraseCursor==NULL ) {
            old = RegenPos( OldMouseRow, OldMouseCol );
            if( uicharlen( *old ) == 2 ) {
                old[3] = OldAttr;
            }
            old[1] = OldAttr;
            area.row  = OldMouseRow;
            area.col = OldMouseCol + ColAdjust;
            area.height = 1;
            area.width = 1 - ColAdjust;
            physupdate( &area );
//          physupdate( ( old - (char __FAR *)UIData->screen.origin ), 1 );
        } else{
            (*EraseCursor)();               /*  Hide text-graphics mouse    */
        }
    }
}

static void FlipAttr( char __FAR *p )
{
    OldAttr = *p;
    if( UIData->colour == M_MONO ){
        *p = (OldAttr & 0x79) ^ 0x71;
    } else {
        *p = (OldAttr & 0x7f) ^ 0x77;
    }
}

static void uisetmouseon( row, col )
/********************************/
    register MOUSEORD        row;
    register MOUSEORD        col;
{
    char                __FAR *new;
    SAREA               area;

    if( MouseOn ){
        if( DrawCursor==NULL ) {
            new = RegenPos( row, col );
            if( uicharlen( *new ) == 2 ) {
                FlipAttr( new+3 );
            }
            FlipAttr( new+1 );
            area.row = row;
            area.col = col + ColAdjust;
            area.width = 1 - ColAdjust;
            area.height = 1;
            physupdate( &area );
//          physupdate( ( new - (char __FAR *)UIData->screen.origin ), 1 );
        } else {
            (*DrawCursor)();
        }
        OldMouseRow = row;
        OldMouseCol = col;
    }
}

int global uimousealign()
/***********************/
{
    return( ColAdjust );
}

void global uisetmouse( row, col )
/********************************/
    register MOUSEORD        row;
    register MOUSEORD        col;
{
    if( OldMouseRow == row && OldMouseCol == col ) return;
    uisetmouseoff();
    uisetmouseon( row, col );
}


void global uimouse( func )
/*************************/
{
    if( func == MOUSE_ON ) {
        MouseOn = TRUE;
        uisetmouseon( OldMouseRow, OldMouseCol );
    } else {
        uisetmouseoff();
        MouseOn = FALSE;
    }
}
