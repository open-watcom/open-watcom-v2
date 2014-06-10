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
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <dos.h>
#include <io.h>
#include "heapwalk.h"
#include "wdebug.h"
#include "jdlg.h"

static char className[] = "watheapwalk";


/*
 * heapWalkInit - initialization
 */
static BOOL heapWalkInit( HANDLE currinst, HANDLE previnst, int cmdshow )
{
    WNDCLASS    wc;
    int         x,y;

    Instance = currinst;
    if( !InitStringTable() ) {
        return( FALSE );
    }
    SetInstance( Instance );
    CvrCtl3DInit( Instance );
    CvrCtl3dRegister( Instance );
    CvrCtl3dAutoSubclass( Instance );
    MemStart();
    JDialogInit();
    if( CheckWin386Debug() == WGOD_VERSION ) {
        WDebug386 = TRUE;
    }
    SetDefMemConfig();
    ReadConfig();

    /*
     * set up window class
     */
    if( !previnst ) {

        /* main window */
        wc.style = 0L;
        wc.lpfnWndProc = (WNDPROC)HeapWalkProc;
        wc.cbClsExtra = 0;
        wc.cbWndExtra = 4;
        wc.hInstance = Instance;
        wc.hIcon = LoadIcon( Instance, "APPLICON" );
        wc.hCursor = LoadCursor( NULL, IDC_ARROW);
        wc.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);
        wc.lpszMenuName = "HEAPMENU";
        wc.lpszClassName = className;
        if( !RegisterClass( &wc ) ) return( FALSE );

        /* window for bitmaps, icons, etc. */
        wc.style = 0L;
        wc.lpfnWndProc = (WNDPROC)ItemDisplayProc;
        wc.cbClsExtra = 0;
        wc.cbWndExtra = 6;
        wc.hInstance = Instance;
        wc.hIcon = NULL;
        wc.hCursor = LoadCursor( NULL, IDC_ARROW);
        wc.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);
        wc.lpszMenuName = NULL;
        wc.lpszClassName = ITEM_DISPLAY_CLASS;
        if( !RegisterClass( &wc ) ) return( FALSE );

        /* window for local heap */
        wc.style = 0L;
        wc.lpfnWndProc = (WNDPROC)LocalHeapProc;
        wc.cbClsExtra = 0;
        wc.cbWndExtra = 4;
        wc.hInstance = Instance;
        wc.hIcon = NULL;
        wc.hCursor = LoadCursor( NULL, IDC_ARROW);
        wc.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);
        wc.lpszMenuName = "LOCALHEAPMENU";
        wc.lpszClassName = LOCAL_DISPLAY_CLASS;
        if( !RegisterClass( &wc ) ) return( FALSE );

        /* window for local heap monitoring */
        wc.style = 0L;
        wc.lpfnWndProc = (WNDPROC)LocalMonitorProc;
        wc.cbClsExtra = 0;
        wc.cbWndExtra = 4;
        wc.hInstance = Instance;
        wc.hIcon = NULL;
        wc.hCursor = LoadCursor( NULL, IDC_ARROW);
        wc.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);
        wc.lpszMenuName = NULL;
        wc.lpszClassName = LOCAL_MONITOR_CLASS;
        if( !RegisterClass( &wc ) ) return( FALSE );

        RegMemWndClass( Instance );
        RegPushWin( Instance );
        RegTextBox( Instance );
    }

    InitializeStringTables();

    /*
     * now make the main window
     */
    x = GetSystemMetrics( SM_CXSCREEN );
    y = GetSystemMetrics( SM_CYSCREEN );
    HeapWalkMainWindow = CreateWindow(
        className,          /* Window class name */
        HeapWalkName,       /* Window caption */
        WS_OVERLAPPEDWINDOW,/* Window style */
        Config.glob_xpos,   /* Initial X position */
        Config.glob_ypos,   /* Initial Y position */
        Config.glob_xsize,  /* Initial X size */
        Config.glob_ysize,  /* Initial Y size */
        NULL,               /* Parent window handle */
        NULL,               /* Window menu handle */
        Instance,           /* Program instance handle */
        NULL);              /* Create parameters */

    if( HeapWalkMainWindow == NULL ) return( FALSE );

    ShowWindow( HeapWalkMainWindow, cmdshow );
    UpdateWindow( HeapWalkMainWindow );
    return( TRUE );

} /* heapWalkInit */

/*
 * WinMain - main entry point
 */
int PASCAL WinMain( HINSTANCE currinst, HINSTANCE previnst, LPSTR cmdline, int cmdshow)
{
    MSG         msg;

    cmdline = cmdline;
    if( !heapWalkInit( currinst, previnst, cmdshow ) ) {
        return( 0 );
    }

    while( GetMessage( &msg, NULL, 0, 0 ) ) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    JDialogFini();
    return( 1 );

} /* WinMain */
