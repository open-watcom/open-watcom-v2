/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2026 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Carving memory manager for linker.
*
****************************************************************************/


#include <stddef.h>
#include <stdlib.h>

#include "linkstd.h"
#include "ideentry.h"
#include "carve.h"


#define CARVE_SIZE(s)   (__ROUND_UP_SIZE( sizeof( blk_t ), sizeof( void * ) ) + (s))
#define CARVE_DATA(c,d) ((char *)(c) + __ROUND_UP_SIZE( sizeof( blk_t ), sizeof( void * ) ) + (d))

// assumes '->free_list' is non-NULL
#define _REMOVE_FROM_FREE( pcv, p ) \
    { \
        free_t *head = pcv->free_list; \
        pcv->free_list = head->next_free; \
        p = head; \
    }

#define _ADD_TO_FREE( fl, p ) \
    { \
        free_t *node = (free_t *) (p); \
        node->next_free = (fl); \
        (fl) = node; \
    }

typedef struct blk_t {
    struct blk_t    *next;
    boolbit         modified    : 1;
    unsigned        index;
} blk_t;

typedef struct free_t {
    struct free_t   *next_free;
} free_t;

typedef struct cv_t {
    free_t          *free_list;
    free_t          *insert;
    blk_t           *blk_list;
    blk_t           **blk_map;
    unsigned        elm_size;
    unsigned        elm_count;
    unsigned        blk_top;
    unsigned        blk_count;
    unsigned        blk_size;
    boolbit         size_chg : 1;
} cv_t;


static blk_t *newBlk( carve_t carver )
/************************************/
{
    blk_t           *newblk;
    blk_t           **blklist;

    newblk = MemAllocSafe( CARVE_SIZE( carver->blk_size ) );
    /*
     * keep list sorted by memory address, biggest first.
     */
    for( blklist = &carver->blk_list; *blklist > newblk; blklist = &(*blklist)->next ) {}
    newblk->next = *blklist;
    *blklist = newblk;
    carver->blk_count++;
    carver->size_chg = true;
    return( newblk );
}

static void MakeFreeList( carve_t carver, blk_t *newblk, unsigned offset )
/************************************************************************/
{
    unsigned        elm_size;
    char            *top_elm;
    char            *bottom_elm;
    char            *free_elm;
    free_t          *free_list;

    elm_size = carver->elm_size;
    bottom_elm = CARVE_DATA( newblk, offset );
    top_elm = CARVE_DATA( newblk, carver->blk_top );
    free_list = carver->free_list;
    free_elm = top_elm;
    do {                         /* free_list must be maintained in order */
        free_elm -= elm_size;
        DbgZapFreed( free_elm, elm_size );
        _ADD_TO_FREE( free_list, free_elm );
    } while( free_elm != bottom_elm );
    carver->free_list = free_list;
}

carve_t CarveCreate( unsigned elm_size, unsigned blk_size )
/*********************************************************/
{
    carve_t         carver;
    unsigned        elm_count;

    if( elm_size < sizeof( free_t ) ) {
        elm_size = sizeof( free_t );
    }
    elm_size = __ROUND_UP_SIZE( elm_size, sizeof( void * ) );
    elm_count = blk_size / elm_size;
    carver = MemAllocSafe( sizeof( *carver ) );
    carver->elm_size = elm_size;
    carver->blk_size = blk_size;
    carver->elm_count = elm_count;
    carver->blk_top = elm_count * elm_size;
    carver->blk_count = 0;
    carver->blk_list = NULL;
    carver->free_list = NULL;
    carver->blk_map = NULL;
    carver->size_chg = false;
//    DbgAssert( carver->elm_size >= 2 * sizeof( void * ) );
    DbgAssert( carver->elm_count != 0 );
    DbgVerify( carver->blk_top < _64K, "carve: size * #/block > 64k" );
    return( carver );
}

#ifdef DEVBUILD
void CarveVerifyAllGone( carve_t carver, const char *node_name )
/**************************************************************/
{
    free_t          *check;
    blk_t           *block;
    char            *compare;
    char            buff[80];
    bool            some_unfreed;

    some_unfreed = false;
    for( block = carver->blk_list; block != NULL; block = block->next ) {
        compare = CARVE_DATA( block, carver->blk_top );
        do {
            compare -= carver->elm_size;
            /* verify every block has been freed */
            for( check = carver->free_list; check != NULL; check = check->next_free ) {
                if( compare == (char *)check ) {
                    break;
                }
            }
            if( check == NULL ) {
                if( ! some_unfreed ) {
                    FmtStr( buff, 80, "carve %s unfreed:", node_name );
                    WriteStdOut( buff );
                    some_unfreed = true;
                }
                FmtStr( buff, 80, " %h", compare );
                WriteStdOut( buff );
            }
        } while( compare != CARVE_DATA( block, 0 ) );
    }
    if( some_unfreed ) {
        WriteStdOutNL();
    }
}
#endif

