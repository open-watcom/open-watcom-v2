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
#include <string.h>
#include "guistr.h"

static bool SetStructNum( hintinfo *hint, hint_type type,
                          gui_hint_struct *hint_struct, int hint_num_items )
{
    switch( type ) {
    case MENU_HINT :
        hint->menu = hint_struct;
        hint->num_menu = hint_num_items;
        return( true );
    case TOOL_HINT :
        hint->tool = hint_struct;
        hint->num_tool = hint_num_items;
        return( true );
    case FLOAT_HINT :
        hint->floating = hint_struct;
        hint->num_float = hint_num_items;
        return( true );
    case GUI_HINT :
        hint->gui = hint_struct;
        hint->num_gui = hint_num_items;
        return( true );
    default :
        return( false );
    }
}

static bool GetStructNum( hintinfo *hint, hint_type type,
                          gui_hint_struct **hint_struct, int *hint_num_items )
{
    switch( type ) {
    case MENU_HINT :
        *hint_struct = hint->menu;
        *hint_num_items = hint->num_menu;
        return( true );
    case TOOL_HINT :
        *hint_struct = hint->tool;
        *hint_num_items = hint->num_tool;
        return( true );
    case FLOAT_HINT :
        *hint_struct = hint->floating;
        *hint_num_items = hint->num_float;
        return( true );
    case GUI_HINT :
        *hint_struct = hint->gui;
        *hint_num_items = hint->num_gui;
        return( true );
    default :
        return( false );
    }
}

static bool HintTextSet( hintinfo *hint, gui_ctl_id id, hint_type type, const char *text )
{
    int                 item;
    gui_hint_struct     *hint_struct;
    int                 hint_num_items;

    if( GetStructNum( hint, type, &hint_struct, &hint_num_items ) ) {
        for( item = 0; item < hint_num_items; item++ ) {
            if( hint_struct[item].id == id ) {
                hint_struct[item].hinttext = text;
                return( true );
            }
        }
    }
    return( false );
}

static const char *HintTextGet( hintinfo *hint, gui_ctl_id id, hint_type type )
{
    int                 item;
    gui_hint_struct     *hint_struct;
    int                 hint_num_items;

    if( GetStructNum( hint, type, &hint_struct, &hint_num_items ) ) {
        for( item = 0; item < hint_num_items; item++ ) {
            if( hint_struct[item].id == id ) {
                return( hint_struct[item].hinttext );
            }
        }
    }
    return( NULL );
}

