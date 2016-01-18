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


#include "guiwind.h"
#include "guimenu.h"
#include "guixwind.h"
#include "guiutil.h"
#include "guistr.h"
#include <string.h>

#define FIX_TOOLBAR     1

static gui_menu_struct Menu = {
    NULL, FIX_TOOLBAR, GUI_ENABLED, NULL
};
#define NUM_MENU_ITEMS ( sizeof( Menu ) / sizeof( gui_menu_struct ) )

static GUICALLBACK ToolbarCallBack;

static gui_create_info FloatingToolbar = {
    NULL,
    { 0, 0, 0, 0 },
    GUI_NOSCROLL,
    GUI_SYSTEM_MENU | GUI_VISIBLE | GUI_RESIZEABLE | GUI_CLOSEABLE | GUI_INIT_INVISIBLE,
    NULL,
    NUM_MENU_ITEMS,
    &Menu,
    0,
    NULL,
    ToolbarCallBack,
    NULL,
    NULL,
    NULL                                // Menu Resource
};

static gui_control_info Button = {
    GUI_PUSH_BUTTON,
    NULL,
    { 0, 0, 0, 0 },
    NULL,
    GUI_NOSCROLL,
    GUI_NONE,
    0
};

extern bool GUIXCreateFixedToolbar( gui_window *wnd )
{
    int                 i;
    int                 j;
    gui_menu_struct     menu;
    char                *with_excl;
    toolbarinfo         *tbar;

    tbar = wnd->tbinfo;
    tbar->fixed = true;
    menu.num_child_menus = 0;
    menu.child = NULL;
    menu.style = GUI_ENABLED;

    for( i = 0; i < tbar->num_items; i++ ) {
        menu.label = tbar->info[i].label;
        if( menu.label != NULL ) {
            with_excl = (char *)GUIMemAlloc( strlen( menu.label ) + 2 );
            if( with_excl != NULL ) {
                strcpy( with_excl, menu.label );
                strcat( with_excl, LIT( Exclamation ) );
            }
            menu.label = with_excl;
        }
        menu.id = tbar->info[i].id;
        menu.hinttext = tbar->info[i].hinttext;
        if( !GUIAppendToolbarMenu( wnd, &menu, ( i == ( tbar->num_items - 1 ) ) ) ) {
            GUIMemFree( with_excl );
            for( j = 0; j < i; j++ ) {
                GUIDeleteToolbarMenuItem( wnd, tbar->info[j].id );
            }
            return( false );
        }
        GUIMemFree( with_excl );
    }
    GUIEVENTWND( wnd, GUI_TOOLBAR_FIXED, NULL );
    return( true );
}


static bool FixToolbar( gui_window *wnd )
{
    gui_window  *parent;

    parent = wnd->parent;
    wnd->parent->tbinfo->switching = true;
    GUICloseWnd( wnd );
    parent->tbinfo->switching = false;
    return( GUIXCreateFixedToolbar( parent ) );
}

