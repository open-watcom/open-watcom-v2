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

extern  WPI_INST        GUIResHInst;
extern  gui_menu_struct GUIHint[];

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

    tbar = wnd->tbinfo;
    if( tbar != NULL ) {
        wnd->tbinfo = NULL;
        if( tbar->hdl != NULL ) {
            ToolBarFini( tbar->hdl );
            for( i = 0; i < tbar->num; i++ ) {
                _wpi_deletebitmap( tbar->bitmaps[i] );
            }
            GUIMemFree( tbar->bitmaps );
        }
        GUIMemFree( tbar );
        if( (wnd->flags & DOING_DESTROY) == 0 ) {
            GUIResizeBackground( wnd, true );
        }
        GUIEVENTWND( wnd, GUI_TOOLBAR_DESTROYED, NULL );
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

static void guiToolBarHelp( HWND hwnd, int id, bool down )
{
    gui_window          *wnd;
    gui_menu_styles     style;

    wnd = GetToolWnd( hwnd );
    if( wnd != NULL ) {
        style = ( down ) ? GUI_ENABLED : GUI_IGNORE;
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
    HMENU                   hmenu;
    gui_ctl_id              id;

    wnd = GetToolWnd( hwnd );
    if( wnd == NULL ) {
        return( false );
    }
    tbar = wnd->tbinfo;
    if( tbar == NULL ) {
        return( false );
    }
    switch( msg ) {
    case WM_CREATE :
        hwnd = _wpi_getframe( hwnd );
        hmenu = _wpi_getsystemmenu( hwnd );
        if( hmenu != NULLHANDLE ) {
            if( _wpi_appendmenu( hmenu, MF_SEPARATOR, 0, 0, NULLHANDLE, NULL ) ) {
                _wpi_appendmenu( hmenu, MF_ENABLED|MF_SYSMENU, 0,
                                 GUIHint[GUI_MENU_FIX_TOOLBAR].id, NULLHANDLE,
                                 GUIHint[GUI_MENU_FIX_TOOLBAR].label );
            }
        }
        break;
#ifdef __OS2_PM__
    case WM_CHAR :
    case WM_TRANSLATEACCEL :
#else
    case WM_SYSKEYDOWN :
    case WM_SYSKEYUP :
    case WM_KEYUP :
    case WM_KEYDOWN :
#endif
        return( GUIProcesskey( hwnd, msg, wparam, lparam ) != 0 );
    case WM_MENUSELECT :
        GUIProcessMenuSelect( wnd, hwnd, msg, wparam, lparam );
        return( true );
    case WM_SYSCOMMAND :
        id = _wpi_getid( wparam );
        switch( id ) {
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
            _wpi_getwindowrect( hwnd, &tbar->floatrect );
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
    case WM_CLOSE :
        GUICloseToolBar( wnd );
        return( true );
        break;
    }
    return( false );
}

/*
 *  GUIXCreateToolBarWithTips -- create a tool bar, possibly with tooltips
 */

bool GUIXCreateToolBarWithTips( gui_window *wnd, bool fixed, gui_ord height,
                                int num_toolbar_items, gui_toolbar_struct *toolinfo,
                                bool excl, gui_colour_set *plain,
                                gui_colour_set *standout, gui_rect *float_pos,
                                bool use_tips )
{
    gui_coord           size;
    gui_coord           pos;
    HWND                parent;
    HWND                toolhwnd;
    toolbarinfo         *tbar;
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
    fixed_height = 0;
    fixed_width = 0;
    if( ( wnd == NULL ) || ( num_toolbar_items < 1 ) || ( toolinfo == NULL ) ||
        ( wnd->hwnd == NULLHANDLE ) || ( wnd->root == NULLHANDLE ) ) {
        return( false );
    }
    if( wnd->tbinfo != NULL ) {
        GUICloseToolBar( wnd );
    }
    tbar = wnd->tbinfo = (toolbarinfo *)GUIMemAlloc( sizeof( toolbarinfo ) );
    if( tbar == NULL ) {
        return( false );
    }
    memset( tbar, 0, sizeof( toolbarinfo ) );
    parent = wnd->root;
    tbar->fixedrect = wnd->hwnd_client_rect;
    tbar->bitmaps = (HBITMAP *)GUIMemAlloc( num_toolbar_items * sizeof( HBITMAP ) );
    if( tbar->bitmaps == NULL ) {
        GUIMemFree( tbar );
        wnd->tbinfo = NULL;
        return( false );
    }
    for( i = 0; i < num_toolbar_items; i++ ) {
        tbar->bitmaps[i] = _wpi_loadbitmap( GUIResHInst,
                                _wpi_makeintresource( toolinfo[i].bitmap ) );
        if( height == 0 ) {
            _wpi_getbitmapdim( tbar->bitmaps[i], &bm_w, &bm_h );
            if( bm_h > fixed_height ) {
                fixed_height = bm_h;
            }
            if( bm_w > fixed_width ) {
                fixed_width = bm_w;
            }
        }
    }
    tbar->info.border_size.x = BORDER_AMOUNT;
    tbar->info.border_size.y = BORDER_AMOUNT;
    /* space for border and space before border */
    adjust_amount = 2*(_wpi_getsystemmetrics( SM_CYBORDER )+BORDER_AMOUNT);
    if( height == 0 ) { /* maintian # of pixels in bitmap */
        height = fixed_height + adjust_amount + OUTLINE_AMOUNT;
        width = fixed_width + OUTLINE_AMOUNT;
    } else {
        /* only height of windows given, make bitmaps square */
        size.x = 0;
        size.y = height - 2;
        GUIScaleToScreenR( &size );
        height = size.y;
        width = size.y;
    }

    _wpi_getrectvalues( tbar->fixedrect, &left, &top, &right, &bottom );
    h      = _wpi_getheightrect( tbar->fixedrect );
    bottom = _wpi_cvth_y_plus1( height, h );
    top    = _wpi_cvth_y_plus1( top, h );
    _wpi_setwrectvalues( &tbar->fixedrect, left, top, right, bottom );
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
    new_right = width * num_toolbar_items -
                (num_toolbar_items - 1) * tbar->info.border_size.x +
                left + 2 * _wpi_getsystemmetrics( SM_CXFRAME ) +
                BORDER_AMOUNT * 2;
    if( new_right < right ) {
        right = new_right;
    }

    _wpi_setwrectvalues( &tbar->floatrect, left, top, right, bottom );
    _wpi_mapwindowpoints( parent, HWND_DESKTOP, (WPI_PPOINT)&tbar->floatrect, 2 );

    if( fixed ) {
        tbar->info.area = tbar->fixedrect;
        tbar->info.style = TOOLBAR_FIXED_STYLE;
    } else {
        if( float_pos != NULL ) {
            GUICalcLocation( float_pos, &pos, &size, parent );
            _wpi_setwrectvalues( &tbar->floatrect, pos.x, pos.y,
                                 pos.x + size.x, pos.y + size.y );
            _wpi_mapwindowpoints( parent, HWND_DESKTOP, (WPI_PPOINT)&tbar->floatrect, 2 );
        }
        tbar->info.area = tbar->floatrect;
        tbar->info.style = TOOLBAR_FLOAT_STYLE;
    }

    tbar->info.hook = guiToolBarProc;
    tbar->info.helphook = guiToolBarHelp;
    tbar->info.background = 0;
    tbar->info.foreground = 0;
    tbar->num = num_toolbar_items;
    tbar->info.is_fixed = fixed;
    tbar->info.use_tips = use_tips;

    tbar->hdl = ToolBarInit( parent );

    ToolBarDisplay( tbar->hdl, &tbar->info );

    GUIResizeBackground( wnd, true );

    for( i = 0; i < num_toolbar_items; i++ ) {
        info.u.bmp = tbar->bitmaps[i];
        info.id = toolinfo[i].id;
        info.flags = 0;
        if( use_tips && toolinfo[i].tip != NULL ) {
            strncpy( info.tip, toolinfo[i].tip, MAX_TIP );
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
                        int num_toolbar_items, gui_toolbar_struct *toolinfo,
                        bool excl, gui_colour_set *plain,
                        gui_colour_set *standout, gui_rect *float_pos )
{
    return( GUIXCreateToolBarWithTips( wnd, fixed, height, num_toolbar_items, toolinfo,
                                       excl, plain, standout, float_pos, false ) );
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
    toolbarinfo *tbar;

    tbar = wnd->tbinfo;
    if( tbar != NULL ) {
        rect = wnd->root_client_rect;
        if( wnd->root == NULLHANDLE ) {
            rect = wnd->hwnd_client_rect;
        }
        _wpi_rationalize_rect( &rect );
        if( tbar->info.is_fixed ) {
            height = _wpi_getheightrect( tbar->fixedrect );
            h = _wpi_getheightrect( rect );
            _wpi_getrectvalues( rect, &left, &top, &right, &bottom );
            t = _wpi_cvth_y_plus1( top, h );
            bottom = _wpi_cvth_y_plus1( height, h );
            _wpi_setwrectvalues( &tbar->fixedrect, left, t, right, bottom );
            t = _wpi_cvth_y_size_plus1( top, h, height );
            _wpi_movewindow( ToolBarWindow( tbar->hdl ), left, t, right - left, height, TRUE );
        }
    }
}

extern bool GUIHasToolBar( gui_window *wnd )
{
    return( wnd->tbinfo != NULL );
}

bool GUIChangeToolBar( gui_window *wnd )
{
    gui_event   gui_ev;
    toolbarinfo *tbar;
    HWND        toolhwnd;
    int         t;
    GUI_RECTDIM left, top, right, bottom;

    tbar = wnd->tbinfo;
    if( !tbar->info.is_fixed ) {
        tbar->info.is_fixed = true;
        tbar->info.style = TOOLBAR_FIXED_STYLE;
        tbar->info.area = tbar->fixedrect;
        gui_ev = GUI_TOOLBAR_FIXED;
    } else {
        tbar->info.is_fixed = false;
        tbar->info.style = TOOLBAR_FLOAT_STYLE;
        tbar->info.area = tbar->floatrect;
        _wpi_cvtc_rect_plus1( wnd->root, &tbar->info.area );
        gui_ev = GUI_TOOLBAR_FLOATING;
    }
    ToolBarDisplay( tbar->hdl, &tbar->info );
    if( tbar->info.style != TOOLBAR_FLOAT_STYLE ) {
        ToolBarRedrawButtons( tbar->hdl );
    }
    toolhwnd = ToolBarWindow( tbar->hdl );
    if( tbar->info.style == TOOLBAR_FLOAT_STYLE ) {
        _wpi_getrectvalues( tbar->floatrect, &left, &top, &right, &bottom );
        t = top;
        //t = _wpi_cvtc_y_size( wnd->hwnd, t, bottom - top );
        t = _wpi_cvtc_y_plus1( wnd->root, t );
        _wpi_movewindow( toolhwnd, left, t, right-left, bottom-top, TRUE );
    }
    GUIResizeBackground( wnd, true );
    _wpi_showwindow( toolhwnd, SW_SHOWNORMAL );
    _wpi_updatewindow( toolhwnd );
    GUIEVENTWND( wnd, gui_ev, NULL );
    return( true );
}

bool GUIToolBarFixed( gui_window *wnd )
{
    if( GUIHasToolBar( wnd ) ) {
        return( wnd->tbinfo->info.is_fixed );
    }
    return( false );
}