bool GUIHasHintType( gui_window *wnd, hint_type type )
{
    gui_hint_struct     *hint_struct;
    int                 hint_num_items;

    if( GetStructNum( &wnd->hint, type, &hint_struct, &hint_num_items ) ) {
        return( hint_num_items > 0 );
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
            text = HintTextGet( &wnd->hint, id, type );
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

bool GUISetHintText( gui_window *wnd, gui_ctl_id id, const char *text )
{
    return( HintTextSet( &wnd->hint, id, MENU_HINT, text ) );
}

bool GUIHasHintText( gui_window *wnd, gui_ctl_id id, hint_type type )
{
    return( HintTextGet( &wnd->hint, id, type ) != NULL );
}

bool GUIDeleteHintText( gui_window *wnd, gui_ctl_id id )
{
    int                 item;
    gui_hint_struct     *new_menu;

    if( GUIHasHintType( wnd, MENU_HINT ) ) {
        for( item = 0; item < wnd->hint.num_menu; item++ ) {
            if( wnd->hint.menu[item].id == id ) {
                new_menu = (gui_hint_struct *)GUIMemAlloc( sizeof( gui_hint_struct ) * ( wnd->hint.num_menu - 1 ) );
                memcpy( new_menu, wnd->hint.menu, sizeof( gui_hint_struct ) * item );
                memcpy( &new_menu[item], &wnd->hint.menu[item + 1], sizeof( gui_hint_struct ) * ( wnd->hint.num_menu - item - 1 ) );
                GUIMemFree( wnd->hint.menu );
                wnd->hint.menu = new_menu;
                wnd->hint.num_menu--;
                return( true );
            }
        }
    }
    return( false );
}

static int CountMenus( gui_menu_struct *menu )
{
    int         item;
    int         num_items;

    if( menu == NULL ) {
        return( 0 );
    }
    num_items = 1;
    for( item = 0; item < menu->child_num_items; item++ ) {
        num_items += CountMenus( &menu->child[item] );
    }
    return( num_items );
}

static void InsertHint( gui_menu_struct *menu, gui_hint_struct *hint, int *index )
{
    int     item;

    hint[*index].id = menu->id;
    hint[*index].hinttext = menu->hinttext;
    (*index)++;
    for( item = 0; item < menu->child_num_items; item++ ) {
        InsertHint( &menu->child[item], hint, index );
    }
}

bool GUIAppendHintText( gui_window *wnd, gui_menu_struct *menu, hint_type type )
{
    gui_hint_struct     *hint;
    int                 new_num;
    gui_hint_struct     *new_hint;
    int                 hint_num_items;

    if( GetStructNum( &wnd->hint, type, &hint, &hint_num_items ) ) {
        new_num = CountMenus( menu );
        new_hint = (gui_hint_struct *)GUIMemRealloc( hint, ( hint_num_items + new_num ) * sizeof( gui_hint_struct ) );
        if( new_hint == NULL ) {
            return( false );
        }
        InsertHint( menu, new_hint, &hint_num_items );
        SetStructNum( &wnd->hint, type, new_hint, hint_num_items );
        return( true );
    }
    return( false );
}

void GUIInitHint( gui_window *wnd, int num_items, gui_menu_struct *menu, hint_type type )
{
    int                 item;
    int                 index;
    gui_hint_struct     *hint_struct;
    int                 hint_num_items;

    if( type == TOOL_HINT ) {
        return;
    }
    if( GetStructNum( &wnd->hint, type, &hint_struct, &hint_num_items ) ) {
        if( hint_struct != NULL ) {
            GUIMemFree( hint_struct );
        }
        hint_num_items = 0;
        for( item = 0; item < num_items; item++ ) {
            hint_num_items += CountMenus( &menu[item] );
        }
        if( hint_num_items == 0 ) {
            hint_struct = NULL;
        } else {
            hint_struct = (gui_hint_struct *)GUIMemAlloc( sizeof( gui_hint_struct ) * hint_num_items );
            index = 0;
            for( item = 0; item < num_items; item++ ) {
                InsertHint( &menu[item], hint_struct, &index );
            }
        }
        SetStructNum( &wnd->hint, type, hint_struct, hint_num_items );
    }
}

void GUIInitToolbarHint( gui_window *wnd, int num_items, gui_toolbar_struct *toolinfo )
{
    int                 item;
    int                 hint_num_items;
    gui_hint_struct     *hint_struct;

    if( GetStructNum( &wnd->hint, TOOL_HINT, &hint_struct, &hint_num_items ) ) {
        if( hint_struct != NULL ) {
            GUIMemFree( hint_struct );
        }
        hint_num_items = num_items;
        if( hint_num_items == 0 ) {
            hint_struct = NULL;
        } else {
            hint_struct = (gui_hint_struct *)GUIMemAlloc( sizeof( gui_hint_struct ) * hint_num_items );
            for( item = 0; item < hint_num_items; item++ ) {
                hint_struct[item].id = toolinfo->id;
                hint_struct[item].hinttext = toolinfo->hinttext;
                toolinfo++;
            }
        }
        SetStructNum( &wnd->hint, TOOL_HINT, hint_struct, hint_num_items );
    }
}

void GUIFreeHint( gui_window *wnd )
{
    hint_type   type;

    for( type = FIRST_HINT; type <= LAST_HINT; type++ ) {
        GUIInitHint( wnd, 0, NULL, type );
    }
}
