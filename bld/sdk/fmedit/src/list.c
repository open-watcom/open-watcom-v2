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
* Description:  Routines to handle the operations on a LIST.
*
****************************************************************************/


#include <stdlib.h>
#include <windows.h>
#include "fmedit.def"
#include "memory.def"
#include "object.def"

#include "list.h"

void FMEDITAPI ListAddElt( LIST **head, OBJPTR obj )
/**************************************************/
{
    /* add a new element to the list */
    LIST    *new;

    new = EdAlloc( sizeof( LIST ) );
    new->elt = obj;
    new->next = *head;
    new->prev = NULL;
    if( *head != NULL ) {
        (*head)->prev = new;
    }
    *head = new;
}


void FMEDITAPI ListFree( LIST *lst )
/**********************************/
{
    /* free the passed list */
    LIST    *next;

    while( lst != NULL ) {
        next = lst->next;
        EdFree( lst );
        lst = next;
    }
}


OBJPTR FMEDITAPI ListElement( LIST *lst )
/***************************************/
{
    /* return the first elt of the list */
    return( lst->elt );
}


LIST *FMEDITAPI ListNext( LIST *curr )
/************************************/
{
    /* return the next element of the list */
    return( curr->next );
}


LIST *FMEDITAPI ListPrev( LIST *curr )
/************************************/
{
    /* return the next element of the list */
    return( curr->prev );
}


void FMEDITAPI ListRemoveElt( LIST **lst, OBJPTR obj )
/****************************************************/
{
    /* delete the object from the list */
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
            EdFree( node );
            break;
        }
    }
}

LIST *FMEDITAPI ListConsume( LIST *curr )
/***************************************/
{
    /* free the current element and return the next element */
    LIST *next;

    next = curr->next;
    EdFree( curr );
    return( next );
}


LIST *FMEDITAPI ListFindElt( LIST *l, OBJPTR elt )
/************************************************/
{
    /* find the given element in the list */
    while( l != NULL ) {
        if( l->elt == elt ) {
            break;
        }
        l = l->next;
    }
    return( l );
}


LIST *FMEDITAPI ListCopy( LIST *l )
/*********************************/
{
    /* make a copy of the passed list */
    LIST *head;

    head = NULL;
    for( ; l != NULL; l = l->next ) {
        ListAddElt( &head, l->elt );
    }
    return( head );
}

int FMEDITAPI ListCount( LIST *l )
/********************************/
{
    /* count the number of elements in the list */
    int count;

    count = 0;
    for( ; l != NULL; l = l->next ) {
        ++count;
    }
    return( count );
}

void FMEDITAPI ListInsertElt( LIST *prev, OBJPTR obj )
/****************************************************/
{
    /* add a new element to the list */
    LIST *new;

    new = EdAlloc( sizeof( LIST ) );
    new->elt = obj;
    new->next = prev->next;
    new->prev = prev;
    prev->next = new;
    if( new->next != NULL ) {
        new->next->prev = new;
    }
}


void FMEDITAPI ListMerge( LIST **destlist, LIST *src )
/**************¨*************************************/
{
    LIST *curr;
    LIST *last;

    last = NULL;
    if( *destlist == NULL ) {
        *destlist = src;
        return;
    }
    for( curr = *destlist; curr->next != NULL; curr = curr->next );
    curr->next = src;
    if( src != NULL ) {
        src->prev = curr;
    }
}
