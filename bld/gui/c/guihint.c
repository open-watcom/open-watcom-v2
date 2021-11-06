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


#include "guiwind.h"
#include <string.h>
#include "guistr.h"

static bool SetStructNum( hints_info *hintsinfo, hint_type type, const gui_hint_items *hints )
{
    switch( type ) {
    case MENU_HINT:
        hintsinfo->menu = *hints;
        return( true );
    case TOOL_HINT:
        hintsinfo->tool = *hints;
        return( true );
    case FLOAT_HINT:
        hintsinfo->floating = *hints;
        return( true );
    case GUI_HINT:
        hintsinfo->gui = *hints;
        return( true );
    default:
        return( false );
    }
}

static bool GetStructNum( hints_info *hintsinfo, hint_type type, gui_hint_items *hints )
{
    switch( type ) {
    case MENU_HINT:
        *hints = hintsinfo->menu;
        return( true );
    case TOOL_HINT:
        *hints = hintsinfo->tool;
        return( true );
    case FLOAT_HINT:
        *hints = hintsinfo->floating;
        return( true );
    case GUI_HINT:
        *hints = hintsinfo->gui;
        return( true );
    default:
        return( false );
    }
}

static bool HintTextSet( hints_info *hintsinfo, gui_ctl_id id, hint_type type, const char *text )
{
    int                 i;
    gui_hint_items      hints;

    if( GetStructNum( hintsinfo, type, &hints ) ) {
        for( i = 0; i < hints.num_items; i++ ) {
            if( hints.hint[i].id == id ) {
                hints.hint[i].hinttext = text;
                return( true );
            }
        }
    }
    return( false );
}

static const char *HintTextGet( hints_info *hintsinfo, gui_ctl_id id, hint_type type )
{
    int                 i;
    gui_hint_items      hints;

    if( GetStructNum( hintsinfo, type, &hints ) ) {
        for( i = 0; i < hints.num_items; i++ ) {
            if( hints.hint[i].id == id ) {
                return( hints.hint[i].hinttext );
            }
        }
    }
    return( NULL );
}

static bool HintTextDelete( hints_info *hintsinfo, gui_ctl_id id, hint_type type )
{
    int                 i;
    gui_hint_items      hints;
    gui_hint_struct     *new_hints;

    if( GetStructNum( hintsinfo, type, &hints ) ) {
        for( i = 0; i < hints.num_items; i++ ) {
            if( hints.hint[i].id == id ) {
                hints.num_items--;
                new_hints = (gui_hint_struct *)GUIMemAlloc( sizeof( gui_hint_struct ) * hints.num_items );
                memcpy( new_hints, hints.hint, sizeof( gui_hint_struct ) * i );
                memcpy( &new_hints[i], &hints.hint[i + 1], sizeof( gui_hint_struct ) * ( hints.num_items - i ) );
                GUIMemFree( hints.hint );
                hints.hint = new_hints;
                SetStructNum( hintsinfo, type, &hints );
                return( true );
            }
        }
    }
    return( false );
}

bool GUIHasHintType( gui_window *wnd, hint_type type )
{
    gui_hint_items      hints;

    if( GetStructNum( &wnd->hintsinfo, type, &hints ) ) {
        return( hints.num_items > 0 );
    }
    return( false );
}

bool GUIDisplayHintText( gui_window *wnd_with_status, gui_window *wnd,
                         gui_ctl_id id, hint_type type, gui_menu_styles style )
{
    const char      *text;

    if( GUIHasStatus( wnd_with_status ) && GUIHasHintType( wnd, type ) ) {
        if( (style & GUI_STYLE_MENU_IGNORE) || (style & GUI_STYLE_MENU_SEPARATOR) ) {
            GUIClearStatusText( wnd_with_status );
        } else {
            text = HintTextGet( &wnd->hintsinfo, id, type );
            if( text != NULL ) {
                GUIDrawStatusText( wnd_with_status, text );
                return( true );
            }
            GUIClearStatusText( wnd_with_status );
            return( true );
        }
        return( true );
    }
    return( false );
}

bool GUIAPI GUISetMenuHintText( gui_window *wnd, gui_ctl_id id, const char *text )
{
    return( HintTextSet( &wnd->hintsinfo, id, MENU_HINT, text ) );
}

bool GUISetHintText( gui_window *wnd, gui_ctl_id id, hint_type type, const char *text )
{
    return( HintTextSet( &wnd->hintsinfo, id, type, text ) );
}

