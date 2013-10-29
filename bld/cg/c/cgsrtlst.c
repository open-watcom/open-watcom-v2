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


#include "cgstd.h"
#include "cgmem.h"
#include "memout.h"

extern  mem_out_action          SetMemOut(mem_out_action);

#define _NEXT( list, offset ) (*((void **)((char *)list + offset)))

static  void            ShellSort( void **array, unsigned length,
                                   bool (*before)(void*,void*) );

static  void            *BuildList( void **array,
                                    unsigned next_offset, unsigned length ) {
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


static  void            *MergeList( void *list1, void *list2,
                                   unsigned next_offset,
                                   bool (*before)(void*,void*) ) {
/***************************************************************/

    void        *list;
    void        *next;
    void        **owner;

    list = NULL;
    owner = &list;
    while( list1 != NULL && list2 != NULL ) {
        if( (*before)( list1, list2 ) ) {
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


static  void            *DoSortList( void *list, unsigned next_offset,
                                  bool (*before)(void*,void*),
                                  unsigned length ) {
/**************************************************************************/

    void        **array;
    void        **parray;
    void        *list2;
    unsigned    mid;
    int         i;

    if( list == NULL ) return( NULL );
    array = CGAlloc( length * sizeof( void * ) );
    if( array == NULL ) {
        mid = length / 2;
        if( mid == 0 ) return( list ); /* FATAL ERROR! */
        list2 = list;
        for( i = mid; i-- > 0; ) {
            list2 = _NEXT( list2, next_offset );
        }
        list = DoSortList( list, next_offset, before, mid );
        list2 = DoSortList( list2, next_offset, before, length - mid );
        list = MergeList( list, list2, next_offset, before );
    } else {
        list2 = list;
        parray = array;
        for( i = length; i-- > 0; ) {
            *parray = list2;
            ++parray;
            list2 = _NEXT( list2, next_offset );
        }
        ShellSort( array, length, before );
        list = BuildList( array, next_offset, length );
        CGFree( array );
    }
    return( list );
}

static  void            ShellSort( void **array, unsigned length,
                                   bool (*before)(void*,void*) ) {
/****************************************************************/


    unsigned    i;
    bool        swap;
    void        *t;
    unsigned    gap;
    unsigned    adjust;

    gap = length;
    adjust = 1;
    do {
        adjust = !adjust;
        gap = gap / 2 + adjust;
        do {
            swap = FALSE;
            for( i=0; i<(length-gap); ++i ) {
                if( (*before)( array[i+gap], array[i] ) ) {
                    t = array[i];
                    array[i] = array[i+gap];
                    array[i+gap] = t;
                    swap = TRUE;
                }
            }
        } while( swap );
    } while( gap != 1 );
}


extern  void            *SortList( void *list, unsigned next_offset,
                                  bool (*before)(void*,void*) ) {
/****************************************************************/

    void                *list2;
    unsigned            length;
    mem_out_action      old_memout;

    list2 = list;
    length = 0;
    while( list2 != NULL ) {
        ++length;
        list2 = _NEXT( list2, next_offset );
    }
    if( length > 1 ) {
        old_memout = SetMemOut( MO_OK );
        list = DoSortList( list, next_offset, before, length );
        SetMemOut( old_memout );
    }
    return( list );
}
