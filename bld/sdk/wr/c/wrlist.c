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


#include <windows.h>
#include <stdlib.h>
#include <string.h>
#include "watcom.h"
#include "wrglbl.h"
#include "wrlist.h"

void WRInsertObject( LIST **list, void *obj )
{
    LIST *end;

    if( list != NULL ) {
        if( *list == NULL ) {
            ListAddElt( list, obj );
        } else {
            WRListLastElt( *list, &end );
            ListInsertElt( end, obj );
        }
    }
}

void WRListLastElt( LIST *list, LIST **last )
{
    LIST *end;

    if( last != NULL ) {
        for( end = list; end != NULL && ListNext( end ) != NULL; end = ListNext( end ) );
        *last = end;
    }
}

int WRListConcat( LIST **dest, LIST *src, uint_32 size )
{
    LIST    *end;
    LIST    *olist;
    void    *elt;

    if( dest == NULL ) {
        return( TRUE );
    }

    WRListLastElt( *dest, &end );

    for( olist = src; olist != NULL; olist = ListNext( olist ) ) {
        if( size == 0 ) {
            elt = ListElement( olist );
        } else {
            elt = WRMemAlloc( size );
            if( elt == NULL ) {
                return( FALSE );
            }
            memcpy( elt, ListElement( olist ), size );
        }
        if( end == NULL ) {
            ListAddElt( dest, elt );
            end = *dest;
        } else {
            ListInsertElt( end, elt );
            end = ListNext( end );
        }
    }

    return( TRUE );
}

LIST *WRListCopy( LIST *src )
{
    LIST    *new;
    LIST    *end;
    LIST    *olist;

    end = NULL;
    new = NULL;

    for( olist = src; olist != NULL; olist = ListNext( olist ) ) {
        if( end == NULL ) {
            ListAddElt( &new, ListElement( olist ) );
            end = new;
        } else {
            ListInsertElt( end, ListElement( olist ) );
            end = ListNext( end );
        }
    }

    return( new );
}

void ListAddElt( LIST **head, void *obj )
{
    LIST *new;

    new = WRMemAlloc( sizeof( LIST ) );
    new->elt = obj;
    new->next = *head;
    new->prev = NULL;
    if( *head != NULL ) {
        (*head)->prev = new;
    }
    *head = new;
}

void ListFree( LIST *lst )
{
    LIST *next;

    while( lst != NULL ) {
        next = lst->next;
        WRMemFree( lst );
        lst = next;
    }
}

void *ListElement( LIST *lst )
{
    return( lst->elt );
}

LIST *ListNext( LIST *curr )
{
    return( curr->next );
}

LIST *ListPrev( LIST *curr )
{
    return( curr->prev );
}

void ListRemoveElt( LIST **lst, void *obj )
{
    LIST *node;

    for( node = *lst; node != NULL; node = node->next ) {
        if( node->elt == obj ) {
            if( node->next != NULL ) {
                node->next->prev = node->prev;
            }
            if( node->prev != NULL ) {
                node->prev->next = node->next;
            } else {
                *lst = node->next;
            }
            WRMemFree( node );
            break;
        }
    }
}

LIST *ListConsume( LIST *curr )
{
    LIST *next;

    next = curr->next;
    WRMemFree( curr );
    return( next );
}

LIST *ListFindElt( LIST *l, void *elt )
{
    while( l != NULL ) {
        if( l->elt == elt ) {
            break;
        }
        l = l->next;
    }
    return( l );
}

LIST *ListCopy( LIST * l )
{
    LIST *head;

    head = NULL;
    for( ; l != NULL; l = l->next ) {
        ListAddElt( &head, l->elt );
    }
    return( head );
}

int ListCount( LIST *l )
{
    int count;

    count = 0;
    for( ; l != NULL; l = l->next ) {
        count++;
    }
    return( count );
}

void ListInsertElt( LIST *prev, void *obj )
{
    LIST *new;

    new = WRMemAlloc( sizeof( LIST ) );
    new->elt = obj;
    new->next = prev->next;
    new->prev = prev;
    prev->next = new;
    if( new->next != NULL ) {
        new->next->prev = new;
    }
}

void ListMerge( LIST **destlist, LIST *src )
{
    LIST    *curr;
    LIST    *last;

    last = NULL;
    if( *destlist == NULL ) {
        *destlist = src;
        return;
    }
    for( curr = *destlist; curr->next != NULL; curr = curr->next );
    curr->next = src;
    src->prev = curr;
}
