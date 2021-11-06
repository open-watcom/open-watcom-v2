/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2015-2021 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Dockable GUI toolbar.
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
#include "guirdlg.h"


/* total height/width taken up by outline around  bitmap, on button */
#if defined (__NT__)
#define OUTLINE_AMOUNT  4   // Should be changed later.
#else
#define OUTLINE_AMOUNT  4
#endif
#define BORDER_AMOUNT   1 /* space outside row of buttons */

/*
 * GUIXCloseToolBar -- close the tool bar and free memory.  Can be called by
 *                    app.  Gets called when parent window closed or floating
 *                    toolbar window gets closed.
 */

bool GUIXCloseToolBar( gui_window *wnd )
{
    toolbarinfo     *tbar;
    int             i;

    tbar = wnd->tbar;
    if( tbar != NULL ) {
        wnd->tbar = NULL;
        if( tbar->hdl != NULL ) {
            ToolBarFini( tbar->hdl );
            for( i = 0; i < tbar->num_items; i++ ) {
                _wpi_deletebitmap( tbar->hbitmaps[i] );
            }
            GUIMemFree( tbar->hbitmaps );
        }
        GUIMemFree( tbar );
        if( (wnd->flags & DOING_DESTROY) == 0 ) {
            GUIResizeBackground( wnd, true );
        }
        GUIEVENT( wnd, GUI_TOOLBAR_DESTROYED, NULL );
    }
    return( true );
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

static void guiToolBarHelp( HWND hwnd, ctl_id id, bool down )
{
    gui_window          *wnd;
    gui_menu_styles     style;

    wnd = GetToolWnd( hwnd );
    if( wnd != NULL ) {
        style = ( down ) ? GUI_STYLE_MENU_ENABLED : GUI_STYLE_MENU_IGNORE;
        GUIDisplayHintText( wnd, wnd, id, TOOL_HINT, style );
    }
}

/*
 * guiToolBarProc - hook message handler for the tool bar.
 */

static bool guiToolBarProc( HWND hwnd, WPI_MSG msg, WPI_PARAM1 wparam, WPI_PARAM2 lparam )
{
    gui_window              *wnd;
    toolbarinfo             *tbar;
    HMENU                   hsysmenu;
    gui_ctl_id              id;

    wnd = GetToolWnd( hwnd );
    if( wnd == NULL ) {
        return( false );
    }
    tbar = wnd->tbar;
    if( tbar == NULL ) {
        return( false );
    }
    switch( msg ) {
    case WM_CREATE:
        hwnd = _wpi_getframe( hwnd );
        hsysmenu = _wpi_getsystemmenu( hwnd );
        GUIAppendSystemMenuItem( hsysmenu, GUI_MENU_IDX( GUI_FIX_TOOLBAR ) );
        break;
#ifdef __OS2_PM__
    case WM_CHAR:
    case WM_TRANSLATEACCEL:
#else
    case WM_SYSKEYDOWN:
    case WM_SYSKEYUP:
    case WM_KEYUP:
    case WM_KEYDOWN:
#endif
        return( GUIProcesskey( hwnd, msg, wparam, lparam ) != 0 );
    case WM_MENUSELECT:
        GUIProcessMenuSelect( wnd, hwnd, msg, wparam, lparam );
        return( true );
    case WM_SYSCOMMAND:
        id = _wpi_getid( wparam );
        switch( id ) {
        case GUI_FIX_TOOLBAR:
            GUIChangeToolBar( wnd );
        }
        break;
#ifndef __OS2_PM__
    case WM_NCLBUTTONDBLCLK:
#endif
    case WM_RBUTTONDBLCLK:
    case WM_LBUTTONDBLCLK:
        /* flip the current state of the toolbar -
         * if we are fixed then start to float or vice versa
         */
        if( !HasToolAtPoint( tbar->hdl, wparam, lparam ) ) {
#ifdef __OS2_PM__
            // Hack: For some reason we will get here with bogus coords,
            // we need to ignore the event. Should really find out where
            // the message is coming from.
            if( (ULONG)wparam != 0x0FFFFFFF )
                GUIChangeToolBar( wnd );
#else
            GUIChangeToolBar( wnd );
#endif
            return( true );
        }
        break;
    case WM_MOVE:
    case WM_SIZE:
        // Whenever we are moved or sized as a floating toolbar, we remember our position
        // so that we can restore it when dbl. clicked
        if( tbar->info.style == TOOLBAR_FLOAT_STYLE ) {
            hwnd = _wpi_getframe( hwnd );
            _wpi_getwindowrect( hwnd, &tbar->float_wpi_rect );
        }
        break;
    case WM_GETMINMAXINFO:
        {
#ifdef __WINDOWS_386__
            WPI_MINMAXINFO __far *minmax= (WPI_MINMAXINFO __far *)MK_FP32( (void *)lparam );
#else
            WPI_MINMAXINFO *minmax = (WPI_MINMAXINFO *)lparam;
#endif
            _wpi_setmintracksize( minmax,
                ( tbar->info.border_size.x + _wpi_getsystemmetrics( SM_CXFRAME ) ) * 2 + tbar->info.button_size.x,
                ( tbar->info.border_size.y + _wpi_getsystemmetrics( SM_CYFRAME ) ) * 2 + tbar->info.button_size.y + _wpi_getsystemmetrics( SM_CYCAPTION ) );
        }
        break;
    case WM_CLOSE:
        GUICloseToolBar( wnd );
        return( true );
    }
    return( false );
}

/*
 *  GUIXCreateToolBarWithTips -- create a tool bar, possibly with tooltips
 */

bool GUIXCreateToolBarWithTips( gui_window *wnd, bool fixed, gui_ord in_height,
                                const gui_toolbar_items *toolinfo,
                                bool excl, gui_colour_set *plain,
                                gui_colour_set *standout, const gui_rect *float_pos,
                                bool use_tips )
{
    guix_coord          scr_size;
    guix_coord          scr_pos;
    HWND                parent;
    HWND                toolhwnd;
    toolbarinfo         *tbar;
    int                 i;
    TOOLITEMINFO        info;
    guix_ord            fixed_height;
    guix_ord            fixed_width;
    guix_ord            adjust_amount;
    guix_ord            width;
    guix_ord            height;
    int                 new_right;
    int                 bm_h;
    int                 bm_w;
    WPI_RECTDIM         left, top, right, bottom;
    int                 h;
    int                 num_items;

    excl = excl;
    plain = plain;
    standout = standout;
    fixed_height = 0;
    fixed_width = 0;
    if( ( wnd == NULL ) || ( wnd->hwnd == NULLHANDLE ) || ( wnd->root == NULLHANDLE ) ) {
        return( false );
    }
    if( wnd->tbar != NULL ) {
        GUICloseToolBar( wnd );
    }
    tbar = wnd->tbar = (toolbarinfo *)GUIMemAlloc( sizeof( toolbarinfo ) );
    if( tbar == NULL ) {
        return( false );
    }
    num_items = toolinfo->num_items;
    memset( tbar, 0, sizeof( toolbarinfo ) );
    parent = wnd->root;
    tbar->fixed_wpi_rect = wnd->hwnd_client_rect;
    tbar->hbitmaps = (WPI_HBITMAP *)GUIMemAlloc( num_items * sizeof( WPI_HBITMAP ) );
    if( tbar->hbitmaps == NULL ) {
        GUIMemFree( tbar );
        wnd->tbar = NULL;
        return( false );
    }
    for( i = 0; i < num_items; i++ ) {
        tbar->hbitmaps[i] = _wpi_loadbitmap( GUIResHInst, MAKEINTRESOURCE( toolinfo->toolbar[i].bitmap_id ) );
        if( in_height == 0 ) {
            _wpi_getbitmapdim( tbar->hbitmaps[i], &bm_w, &bm_h );
            if( fixed_height < bm_h ) {
                fixed_height = bm_h;
            }
            if( fixed_width < bm_w ) {
                fixed_width = bm_w;
            }
        }
    }
    tbar->info.border_size.x = BORDER_AMOUNT;
    tbar->info.border_size.y = BORDER_AMOUNT;
    /* space for border and space before border */
    adjust_amount = 2 * ( _wpi_getsystemmetrics( SM_CYBORDER ) + BORDER_AMOUNT );
    if( in_height == 0 ) { /* maintian # of pixels in bitmap */
        height = fixed_height + adjust_amount + OUTLINE_AMOUNT;
        width = fixed_width + OUTLINE_AMOUNT;
    } else {
        /* only height of windows given, make bitmaps square */
        width = height = GUIScaleToScreenV( in_height - 2 );
    }

    _wpi_getrectvalues( tbar->fixed_wpi_rect, &left, &top, &right, &bottom );
    h      = _wpi_getheightrect( tbar->fixed_wpi_rect );
    bottom = _wpi_cvth_y_plus1( height, h );
    top    = _wpi_cvth_y_plus1( top, h );
    _wpi_setwrectvalues( &tbar->fixed_wpi_rect, left, top, right, bottom );
    height -= adjust_amount; /* leaving just button size */
    tbar->info.button_size.x = width;
    tbar->info.button_size.y = height;
    bottom = height + BORDER_AMOUNT * 2 +
             _wpi_getsystemmetrics( SM_CYCAPTION ) +
             2 * ( _wpi_getsystemmetrics( SM_CYFRAME ) -
                   _wpi_getsystemmetrics( SM_CYBORDER ) );
    bottom = _wpi_cvth_y_plus1( bottom, h );
#ifdef __OS2_PM__
    bottom -= 2;
#endif
    new_right = width * num_items -
                ( num_items - 1 ) * tbar->info.border_size.x +
                left + 2 * _wpi_getsystemmetrics( SM_CXFRAME ) +
                BORDER_AMOUNT * 2;
    if( new_right < right ) {
        right = new_right;
    }

    _wpi_setwrectvalues( &tbar->float_wpi_rect, left, top, right, bottom );
    _wpi_mapwindowpoints( parent, HWND_DESKTOP, (WPI_PPOINT)&tbar->float_wpi_rect, 2 );

    if( fixed ) {
        tbar->info.area = tbar->fixed_wpi_rect;
        tbar->info.style = TOOLBAR_FIXED_STYLE;
    } else {
        if( float_pos != NULL ) {
            GUICalcLocation( float_pos, &scr_pos, &scr_size, parent );
            _wpi_setwrectvalues( &tbar->float_wpi_rect, scr_pos.x, scr_pos.y,
                                 scr_pos.x + scr_size.x, scr_pos.y + scr_size.y );
            _wpi_mapwindowpoints( parent, HWND_DESKTOP, (WPI_PPOINT)&tbar->float_wpi_rect, 2 );
        }
        tbar->info.area = tbar->float_wpi_rect;
        tbar->info.style = TOOLBAR_FLOAT_STYLE;
    }

    tbar->info.hook = guiToolBarProc;
    tbar->info.helphook = guiToolBarHelp;
    tbar->info.background = 0;
    tbar->info.foreground = 0;
    tbar->num_items = num_items;
    tbar->info.is_fixed = fixed;
    tbar->info.use_tips = use_tips;

    tbar->hdl = ToolBarInit( parent );

    ToolBarDisplay( tbar->hdl, &tbar->info );

    GUIResizeBackground( wnd, true );

    for( i = 0; i < num_items; i++ ) {
        info.u.hbitmap = tbar->hbitmaps[i];
        info.id = toolinfo->toolbar[i].id;
        info.flags = 0;
        if( use_tips && toolinfo->toolbar[i].tip != NULL ) {
            strncpy( info.tip, toolinfo->toolbar[i].tip, MAX_TIP );
        } else {
            info.tip[0] = '\0';
        }
        ToolBarAddItem( tbar->hdl, &info );
    }
    toolhwnd = ToolBarWindow( tbar->hdl );
    _wpi_showwindow( toolhwnd, SW_SHOWNORMAL );
    _wpi_updatewindow( toolhwnd );
    return( true );
}

/*
 *  GUIXCreateToolBar -- create a tool bar, fixed or not
 */

bool GUIXCreateToolBar( gui_window *wnd, bool fixed, gui_ord height,
                        const gui_toolbar_items *toolinfo,
                        bool excl, gui_colour_set *plain,
                        gui_colour_set *standout, const gui_rect *float_pos )
{
    return( GUIXCreateToolBarWithTips( wnd, fixed, height, toolinfo,
                                       excl, plain, standout, float_pos, false ) );
}

/*
 * GUIResizeToolBar -- used to resize the fixed tool bar when the parent
 *                     window resizes
 */

void GUIResizeToolBar( gui_window *wnd )
{
    WPI_RECT    wpi_rect;
    WPI_RECTDIM left, top, right, bottom;
    WPI_RECTDIM height;
    WPI_RECTDIM t, h;
    toolbarinfo *tbar;

    tbar = wnd->tbar;
    if( tbar != NULL ) {
        wpi_rect = wnd->root_client_rect;
        if( wnd->root == NULLHANDLE ) {
            wpi_rect = wnd->hwnd_client_rect;
        }
        _wpi_rationalize_rect( &wpi_rect );
        if( tbar->info.is_fixed ) {
            height = _wpi_getheightrect( tbar->fixed_wpi_rect );
            h = _wpi_getheightrect( wpi_rect );
            _wpi_getrectvalues( wpi_rect, &left, &top, &right, &bottom );
            t = _wpi_cvth_y_plus1( top, h );
            bottom = _wpi_cvth_y_plus1( height, h );
            _wpi_setwrectvalues( &tbar->fixed_wpi_rect, left, t, right, bottom );
            t = _wpi_cvth_y_size_plus1( top, h, height );
            _wpi_movewindow( ToolBarWindow( tbar->hdl ), left, t, right - left, height, TRUE );
        }
    }
}

bool GUIAPI GUIHasToolBar( gui_window *wnd )
{
    return( wnd->tbar != NULL );
}

bool GUIAPI GUIChangeToolBar( gui_window *wnd )
{
    gui_event   gui_ev;
    toolbarinfo *tbar;
    HWND        toolhwnd;
    int         t;
    WPI_RECTDIM left, top, right, bottom;

    tbar = wnd->tbar;
    if( !tbar->info.is_fixed ) {
        tbar->info.is_fixed = true;
        tbar->info.style = TOOLBAR_FIXED_STYLE;
        tbar->info.area = tbar->fixed_wpi_rect;
        gui_ev = GUI_TOOLBAR_FIXED;
    } else {
        tbar->info.is_fixed = false;
        tbar->info.style = TOOLBAR_FLOAT_STYLE;
        tbar->info.area = tbar->float_wpi_rect;
        _wpi_cvtc_rect_plus1( wnd->root, &tbar->info.area );
        gui_ev = GUI_TOOLBAR_FLOATING;
    }
    ToolBarDisplay( tbar->hdl, &tbar->info );
    if( tbar->info.style != TOOLBAR_FLOAT_STYLE ) {
        ToolBarRedrawButtons( tbar->hdl );
    }
    toolhwnd = ToolBarWindow( tbar->hdl );
    if( tbar->info.style == TOOLBAR_FLOAT_STYLE ) {
        _wpi_getrectvalues( tbar->float_wpi_rect, &left, &top, &right, &bottom );
        t = top;
        //t = _wpi_cvtc_y_size( wnd->hwnd, t, bottom - top );
        t = _wpi_cvtc_y_plus1( wnd->root, t );
        _wpi_movewindow( toolhwnd, left, t, right-left, bottom-top, TRUE );
    }
    GUIResizeBackground( wnd, true );
    _wpi_showwindow( toolhwnd, SW_SHOWNORMAL );
    _wpi_updatewindow( toolhwnd );
    GUIEVENT( wnd, gui_ev, NULL );
    return( true );
}

bool GUIAPI GUIToolBarFixed( gui_window *wnd )
{
    if( GUIHasToolBar( wnd ) ) {
        return( wnd->tbar->info.is_fixed );
    }
    return( false );
}
