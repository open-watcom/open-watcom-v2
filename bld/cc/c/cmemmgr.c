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
* Description:  C compiler memory management routines.
*
****************************************************************************/


#include "cvars.h"
#include "cgdefs.h"
#include "feprotos.h"
#include "roundmac.h"


/*
 * RISC platforms are likely to take a big hit for misaligned accesses.
 * Memory blocks are ordinarily aligned to sizeof( unsigned ) which is
 * OK for many platforms, but notably on Alpha we need 8-byte alignment
 * to make sure doubles won't get us in trouble.
 * NB: MEM_ALIGN must be at least int-sized
 */

#if defined( __AXP__ )
    #define MEM_ALIGN   8
#else
    #define MEM_ALIGN   sizeof( void * )
#endif

#define ALLOC_FLAG      1

#define BLK_HDR_SIZE    __ROUND_UP_SIZE( sizeof( mem_blk ), MEM_ALIGN )
#define BLK_SIZE(s)     (BLK_HDR_SIZE + (s))
#define BLK_DATA(m,x)   ((char *)(m) + BLK_HDR_SIZE + (x))

#define MCB_HDR_SIZE    __ROUND_UP_SIZE( sizeof( MCB ), MEM_ALIGN )
#define MCB_SIZE(s)     (MCB_HDR_SIZE + (s))
#define MCB_DATA(m,x)   ((char *)(m) + MCB_HDR_SIZE + (x))
#define MCB_FROM_C(p)   ((MCB *)((char *)(p) - MCB_HDR_SIZE))
#define MCB_NEXT(x)     ((MCB *)((char *)(x) + ((x)->size & ~ALLOC_FLAG)))

/*
 * Size of permanent area. Needs to be reasonably big to satisfy
 * large allocation requests. Must by multiple of 0x20
 */
#define MAX_PERM_SIZE   0x00100000

#define PERM_BLK_END    (0 | ALLOC_FLAG)

enum cmem_kind {
    CMEM_PERM,
    CMEM_MEM,
    CMEM_NONE,
};

typedef struct  mem_block {
    size_t              size;   /* length of stg */
    struct mem_block    *prev;  /* pointer to previous free memory block */
    struct mem_block    *next;  /* pointer to next     free memory block */
} MCB;

typedef struct mem_blk {
    struct mem_blk      *next;
    char                *ptr;   // old permanent area block pointer
    size_t              size;   // old permanent area block size
} mem_blk;

/*
 *  variables used:
 *      char *PermPtr;          first free byte in permanent area block
 *      size_t PermSize;        total size of permanent area block
 *      size_t PermAvail;       number of bytes available in permanent area block
 *
 *  Permanent memory is allocated from low to high.
 *  Memory allocated using CPermAlloc is never given back.
 *  Temporary allocations are made at the high end of permanent memory.
*/

static  char        *PermPtr;   /* first free byte in permanent area block */
static  size_t      PermSize;   /* total size of permanent area block */
static  size_t      PermAvail;  /* # of bytes available in permanent area block */

static  MCB         CFreeList;
static  mem_blk     *Blks;

static void InitPermArea( void )
/******************************/
{
    Blks = NULL;
    PermAvail = 0;
    PermPtr = NULL;
    PermSize = 0;
}

static void FiniPermArea( void )
/******************************/
{
    mem_blk *curr;
    mem_blk *next;

    for( curr = Blks; curr != NULL; curr = next ) {
        next = curr->next;
        free( curr );
    }
    Blks = NULL;
}

static void addMCBtoFreeList( MCB *mcb )
/**************************************/
{
    MCB     *mcb_prev;
    MCB     *mcb_next;

    mcb->size &= ~ALLOC_FLAG;
    mcb_prev = &CFreeList;
    for( ;; ) {         /* insert in sorted order */
        mcb_next = mcb_prev;
        mcb_prev = mcb_prev->prev;
        if( mcb_prev == &CFreeList )
            break;
        if( mcb_prev < mcb ) {
            break;
        }
    }
    mcb_next->prev = mcb;
    mcb_prev->next = mcb;
    mcb->prev = mcb_prev;
    mcb->next = mcb_next;
}

