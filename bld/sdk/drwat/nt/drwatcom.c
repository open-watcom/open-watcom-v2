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


#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include "drwatcom.h"
#include "memwnd.h"
#include "mem.h"
#include "srchmsg.h"
#include "jdlg.h"
#include "madrtn.h"
#include "reglist.h"

static char     className[] = "drwatcom_nt";

#define CMD_LINE_BUF_SIZE       1024
#define WIN95_PATH              "\\binnt\\_drwin95.exe"

extern msglist ThreadWaitMsgs[];
extern msglist ThreadStateMsgs[];
extern msglist Actions[];
extern msglist ExceptionMsgs[];

static BOOL initRCStrings( void ) {

    SetInstance( Instance );
    AppName = AllocRCString( STR_APP_NAME );
    if( AppName == NULL ) return( FALSE );
    if( !InitSrchTable( Instance, ThreadWaitMsgs ) ) return( FALSE );
    if( !InitSrchTable( Instance, ThreadStateMsgs ) ) return( FALSE );
    if( !InitSrchTable( Instance, Actions ) ) return( FALSE );
    if( !InitSrchTable( Instance, ExceptionMsgs ) ) return( FALSE );
    if( !InitSrchTable( Instance, MADMsgs ) ) return( FALSE );
    return( TRUE );
}

static BOOL initClass( void ) {
    WNDCLASS            wc;

    wc.style = 0L;
    wc.lpfnWndProc = MainWindowProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = Instance;
    wc.hIcon = LoadIcon( Instance, "APPLICON" );
    wc.hCursor = LoadCursor( NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);
    wc.lpszMenuName = "APPLMENU";
    wc.lpszClassName = className;
    if( !RegisterClass( &wc ) ) {
        return( FALSE );
    }
    return( TRUE );
}

