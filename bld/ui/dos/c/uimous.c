/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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
#include "int33.h"
#include "uimouse.h"
#include "uibmous.h"


unsigned short          Points;         /* Number of lines per character */

/* MickeyRow and MickeyCol are accurate under DOS and OS2's DOS */
static int              MickeyRow;
static int              MickeyCol;

void intern checkmouse( MOUSESTAT *status, MOUSEORD *row, MOUSEORD *col, MOUSETIME *time )
/****************************************************************************************/
{
    mouse_status    state;
    char            change;

    _BIOSMouseGetPositionAndButtonStatus( &state );

    *status = state.button_status;

    if( DrawCursor == NULL ) {
        *col = state.x / MOUSE_SCALE;
        *row = state.y / MOUSE_SCALE;
    } else {
        _BIOSMouseGetMotionCounters( &state );
        MickeyCol += (short)state.x; /* delta of mickeys */
        MickeyRow += (short)state.y; /* delta of mickeys */
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
            _BIOSMouseSetPointerPosition( *col, *row );
            _BIOSMouseGetMotionCountersReset();
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

    _BIOSMouseSetMickeysToPixelsRatio( speed, speed * 2 );
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
    _BIOSMouseSetHorizontalLimitsForPointer( 0, dx );

    if( DrawCursor == NULL ) {
        dx = ( UIData->height - 1 ) * MOUSE_SCALE;
    } else {
        dx = UIData->height * Points - 1;
    }
    _BIOSMouseSetVerticalLimitsForPointer( 0, dx );

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
    if( install != INIT_MOUSELESS && mouse_installed() ) {
        if( install == INIT_MOUSE_INITIALIZED ) {
            if( _BIOSMouseDriverReset() != MOUSE_DRIVER_OK ) {
                install = INIT_MOUSELESS;   /* mouse initialization failed */
            }
        }
        if( install != INIT_MOUSELESS ) {
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
        _BIOSMouseSetPointerPosition( col * MOUSE_SCALE, row * MOUSE_SCALE );
    } else {
        _BIOSMouseSetPointerPosition( MouseCol, MouseRow );
        _BIOSMouseGetMotionCountersReset();
        MickeyRow = MouseRow; /* initialize these and syncronize the INT B */
        MickeyCol = MouseCol; /* because we keep a running total */
    }
}
