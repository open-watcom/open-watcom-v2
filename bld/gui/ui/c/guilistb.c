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

static bool FillInEmptyList( a_list *list )
{
    list->choice = 0;
    list->data = (char ** )GUIMemAlloc( sizeof( char * ) );
    if( list->data == NULL ) {
        return( FALSE );
    }
    *((char **)list->data) = NULL;
    return( TRUE );
}

bool GUIClearListBox( a_list *list )
{
    bool ret;

    GUIFreeList( list, FALSE );
    ret = FillInEmptyList( list );
    uiupdatelistbox( list );
    return( ret );
}

bool GUIListBoxDeleteItem( a_list *list, int choice )
{
    int         size;
    char        **data;
    int         i;

    size = GUIListSize( list );
    if( choice >= size ) {
        return( FALSE );
    }
    data = (char **)list->data;
    list->data = (char ** )GUIMemAlloc( sizeof( char * ) * size );
    if( list->data == NULL ) {
        list->data = data;
        return( FALSE );
    }
    for( i = 0; i < choice; i++ ) {
        ((char**)list->data)[i] = data[i];
    }
    GUIMemFree( data[choice] );
    for( i = choice; i < size; i++ ) {
        ((char **)list->data)[i] = data[i+1];
    }
    GUIMemFree( data );
    if( choice >= GUIListSize( list )  ) {
        list->choice = 0;
    }
    uiupdatelistbox( list );
    return( TRUE );
}

void GUIFreeList( a_list *list, bool free_list )
{
    int     num;
    int     i;
    char ** data;

    if( list->get == NULL ) {
        list->get = ( bool (*) ( void *, unsigned, char *, unsigned ) )
                        uigetlistelement;
    }
    num = GUIListSize( list );
    data = (char **)list->data;
    for( i = 0; i < num; i++ ) {
        GUIMemFree( data[i] );
    }
    GUIMemFree( data );
    list->choice = 0;
    list->data = NULL;
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

    list = (a_list * )GUIMemAlloc( sizeof( a_list ) );
    if( list == NULL ) {
        return( NULL );
    }
    if( !GUIFillInListBox( list ) ) {
        GUIMemFree( list );
        list = NULL;
    }
    return( list );
}

a_list *GUICreateEditMLE( char * text )
{
    a_list      *list;
    char        *text_copy;
    char        *line;
    char        *end;
    char        *absolute_end;
    #define     MLE_NEWLINE     "\r\n"

    list = (a_list * )GUIMemAlloc( sizeof( a_list ) );
    if( list == NULL ) {
        return( NULL );
    }
    if( !GUIFillInListBox( list ) ) {
        GUIMemFree( list );
        list = NULL;
    } else {
        GUIStrDup( text, &text_copy );
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

static char **ResizeList( a_list *list, unsigned num_to_add, int *choice )
{
    char        **data;
    int         num;
    char        **old_data;

    if( list->get == NULL ) {
        list->get = ( bool (*) ( void *, unsigned, char *, unsigned ) )
                        uigetlistelement;
    }
    num = GUIListSize( list );
    data = (char **)GUIMemAlloc( ( num + num_to_add + 1 ) * sizeof( char * ) );
    if( data == NULL ) {
        return( NULL );
    }
    old_data = (char **)list->data;
    if( *choice == -1 ) {
        *choice = num;
    }
    if( list->data != NULL ) {
        memcpy( data, old_data, *choice * sizeof( char *) );
        memcpy( &data[*choice+num_to_add], &old_data[*choice], ( num - *choice ) *
                sizeof( char *) );
    }
    data[num+num_to_add] = NULL;
    return( data );
}


static bool AddString( char **data, char *text, int choice )
{
    int length;

    if( text != NULL ) {
        length = strlen( text );
        data[choice] = (char *)GUIMemAlloc( length + 1 );
        if( data[choice] == NULL ) {
            GUIMemFree( data );
            return( FALSE );
        }
        strcpy( data[choice], text );
    } else {
        data[choice] = (char *)GUIMemAlloc( sizeof( char ) );
        if( data[choice] == NULL ) {
            GUIMemFree( data );
            return( FALSE );
        }
        data[choice][0] = NULLCHAR;
    }
    return( TRUE );
}

bool GUIListBoxAddText( a_list *list, char *text, int choice )
{
    char        **data;
    char        **old_data;

    old_data = (char **)list->data;
    data = ResizeList( list, 1, &choice );
    if( data == NULL ) {
        return( FALSE );
    }
    if( !AddString( data, text, choice ) ) {
        return( FALSE );
    }
    GUIMemFree( old_data );
    list->data = (void *)data;
    uiupdatelistbox( list );
    return( TRUE );
}

bool GUIListBoxAddTextList( a_list *list, unsigned items, void *handle,
                            char *(*getstring)(void*,unsigned) )
{
    int         i;
    char        *curr_str;
    char        **old_data;
    char        **data;
    int         choice;

    old_data = (char **)list->data;
    choice = -1;
    data = ResizeList( list, items, &choice );
    if( data == NULL ) {
        return( FALSE );
    }
    for( i = 0; i < items; ++i ) {
        curr_str = getstring( handle, i );
        if( !AddString( data, curr_str, choice + i ) ) {
            return( FALSE );
        }
    }
    GUIMemFree( old_data );
    list->data = (void *)data;
    uiupdatelistbox( list );
    return( TRUE );
}

char *GUIGetListBoxText( a_list *list, int choice, bool get_curr )
{
    char        *text;
    char        **data;

    data = (char **)list->data;
    if( get_curr ) {
        choice = list->choice;
    }
    GUIStrDup( data[choice], &text );
    return( text );
}

bool GUIListCurr( a_list *list, int choice, bool set, int *ret )
{
    if( set ) {
        /* set */
        if( list->get == NULL ) {
            list->get = ( bool (*) ( void *, unsigned, char *, unsigned ) )
                            uigetlistelement;
        }
        if( GUIListSize( list ) >= choice ) {
            list->choice = choice;
            uiupdatelistbox( list );
            return( TRUE );
        } else {
            return( FALSE );
        }
    } else {
        /* get */
        *ret = list->choice;
        return( TRUE );
    }
}

bool GUIListBoxTopIndex( a_list *list, int choice, bool set, int *ret )
{
    if( list->box == NULL ) {
        return( FALSE );
    }
    if( set ) {
        list->box->line = choice;
        uiupdatelistbox( list );
    } else {
        *ret = list->box->line;
    }
    return( TRUE );
}

/* NYI */

bool GUIDropDown( gui_window *wnd, unsigned id, bool drop )
{
    wnd = wnd;
    id = id;
    drop = drop;
    return( FALSE );
}

bool GUISetListItemData( gui_window *wnd, unsigned id, unsigned choice,
                         void *data )
{
    wnd = wnd;
    id = id;
    choice = choice;
    data = data;
    return( FALSE );
}

void *GUIGetListItemData( gui_window *wnd, unsigned id, unsigned choice )
{
    wnd = wnd;
    id = id;
    choice = choice;
    return( NULL );
}
