/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2015-2016 The Open Watcom Contributors. All Rights Reserved.
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
#include "snoop.h"
#include "rcstr.gh"
#ifdef __NT__
    #include <shlobj.h>
#endif
#include "wprocmap.h"


/* Local Windows CALLBACK function prototypes */
#ifdef __NT__
WINEXPORT int CALLBACK BrowseCallbackProc( HWND hwnd, UINT msg, LPARAM lparam, LPARAM data );
#endif
WINEXPORT BOOL CALLBACK SnoopDlgProc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam );

#ifdef __NT__
typedef LPITEMIDLIST    (CALLBACK *PFNSHBFF)( LPBROWSEINFO );
typedef BOOL            (CALLBACK *PFNSHGPFIL)( LPCITEMIDLIST, LPSTR );

static HINSTANCE        hInstShell = NULL;
static PFNSHBFF         pfnSHBrowseForFolder = NULL;
static PFNSHGPFIL       pfnSHGetPathFromIDList = NULL;
#endif

static fancy_find       snoopData =
    { -1, -1, NULL, 0, NULL, 0, NULL, 0, NULL, 0, true, false, true, true, false, false };

#ifdef __NT__

/*
 * BrowseCallbackProc - callback routine for the browse dialog
 */
WINEXPORT int CALLBACK BrowseCallbackProc( HWND hwnd, UINT msg, LPARAM lparam, LPARAM data )
{
    lparam=lparam;
    switch( msg ) {
    case BFFM_INITIALIZED:
        SendMessage( hwnd, BFFM_SETSELECTION, TRUE, data );
        break;
    }
    return( 0 );

} /* BrowseCallbackProc */

#endif

/*
 * snoopDlgProc - callback routine for snoop dialog
 */
WINEXPORT BOOL CALLBACK SnoopDlgProc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam )
{
    // int                 i;
    int                 cmd;
    int                 index;
    char                snoop[MAX_INPUT_LINE];
#ifdef __NT__
    BROWSEINFO          bi;
    char                buffer1[MAX_PATH];
    char                buffer2[MAX_PATH];
    LPITEMIDLIST        pidl;
#endif

#ifdef __NT__
    lparam = lparam;
#endif
    switch( msg ) {
    case WM_INITDIALOG:
        CenterWindowInRoot( hwnd );

        CheckDlgButton( hwnd, SNOOP_IGNORE_CASE, ( snoopData.case_ignore ) ? BST_CHECKED : BST_UNCHECKED );
        CheckDlgButton( hwnd, SNOOP_REGULAR_EXPRESSIONS, ( snoopData.use_regexp ) ? BST_CHECKED : BST_UNCHECKED );
        SetDlgItemText( hwnd, SNOOP_STRING, snoopData.find );
        SetDlgItemText( hwnd, SNOOP_PATH, snoopData.path );

        // default extsion should be grep extension
        SetDlgItemText( hwnd, SNOOP_EXT, snoopData.ext );

        // this isn't quite right. but it's close.
        /*
        for( i = 0; i < extension.max; i++ ) {
            SendDlgItemMessage( hwnd, SNOOP_LISTBOX, LB_ADDSTRING, 0, (LPARAM)extension[i] );
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
                index = (int)SendDlgItemMessage( hwnd, SNOOP_EXT, LB_GETCURSEL, 0, 0L );
                if( index == LB_ERR ) {
                    break;
                }
                SendDlgItemMessage( hwnd, SNOOP_EXT, LB_GETTEXT, index, (LPARAM)snoop );
                SetDlgItemText( hwnd, SNOOP_STRING, snoop );
            }
            break;
#ifdef __NT__
        case SNOOP_BROWSE:
            bi.hwndOwner = hwnd;
            bi.pidlRoot = NULL;
            bi.pszDisplayName = NULL;
            LoadString( GET_HINSTANCE( hwnd ), VI_BROWSE_MSG, buffer1, sizeof( buffer1 ) );
            bi.lpszTitle = buffer1;
            bi.ulFlags = BIF_RETURNONLYFSDIRS;
            bi.lpfn = BrowseCallbackProc;
            GetDlgItemText( hwnd, SNOOP_PATH, buffer2, MAX_PATH );
            bi.lParam = (LPARAM)buffer2;
            if( (pidl = pfnSHBrowseForFolder( &bi )) != NULL ) {
                if( pfnSHGetPathFromIDList( pidl, buffer1 ) ) {
                    SetDlgItemText( hwnd, SNOOP_PATH, buffer1 );
                }
            }
            break;
#endif
        case IDCANCEL:
            // RemoveEditSubClass( hwnd, SNOOP_STRING );
            EndDialog( hwnd, FALSE );
            break;
        case IDOK:
            GetDlgItemText( hwnd, SNOOP_STRING, snoop, MAX_INPUT_LINE );
            ReplaceString( &snoopData.find, snoop );
            GetDlgItemText( hwnd, SNOOP_EXT, snoop, MAX_INPUT_LINE );
            ReplaceString( &snoopData.ext, snoop );
            GetDlgItemText( hwnd, SNOOP_PATH, snoop, MAX_INPUT_LINE );
            ReplaceString( &snoopData.path, snoop );
            snoopData.case_ignore = IsDlgButtonChecked( hwnd, SNOOP_IGNORE_CASE );
            snoopData.use_regexp = IsDlgButtonChecked( hwnd, SNOOP_REGULAR_EXPRESSIONS );
            // RemoveEditSubClass( hwnd, SNOOP_STRING );
            EndDialog( hwnd, TRUE );
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
    FARPROC     proc;
    bool        rc;

#ifdef __NT__
    if( hInstShell == NULL ) {
        hInstShell = GetModuleHandle( "SHELL32.DLL" );
        pfnSHBrowseForFolder = (PFNSHBFF)GetProcAddress( hInstShell, "SHBrowseForFolderA" );
        pfnSHGetPathFromIDList = (PFNSHGPFIL)GetProcAddress( hInstShell, "SHGetPathFromIDListA" );
    }
#endif

    /* set case_ignore and extension defaults based on global settings
     * all other values are saved from the last fgrep before
     */
    snoopData.case_ignore = EditFlags.CaseIgnore;
    ReplaceString( &snoopData.ext, EditVars.GrepDefault );
    *ff = &snoopData; /* data is no longer copied */
    proc = MakeDlgProcInstance( SnoopDlgProc, InstanceHandle );
#ifdef __NT__
    if( pfnSHBrowseForFolder != NULL ) {
        rc = DialogBox( InstanceHandle, "SNOOPDLG95", Root, (DLGPROC)proc );
    } else {
#endif
        rc = DialogBox( InstanceHandle, "SNOOPDLG", Root, (DLGPROC)proc );
#ifdef __NT__
    }
#endif
    FreeProcInstance( proc );
    SetWindowCursor();
    return( rc );

} /* GetsnoopStringDialog */
