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
    *((lb_data)list->data_handle) = NULL;
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
    int         size;
    lb_data     old_data;
    lb_data     new_data;
    int         i;

    size = GUIListSize( list );
    if( choice >= size ) {
        return( false );
    }
    new_data = (lb_data)GUIMemAlloc( sizeof( char * ) * size );
    if( new_data == NULL ) {
        return( false );
    }
    old_data = (lb_data)list->data_handle;
    for( i = 0; i < choice; i++ ) {
        new_data[i] = old_data[i];
    }
    GUIMemFree( (void *)old_data[choice] );
    for( i = choice; i < size; i++ ) {
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
    int         num;
    int         i;
    lb_data     data;

    if( list->get == NULL ) {
        list->get = uigetlistelement;
    }
    num = GUIListSize( list );
    data = (lb_data)list->data_handle;
    for( i = 0; i < num; i++ ) {
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
    int         num;
    lb_data     old_data;
    lb_data     new_data;

    if( list->get == NULL ) {
        list->get = uigetlistelement;
    }
    num = GUIListSize( list );
    new_data = (lb_data)GUIMemAlloc( ( num + num_to_add + 1 ) * sizeof( char * ) );
    if( new_data == NULL ) {
        return( NULL );
    }
    if( *choice == -1 ) {
        *choice = num;
    }
    old_data = (lb_data)list->data_handle;
    if( old_data != NULL ) {
        memcpy( (void *)new_data, old_data, *choice * sizeof( char * ) );
        memcpy( (void *)&new_data[*choice + num_to_add], &old_data[*choice], ( num - *choice ) * sizeof( char * ) );
    }
    new_data[num + num_to_add] = NULL;
    return( new_data );
}


static bool AddString( lb_data data, const char *text, int choice )
{
    int length;
    char *str;

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

bool GUIListBoxAddTextList( a_list *list, int items, const void *data_handle, PICKGETTEXT *getstring )
{
    int         item;
    lb_data     old_data;
    lb_data     new_data;
    int         choice;

    choice = -1;
    old_data = (lb_data)list->data_handle;
    new_data = ResizeList( list, items, &choice );
    if( new_data == NULL ) {
        return( false );
    }
    for( item = 0; item < items; ++item ) {
        if( !AddString( new_data, getstring( data_handle, item ), choice + item ) ) {
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

bool GUIListCurr( a_list *list, int choice, bool set, int *ret )
{
    if( set ) {
        /* set */
        if( list->get == NULL ) {
            list->get = uigetlistelement;
        }
        if( GUIListSize( list ) >= choice ) {
            list->choice = choice;
            uiupdatelistbox( list );
            return( true );
        } else {
            return( false );
        }
    } else {
        /* get */
        *ret = list->choice;
        return( true );
    }
}

bool GUIListBoxTopIndex( a_list *list, int choice, bool set, int *ret )
{
    if( list->box == NULL ) {
        return( false );
    }
    if( set ) {
        list->box->line = choice;
        uiupdatelistbox( list );
    } else {
        *ret = list->box->line;
    }
    return( true );
}

/* NYI */

bool GUIDropDown( gui_window *wnd, gui_ctl_id id, bool drop )
{
    wnd = wnd;
    id = id;
    drop = drop;
    return( false );
}

bool GUISetListItemData( gui_window *wnd, gui_ctl_id id, int choice, void *data )
{
    wnd = wnd;
    id = id;
    choice = choice;
    data = data;
    return( false );
}

void *GUIGetListItemData( gui_window *wnd, gui_ctl_id id, int choice )
{
    wnd = wnd;
    id = id;
    choice = choice;
    return( NULL );
}
