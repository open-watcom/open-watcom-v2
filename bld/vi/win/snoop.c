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
#include "snoop.h"

static fancy_find       snoopData = {TRUE,FALSE,TRUE,TRUE,FALSE,FALSE,0,NULL,0,NULL,0,NULL,0};

/*
 * snoopDlgProc - callback routine for snoop dialog
 */
BOOL WINEXP SnoopDlgProc( HWND hwnd, UINT msg, UINT wparam, LONG lparam )
{
    // int                      i;
    int                 cmd;
    DWORD               index;
    char                snoop[MAX_INPUT_LINE];

    lparam = lparam;
    switch( msg ) {
    case WM_INITDIALOG:
        CenterWindowInRoot( hwnd );

        CheckDlgButton( hwnd, SNOOP_IGNORE_CASE, snoopData.case_ignore );
        CheckDlgButton( hwnd, SNOOP_REGULAR_EXPRESSIONS, snoopData.use_regexp );
        SetDlgItemText( hwnd, SNOOP_STRING, snoopData.find );
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
        case SNOOP_EXT:
            cmd = GET_WM_COMMAND_CMD( wparam, lparam );
            if( cmd == LBN_SELCHANGE || cmd == LBN_DBLCLK ) {
                index = SendDlgItemMessage( hwnd, SNOOP_EXT, LB_GETCURSEL,
                                                        0, 0L );
                if( index == LB_ERR ) {
                    break;
                }
                SendDlgItemMessage( hwnd, SNOOP_EXT, LB_GETTEXT, index,
                                        (LONG) snoop );
                SetDlgItemText( hwnd, SNOOP_STRING, snoop );
            }
            break;
        case IDCANCEL:
            // RemoveEditSubClass( hwnd, SNOOP_STRING );
            EndDialog( hwnd, 0 );
            break;
        case IDOK:
            GetDlgItemText( hwnd, SNOOP_STRING, snoop, MAX_INPUT_LINE );
            AddString2( &snoopData.find, snoop );
            GetDlgItemText( hwnd, SNOOP_EXT, snoop, MAX_INPUT_LINE );
            AddString2( &snoopData.ext, snoop );
            GetDlgItemText( hwnd, SNOOP_PATH, snoop, MAX_INPUT_LINE );
            AddString2( &snoopData.path, snoop );
            snoopData.case_ignore = IsDlgButtonChecked( hwnd, SNOOP_IGNORE_CASE );
            snoopData.use_regexp = IsDlgButtonChecked( hwnd, SNOOP_REGULAR_EXPRESSIONS );
            // RemoveEditSubClass( hwnd, SNOOP_STRING );
            EndDialog( hwnd, 1 );
            break;
        default:
            return( FALSE );
        }
        // hand it off to fgrep
        return( TRUE );
    }
    return( FALSE );

} /* SnoopDlgProc */

/*
 * GetsnoopStringDialog - create dialog settings
 */
bool GetSnoopStringDialog( fancy_find **ff )
{
    DLGPROC     proc;
    bool        rc;

    /* set case_ignore and extension defaults based on global settings
     * all other values are saved from the last fgrep before
     */
    snoopData.case_ignore = EditFlags.CaseIgnore;
    AddString2( &snoopData.ext, GrepDefault );
    *ff = &snoopData; /* data is no longer copied */
    proc = (DLGPROC) MakeProcInstance( (FARPROC) SnoopDlgProc, InstanceHandle );
    rc = DialogBox( InstanceHandle, "SNOOPDLG", Root, proc );
    FreeProcInstance( (FARPROC) proc );
    SetWindowCursor();
    return( rc );

} /* GetsnoopStringDialog */
