/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2020 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Windows mouse input handling.
*
****************************************************************************/


#include <dos.h>
#include "uidef.h"
#include <windows.h>
#include "uimouse.h"
#include "int33.h"
#include "uiwin.h"


static MOUSESTAT    MouseStatusBits;
static int          MouseX;
static int          MouseY;
static int          ScreenXFudge;
static int          ScreenYFudge;

void intern checkmouse( MOUSESTAT *status, MOUSEORD *row, MOUSEORD *col, MOUSETIME *time )
/****************************************************************************************/
{
    _BIOSMouseGetPositionAndButtonStatusReset();
    *status = MouseStatusBits;
    *col = MouseX;
    *row = MouseY;
    *time = uiclock();
    uisetmouse( *row, *col );
}

bool UIAPI initmouse( init_mode install )
/***************************************/
{
    int         cx;
    int         dx;

    MouseInstalled = false;
    ScreenXFudge = (WORD)( (DWORD)GetSystemMetrics( SM_CXSCREEN ) / (DWORD)UIData->width );
    ScreenYFudge = (WORD)( (DWORD)GetSystemMetrics( SM_CYSCREEN ) / (DWORD)UIData->height );
    if( install != INIT_MOUSELESS ) {
        dx = ( UIData->width - 1 ) * MOUSE_SCALE;
        _BIOSMouseSetHorizontalLimitsForPointer( 0, dx );
        dx = ( UIData->height - 1 ) * MOUSE_SCALE;
        _BIOSMouseSetVerticalLimitsForPointer( 0, dx );

        cx = ( UIData->colour == M_MONO ? 0x79ff : 0x7fff );
        dx = ( UIData->colour == M_MONO ? 0x7100 : 0x7700 );
        _BIOSMouseSetTextPointerType( SOFTWARE_CURSOR, cx, dx );
        _BIOSMouseSetPointerExclusionArea( 0, 0, 0, 0 );

        UIData->mouse_swapped = false;
        UIData->mouse_xscale = 1;
        UIData->mouse_yscale = 1;
        uisetmouseposn( UIData->height / 2 - 1, UIData->width / 2 - 1 );
        MouseInstalled = true;
        MouseOn = false;
        checkmouse( &MouseStatus, &MouseRow, &MouseCol, &MouseTime );
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

/*
 * Set mouse position
 */
void UIAPI uisetmouseposn( ORD row, ORD col )
{
    MouseRow = row;
    MouseCol = col;
//  _BIOSMouseSetPointerPosition( col * MOUSE_SCALE, row * MOUSE_SCALE );
    SetCursorPos( col * ScreenXFudge, row * ScreenYFudge );
}

/*
 * WindowsMouseEvent
 */
void WindowsMouseEvent( unsigned event, unsigned info )
{
    POINT p;

    info = info;    /* shut the compiler up */
    switch( event ) {
    case WM_MOUSEMOVE:
        GetCursorPos( &p );
        MouseX = (WORD)((DWORD)p.x / (DWORD)ScreenXFudge);
        if( MouseX > UIData->width - 1 )
            MouseX = UIData->width - 1;
        MouseY = (WORD)((DWORD)p.y / (DWORD)ScreenYFudge);
        if( MouseY > UIData->height - 1 )
            MouseY = UIData->height - 1;
        break;
    case WM_LBUTTONUP:
        MouseStatusBits &= ~UI_MOUSE_PRESS;
        break;
    case WM_RBUTTONUP:
        MouseStatusBits &= ~UI_MOUSE_PRESS_RIGHT;
        break;
    case WM_LBUTTONDOWN:
        MouseStatusBits |= UI_MOUSE_PRESS;
        break;
    case WM_RBUTTONDOWN:
        MouseStatusBits |= UI_MOUSE_PRESS_RIGHT;
        break;
    } /* switch */

} /* WindowsMouseEvent */
