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


#include <string.h>
#include <stdarg.h>
#include "winvi.h"
#include "color.h"
#include "font.h"
#include "utils.h"

static BOOL Init( window *, void * );
static BOOL Fini( window *, void * );

window RepeatCountWindow = {
    &repcntw_info,
    { 0, 0, 0, 0 },
    Init,
    Fini
};

LONG WINEXP RepeatWindowProc( HWND, unsigned, UINT, LONG );

static char     *className = "RepeatWindow";
static char     repString[ MAX_STR ];
static HWND     repeatWindow;

static BOOL Init( window *w, void *parm )
{
    WNDCLASS        wc;

    parm = parm;
    w = w;

    repString[ 0 ] = 0;

    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = (LPVOID) RepeatWindowProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = sizeof( LPVOID );
    wc.hInstance = InstanceHandle;
    wc.hIcon = LoadIcon( (HINSTANCE) NULL, IDI_APPLICATION );
    wc.hCursor = LoadCursor( (HINSTANCE) NULL, IDC_ARROW );
    wc.hbrBackground = (HBRUSH) COLOR_APPWORKSPACE;
    wc.lpszMenuName = NULL;
    wc.lpszClassName = className;
    return( RegisterClass( &wc ) );

} /* Init */

static BOOL Fini( window *w, void *parm )
{
    w = w;
    parm = parm;
    return( FALSE );

} /* Fini */

/*
 * drawRepeatString - draw the current repeat string
 */
static void drawRepeatString( void )
{
    int     height;
    RECT    rect;
    HDC     hdc;

    if( !AllowDisplay || BAD_ID( repeatWindow ) ) {
        return;
    }
    GetClientRect( repeatWindow, &rect );
    height = FontHeight( WIN_FONT( &RepeatCountWindow ) );
    rect.bottom = rect.top + height;
    hdc = TextGetDC( repeatWindow, WIN_STYLE( &RepeatCountWindow ) );
    FillRect( hdc, &rect, ColorBrush( WIN_BACKCOLOR( &RepeatCountWindow ) ) );
    TextReleaseDC( repeatWindow, hdc );
    WriteString( repeatWindow, 0, rect.top, WIN_STYLE( &RepeatCountWindow ),
                repString );

} /* drawRepeatString */

/*
 * RepeatWindowProc - message procedure for the repeat count window
 */
LONG WINEXP RepeatWindowProc( HWND hwnd, unsigned msg, UINT w, LONG l )
{
    PAINTSTRUCT ps;

    switch( msg ) {
    case WM_CREATE:
        SetWindowLong( hwnd, 0, (LONG)(LPVOID)&RepeatCountWindow );
        break;
    case WM_PAINT:
        BeginPaint( hwnd, &ps );
        if( !BAD_ID( repeatWindow ) ) {
            drawRepeatString();
        }
        EndPaint( hwnd, &ps );
        return( TRUE );
    case WM_SETFOCUS:
        SetFocus( Root );
        return( TRUE );
    }
    return( DefWindowProc( hwnd, msg, w, l ) );

} /* RepeatWindowProc */

/*
 * NewRepeatCountWindow - create a new repeat count window
 */
window_id NewRepeatCountWindow( void )
{
    RECT        *size;
    POINT       p;

    size = &RepeatCountWindow.area;

    p.x = size->left;
    p.y = size->top;
    ClientToScreen( Root, &p );

    repString[ 0 ] = 0;
    repeatWindow = CreateWindow( className, "Repeat Count",
        WS_POPUPWINDOW | WS_BORDER | WS_CLIPSIBLINGS,
        p.x, p.y, size->right - size->left, size->bottom - size->top,
        Root, (HMENU) NULL, InstanceHandle, NULL );
    ShowWindow( repeatWindow, SW_SHOWNORMAL );
    UpdateWindow( repeatWindow );
    return( repeatWindow );

} /* NewRepeatCountWindow */

/*
 * UpdateRepeatString - a new repeat count has come in
 */
void UpdateRepeatString( char *str )
{
    ClearWindow( repeatWindow );
    strcpy( repString, str );
    drawRepeatString();

} /* UpdateRepeatString */