bool GUIHasHintText( gui_window *wnd, gui_ctl_id id, hint_type type )
{
    return( HintTextGet( &wnd->hintsinfo, id, type ) != NULL );
}

bool GUIDeleteHintText( gui_window *wnd, gui_ctl_id id, hint_type type )
{
    return( HintTextDelete( &wnd->hintsinfo, id, type ) );
}

static int CountMenus( const gui_menu_struct *menu )
{
    int         i;
    int         num_items;

    if( menu == NULL ) {
        return( 0 );
    }
    num_items = 1;
    for( i = 0; i < menu->child.num_items; i++ ) {
        num_items += CountMenus( &menu->child.menu[i] );
    }
    return( num_items );
}

static void InsertHint( const gui_menu_struct *menu, gui_hint_items *hints )
{
    int     i;

    hints->hint[hints->num_items].id = menu->id;
    hints->hint[hints->num_items].hinttext = menu->hinttext;
    hints->num_items++;
    for( i = 0; i < menu->child.num_items; i++ ) {
        InsertHint( &menu->child.menu[i], hints );
    }
}

bool GUIAppendHintText( gui_window *wnd, const gui_menu_struct *menu, hint_type type )
{
    gui_hint_items      hints;
    int                 new_num;
    gui_hint_struct     *new_hints;

    if( GetStructNum( &wnd->hintsinfo, type, &hints ) ) {
        new_num = CountMenus( menu );
        new_hints = (gui_hint_struct *)GUIMemRealloc( hints.hint, ( hints.num_items + new_num ) * sizeof( gui_hint_struct ) );
        if( new_hints != NULL ) {
            hints.hint = new_hints;
            InsertHint( menu, &hints );
            SetStructNum( &wnd->hintsinfo, type, &hints );
            return( true );
        }
    }
    return( false );
}

void GUIInitHint( gui_window *wnd, const gui_menu_items *menus, hint_type type )
{
    int                 i;
    gui_hint_items      hints;

    if( type == TOOL_HINT ) {
        return;
    }
    if( GetStructNum( &wnd->hintsinfo, type, &hints ) ) {
        if( hints.hint != NULL ) {
            GUIMemFree( hints.hint );
            hints.hint = NULL;
        }
        hints.num_items = 0;
        for( i = 0; i < menus->num_items; i++ ) {
            hints.num_items += CountMenus( &menus->menu[i] );
        }
        if( hints.num_items > 0 ) {
            hints.hint = (gui_hint_struct *)GUIMemAlloc( sizeof( gui_hint_struct ) * hints.num_items );
            hints.num_items = 0;
            for( i = 0; i < menus->num_items; i++ ) {
                InsertHint( &menus->menu[i], &hints );
            }
        }
        SetStructNum( &wnd->hintsinfo, type, &hints );
    }
}

void GUIInitToolbarHint( gui_window *wnd, const gui_toolbar_items *toolinfo )
{
    int                 i;
    gui_hint_items      hints;

    if( GetStructNum( &wnd->hintsinfo, TOOL_HINT, &hints ) ) {
        if( hints.hint != NULL ) {
            GUIMemFree( hints.hint );
            hints.hint = NULL;
        }
        hints.num_items = toolinfo->num_items;
        if( hints.num_items > 0 ) {
            hints.hint = (gui_hint_struct *)GUIMemAlloc( sizeof( gui_hint_struct ) * hints.num_items );
            for( i = 0; i < hints.num_items; i++ ) {
                hints.hint[i].id = toolinfo->toolbar[i].id;
                hints.hint[i].hinttext = toolinfo->toolbar[i].hinttext;
            }
        }
        SetStructNum( &wnd->hintsinfo, TOOL_HINT, &hints );
    }
}

void GUIFiniHint( gui_window *wnd, hint_type type )
{
    gui_hint_items      hints;

    if( type == TOOL_HINT ) {
        return;
    }
    if( GetStructNum( &wnd->hintsinfo, type, &hints ) ) {
        if( hints.hint != NULL ) {
            GUIMemFree( hints.hint );
            hints.hint = NULL;
        }
        hints.num_items = 0;
        SetStructNum( &wnd->hintsinfo, type, &hints );
    }
}

void GUIFreeHint( gui_window *wnd )
{
    hint_type   type;

    for( type = FIRST_HINT; type <= LAST_HINT; type++ ) {
        GUIFiniHint( wnd, type );
    }
}
