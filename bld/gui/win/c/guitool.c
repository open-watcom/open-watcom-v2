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


#include "guiwind.h"
#include <stdlib.h>
#include <string.h>
#include "guixwind.h"
#include "guiscale.h"
#include "guitool.h"
#include "guimapky.h"
#include "guimenus.h"
#include "guixutil.h"

extern  WPI_INST        GUIResHInst;
extern  gui_menu_struct GUIHint[];

#define OUTLINE_AMOUNT  4 /* total height/width taken up by outline around
                             bitmap, on button */
#define BORDER_AMOUNT   1 /* space outside row of bottons */

/*
 * GUIXCloseToolBar -- close the tool bar and free memory.  Can be called by
 *                    app.  Gets called when parent window closed or floating
 *                    toolbar window gets closed.
 */

bool GUIXCloseToolBar( gui_window *wnd )
{
    toolbarinfo         *toolbar;
    int                 i;

    if( ( wnd->toolbar != NULL ) && ( wnd->toolbar->hdl != NULL ) ) {
        toolbar = wnd->toolbar;
        wnd->toolbar = NULL;
        ToolBarFini( toolbar->hdl );
        for( i=0; i < toolbar->num; i++ ) {
            _wpi_deletebitmap( toolbar->bitmaps[i] );
        }
        GUIFree( toolbar->bitmaps );
        GUIFree( toolbar );
        GUIResizeBackground( wnd, TRUE );
        GUIEVENTWND( wnd, GUI_TOOLBAR_DESTROYED, NULL );
    }
    return( TRUE );
}

static gui_window *GetToolWnd( HWND hwnd )
{
    HWND        parent;

#ifndef __OS2_PM__
    parent = _wpi_getparent( hwnd );
    if( parent != HWND_DESKTOP ) {
        return( GUIGetWindow( parent ) );
    }
#else
    parent = _wpi_getparent(_wpi_getparent( hwnd ));
    if( parent == HWND_DESKTOP ) {
        parent = _wpi_getowner( _wpi_getparent( hwnd ) );
    }
    if( parent != HWND_DESKTOP ) {
        return( GUIGetWindow( parent ) );
    }
#endif
    return( NULL );
}

void GUIToolBarHelp( HWND hwnd, WPI_PARAM1 id, BOOL down )
{
    gui_window          *wnd;
    gui_menu_styles     style;

    wnd = GetToolWnd( hwnd );
    if( wnd != NULL ) {
        style = ( down ? GUI_ENABLED : GUI_IGNORE );
        GUIDisplayHintText( wnd, wnd, (int)id, TOOL_HINT, style );
    }
}

/*
 * GUIToolBarProc - hook message handler for the tool bar.
 */