void CarveDestroy( carve_t carver )
/*********************************/
{
    blk_t           *cur;
    blk_t           *next;

    if( carver != NULL ) {
        if( carver->blk_map != NULL ) {
            MemFree( carver->blk_map );
        }
        for( cur = carver->blk_list; cur != NULL; cur = next ) {
            next = cur->next;
            MemFree( cur );
        }
        MemFree( carver );
    }
}

void *CarveAlloc( carve_t carver )
/********************************/
{
    void            *p;

    if( carver->free_list == NULL ) {
        MakeFreeList( carver, newBlk( carver ), 0 );
    }
    _REMOVE_FROM_FREE( carver, p );
    DbgZapAlloc( p, carver->elm_size );
    return( p );
}

void *CarveZeroAlloc( carve_t carver )
/************************************/
{
    void            *v;
    void            **p;

    if( carver->free_list == NULL ) {
        MakeFreeList( carver, newBlk( carver ), 0 );
    }
    _REMOVE_FROM_FREE( carver, v );
    p = v;
    DbgAssert( ( carver->elm_size / sizeof( *p ) ) <= 16 );
    switch( carver->elm_size / sizeof( *p ) ) {
    case 16:
        p[15] = 0;
    case 15:
        p[14] = 0;
    case 14:
        p[13] = 0;
    case 13:
        p[12] = 0;
    case 12:
        p[11] = 0;
    case 11:
        p[10] = 0;
    case 10:
        p[9] = 0;
    case 9:
        p[8] = 0;
    case 8:
        p[7] = 0;
    case 7:
        p[6] = 0;
    case 6:
        p[5] = 0;
    case 5:
        p[4] = 0;
    case 4:
        p[3] = 0;
    case 3:
        p[2] = 0;
    case 2:
        p[1] = 0;
    case 1:
        p[0] = 0;
    }
    return( p );
}


#ifdef DEVBUILD
static void CarveDebugFree( carve_t carver, void *elm )
{
    free_t          *check;
    blk_t           *block;
    char            *compare;
    char            *start;
    unsigned        esize;

    /* make sure object hasn't been freed before */
    for( check = carver->free_list; check != NULL; check = check->next_free ) {
        if( elm == (void *)check ) {
            LnkFatal( "carve: freed object was previously freed" );
        }
    }
    /* make sure object is from this carve allocator */
    for( block = carver->blk_list; block != NULL; block = block->next ) {
        start = CARVE_DATA( block, 0 );
        compare = start + carver->blk_top;
#if ! ( defined(__COMPACT__) || defined(__LARGE__) )
        /* quick check */
        if( (char *)elm < start || (char *)elm > compare ) {
            continue;
        }
#endif
        esize = carver->elm_size;
        for( ;; ) {
            if( compare == start )
                break;
            compare -= esize;
            if( (char *)elm == compare ) {
                break;
            }
        }
        if( (char *)elm == compare ) {
            break;
        }
    }
    if( block == NULL ) {
        LnkFatal( "carve: freed object was never allocated" );
    }
    DbgZapFreed( elm, carver->elm_size );
}
#else
#define CarveDebugFree( carver, elm )
#endif

void CarveFree( carve_t carver, void *elm )
/*****************************************/
{
    if( elm == NULL ) {
        return;
    }
    CarveDebugFree( carver, elm );
    _ADD_TO_FREE( carver->free_list, elm );
}

void *CarveGetIndex( carve_t carver, void *elm )
/***********************************************
 * note this assumes carve block list sorted by size, biggest first
 */
{
    blk_t           *block;
    unsigned        block_index;

    if( elm == NULL ) {
        return( (void *)CARVE_NULL_INDEX );
    }
    block_index = carver->blk_count;
    for( block = carver->blk_list; elm < (void *)block; block = block->next ) {
        --block_index;
    }
    DbgAssert( block != NULL );
    return( (void *)MK_INDEX( block_index, (char *)elm - CARVE_DATA( block, 0 ) ) );
}

