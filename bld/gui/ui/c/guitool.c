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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#include "guiwind.h"
#include "guimenu.h"
#include "guixwind.h"
#include "guixutil.h"
#include "guiutil.h"
#include "guistr.h"
#include <string.h>
#include "guitool.h"

#define FIX_TOOLBAR     1

static gui_menu_struct Menu[] = {
    NULL, FIX_TOOLBAR, GUI_STYLE_MENU_ENABLED, NULL
};

static gui_control_info Button = {
    GUI_PUSH_BUTTON,
    NULL,
    { 0, 0, 0, 0 },
    NULL,
    GUI_NOSCROLL,
    GUI_STYLE_CONTROL_NOSTYLE,
    0
};

bool GUIXCreateFixedToolbar( gui_window *wnd )
{
    int                     i;
    gui_menu_struct         menu;
    char                    *with_excl;
    toolbarinfo             *tbar;

    tbar = wnd->tbar;
    tbar->fixed = true;
    menu.child = NoMenu;
    menu.style = GUI_STYLE_MENU_ENABLED;

    for( i = 0; i < tbar->toolinfo.num_items; i++ ) {
        with_excl = NULL;
        menu.label = tbar->toolinfo.toolbar[i].label;
        if( menu.label != NULL ) {
            with_excl = (char *)GUIMemAlloc( strlen( menu.label ) + strlen( LIT( Exclamation ) ) + 1 );
            if( with_excl != NULL ) {
                strcpy( with_excl, menu.label );
                strcat( with_excl, LIT( Exclamation ) );
            }
            menu.label = with_excl;
        }
        menu.id = tbar->toolinfo.toolbar[i].id;
        menu.hinttext = tbar->toolinfo.toolbar[i].hinttext;
        if( !GUIAppendToolbarMenu( wnd, &menu, ( i == ( tbar->toolinfo.num_items - 1 ) ) ) ) {
            GUIMemFree( with_excl );
            while( i-- > 0 ) {
                GUIDeleteToolbarMenuItem( wnd, tbar->toolinfo.toolbar[i].id );
            }
            return( false );
        }
        GUIMemFree( with_excl );
    }
    GUIEVENT( wnd, GUI_TOOLBAR_FIXED, NULL );
    return( true );
}


static bool FixToolbar( gui_window *wnd )
{
    gui_window  *parent_wnd;

    parent_wnd = wnd->parent;
    wnd->parent->tbar->switching = true;
    GUICloseWnd( wnd );
    parent_wnd->tbar->switching = false;
    return( GUIXCreateFixedToolbar( parent_wnd ) );
}

static bool ToolbarGUIEventProc( gui_window *wnd, gui_event gui_ev, void *param )
{
    gui_ctl_id  id;

    switch( gui_ev ) {
    case GUI_INIT_WINDOW:
        GUIEVENT( wnd->parent, GUI_TOOLBAR_FLOATING, NULL );
        return( true );
    case GUI_KEYDOWN:
    case GUI_KEYUP:
        GUIEVENT( wnd->parent, gui_ev, param );
        return( true );
    case GUI_CLICKED:
        GUI_GETID( param, id );
        if( id == FIX_TOOLBAR ) {
            FixToolbar( wnd );
        }
        return( true );
    case GUI_CONTROL_CLICKED:
        GUI_GETID( param, id );
        id = EV2ID( id );
        GUIEVENT( wnd->parent, GUI_CLICKED, &id );
        return( true );
    case GUI_LBUTTONDBLCLK:
        FixToolbar( wnd );
        return( true );
    case GUI_DESTROY:
        /* didn't get close first */
        if( wnd->parent->tbar->floattoolbar != NULL ) {
            wnd->parent->tbar->floattoolbar = NULL;
        }
        GUICloseToolBar( wnd->parent );
        return( true );
    case GUI_CLOSE:
        wnd->parent->tbar->floattoolbar = NULL;
        return( true );
    default:
        break;
    }
    return( false );
}

