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


/* DLIST.C - routines to handle the operations on a DLIST */

#include <windows.h>

#include "fmedit.def"
#include "memory.def"

#include "dlist.def"
#include "global.h"

extern void DListAddElt( DLIST ** head, DLIST_ELT obj )
/******************************************************/

/* add a new element to the dlist */

  {
    DLIST * new;

    new = EdAlloc( sizeof( DLIST ) );
    new->elt = obj;
    new->next = *head;
    new->prev = NULL;
    if( *head != NULL ) {
        (*head)->prev = new;
    }
    *head = new;
  }


extern void DListFree( DLIST * lst )
/**********************************/

/* free the passed dlist */

  {
    DLIST * next;

    while( lst != NULL ) {
        next = lst->next;
        EdFree( lst );
        lst = next;
    }
  }


extern DLIST_ELT DListElement( DLIST * lst )
/******************************************/

/* return the first elt of the dlist */

  {
    return( lst->elt );
  }


extern DLIST * DListNext( DLIST * curr )
/**************************************/

/* return the next element of the dlist */

  {
    return( curr->next );
  }


extern DLIST * DListPrev( DLIST * curr )
/**************************************/

/* return the next element of the dlist */

  {
    return( curr->prev );
  }


extern void DListRemoveElt( DLIST ** lst, DLIST_ELT obj )
/*******************************************************/

/* delete the object from the dlist */

  {
    DLIST * node;

    for( node = *lst; node != NULL; node = node->next ) {
        if( node->elt.original == obj.original ) {
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

extern DLIST * DListConsume( DLIST * curr )
/*****************************************/

/* free the current element and return the next element */

  {
    DLIST * next;

    next = curr->next;
    EdFree( curr );
    return( next );
  }


extern DLIST * DListFindElt( DLIST * l, DLIST_ELT  elt )
/******************************************************/

/* find the given element in the dlist */

  {
    while( l != NULL ) {
        if( elt.original != NULL ) {
            if( l->elt.original == elt.original ) {
                break;
            }
        } else {
            if( l->elt.copy == elt.copy ) {
                break;
            }
        }
        l = l->next;
    }
    return( l );
  }


extern DLIST * DListCopy( DLIST * l )
/***********************************/

/* make a copy of the passed dlist */

  {
    DLIST * head;

    head = NULL;
    for( ; l != NULL; l = l->next ) {
        DListAddElt( &head, l->elt );
    }
    return( head );
  }

extern void DListInsertElt( DLIST * prev, DLIST_ELT obj )
/*******************************************************/

/* add a new element to the list */

  {
    DLIST * new;

    new = EdAlloc( sizeof( DLIST ) );
    new->elt = obj;
    new->next = prev->next;
    new->prev = prev;
    prev->next = new;
    if( new->next != NULL ) {
        new->next->prev = new;
    }
  }