static void removeMCBfromFreeList( MCB *mcb )
/*******************************************/
{
    MCB     *mcb_prev;
    MCB     *mcb_next;

    mcb_next = mcb->next;
    mcb_prev = mcb->prev;
    mcb_prev->next = mcb_next;
    mcb_next->prev = mcb_prev;
}

static void AllocPermArea( void )
/*******************************/
{
    for( PermSize = MAX_PERM_SIZE; PermSize != 0; PermSize -= 0x20 ) {
        mem_blk *blk;
        blk = calloc( 1, BLK_SIZE( PermSize + sizeof( blk->size ) ) );
        if( blk != NULL ) {
            blk->next = Blks;
            Blks = blk;
            blk->ptr = PermPtr;
            blk->size = PermSize;
            ((MCB *)BLK_DATA( blk, PermSize ))->size = PERM_BLK_END;    /* null length tag */
            PermPtr = BLK_DATA( blk, 0 );
            PermAvail = PermSize;
            return;
        }
    }
    PermPtr = NULL;
    PermAvail = PermSize = 0;
}


void CMemInit( void )
/*******************/
{
    InitPermArea();
    CFreeList.size = 0;
    CFreeList.next = &CFreeList;
    CFreeList.prev = &CFreeList;
}

void CMemFini( void )
/*******************/
{
    CFreeList.size = 0;
    CFreeList.next = &CFreeList;
    CFreeList.prev = &CFreeList;
    FiniPermArea();
}

static void Ccoalesce( MCB *mcb1 )
/********************************/
{
    MCB *mcb2;

    for( ;; ) {
        mcb2 = MCB_NEXT( mcb1 );
        /*
         * quit if next block not free
         * or if no more blocks follow in permanet block
         */
        if( mcb2->size & ALLOC_FLAG )
            break;
        /*
         * coalesce mcb1 and mcb2 and remove mcb2 from free list
         */
        mcb1->size += mcb2->size;
        removeMCBfromFreeList( mcb2 );
    }
}

static void *CFastAlloc( size_t size )
/************************************/
{
    size_t      mcb_size;
    MCB         *mcb;

    size = MCB_SIZE( size );
    if( size < sizeof( MCB ) )
        size = sizeof( MCB );
    mcb_size = _RoundUp( size, MEM_ALIGN );
    /*
     * search free list before getting memory from permanent area
     */
    for( mcb = CFreeList.prev; mcb != &CFreeList; mcb = mcb->prev ) {
        Ccoalesce( mcb );
        if( mcb->size >= mcb_size ) {
            if( mcb->size - mcb_size > MCB_SIZE( 0 ) ) {
                /*
                 * block is big enough to split it
                 */
                mcb->size -= mcb_size;
                mcb = MCB_NEXT( mcb );
                mcb->size = mcb_size;
            } else {
                removeMCBfromFreeList( mcb );
            }
            mcb->size |= ALLOC_FLAG;      /* indicate block allocated */
            return( MCB_DATA( mcb, 0 ) );
        }
    }
    if( mcb_size > PermAvail )
        return( NULL );
    PermAvail -= mcb_size;
    mcb = (MCB *)( PermPtr + PermAvail );
    mcb->size = mcb_size | ALLOC_FLAG;
    return( MCB_DATA( mcb, 0 ) );
}


void *CMemAlloc( size_t size )
/****************************/
{
    void    *p;

    p = CFastAlloc( size );
    if( p == NULL ) {
        AllocPermArea();            /* allocate another permanent area block */
        p = CFastAlloc( size );
        if( p == NULL ) {
            CErr1( ERR_OUT_OF_MEMORY );
            CSuicide();
        }
    }
    /*
     * make sure pointer is properly aligned
     */
    assert( ((unsigned)(pointer_uint)p & (MEM_ALIGN - 1)) == 0 );

    return( memset( p, 0, size ) );
}

