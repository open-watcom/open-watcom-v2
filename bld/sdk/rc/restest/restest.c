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
#ifndef __NT__
#include <ver.h>
#endif
#include "restest.h"

HANDLE          Instance;
HWND            MainHwnd;
HWND            AccelHwnd;
HACCEL          Accel;

/*
 * FirstInstInit - register classes and do other initializiation that
 *                 is only done by the first instance of the spy
 */
static BOOL FirstInstInit()
{
    WNDCLASS    wc;

    /* fixed window */
    wc.style = 0L;
    wc.lpfnWndProc = (LPVOID) MainWndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 4;
    wc.hInstance = Instance;
    wc.hIcon = NULL;
    wc.hCursor = LoadCursor( NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);
    wc.lpszMenuName = "RESTEST_MENU";
    wc.lpszClassName = MAIN_CLASS;
    if( !RegisterClass( &wc ) ) return( FALSE );
    if( !RegisterMenuClass() ) return( FALSE );
    if( !RegisterBitmapClass() ) return( FALSE );

    return( TRUE );
}

/*
 * EveryInstInit - do initialization required by every instance of the spy
 */
static BOOL EveryInstInit( int cmdshow ) {

    MainHwnd = CreateWindow(
        MAIN_CLASS,             /* Window class name */
        "WRC test shell",       /* Window caption */
        WS_OVERLAPPEDWINDOW,    /* Window style */
        CW_USEDEFAULT,          /* Initial X position */
        CW_USEDEFAULT,          /* Initial Y position */
        500,                    /* Initial X size */
        200,                    /* Initial Y size */
        NULL,                   /* Parent window handle */
        NULL,                   /* Window menu handle */
        Instance,               /* Program instance handle */
        NULL );                 /* Create parameters */
    if( MainHwnd == NULL ) return( FALSE );
    ShowWindow( MainHwnd, cmdshow );
    UpdateWindow( MainHwnd );
    return( TRUE );
}

int PASCAL WinMain( HANDLE currinst, HANDLE previnst, LPSTR cmdline, int cmdshow)
{
    MSG         msg;

    cmdline = cmdline;
    Instance = currinst;
    if( previnst == NULL ) {
        if( !FirstInstInit() ) return( 0 );
    }
    if( !EveryInstInit( cmdshow ) ) return( 0 );

    while( GetMessage( &msg, NULL, 0, 0 ) ) {
        if( AccelHwnd != NULL ) {
            if( TranslateAccelerator( AccelHwnd, Accel, &msg ) ) continue;
        }
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return( 1 );
}
