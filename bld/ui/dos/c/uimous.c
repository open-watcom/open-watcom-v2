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
* Description:  DOS mouse input handling.
*
****************************************************************************/

#include <dos.h>
#include "uidef.h"
#include "uidos.h"
#include "biosui.h"
#include "uimouse.h"
#include "uibmous.h"


#define MOUSE_SCALE     8

unsigned short          Points;         /* Number of lines per character */

/* MickeyRow and MickeyCol are accurate under DOS and OS2's DOS */
static int              MickeyRow;
static int              MickeyCol;

void intern checkmouse( MOUSESTAT *status, MOUSEORD *row, MOUSEORD *col, MOUSETIME *time )
/****************************************************************************************/
{
    struct mouse_data   state;
    char                change;

    MouseDrvCallRetState( 3, &state );

    *status = state.bx;

    if( DrawCursor == NULL ) {
        *col = state.cx / MOUSE_SCALE;
        *row = state.dx / MOUSE_SCALE;
    } else {
        MouseDrvCallRetState( 0x0B, &state );
        MickeyCol += (short int)state.cx; /* delta of mickeys */
        MickeyRow += (short int)state.dx; /* delta of mickeys */
        if( MickeyRow < 0 ) {
            MickeyRow = 0;
            change = true;
        } else {
            /* -2 makes sure one pixel of mouse shows */
            if( MickeyRow > ( UIData->height * Points - 2 ) ) {
                MickeyRow = UIData->height * Points - 2;
                change = true;
            } else {
                change = false;
            }
        }
        if( MickeyCol < 0 ) {
            MickeyCol = 0;
            change = true;
        } else {
            /* -2 makes sure one pixel of mouse shows */
            if( MickeyCol > ( UIData->width * MOUSE_SCALE - 2 ) ) {
                MickeyCol = UIData->width * MOUSE_SCALE - 2;
                change = true;
            } else {
                change = false;
            }
        }
        *row = MickeyRow;
        *col = MickeyCol;
        if( change ) {
            MouseDrvCall2( 4, 0, *col, *row );
            MouseDrvCall1( 0x0B );
        }
    }

    *time = uiclock();
    uisetmouse( *row, *col );
    *col += uimousealign();
}


void uimousespeed( unsigned speed )
/*********************************/
/* set speed of mouse. 0 is fastest; the higher the number the slower
   it goes */
{
    if( speed == 0 ) {
        speed = 1;
    }

    MouseDrvCall3( 0x0F, speed, speed * 2, 0, 0 );
    UIData->mouse_speed = speed;
}

void intern setupmouse( void )
{
    int     dx;

    if( DrawCursor == NULL ) {
        dx = ( UIData->width - 1 ) * MOUSE_SCALE;
    } else {
        dx = UIData->width * MOUSE_SCALE - 1;
    }
    MouseDrvCall2( 7, 0, 0, dx );

    if( DrawCursor == NULL ) {
        dx = ( UIData->height - 1 ) * MOUSE_SCALE;
    } else {
        dx = UIData->height * Points - 1;
    }
    MouseDrvCall2( 8, 0, 0, dx );

    uisetmouseposn( UIData->height / 2 - 1, UIData->width / 2 - 1 );
    MouseInstalled = true;
    MouseOn = false;
    UIData->mouse_swapped = false;
    checkmouse( &MouseStatus, &MouseRow, &MouseCol, &MouseTime );
    if( DrawCursor != NULL )
        UIData->mouse_speed /= 2;
    uimousespeed( UIData->mouse_speed );
}

bool UIAPI initmouse( init_mode install )
/***************************************/
{
    MouseInstalled = false;
    if( install > INIT_MOUSELESS && mouse_installed() ) {
        if( install > INIT_MOUSE ) {
            if( MouseDrvReset() != MOUSE_DRIVER_OK ) {
                install = INIT_MOUSELESS;   /* mouse initialization failed */
            }
        }
        if( install > INIT_MOUSELESS ) {
            UIData->mouse_yscale = 1;
            UIData->mouse_xscale = 1;
            setupmouse();
        }
    }
    return( MouseInstalled );
}


void UIAPI finimouse( void )
/**************************/
{
    if( MouseInstalled ) {
        uioffmouse();
    }
}

void UIAPI uisetmouseposn( ORD row, ORD col )
/*******************************************/
{
    MouseRow = row * UIData->mouse_yscale;
    MouseCol = col * UIData->mouse_xscale;
    if( DrawCursor == NULL ) {
        MouseDrvCall2( 4, 0, col * MOUSE_SCALE, row * MOUSE_SCALE );
    } else {
        MouseDrvCall2( 4, 0, MouseCol, MouseRow );
        MouseDrvCall1( 0x0B );
        MickeyRow = MouseRow; /* initialize these and syncronize the INT B */
        MickeyCol = MouseCol; /* because we keep a running total */
    }
}