char *ToStringDup( const char *buf, size_t len )
/**********************************************/
{
    char    *str;

    str = CMemAlloc( len + 1 );
    if( str != NULL ) {
        memcpy( str, buf, len );
        str[len] = '\0';
    }
    return( str );
}

char *CMemStrdup( const char *buf )
/*********************************/
{
    if( buf != NULL ) {
        return( strcpy( CMemAlloc( strlen( buf ) + 1 ), buf ) );
    }
    return( NULL );
}

void *CMemRealloc( void *old_p, size_t size )
/*******************************************/
{
    void            *p;
    size_t          old_size;
    MCB             *mcb;

    if( old_p == NULL )
        return( CMemAlloc( size ) );

    mcb = MCB_FROM_C( old_p );
    old_size = ((mcb)->size & ~ALLOC_FLAG) - MCB_SIZE( 0 );
    if( size > old_size ) {
        p = CMemAlloc( size );
        memcpy( p, old_p, old_size );
        CMemFree( old_p );
    } else {
        /*
         * the current block is big enough -- nothing to do (very lazy realloc)
         */
        p = old_p;
    }
    return( p );
}

static enum cmem_kind CMemKind( const char *p )
/*********************************************/
{
    char            *ptr;
    size_t          size;
    mem_blk         *blk;

    ptr = PermPtr;
    size = PermSize;
    for( blk = Blks; blk != NULL; blk = blk->next ) {
        if( p > (char *)blk ) {
            /*
             * check if permanent memory (from beginning of block)
             */
            if( p < ptr ) {
                return( CMEM_PERM );
            }
            /*
             * check if dynamic memory (from end of block)
             */
            if( p < BLK_DATA( blk, size ) ) {
                return( CMEM_MEM );
            }
        }
        ptr = blk->ptr;
        size = blk->size;
    }
    return( CMEM_NONE );
}

void CMemFree( void *p )
/**********************/
{
    MCB         *mcb;

    /*
     * Should try and get rid of these error cases
     *
     * ignore precompiled header memory block
     */
    if( p != NULL && ( PCH_Start == NULL || (char *)p < PCH_Start || (char *)p >= PCH_End ) ) {
        if( CMemKind( p ) == CMEM_MEM ) {
            mcb = MCB_FROM_C( p );
            if( (char *)mcb == PermPtr + PermAvail ) {
                PermAvail += mcb->size & ~ALLOC_FLAG;
                mcb = MCB_NEXT( mcb );
                if( (mcb->size & ALLOC_FLAG) == 0 ) {
                    if( (char *)mcb == PermPtr + PermAvail ) {
                        PermAvail += mcb->size;
                        removeMCBfromFreeList( mcb );
                    }
                }
            } else {
                addMCBtoFreeList( mcb );
                Ccoalesce( mcb );
            }
        }
    }
}


void *CPermAlloc( size_t size )
/*****************************/
{
    void    *p;

    size = _RoundUp( size, MEM_ALIGN );
    if( size > PermAvail ) {
        if( size <= MAX_PERM_SIZE ) {
            /*
             * allocate another permanent area block
             */
            AllocPermArea();
        }
        if( size > PermAvail ) {
            CErr1( ERR_OUT_OF_MEMORY );
            CSuicide();
        }
    }
    PermAvail -= size;
    p = PermPtr;
    PermPtr += size;
    return( memset( p, 0, size ) );
}


void *FEmalloc( size_t size )
/***************************/
{
    void    *p;

    p = malloc( size );
    if( p == NULL ) {
        CErr1( ERR_OUT_OF_MEMORY );
        CSuicide();
    }
    return( p );
}


void FEfree( void *p )
/********************/
{
    if( p != NULL ) {
        free( p );
    }
}


int FEMoreMem( size_t size )
/**************************/
{
    /* unused parameters */ (void)size;

    return( 0 );
}
