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
                          gui_hint_struct *hint_struct, int num )
{
    switch( type ) {
    case MENU_HINT :
        hint->menu = hint_struct;
        hint->num_menu = num;
        return( true );
    case TOOL_HINT :
        hint->tool = hint_struct;
        hint->num_tool = num;
        return( true );
    case FLOAT_HINT :
        hint->floating = hint_struct;
        hint->num_float = num;
        return( true );
    case GUI_HINT :
        hint->gui = hint_struct;
        hint->num_gui = num;
        return( true );
    default :
        return( false );
    }
}

static bool GetStructNum( hintinfo *hint, hint_type type,
                          gui_hint_struct **hint_struct, int *num )
{
    switch( type ) {
    case MENU_HINT :
        *hint_struct = hint->menu;
        *num = hint->num_menu;
        return( true );
    case TOOL_HINT :
        *hint_struct = hint->tool;
        *num = hint->num_tool;
        return( true );
    case FLOAT_HINT :
        *hint_struct = hint->floating;
        *num = hint->num_float;
        return( true );
    case GUI_HINT :
        *hint_struct = hint->gui;
        *num = hint->num_gui;
        return( true );
    default :
        return( false );
    }
}

static bool HintTextSet( hintinfo *hint, gui_ctl_id id, hint_type type, const char *text )
{
    int                 i;
    gui_hint_struct     *hint_struct;
    int                 num;

    if( GetStructNum( hint, type, &hint_struct, &num ) ) {
        for( i = 0; i < num; i++ ) {
            if( hint_struct[i].id == id ) {
                hint_struct[i].hinttext = text;
                return( true );
            }
        }
    }
    return( false );
}

static const char *HintTextGet( hintinfo *hint, gui_ctl_id id, hint_type type )
{
    int                 i;
    gui_hint_struct     *hint_struct;
    int                 num;

    if( GetStructNum( hint, type, &hint_struct, &num ) ) {
        for( i = 0; i < num; i++ ) {
            if( hint_struct[i].id == id ) {
                return( hint_struct[i].hinttext );
            }
        }
    }
    return( NULL );
}

bool GUIHasHintType( gui_window *wnd, hint_type type )
{
    gui_hint_struct     *hint_struct;
    int                 num;

    if( GetStructNum( &wnd->hint, type, &hint_struct, &num ) ) {
        return( num > 0 );
    }
    return( false );
}

bool GUIDisplayHintText( gui_window *wnd_with_status, gui_window *wnd,
                         gui_ctl_id id, hint_type type, gui_menu_styles style )
{
    const char      *text;

    if( GUIHasStatus( wnd_with_status ) && GUIHasHintType( wnd, type ) ) {
        if( ( style & GUI_IGNORE ) || ( style & GUI_SEPARATOR ) ) {
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
    int                 i;
    gui_hint_struct     *new_menu;
    int                 index;
    bool                found;

    found = false;
    if( GUIHasHintType( wnd, MENU_HINT ) ) {
        index = 0;
        for( i = 0; (i < wnd->hint.num_menu) && !found; i++ ) {
            if( wnd->hint.menu[i].id == id ) {
                found = true;
                index = i;
            }
        }
        if( found ) {
            new_menu = (gui_hint_struct *)GUIMemAlloc( sizeof( gui_hint_struct )
                                    * ( wnd->hint.num_menu - 1 ) );
            memcpy( new_menu, wnd->hint.menu, sizeof( gui_hint_struct ) * index );
            memcpy( &new_menu[index], &wnd->hint.menu[index+1],
                    sizeof( gui_hint_struct ) * ( wnd->hint.num_menu - index - 1 ) );
            GUIMemFree( wnd->hint.menu );
            wnd->hint.menu = new_menu;
            wnd->hint.num_menu--;
        }
    }
    return( found );
}

static int CountMenus( gui_menu_struct *menu )
{
    int i;
    int num;

    if( menu == NULL ) {
        return( 0 );
    }
    num = 1;
    for( i = 0; i < menu->num_child_menus; i++ ) {
        num += CountMenus( &menu->child[i] );
    }
    return( num );
}

static void InsertHint( gui_menu_struct *menu, gui_hint_struct *hint, int *index )
{
    int i;

    hint[*index].id = menu->id;
    hint[*index].hinttext = menu->hinttext;
    (*index)++;
    for( i = 0; i < menu->num_child_menus; i++ ) {
        InsertHint( &menu->child[i], hint, index );
    }
}

bool GUIAppendHintText( gui_window *wnd, gui_menu_struct *menu, hint_type type )
{
    int                 num;
    gui_hint_struct     *hint;
    int                 new_num;
    gui_hint_struct     *new_hint;

    if( GetStructNum( &wnd->hint, type, &hint, &num ) ) {
        new_num = CountMenus( menu );
        new_hint = (gui_hint_struct *)GUIMemRealloc( hint,
                        ( num + new_num ) * sizeof( gui_hint_struct ) );
        if( new_hint == NULL ) {
            return( false );
        }
        InsertHint( menu, new_hint, &num );
        SetStructNum( &wnd->hint, type, new_hint, num );
        return( true );
    }
    return( false );
}

void GUIInitHint( gui_window *wnd, int num_menus, gui_menu_struct *menu, hint_type type )
{
    int                 size;
    int                 i;
    int                 index;
    gui_hint_struct     *hint_struct;
    int                 num;

    if( type == TOOL_HINT ) {
        return;
    }
    if( GetStructNum( &wnd->hint, type, &hint_struct, &num ) ) {
        if( hint_struct != NULL ) {
            GUIMemFree( hint_struct );
        }
        num = 0;
        for( i = 0; i < num_menus; i++ ) {
            num += CountMenus( &menu[i] );
        }
        size = sizeof( gui_hint_struct ) * num;
        if( size == 0 ) {
            hint_struct = NULL;
            num = 0;
        } else {
            hint_struct = (gui_hint_struct *)GUIMemAlloc( size );
            index = 0;
            for( i = 0; i < num_menus; i++ ) {
                InsertHint( &menu[i], hint_struct, &index );
            }
        }
        SetStructNum( &wnd->hint, type, hint_struct, num );
    }
}

void GUIInitToolbarHint( gui_window *wnd, int num_items,
                         gui_toolbar_struct *toolinfo )
{
    int                 i;
    int                 size;
    gui_hint_struct     *hint_struct;
    int                 num;

    if( GetStructNum( &wnd->hint, TOOL_HINT, &hint_struct, &num ) ) {
        if( hint_struct != NULL ) {
            GUIMemFree( hint_struct );
        }
        size = sizeof( gui_hint_struct ) * num_items;
        if( size == 0 ) {
            hint_struct = NULL;
            num = 0;
        } else {
            hint_struct = (gui_hint_struct *)GUIMemAlloc( size );
            num = num_items;
            for( i = 0; i < num_items; i++ ) {
                hint_struct[i].id = toolinfo[i].id;
                hint_struct[i].hinttext = toolinfo[i].hinttext;
            }
        }
        SetStructNum( &wnd->hint, TOOL_HINT, hint_struct, num );
    }
}

void GUIFreeHint( gui_window *wnd )
{
    hint_type   type;

    for( type = FIRST_HINT; type <= LAST_HINT; type ++ ) {
        GUIInitHint( wnd, 0, NULL, type );
    }
}
