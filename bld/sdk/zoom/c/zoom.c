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
* Description:  Screen zoom utility.
*
****************************************************************************/


#include <string.h>
#include "bool.h"
#include "wzoom.h"
#ifndef NOUSE3D
#include "ctl3dcvr.h"
#endif

#define MAIN_CLASS      "WZOOM_FIXED_CLASS"
#define FLOAT_CLASS     "WZOOM_FLOATING_CLASS"

/*
 * FirstInstInit - register classes and do other initializiation that
 *                 is only done by the first instance of the zoom
 */
static BOOL FirstInstInit( void )
{
    WNDCLASS    wc;

    /* fixed window */
    wc.style = 0L;
    wc.lpfnWndProc = ZOOMMainWndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = sizeof( LONG_PTR );
    wc.hInstance = Instance;
    wc.hIcon = LoadIcon( Instance, "APPLICON" );
    wc.hCursor = LoadCursor( NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);
    wc.lpszMenuName = "ZOOM_MENU";
    wc.lpszClassName = MAIN_CLASS;
    if( !RegisterClass( &wc ) ) return( FALSE );

    return( TRUE );
}

/*
 * EveryInstInit - do initialization required by every instance of zoom
 */
static BOOL EveryInstInit( int cmdshow )
{
    MainWndInfo         *info;

#ifndef NOUSE3D
    CvrCtl3DInit( Instance );
    CvrCtl3dRegister( Instance );
    CvrCtl3dAutoSubclass( Instance );
#endif
    MemStart();
    LoadConfig();

    AppName = AllocRCString( STR_APP_NAME );
    info = MemAlloc( sizeof( MainWndInfo ) );
    memset( info, 0, sizeof( MainWndInfo ) );

    MainWin = CreateWindow(
        MAIN_CLASS,             /* Window class name */
        AppName,                /* Window caption */
        WS_OVERLAPPEDWINDOW,    /* Window style */
        CW_USEDEFAULT,          /* Initial X position */
        CW_USEDEFAULT,          /* Initial Y position */
        200,                    /* Initial X size */
        200,                    /* Initial Y size */
        NULL,                   /* Parent window handle */
        NULL,                   /* Window menu handle */
        Instance,               /* Program instance handle */
        info);                  /* Create parameters */
    if( MainWin == NULL ) return( FALSE );
    ShowWindow( MainWin, cmdshow );
    UpdateWindow( MainWin );
    return( TRUE );
}

int PASCAL WinMain( HINSTANCE currinst, HINSTANCE previnst, LPSTR cmdline, int cmdshow)
{
    MSG         msg;

    cmdline = cmdline;
    Instance = currinst;
    SetInstance( Instance );
    if( previnst == NULL ) {
        if( !FirstInstInit() ) return( 0 );
    }
    if( !EveryInstInit( cmdshow ) ) return( 0 );

    while( GetMessage( &msg, NULL, 0, 0 ) ) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
#ifndef NOUSE3D
    CvrCtl3dUnregister( Instance );
    CvrCtl3DFini( Instance );
#endif
    return( 1 );
}
