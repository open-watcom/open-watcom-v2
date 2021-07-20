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
#include "guicontr.h"
#include "guilistb.h"
#include "guiutil.h"
#include "guixutil.h"
#include "guixdlg.h"
#include <string.h>

typedef const char  **lb_data;

static bool FillInEmptyList( a_list *list )
{
    list->choice = 0;
    list->data_handle = (const void *)GUIMemAlloc( sizeof( char * ) );
    if( list->data_handle == NULL ) {
        return( false );
    }
    ((lb_data)list->data_handle)[0] = NULL;
    return( true );
}

bool GUIClearListBox( a_list *list )
{
    bool ret;

    GUIFreeList( list, false );
    ret = FillInEmptyList( list );
    uiupdatelistbox( list );
    return( ret );
}

bool GUIListBoxDeleteItem( a_list *list, int choice )
{
    int         num_items;
    lb_data     old_data;
    lb_data     new_data;
    int         i;

    num_items = GUIListSize( list );
    if( choice >= num_items ) {
        return( false );
    }
    new_data = (lb_data)GUIMemAlloc( sizeof( char * ) * num_items );
    if( new_data == NULL ) {
        return( false );
    }
    old_data = (lb_data)list->data_handle;
    for( i = 0; i < choice; i++ ) {
        new_data[i] = old_data[i];
    }
    GUIMemFree( (void *)old_data[choice] );
    for( i = choice; i < num_items; i++ ) {
        new_data[i] = old_data[i + 1];
    }
    GUIMemFree( (void *)old_data );
    list->data_handle = (const void *)new_data;
    if( choice >= GUIListSize( list )  ) {
        list->choice = 0;
    }
    uiupdatelistbox( list );
    return( true );
}

void GUIFreeList( a_list *list, bool free_list )
{
    int         num_items;
    int         i;
    lb_data     data;

    num_items = GUIListSize( list );
    data = (lb_data)list->data_handle;
    for( i = 0; i < num_items; i++ ) {
        GUIMemFree( (void *)data[i] );
    }
    GUIMemFree( (void *)data );
    list->choice = 0;
    list->data_handle = NULL;
    if( free_list ) {
        GUIMemFree( list );
    }
}

bool GUIFillInListBox( a_list *list )
{
    list->get = NULL;
    list->box = NULL;
    list->choice = 0;
    return( FillInEmptyList( list ) );
}

a_list *GUICreateListBox( void )
{
    a_list      *list;

    list = (a_list *)GUIMemAlloc( sizeof( a_list ) );
    if( list == NULL ) {
        return( NULL );
    }
    if( !GUIFillInListBox( list ) ) {
        GUIMemFree( list );
        list = NULL;
    }
    return( list );
}

a_list *GUICreateEditMLE( const char *text )
{
    a_list      *list;
    char        *text_copy;
    char        *line;
    char        *end;
    char        *absolute_end;
    #define     MLE_NEWLINE     "\r\n"

    list = (a_list *)GUIMemAlloc( sizeof( a_list ) );
    if( list == NULL ) {
        return( NULL );
    }
    if( !GUIFillInListBox( list ) ) {
        GUIMemFree( list );
        list = NULL;
    } else {
        text_copy = GUIStrDup( text, NULL );
        line = text_copy;
        absolute_end = text_copy + strlen( text_copy );
        end = strstr( line, MLE_NEWLINE );
        if( end != NULL ) {
            *end = '\0';
        }
        while( line != absolute_end && end != NULL ) {
            GUIListBoxAddText( list, line, -1 );
            line = end + 2;
            end = strstr( line, MLE_NEWLINE );
            if( end != NULL ) {
                *end = '\0';
            }
        }
        GUIListBoxAddText( list, line, -1 );
        GUIMemFree( text_copy );
    }
    return( list );
}

int GUIListSize( a_list *list )
{
    return( uilistsize( list ) );
}

