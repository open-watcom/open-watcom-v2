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
#include "winvi.h"
#include "startup.h"

/*
 * StartupProc - callback routine for startup modeless dialog
 */
BOOL WINEXP StartupProc( HWND hwnd, UINT msg, UINT wparam, LONG lparam )
{
    RECT    r;
    int     maxx,maxy;
    int     width;
    int     height;
    int     newx,newy;

    lparam = lparam;
    wparam = wparam;
    hwnd = hwnd;
    switch( msg ) {
    case WM_INITDIALOG:
    GetWindowRect( hwnd, &r );
    maxx = GetSystemMetrics( SM_CXSCREEN );
    maxy = GetSystemMetrics( SM_CYSCREEN );
    width = r.right - r.left;
    height = r.bottom - r.top;
    newx = (maxx - width)/2;
    newy = (maxy - height)/2;
    SetWindowPos( hwnd, HWND_TOPMOST,newx,newy,0,0,
                SWP_NOSIZE );
        SetDlgItemText( hwnd, STARTUP_EDITORNAME, WATCOM_ABOUT_EDITOR );
    return( TRUE );
    }
    return( FALSE );

} /* StartupProc */

static HWND startDlgWindow;
static FARPROC  startDlgProc;

/*
 * ShowStartupDialog - show the startup dialog
 */
void ShowStartupDialog( void )
{
    startDlgProc = MakeProcInstance( (FARPROC) StartupProc, InstanceHandle );
    startDlgWindow = CreateDialog( InstanceHandle, "Startup", (HWND) NULL,
                (DLGPROC) startDlgProc );

} /* ShowStartupDialog */


/*
 * CloseStartupDialog - close the startup dialog
 */
void CloseStartupDialog( void )
{
    if( startDlgWindow == NULL ) {
    return;
    }
    DestroyWindow( startDlgWindow );
    startDlgWindow = NULL;
    (void)FreeProcInstance( (FARPROC) startDlgProc );

} /* CloseStartupDialog */
