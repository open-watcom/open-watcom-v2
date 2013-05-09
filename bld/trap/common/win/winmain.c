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


#include <stddef.h>
#include <string.h>
#include <windows.h>
#include "trpimp.h"
#include "trperr.h"
#include "winserv.h"
#include "packet.h"

extern void OpeningStatement( void );
extern int Initialize( char * );

int RemoteLinkObtained = FALSE;
int SessionStarted = FALSE;

HANDLE CProcHandle;
HWND MainWindowHandle;
HWND DebugWindow;

extern void windows_main(void);

extern void ProcAppl(HWND, unsigned, WORD, LONG);
extern void SetAppl(void);
extern bool CreateSelections(short, short, short, short );

char _mainclass[] = "wdservw";
extern char ServName[];

HANDLE  Instance;

/*
 * WinMain - main entry point from windows
 */
int PASCAL WinMain( HANDLE thishandle, HANDLE prevhandle, LPSTR cmdline, int cmdshow )
{
    MSG         msg;
    char        str[128];

    Instance = thishandle;
    cmdshow = cmdshow;
    if( prevhandle ) {
        MessageBox( MainWindowHandle, TRP_ERR_one_allowed, ServName, MB_ICONEXCLAMATION | MB_OK );
        return( FALSE );
    }
    if( !( GetWinFlags() & WF_PMODE ) ) {
        MessageBox( MainWindowHandle, TRP_WIN_not_protect_mode, ServName, MB_ICONEXCLAMATION | MB_OK );
        return( FALSE );
    }
    _fstrcpy( str, cmdline );
    CProcHandle = thishandle;
    if( !DoRegisterClass()) {
        return( FALSE );
    }
    if( !initAppl() ) {
        return( FALSE );
    }
    if( !Initialize( str ) ) {
        DestroyWindow( MainWindowHandle );
        return( FALSE );
    }
    SetHandleCount( 40 );

    for( ;; ) {

        SetAppl();
        while( !MessageLoop() );

        if( SessionStarted ) {
            while( PeekMessage( &msg, NULL,0,0,PM_NOYIELD | PM_NOREMOVE ) ) {
                GetMessage( &msg, NULL, NULL, NULL );
                TranslateMessage( &msg );
                DispatchMessage( &msg );
            }
            windows_main();
            SessionStarted = FALSE;
        } else {
            break;
        }
        if( !initAppl() ) {
            return( FALSE );
        }

    }

    if( RemoteLinkObtained ) {
        RemoteUnLink();
    }
    KillTrap();

    return( msg.wParam );

} /* WinMain */

/*
 * DoRegisterClass - window class registration
 */
BOOL DoRegisterClass( void )
{
    WNDCLASS wc;
    BOOL rc;

    /*
     * register main window class
     */
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = MainDriver;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = CProcHandle;
    wc.hIcon = LoadIcon( CProcHandle, "ApplIcon" );
    wc.hCursor = LoadCursor( NULL, IDC_ARROW );
    wc.hbrBackground = GetStockObject( WHITE_BRUSH );
    wc.lpszMenuName = "ApplMenu";
    wc.lpszClassName = _mainclass;
    rc = RegisterClass( &wc );
    return( rc );

} /* DoRegisterClass */

#define AvgXChar( tm ) (tm.tmAveCharWidth  + (7+tm.tmAveCharWidth)/8)
#define AvgYChar( tm ) (tm.tmHeight + tm.tmExternalLeading)
/*
 * initAppl - start up our server application
 */