bool ToolbarCallBack( gui_window *wnd, gui_event gui_ev, void *param )
{
    gui_ctl_id  id;

    switch( gui_ev ) {
    case GUI_INIT_WINDOW :
        GUIEVENTWND( wnd->parent, GUI_TOOLBAR_FLOATING, NULL );
        return( true );
    case GUI_KEYDOWN :
    case GUI_KEYUP :
        GUIEVENTWND( wnd->parent, gui_ev, param );
        break;
    case GUI_CLICKED :
        GUI_GETID( param, id );
        if( id == FIX_TOOLBAR ) {
            FixToolbar( wnd );
        }
        break;
    case GUI_CONTROL_CLICKED :
        GUI_GETID( param, id );
        id = EV2ID( id );
        GUIEVENTWND( wnd->parent, GUI_CLICKED, &id );
        break;
    case GUI_LBUTTONDBLCLK :
        FixToolbar( wnd );
        break;
    case GUI_DESTROY :
        /* didn't get close first */
        if( wnd->parent->tbinfo->floattoolbar != NULL ) {
            wnd->parent->tbinfo->floattoolbar = NULL;
        }
        GUICloseToolBar( wnd->parent );
        break;
    case GUI_CLOSE :
        wnd->parent->tbinfo->floattoolbar = NULL;
        break;
    default :
        break;
    }
    return( true );
}


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

    tbar = wnd->tbinfo;
    tbar->fixed = false;
    GUIGetTextMetrics( wnd, &metrics );
    if( height == 0 ) {
        height = 2 * metrics.avg.y;
    }
    height += 2 * metrics.avg.y;
    GUIGetClientRect( wnd, &size );
    FloatingToolbar.parent = wnd;
    FloatingToolbar.title = LIT( Floating_Toolbar );
    FloatingToolbar.menu->label = LIT( XFix_Toolbar );
    FloatingToolbar.menu->hinttext = LIT( Fix_Toolbar_Hint );
    FloatingToolbar.rect.height = height;
    FloatingToolbar.rect.width = size.width;
    FloatingToolbar.num_attrs = GUIGetNumWindowColours( wnd );
    FloatingToolbar.colours = GUIGetWindowColours( wnd );
    FloatingToolbar.colours[GUI_FRAME_INACTIVE] = FloatingToolbar.colours[GUI_FRAME_ACTIVE];
    tbar->floattoolbar = GUICreateWindow( &FloatingToolbar );
    GUIMemFree( FloatingToolbar.colours );
    FloatingToolbar.colours = NULL;
    Button.parent = tbar->floattoolbar;
    loc = 0;
    if( tbar->has_colours ) {
        plain = &tbar->plain;
        standout = &tbar->standout;
    } else {
        plain = NULL;
        standout = NULL;
    }
    for( i = 0; i < tbar->num_items; i++ ) {
        Button.text = tbar->info[i].label;
        Button.id = tbar->info[i].id;
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

bool GUIXCreateToolBar( gui_window *wnd, bool fixed, gui_ord height,
                       int num_items, gui_toolbar_struct *toolinfo, bool excl,
                       gui_colour_set *plain, gui_colour_set *standout,
                       gui_rect *float_pos )
{
    int         size;
    int         i;
    int         j;
    toolbarinfo *tbar;

    float_pos = float_pos;

    if( ( wnd->parent != NULL ) || ( plain == NULL ) || ( standout == NULL ) ) {
        return( false );
    }
    tbar = wnd->tbinfo = (toolbarinfo *)GUIMemAlloc( sizeof( toolbarinfo ) );
    if( tbar == NULL ) {
        return( false );
    }
    size = sizeof( gui_toolbar_struct ) * num_items;
    tbar->info = (gui_toolbar_struct *)GUIMemAlloc( size );
    if( tbar->info == NULL ) {
        GUIMemFree( tbar );
        wnd->tbinfo = NULL;
        return( false );
    }
    memset( tbar->info, 0, size );
    tbar->excl = excl;
    tbar->has_colours = ( plain != NULL ) && ( standout != NULL );
    if( tbar->has_colours ) {
        tbar->plain.fore = plain->fore;
        tbar->plain.back = plain->back;
        tbar->standout.fore = standout->fore;
        tbar->standout.back = standout->back;
    }
    for( i = 0; i < num_items; i++ ) {
        bool    ok;

        tbar->info[i].label = GUIStrDup( toolinfo[i].label, &ok );
        if( !ok ) {
            for( j = 0; j < i; j++ ) {
                GUIMemFree( (void *)tbar->info[j].label );
            }
            GUIMemFree( tbar->info );
            GUIMemFree( tbar );
            wnd->tbinfo = NULL;
            return( false );
        }
        tbar->info[i].id = toolinfo[i].id;
    }
    tbar->num_items = num_items;
    tbar->floattoolbar = NULL;
    tbar->switching = false;
    if( fixed ) {
        return( GUIXCreateFixedToolbar( wnd ) );
    } else {
        return( CreateFloatingToolbar( wnd, height ) );
    }
}

bool GUIXCreateToolBarWithTips( gui_window *wnd, bool fixed, gui_ord height,
                                int num_items, gui_toolbar_struct *toolinfo, bool excl,
                                gui_colour_set *plain, gui_colour_set *standout,
                                gui_rect *float_pos, bool use_tips )
{
    use_tips = use_tips;
    return( GUIXCreateToolBar( wnd, fixed, height, num_items, toolinfo, excl, plain,
                               standout, float_pos ) );
}

bool GUIXCloseToolBar( gui_window *wnd )
{
    int         i;
    bool        switching;
    toolbarinfo *tbar;

    if( wnd->tbinfo == NULL ) {
        return( true );
    }
    tbar = wnd->tbinfo;
    switching = tbar->switching;
    if( tbar->fixed ) {
        for( i = 0; i < tbar->num_items; i++ ) {
            GUIDeleteToolbarMenuItem( wnd, tbar->info[i].id );
        }
    } else {
        if( tbar->floattoolbar != NULL ) {
            GUIDestroyWnd( tbar->floattoolbar );
        }
    }
    if( ( tbar->info != NULL ) && !switching ) {
        for( i = 0; i < tbar->num_items; i++ ) {
            GUIMemFree( (void *)tbar->info[i].label );
        }
        GUIMemFree( tbar->info );
        GUIMemFree( tbar );
        wnd->tbinfo = NULL;
    }
    if( !switching ) {
        GUIEVENTWND( wnd, GUI_TOOLBAR_DESTROYED, NULL );
    }
    return( true );
}

bool GUIHasToolBar( gui_window *wnd )
{
    return( wnd->tbinfo != NULL );
}

bool GUIToolBarFixed( gui_window *wnd )
{
    if( GUIHasToolBar( wnd ) ) {
        return( wnd->tbinfo->fixed );
    }
    return( false );
}

static bool FloatToolbar( gui_window *wnd )
{
    wnd->tbinfo->switching = true;
    GUIXCloseToolBar( wnd );
    wnd->tbinfo->switching = false;
    return( CreateFloatingToolbar( wnd, 0 ) );
}

bool GUIChangeToolBar( gui_window *wnd )
{
    if( GUIHasToolBar( wnd ) ) {
        if( GUIToolBarFixed( wnd ) ) {
            return( FloatToolbar( wnd ) );
        } else {
            return( FixToolbar( wnd->tbinfo->floattoolbar ) );
        }
    }
    return( false );
}
