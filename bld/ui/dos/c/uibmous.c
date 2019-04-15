/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2018 The Open Watcom Contributors. All Rights Reserved.
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
#include "uibmous.h"


#define OFF_SCREEN      200

void (intern * DrawCursor)( void ) = NULL;
void (intern * EraseCursor)( void ) = NULL;

static MOUSEORD OldMouseRow;
static MOUSEORD OldMouseCol = OFF_SCREEN;
static bool     mouseOn = false;
static ATTR     OldAttr;
static int      ColAdjust;

static LP_PIXEL RegenPos( unsigned row, unsigned col )
/****************************************************/
{
    LP_PIXEL    pos;
    LP_PIXEL    col0;

    col0 = UIData->screen.origin + row * UIData->screen.increment;
    pos = col0 + col;
    while( col0 < pos ) {
        col0 += uicharlen( UCHAR_VALUE( col0->ch ) );
    }
    if( col0 != pos ) {
        ColAdjust = -1;
        pos--;          // put on char boundary
    } else {
        ColAdjust = 0;
    }
    vertretrace();
    return( pos );
}

static void uisetmouseoff( void )
/*******************************/
{
    LP_PIXEL    old;
    SAREA       area;

    if( mouseOn ) {
        if( EraseCursor == NULL ) {
            old = RegenPos( OldMouseRow, OldMouseCol );
            if( uicharlen( UCHAR_VALUE( old->ch ) ) == 2 ) {
                old[1].attr = OldAttr;
            }
            old[0].attr = OldAttr;
            area.row = OldMouseRow;
            area.col = OldMouseCol + ColAdjust;
            area.height = 1;
            area.width = 1 - ColAdjust;
            physupdate( &area );
//            physupdate( ( old - (LP_STRING)UIData->screen.origin ), 1 );
        } else{
            (*EraseCursor)();   /* Hide text-graphics mouse */
        }
    }
}

static void FlipAttr( LP_PIXEL p )
{
    OldAttr = p->attr;

    if( UIData->colour == M_MONO ) {
        p->attr = (OldAttr & 0x79) ^ 0x71;
    } else {
        p->attr = (OldAttr & 0x7f) ^ 0x77;
    }
}

static void uisetmouseon( MOUSEORD row, MOUSEORD col )
/****************************************************/
{
    LP_PIXEL    new;
    SAREA       area;

    if( mouseOn ) {
        if( DrawCursor == NULL ) {
            new = RegenPos( row, col );
            if( uicharlen( UCHAR_VALUE( new->ch ) ) == 2 ) {
                FlipAttr( new + 1 );
            }
            FlipAttr( new );
            area.row = row;
            area.col = col + ColAdjust;
            area.width = 1 - ColAdjust;
            area.height = 1;
            physupdate( &area );
//            physupdate( ( new - (LP_STRING)UIData->screen.origin ), 1 );
        } else {
            (*DrawCursor)();
        }
        OldMouseRow = row;
        OldMouseCol = col;
    }
}

int UIAPI uimousealign( void )
/****************************/
{
    return( ColAdjust );
}

void UIAPI uisetmouse( MOUSEORD row, MOUSEORD col )
/*************************************************/
{
    if( OldMouseRow == row && OldMouseCol == col )
        return;
    uisetmouseoff();
    uisetmouseon( row, col );
}


void UIAPI uimouse( mouse_func func )
/***********************************/
{
    if( func == MOUSE_ON ) {
        mouseOn = true;
        uisetmouseon( OldMouseRow, OldMouseCol );
    } else {
        uisetmouseoff();
        mouseOn = false;
    }
}