int PASCAL WinMain( HINSTANCE currinst, HINSTANCE previnst, LPSTR cmdline, int cmdshow)
{
    MSG                 msg;
    HWND                prev_hwnd;
    HANDLE              thisthread;
    HANDLE              thisprocess;
    MemWndConfig        memwndcfg;
    COPYDATASTRUCT      data;
#ifndef CHICAGO
    DWORD               ver;
    STARTUPINFO         startup;
    PROCESS_INFORMATION procinfo;
    BOOL                ret;
    char                cmd[_MAX_PATH];
    char*               env;

    ver = GetVersion();
    if( !( ver < 0x80000000 ) && !( LOBYTE( LOWORD( ver ) ) < 4 ) ) {
        env = getenv( "WATCOM" );
        if( env == NULL ) {
            if( !env ) {
                MessageBox( NULL, "Unable to open DrWatcom for Windows 95",
                            "DrWatcom", MB_OK |  MB_ICONEXCLAMATION );
            }
        } else {
            strcpy( cmd, env );
            strcat( cmd, WIN95_PATH );

            memset( &startup, 0, sizeof( STARTUPINFO ) );
            startup.cb = sizeof( STARTUPINFO );
            ret = CreateProcess( cmd,           /* application path */
                        cmdline,                /* command line */
                        NULL,                   /* process security
                                                        attributes */
                        NULL,                   /* main thread security
                                                        attributes */
                        FALSE,                  /* inherits parent handles */
                        NORMAL_PRIORITY_CLASS,  /* create parameters */
                        NULL,                   /* environment block */
                        NULL,                   /* current directory */
                        &startup,               /* other startup info */
                        &procinfo );            /* structure to get process
                                                        info */
            if( !ret ) {
                MessageBox( NULL, "Unable to open DrWatcom for Windows 95",
                            "DrWatcom", MB_OK |  MB_ICONEXCLAMATION );
            }
        }
        return( 0 );
    }
#endif

    cmdline = cmdline;
    cmdshow = cmdshow;
    previnst = previnst;
    Instance = currinst;
    thisprocess = GetCurrentProcess();
    SetPriorityClass( thisprocess, HIGH_PRIORITY_CLASS );
    if( !initRCStrings() ) {
        // this string cannot be resourcified because it is used
        // when the resourcified strings cannot be found
        MessageBox( NULL, "Unable to access string resource",
                    "DrNT", MB_OK | MB_ICONHAND );
        return( FALSE );
    }

    SetDebugErrorLevel( SLE_WARNING );
    prev_hwnd = FindWindow( className, NULL );
    if( prev_hwnd != NULL ) {
        data.dwData = 0;
        data.cbData = strlen( cmdline ) + 1;
        data.lpData = cmdline;
        SendMessage( prev_hwnd, WM_COPYDATA, (WPARAM)(HWND)NULL,
                     (LPARAM)(PCOPYDATASTRUCT)&data );
        return( 0 );
    }

    /* set this threads priority higher so it doesn't get preempted when the
       costly registry info is refreshed */

    thisthread = GetCurrentThread();
    SetThreadPriority( thisthread, THREAD_PRIORITY_HIGHEST );
    MemStart();
    GetProfileInfo();
    InitRegList();
    if ( JDialogInit() == FALSE ){
        return ( 0 );
    }
    if ( InitMADInfo() == FALSE ){
        JDialogFini();
        return ( 0 );
    }

    if( InitDip() == FALSE ) {
        FiniMADInfo();
        JDialogFini();
        return ( 0 );
    }
    InitReg();

    /*
     * initialize
     */

    if( initClass() == FALSE ) {
        FiniMADInfo();
        JDialogFini();
        return ( 0 );
    }

    if( !RegisterMemWalker() ) {
        FiniMADInfo();
        JDialogFini();
        return( 0 );
   }

    /*
     * set up the memory window
     */
    if( !RegMemWndClass( Instance ) ) {
        FiniMADInfo();
        JDialogFini();
        return( 0 );
    }
    SetDefMemConfig();
    GetMemWndConfig( &memwndcfg );
    memwndcfg.allowmult = WND_SINGLE;
    memwndcfg.forget_pos = TRUE;
    memwndcfg.disp_info = FALSE;
    SetMemWndConfig( &memwndcfg );

    CheckLogSize();
    InitAutoAttatch();

    /*
     * now make the main window
     */
    MainHwnd = CreateWindow(
        className,          /* Window class name */
        AppName,            /* Window caption */
        WS_OVERLAPPEDWINDOW,/* Window style */
        CW_USEDEFAULT,      /* Initial X position */
        0,                  /* Initial Y position */
        CW_USEDEFAULT,      /* Initial X size */
        0,                  /* Initial Y size */
        NULL,               /* Parent window handle */
        NULL,               /* Window menu handle */
        Instance,           /* Program instance handle */
        NULL);              /* Create parameters */

    ShowWindow( MainHwnd, cmdshow );
    UpdateWindow( MainHwnd );

    /* this thread no longer needs high priority since it now has a window
     * and NT assigns higher priority to threads processing user input */

    SetThreadPriority( thisthread, THREAD_PRIORITY_NORMAL );
    ProcessCommandLine( cmdline );
    while( GetMessage( &msg, NULL, 0, 0 ) ) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    JDialogFini();
    FiniMADInfo();
    return( 0 );

}  /* WinMain */


static int alertedState = 0;

static void newIcon( HWND hwnd, HICON icon )
{
    SetClassLong( hwnd, GCL_HICON, (DWORD)icon );
    if( IsIconic( hwnd ) ) {
        ShowWindow( hwnd, SW_HIDE );
        ShowWindow( hwnd, SW_SHOWMINNOACTIVE );
    }
}

void Alert( void )
{
    if( !alertedState ) {
        alertedState = TRUE;
        newIcon( MainHwnd, LoadIcon( Instance, "ERRICON" ) );
    }
}

void ClearAlert( void )
{
    if( alertedState ) {
        alertedState = FALSE;
        newIcon( MainHwnd, LoadIcon( Instance, "APPLICON" ) );
    }
}