void CarveWalkBlocks( carve_t carver, void (*cbfn)(carve_t, void *, void *), void *cookie )
/*****************************************************************************************/
{
    blk_t           *block;

    for( block = carver->blk_list; block != NULL; block = block->next ) {
        cbfn( carver, block, cookie );
    }
}

bool CarveBlockModified( void *blk )
/**********************************/
{
    return( ((blk_t *)blk)->modified != 0 );
}

void CarveBlockScan( carve_t carver, void *blk, void (*rtn)(void *, void *), void *data )
/***************************************************************************************/
{
    char            *compare;
    char            *end;
    unsigned        esize;

    esize = carver->elm_size;
    compare = CARVE_DATA( blk, 0 );
    end = compare + carver->blk_top;
    do {
        (*rtn)( compare, data );
        compare += esize;
    } while( compare != end );
}

unsigned CarveBlockSize( carve_t carver )
/***************************************/
{
    return( carver->blk_size );
}

void *CarveBlockData( void *block )
/*********************************/
{
    return( CARVE_DATA( block, 0 ) );
}

bool CarveSizeChanged( carve_t carver )
/*************************************/
{
    return( carver->size_chg != 0 );
}

unsigned CarveNumElements( carve_t carver )
/*****************************************/
{
    return( carver->blk_count * carver->elm_count );
}

void CarveWalkAllFree( carve_t carver, void (*rtn)( void * ) )
/************************************************************/
{
    free_t          *check;

    for( check = carver->free_list; check != NULL; check = check->next_free ) {
#ifdef DEVBUILD
        free_t *check_next = check->next_free;
        (*rtn)( check );
        if( check->next_free != check_next ) {
            LnkFatal( "carve walk free routine destroyed links" );
        }
#else
        (*rtn)( check );
#endif
    }
}

void CarveWalkAll( carve_t carver, void (*rtn)( void *, void * ), void *data )
/****************************************************************************/
{
    blk_t           *block;

    for( block = carver->blk_list; block != NULL; block = block->next ) {
        CarveBlockScan( carver, block, rtn, data );
    }
}

void CarveRestart( carve_t carver, unsigned num )
/***********************************************/
{
    unsigned        numblks;
    unsigned        remainder;
    unsigned        index;
    blk_t           *block;

    if( num == 0 )
        return;
    numblks = (num + carver->elm_count - 1) / carver->elm_count;
    for( index = 0; index < numblks; index++ ) {
        newBlk( carver );
    }
    carver->blk_map = MemAllocSafe( numblks * sizeof( *carver->blk_map ) );
    index = numblks - 1;
    for( block = carver->blk_list; block != NULL; block = block->next ) {
        carver->blk_map[index] = block;
        index -= 1;
    }
    remainder = num % carver->elm_count;
    if( remainder != 0 ) {
        MakeFreeList( carver, carver->blk_map[0], remainder * carver->elm_size );
    }
    carver->insert = NULL;
}

static void CarveZapBlock( carve_t carver, void *blk, void *dummy )
/*****************************************************************/
{
    /* unused parameters */ (void)dummy;

    MakeFreeList( carver, blk, 0 );
}

void CarvePurge( carve_t carver )
/********************************
 * clean out a carve block that had been prepared for incremental linking
 */
{
    carver->free_list = NULL;
    CarveWalkBlocks( carver, CarveZapBlock, NULL );
}

void CarveInsertFree( carve_t carver, void *data )
/************************************************/
{
    free_t          *freeblk;

    freeblk = data;
    if( carver->insert == NULL ) {
        freeblk->next_free = carver->free_list;
        carver->free_list = freeblk;
    } else {
        freeblk->next_free = carver->insert->next_free;
        carver->insert->next_free = freeblk;
    }
    carver->insert = freeblk;
}

void *CarveMapIndex( carve_t carver, void *_index )
/*************************************************/
{
    unsigned        index = (unsigned)(pointer_uint)_index;
    blk_t           *block;
    blk_t           **block_map;
    unsigned        block_index;
    unsigned        block_offset;

    /* given an index; find and allocate the carve element */
    if( index == CARVE_NULL_INDEX ) {
        return( NULL );
    }
    block_index = GET_BLOCK( index );
    block_offset = GET_OFFSET( index );
    block_map = carver->blk_map;
    block = block_map[block_index - 1];
    return( CARVE_DATA( block, block_offset ) );
}
