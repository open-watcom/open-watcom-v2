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
#include <assert.h>
#include "winvi.h"
#include "tagdlg.h"

static int      tagCnt;
static char     **tagList;

/*
 * TagListProc - handle the tag selection dialog
 */
BOOL WINEXP TagListProc( HWND hwnd, UINT msg, UINT wparam, LONG lparam )
{
    int         i;

    switch( msg ) {
    case WM_INITDIALOG:
        CenterWindowInRoot( hwnd );
        SetDlgItemText( hwnd, TAGS_TAGNAME, (LPSTR) lparam );
        for( i=0;i<tagCnt;i++ ) {
            SendDlgItemMessage( hwnd, TAGS_LISTBOX, LB_ADDSTRING, 0,
                                (LONG) (LPSTR) tagList[i] );
        }
        SendDlgItemMessage( hwnd, TAGS_LISTBOX, LB_SETCURSEL, 0, 0L );
        return( TRUE );
    case WM_COMMAND:
        switch( LOWORD( wparam ) ) {
        case IDCANCEL:
            EndDialog( hwnd, -1 );
            break;
        case IDOK:
            i = SendDlgItemMessage( hwnd, TAGS_LISTBOX, LB_GETCURSEL, 0, 0L );
            if( i == LB_ERR ) {
                MessageBox( hwnd, "No selection", EditorName,
                                MB_ICONEXCLAMATION | MB_OK );
            } else {
                EndDialog( hwnd, i );
            }
            break;
        case TAGS_LISTBOX:
            if( GET_WM_COMMAND_CMD( wparam, lparam ) == LBN_DBLCLK ) {
                i = SendDlgItemMessage( hwnd, TAGS_LISTBOX, LB_GETCURSEL, 0, 0L );
                EndDialog( hwnd, i );
            }
            break;
        default:
            return( FALSE );
        }
        return( TRUE );
    }
    return( FALSE );

} /* TagListProc */

/*
 * PickATag - create dialog to select a specific tag
 */
int PickATag( int clist, char **list, char *tagname )
{
    DLGPROC     proc;
    int         rc;

    tagCnt = clist;
    tagList = list;

    proc = (DLGPROC) MakeProcInstance( (FARPROC) TagListProc, InstanceHandle );
    rc = DialogBoxParam( InstanceHandle, "TAGS", Root, proc, (LONG) (LPSTR) tagname );
    FreeProcInstance( (FARPROC) proc );
    return( rc );
} /* PickATag */
