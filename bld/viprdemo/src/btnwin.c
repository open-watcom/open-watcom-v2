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


#include <windows.h>
#include "button.h"
#include "win1632.h"

static void button_size( HWND hwnd, int id, int FAR * pwidth, int FAR * pheight )
{
    HINSTANCE inst;
    HBITMAP bmp;
    BITMAP bitmap;

    inst = GET_HINSTANCE( hwnd );
    bmp = LoadBitmap( inst, MAKEINTRESOURCE( id ) );
    if( !bmp ) return;
    GetObject( bmp, sizeof(BITMAP), &bitmap );
    *pwidth = bitmap.bmWidth +5;
    *pheight = bitmap.bmHeight + 5;
    DeleteObject( bmp );
}

void add_button(
    HWND        parent,
    int         top,
    int         left,
    int         id,
    int FAR *  pwidth,
    int FAR *  pheight )
{
    HWND        hbutton;
    HINSTANCE inst;

    // Note that the resource ID is the same as the control ID
    inst = GET_HINSTANCE( parent );
    button_size( parent, id, pwidth, pheight );

    hbutton = CreateWindow(
        "BUTTON",
        NULL,
        WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,   // Window style
        left,                   // horizontal
        top,                    // vertical
        *pwidth,                // width
        *pheight,               // height
        parent,                 // parent window
        (HMENU)id,              // menu is really child number
        inst,                   // This instance owns this window.
        NULL                    // Pointer not needed.
    );
}

void measure_button(HWND parent, int button_id, MEASUREITEMSTRUCT FAR * measure )
{
    int width, height;  // for proper typing since itemWidth and itemHeight are UNIT's

    button_size( parent, button_id, &width, &height );
    measure->itemWidth = width;
    measure->itemHeight = height;
}

static void horizontal( DRAWITEMSTRUCT FAR * draw, int row )
{
    MoveToEx( draw->hDC,draw->rcItem.left + 1, row, NULL );
    LineTo( draw->hDC, draw->rcItem.right - 1, row );
}

static void vertical( DRAWITEMSTRUCT FAR * draw, int column )
{
    MoveToEx( draw->hDC, column, draw->rcItem.top + 1, NULL );
    LineTo( draw->hDC, column, draw->rcItem.bottom - 1 );
}

void draw_button( int button_id, DRAWITEMSTRUCT FAR * draw )
{
    HBITMAP     bmp;
    HBITMAP oldbmp;
    HPEN blackpen, shadowpen, brightpen, facepen;
    HPEN oldpen;
    BITMAP      bitmap;
    HDC memdc;
    HINSTANCE inst;
    int shift;

    inst = GET_HINSTANCE( draw->hwndItem );
    bmp = LoadBitmap( inst, MAKEINTRESOURCE( button_id ) );
    if( !bmp ) return;
    GetObject( bmp, sizeof(BITMAP), &bitmap );
    memdc = CreateCompatibleDC( draw->hDC );
    oldbmp = SelectObject( memdc, bmp );
    if( draw->itemState & ODS_SELECTED ) {
        shift = 4;
    } else {
        shift = 2;
    }
    BitBlt( draw->hDC, draw->rcItem.left + shift, draw->rcItem.top + shift,
        bitmap.bmWidth, bitmap.bmHeight, memdc, 0, 0, SRCCOPY );
    SelectObject( memdc, oldbmp );
    DeleteDC( memdc );
    DeleteObject( bmp );
    // Draw four sides of the button except one pixel in each corner
    blackpen = CreatePen( PS_SOLID, 0, RGB(0,0,0) );
    brightpen = CreatePen( PS_SOLID, 0, RGB(255,255,255) );
    shadowpen = CreatePen( PS_SOLID, 0, GetSysColor( COLOR_BTNSHADOW ) );
    facepen = CreatePen( PS_SOLID, 0, GetSysColor( COLOR_BTNFACE ) );
    oldpen = SelectObject( draw->hDC, blackpen );
    horizontal( draw, draw->rcItem.top );
    horizontal( draw, draw->rcItem.bottom - 1 );
    vertical( draw, draw->rcItem.left );
    vertical( draw, draw->rcItem.right - 1 );
   // Now the shading
    SelectObject( draw->hDC, shadowpen );
    if( draw->itemState & ODS_SELECTED ) {
        horizontal( draw, draw->rcItem.top + 1 );
        vertical( draw, draw->rcItem.left + 1 );
        SelectObject( draw->hDC, facepen );
        horizontal( draw, draw->rcItem.top + 2 );
        vertical( draw, draw->rcItem.left + 2 );
        horizontal( draw, draw->rcItem.top + 3 );
        vertical( draw, draw->rcItem.left + 3 );
    } else {
        horizontal( draw, draw->rcItem.bottom - 2 );
        horizontal( draw, draw->rcItem.bottom - 3 );
        vertical( draw, draw->rcItem.right - 2 );
        vertical( draw, draw->rcItem.right - 3 );
        SelectObject( draw->hDC, brightpen );
        horizontal( draw, draw->rcItem.top + 1 );
        vertical( draw, draw->rcItem.left + 1 );
    }
    SelectObject( draw->hDC, oldpen );
    DeleteObject( blackpen );
    DeleteObject( brightpen );
    DeleteObject( shadowpen );
}
