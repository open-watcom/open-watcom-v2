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
* Description:  Display a menu resource.
*
****************************************************************************/


#include <stdio.h>
#include <windows.h>
#include "restest.h"
#include "resname.h"

static char menuName[256];

LRESULT CALLBACK MenuWndProc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam )
{
    WORD        cmd;
    char        buf[256];

    switch( msg ) {
    case WM_COMMAND:
        cmd = LOWORD( wparam );
        sprintf( buf, "Message id: %04X (%d)", cmd, cmd );
        MessageBox( hwnd, buf, "", MB_OK );
        break;
    case WM_DESTROY:
        if( hwnd == AccelHwnd ) AccelHwnd = NULL;
        break;
    default:
        return( DefWindowProc( hwnd, msg, wparam, lparam ) );
    }
    return( FALSE );
}

BOOL RegisterMenuClass( void )
{
    WNDCLASS    wc;

    /* fixed window */
    wc.style = 0L;
    wc.lpfnWndProc = MenuWndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 4;
    wc.hInstance = Instance;
    wc.hIcon = NULL;
    wc.hCursor = LoadCursor( NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = MENU_CLASS;
    if( !RegisterClass( &wc ) ) return( FALSE );

    return( TRUE );
}

BOOL CALLBACK GetMenuNameDlgProc( HWND hwnd, UINT msg, UINT wparam, DWORD lparam )
{
    lparam = lparam;
    switch( msg ) {
    case WM_COMMAND:
        if( LOWORD( wparam ) == IDOK ) {
            GetDlgItemText( hwnd, INPUT_FIELD, menuName, sizeof( menuName ) );
            EndDialog( hwnd, 0 );
        }
        break;
    default:
        return( FALSE );
        break;
    }
    return( TRUE );
}

void DisplayMenu( void )
{
    FARPROC     fp;
    HMENU       menu;
    char        buf[256];
    HWND        hwnd;

    fp = MakeProcInstance( (FARPROC)GetMenuNameDlgProc, Instance );
    DialogBox( Instance, "GET_RES_NAME_DLG" , NULL, (DLGPROC)fp );
    FreeProcInstance( fp );
    menu = LoadMenu( Instance, menuName );
    if( menu == NULL ) {
        sprintf( buf, "Can't Load Menu %s", menuName );
        Error( "menu", buf );
        return;
    }
    hwnd = CreateWindow(
        MENU_CLASS,             /* Window class name */
        "Menu test window",     /* Window caption */
        WS_OVERLAPPEDWINDOW,    /* Window style */
        CW_USEDEFAULT,          /* Initial X position */
        CW_USEDEFAULT,          /* Initial Y position */
        500,                    /* Initial X size */
        200,                    /* Initial Y size */
        NULL,                   /* Parent window handle */
        menu,                   /* Window menu handle */
        Instance,               /* Program instance handle */
        NULL );                 /* Create parameters */
    if( hwnd == NULL ) return;
    ShowWindow( hwnd, SW_SHOW );
    UpdateWindow( hwnd );
}
