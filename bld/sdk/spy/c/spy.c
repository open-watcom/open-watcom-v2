/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2019 The Open Watcom Contributors. All Rights Reserved.
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
#include "spydll.h"
#include "log.h"
#include "spymem.h"


#ifdef __WATCOMC__
extern unsigned     _STACKLOW;
#endif

#ifdef __WINDOWS__
static message_func *HandleMessageInst;
#endif

/*
 * spyInit - initialization
 */
static bool spyInit( HANDLE currinst, HANDLE previnst, int cmdshow )
{
    WNDCLASS    wc;

    JDialogInit();
    Instance = currinst;
    ResInstance = currinst;
    if( !InitGblStrings() ) {
        return( false );
    }
    SpyMenu = LoadMenu( ResInstance, "SPYMENU" );
#ifdef __WATCOMC__
    _STACKLOW = 0;
#endif

#ifdef __WINDOWS__
    HandleMessageInst = (message_func *)MakeProcInstance( (FARPROC)HandleMessage, Instance );
    SetFilterProc( HandleMessageInst );
#endif
    HintWndInit( Instance, NULL, 0 );


    /*
     * set up window class
     */
    if( !previnst ) {
        wc.style = 0L;
        wc.lpfnWndProc = SpyWindowProc;
        wc.cbClsExtra = 0;
        wc.cbWndExtra = sizeof( LONG_PTR );
        wc.hInstance = Instance;
        wc.hIcon = LoadIcon( ResInstance, "APPLICON" );
        wc.hCursor = LoadCursor( (HANDLE)NULL, IDC_ARROW);
#ifdef __WINDOWS__
        wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
#else
        wc.hbrBackground = NULL;
#endif
        wc.lpszMenuName = NULL;
        wc.lpszClassName = SPY_CLASS_NAME;
        if( !RegisterClass( &wc ) ) {
            return( false );
        }

#ifdef __NT__
        if( !RegisterSnapClass( Instance ) ) {
            return( false );
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
        return( false );
    }
    MyTask = GetWindowTask( SpyMainWindow );

    ShowWindow( SpyMainWindow, cmdshow );
    UpdateWindow( SpyMainWindow );

    InitMessages();
    return( true );

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
    LogFini();
    SaveSpyConfig( NULL );
#ifdef __WINDOWS__
    FreeProcInstance( (FARPROC)HandleMessageInst );
#endif
    JDialogFini();

} /* SpyFini */

/*
 * WinMain - main entry point
 */
int WINMAINENTRY WinMain( HINSTANCE currinst, HINSTANCE previnst, LPSTR cmdline, int cmdshow )
{
    MSG         msg;
    int         rc;
#ifdef __NT__
    HWND        prev_hwnd;
    const char  *errstr;
#endif

    /* unused parameters */ (void)cmdline;

    rc = 1;
    MemOpen();
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
        rc = 0;
    }
#endif
    if( rc && !spyInit( currinst, previnst, cmdshow ) ) {
        rc = 0;
    }
    if( rc ) {
        while( GetMessage( &msg, (HWND)NULL, 0, 0 ) ) {
            TranslateMessage( &msg );
            DispatchMessage( &msg );
        }
        SpyFini();
    }
    MemClose();
    return( rc );

} /* WinMain */

