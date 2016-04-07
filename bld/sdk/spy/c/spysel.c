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
* Description:  Spy window selection functions.
*
****************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "spy.h"


/* Local Window callback functions prototypes */
WINEXPORT BOOL CALLBACK EnumWindowsFunc( HWND hwnd, LPARAM lparam );
WINEXPORT BOOL CALLBACK ShowInfoProc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam );
WINEXPORT BOOL CALLBACK ShowSelectedDialog( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam );

static HWND     *tmpWndList;
static WORD     tmpWndCnt;
static BOOL     tmpSpyAll;

/*
 * ClearSelectedWindows - get rid of all selected windows
 */
void ClearSelectedWindows( void )
{
    WindowCount = 0;
    MemFree( WindowList );
    WindowList = NULL;

} /* ClearSelectedWindows */

static HWND *doAddSelectedWindow( HWND hwnd, HWND *list, WORD *cnt )
{
    HWND        *ret;

    ret = realloc( list, (*cnt + 1) * sizeof( HWND ) );
    ret[*cnt] = hwnd;
    (*cnt)++;
    return( ret );

} /* AddSelectedWindow */

/*
 * AddSelectedWindow - add a window to the monitor list
 */
void AddSelectedWindow( HWND hwnd )
{
    WindowList = doAddSelectedWindow( hwnd, WindowList, &WindowCount );

} /* AddSelectedWindow */

/*
 * deleteSelectedWindow - remove a window from monitor list
 */
static void deleteSelectedWindow( HWND hwnd )
{
    int         i, j;
    BOOL        found;

    if( tmpWndCnt == 0 ) {
        return;
    }
    found = FALSE;

    for( i = 0; i < tmpWndCnt; i++ ) {
        if( tmpWndList[i] == hwnd ) {
            for( j = i; j < tmpWndCnt - 1; j++ ) {
                tmpWndList[j] = tmpWndList[j + 1];
            }
            found = TRUE;
            break;
        }
    }
    if( found ) {
        tmpWndCnt--;
        tmpWndList = realloc( tmpWndList, tmpWndCnt * sizeof( HWND ) );
    }

} /* deleteSelectedWindow */

static void addFormattedWindow( HWND hwnd );

static int      indentLevel;
static HWND     hWndDialog;

/*
 * EnumWindowsFunc - enumerate all windows
 */
BOOL CALLBACK EnumWindowsFunc( HWND hwnd, LPARAM lparam )
{
    FARPROC     fp;

    if( lparam != 0 ) {
        if( GetParent( hwnd ) != (HWND)lparam ) {
            return( 1 );
        }
    }
    addFormattedWindow( hwnd );

    indentLevel += 3;
    fp = MakeProcInstance( (FARPROC)EnumWindowsFunc, Instance );
    EnumChildWindows( hwnd, (WNDENUMPROC)fp, (LPARAM)hwnd );
    FreeProcInstance( fp );
    indentLevel -= 3;
    return( 1 );

} /* EnumWindowsFunc */


/*
 * addFormattedWindow - add a window to the list box, formatted
 */
static void addFormattedWindow( HWND hwnd )
{
    char        res[512];
    char        name[128];
    char        tmp[5];
    char        lead_bl[128];
    int         i, len;
    if( IsMyWindow( hwnd ) ) {
        return;
    }
    for( i = 0; i < indentLevel; i++ ) {
        lead_bl[i] = ' ';
    }
    lead_bl[i] = 0;

    name[0] = 0;
    len = GetWindowText( hwnd, name, sizeof( name ) );
    name[len] = 0;
    tmp[0] = ' ';
    tmp[1] = 0;
    if( !tmpSpyAll ) {
        for( i = 0; i < tmpWndCnt; i++ ) {
            if( hwnd == tmpWndList[i] ) {
                tmp[0] = '*';
                break;
            }
        }
    }
    sprintf( res, "%s%0*x%s %s", lead_bl, UINT_STR_LEN, (UINT)(pointer_int)hwnd, tmp, name );
    SendDlgItemMessage( (HWND)hWndDialog, SELWIN_LISTBOX, LB_ADDSTRING, 0, (LPARAM)(LPSTR)res );

} /* addFormattedWindow */