static BOOL initAppl( void )
{
    BOOL        rc;
    short       x,y;
    HDC         dchandle;
    TEXTMETRIC  tm;
    short       avgx,avgy;


    x = GetSystemMetrics( SM_CXSCREEN );
    y = GetSystemMetrics( SM_CYSCREEN );

    MainWindowHandle = CreateWindow(
        _mainclass,             /* class */
        ServName,               /* caption */
        WS_CLIPCHILDREN | WS_OVERLAPPEDWINDOW,
        0,                      /* init. x pos */
        0,                      /* init. y pos */
        x,                      /* init. x size */
        y,                      /* init. y size */
        NULL,                   /* parent window */
        NULL,                   /* menu handle */
        CProcHandle,                    /* program handle */
        NULL                    /* create parms */
        );

    if( !MainWindowHandle ) {
        return( FALSE );
    }
    ShowWindow( MainWindowHandle, SW_SHOWNORMAL );
    UpdateWindow( MainWindowHandle );
    dchandle = GetDC( MainWindowHandle );
    GetTextMetrics( dchandle, &tm );
    avgx = AvgXChar( tm );
    avgy = AvgYChar( tm );
    ReleaseDC( MainWindowHandle, dchandle );

    rc = CreateSelections( x, y, avgx, avgy );
    if( !rc ) {
        return( FALSE );
    }
    return( StartOutput( x, y ) );

} /* initAppl */

BOOL StartOutput( short x, short y )
{
    DebugWindow = CreateWindow(
        "LISTBOX",              /* class */
        "",                     /* caption */
        WS_CHILD | WS_CAPTION | WS_HSCROLL | WS_VSCROLL | WS_BORDER, /* style */
        15,                     /* init. x pos */
        20,                     /* init. y pos */
        3*(x/4),                    /* init. x size */
        y/2,                    /* init. y size */
        MainWindowHandle,       /* parent window */
        NULL,                   /* menu handle */
        CProcHandle,            /* program handle */
        NULL                    /* create parms */
        );

    if( !DebugWindow ) {
        return( FALSE );
    }
    ShowWindow( DebugWindow, SW_SHOWNORMAL );
    UpdateWindow( DebugWindow );
    OpeningStatement();
    return( TRUE );

} /* StartOutput */

/*
 * GenericBox - display a dialog box
 */
BOOL CALLBACK GenericBox( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam )
{
    /*
     * process messages
     */
    lparam = lparam;
    switch( msg ) {
    case WM_INITDIALOG:
        return( TRUE );

    case WM_COMMAND:
        if( wparam == IDOK || wparam == IDCANCEL) {
            EndDialog( hwnd, TRUE );
            return( TRUE );
            break;
        }
    }

    return( FALSE );

} /* GenericBox */

/*
 * MainDriver - receives all messages for the main window
 */
LRESULT __export CALLBACK MainDriver( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam )
{
    FARPROC     farproc;

    switch( msg ) {
    case WM_DESTROY:
        PostQuitMessage( 0 );
        return( DefWindowProc( hwnd, msg, wparam, lparam ) );
        break;

    case WM_COMMAND:
        switch( wparam ) {
        case MSG_CLEAR:
            SendMessage( DebugWindow, LB_RESETCONTENT, 0,  0L );
            break;

        case MSG_ABOUT:
            farproc = MakeProcInstance( GenericBox, CProcHandle );
            DialogBox( CProcHandle, "AboutBox", hwnd, farproc );
            FreeProcInstance( farproc );
            break;

        case MSG_EXIT:
            PostQuitMessage( 0 );
            break;

        default:
            ProcAppl( hwnd, msg, wparam, lparam );
            break;
        }
        break;

    default:
        return( DefWindowProc(hwnd,msg,wparam,lparam) );

    }
    return( NULL );

} /* MainDriver */

/*
 * MessageLoop
 */
BOOL MessageLoop( void )
{
    MSG         msg;
    WORD        rc;

    while( PeekMessage( &msg, NULL, NULL, NULL, PM_NOREMOVE | PM_NOYIELD ) ) {
        rc = GetMessage( &msg, NULL, NULL, NULL );
        if( !rc ) {
            return( TRUE );
        }
        TranslateMessage( &msg );
        DispatchMessage( &msg );
    }
    Yield();
    if( !SessionStarted ) {
        if( RemoteLinkObtained ) {
            if( RemoteConnect() ) {
                SessionStarted = TRUE;
                DebugWindow = 0;
                DestroyWindow( MainWindowHandle );
            }
        }
    }
    return( FALSE );

} /* MessageLoop */
