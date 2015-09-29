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


#include "vi.h"
#include "linedlg.h"
#include "wprocmap.h"


/* Local Windows CALLBACK function prototypes */
WINEXPORT BOOL CALLBACK GotoLineDlgProc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam );

static char     lineStr[20];
static char     lineLen = sizeof( lineStr ) - 1;
static linenum  *lineVal;

/*
 * GotoLineDlgProc - callback routine for goto line dialog
 */
WINEXPORT BOOL CALLBACK GotoLineDlgProc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam )
{
    lparam = lparam;
    switch( msg ) {
    case WM_INITDIALOG:
        CenterWindowInRoot( hwnd );
        return( TRUE );
    case WM_CLOSE:
        PostMessage( hwnd, WM_COMMAND, GET_WM_COMMAND_MPS( IDCANCEL, 0, 0 ) );
        return( TRUE );
    case WM_COMMAND:
        switch( LOWORD( wparam ) ) {
        case IDCANCEL:
            EndDialog( hwnd, FALSE );
            break;
        case IDOK:
            GetDlgItemText( hwnd, GOTOLINE_EDIT, lineStr, lineLen );
            *lineVal = atol( lineStr );
            if( *lineVal > 0 ) {
                EndDialog( hwnd, TRUE );
            } else {
                EndDialog( hwnd, FALSE );
            }
            break;
        default:
            return( FALSE );
        }
        return( TRUE );
    }
    return( FALSE );

} /* GotoLineDlgProc */

/*
 * GetLineDialog - create dialog box & get result
 */
bool GetLineDialog( linenum *line )
{
    FARPROC     proc;
    bool        rc;

    lineStr[0] = '\0';
    lineVal = line;
    proc = MakeDlgProcInstance( GotoLineDlgProc, InstanceHandle );
    rc = DialogBox( InstanceHandle, "LINEDLG", Root, (DLGPROC)proc );
    FreeProcInstance( proc );
    SetWindowCursor();
    return( rc );

} /* GetLineDialog */