/*
 * setUpWindows - set up windows in selection dialog
 */
static void setUpWindows( void )
{
    FARPROC     fp;

    indentLevel = 0;
    SendDlgItemMessage( hWndDialog, SELWIN_LISTBOX, LB_RESETCONTENT, 0, 0L );
    fp = MakeProcInstance( (FARPROC)EnumWindowsFunc, Instance);
    EnumWindows( (WNDENUMPROC)fp, (LPARAM)NULL );
    FreeProcInstance( fp );
    addFormattedWindow( GetDesktopWindow() );

} /* setUpWindows */


/*
 * ShowInfoProc - show info for framed window dialog
 */
BOOL CALLBACK ShowInfoProc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam )
{
    switch( msg ) {
    case WM_INITDIALOG:
        UpdateFramedInfo( hwnd, (HWND)lparam, true );
        return( TRUE );
        break;
#ifndef NOUSE3D
    case WM_SYSCOLORCHANGE:
        CvrCtl3dColorChange();
        break;
#endif
    case WM_CLOSE:
        EndDialog( hwnd, TRUE );
        break;
    case WM_COMMAND:
        switch( LOWORD( wparam ) ) {
        case IDOK:
            EndDialog( hwnd, TRUE );
            break;
        }
        break;
    }
    return( FALSE );

} /* ShowInfoProc */

/*
 * ShowFramedInfo - create the dialog to show info for our framed proc
 */
void ShowFramedInfo( HWND hwnd, HWND framed )
{
    DLGPROC     fp;

    fp = (DLGPROC)MakeProcInstance( (FARPROC)ShowInfoProc, Instance );
    JDialogBoxParam( Instance, "PEEKWIN", (HWND)hwnd, (DLGPROC)fp, (LPARAM)framed );
    FreeProcInstance( (FARPROC)fp );

} /* ShowFramedInfo */

/*
 * ShowSelectedDialog - show all selected windows
 */