BOOL GUIToolBarProc( HWND hwnd, WPI_MSG msg, WPI_PARAM1 wparam, WPI_PARAM2 lparam )
{
#ifdef __WINDOWS_386__
    WPI_MINMAXINFO FAR  *minmax;
#else
    WPI_MINMAXINFO      *minmax;
#endif
    gui_window          *wnd;
    toolbarinfo         *toolbar;
    HMENU               hmenu;
    WORD                param;

    wnd = GetToolWnd( hwnd );
    if( wnd == NULL ) {
        return( FALSE );
    }
    toolbar = wnd->toolbar;
    if( toolbar == NULL ) {
        return( FALSE );
    }
    switch( msg ) {
    case WM_CREATE :
        hwnd = _wpi_getframe( hwnd );
        hmenu = _wpi_getsystemmenu( hwnd );
        if( hmenu != NULLHANDLE ) {
            if( _wpi_appendmenu( hmenu, MF_SEPARATOR, 0, 0,
                                 NULLHANDLE, NULL ) ) {
                _wpi_appendmenu( hmenu, MF_ENABLED|MF_SYSMENU, 0,
                                 GUIHint[GUI_MENU_FIX_TOOLBAR].id, NULLHANDLE,
                                 GUIHint[GUI_MENU_FIX_TOOLBAR].label );
            }
        }
        break;
#ifdef __OS2_PM__
    case WM_CHAR :
#else
    case WM_SYSKEYDOWN :
    case WM_SYSKEYUP :
    case WM_KEYUP :
    case WM_KEYDOWN :
#endif
        return( (BOOL)GUIProcesskey( hwnd, msg, wparam, lparam ) );
    case WM_MENUSELECT :
        GUIProcessMenuSelect( wnd, hwnd, msg, wparam, lparam );
        return( TRUE );
        break;
    case WM_SYSCOMMAND :
        param = _wpi_getid( wparam );
        switch( param ) {
        case GUI_FIX_TOOLBAR :
            GUIChangeToolBar( wnd );
        }
        break;
#ifndef __OS2_PM__
    case WM_NCLBUTTONDBLCLK :
#endif
    case WM_RBUTTONDBLCLK :
    case WM_LBUTTONDBLCLK :
        /* flip the current state of the toolbar -
         * if we are fixed then start to float or vice versa
         */
        if( !HasToolAtPoint( toolbar->hdl, wparam, lparam ) ) {
            GUIChangeToolBar( wnd );
            return( TRUE );
        }
        break;
    case WM_MOVE:
    case WM_SIZE:
        // Whenever we are moved or sized as a floating toolbar, we remember our position
        // so that we can restore it when dbl. clicked
        if( toolbar->info.style == TOOLBAR_FLOAT_STYLE ) {
            hwnd = _wpi_getframe( hwnd );
            _wpi_getwindowrect( hwnd, &( toolbar->floatrect ) );
        }
        break;
    case WM_GETMINMAXINFO:
#ifdef __WINDOWS_386__
        minmax = (WPI_MINMAXINFO FAR *)MK_FP32( (void *)lparam );
#else
        minmax = (WPI_MINMAXINFO *)lparam;
#endif
        _wpi_setmintracksize( minmax, ( toolbar->info.border_size.x +
                                     _wpi_getsystemmetrics( SM_CXFRAME ) ) * 2 +
                                    toolbar->info.button_size.x,
                                    ( toolbar->info.border_size.y +
                                     _wpi_getsystemmetrics( SM_CYFRAME ) ) * 2 +
                                     toolbar->info.button_size.y +
                                     _wpi_getsystemmetrics( SM_CYCAPTION ) );
        break;
    case WM_CLOSE :
        GUICloseToolBar( wnd );
        return( TRUE );
        break;
    }
    return( FALSE );
}

/*
 *  GUIXCreateToolBar -- create a tool bar, fixed or not
 */

