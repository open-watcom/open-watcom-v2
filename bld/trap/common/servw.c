/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2023 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Remote debug server mainline for Windows GUI versions.
*
****************************************************************************/


#include <stddef.h>
#include <process.h>
#include <wwindows.h>
#include "servw.h"
#include "trpld.h"
#include "trpsys.h"
#include "trperr.h"
#include "packet.h"
#include "servname.rh"
#include "servio.h"
#include "nothing.h"
#include "options.h"
#include "wclbproc.h"


extern trap_version TrapVersion;

HANDLE          Instance;

static char     ServerClass[32]="ServerClass";
static HWND     hwndMain;
static bool     SessionError = false;
static bool     Connected = false;
static bool     Linked = false;
static bool     OneShot = false;

static bool     FirstInstance( HINSTANCE );
static bool     AnyInstance( HINSTANCE, int );

#define MENU_ON     (MF_ENABLED+MF_BYCOMMAND)
#define MENU_OFF    (MF_DISABLED+MF_GRAYED+MF_BYCOMMAND)

/*
 * WinMain - initialization, message loop
 */
int PASCAL WinMain( HINSTANCE this_inst, HINSTANCE prev_inst, LPSTR cmdline, int cmdshow )
{
    MSG         msg;
    const char  *err;
    char        trapparms[PARMS_MAXLEN];

    Instance = this_inst;
    if( !prev_inst ) {
        if( !FirstInstance( this_inst ) ) {
            return( FALSE );
        }
    }
    err = ParseCommandLine( cmdline, trapparms, RWBuff, &OneShot );
    if( err == NULL ) {
        err = RemoteLinkSet( RWBuff );
        if( err == NULL ) {
            switch( LoadTrap( trapparms, RWBuff, &TrapVersion ) ) {
            #define DIGS_ERROR(e,t) case e: err = t; break;
            DIGS_ERRORS( "TRAP Loader: ", RWBuff )
            #undef DIGS_ERROR
            default: err = DIGS_ERRORS_default( "TRAP Loader: " ); break;
            }
        }
    }
    if( err != NULL ) {
        StartupErr( err );
        return( FALSE );
    }
    if( !AnyInstance( this_inst, cmdshow ) )
        return( FALSE );

    /*
     * toggle start/end processing and RemoteLink/RemoteUnLink
     * here is used for first time that call RemoteLink and start processing
     */
    SendMessage( hwndMain, WM_COMMAND, MENU_CONNECT, 0 );

    while( GetMessage( (LPVOID)&msg, (HWND)0, 0, 0 ) ) {
        TranslateMessage( &msg );
        DispatchMessage( &msg );
    }
    if( Linked )
        RemoteUnLink();
    UnLoadTrap();

    return( msg.wParam );

} /* WinMain */

/*
 * FirstInstance - register window class for the application,
 *                 and do any other application initialization
 */
static bool FirstInstance( HINSTANCE this_inst )
{
    WNDCLASS    wc;

    /*
     * set up and register window class
     */
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WindowProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = sizeof( DWORD );
    wc.hInstance = this_inst;
    wc.hIcon = LoadIcon( this_inst, "ServerIcon" );
    wc.hCursor = LoadCursor( NULL, IDC_ARROW );
    wc.hbrBackground = GetStockObject( WHITE_BRUSH );
    wc.lpszMenuName = "ServerMenu";
    wc.lpszClassName = ServerClass;
    return( RegisterClass( &wc ) != 0 );

} /* FirstInstance */

static void EnableMenus( HWND hwnd, bool connected, bool session )
/****************************************************************/
{
    HMENU hMenu;

    hMenu = GetMenu( hwnd );
    if( hMenu == NULL )
        return;
    EnableMenuItem( hMenu, MENU_EXIT, !session ? MENU_ON : MENU_OFF );
    EnableMenuItem( hMenu, MENU_CONNECT, !connected ? MENU_ON : MENU_OFF );
    EnableMenuItem( hMenu, MENU_OPTIONS, !connected ? MENU_ON : MENU_OFF );
    EnableMenuItem( hMenu, MENU_DISCONNECT, ( connected && !session ) ? MENU_ON : MENU_OFF );
    EnableMenuItem( hMenu, MENU_BREAK, session ? MENU_ON : MENU_OFF );
    DrawMenuBar( hwnd );
}

/*
 * AnyInstance - do work required for every instance of the application:
 *                create the window, initialize data
 */
static bool AnyInstance( HINSTANCE this_inst, int cmdshow )
{
    /*
     * create main window
     */
    hwndMain = CreateWindow(
        ServerClass,            /* class */
        SERVNAME,
        WS_OVERLAPPEDWINDOW,    /* style */
        CW_USEDEFAULT,          /* init. x pos */
        CW_USEDEFAULT,          /* init. y pos */
        CW_USEDEFAULT,          /* init. x size */
        CW_USEDEFAULT,          /* init. y size */
        NULL,                   /* parent window */
        NULL,                   /* menu handle */
        this_inst,              /* program handle */
        NULL                    /* create parms */
        );

    if( !hwndMain )
        return( false );

#ifdef __NT__
    TRAP_EXTFUNC( InfoFunction )( hwndMain );
#endif

    /*
     * display window
     */
    ShowWindow( hwndMain, cmdshow );
    UpdateWindow( hwndMain );

    return( true );

} /* AnyInstance */

