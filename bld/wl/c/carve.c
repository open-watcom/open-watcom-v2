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
* Description:  Carving memory manager for linker.
*
****************************************************************************/


#include <stddef.h>
#include <stdlib.h>

#include "linkstd.h"
#include "msg.h"
#include "ideentry.h"
#include "alloc.h"
#include "carve.h"

struct blk {
    blk_t *     next;
    unsigned    index;
    unsigned    modified : 1;
    unsigned    : 15;
    char        data[1];
};

struct free_t {
    free_t *    next_free;
};

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


static blk_t * newBlk( cv_t *cv )
/*******************************/
{
    blk_t *     newblk;
    blk_t **    blklist;

    _ChkAlloc( newblk, sizeof( blk_t ) - 1 + cv->blk_size );
    for( blklist = &cv->blk_list; *blklist > newblk; ) {    // keep list sorted by memory address
        blklist = &(*blklist)->next;    // biggest first.
    }
    newblk->next = *blklist;
    *blklist = newblk;
    cv->blk_count++;
    cv->size_chg = true;
    return newblk;
}

static void MakeFreeList( cv_t *cv, blk_t *newblk, unsigned offset )
/******************************************************************/
{
    unsigned    elm_size;
    char *      top_elm;
    char *      bottom_elm;
    char *      free_elm;
    free_t *    free_list;

    elm_size = cv->elm_size;
    bottom_elm = newblk->data + offset;
    top_elm = newblk->data + cv->blk_top;
    free_list = cv->free_list;
    free_elm = top_elm;
    do {                         /* free_list must be maintained in order */
        free_elm -= elm_size;
        DbgZapFreed( free_elm, elm_size );
        _ADD_TO_FREE( free_list, free_elm );
    } while( free_elm != bottom_elm );
    cv->free_list = free_list;
}

carve_t CarveCreate( unsigned elm_size, unsigned blk_size )
/******************************************************/
{
    cv_t *      cv;

    elm_size = ROUND_UP( elm_size, sizeof( int ) );
    if( elm_size < sizeof( free_t ) ) {
        elm_size = sizeof( free_t );
    }
    _ChkAlloc( cv, sizeof( *cv ) );
    cv->elm_size = elm_size;
    cv->blk_size = blk_size;
    cv->elm_count = cv->blk_size / cv->elm_size;
    cv->blk_top = cv->elm_count * elm_size;
    cv->blk_count = 0;
    cv->blk_list = NULL;
    cv->free_list = NULL;
    cv->blk_map = NULL;
    cv->size_chg = false;
    DbgAssert( cv->elm_size >= 2 * sizeof(void *) );
    DbgAssert( cv->elm_count != 0 );
    DbgVerify( cv->blk_top < 0x10000, "carve: size * #/block > 64k" );
    return( cv );
}