bool GUIXCloseToolBar( gui_window *wnd )
{
    int         i;
    bool        switching;
    toolbarinfo *tbar;

    if( wnd->tbar == NULL ) {
        return( true );
    }
    tbar = wnd->tbar;
    switching = tbar->switching;
    if( tbar->fixed ) {
        for( i = 0; i < tbar->toolinfo.num_items; i++ ) {
            GUIDeleteToolbarMenuItem( wnd, tbar->toolinfo.toolbar[i].id );
        }
    } else {
        if( tbar->floattoolbar != NULL ) {
            GUIDestroyWnd( tbar->floattoolbar );
        }
    }
    if( ( tbar->toolinfo.num_items > 0 ) && !switching ) {
        for( i = 0; i < tbar->toolinfo.num_items; i++ ) {
            GUIMemFree( (void *)tbar->toolinfo.toolbar[i].label );
        }
        GUIMemFree( (void *)tbar->toolinfo.toolbar );
        GUIMemFree( tbar );
        wnd->tbar = NULL;
    }
    if( !switching ) {
        GUIEVENT( wnd, GUI_TOOLBAR_DESTROYED, NULL );
    }
    return( true );
}

bool GUIAPI GUIHasToolBar( gui_window *wnd )
{
    return( wnd->tbar != NULL );
}

bool GUIAPI GUIToolBarFixed( gui_window *wnd )
{
    if( GUIHasToolBar( wnd ) ) {
        return( wnd->tbar->fixed );
    }
    return( false );
}

static gui_create_info FloatingToolbar = {
    NULL,
    { 0, 0, 0, 0 },
    GUI_NOSCROLL,
    GUI_SYSTEM_MENU | GUI_VISIBLE | GUI_RESIZEABLE | GUI_CLOSEABLE | GUI_INIT_INVISIBLE,
    NULL,
    GUI_MENU_ARRAY( Menu ),             // Menu array
    GUI_NO_COLOUR,                      // Colour attribute array
    ToolbarGUIEventProc,                // GUI Event Callback function
    NULL,
    NULL,
    NULL                                // Menu Resource
};


static bool CreateFloatingToolbar( gui_window *wnd, gui_ord height )
{
    gui_text_metrics    metrics;
    int                 i;
    int                 loc;
    gui_rect            client;
    gui_rect            size;
    toolbarinfo         *tbar;
    gui_colour_set      *plain;
    gui_colour_set      *standout;

    tbar = wnd->tbar;
    tbar->fixed = false;
    GUIGetTextMetrics( wnd, &metrics );
    if( height == 0 ) {
        height = 2 * metrics.avg.y;
    }
    height += 2 * metrics.avg.y;
    GUIGetClientRect( wnd, &size );
    FloatingToolbar.parent = wnd;
    FloatingToolbar.title = LIT( Floating_Toolbar );
    FloatingToolbar.menus.menu[0].label = LIT( XFix_Toolbar );
    FloatingToolbar.menus.menu[0].hinttext = LIT( Fix_Toolbar_Hint );
    FloatingToolbar.rect.height = height;
    FloatingToolbar.rect.width = size.width;
    FloatingToolbar.colours.num_items = GUIGetNumWindowColours( wnd );
    FloatingToolbar.colours.colour = GUIGetWindowColours( wnd );
    FloatingToolbar.colours.colour[GUI_FRAME_INACTIVE] = FloatingToolbar.colours.colour[GUI_FRAME_ACTIVE];
    tbar->floattoolbar = GUICreateWindow( &FloatingToolbar );
    GUIMemFree( FloatingToolbar.colours.colour );
    FloatingToolbar.colours.colour = NULL;
    Button.parent = tbar->floattoolbar;
    loc = 0;
    if( tbar->has_colours ) {
        plain = &tbar->plain;
        standout = &tbar->standout;
    } else {
        plain = NULL;
        standout = NULL;
    }
    for( i = 0; i < tbar->toolinfo.num_items; i++ ) {
        Button.text = tbar->toolinfo.toolbar[i].label;
        Button.id = tbar->toolinfo.toolbar[i].id;
        Button.rect.x = loc;
        Button.rect.width = ( strlen( Button.text ) + 4 ) * metrics.avg.x;
        Button.rect.height = metrics.avg.y * 2;
        loc += Button.rect.width;
        if( !GUIAddControl( &Button, plain, standout ) ) {
            GUIDestroyWnd( tbar->floattoolbar );
            return( false );
        }
    }
    GUIGetClientRect( tbar->floattoolbar, &client );
    GUIGetRect( tbar->floattoolbar, &size );
    size.width += loc - client.width;
    GUIResizeWindow( tbar->floattoolbar, &size );
    GUIShowWindow( tbar->floattoolbar );
    return( true );
}

