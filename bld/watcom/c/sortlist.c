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


#include <stdlib.h>
#include <limits.h>
#include "sortlist.h"

#define _NEXT( list, offset ) (*((void **)((char *)list + offset)))

static  void            *MergeList( void *list1, void *list2,
                                   unsigned next_offset,
                                   int (*compare)(void*,void*) ) {
/***************************************************************/

    void        *list;
    void        *next;
    void        **owner;

    list = NULL;
    owner = &list;
    while( list1 != NULL && list2 != NULL ) {
        if( (*compare)( &list1, &list2 ) < 0 ) {
            *owner = list1;
            owner = &_NEXT( list1, next_offset );
            next = _NEXT( list1, next_offset );
            _NEXT( list1, next_offset ) = NULL;
            list1 = next;
        } else {
            *owner = list2;
            owner = &_NEXT( list2, next_offset );
            next = _NEXT( list2, next_offset );
            _NEXT( list2, next_offset ) = NULL;
            list2 = next;
        }
    }
    if( list1 != NULL ) {
        *owner = list1;
    } else if ( list2 != NULL ) {
        *owner = list2;
    }
    return( list );
}


static  void            *BuildList( void **array,
                                    unsigned next_offset, unsigned long length ) {
/***************************************************************************/

    void        *list;
    void        **owner;

    list = NULL;
    owner = &list;
    while( length != 0 ) {
        *owner = *array;
        _NEXT( *owner, next_offset ) = NULL;
        owner = &_NEXT( *owner, next_offset );
        ++array;
        --length;
    }
    return( list );
}


static  void            *DoSortList( void *list, unsigned next_offset,
                                  int (*compare)(void*,void*),
                                  unsigned long length,
                                  void *(*allocrtn)(size_t),
                                  void (*freertn)(void*) ) {
/**************************************************************************/

    void        **array;
    void        **parray;
    void        *list2;
    unsigned long       mid;
    long        i;

    if( list == NULL ) return( NULL );
    if( length * sizeof( void * ) > INT_MAX ) {
        array = NULL;
    } else {
        array = allocrtn( length * sizeof( void * ) );
    }
    if( array == NULL ) {
        mid = length / 2;
        if( mid == 0 ) return( list ); /* FATAL ERROR! */
        i = mid;
        list2 = list;
        while( --i >= 0 ) {
            list2 = _NEXT( list2, next_offset );
        }
        list = DoSortList( list, next_offset, compare, mid, allocrtn, freertn );
        list2 = DoSortList( list2, next_offset, compare, length - mid, allocrtn, freertn );
        list = MergeList( list, list2, next_offset, compare );
    } else {
        list2 = list;
        parray = array;
        i = length;
        while( --i >= 0 ) {
            *parray = list2;
            ++parray;
            list2 = _NEXT( list2, next_offset );
        }
        qsort( array, length, sizeof( void * ), (int (*)(void const*,void const*))compare );
        list = BuildList( array, next_offset, length );
        freertn( array );
    }
    return( list );
}

extern  void            *SortLinkedList( void *list, unsigned next_offset,
                                   int (*compare)(void*,void*),
                                   void *(*allocrtn)(size_t),
                                   void (*freertn)(void*) ) {
/****************************************************************/

    void                *list2;
    unsigned long       length;

    list2 = list;
    length = 0;
    while( list2 != NULL ) {
        ++length;
        list2 = _NEXT( list2, next_offset );
    }
    list = DoSortList( list, next_offset, compare, length, allocrtn, freertn );
    return( list );
}