BOOL CALLBACK ShowSelectedDialog( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam )
{
    char        resdata[256], ch;
    char        *errstr;
    char        *res;
    LRESULT     top;
    int         sel;
    HWND        id;
    WORD        parm;
    static HWND framedHwnd;
    HWND        ctl;

    lparam = lparam;
    switch( msg ) {
    case WM_INITDIALOG:
        SetDlgMonoFont( hwnd, SELWIN_LISTBOX );
        hWndDialog = hwnd;
        setUpWindows();
        if( tmpSpyAll ) {
            CheckDlgButton( hwnd, SELWIN_SPYALL, BST_CHECKED );
            ctl = GetDlgItem( hwnd, SELWIN_ADD );
            EnableWindow( ctl, FALSE );
            ctl = GetDlgItem( hwnd, SELWIN_DELETE );
            EnableWindow( ctl, FALSE );
        }
        framedHwnd = NULL;
        break;
#ifndef NOUSE3D
    case WM_SYSCOLORCHANGE:
        CvrCtl3dColorChange();
        break;
#endif
    case WM_SETFOCUS:
        setUpWindows();
        break;
    case WM_COMMAND:
        parm = LOWORD( wparam );
        switch( parm ) {
        case SELWIN_SPYALL:
            tmpSpyAll = !tmpSpyAll;
            CheckDlgButton( hwnd, SELWIN_SPYALL, ( tmpSpyAll ) ? BST_CHECKED : BST_UNCHECKED );
            ctl = GetDlgItem( hwnd, SELWIN_ADD );
            EnableWindow( ctl, !tmpSpyAll );
            ctl = GetDlgItem( hwnd, SELWIN_DELETE );
            EnableWindow( ctl, !tmpSpyAll );
            setUpWindows();
            break;
        case SELWIN_LISTBOX:
            if( GET_WM_COMMAND_CMD( wparam, lparam ) == LBN_SELCHANGE ) {
                parm = SELWIN_HILIGHT;
            } else {
                if( GET_WM_COMMAND_CMD( wparam, lparam ) != LBN_DBLCLK ) {
                    break;
                }
                if( tmpSpyAll ) {
                    break;
                }
            }
        case SELWIN_ADD:
        case SELWIN_DELETE:
        case SELWIN_SHOWINFO:
            sel = (int)SendDlgItemMessage( hwnd, SELWIN_LISTBOX, LB_GETCURSEL, 0, 0L );
            if( sel == LB_ERR ) {
                if( parm != SELWIN_LISTBOX ) {
                    errstr = GetRCString( STR_NO_CUR_SELECTION );
                    MessageBox( hwnd, errstr, SpyName, MB_OK );
                }
                break;
            }
            top = SendDlgItemMessage( hwnd, SELWIN_LISTBOX, LB_GETTOPINDEX, 0, 0L );
            SendDlgItemMessage( hwnd, SELWIN_LISTBOX, LB_GETTEXT, sel, (LPARAM)(LPSTR)resdata );
            res = resdata;
            while( isspace( *res ) ) {
                res++;
            }
            if( res[0] == 0 ) {
                break;
            }
            ch = res[SPYOUT_HWND_LEN];
            res[SPYOUT_HWND_LEN] = 0;
            id = (HWND)strtol( res, NULL, 16 );
            if( parm == SELWIN_LISTBOX ) {
                if( ch == '*' ) {
                    parm = SELWIN_DELETE;
                } else {
                    parm = SELWIN_ADD;
                }
            }
            if( parm == SELWIN_SHOWINFO ) {
                ShowFramedInfo( hwnd, id );
            } else if( parm == SELWIN_DELETE ) {
                deleteSelectedWindow( id );
            } else if( parm == SELWIN_ADD ) {
                tmpWndList = doAddSelectedWindow( id, tmpWndList, &tmpWndCnt );
            } else {
                if( framedHwnd != NULL ) {
                    FrameAWindow( framedHwnd );
                }
                if( id == framedHwnd ) {
                    framedHwnd = NULL;
                } else {
                    framedHwnd = id;
                    FrameAWindow( framedHwnd );
                }
                break;
            }
            setUpWindows();
            SendDlgItemMessage( hwnd, SELWIN_LISTBOX, LB_SETTOPINDEX, top, 0L );
            SendDlgItemMessage( hwnd, SELWIN_LISTBOX, LB_SETCURSEL, sel, 0L );
            break;
        case IDCANCEL:
            FrameAWindow( framedHwnd );
            EndDialog( hwnd, 0 );
            break;
        case IDOK:
            FrameAWindow( framedHwnd );
            EndDialog( hwnd, 1 );
            break;
        }
        break;
    case WM_CLOSE:
        FrameAWindow( framedHwnd );
        EndDialog( hwnd, 1 );
        break;
    default:
        return( FALSE );
    }
    return( TRUE );

} /* ShowSelectedDialog */

/*
 * DoShowSelectedDialog - start SELETEDWINS dialog
 */
void DoShowSelectedDialog( HWND hwnd, BOOL *spyall )
{
    FARPROC     fp;
    INT_PTR     rc;

    tmpWndCnt = WindowCount;
    tmpSpyAll = *spyall;
    tmpWndList = NULL;
    if( WindowCount > 0 ) {
        tmpWndList = MemAlloc( WindowCount * sizeof( HWND ) );
        memcpy( tmpWndList, WindowList, WindowCount * sizeof( HWND ) );
    }
    fp = MakeProcInstance( (FARPROC)ShowSelectedDialog, Instance );
    rc = JDialogBox( ResInstance, "SELECTEDWINS", hwnd, (DLGPROC)fp );
    if( rc ) {
        *spyall = tmpSpyAll;
        WindowCount = tmpWndCnt;
        MemFree( WindowList );
        WindowList = tmpWndList;
    } else {
        MemFree( tmpWndList );
    }
    FreeProcInstance( fp );

} /* DoShowSelectedDialog */