static lb_data ResizeList( a_list *list, unsigned num_to_add, int *choice )
{
    int         num_items;
    lb_data     old_data;
    lb_data     new_data;

    num_items = GUIListSize( list );
    new_data = (lb_data)GUIMemAlloc( ( num_items + num_to_add + 1 ) * sizeof( char * ) );
    if( new_data == NULL ) {
        return( NULL );
    }
    if( *choice == -1 ) {
        *choice = num_items;
    }
    old_data = (lb_data)list->data_handle;
    if( old_data != NULL ) {
        memcpy( (void *)new_data, old_data, *choice * sizeof( char * ) );
        memcpy( (void *)&new_data[*choice + num_to_add], &old_data[*choice], ( num_items - *choice ) * sizeof( char * ) );
    }
    new_data[num_items + num_to_add] = NULL;
    return( new_data );
}


static bool AddString( lb_data data, const char *text, int choice )
{
    size_t  length;
    char    *str;

    if( text != NULL ) {
        length = strlen( text );
        str = (char *)GUIMemAlloc( length + 1 );
        if( str == NULL ) {
            GUIMemFree( (void *)data );
            return( false );
        }
        strcpy( str, text );
    } else {
        str = (char *)GUIMemAlloc( sizeof( char ) );
        if( str == NULL ) {
            GUIMemFree( (void *)data );
            return( false );
        }
        str[0] = '\0';
    }
    data[choice] = str;
    return( true );
}

bool GUIListBoxAddText( a_list *list, const char *text, int choice )
{
    lb_data     new_data;
    lb_data     old_data;

    old_data = (lb_data)list->data_handle;
    new_data = ResizeList( list, 1, &choice );
    if( new_data == NULL ) {
        return( false );
    }
    if( !AddString( new_data, text, choice ) ) {
        return( false );
    }
    GUIMemFree( (void *)old_data );
    list->data_handle = (const void *)new_data;
    uiupdatelistbox( list );
    return( true );
}

bool GUIListBoxAddTextList( a_list *list, int num_items, const void *data_handle, GUIPICKGETTEXT *getstring )
{
    int         i;
    lb_data     old_data;
    lb_data     new_data;
    int         choice;

    choice = -1;
    old_data = (lb_data)list->data_handle;
    new_data = ResizeList( list, num_items, &choice );
    if( new_data == NULL ) {
        return( false );
    }
    for( i = 0; i < num_items; i++ ) {
        if( !AddString( new_data, getstring( data_handle, i ), choice + i ) ) {
            return( false );
        }
    }
    GUIMemFree( (void *)old_data );
    list->data_handle = (const void *)new_data;
    uiupdatelistbox( list );
    return( true );
}

char *GUIGetListBoxText( a_list *list, int choice, bool get_curr )
{
    lb_data     data;

    data = (lb_data)list->data_handle;
    if( get_curr ) {
        choice = list->choice;
    }
    return( GUIStrDup( data[choice], NULL ) );
}

bool GUIListGetCurr( a_list *list, int *choice )
{
    *choice = list->choice;
    return( true );
}

bool GUIListSetCurr( a_list *list, int choice )
{
    if( GUIListSize( list ) >= choice ) {
        list->choice = choice;
        uiupdatelistbox( list );
        return( true );
    }
    return( false );
}

bool GUIListBoxGetTopIndex( a_list *list, int *choice )
{
    if( list->box == NULL ) {
        return( false );
    }
    *choice = list->box->line;
    return( true );
}

bool GUIListBoxSetTopIndex( a_list *list, int choice )
{
    if( list->box == NULL ) {
        return( false );
    }
    list->box->line = choice;
    uiupdatelistbox( list );
    return( true );
}

/* NYI */

bool GUIAPI GUIDropDown( gui_window *wnd, gui_ctl_id id, bool drop )
{
    /* unused parameters */ (void)wnd; (void)id; (void)drop;

    return( false );
}

bool GUIAPI GUISetListItemData( gui_window *wnd, gui_ctl_id id, int choice, void *data )
{
    /* unused parameters */ (void)wnd; (void)id; (void)choice; (void)data;

    return( false );
}

void * GUIAPI GUIGetListItemData( gui_window *wnd, gui_ctl_id id, int choice )
{
    /* unused parameters */ (void)wnd; (void)id; (void)choice;

    return( NULL );
}
