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


#include <stddef.h>
#include "mapper.h"

typedef struct free_element {
    mapper_id   next;
    mapper_id   magic;
} free_element;

typedef union {
    free_element        free;
    unsigned long       v;
    void                *p;
} map_entry;

struct mapper_handle {
    mapper_id           size;
    mapper_id           max_num;
    mapper_id           base;
    mapper_id           entries;
    mapper_id           free_list;
    mapper_alloc_rtn    *alloc;
    mapper_free_rtn     *free;
    map_entry           *table[1];      /* variable sized */
};

#define         MAX_MAX         0x3f00
#define         FREE_MAGIC      0xffff
#ifdef __cplusplus
    #define     TABLE_ENTRIES   (mapper_id)128U
#else
    #define     TABLE_ENTRIES   128U
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*
    MapperCreate - create a mapping table, for up to 'max_num' elements,
        each of size 'size'. The lowest map index number used will be
        'base'. Return a pointer to the mapper_handle, or NULL if it can't
        be created. Some memory can be saved if smaller values for 'max_num'
        are used, but it's not too bad to specify a big number here, since
        the code allocates most of the memory incrementally.
*/

mapper_handle   *MapperCreate( mapper_id base, mapper_id max_num,
                               mapper_id size,
                               mapper_alloc_rtn *alloc, mapper_free_rtn *free )
{
    mapper_handle       *mh;
    mapper_id   entries;
    mapper_id   i;

    if( max_num > MAX_MAX ) max_num = MAX_MAX;
    entries = (max_num / TABLE_ENTRIES) + 1;
    mh = (mapper_handle*)alloc(
                sizeof( *mh ) + (entries-1) * sizeof( map_entry * ) );
    if( mh != NULL ) {
        mh->size = size;
        mh->base = base;
        mh->max_num = max_num;
        mh->entries = entries;
        mh->free_list = MAPPER_NOMAP;
        mh->alloc = alloc;
        mh->free = free;
        for( i = 0; i < entries; ++i ) {
            mh->table[i] = NULL;
        }
    }
    return( mh );
}

/*
    MapperDestroy - free all the memory associated with the 'mh' mapping
        table.
*/
void            MapperDestroy( mapper_handle *mh )
{
    mapper_free_rtn*    free_rtn;
    mapper_id           free;
    mapper_id           i;
    mapper_id           j;

    free_rtn = mh->free;
    free = mh->free_list;
    for( i = 0; i < mh->entries; ++i ) {
        if( mh->table[i] != NULL ) {
            for( j = 0; j < TABLE_ENTRIES; ++j ) {
                if( free == (i * TABLE_ENTRIES) + j ) {
                    free = mh->table[i][j].free.next;
                } else if( mh->size > sizeof( map_entry ) ) {
                    free_rtn( mh->table[i][j].p );
                }
            }
            free_rtn( mh->table[i] );
        }
    }
    free_rtn( mh );
}

/*
    MapperAlloc - Allocate a new map table entry from 'mh'. The map id will
        be returned, or MAPPER_NOMAP if the entry couldn't be allocated.
*/
mapper_id       MapperAlloc( mapper_handle *mh )
{
    mapper_id  map;
    mapper_id  i;
    mapper_id  j;
    mapper_id  map_base;
    map_entry       *entry;
    void            *map_data = NULL;

    if( mh->free_list == MAPPER_NOMAP ) {
        i = 0;
        for( ;; ) {
            if( i >= mh->entries ) return( MAPPER_NOMAP );
            if( mh->table[i] == NULL ) break;
            ++i;
        }
        mh->table[i] = (map_entry*)mh->alloc(
                                    TABLE_ENTRIES * sizeof( map_entry ) );
        if( mh->table[i] == NULL ) return( MAPPER_NOMAP );
        map_base = i * TABLE_ENTRIES;
        for( j = 0; j < TABLE_ENTRIES-1; ++j ) {
            mh->table[i][j].free.next = map_base + j + 1;
            mh->table[i][j].free.magic = FREE_MAGIC;
        }
        mh->table[i][j].free.next = MAPPER_NOMAP;
        mh->table[i][j].free.magic = FREE_MAGIC;
        mh->free_list = map_base;
    }
    map = mh->free_list;
    if( map > mh->max_num ) return( MAPPER_NOMAP );
    if( mh->size > sizeof( map_entry ) ) {
        map_data = mh->alloc( mh->size );
        if( map_data == NULL ) return( MAPPER_NOMAP );
    }
    entry = &mh->table[map / TABLE_ENTRIES] [map % TABLE_ENTRIES];
    mh->free_list = entry->free.next;
    if( mh->size > sizeof( map_entry ) ) {
        mh->table[map / TABLE_ENTRIES][map % TABLE_ENTRIES].p = map_data;
    }
    return( map + mh->base );
}

