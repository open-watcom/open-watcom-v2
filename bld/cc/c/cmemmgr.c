/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2022 The Open Watcom Contributors. All Rights Reserved.
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


/*
 * RISC platforms are likely to take a big hit for misaligned accesses.
 * Memory blocks are ordinarily aligned to sizeof( unsigned ) which is
 * OK for many platforms, but notably on Alpha we need 8-byte alignment
 * to make sure doubles won't get us in trouble.
 * NB: MEM_ALIGN must be at least int-sized
 */

#if defined( LONG_IS_64BITS ) || defined( _WIN64 )
    #define MEM_ALIGN   8
#elif defined( __AXP__ )
    #define MEM_ALIGN   8
#else
    #define MEM_ALIGN   4
#endif

#define ALLOC_FLAG      1

#define MCB_SHIFT       MEM_ALIGN

/*
 * Mask to get real allocation size
 */
#define SIZE_MASK       ~ALLOC_FLAG

#define NEXT_MCB(x)     ((MCB *)((char *)(x) + ((x)->len & SIZE_MASK)))
#define PTR2MCB(x)      ((MCB *)((char *)(x) - MCB_SHIFT))
#define MCB2PTR(x)      ((void *)((char *)(x) + MCB_SHIFT))

#define PTR2MCB_SIZE(x) ((x) + MCB_SHIFT)
#define MCB2PTR_SIZE(x) ((x) - MCB_SHIFT)

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
    size_t              len;    /* length of stg */
    struct mem_block    *prev;  /* pointer to previous free memory block */
    struct mem_block    *next;  /* pointer to next     free memory block */
} MCB;

typedef struct mem_blk {
    struct mem_blk      *next;
    char                *ptr;   // old permanent area block pointer
    size_t              size;   // old permanent area block size
#ifdef __AXP__
    unsigned            pad;    // padding to get quadword aligned size
#endif
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

static void addMCBtoFreeList( MCB *mcb, size_t mcb_len )
/******************************************************/
{
    MCB     *mcb_prev;
    MCB     *mcb_next;

    mcb->len = mcb_len;
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
    char    *perm_area;

    perm_area = NULL;
    for( PermSize = MAX_PERM_SIZE; PermSize != 0; PermSize -= 0x20 ) {
        mem_blk *blk;
        blk = calloc( 1, sizeof( mem_blk ) + PermSize + sizeof( size_t ) );
        if( blk != NULL ) {
            blk->next = Blks;
            blk->ptr = PermPtr;
            blk->size = PermSize;
            Blks = blk;
            perm_area = (char *)blk + sizeof( mem_blk );
            ((MCB *)(perm_area + PermSize))->len = PERM_BLK_END;    /* null length tag */
            break;
        }
    }
    PermPtr = perm_area;
    PermAvail = PermSize;
}


void CMemInit( void )
/*******************/
{
    InitPermArea();
    CFreeList.len = 0;
    CFreeList.next = &CFreeList;
    CFreeList.prev = &CFreeList;
}

void CMemFini( void )
/*******************/
{
    CFreeList.len = 0;
    CFreeList.next = &CFreeList;
    CFreeList.prev = &CFreeList;
    FiniPermArea();
}

static void Ccoalesce( MCB *mcb1 )
/********************************/
{
    MCB *mcb2;

    for( ;; ) {
        mcb2 = NEXT_MCB( mcb1 );
        /*
         * quit if next block not free
         * or if no more blocks follow in permanet block
         */
        if( mcb2->len & ALLOC_FLAG )
            break;
        /*
         * coalesce mcb1 and mcb2 and remove mcb2 from free list
         */
        mcb1->len += mcb2->len;
        removeMCBfromFreeList( mcb2 );
    }
}

static void *CFastAlloc( size_t size )
/************************************/
{
    size_t      mcb_len;
    MCB         *mcb;

    mcb_len = _RoundUp( PTR2MCB_SIZE( size ), MEM_ALIGN );
    if( mcb_len < sizeof( MCB ) )
        mcb_len = sizeof( MCB );
    /*
     * search free list before getting memory from permanent area
     */
    for( mcb = CFreeList.prev; mcb != &CFreeList; mcb = mcb->prev ) {
        Ccoalesce( mcb );
        if( mcb->len >= mcb_len ) {
            if( mcb->len - mcb_len > sizeof( MCB ) ) {
                /*
                 * block is big enough to split it
                 */
                mcb->len -= mcb_len;
                mcb = NEXT_MCB( mcb );
                mcb->len = mcb_len;
            } else {
                removeMCBfromFreeList( mcb );
            }
            mcb->len |= ALLOC_FLAG;      /* indicate block allocated */
            return( MCB2PTR( mcb ) );
        }
    }
    if( mcb_len > PermAvail )
        return( NULL );
    PermAvail -= mcb_len;
    mcb = (MCB *)( PermPtr + PermAvail );
    mcb->len = mcb_len | ALLOC_FLAG;
    return( MCB2PTR( mcb ) );
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


void *CMemRealloc( void *old_p, size_t size )
/*******************************************/
{
    void            *p;
    size_t          old_size;

    if( old_p == NULL )
        return( CMemAlloc( size ) );

    p = old_p;
    old_size = MCB2PTR_SIZE( PTR2MCB( old_p )->len & SIZE_MASK );
    if( size > old_size ) {
        p = CMemAlloc( size );
        memcpy( p, old_p, old_size );
        CMemFree( old_p );
#if 0
    } else {
        /*
         * the current block is big enough -- nothing to do (very lazy realloc)
         */
#endif
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
            if( p < ( (char *)blk + sizeof( mem_blk ) + size ) ) {
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
    size_t      mcb_len;
    MCB         *mcb;

    /*
     * Should try and get rid of these error cases
     *
     * ignore precompiled header memory block
     */
    if( p != NULL && ( PCH_Start == NULL || (char *)p < PCH_Start || (char *)p >= PCH_End ) ) {
        if( CMemKind( (char *)p ) == CMEM_MEM ) {
            mcb = PTR2MCB( p );
            mcb_len = mcb->len & SIZE_MASK;
            if( (char *)mcb == PermPtr + PermAvail ) {
                PermAvail += mcb_len;
                mcb = (MCB *)( (char *)mcb + mcb_len );
                if( (mcb->len & ALLOC_FLAG) == 0 ) {
                    if( (char *)mcb == PermPtr + PermAvail ) {
                        PermAvail += mcb->len;
                        removeMCBfromFreeList( mcb );
                    }
                }
            } else {
                addMCBtoFreeList( mcb, mcb_len );
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
