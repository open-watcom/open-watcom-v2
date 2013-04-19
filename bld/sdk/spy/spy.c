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
* Description:  Main source file for the spy.
*
****************************************************************************/


#include "spy.h"
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

#ifdef __WATCOMC__
extern WORD _STACKLOW;
#endif

/*
 * spyInit - initialization
 */
static BOOL spyInit( HANDLE currinst, HANDLE previnst, int cmdshow )
{
    WNDCLASS    wc;
    HANDLE      memhdl;
    WORD        i;

    i=i;
    memhdl = memhdl;    /* shut up the compiler for non-NT version */
    JDialogInit();
    Instance = currinst;
    ResInstance = currinst;
    if( !InitGblStrings() ) {
        return( FALSE );
    }
    SpyMenu = LoadMenu( ResInstance, "SPYMENU" );
#ifdef __WATCOMC__
    _STACKLOW = 0;
#endif
    MemStart();

    HandleMessageInst = MakeProcInstance( (FARPROC) HandleMessage, Instance );
    HintWndInit( Instance, NULL, 0 );


    /*
     * set up window class
     */
    if( !previnst ) {
        wc.style = 0L;
        wc.lpfnWndProc = (LPVOID)SpyWindowProc;
        wc.cbClsExtra = 0;
        wc.cbWndExtra = sizeof( LONG_PTR );
        wc.hInstance = Instance;
        wc.hIcon = LoadIcon( ResInstance, "APPLICON" );
        wc.hCursor = LoadCursor( (HANDLE)NULL, IDC_ARROW);
#ifdef __NT__
        wc.hbrBackground = NULL;
#else
        wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
#endif
        wc.lpszMenuName = NULL;
        wc.lpszClassName = SPY_CLASS_NAME;
        if( !RegisterClass( &wc ) ) {
            return( FALSE );
        }

#ifdef USE_SNAP_WINDOW
        if( !RegisterSnapClass( Instance ) ) {
            return( FALSE );
        }
#endif
    }

#ifndef NOUSE3D
    CvrCtl3DInit( Instance );
    CvrCtl3dRegister( Instance );
    CvrCtl3dAutoSubclass( Instance );
#endif

    /*
     * now make the main window
     */
    LoadSpyConfig( NULL );
    SpyMainWindow = CreateWindow(
        SPY_CLASS_NAME,         /* Window class name */
        SpyName,                /* Window caption */
        WS_OVERLAPPEDWINDOW,    /* Window style */
        SpyMainWndInfo.xpos,    /* Initial x position */
        SpyMainWndInfo.ypos,    /* Initial y position */
        SpyMainWndInfo.xsize,   /* Initial x size */
        SpyMainWndInfo.ysize,   /* Initial y size */
        (HWND)NULL,             /* Parent window handle */
        (HMENU)SpyMenu,         /* Window menu handle */
        Instance,               /* Program instance handle */
        NULL );                 /* Create parameters */

    if( SpyMainWindow == NULL ) {
        return( FALSE );
    }
    MyTask = GetWindowTask( SpyMainWindow );

    ShowWindow( SpyMainWindow, cmdshow );
    UpdateWindow( SpyMainWindow );

    InitMessages();
    return( TRUE );

} /* spyInit */

/*
 * SpyFini - cleanup at end
 */
void SpyFini( void )
{
#ifndef NOUSE3D
    CvrCtl3dUnregister( Instance );
    CvrCtl3DFini( Instance );
#endif
    ClearFilter();
    SpyLogClose();
    SaveSpyConfig( NULL );
    JDialogFini();

} /* SpyFini */

/*
 * WinMain - main entry point
 */
int WINMAINENTRY WinMain( HINSTANCE currinst, HINSTANCE previnst, LPSTR cmdline,
                          int cmdshow )
{
    MSG         msg;
    HWND        prev_hwnd;
    char        *errstr;

    cmdline = cmdline;
    errstr = errstr;
    SetInstance( currinst );
#ifdef __NT__

    /* because of the shared memory used by the NT spy we can only allow
     * one spy to exist at a time */

    prev_hwnd = FindWindow( SPY_CLASS_NAME, NULL );
    if( prev_hwnd != NULL ) {
        errstr = GetRCString( STR_ONLY_1_SPY_ALLOWED );
        MessageBox( NULL, errstr, SpyName, MB_OK | MB_ICONEXCLAMATION );

        /* setting the window topmost then not topmost is a kludge to
         * make sure the window is really moved to the top */

        SetWindowPos( prev_hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE );
        SetWindowPos( prev_hwnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE );
        exit( 0 );
    }
#else
    prev_hwnd = prev_hwnd;
#endif
    if( !spyInit( currinst, previnst, cmdshow ) ) {
        exit( 0 );
    }

    while( GetMessage( &msg, (HWND)NULL, 0, 0 ) ) {
        TranslateMessage( &msg );
        DispatchMessage( &msg );
    }
    SpyFini();
    return( 1 );

} /* WinMain */

