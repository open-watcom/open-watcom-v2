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
#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#define INCL_WIN
#define INCL_GPI
#include <os2.h>
#include "win.h"
#include "pmmenu.h"

extern HWND _GetWinMenuHandle( void );

#define DISPLAY(x)      WinMessageBox( HWND_DESKTOP, NULL, x, "Error", 0, MB_APPLMODAL | MB_NOICON | MB_OK | MB_MOVEABLE );

int _MessageLoop( BOOL doexit ) {
//===============================

    QMSG        msg;
    int         rc;
    extern int  DoStdIO;

    rc = 1;
    while ( WinPeekMsg( _AnchorBlock, &msg, NULL, 0, 0, PM_NOREMOVE ) ) {
        rc = WinGetMsg( _AnchorBlock, &msg, NULL, 0, 0 );
        if( !rc ) {
            if( doexit ) {
                _WindowsExitRtn = NULL;
                exit( 0 );
            }
            return( rc );
        }
        WinDispatchMsg( _AnchorBlock, &msg );
    }
    return( rc );
}

int _BlockingMessageLoop( BOOL doexit ) {
//======================================

    int         rc;
    QMSG        msg;

    rc = WinGetMsg( _AnchorBlock, &msg, NULL, 0, 0 );
    if( !rc ) {
        if( doexit ) {
            _WindowsExitRtn = NULL;
            exit( 0 );
        }
        return( rc );
    }
    WinDispatchMsg( _AnchorBlock, &msg );
    return( _MessageLoop( doexit ) );
}

int     _SetConTitle( LPWDATA w, char *title ) {
//==============================================

    return( WinSetWindowText( w->frame, title ) );
}

int     _SetAppTitle( char *title ) {
//===================================

    return( WinSetWindowText( _MainFrameWindow, title ) );
}

int     _ShutDown( void ) {
//=========================

    WinSetWindowPos( _MainFrameWindow, 0, 0, 0, 0, 0, SWP_MINIMIZE );
    WinSendMsg( _MainFrameWindow, WM_CLOSE, 0, 0 );
    return( 0 );
}

int     _CloseWindow( LPWDATA w ) {
//=================================

    if( w->destroy ) {
        WinSendMsg( w->hwnd, WM_CLOSE, 0, 0 );
    }
    return( 0 );
}

void    _NewCursor( LPWDATA w, cursors type ) {
//============================================

// change the cursor type

    if( w->hascursor ) {
        WinDestroyCursor( w->hwnd );
        w->hascursor = FALSE;
    }
    if( type == KILL_CURSOR ) return;
    w->CaretType = type;
    switch( type ) {
    case SMALL_CURSOR:
        WinCreateCursor( w->hwnd, 0, 0, 0, w->ychar - SPACE_BETWEEN_LINES,
                         CURSOR_SOLID | CURSOR_FLASH, NULL );
        w->hascursor = TRUE;
        break;
    case FAT_CURSOR:
        WinCreateCursor( w->hwnd, 0, 0, 4, w->ychar - SPACE_BETWEEN_LINES,
                         CURSOR_SOLID | CURSOR_FLASH, NULL );
        w->hascursor = TRUE;
        break;
    case ORIGINAL_CURSOR:
        break;
    }
}


void    _DisplayCursor( LPWDATA w ) {
//==================================

// show the current cursor position

    HPS                 ps;
    int                 x;
    int                 y;
    POINTL              points[TXTBOX_COUNT];

    ps = WinGetPS( w->hwnd );
    _SelectFont( ps );
    #ifdef _MBCS
        GpiQueryTextBox( ps, FAR_mbsnbcnt(w->tmpbuff->data,w->curr_pos),
                         w->tmpbuff->data, TXTBOX_COUNT, points );
    #else
        GpiQueryTextBox( ps, w->curr_pos, w->tmpbuff->data, TXTBOX_COUNT, points );
    #endif
    x = points[TXTBOX_CONCAT].x;
    y = (w->y2 - w->y1) - (w->LastLineNumber-w->TopLineNumber+1) * w->ychar;
    WinReleasePS( ps );
    WinCreateCursor( w->hwnd, x, y, 0, 0, CURSOR_SETPOS, NULL );
    WinShowCursor( w->hwnd, TRUE );
}


void    _SetInputMode( LPWDATA w, BOOL val ) {
//===========================================

// set whether or not we are in input mode

    w->InputMode = val;
}


/*
 * _ShowWindowActive - do windows-specific stuff to make a window active
 */
