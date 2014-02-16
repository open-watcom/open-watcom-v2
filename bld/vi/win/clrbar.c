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
* Description:  Colour selection dialog.
*
****************************************************************************/


#include "vi.h"
#include "clrbar.h"
#include "utils.h"
#include "wprocmap.h"

HWND        hColorbar;

/*
 * ClrDlgProc - callback routine for colour drag & drop dialog
 */
WINEXPORT BOOL CALLBACK ClrDlgProc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam )
{
    lparam = lparam;
    wparam = wparam;
    hwnd = hwnd;

    switch( msg ) {
    case WM_INITDIALOG:
        hColorbar = hwnd;
        MoveWindowTopRight( hwnd );
        return( TRUE );
    case WM_COMMAND:
        switch( LOWORD( wparam ) ) {
            case IDOK:
            case IDCANCEL:
                EndDialog( hwnd, TRUE );
                return( TRUE );
        }
        break;
    case WM_CLOSE:
        DestroyWindow( hwnd );
        hColorbar = (HWND)NULLHANDLE;
        // update editflags (may have closed from system menu)
        EditFlags.Colorbar = FALSE;
        break;
    }
    return( FALSE );

} /* ClrDlgProc */

/*
 * RefreshColorbar - turn color bar on/off to reflect current editflag state
 */
void RefreshColorbar( void )
{
    static FARPROC      proc = NULL;

    if( EditFlags.Colorbar ) {
        if( hColorbar != NULL ) {
            return;
        }
        // if( proc ){
        //     proc = NULL;
        // }
        proc = MakeDlgProcInstance( ClrDlgProc, InstanceHandle );
        hColorbar = CreateDialog( InstanceHandle, "CLRBAR", Root, (DLGPROC)proc );
        SetMenuHelpString( "Left button = foreground, right button = background.  Ctrl affects all syntax elements" );
    } else {
        if( hColorbar == NULL ) {
            return;
        }
        SendMessage( hColorbar, WM_CLOSE, 0, 0L );
        FreeProcInstance( proc );
        SetMenuHelpString( "" );
    }
    UpdateStatusWindow();

} /* RefreshColorbar */
