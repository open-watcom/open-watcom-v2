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


#include "variety.h"
#include "win.h"
#include <string.h>
#include <malloc.h>
#include <stdio.h>
#include <signal.h>

static char  DefaultAboutMsg[] = "   WATCOM Default Window System\n\n                Version 10.0\n\n \251 1991-1994 WATCOM Systems, Inc.";
static char  _WCI86FAR *AboutMsg = DefaultAboutMsg;
static char  DefaultAboutTitle[] = "About WATCOM Default Windowing System";
static char  _WCI86FAR *AboutTitle = DefaultAboutTitle;
static long  shiftState = 0;

#define SS_ALT          0x01
#define SS_SHIFT        0x02
#define SS_CAPS         0x04
#define SS_CTRL         0x08

int     _SetAboutDlg( char *title, char *text ) {
//===============================================

        if( title ) {
            if( DefaultAboutTitle != AboutTitle ) {
                _MemFree( AboutTitle );
            }
            AboutTitle = _MemAlloc( FARstrlen( title ) + 1 );
            if( !AboutTitle ) return( 0 );
            FARstrcpy( AboutTitle, title );
        }
        if( text ) {
            if( DefaultAboutMsg != AboutMsg ) {
                _MemFree( AboutMsg );
            }
            AboutMsg = _MemAlloc( FARstrlen( text ) + 1 );
            if( !AboutMsg ) return( 0 );
            FARstrcpy( AboutMsg, text );
        }
        return( 1 );
}

/*
 * MainWindowProc - message handler for the frame window
 */
static long MainWindowProc( HWND hwnd, UINT message, UINT wparam,
                                DWORD lparam )
{
    LPWDATA     w;
    DWORD       old;
    char        tmp[128];

    switch (message) {
    case WM_KILLFOCUS:
        w = _GetActiveWindowData();
        if( w != NULL ) {
            _ShowWindowActive( NULL, w );
        }
        return( DefWindowProc( hwnd, message, wparam, lparam ) );
        break;
    case WM_SETFOCUS:
        w = _GetActiveWindowData();
        if( w != NULL ) {
            _ShowWindowActive( w, NULL );
        }
        return( DefWindowProc( hwnd, message, wparam, lparam ) );
        break;
    case WM_COMMAND:
        if( LOWORD( wparam ) >= MSG_WINDOWS ) {
            w = _IsWindowedHandle( LOWORD( wparam ) - MSG_WINDOWS );
            if( w != NULL ) {
                _MakeWindowActive( w );
            }
            break;
        }
        switch( LOWORD( wparam ) ) {
        case MSG_ABOUT:
            MessageBox( hwnd, AboutMsg, AboutTitle, MB_APPLMODAL | MB_ICONINFORMATION | MB_OK );
            break;
        case MSG_SETCLEARINT:
            old = _AutoClearLines;
            _GetClearInterval();
            if( _AutoClearLines != old ) {
                sprintf( tmp,"Buffers will be cleared after %ld lines",
                    _AutoClearLines );
                MessageBox( NULL,tmp,"Amount Set", MB_OK );
            }
            break;
        case MSG_COPY:
            w = _GetActiveWindowData();
            if( w != NULL && !w->gphwin ) {
                _CopyAllLines( w );
            }
            break;
        case MSG_EXIT:
            DestroyWindow( hwnd );
            break;
        case MSG_FLUSH:
            w = _GetActiveWindowData();
            if( w != NULL && !w->InputMode ) {
                if( w != NULL && !w->gphwin ) {
                    _FreeAllLines( w );
                    _ClearWindow( w );
                }
            }
            break;
        case MSG_WRITE:
            w = _GetActiveWindowData();
            if( w != NULL ) {
                _SaveAllLines( w );
            }
            break;
        default:
            return( DefWindowProc( hwnd, message, wparam, lparam ) );
        }
        break;
    case WM_DESTROY:
        DeleteObject( _FixedFont );
        PostQuitMessage( 0 );
        break;
    case WM_KEYDOWN:
        w = _GetActiveWindowData();
        if( w == NULL )  break;
        if( wparam == VK_CONTROL ) {
            shiftState |= SS_CTRL;
        } else if( wparam == VK_CANCEL ) {
            MessageBox( hwnd, "", "SIGBREAK",
                        MB_APPLMODAL | MB_ICONINFORMATION | MB_OK );
            shiftState &= ~SS_CTRL;
            raise( SIGBREAK );
            break;
        } else if( ( shiftState & SS_CTRL ) && ( wparam == 'C' ) ) {
            MessageBox( hwnd, "", "SIGINT",
                        MB_APPLMODAL | MB_ICONINFORMATION | MB_OK );
            shiftState &= ~SS_CTRL;
            raise( SIGINT );
            break;
        }
        ShowCursor( FALSE );
        switch( wparam ) {
        case VK_DOWN:
            _MoveLineDown( w );
            break;
        case VK_UP:
            _MoveLineUp( w );
            break;
        case VK_PRIOR:
            _MovePageUp( w );
            break;
        case VK_NEXT:
            _MovePageDown( w );
            break;
        case VK_HOME:
            _MoveToLine( w, 1, TRUE );
            break;
        case VK_END:
            _MoveToLine( w, _GetLastLineNumber( w ), TRUE );
            break;
        default:
            if( wparam==VK_HOME    ||  wparam==VK_END       ||
                wparam==VK_RETURN  ||  wparam==VK_LEFT      ||
                wparam==VK_RIGHT   ||  wparam==VK_DELETE    ||
                wparam==VK_BACK    ||  wparam==VK_INSERT )
            {
                _WindowsVirtualKeyPush( wparam, HIWORD(lparam) );
            }
            break;
        }
        ShowCursor( TRUE );
        break;

    case WM_CHAR:
        if( wparam!=13 && wparam!=8 )
            _WindowsKeyPush( wparam, HIWORD(lparam) );
        break;

    case WM_KEYUP:
        if( wparam == VK_CONTROL ) {
            shiftState &= ~SS_CTRL;
        }
        _WindowsKeyUp( wparam, HIWORD( lparam ) );
        break;

    default:
        return( DefWindowProc( hwnd, message, wparam, lparam ) );
    }
    return( NULL );
} /* MainWindowProc */