void _ShowWindowActive( LPWDATA w, LPWDATA last )
{
    MENUITEM    menudesc;

    if( last != NULL ) {
        menudesc.hItem = 0;
        menudesc.afStyle = MIS_TEXT;
        menudesc.afAttribute = 0;
        menudesc.iPosition = 0;
        menudesc.id = DID_WIND_STDIO + last->handles[0];
        menudesc.hwndSubMenu = NULL;
        WinSendMsg( _GetWinMenuHandle(), ( ULONG )MM_SETITEM, FALSE, MPFROMP( &menudesc ) );
        if( last->CaretType != ORIGINAL_CURSOR ) {
            _NewCursor( last, KILL_CURSOR );
        }
        WinSendMsg( WinWindowFromID( last->frame, FID_TITLEBAR ), TBM_SETHILITE,
                FALSE, 0 );
        WinSetFocus( HWND_DESKTOP, _MainWindow );
    }
    if( w != NULL ) {
        WinSetActiveWindow( HWND_DESKTOP, w->frame );
        WinSetFocus( w->frame, FALSE );
        WinSendMsg( _MainWindow, WM_ACTIVATE, FALSE, MPFROMP( w->frame ) );
        WinSetFocus( _MainWindow, TRUE );
        WinSetActiveWindow( HWND_DESKTOP, _MainWindow );
        menudesc.hItem = 0;
        menudesc.afStyle = MIS_TEXT;
        menudesc.afAttribute = MIA_CHECKED;
        menudesc.iPosition = 0;
        menudesc.id = DID_WIND_STDIO + w->handles[0];
        menudesc.hwndSubMenu = NULL;
        WinSendMsg( _GetWinMenuHandle(), ( ULONG )MM_SETITEM, FALSE, MPFROMP( &menudesc ) );
        if( w->CaretType != ORIGINAL_CURSOR ) {
            _NewCursor( w, w->CaretType );
            _DisplayCursor( w );
        }
        WinSendMsg( WinWindowFromID( w->frame, FID_TITLEBAR ), TBM_SETHILITE,
                MPFROMSHORT( TRUE ), 0 );
    }

} /* _ShowWindowActive */


/*
 * _OutOfMemory - display out of memory message
 */
void _OutOfMemory( void )
{
        WinMessageBox( HWND_DESKTOP, _MainWindow, "Out Of Memory!",
                       "SYSTEM ERROR", 0, MB_OK );
} /* _OutOfMemory */


/*
 * _ExecutionComplete - display execution complete message
 */
void _ExecutionComplete( void )
{
    flushall();
//    WinMessageBox( HWND_DESKTOP, _MainWindow, "Execution Complete",
//                   "", 0, MB_OK | MB_ICONEXCLAMATION );

} /* _ExecutionComplete */


void _Error( HWND hwndDlg, char *caption, char *msg ) {
//=======================================================

    WinMessageBox( HWND_DESKTOP, hwndDlg, msg, caption, 0,
                   MB_APPLMODAL | MB_NOICON | MB_OK | MB_MOVEABLE );
}

/*
 * _ResizeWindows - Resize windows after a main window resize
 */
void _ResizeWindows( void ) {

    int         i;
    int         place = 0;
    LPWDATA     w;
    SWP         swps;
    SWP         mwps;
    int         resize = FALSE;

    WinQueryWindowPos( _MainFrameWindow, &mwps );
    for( i=0;i<_MainWindowData->window_count;i++ ) {
        w = _MainWindowData->windows[i];
        WinQueryWindowPos( w->frame, &swps );
        if ( swps.fl & SWP_MAXIMIZE ) {
            WinSetWindowPos( w->frame, swps.hwndInsertBehind, swps.x,
                                swps.y, swps.cx, swps.cy, SWP_MINIMIZE );
            WinSetWindowPos( w->frame, swps.hwndInsertBehind, swps.x,
                                swps.y, swps.cx, swps.cy, SWP_MAXIMIZE );
        } else if ( !( swps.fl & SWP_MINIMIZE ) ) {
            if ( ( swps.x + swps.cx ) > mwps.cx ) {
                swps.cx = mwps.cx - ( place * w->xchar ) + 4;
                swps.x = ( place * w->xchar ) - 4;
                resize = TRUE;
            }
            if ( ( swps.y + swps.cy ) > mwps.cy ) {
                swps.cy = mwps.cy - ( ( place + 3 ) * w->ychar );
                swps.y = 0;
                resize = TRUE;
            }
            if ( resize ) {
                place += 4;
                WinSetWindowPos( w->frame, swps.hwndInsertBehind, swps.x,
                                swps.y, swps.cx, swps.cy, SWP_MOVE | SWP_RESTORE | SWP_SIZE );
            }
        }
    }
}
