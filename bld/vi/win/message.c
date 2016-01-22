/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2015-2016 The Open Watcom Contributors. All Rights Reserved.
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
#include "winifini.h"


/* Local Windows CALLBACK function prototypes */
WINEXPORT LRESULT CALLBACK MessageWindowProc( HWND, UINT, WPARAM, LPARAM );

window MessageBar = {
    &messagew_info,
    { 0, 0, 0, 0 }
};

static char *ClassName = "MessageWindow";
static char msgString1[MAX_STR];
static char msgString2[MAX_STR];
static void msgString( int, const char * );

bool MessageBarInit( void )
{
    WNDCLASS        wc;

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

bool MessageBarFini( void )
{
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
        if( !BAD_ID( message_window_id ) ) {
            msgString( 1, msgString1 );
            msgString( 2, msgString2 );
        }
        EndPaint( hwnd, &ps );
        return( 0 );
    case WM_SETFOCUS:
        SetFocus( root_window_id );
        return( 0 );
    }
    return( DefWindowProc( hwnd, msg, w, l ) );
}


window_id NewMsgWindow( void )
{
    window_id   wid;
    RECT        *size;
    int         height;

    size = &MessageBar.area;
    msgString1[0] = 0;
    msgString2[0] = 0;
    height = size->bottom - size->top;
    if( !EditFlags.StatusInfo ) {
        height += 1;
    }
    wid = CreateWindow( ClassName, "Message",
                        WS_CHILD | WS_BORDER | WS_CLIPSIBLINGS,
                        size->left - 1, size->top,
                        size->right - size->left + 2, height,
                        root_window_id, (HMENU)NULLHANDLE, InstanceHandle, NULL );
    ShowWindow( wid, SW_SHOWNORMAL );
    UpdateWindow( wid );
    return( wid );
}

static void msgString( int line_no, const char *str )
{
    int     height;
    RECT    rect;
    HDC     hdc;

    if( !AllowDisplay || BAD_ID( message_window_id ) ) {
        return;
    }
    GetClientRect( message_window_id, &rect );
    height = FontHeight( WIN_TEXT_FONT( &MessageBar ) );
    rect.top += (line_no - 1) * height;
    rect.bottom = rect.top + height;
    hdc = TextGetDC( message_window_id, WIN_TEXT_STYLE( &MessageBar ) );
    FillRect( hdc, &rect, ColorBrush( WIN_TEXT_BACKCOLOR( &MessageBar ) ) );
    TextReleaseDC( message_window_id, hdc );
    WriteString( message_window_id, 0, rect.top, WIN_TEXT_STYLE( &MessageBar ), str );
}

void Message1( const char *fmt, ... )
{
    va_list     args;
    char        tmp[MAX_STR];

    ClearWindow( message_window_id );
    va_start( args, fmt );
    MyVSprintf( tmp, fmt, args );
    va_end( args );
    strcpy( msgString1, tmp );
    msgString( 1, tmp );
}

void Message1Box( const char *fmt, ... )
{
    va_list     args;
    char        tmp[MAX_STR];

    va_start( args, fmt );
    MyVSprintf( tmp, fmt, args );
    va_end( args );
    strcpy( msgString1, tmp );
    MessageBox( root_window_id, tmp, NULL, MB_OK );
}

void Message2( const char *fmt, ... )
{
    va_list     args;
    char        tmp[MAX_STR];

    va_start( args, fmt );
    MyVSprintf( tmp, fmt, args );
    va_end( args );
    strcpy( msgString2, tmp );
    msgString( 2, tmp );
}