#ifndef NDEBUG
void CarveVerifyAllGone( carve_t cv, char *node_name )
/****************************************************/
{
    free_t *    check;
    blk_t *     block;
    char *      compare;
    char        buff[80];
    bool        some_unfreed;

    some_unfreed = false;
    for( block = cv->blk_list; block != NULL; block = block->next ) {
        compare = block->data + cv->blk_top;
        do {
            compare = (char *)compare - cv->elm_size;
            /* verify every block has been freed */
            for( check = cv->free_list; check != NULL; check = check->next_free ) {
                if( compare == (char *)check ) break;
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
        } while( compare != block->data );
    }
    if( some_unfreed ) {
        WriteStdOutNL();
    }
}
#endif

void CarveDestroy( carve_t cv )
/*****************************/
{
    blk_t *cur;
    blk_t *next;

    if( cv != NULL ) {
        if( cv->blk_map != NULL ) {
            _LnkFree( cv->blk_map );
        }
        for( cur = cv->blk_list; cur != NULL; cur = next ) {
            next = cur->next;
            _LnkFree( cur );
        }
        _LnkFree( cv );
    }
}

void *CarveAlloc( carve_t cv )
/****************************/
{
    void *      p;

    if( cv->free_list == NULL ) {
        MakeFreeList( cv, newBlk( cv ), 0 );
    }
    _REMOVE_FROM_FREE( cv, p );
    DbgZapAlloc( p, cv->elm_size );
    return p;
}

void *CarveZeroAlloc( carve_t cv )
/********************************/
{
    void *v;
    void **p;

    if( cv->free_list == NULL ) {
        MakeFreeList( cv, newBlk( cv ), 0 );
    }
    _REMOVE_FROM_FREE( cv, v );
    p = v;
    DbgAssert( ( cv->elm_size / sizeof(*p) ) <= 16 );
    switch( cv->elm_size / sizeof(*p) ) {
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
    return p;
}


#ifndef NDEBUG
static void CarveDebugFree( carve_t cv, void *elm )
{
    free_t *check;
    blk_t *block;
    char *compare;
    char *start;
    unsigned esize;

    /* make sure object hasn't been freed before */
    for( check = cv->free_list; check != NULL; check = check->next_free ) {
        if( elm == (void *)check ) {
            LnkFatal( "carve: freed object was previously freed" );
        }
    }
    /* make sure object is from this carve allocator */
    for( block = cv->blk_list; block != NULL; block = block->next ) {
        start = block->data;
        compare = (char *)start + cv->blk_top;
#if ! ( defined(__COMPACT__) || defined(__LARGE__) )
        /* quick check */
        if( elm < start || elm > compare ) {
            continue;
        }
#endif
        esize = cv->elm_size;
        for(;;) {
            if( compare == start ) break;
            compare = (char *)compare - esize;
            if( elm == compare ) break;
        }
        if( elm == compare ) break;
    }
    if( block == NULL ) {
        LnkFatal( "carve: freed object was never allocated" );
    }
    DbgZapFreed( elm, cv->elm_size );
}
#else
#define CarveDebugFree( cv, elm )
#endif

void CarveFree( carve_t cv, void *elm )
/*************************************/
{
    if( elm == NULL ) {
        return;
    }
    CarveDebugFree( cv, elm );
    _ADD_TO_FREE( cv->free_list, elm );
}

void *CarveGetIndex( carve_t cv, void *elm )
/******************************************/
/* note this assumes carve block list sorted by size, biggest first */
{
    blk_t *     block;
    unsigned    block_index;

    if( elm == NULL ) {
        return( (void *)CARVE_NULL_INDEX );
    }
    block_index = cv->blk_count;
    for( block = cv->blk_list; elm < (void *)block; block = block->next ) {
        --block_index;
    }
    DbgAssert( block != NULL );
    return( (void *)MK_INDEX( block_index, (char *)elm - block->data ) );
}

void CarveWalkBlocks( carve_t cv, void (*cbfn)(carve_t, void *, void *), void *cookie )
/*************************************************************************************/
{
    blk_t *     block;

    for( block = cv->blk_list; block != NULL; block = block->next ) {
        cbfn( cv, block, cookie );
    }
}

bool CarveBlockModified( void *blk )
/**********************************/
{
    return( ((blk_t *)blk)->modified != 0 );
}

void CarveBlockScan( carve_t cv, void *blk, void (*rtn)(void *, void *), void *data )
/***********************************************************************************/
{
    char *      compare;
    char *      end;
    unsigned    esize;

    esize = cv->elm_size;
    compare = ((blk_t *)blk)->data;
    end = compare + cv->blk_top;
    do {
        (*rtn)( compare, data );
        compare += esize;
    } while( compare != end );
}

unsigned CarveBlockSize( carve_t cv )
/***********************************/
{
    return( cv->blk_size );
}

void * CarveBlockData( void *block )
/**********************************/
{
    return( ((blk_t *)block)->data );
}

bool CarveSizeChanged( carve_t cv )
/*********************************/
{
    return( cv->size_chg != 0 );
}

unsigned CarveNumElements( carve_t cv )
/*************************************/
{
    return( cv->blk_count * cv->elm_count );
}

void CarveWalkAllFree( carve_t cv, void (*rtn)( void * ) )
/********************************************************/
{
    free_t *check;

    for( check = cv->free_list; check != NULL; check = check->next_free ) {
#ifndef NDEBUG
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

void CarveWalkAll( carve_t cv, void (*rtn)( void *, void * ), void *data )
/************************************************************************/
{
    blk_t *     block;

    for( block = cv->blk_list; block != NULL; block = block->next ) {
        CarveBlockScan( cv, block, rtn, data );
    }
}

void CarveRestart( carve_t cv, unsigned num )
/**************************************************/
{
    unsigned    numblks;
    unsigned    remainder;
    unsigned    index;
    blk_t *     block;

    if( num == 0 ) return;
    numblks = (num + cv->elm_count - 1) / cv->elm_count;
    for( index = 0; index < numblks; index++ ) {
        newBlk( cv );
    }
    _ChkAlloc( cv->blk_map, numblks * sizeof( blk_t * ) );
    index = numblks - 1;
    for( block = cv->blk_list; block != NULL; block = block->next ) {
        cv->blk_map[ index ] = block;
        index -= 1;
    }
    remainder = num % cv->elm_count;
    if( remainder != 0 ) {
        MakeFreeList( cv, cv->blk_map[0], remainder * cv->elm_size );
    }
    cv->insert = NULL;
}

static void CarveZapBlock( carve_t cv, void *blk, void *dummy )
/*************************************************************/
{
    dummy = dummy;
    MakeFreeList( cv, blk, 0 );
}

void CarvePurge( carve_t cv )
/**********************************/
/* clean out a carve block that had been prepared for incremental linking */
{
    cv->free_list = NULL;
    CarveWalkBlocks( cv, CarveZapBlock, NULL );
}

void CarveInsertFree( carve_t cv, void *data )
/********************************************/
{
    free_t *    freeblk;

    freeblk = data;
    if( cv->insert == NULL ) {
        freeblk->next_free = cv->free_list;
        cv->free_list = freeblk;
    } else {
        freeblk->next_free = cv->insert->next_free;
        cv->insert->next_free = freeblk;
    }
    cv->insert = freeblk;
}

void *CarveMapIndex( carve_t cv, void *aindex )
/*********************************************/
{
    unsigned    index = (unsigned)(pointer_int)aindex;
    blk_t *     block;
    blk_t **    block_map;
    unsigned    block_index;
    unsigned    block_offset;

    /* given an index; find and allocate the carve element */
    if( index == CARVE_NULL_INDEX ) {
        return( NULL );
    }
    block_index = GET_BLOCK( index );
    block_offset = GET_OFFSET( index );
    block_map = cv->blk_map;
    block = block_map[ block_index - 1 ];
    return( &(block->data[ block_offset ]) );
}