/*
 * _MainDriver - main message driver for the editor
 */
long CALLBACK _MainDriver( HWND hwnd, UINT message, UINT wparam, LONG lparam )
{
    PAINTSTRUCT ps;
    HDC         dc;
    HBRUSH      oldbrush;
    LPWDATA     w;
    RECT        rect;
    int         rc;
    WORD        height,width;

    if( hwnd == _MainWindow ) {
        return( MainWindowProc( hwnd, message, wparam, lparam ) );
    }

    w = _GetWindowData( hwnd );
    if( w == NULL ) {
        return( DefWindowProc( hwnd, message, wparam, lparam ) );
    }

    switch (message) {
    case WM_SETFOCUS:
        SetFocus( _MainWindow );
    case WM_SYSCOMMAND:
    case WM_MOUSEACTIVATE:
        _MakeWindowActive( w );
        return( DefWindowProc( hwnd, message, wparam, lparam ) );

    case WM_KILLFOCUS:
        if( ( (HWND)wparam != NULL) && ( (HWND)wparam != _MainWindow ) ) {
            _ShowWindowActive( NULL, w );
        }
        return( DefWindowProc( hwnd, message, wparam, lparam ) );

    case WM_COMPACTING:
        rc = MessageBox( NULL,
            "System has indicated low memory, Clear Lines?",
            "System Request",
            MB_OKCANCEL | MB_TASKMODAL | MB_ICONEXCLAMATION );
        if( rc == IDOK ) {
            _FreeAllLines( w );
            _ClearWindow( w );
        }
        break;

    case WM_DESTROY:
        _DestroyAWindow( w );
        break;

    case WM_PAINT:
        dc = BeginPaint( hwnd, &ps );
        _RepaintWindow( w, &ps.rcPaint, dc );
        EndPaint( hwnd, &ps );
        break;

    case WM_SIZE:
        if( w->resizing ) {
            w->resizing = FALSE;
            return( DefWindowProc( hwnd, message, wparam, lparam ) );
        }
        height = HIWORD( lparam );
        width = LOWORD( lparam );
        dc = GetDC( hwnd );
        #ifndef __NT__
            UnrealizeObject( w->brush );
        #endif
        oldbrush = SelectObject( dc, w->brush );
        #ifdef __NT__
            SetBrushOrgEx( dc, 0, 0, NULL  );
        #endif
        GetClientRect( hwnd, &rect );
        FillRect( dc, &rect, w->brush );
        SelectObject( dc, oldbrush );
        ReleaseDC( hwnd, dc );
        _ResizeWin( w, rect.left, rect.top, rect.left+width, rect.top+height );
        _DisplayAllLines( w, FALSE );
        return( NULL );

    case WM_VSCROLL:
        ShowCursor( FALSE );
        switch( LOWORD( wparam ) ) {
        case SB_THUMBPOSITION:
            _MoveToLine( w, _GetLineFromThumbPosition( w,
                            GET_WM_VSCROLL_POS( wparam, lparam ) ), TRUE  );
            break;
        case SB_PAGEDOWN:
            _MovePageDown( w );
            break;
        case SB_PAGEUP:
            _MovePageUp( w );
            break;
        case SB_LINEDOWN:
            _MoveLineDown( w );
            break;
        case SB_LINEUP:
            _MoveLineUp( w );
            break;
        }
        ShowCursor( TRUE );

    default:
        return( DefWindowProc( hwnd, message, wparam, lparam ) );
    }
    return (NULL);

} /* _MainDriver */