bool GUIXCreateToolBar( gui_window *wnd, bool fixed, gui_ord height,
                       int num_toolbar_items, gui_toolbar_struct *toolinfo,
                       bool excl, gui_colour_set *plain,
                       gui_colour_set *standout, gui_rect *float_pos  )
{
    gui_coord           size;
    gui_coord           pos;
    HWND                parent;
    HWND                toolhwnd;
    toolbarinfo         *toolbar;
    int                 i;
    TOOLITEMINFO        info;
    int                 fixed_height;
    int                 fixed_width;
    int                 adjust_amount;
    int                 width;
    int                 new_right;
    int                 bm_h;
    int                 bm_w;
    GUI_RECTDIM         left, top, right, bottom;
    int                 h;

    excl = excl;
    plain = plain;
    standout = standout;
    if( ( wnd == NULL ) || ( num_toolbar_items < 1 ) || ( toolinfo == NULL ) ||
        ( wnd->hwnd == NULLHANDLE ) || ( wnd->root == NULLHANDLE ) ) {
        return( FALSE );
    }
    if( wnd->toolbar != NULL ) {
        GUICloseToolBar( wnd );
    }
    wnd->toolbar = ( toolbarinfo * )GUIAlloc( sizeof( toolbarinfo ) );
    if( wnd->toolbar == NULL ) {
        return( FALSE );
    }
    toolbar = wnd->toolbar;
    memset( toolbar, 0, sizeof( toolbarinfo ) );
    parent = wnd->root;
    toolbar->fixedrect = wnd->hwnd_client;
    toolbar->bitmaps = (HBITMAP *)GUIAlloc( num_toolbar_items * sizeof( HBITMAP ) );
    if( toolbar->bitmaps == NULL ) {
        GUIFree( wnd->toolbar );
        wnd->toolbar = NULL;
        return( FALSE );
    }
    if( height == 0 ) {
        fixed_height = 0;
        fixed_width = 0;
    }
    for( i = 0; i < num_toolbar_items; i++ ) {
        toolbar->bitmaps[i] = _wpi_loadbitmap( GUIResHInst,
                                _wpi_makeintresource( toolinfo[i].bitmap ) );
        if( height == 0 ) {
            _wpi_getbitmapdim( toolbar->bitmaps[i], &bm_w, &bm_h );
            if( bm_h > fixed_height ) {
                fixed_height = bm_h;
            }
            if( bm_w > fixed_width ) {
                fixed_width = bm_w;
            }
        }
    }
    toolbar->info.border_size.x = BORDER_AMOUNT;
    toolbar->info.border_size.y = BORDER_AMOUNT;
    /* space for border and space before border */
    adjust_amount = 2*(_wpi_getsystemmetrics( SM_CYBORDER )+BORDER_AMOUNT);
    if( height == 0 ) { /* maintian # of pixels in bitmap */
        height = fixed_height + adjust_amount + OUTLINE_AMOUNT;
        width = fixed_width + OUTLINE_AMOUNT;
    } else {
        /* only height of windows given, make bitmaps square */
        size.x = 0;
        size.y = height;
        GUIScaleToScreenR( &size );
        height = size.y;
        width = size.y;
    }

    _wpi_getrectvalues( toolbar->fixedrect, &left, &top, &right, &bottom );
    h      = _wpi_getheightrect( toolbar->fixedrect );
    bottom = _wpi_cvth_y_plus1( height, h );
    top    = _wpi_cvth_y_plus1( top, h );
    _wpi_setwrectvalues( &toolbar->fixedrect, left, top, right, bottom );
    height -= adjust_amount; /* leaving just button size */
    toolbar->info.button_size.x = width;
    toolbar->info.button_size.y = height;
    bottom = height + BORDER_AMOUNT * 2 +
             _wpi_getsystemmetrics( SM_CYCAPTION ) +
             2 * ( _wpi_getsystemmetrics( SM_CYFRAME ) -
                   _wpi_getsystemmetrics( SM_CYBORDER ) );
    bottom = _wpi_cvth_y_plus1( bottom, h );
#ifdef __OS2_PM__
    bottom -= 2;
#endif
    new_right = width * num_toolbar_items -
                (num_toolbar_items - 1) * toolbar->info.border_size.x +
                left + 2 * _wpi_getsystemmetrics( SM_CXFRAME ) +
                BORDER_AMOUNT * 2;
    if( new_right < right ) {
        right = new_right;
    }

    _wpi_setwrectvalues( &toolbar->floatrect, left, top, right, bottom );
    _wpi_mapwindowpoints( parent, HWND_DESKTOP, (WPI_PPOINT)&toolbar->floatrect, 2 );

    if( fixed ) {
        toolbar->info.area = toolbar->fixedrect;
        toolbar->info.style = TOOLBAR_FIXED_STYLE;
    } else {
        if( float_pos != NULL ) {
            GUICalcLocation( float_pos, &pos, &size, parent );
            _wpi_setwrectvalues( &toolbar->floatrect, pos.x, pos.y,
                                 pos.x + size.x, pos.y + size.y );
            _wpi_mapwindowpoints( parent, HWND_DESKTOP, (WPI_PPOINT)&toolbar->floatrect, 2 );
        }
        toolbar->info.area = toolbar->floatrect;
        toolbar->info.style = TOOLBAR_FLOAT_STYLE;
    }

    toolbar->info.hook = GUIToolBarProc;
    toolbar->info.helphook = GUIToolBarHelp;
    toolbar->info.background = 0;
    toolbar->info.foreground = 0;
    toolbar->num = num_toolbar_items;
    toolbar->info.is_fixed = fixed;

    toolbar->hdl = ToolBarInit( parent );

    ToolBarDisplay( toolbar->hdl, &toolbar->info );

    GUIResizeBackground( wnd, TRUE );

    for( i = 0; i < num_toolbar_items; i++ ) {
        info.u.bmp = toolbar->bitmaps[i];
        info.id = toolinfo[i].id;
        info.flags = 0;
        ToolBarAddItem( toolbar->hdl, &info );
    }
    toolhwnd = ToolBarWindow( toolbar->hdl );
    _wpi_showwindow( toolhwnd, SW_SHOWNORMAL );
    _wpi_updatewindow( toolhwnd );
    return( TRUE );
}

