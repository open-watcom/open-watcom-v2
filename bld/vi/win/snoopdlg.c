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


#include "winvi.h"
#include <string.h>
#include "snoopdlg.h"

static fancy_snoop      snoopData = {TRUE,FALSE,TRUE,TRUE,FALSE,FALSE,0,NULL,0,NULL,0};

/*
 * snoopDlgProc - callback routine for snoop dialog
 */
BOOL WINEXP SeekDlgProc( HWND hwnd, UINT msg, UINT wparam, LONG lparam )
{
    int                 i;
    int                 cmd;
    DWORD               index;
    char                snoop[MAX_INPUT_LINE];

    lparam = lparam;
    switch( msg ) {
    case WM_INITDIALOG:
        CenterWindowInRoot( hwnd );

        EditSubClass( hwnd, SNOOP_STRING, &snoopHist );

        CheckDlgButton( hwnd, SNOOP_IGNORE_CASE, snoopData.case_ignore );
        CheckDlgButton( hwnd, SNOOP_REGULAR_EXPRESSIONS, snoopData.use_regexp );
        SetDlgItemText( hwnd, SNOOP_STRING, snoopData.snoop );
        SetDlgItemText( hwnd, SNOOP_PATH, snoopData.path );

        // default extsion should be grep extension
        SetDlgItemText( hwnd, SNOOP_EXT, snoopData.ext );

        // this isn't quite right. but it's close.
        /*
        for( i=0; i<extension.max; i++ ) {
            SendDlgItemMessage( hwnd, SNOOP_LISTBOX, LB_ADDSTRING, 0,
                (LONG)extension[i] );
        }
        */

        return( TRUE );
    case WM_CLOSE:
        PostMessage( hwnd, WM_COMMAND, GET_WM_COMMAND_MPS( IDCANCEL, 0, 0 ) );
        return( TRUE );
    case WM_COMMAND:
        switch( LOWORD( wparam ) ) {
        case SNOOP_LISTBOX:
            cmd = GET_WM_COMMAND_CMD( wparam, lparam );
            if( cmd == LBN_SELCHANGE || cmd == LBN_DBLCLK ) {
                index = SendDlgItemMessage( hwnd, SNOOP_LISTBOX, LB_GETCURSEL,
                                                        0, 0L );
                if( index == LB_ERR ) {
                    break;
                }
                SendDlgItemMessage( hwnd, SNOOP_LISTBOX, LB_GETTEXT, index,
                                        (LONG) snoop );
                SetDlgItemText( hwnd, SNOOP_STRING, snoop );
            }
            break;
        case IDCANCEL:
            RemoveEditSubClass( hwnd, SNOOP_STRING );
            EndDialog( hwnd, 0 );
            break;
        case IDOK:
            GetDlgItemText( hwnd, SNOOP_STRING, snoopData.snoop, snoopData.snooplen );
            GetDlgItemText( hwnd, SNOOP_EXT, snoopData.ext, snoopData.extlen );
            GetDlgItemText( hwnd, SNOOP_PATH, snoopData.path, snoopData.pathlen );
            snoopData.case_ignore = IsDlgButtonChecked( hwnd, SNOOP_IGNORE_CASE );
            snoopData.use_regexp = IsDlgButtonChecked( hwnd, SNOOP_REGULAR_EXPRESSIONS );
            RemoveEditSubClass( hwnd, SNOOP_STRING );
            EndDialog( hwnd, 1 );
            break;
        default:
            return( FALSE );
        }
        // hand it off to fgrep
        return( TRUE );
    }
    return( FALSE );

} /* snoopDlgProc */

/*
 * GetsnoopStringDialog - create dialog settings
 */
bool GetSnoopStringDialog( fancy_snoop *ff )
{
    DLGPROC     proc;
    bool        rc;

    snoopData.snoop = ff->snoop;
    snoopData.snooplen = ff->snooplen;
    proc = (DLGPROC) MakeProcInstance( (FARPROC) snoopDlgProc, InstanceHandle );
    rc = DialogBox( InstanceHandle, "SNOOPDLG", Root, proc );
    FreeProcInstance( (FARPROC) proc );
    SetWindowCursor();
    if( rc ) {
        ff->case_ignore = snoopData.case_ignore;
        ff->use_regexp = snoopData.use_regexp;
        ff->search_forward = snoopData.search_forward;
        ff->search_wrap = snoopData.search_wrap;
    }
    return( rc );

} /* GetsnoopStringDialog */