/*
 * AboutDlgProc - processes messages for the about dialog.
 */
WINEXPORT BOOL CALLBACK AboutDlgProc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam )
{
    lparam = lparam;                    /* turn off warning */

    switch( msg ) {
    case WM_INITDIALOG:
        return( TRUE );

    case WM_COMMAND:
        if( LOWORD( wparam ) == IDOK ) {
            EndDialog( hwnd, TRUE );
            return( TRUE );
        }
        break;
    }
    return( FALSE );

} /* AboutDlgProc */

static bool Disconnect = false;
static bool Exit = false;
/*
 * WindowProc - handle messages for the main application window
 */
WINEXPORT LRESULT CALLBACK WindowProc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam )
{
    DLGPROC     dlgproc;
    const char  *err;
    HMENU       hMenu;

    #define ServTerminateWin(rc)    PostQuitMessage(rc)

    switch( msg ) {
    case WM_COMMAND:
        switch( LOWORD( wparam ) ) {
        case MENU_ABOUT:
            dlgproc = MakeProcInstance_DLG( AboutDlgProc, Instance );
            DialogBox( Instance,"AboutBox", hwnd, dlgproc );
            FreeProcInstance_DLG( dlgproc );
            break;

        case MENU_DISCONNECT:
            Disconnect = true;
            break;
        case MENU_CONNECT:
            Disconnect = false;
            SessionError = false;
            err = NULL;
            if( !Linked ) {
                HCURSOR cursor = SetCursor( LoadCursor( NULL, IDC_WAIT ) );
                err = RemoteLink( NULL, true );
                SetCursor( cursor );
            }
            EnableMenus( hwnd, true, false );
            if( err != NULL ) {
                ServError( err );
                ServTerminateWin( 1 );
            } else {
                Linked = true;
                while( !Disconnect ) {
                    MSG         peek;
                    if( PeekMessage( &peek, (HWND)0, 0, (UINT)-1, PM_NOREMOVE ) ) {
                        if( !GetMessage( (LPVOID)&peek, (HWND)0, 0, 0 ) ) {
                            Disconnect = true;
                            Exit = true;
                            break;
                        }
                        TranslateMessage( &peek );
                        DispatchMessage( &peek );
                    }
                    if( RemoteConnect() ) {
                        Connected = true;
                        ShowWindow( hwnd, SW_MINIMIZE );
                        UpdateWindow( hwnd );
                        hMenu = GetMenu( hwnd );
                        EnableMenus( hwnd, true, true );
                        Session();
                        EnableMenus( hwnd, true, false );
                        ShowWindow( hwnd, SW_RESTORE );
                        RemoteDisco();
                        Connected = false;
                        if( OneShot )
                            ServTerminateWin( 0 );
                        break;
                    }
                    NothingToDo();
                }
            }
            EnableMenus( hwnd, false, false );
            if( !Disconnect && !SessionError ) {
                SendMessage( hwndMain, WM_COMMAND, MENU_CONNECT, 0 );
            }
            if( Exit )
                ServTerminateWin( 0 );
            break;
        case MENU_OPTIONS:
            dlgproc = MakeProcInstance_DLG( OptionsDlgProc, Instance );
            if( Linked )
                RemoteUnLink();
            Linked = false;
            DialogBox( Instance, "Options", hwnd, dlgproc );
            FreeProcInstance_DLG( dlgproc );
            break;
        case MENU_EXIT:
            Disconnect = true;
            Exit = true;
            ServTerminateWin( 0 );
            break;
        case MENU_BREAK:
#if defined( _M_I86 )
            OutputLine( "Press CTRL-ALT-F to interrupt the program" );
#else
            {
                DWORD   osver;

                osver = GetVersion();
                if( osver >= 0x80000000 && LOBYTE( LOWORD( osver ) ) < 4 ) {
                    OutputLine( "You must press CTRL-ALT-F11 to interrupt a program under Win32s" );
                }
            }
#endif
            break;
        }
        break;

    case WM_DESTROY:
        ServTerminateWin( 0 );
        break;

    case WM_CLOSE:
        if( Connected )
            return( 0 );
        /* fall through to default */
    default:
        return( DefWindowProc( hwnd, msg, wparam, lparam ) );
    }
    return( 0L );

    #undef ServTerminateWin
} /* WindowProc */


void ServError( const char *msg )
{
    ShowWindow( hwndMain, SW_RESTORE );
    OutputLine( msg );
    SessionError = true;
}

void StartupErr( const char *err )
{
    OutputLine( err );
}

void ServMessage( const char *msg )
{
    /* unused parameters */ (void)msg;
}

int WantUsage( const char *ptr )
{
    return( *ptr == '?' );
}

void OutputLine( const char *str )
{
    MessageBox( NULL, str, TRP_The_WATCOM_Debugger, MB_APPLMODAL+MB_OK );
}