/*
 * GUIResizeToolBar -- used to resize the fixed tool bar when the parent
 *                     window resizes
 */

void GUIResizeToolBar( gui_window *wnd )
{
    WPI_RECT    rect;
    GUI_RECTDIM left, top, right, bottom;
    GUI_RECTDIM height;
    GUI_RECTDIM t, h;

    if( wnd->toolbar != NULL ) {
        rect = wnd->root_client;
        if( wnd->root == NULLHANDLE ) {
            rect = wnd->hwnd_client;
        }
        _wpi_rationalize_rect( &rect );
        if( wnd->toolbar->info.is_fixed ) {
            height = _wpi_getheightrect( wnd->toolbar->fixedrect );
            h = _wpi_getheightrect( rect );
            _wpi_getrectvalues( rect, &left, &top, &right, &bottom );
            t = _wpi_cvth_y_plus1( top, h );
            bottom = _wpi_cvth_y_plus1( height, h );
            _wpi_setwrectvalues( &wnd->toolbar->fixedrect, left, t, right, bottom );
            t = _wpi_cvth_y_size_plus1( top, h, height );
            _wpi_movewindow( ToolBarWindow( wnd->toolbar->hdl ),
                             left, t, right - left, height, TRUE );
        }
    }
}

extern bool GUIHasToolBar( gui_window *wnd )
{
    return( wnd->toolbar != NULL );
}

bool GUIChangeToolBar( gui_window *wnd )
{
    gui_event   gui_ev;
    toolbarinfo *toolbar;
    HWND        toolhwnd;
    int         t;
    GUI_RECTDIM left, top, right, bottom;

    toolbar = wnd->toolbar;
    if( !toolbar->info.is_fixed ) {
        toolbar->info.is_fixed = TRUE;
        toolbar->info.style = TOOLBAR_FIXED_STYLE;
        toolbar->info.area = toolbar->fixedrect;
        gui_ev = GUI_TOOLBAR_FIXED;
    } else {
        toolbar->info.is_fixed = FALSE;
        toolbar->info.style = TOOLBAR_FLOAT_STYLE;
        toolbar->info.area = toolbar->floatrect;
        _wpi_cvtc_rect_plus1( wnd->root, &toolbar->info.area );
        gui_ev = GUI_TOOLBAR_FLOATING;
    }
    ToolBarDisplay( toolbar->hdl, &toolbar->info );
    if( toolbar->info.style != TOOLBAR_FLOAT_STYLE ) {
        ToolBarRedrawButtons( toolbar->hdl );
    }
    toolhwnd = ToolBarWindow( toolbar->hdl );
    if( toolbar->info.style == TOOLBAR_FLOAT_STYLE ) {
        _wpi_getrectvalues( toolbar->floatrect, &left, &top, &right, &bottom );
        t = top;
        //t = _wpi_cvtc_y_size( wnd->hwnd, t, bottom - top );
        t = _wpi_cvtc_y_plus1( wnd->root, t );
        _wpi_movewindow( toolhwnd, left, t, right-left, bottom-top, TRUE );
    }
    GUIResizeBackground( wnd, TRUE );
    _wpi_showwindow( toolhwnd, SW_SHOWNORMAL );
    _wpi_updatewindow( toolhwnd );
    GUIEVENTWND( wnd, gui_ev, NULL );
    return( TRUE );
}

bool GUIToolBarFixed( gui_window *wnd )
{
    if( GUIHasToolBar( wnd ) ) {
        return( wnd->toolbar->info.is_fixed );
    }
    return( FALSE );
}
