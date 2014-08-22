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
* Description:  Message window.
*
****************************************************************************/


#include "vi.h"
#include "color.h"
#include "font.h"
#include "utils.h"
#include "myprtf.h"
#include "wprocmap.h"

static bool Init( window *, void * );
static bool Fini( window *, void * );

window MessageBar = {
    &messagew_info,
    { 0, 0, 0, 0 },
    Init,
    Fini
};

WINEXPORT LRESULT CALLBACK MessageWindowProc( HWND, UINT, WPARAM, LPARAM );

static char *ClassName = "MessageWindow";
static char msgString1[MAX_STR];
static char msgString2[MAX_STR];
static void msgString( int, char * );

static bool Init( window *w, void *parm )
{
    WNDCLASS        wc;

    w = w;
    parm = parm;

    msgString1[0] = 0;
    msgString2[0] = 0;

    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = GetWndProc( MessageWindowProc );
    wc.cbClsExtra = 0;
    wc.cbWndExtra = sizeof( LONG_PTR );
    wc.hInstance = InstanceHandle;
    wc.hIcon = LoadIcon( (HINSTANCE)NULLHANDLE, IDI_APPLICATION );
    wc.hCursor = LoadCursor( (HINSTANCE)NULLHANDLE, IDC_ARROW );
    wc.hbrBackground = (HBRUSH) COLOR_APPWORKSPACE;
    wc.lpszMenuName = NULL;
    wc.lpszClassName = ClassName;
    return( RegisterClass( &wc ) != 0 );
}

static bool Fini( window *w, void *parm )
{
    w = w;
    parm = parm;
    return( true );
}

WINEXPORT LRESULT CALLBACK MessageWindowProc( HWND hwnd, UINT msg, WPARAM w, LPARAM l )
{
    PAINTSTRUCT ps;

    switch( msg ) {
    case WM_CREATE:
        SET_WNDINFO( hwnd, (LONG_PTR)&MessageBar );
        break;
    case WM_PAINT:
        BeginPaint( hwnd, &ps );
        if( !BAD_ID( MessageWindow ) ) {
            msgString( 1, msgString1 );
            msgString( 2, msgString2 );
        }
        EndPaint( hwnd, &ps );
        return( 0 );
    case WM_SETFOCUS:
        SetFocus( Root );
        return( 0 );
    }
    return( DefWindowProc( hwnd, msg, w, l ) );
}


window_id NewMsgWindow( void )
{
    window_id   msg;
    RECT        *size;
    int         height;

    size = &MessageBar.area;
    msgString1[0] = 0;
    msgString2[0] = 0;
    height = size->bottom - size->top;
    if( !EditFlags.StatusInfo ) {
        height += 1;
    }
    msg = CreateWindow( ClassName, "Message",
                        WS_CHILD | WS_BORDER | WS_CLIPSIBLINGS,
                        size->left - 1, size->top,
                        size->right - size->left + 2, height,
                        Root, (HMENU)NULLHANDLE, InstanceHandle, NULL );
    ShowWindow( msg, SW_SHOWNORMAL );
    UpdateWindow( msg );
    return( msg );
}

static void msgString( int line_no, char *str )
{
    int     height;
    RECT    rect;
    HDC     hdc;

    if( !AllowDisplay || BAD_ID( MessageWindow ) ) {
        return;
    }
    GetClientRect( MessageWindow, &rect );
    height = FontHeight( WIN_FONT( &MessageBar ) );
    rect.top += (line_no - 1) * height;
    rect.bottom = rect.top + height;
    hdc = TextGetDC( MessageWindow, WIN_STYLE( &MessageBar ) );
    FillRect( hdc, &rect, ColorBrush( WIN_BACKCOLOR( &MessageBar ) ) );
    TextReleaseDC( MessageWindow, hdc );
    WriteString( MessageWindow, 0, rect.top, WIN_STYLE( &MessageBar ), str );
}
   
void Message1( char *fmt, ... )
{
    va_list     args;
    char        tmp[MAX_STR];

    ClearWindow( MessageWindow );
    va_start( args, fmt );
    MyVSprintf( tmp, fmt, args );
    va_end( args );
    strcpy( msgString1, tmp );
    msgString( 1, tmp );
}

void Message1Box( char *fmt, ... )
{
    va_list     args;
    char        tmp[MAX_STR];

    va_start( args, fmt );
    MyVSprintf( tmp, fmt, args );
    va_end( args );
    strcpy( msgString1, tmp );
    MessageBox( Root, tmp, NULL, MB_OK );
}

void Message2( char *fmt, ... )
{
    va_list     args;
    char        tmp[MAX_STR];

    va_start( args, fmt );
    MyVSprintf( tmp, fmt, args );
    va_end( args );
    strcpy( msgString2, tmp );
    msgString( 2, tmp );
}
