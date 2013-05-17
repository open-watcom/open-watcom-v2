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
* Description:  Auto-save confirmation dialog.
*
****************************************************************************/


#include "vi.h"
#include "asavedlg.h"

/*
 * ASaveDlgProc - callback routine for autosave response dialog
 */
WINEXPORT BOOL CALLBACK ASaveDlgProc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam )
{
    lparam = lparam;
    switch( msg ) {
    case WM_INITDIALOG:
        CenterWindowInRoot( hwnd );
        return( TRUE );
    case WM_CLOSE:
        PostMessage( hwnd, WM_COMMAND, IDCANCEL, 0L );
        return( TRUE );
    case WM_COMMAND:
        switch( LOWORD( wparam ) ) {
        case DLG_IGNORE:
            EndDialog( hwnd, 'i' );
            break;
        case IDCANCEL:
            EndDialog( hwnd, 'q' );
            break;
        case DLG_RECOVER:
            EndDialog( hwnd, 'r' );
            break;
        default:
            return( FALSE );
        }
        return( TRUE );
    }
    return( FALSE );

} /* ASaveDlgProc */

/*
 * GetAutosaveResponse - create dialog for controlling settings
 */
vi_key GetAutosaveResponse( void )
{
    DLGPROC     proc;
    int         rc;

    proc = (DLGPROC)MakeProcInstance( (FARPROC)ASaveDlgProc, InstanceHandle );
    rc = DialogBox( InstanceHandle, "ASaveDlg", (HWND)NULLHANDLE, proc );
    FreeProcInstance( (FARPROC)proc );

    return( rc );

} /* GetAutosaveResponse */