bool GUIXCreateToolBarWithTips( gui_window *wnd, bool fixed, gui_ord height,
                       const gui_toolbar_items *toolinfo, bool excl,
                       gui_colour_set *plain, gui_colour_set *standout,
                       const gui_rect *float_pos, bool use_tips )
{
    int                 size;
    int                 i;
    toolbarinfo         *tbar;
    gui_toolbar_struct  *new_toolinfo;
    bool                ok;

    /* unused parameters */ (void)float_pos; (void)use_tips;

    if( ( wnd->parent != NULL ) || ( plain == NULL ) || ( standout == NULL ) ) {
        return( false );
    }
    tbar = wnd->tbar = (toolbarinfo *)GUIMemAlloc( sizeof( toolbarinfo ) );
    if( tbar == NULL ) {
        return( false );
    }
    if( toolinfo->num_items == 0 ) {
        tbar->toolinfo = NoToolbar;
    } else {
        size = sizeof( gui_toolbar_struct ) * toolinfo->num_items;
        new_toolinfo = (gui_toolbar_struct *)GUIMemAlloc( size );
        if( new_toolinfo == NULL ) {
            GUIMemFree( tbar );
            wnd->tbar = NULL;
            return( false );
        }
        memset( new_toolinfo, 0, size );
        for( i = 0; i < toolinfo->num_items; i++ ) {
            new_toolinfo[i].label = GUIStrDup( toolinfo->toolbar[i].label, &ok );
            if( !ok ) {
                while( i-- > 0  ) {
                    GUIMemFree( (void *)new_toolinfo[i].label );
                }
                GUIMemFree( new_toolinfo );
                GUIMemFree( tbar );
                wnd->tbar = NULL;
                return( false );
            }
            new_toolinfo[i].id = toolinfo->toolbar[i].id;
        }
        tbar->toolinfo.num_items = toolinfo->num_items;
        tbar->toolinfo.toolbar = new_toolinfo;
    }
    tbar->floattoolbar = NULL;
    tbar->switching = false;
    tbar->excl = excl;
    tbar->has_colours = ( plain != NULL ) && ( standout != NULL );
    if( tbar->has_colours ) {
        tbar->plain.fore = plain->fore;
        tbar->plain.back = plain->back;
        tbar->standout.fore = standout->fore;
        tbar->standout.back = standout->back;
    }
    if( fixed ) {
        return( GUIXCreateFixedToolbar( wnd ) );
    } else {
        return( CreateFloatingToolbar( wnd, height ) );
    }
}

bool GUIXCreateToolBar( gui_window *wnd, bool fixed, gui_ord height,
                                const gui_toolbar_items *toolinfo, bool excl,
                                gui_colour_set *plain, gui_colour_set *standout,
                                const gui_rect *float_pos )
{
    return( GUIXCreateToolBarWithTips( wnd, fixed, height, toolinfo, excl, plain, standout, float_pos, false ) );
}

static bool FloatToolbar( gui_window *wnd )
{
    wnd->tbar->switching = true;
    GUIXCloseToolBar( wnd );
    wnd->tbar->switching = false;
    return( CreateFloatingToolbar( wnd, 0 ) );
}

bool GUIAPI GUIChangeToolBar( gui_window *wnd )
{
    if( GUIHasToolBar( wnd ) ) {
        if( GUIToolBarFixed( wnd ) ) {
            return( FloatToolbar( wnd ) );
        } else {
            return( FixToolbar( wnd->tbar->floattoolbar ) );
        }
    }
    return( false );
}
