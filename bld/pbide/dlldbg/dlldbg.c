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
#include "windows.h"

#define PB_DLLDBG_CLASS         "PB_DLLDBG_CLASS"
#define APPNAME                 "DLL Debugger"

#ifdef __NT__    
    #define hinstance_error(x) ( (x) == NULL )
#else        
    #define hinstance_error(x) ( (x) <= HINSTANCE_ERROR )
#endif

int     PB_DEBUGGER_STUB_OK_TO_END;
int     PB_DEBUGGER_GOT_DLL_LOADED;
static char     *dllName;
static char     fmtBuffer[1024];
static HINSTANCE        dllinst;

#define TIMER_ID 123

void PB_DLL_DEBUGGING_MAIN_LINE( HWND hwnd ) {

    MSG         msg;

    while( GetMessage( &msg, NULL, 0, 0 ) ) {
        if( PB_DEBUGGER_STUB_OK_TO_END ) {
            PB_DEBUGGER_STUB_OK_TO_END = 0;
            DestroyWindow( hwnd );
        }
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    KillTimer( hwnd, TIMER_ID );
}

BOOL __export FAR WINAPI MainWndProc( HWND hwnd, UINT msg, UINT wparam,
                                    LONG lparam )
{
    int         rc;

    switch( msg ) {
    case WM_QUERYOPEN:
        sprintf( fmtBuffer,
                "You are currently debugging\n%s.\nDo you want to quit?",
                dllName );
        rc = MessageBox( hwnd, fmtBuffer, APPNAME, MB_YESNO | MB_ICONQUESTION );
        if( rc == IDYES ) {
            DestroyWindow( hwnd );
        }
        break;
    case WM_DESTROY:
        PostQuitMessage( 0 );
        break;
    default:
        return( DefWindowProc( hwnd, msg, wparam, lparam ) );
    }
    return( FALSE );
}

BOOL InitFirstInst( HANDLE hinst ) {
    WNDCLASS    wc;

    wc.style = 0L;
    wc.lpfnWndProc = (LPVOID) MainWndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hinst;
    wc.hIcon = LoadIcon( hinst, "APPLICON" );
    wc.hCursor = LoadCursor( NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = PB_DLLDBG_CLASS;
    return( RegisterClass( &wc ) );
}

int PASCAL WinMain( HINSTANCE currinst, HINSTANCE previnst, LPSTR cmdline, int cmdshow)
{
    HWND        hwnd;

    currinst = currinst;
    previnst = previnst;
    cmdshow = cmdshow;
    if( !previnst ) {
        if( !InitFirstInst( currinst ) ) {
            MessageBox( NULL, "Cannot Register window Class.", APPNAME,
                        MB_OK | MB_ICONEXCLAMATION );
            return( 0 );
        }
    }
    hwnd = CreateWindow(
        PB_DLLDBG_CLASS,        /* Window class name */
        APPNAME,                /* Window caption */
        WS_OVERLAPPEDWINDOW,    /* Window style */
        CW_USEDEFAULT,          /* Initial X position */
        CW_USEDEFAULT,          /* Initial Y position */
        0,                      /* Initial X size */
        0,                      /* Initial Y size */
        NULL,                   /* Parent window handle */
        NULL,                   /* Window menu handle */
        currinst,               /* Program instance handle */
        NULL );                 /* Create parameters */
    // NYI - check for hwnd == NULL
    if( hwnd == NULL ) {
        MessageBox( NULL, "Unable to create debugger window.", APPNAME,
                    MB_OK | MB_ICONEXCLAMATION );
        return( 0 );
    }
    ShowWindow( hwnd, SW_MINIMIZE );
    UpdateWindow( hwnd );
    SetTimer( hwnd, TIMER_ID, 500, 0L); /* 0.5 seconds */
    SetErrorMode( SEM_NOOPENFILEERRORBOX );
    dllinst = LoadLibrary( cmdline );
    dllName = cmdline;
    PB_DEBUGGER_GOT_DLL_LOADED = TRUE;
    if( hinstance_error(dllinst) ) {
        PB_DEBUGGER_GOT_DLL_LOADED = FALSE;
        PostMessage( hwnd, WM_CLOSE, 0, 0 );
    }
    PB_DLL_DEBUGGING_MAIN_LINE( hwnd );
    if( !hinstance_error(dllinst) ) {
        FreeLibrary( dllinst );
    }
    return( 0 );
}
