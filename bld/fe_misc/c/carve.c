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
* Description:  Carver implementation.
*
****************************************************************************/


#ifdef __header1
#   include __header1
#endif
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#if 0
#include "plusplus.h"
#include "errdefns.h"
#include "stats.h"
#include "memmgr.h"
#include "ring.h"
#include "pcheader.h"
#include "pragdefn.h"
#include "carve.h"
#else

#include "ringcarv.h"

#endif

#if ( defined(__COMPACT__) || defined(__LARGE__) )
#error code will not work with segmented pointers
#endif

#ifndef TRUE
#   define TRUE  1
#   define FALSE 0
#endif


struct blk {
    blk_t       *next;
    char        data[1];
};

struct free_t {
    free_t      *next_free;
};

// assumes '->free_list' is non-NULL
#define _REMOVE_FROM_FREE( pcv, p ) \
    { \
        free_t *head = pcv->free_list; \
        pcv->free_list = head->next_free; \
        p = head; \
        DbgZapVerify( p, pcv->elm_size ); \
    }


#define _ADD_TO_FREE( fl, p ) \
    { \
        free_t *node = (free_t *) (p); \
        node->next_free = (fl); \
        (fl) = node; \
    }

#ifndef NDEBUG
static bool    restoreFromZapped( cv_t *cv )
{
    unsigned elm_size;
    free_t *free_list;
    free_t *curr_zapped;
    free_t *next_zapped;

    // we now check to make sure freed carve blocks have not been modified
    // but during PCH writing, we intentionally modify a free block to
    // distinguish them from allocated blocks thus we have to keep them
    // out of the free list until we can re-zap them
    elm_size = cv->elm_size;
    free_list = cv->free_list;
    curr_zapped = cv->zapped_free_list;
    if( curr_zapped != NULL ) {
        cv->zapped_free_list = NULL;
        do {
            next_zapped = curr_zapped->next_free;
            DbgZapFreed( curr_zapped, elm_size );
            _ADD_TO_FREE( free_list, curr_zapped );
            curr_zapped = next_zapped;
        } while( curr_zapped != NULL );
        cv->free_list = free_list;
        return( TRUE );
    }
    return( FALSE );
}
#else
#define restoreFromZapped( x )
#endif


static void newBlk( cv_t *cv )
{
    unsigned elm_size;
    char *top_elm;
    char *bottom_elm;
    char *free_elm;
    blk_t *newblk;
    free_t *free_list;

    DbgStmt( if( restoreFromZapped( cv ) ) return; );
    elm_size = cv->elm_size;
    free_list = cv->free_list;
    newblk = _MemoryAllocate( sizeof( blk_t ) - 1 + cv->blk_top );
    newblk->next = cv->blk_list;
    cv->blk_list = newblk;
    cv->blk_count++;
    bottom_elm = newblk->data;
    top_elm = bottom_elm + cv->blk_top;
    free_elm = bottom_elm;
    do {
        /* free_list must be maintained as the reverse of CarveWalkAll ordering */
        DbgZapFreed( free_elm, elm_size );
        _ADD_TO_FREE( free_list, free_elm );
        free_elm += elm_size;
    } while( free_elm != top_elm );
    cv->free_list = free_list;
}

carve_t CarveCreate( unsigned elm_size, unsigned elm_count )
/**********************************************************/
{
    cv_t *cv;

    elm_size = ( elm_size + (sizeof(int)-1) ) & ~(sizeof(int)-1);
    if( elm_size < sizeof( free_t ) ) {
        elm_size = sizeof( free_t );
    }
    cv = _MemoryAllocate( sizeof( *cv ) );
    cv->elm_size = elm_size;
    cv->elm_count = elm_count;
    cv->blk_top = elm_count * elm_size;
    cv->blk_count = 0;
    cv->blk_list = NULL;
    cv->free_list = NULL;
    cv->blk_map = NULL;
    DbgStmt( cv->zapped_free_list = NULL );
    DbgAssert( cv->elm_size != 0 );
    DbgVerify( cv->blk_top <= (1 << CV_SHIFT), "carve: size * #/block is too large" );
    return( cv );
}

void CarveDestroy( carve_t cv )
/*****************************/
{
    blk_t *cur;
    blk_t *next;

    if( cv != NULL ) {
        cur = cv->blk_list;
        while( cur != NULL ) {
            next = cur->next;
            _MemoryFree( cur );
            cur = next;
        }
        DbgAssert( cv->blk_map == NULL );
        _MemoryFree( cv );
    }
}

