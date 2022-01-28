/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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


#include "wglbl.h"
#include "sysall.rh"
#include "wedit.h"
#include "wmsg.h"
#include "ldstr.h"
#include "w_menu.h"
#include "wprev.h"


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

bool WRegisterPrevClass( HINSTANCE inst )
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

    return( RegisterClass( &wc ) != 0 );
}

void WUnRegisterPrevClass( HINSTANCE inst )
{
    UnregisterClass( WPrevClass, inst );
}

bool WResetPrevWindowMenu( WMenuEditInfo *einfo )
{
    HMENU       hmenu;
    HMENU       homenu;
    bool        ok;

    ok = (einfo != NULL && einfo->preview_window != (HWND)NULL);

    if( ok ) {
        ok = WResetPreviewIDs( einfo );
    }

    if( ok ) {
        hmenu = WCreatePreviewMenu( einfo );
        ok = (hmenu != (HMENU)NULL);
    }

    if( ok ) {
        homenu = GetMenu( einfo->preview_window );
        if( homenu != (HMENU)NULL ) {
            DestroyMenu( homenu );
        }
        ok = SetMenu( einfo->preview_window, hmenu ) != 0;
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

bool WCreatePrevWindow( HINSTANCE inst, WMenuEditInfo *einfo )
{
    int         x, y, width, height, i;
    HWND        win;
    HMENU       hsysmenu;
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

    title = AllocRCString( W_PREVIEWMENU );

    einfo->preview_window = CreateWindow( WPrevClass, title,
                                          WS_POPUP | WS_VISIBLE | WS_CAPTION | WS_SYSMENU,
                                          x, y, width, height, einfo->edit_dlg,
                                          (HMENU)NULL, inst, einfo );

    if( title != NULL ) {
        FreeRCString( title );
    }

    if( einfo->preview_window == (HWND)NULL ) {
        return( FALSE );
    }

    hsysmenu = GetSystemMenu( einfo->preview_window, FALSE );
    if( hsysmenu != (HMENU)NULL ) {
        i = GetMenuItemCount( hsysmenu );
        for( ; i >= 0; i-- ) {
            DeleteMenu( hsysmenu, i, MF_BYPOSITION );
        }
    }

    SendMessage( einfo->preview_window, WM_NCACTIVATE, (WPARAM)TRUE, (LPARAM)NULL );

    return( TRUE );
}

void WHandleMenuSelect( WMenuEditInfo *einfo, WPARAM wParam, LPARAM lParam )
{
    WMenuEntry  *entry;
    HWND        lbox;
    HMENU       hpopup;
    WORD        flags;
    WORD        id;
    LRESULT     pos;

    if( einfo == NULL || einfo->menu == NULL || einfo->menu->first_entry == NULL ) {
        return;
    }

    lbox = GetDlgItem( einfo->edit_dlg, IDM_MENUEDLIST );
    if( lbox == (HWND)NULL ) {
        return;
    }

    entry = NULL;
    if( MENU_CLOSED( wParam, lParam ) ) {
        // we ignore WM_MENUSELECT when a menu is closing
    } else {
        flags = GET_WM_MENUSELECT_FLAGS( wParam, lParam );
        if( flags & MF_SYSMENU ) {
            // we ignore WM_MENUSELECT for the system menu
        } else if( flags & MF_SEPARATOR ) {
            // we ignore WM_MENUSELECT for separators, for now...
        } else if( flags & MF_POPUP ) {
#ifdef __NT__
            hpopup = GetSubMenu( (HMENU)lParam, GET_WM_MENUSELECT_ITEM( wParam, lParam ) );
#else
            hpopup = (HMENU)(pointer_uint)GET_WM_MENUSELECT_ITEM( wParam, lParam );
#endif
            entry = WFindEntryFromPreviewPopup( einfo->menu->first_entry, hpopup );
        } else {
            id = GET_WM_MENUSELECT_ITEM( wParam, lParam );
            entry = WFindEntryFromPreviewID( einfo->menu->first_entry, id );
        }
    }

    if( entry == NULL ) {
        return;
    }

    pos = 0;
    if( WFindEntryLBPos( einfo->menu->first_entry, entry, &pos ) ) {
        pos--;
        einfo->current_entry = NULL;
        einfo->current_pos = LB_ERR;
        if( SendMessage( lbox, LB_SETCURSEL, (WPARAM)pos, 0 ) != LB_ERR ) {
            WHandleSelChange( einfo );
        }
    }
}

WINEXPORT LRESULT CALLBACK WPrevWndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    LRESULT             ret;
    bool                pass_to_def;
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
