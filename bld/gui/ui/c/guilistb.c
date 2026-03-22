/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2026 The Open Watcom Contributors. All Rights Reserved.
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


static bool FillInEmptyList( a_list *list )
{
    list->choice = 0;
    list->data = MemAlloc( sizeof( *list->data ) );
    if( list->data == NULL ) {
        return( false );
    }
    (list->data)[0] = NULL;
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
    const char  **old_data;
    const char  **new_data;
    int         i;

    num_items = GUIListSize( list );
    if( choice >= num_items ) {
        return( false );
    }
    new_data = MemAlloc( sizeof( *new_data ) * num_items );
    if( new_data == NULL ) {
        return( false );
    }
    old_data = list->data;
    for( i = 0; i < choice; i++ ) {
        new_data[i] = old_data[i];
    }
    MemFree( (void *)old_data[choice] );
    for( i = choice; i < num_items; i++ ) {
        new_data[i] = old_data[i + 1];
    }
    MemFree( (void *)old_data );
    list->data = new_data;
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
    const char  **data;

    num_items = GUIListSize( list );
    data = list->data;
    for( i = 0; i < num_items; i++ ) {
        MemFree( (void *)data[i] );
    }
    MemFree( (void *)data );
    list->choice = 0;
    list->data = NULL;
    if( free_list ) {
        MemFree( list );
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

    list = MemAlloc( sizeof( *list ) );
    if( list == NULL ) {
        return( NULL );
    }
    if( !GUIFillInListBox( list ) ) {
        MemFree( (void *)list );
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

    list = MemAlloc( sizeof( *list ) );
    if( list == NULL ) {
        return( NULL );
    }
    if( !GUIFillInListBox( list ) ) {
        MemFree( (void *)list );
        list = NULL;
    } else {
        text_copy = MemStrdup( text );
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
        MemFree( text_copy );
    }
    return( list );
}

int GUIListSize( a_list *list )
{
    return( uilistsize( list ) );
}

static const char **ResizeList( a_list *list, unsigned num_to_add, int *choice )
{
    int         num_items;
    const char  **old_data;
    const char  **new_data;

    num_items = GUIListSize( list );
    new_data = MemAlloc( sizeof( *new_data ) * ( num_items + num_to_add + 1 ) );
    if( new_data == NULL ) {
        return( NULL );
    }
    if( *choice == -1 ) {
        *choice = num_items;
    }
    old_data = list->data;
    if( old_data != NULL ) {
        memcpy( (void *)new_data, old_data, *choice * sizeof( char * ) );
        memcpy( (void *)&new_data[*choice + num_to_add], &old_data[*choice], ( num_items - *choice ) * sizeof( char * ) );
    }
    new_data[num_items + num_to_add] = NULL;
    return( new_data );
}

bool GUIListBoxAddText( a_list *list, const char *text, int choice )
{
    const char  **new_data;
    const char  **old_data;

    old_data = list->data;
    new_data = ResizeList( list, 1, &choice );
    if( new_data == NULL ) {
        return( false );
    }
    if( text == NULL )
        text = "";
    new_data[choice] = MemStrdup( text );
    if( new_data[choice] == NULL ) {
        MemFree( (void *)new_data );
        return( false );
    }
    MemFree( (void *)old_data );
    list->data = new_data;
    uiupdatelistbox( list );
    return( true );
}

bool GUIListBoxAddTextList( a_list *list, int num_items, const char **data, GUIPICKGETTEXT *getstring )
{
    int         i;
    const char  **old_data;
    const char  **new_data;
    int         choice;
    const char  *text;

    choice = -1;
    old_data = list->data;
    new_data = ResizeList( list, num_items, &choice );
    if( new_data == NULL ) {
        return( false );
    }
    for( i = 0; i < num_items; i++ ) {
        text = getstring( data, i );
        if( text == NULL )
            text = "";
        new_data[choice + i] = MemStrdup( text );
        if( new_data[choice + i] == NULL ) {
            MemFree( (void *)new_data );
            return( false );
        }
    }
    MemFree( (void *)old_data );
    list->data = new_data;
    uiupdatelistbox( list );
    return( true );
}

char *GUIGetListBoxText( a_list *list, int choice, bool get_curr )
{
    const char  **data;

    data = list->data;
    if( get_curr ) {
        choice = list->choice;
    }
    return( MemStrdup( data[choice] ) );
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