void *CarveAlloc( carve_t cv )
/****************************/
{
    void *p;

    if( cv->free_list == NULL ) {
        newBlk( cv );
    }
    _REMOVE_FROM_FREE( cv, p );
    DbgZapAlloc( p, cv->elm_size );
    return p;
}

#if 0
void *CarveZeroAlloc( carve_t cv )
/********************************/
{
    void *v;
    unsigned *p;

    if( cv->free_list == NULL ) {
        newBlk( cv );
    }
    _REMOVE_FROM_FREE( cv, v );
    p = v;
    DbgAssert( ( cv->elm_size / sizeof(*p) ) <= 8 );
    switch( cv->elm_size / sizeof(*p) ) {
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
#endif


#ifndef NDEBUG
#define TOO_MANY_TO_WALK        512

static blk_t *withinABlock( carve_t cv, void *elm )
{
    blk_t *block;
    char *compare;
    char *start;

    for( block = cv->blk_list; block != NULL; block = block->next ) {
        start = block->data;
        compare = start + cv->blk_top;
        if( (char *)elm < start || (char *)elm > compare ) {
            continue;
        }
        return( block );
    }
    return( NULL );
}

static void CarveDebugFree( carve_t cv, void *elm )
{
    free_t *check;
    blk_t *block;
    char *compare;
    char *start;
    unsigned esize;
    int i;
    bool do_search;

    /* make sure object hasn't been freed before */
    restoreFromZapped( cv );
    esize = cv->elm_size;
    if(( cv->elm_count * cv->blk_count ) > TOO_MANY_TO_WALK ) {
        // there are lots of blocks to search so we weaken the check for speed
        do_search = FALSE;
        check = elm;
        for( i = 0; i < 4; ++i ) {
            if( DbgZapQuery( check, esize ) != 0 ) {
                // would pass check to return as a free block!
                do_search = TRUE;
                break;
            }
            if( ! withinABlock( cv, check->next_free ) ) {
                break;
            }
            check = check->next_free;
        }
    } else {
        do_search = TRUE;
    }
    if( do_search ) {
        for( check = cv->free_list; check != NULL; check = check->next_free ) {
            if( elm == (void *)check ) {
                _FatalAbort( "carve: freed object was previously freed" );
            }
        }
    }
    /* make sure object is from this carve allocator */
    for( block = cv->blk_list; block != NULL; block = block->next ) {
        start = block->data;
        compare = start + cv->blk_top;
        if( (char *)elm < start || (char *)elm > compare ) {
            continue;
        }
        for(;;) {
            if( compare == start ) break;
            compare -= esize;
            if( elm == compare ) break;
        }
        if( elm == compare ) break;
    }
    if( block == NULL ) {
        _FatalAbort( "carve: freed object was never allocated" );
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

#ifndef NDEBUG
void CarveVerifyAllGone( carve_t cv, char const *node_name )
/**********************************************************/
{
    free_t *check;
    blk_t *block;
    char *compare;
    bool some_unfreed;

#ifdef ERR_RET
    if( ERR_RET ) {
        return;
    }
#endif
    restoreFromZapped( cv );
    some_unfreed = FALSE;
    for( block = cv->blk_list; block != NULL; block = block->next ) {
        compare = block->data + cv->blk_top;
        do {
            compare -= cv->elm_size;
            /* verify every block has been freed */
            for( check = cv->free_list; check != NULL; check = check->next_free ) {
                if( compare == (void *) check ) break;
            }
            if( check == NULL ) {
                if( ! some_unfreed ) {
                    printf( "%s nodes unfreed:", node_name );
                    some_unfreed = TRUE;
                }
                printf( " %p", compare );
            }
        } while( compare != block->data );
    }
    if( some_unfreed ) {
        putchar( '\n' );
#ifdef ERR_SET
        ERR_SET;
#endif
    }
}
#endif

#ifdef CARVEPCH

//!!!!!!!!!!!!!
//!!!!!!!!!!!!! THE FOLLOWING IS LIKELY C++ DEPENDENT.
//!!!!!!!!!!!!!
//!!!!!!!!!!!!! I'VE NOT ABSTRACTED THE DEPENDENCIES.
//!!!!!!!!!!!!!
//!!!!!!!!!!!!! JIM WELCH
//!!!!!!!!!!!!!

void *CarveGetIndex( carve_t cv, void *elm )
/******************************************/
{
    char *start;
    char *top;
    blk_t *block;
    unsigned block_index;

    if( elm == NULL ) {
        return( (void *)CARVE_NULL_INDEX );
    }
    block_index = cv->blk_count + 1;
    for( block = cv->blk_list; block != NULL; block = block->next ) {
        --block_index;
        start = block->data;
        if( (char *)elm >= start ) {
            top = start + cv->blk_top;
            if( (char *)elm < top ) {
                DbgAssert( (( (char *)elm - start ) % cv->elm_size ) == 0 );
                return( (void *)MK_INDEX( block_index, (char *)elm - start ) );
            }
        }
    }
    _FatalAbort( "unable to find carve memory block" );
    return( (void *)CARVE_ERROR_INDEX );
}

carve_t CarveRestart( carve_t cv )
/********************************/
{
    blk_t *block;
    blk_t *next;
    char *free_elm;
    char *stop;
    unsigned esize;
    free_t *free_list;

#ifdef DUMP_MEMORY
    if( !PragDbgToggle.dump_memory ) {
        carve_t old_cv = cv;
        restoreFromZapped( old_cv );
        old_cv->free_list = (void *)-1;
        for( block = old_cv->blk_list; block != NULL; block = next ) {
            next = block->next;
            DbgZapMem( block->data, -1, old_cv->blk_top );
            _MemoryDeferredFree( block );
        }
        old_cv->blk_list = (void *) -1;
        cv = CarveCreate( old_cv->elm_size, old_cv->elm_count );
        _MemoryFree( old_cv );
    }
#endif
    block = cv->blk_list;
    cv->blk_list = NULL;
    free_list = NULL;
    esize = cv->elm_size;
    for( ; block != NULL; block = next ) {
        next = block->next;
        block->next = cv->blk_list;
        cv->blk_list = block;
        free_elm = block->data;
        stop = free_elm + cv->blk_top;
        do {
            /* free_list must be maintained as the reverse of CarveWalkAll ordering */
            DbgZapMem( free_elm, 0x1d, esize );
            _ADD_TO_FREE( free_list, free_elm );
            free_elm += esize;
        } while( free_elm != stop );
    }
    cv->free_list = free_list;
    return( cv );
}

void *CarveMapIndex( carve_t cv, void *aindex )
/*********************************************/
{
    unsigned index = (unsigned)(pointer_int)aindex;
    blk_t *block;
    blk_t **block_map;
    unsigned block_index;
    unsigned block_offset;
    unsigned block_count;

    /* given an index; find and allocate the carve element */
    if( index == CARVE_NULL_INDEX ) {
        return( NULL );
    }
    block_index = GET_BLOCK( index );
    block_offset = GET_OFFSET( index );
    block_map = cv->blk_map;
    if( block_map != NULL ) {
        block = block_map[ block_index - 1 ];
        return( &(block->data[ block_offset ]) );
    }
    block_count = cv->blk_count;
    for(;;) {
        /* make sure that there are enough carve blocks */
        if( block_index <= block_count ) {
            break;
        }
        newBlk( cv );
        ++block_count;
    }
    DbgAssert( cv->blk_count == block_count );
    for( block = cv->blk_list; block != NULL; block = block->next ) {
        if( block_index == block_count ) {
            return( &(block->data[ block_offset ]) );
        }
        --block_count;
    }
    _FatalAbort( "unable to find carve memory block" );
    return( NULL );
}

void CarveWalkAllFree( carve_t cv, void (*rtn)( void * ) )
/********************************************************/
{
    free_t *check;

    restoreFromZapped( cv );
    check = cv->free_list;
#ifndef NDEBUG
    // transfer over to another list since these free blocks may be modified
    cv->free_list = NULL;
    cv->zapped_free_list = check;
#endif
    for( ; check != NULL; check = check->next_free ) {
#ifndef NDEBUG
        free_t *check_next = check->next_free;
        (*rtn)( check );
        if( check->next_free != check_next ) {
            // restore to prevent possible memory faults
            check->next_free = check_next;
            _FatalAbort( "carve walk free routine destroyed links" );
        }
#else
        (*rtn)( check );
#endif
    }
}

#ifndef NDEBUG
void CarveVerifyFreeElement( carve_t cv, void *elm )
/**************************************************/
{
    free_t *check;

    for( check = cv->free_list; check != NULL; check = check->next_free ) {
        if( elm == check ) {
            return;
        }
    }
    for( check = cv->zapped_free_list; check != NULL; check = check->next_free ) {
        if( elm == check ) {
            return;
        }
    }
    _FatalAbort( "element looks free but isn't" );
}
#endif

void CarveWalkAll( carve_t cv, void (*rtn)( void *, carve_walk_base * ), carve_walk_base *data )
/**********************************************************************************************/
{
    blk_t *block;
    char *compare;
    char *start;
    unsigned esize;
    unsigned block_index;
    unsigned block_offset;

    esize = cv->elm_size;
    block_index = cv->blk_count;
    for( block = cv->blk_list; block != NULL; block = block->next ) {
        start = block->data;
        block_offset = cv->blk_top;
        compare = start + block_offset;
        do {
            compare -= esize;
            block_offset -= esize;
            data->index = MK_INDEX( block_index, block_offset );
            (*rtn)( compare, data );
        } while( compare != start );
        --block_index;
    }
}

void CarveMapOptimize( carve_t cv, cv_index last_valid_index )
/************************************************************/
{
    blk_t *block;
    blk_t **init;
    unsigned nmaps;
    unsigned mindex;

    if( last_valid_index == CARVE_NULL_INDEX ) {
        return;
    }
    // make sure there are enough blocks allocated
    CarveMapIndex( cv, (void *)(pointer_int)last_valid_index );
    nmaps = GET_BLOCK( last_valid_index );
    init = _MemoryAllocate( nmaps * sizeof( blk_t * ) );
    cv->blk_map = init;
#ifndef NDEBUG
    if( nmaps != cv->blk_count ) {
        _FatalAbort( "incorrect block calculation" );
    }
    DbgZapMem( init, 0xff, nmaps * sizeof( blk_t * ) );
#endif
    mindex = nmaps - 1;
    for( block = cv->blk_list; block != NULL; block = block->next ) {
        init[ mindex ] = block;
        --mindex;
    }
}

void CarveMapUnoptimize( carve_t cv )
/***********************************/
{
    _MemoryFreePtr( &(cv->blk_map) );
}

cv_index CarveLastValidIndex( carve_t cv )
/****************************************/
{
    if( cv->blk_count == 0 ) {
        return( CARVE_NULL_INDEX );
    }
    return( MK_INDEX( cv->blk_count, cv->blk_top - cv->elm_size ) );
}

void *CarveInitElement( cvinit_t *data, cv_index index )
/******************************************************/
{
    blk_t *block;
    free_t *check;
    free_t **head;
    unsigned want_block;
    unsigned curr_block;
    char *init_elm;

    DbgAssert( data->cv->blk_map != NULL );
    block = data->block;
    want_block = GET_BLOCK( index );
    curr_block = data->bindex;
    if( want_block != curr_block ) {
        DbgAssert( want_block <= curr_block );
        while( want_block != curr_block ) {
            block = block->next;
            --curr_block;
        }
        data->block = block;
        data->bindex = curr_block;
    }
    init_elm = &(block->data[ GET_OFFSET( index ) ]);
    head = data->head;
    check = *head;
    for(;;) {
        DbgAssert( check != NULL );
        if( check == (free_t *)init_elm ) {
            *head = check->next_free;
            data->head = head;
            DbgZapMem( init_elm, 0x19, data->cv->elm_size );
            return( init_elm );
        }
        head = &(check->next_free);
        check = *head;
    }
}

void CarveInitStart( carve_t cv, cvinit_t *data )
/***********************************************/
{
    data->block = cv->blk_list;
    data->bindex = cv->blk_count;
    data->head = &(cv->free_list);
    data->cv = cv;
}


#if 0 && ! defined(NDEBUG)
carve_t carveMASTER;
carve_t carveSLAVE1;
carve_t carveSLAVE2;

typedef struct master MASTER;
typedef struct slave1 SLAVE1;
typedef struct slave2 SLAVE2;
struct master {
    MASTER      *next;
    SLAVE1      *ring1;
    SLAVE2      *ring2;
    MASTER      *test;
    unsigned    free : 1;
};

struct slave1 {
    SLAVE1      *next;
    SLAVE2      *sister;
    MASTER      *master;
    unsigned    free : 1;
};

struct slave2 {
    SLAVE2      *next;
    SLAVE1      *brother;
    MASTER      *master;
    unsigned    free : 1;
};

MASTER *masterList;

unsigned waste;
SLAVE1 *waste1;
SLAVE2 *waste2;

SLAVE1 *newSLAVE1()
{
    SLAVE1 *s1;

    s1 = CarveAlloc( carveSLAVE1 );
    ++waste;
    if( !( waste & 3 ) ) {
        s1->next = waste1;
        waste1 = s1;
        s1 = CarveAlloc( carveSLAVE1 );
    }
    return( s1 );
}

SLAVE2 *newSLAVE2()
{
    SLAVE2 *s2;

    s2 = CarveAlloc( carveSLAVE2 );
    ++waste;
    if( !( waste & 3 ) ) {
        s2->next = waste2;
        waste2 = s2;
        s2 = CarveAlloc( carveSLAVE2 );
    }
    return( s2 );
}

void dumpMess()
{
    MASTER *m;
    SLAVE1 *s1;
    SLAVE2 *s2;

    printf( "masterList:\n" );
    RingIterBeg( masterList, m ) {
        printf( "m: %p next: %p ring1: %p ring2: %p test: %p\n", m, m->next, m->ring1, m->ring2, m->test );
        RingIterBeg( m->ring1, s1 ) {
            printf( " 1: %p next: %p sibling: %p master: %p\n", s1, s1->next, s1->sister, s1->master );
        } RingIterEnd( s1 )
        RingIterBeg( m->ring2, s2 ) {
            printf( " 2: %p next: %p sibling: %p master: %p\n", s2, s2->next, s2->brother, s2->master );
        } RingIterEnd( s2 )
    } RingIterEnd( m )
    printf( "---\n" );
}

void createMess()
{
    int i,j;
    MASTER *m;
    SLAVE1 *s1;
    SLAVE2 *s2;

    carveMASTER = CarveCreate( sizeof( MASTER ), 1 );
    carveSLAVE1 = CarveCreate( sizeof( SLAVE1 ), 2 );
    carveSLAVE2 = CarveCreate( sizeof( SLAVE2 ), 31 );
    for( i = 0; i < 5; ++i ) {
        m = CarveAlloc( carveMASTER );
        RingAppend( &masterList, m );
        m->free = FALSE;
        m->ring1 = NULL;
        m->ring2 = NULL;
        if( i & 1 ) {
            m->test = NULL;
        } else {
            m->test = m;
        }
        for( j = 0; j < 5; ++j ) {
            s1 = newSLAVE1();
            s2 = newSLAVE2();
            RingAppend( &m->ring1, s1 );
            RingPush( &m->ring2, s2 );
            s1->free = FALSE;
            s2->free = FALSE;
            s1->master = m;
            s2->master = m;
            s1->sister = s2->next;
            s2->brother = s1->next;
        }
    }
    while( waste1 ) {
        s1 = waste1;
        waste1 = s1->next;
        CarveFree( carveSLAVE1, s1 );
    }
    while( waste2 ) {
        s2 = waste2;
        waste2 = s2->next;
        CarveFree( carveSLAVE2, s2 );
    }
}

void markFreeMaster( void *p )
{
    MASTER *m = p;

    m->free = TRUE;
}

void markFreeSlave1( void *p )
{
    SLAVE1 *s = p;

    s->free = TRUE;
}

void markFreeSlave2( void *p )
{
    SLAVE2 *s = p;

    s->free = TRUE;
}

void saveMaster( void *e, carve_walk_base *d )
{
    MASTER *m = e;

    if( m->free ) {
        return;
    }
    m->next = CarveGetIndex( carveMASTER, m->next );
    m->ring1 = CarveGetIndex( carveSLAVE1, m->ring1 );
    m->ring2 = CarveGetIndex( carveSLAVE2, m->ring2 );
    m->test = CarveGetIndex( carveMASTER, m->test );
    PCHWriteCVIndex( d->index );
    PCHWrite( m, sizeof( *m ) );
}

void saveSlave1( void *e, carve_walk_base *d )
{
    SLAVE1 *s = e;

    if( s->free ) {
        return;
    }
    s->next = CarveGetIndex( carveSLAVE1, s->next );
    s->sister = CarveGetIndex( carveSLAVE2, s->sister );
    s->master = CarveGetIndex( carveMASTER, s->master );
    PCHWriteCVIndex( d->index );
    PCHWrite( s, sizeof( *s ) );
}

void saveSlave2( void *e, carve_walk_base *d )
{
    SLAVE2 *s = e;

    if( s->free ) {
        return;
    }
    s->next = CarveGetIndex( carveSLAVE2, s->next );
    s->brother = CarveGetIndex( carveSLAVE1, s->brother );
    s->master = CarveGetIndex( carveMASTER, s->master );
    PCHWriteCVIndex( d->index );
    PCHWrite( s, sizeof( *s ) );
}

pch_status PCHWriteTest( void )
{
    void *mlist;
    unsigned terminator = CARVE_NULL_INDEX;
    auto carve_walk_base dm;
    auto carve_walk_base d1;
    auto carve_walk_base d2;

    CarveWalkAllFree( carveMASTER, markFreeMaster );
    CarveWalkAllFree( carveSLAVE1, markFreeSlave1 );
    CarveWalkAllFree( carveSLAVE2, markFreeSlave2 );
    mlist = CarveGetIndex( carveMASTER, masterList );
    PCHWrite( &mlist, sizeof( mlist ) );
    CarveWalkAll( carveMASTER, saveMaster, &dm );
    PCHWriteCVIndex( terminator );
    CarveWalkAll( carveSLAVE1, saveSlave1, &d1 );
    PCHWriteCVIndex( terminator );
    CarveWalkAll( carveSLAVE2, saveSlave2, &d2 );
    PCHWriteCVIndex( terminator );
    return( PCHCB_OK );
}

pch_status PCHReadTest( void )
{
    MASTER *m;
    SLAVE1 *s1;
    SLAVE2 *s2;
    cv_index mi;
    auto cvinit_t data;

    PCHRead( &masterList, sizeof( masterList ) );
    masterList = CarveMapIndex( carveMASTER, masterList );
    CarveInitStart( carveMASTER, &data );
    for(;;) {
        mi = PCHReadCVIndex();
        if( mi == CARVE_NULL_INDEX ) break;
        m = CarveInitElement( &data, mi );
        PCHRead( m, sizeof( *m ) );
        m->next = CarveMapIndex( carveMASTER, m->next );
        m->ring1 = CarveMapIndex( carveSLAVE1, m->ring1 );
        m->ring2 = CarveMapIndex( carveSLAVE2, m->ring2 );
        m->test = CarveMapIndex( carveMASTER, m->test );
    }
    CarveInitStart( carveSLAVE1, &data );
    for(;;) {
        mi = PCHReadCVIndex();
        if( mi == CARVE_NULL_INDEX ) break;
        s1 = CarveInitElement( &data, mi );
        PCHRead( s1, sizeof( *s1 ) );
        s1->next = CarveMapIndex( carveSLAVE1, s1->next );
        s1->master = CarveMapIndex( carveMASTER, s1->master );
        s1->sister = CarveMapIndex( carveSLAVE2, s1->sister );
    }
    CarveInitStart( carveSLAVE2, &data );
    for(;;) {
        mi = PCHReadCVIndex();
        if( mi == CARVE_NULL_INDEX ) break;
        s2 = CarveInitElement( &data, mi );
        PCHRead( s2, sizeof( *s2 ) );
        s2->next = CarveMapIndex( carveSLAVE2, s2->next );
        s2->master = CarveMapIndex( carveMASTER, s2->master );
        s2->brother = CarveMapIndex( carveSLAVE1, s2->brother );
    }
    dumpMess();
    return( PCHCB_OK );
}

pch_status PCHInitTest( bool writing )
{
    cv_index nm, ns1, ns2;

    if( writing ) {
        createMess();
        dumpMess();
        nm = CarveLastValidIndex( carveMASTER );
        ns1 = CarveLastValidIndex( carveSLAVE1 );
        ns2 = CarveLastValidIndex( carveSLAVE2 );
        PCHWrite( &nm, sizeof( nm ) );
        PCHWrite( &ns1, sizeof( ns1 ) );
        PCHWrite( &ns2, sizeof( ns2 ) );
    } else {
        createMess();
        carveMASTER = CarveRestart( carveMASTER );
        carveSLAVE1 = CarveRestart( carveSLAVE1 );
        carveSLAVE2 = CarveRestart( carveSLAVE2 );
        PCHRead( &nm, sizeof( nm ) );
        PCHRead( &ns1, sizeof( ns1 ) );
        PCHRead( &ns2, sizeof( ns2 ) );
        CarveMapOptimize( carveMASTER, nm );
        CarveMapOptimize( carveSLAVE1, ns1 );
        CarveMapOptimize( carveSLAVE2, ns2 );
    }
    return( PCHCB_OK );
}

pch_status PCHFiniTest( bool writing )
{
    if( ! writing ) {
        CarveMapUnoptimize( carveMASTER );
        CarveMapUnoptimize( carveSLAVE1 );
        CarveMapUnoptimize( carveSLAVE2 );
    }
    return( PCHCB_OK );
}
#endif // NDEBUG

#endif // CARVEPCH
