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


#include <stdio.h>
#include <string.h>
#include "list.h"
#include "mem.h"

#define ALLOC_INCREMENT         10

List *NewList( unsigned size ) {
/*******************************/
    List        *ret;

    ret = MemMalloc( sizeof( List ) );
    memset( ret, 0, sizeof( List ) );
    ret->itemsize = size;
    return( ret );
}

void AddToList( List *list, void *item ) {
/*****************************************/
    if( list->alloced == list->used ) {
        list->alloced += ALLOC_INCREMENT;
        list->data = MemRealloc( list->data, list->alloced * list->itemsize );
    }
    memcpy( list->data + ( list->used * list->itemsize ), item,
            list->itemsize );
    list->used ++;
}

void FreeList( List *list, void (*freeitem)(void *) ) {
/******************************************************/
    unsigned    i;

    if( freeitem != NULL ) {
        for( i=0; i < list->used; i++ ) {
            freeitem( list->data + ( i * list->itemsize ) );
        }
    }
    MemFree( list->data );
    MemFree( list );
}

void *GetListItem( List *list, unsigned index ) {
/************************************************/
    if( index > list->used ) return( NULL );
    return( list->data + ( index * list->itemsize ) );
}

unsigned GetListCount( List *list ) {
/************************************/
    if( list == NULL ) return( 0 );
    return( list->used );
}
