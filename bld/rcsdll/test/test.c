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


/****************************************************************************

    PROGRAM: EditCntl.c

    PURPOSE: Creates an edit window

    FUNCTIONS:

        WinMain() - calls initialization function, processes message loop
        InitApplication() - initializes window data and registers window
        InitInstance() - saves instance handle and creates main window
        MainWndProc() - processes messages
        About() - processes messages for "About" dialog box

    COMMENTS:

        After setting up the application's window, the size of the client
        area is determined and a child window is created to use for editing.

****************************************************************************/

#include <windows.h>
#include <commdlg.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include "test.h"
#include "api.h"
#define WPROJ 5

HANDLE hInst;

BatchCallbackFP         BatchProc;
MessageBoxCallbackFP    MsgProc;
rcsdata                 Cookie;

HANDLE hAccTable;                                /* handle to accelerator table */
HWND hwnd;                    /* handle to main windows  */

/****************************************************************************

    FUNCTION: WinMain(HANDLE, HANDLE, LPSTR, int)

    PURPOSE: calls initialization function, processes message loop

****************************************************************************/

int PASCAL WinMain( HANDLE hInstance,  HANDLE hPrevInstance,
        LPSTR lpCmdLine, int nCmdShow)
{
    MSG msg;
    lpCmdLine = lpCmdLine;

    if (!hPrevInstance) {
        if (!InitApplication(hInstance)) return (FALSE);
    }
    if (!InitInstance(hInstance, nCmdShow))
        return (FALSE);

    while (GetMessage(&msg, NULL, NULL, NULL)) {

        //if( !TranslateAccelerator(hwnd, hAccTable, &msg)
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return (msg.wParam);
}

int RCSAPI Batcher( rcsstring str, void *cookie )
{
    cookie = cookie;
    return( MessageBox( hwnd, (LPCSTR)str, (LPCSTR)"run batch", MB_OK ) );
}

int RCSAPI Messager( rcsstring text, rcsstring title, rcsstring buffer, int len, void *cookie )
{
    cookie = cookie;
    strcpy( buffer, "this is a message" );
    return( MessageBox( hwnd, (LPCSTR)text, (LPCSTR)title, MB_OK ) );
}


/****************************************************************************

    FUNCTION: InitApplication(HANDLE)

    PURPOSE: Initializes window data and registers window class

****************************************************************************/

BOOL InitApplication(HANDLE hInstance)
{
    WNDCLASS  wc;

    wc.style = NULL;
    wc.lpfnWndProc = MainWndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName =  "WatcomEditCntlMenu";
    wc.lpszClassName = "WEditCntlWClass";

    Cookie = RCSInit( hwnd, getenv( "WATCOM" ) );
    BatchProc = (BatchCallbackFP)MakeProcInstance( &Batcher, hInst );
    MsgProc = (MessageBoxCallbackFP)MakeProcInstance( &Messager, hInst );
    RCSRegisterBatchCallback( Cookie, BatchProc, NULL );
    RCSRegisterMessageBoxCallback( Cookie, MsgProc, NULL );

    return (RegisterClass(&wc));
}


/****************************************************************************

    FUNCTION:  InitInstance(HANDLE, int)

    PURPOSE:  Saves instance handle and creates main window

****************************************************************************/

BOOL InitInstance(HANDLE hInstance, int nCmdShow)
{
    hInst = hInstance;
    hAccTable = LoadAccelerators(hInst, "WatcomEditCntlAcc");

    hwnd = CreateWindow(
        "WEditCntlWClass",
        "Watcom EditCntl Sample Application",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        NULL,
        NULL,
        hInstance,
        NULL
    );

    if (!hwnd)
        return (FALSE);

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);
    return (TRUE);

}

/****************************************************************************

    FUNCTION: MainWndProc(HWND, UINT, WPARAM, LPARAM)

    PURPOSE:  Processes messages

    MESSAGES:

        WM_COMMAND    - application menu (About dialog box)
        WM_DESTROY    - destroy window

****************************************************************************/

long FAR PASCAL MainWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    FARPROC lpProcAbout;

    switch (message) {
    case WM_COMMAND:
        switch (wParam) {
        case IDM_ABOUT:
            lpProcAbout = MakeProcInstance((FARPROC)About, hInst);
            DialogBox(hInst, "AboutBox", hWnd, lpProcAbout);
            FreeProcInstance(lpProcAbout);
            break;
        /* RCS menu commands */
        case IDM_CHECKIN:
            RCSCheckin( Cookie, "foo.c" );
            break;
        case IDM_CHECKOUT:
            RCSCheckout( Cookie, "foo.c" );
            break;
        case IDM_RUNSHELL:
            RCSRunShell( Cookie );
            break;
        case IDM_SET_MKS_RCS:
            RCSSetSystem( Cookie, MKS_RCS );
            if( RCSQuerySystem( Cookie ) == MKS_RCS ) {
                SetWindowText( hwnd, "MKS RCS" );
            } else {
                SetWindowText( hwnd, "error setting RCS system" );
            }
            break;
        case IDM_SET_MKS_SI:
            RCSSetSystem( Cookie, MKS_SI );
            if( RCSQuerySystem( Cookie ) == MKS_SI ) {
                SetWindowText( hwnd, "MKS SI" );
            } else {
                SetWindowText( hwnd, "error setting RCS system" );
            }
            break;
        case IDM_SET_PVCS:
            RCSSetSystem( Cookie, PVCS );
            if( RCSQuerySystem( Cookie ) == PVCS ) {
                SetWindowText( hwnd, "PVCS" );
            } else {
                SetWindowText( hwnd, "error setting RCS system" );
            }
            break;
        case IDM_SET_WPROJ:
            RCSSetSystem( Cookie, WPROJ );
            if( RCSQuerySystem( Cookie ) == WPROJ ) {
                SetWindowText( hwnd, "WPROJ" );
            } else {
                SetWindowText( hwnd, "error setting RCS system" );
            }
            break;
        case IDM_SET_GENERIC:
            RCSSetSystem( Cookie, GENERIC );
            if( RCSQuerySystem( Cookie ) == GENERIC ) {
                SetWindowText( hwnd, "GENERIC" );
            } else {
                SetWindowText( hwnd, "error setting RCS system" );
            }
            break;
        case IDM_QUERY_SYS:
            switch( RCSQuerySystem( Cookie ) ) {
                case NO_RCS: SetWindowText( hwnd, "none" ); break;
                case MKS_RCS: SetWindowText( hwnd, "mksrcs" ); break;
                case MKS_SI: SetWindowText( hwnd, "mkssi" ); break;
                case PVCS: SetWindowText( hwnd, "pvcs" ); break;
                case GENERIC: SetWindowText( hwnd, "GENERIC" ); break;
                case WPROJ: SetWindowText( hwnd, "wproj" ); break;
            }
            break;
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return (DefWindowProc(hWnd, message, wParam, lParam));
    }
    return (NULL);
}


/****************************************************************************

    FUNCTION: About(HWND, unsigned, WORD, LONG)

    PURPOSE:  Processes messages for "About" dialog box

    MESSAGES:

        WM_INITDIALOG - initialize dialog box
        WM_COMMAND    - Input received

****************************************************************************/

BOOL FAR PASCAL About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    lParam = lParam;
    switch (message) {
        case WM_INITDIALOG:
            return (TRUE);

        case WM_COMMAND:
            if (wParam == IDOK
                || wParam == IDCANCEL) {
                EndDialog(hDlg, TRUE);
                return (TRUE);
            }
            break;
    }
    return (FALSE);
}
