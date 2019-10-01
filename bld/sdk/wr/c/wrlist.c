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


#include "wrglbl.h"
#include "wrlist.h"

void WRInsertObject( LIST **list, HWND hdlg )
{
    LIST *end;

    if( list != NULL ) {
        if( *list == NULL ) {
            ListAddElt( list, hdlg );
        } else {
            ListLastElt( *list, &end );
            ListInsertElt( end, hdlg );
        }
    }
}

void ListLastElt( LIST *list, LIST **last )
{
    LIST *end;

    if( last != NULL ) {
        for( end = list; end != NULL && ListNext( end ) != NULL; end = ListNext( end ) );
        *last = end;
    }
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

void ListAddElt( LIST **head, HWND hdlg )
{
    LIST *new;

    new = MemAlloc( sizeof( LIST ) );
    new->hdlg = hdlg;
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
        MemFree( lst );
        lst = next;
    }
}

HWND ListElement( LIST *lst )
{
    return( lst->hdlg );
}

LIST *ListNext( LIST *curr )
{
    return( curr->next );
}

LIST *ListPrev( LIST *curr )
{
    return( curr->prev );
}

void WRRemoveObject( LIST **lst, HWND hdlg )
{
    LIST *node;

    for( node = *lst; node != NULL; node = node->next ) {
        if( node->hdlg == hdlg ) {
            if( node->next != NULL ) {
                node->next->prev = node->prev;
            }
            if( node->prev != NULL ) {
                node->prev->next = node->next;
            } else {
                *lst = node->next;
            }
            MemFree( node );
            break;
        }
    }
}

LIST *ListConsume( LIST *curr )
{
    LIST *next;

    next = curr->next;
    MemFree( curr );
    return( next );
}

LIST *ListFindElt( LIST *l, HWND hdlg )
{
    while( l != NULL ) {
        if( l->hdlg == hdlg ) {
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
        ListAddElt( &head, l->hdlg );
    }
    return( head );
}

int ListCount( LIST *list )
{
    int count;

    count = 0;
    for( ; list != NULL; list = list->next ) {
        count++;
    }
    return( count );
}

void ListInsertElt( LIST *prev, HWND hdlg )
{
    LIST *new;

    new = MemAlloc( sizeof( LIST ) );
    new->hdlg = hdlg;
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
