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


#include "precomp.h"
#include "watcom.h"
#include "wglbl.h"
#include "sys_rc.h"
#include "wedit.h"
#include "wmsg.h"
#include "rcstr.gh"
#include "w_menu.h"

/****************************************************************************/
/* macro definitions                                                        */
/****************************************************************************/

/****************************************************************************/
/* external function prototypes                                             */
/****************************************************************************/
WINEXPORT LRESULT CALLBACK WPrevWndProc( HWND, UINT, WPARAM, LPARAM );

/****************************************************************************/
/* static function prototypes                                               */
/****************************************************************************/
static void WHandleMenuSelect( WMenuEditInfo *, WPARAM, LPARAM );

/****************************************************************************/
/* static variables                                                         */
/****************************************************************************/
static char WPrevClass[] = "WMenuPrevClass";

Bool WRegisterPrevClass( HINSTANCE inst )
{
    WNDCLASS wc;

    /* fill in the window class structure for the preview window */
    wc.style = CS_DBLCLKS | CS_GLOBALCLASS;
    wc.lpfnWndProc = WPrevWndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = sizeof( LONG_PTR );
    wc.hInstance = inst;
    wc.hIcon = NULL;
    wc.hCursor = LoadCursor( (HINSTANCE)NULL, IDC_ARROW );
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = WPrevClass;

    return( RegisterClass( &wc ) );
}

void WUnRegisterPrevClass( HINSTANCE inst )
{
    UnregisterClass( WPrevClass, inst );
}

Bool WResetPrevWindowMenu( WMenuEditInfo *einfo )
{
    HMENU       menu;
    HMENU       omenu;
    Bool        ok;

    ok = (einfo != NULL && einfo->preview_window != (HWND)NULL);

    if( ok ) {
        ok = WResetPreviewIDs( einfo );
    }

    if( ok ) {
        menu = WCreatePreviewMenu( einfo );
        ok = (menu != (HMENU)NULL);
    }

    if( ok ) {
        omenu = GetMenu( einfo->preview_window );
        if( omenu != (HMENU)NULL ) {
            DestroyMenu( omenu );
        }
        ok = SetMenu( einfo->preview_window, menu );
    }

    return( ok );
}

void WMovePrevWindow( WMenuEditInfo *einfo )
{
    HWND        win;
    RECT        rect;

    if( einfo == NULL || einfo->preview_window == (HWND)NULL ) {
        return;
    }

    win = GetDlgItem( einfo->edit_dlg, IDM_MENUEDTESTPOS );
    GetWindowRect( win, &rect );
    SetWindowPos( einfo->preview_window, (HWND)NULL,
                  rect.left, rect.top, 0, 0,
                  SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE );
}

Bool WCreatePrevWindow( HINSTANCE inst, WMenuEditInfo *einfo )
{
    int         x, y, width, height, i;
    HWND        win;
    HMENU       sys_menu;
    RECT        rect;
    char        *title;

    if( einfo == NULL || einfo->edit_dlg == (HWND)NULL ) {
        return( FALSE );
    }

    win = GetDlgItem( einfo->edit_dlg, IDM_MENUEDTESTPOS );
    GetWindowRect( win, &rect );

    x = rect.left;
    y = rect.top;
    width = 206;
    height = 63;

    title = WAllocRCString( W_PREVIEWMENU );

    einfo->preview_window = CreateWindow( WPrevClass, title,
                                          WS_POPUP | WS_VISIBLE | WS_CAPTION | WS_SYSMENU,
                                          x, y, width, height, einfo->edit_dlg,
                                          (HMENU)NULL, inst, einfo );

    if( title != NULL ) {
        WFreeRCString( title );
    }

    if( einfo->preview_window == (HWND)NULL ) {
        return( FALSE );
    }

    sys_menu = GetSystemMenu( einfo->preview_window, FALSE );
    if( sys_menu != (HMENU)NULL ) {
        i = GetMenuItemCount( sys_menu );
        for( ; i >= 0; i-- ) {
            DeleteMenu( sys_menu, i, MF_BYPOSITION );
        }
    }

    SendMessage( einfo->preview_window, WM_NCACTIVATE, (WPARAM)TRUE, (LPARAM)NULL );

    return( TRUE );
}

void WHandleMenuSelect( WMenuEditInfo *einfo, WPARAM wParam, LPARAM lParam )
{
    WMenuEntry  *entry;
    HWND        lbox;
    HMENU       popup;
    WORD        flags;
    WORD        id;
    int         pos;

    if( einfo == NULL || einfo->menu == NULL || einfo->menu->first_entry == NULL ) {
        return;
    }

    lbox = GetDlgItem( einfo->edit_dlg, IDM_MENUEDLIST );
    if( lbox == (HWND)NULL ) {
        return;
    }

    flags = GET_WM_MENUSELECT_FLAGS( wParam, lParam );

    entry = NULL;

    if( flags == (WORD)-1 && GET_WM_MENUSELECT_HMENU( wParam, lParam ) == (HMENU)NULL ) {
        // we ignore WM_MENUSELECT when a menu is closing
    } else if( flags & MF_SYSMENU ) {
        // we ignore WM_MENUSELECT for the system menu
    } else if( flags & MF_SEPARATOR ) {
        // we ignore WM_MENUSELECT for separators, for now...
    } else if( flags & MF_POPUP ) {
        popup = (HMENU)GET_WM_MENUSELECT_ITEM( wParam, lParam );
#ifdef __NT__
        popup = GetSubMenu( (HMENU)lParam, (int)popup );
#endif
        entry = WFindEntryFromPreviewPopup( einfo->menu->first_entry, popup );
    } else {
        id = GET_WM_MENUSELECT_ITEM( wParam, lParam );
        entry = WFindEntryFromPreviewID( einfo->menu->first_entry, id );
    }

    if( entry == NULL ) {
        return;
    }

    pos = 0;

    if ( WFindEntryLBPos( einfo->menu->first_entry, entry, &pos ) ) {
        pos--;
        einfo->current_entry = NULL;
        einfo->current_pos = -1;
        if( SendMessage( lbox, LB_SETCURSEL, (WPARAM)pos, 0 ) != LB_ERR ) {
            WHandleSelChange( einfo );
        }
    }
}

WINEXPORT LRESULT CALLBACK WPrevWndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    LRESULT             ret;
    Bool                pass_to_def;
    WMenuEditInfo       *einfo;

    pass_to_def = TRUE;
    ret = FALSE;
    einfo = (WMenuEditInfo *)GET_WNDLONGPTR( hWnd, 0 );

    switch ( message ) {
    case WM_SETFOCUS:
        if( einfo != NULL && hWnd != (HWND)wParam ) {
            //SetFocus( einfo->win );
            SendMessage( einfo->win, WM_NCACTIVATE, (WPARAM)TRUE, (LPARAM)NULL );
        }
        pass_to_def = FALSE;
        break;

    case WM_MENUSELECT:
        WHandleMenuSelect( einfo, wParam, lParam );
        break;

    case WM_CREATE:
        einfo = ((CREATESTRUCT *)lParam)->lpCreateParams;
        SET_WNDLONGPTR( hWnd, 0, (LONG_PTR)einfo );
        break;

    case WM_CLOSE:
        ret = TRUE;
        pass_to_def = FALSE;
        break;
    }

    if( pass_to_def ) {
        ret = DefWindowProc( hWnd, message, wParam, lParam );
    }

    return( ret );
}