/*
    MapperFree - release the previously allocated mapping for 'map_id'.
*/
void            MapperFree( mapper_handle *mh, mapper_id map_id )
{
    mapper_id  free;
    mapper_id  *free_owner;
    map_entry       *entry;

    map_id -= mh->base;
    if( map_id <= mh->max_num ) {
        free_owner = &mh->free_list;
        for( ;; ) {
            free = *free_owner;
            if( free > map_id ) break;
            entry = &mh->table[free / TABLE_ENTRIES][free % TABLE_ENTRIES];
            free_owner = &entry->free.next;
        }
        entry = &mh->table[map_id / TABLE_ENTRIES][map_id % TABLE_ENTRIES];
        if( mh->size > sizeof( map_entry ) ) {
            mh->free( entry->p );
        }
        entry->free.next = free;
        entry->free.magic = FREE_MAGIC;
        *free_owner = map_id;
    }
}

/*
    MapperGet - given a 'map_id', convert it to a pointer to the storage.
        This is a very fast operation.
*/
void            *MapperGet( mapper_handle *mh, mapper_id map_id )
{
    map_entry   *entry;

    map_id -= mh->base;
    if( map_id > mh->max_num ) return( NULL );
    entry = &mh->table[map_id / TABLE_ENTRIES][map_id % TABLE_ENTRIES];
    return( (mh->size > sizeof( map_entry )) ? entry->p : &entry->v );
}

/*
    MapperGetSafe - given a 'map_id' convert it to a pointer to the storage
        checking if the element is marked as free first. This will be slower
        that MapperGet if 'map_id' is not allocated and may be slower
        (in one case) if it has.
*/
void            *MapperGetSafe( mapper_handle *mh, mapper_id map_id )
{
    map_entry       *entry;
    map_entry       *table;
    map_entry       *fentry;    // free list entry
    mapper_id  free;

    map_id -= mh->base;
    if( map_id > mh->max_num ) return( NULL );
    table = mh->table[map_id / TABLE_ENTRIES];
    if( table == NULL ) return( NULL );
    entry = &table[map_id % TABLE_ENTRIES];

    if( entry->free.magic == FREE_MAGIC ) {
        free = mh->free_list;
        while( free < map_id ) {
            fentry = &mh->table[free / TABLE_ENTRIES][free % TABLE_ENTRIES];
            free = fentry->free.next;
        }
        if( free == map_id ) return( NULL );
    }

    return( (mh->size > sizeof( map_entry )) ? entry->p : &entry->v );
}

/*
    MapperWalk - iterate over the mapping table, calling 'rtn' for each
        entry. The user supplied routine is passed three parameters:

                map_data        - a pointer to the storage for the entry.
                map_id          - the map index for the entry.
                data            - an opaque pointer that the client passes in.

        The user supplied routine does whatever it wants to with the entry
        and the returns either MWR_CONTINUE if it want MapperWalk to continue
        with the next entry in the table, or MWR_STOP if it want to
        terminate the walk. If MapperWalk iterates over all the entries
        in the table it returns MWR_CONTINUE, if it is stopped early it
        returns MWR_STOP.
*/
mapper_walk_res MapperWalk( mapper_handle *mh, mapper_walk_rtn *rtn, void *data )
{
    mapper_id           free;
    mapper_id           i;
    mapper_id           j;
    map_entry           *entry;
    mapper_id           map_id;
    mapper_walk_res     res;

    res = MWR_CONTINUE;
    free = mh->free_list;
    for( i = 0; i < mh->entries; ++i ) {
        if( mh->table[i] != NULL ) {
            for( j = 0; j < TABLE_ENTRIES; ++j ) {
                entry = &mh->table[i][j];
                map_id = (i * TABLE_ENTRIES) + j;
                if( free == map_id ) {
                    free = entry->free.next;
                } else {
                    res = rtn(
                        (mh->size > sizeof( map_entry )) ? entry->p : &entry->v,
                        map_id + mh->base, data );
                    if( res != MWR_CONTINUE ) return( res );
                }
            }
        }
    }
    return( res );
}

#ifdef __cplusplus
}
#endif
