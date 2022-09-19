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

#if defined( LONG_IS_64BITS ) || defined( _WIN64 ) || defined( __AXP__ )
    #define MEM_ALIGN   8
#else
    #define MEM_ALIGN   4
#endif

#define ALLOC_FLAG      1

#define MCB_SHIFT       MEM_ALIGN

/* Mask to get real allocation size */
#define SIZE_MASK       ~ALLOC_FLAG

#define NEXT_MCB(x)     (MCB *)((char *)(x) + ((x)->len & SIZE_MASK))
#define PTR2MCB(x)      (MCB *)((char *)(x) - MCB_SHIFT)
#define MCB2PTR(x)      (void *)((char *)(x) + MCB_SHIFT)

/* Size of permanent area. Needs to be reasonably big to satisfy
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
    char                *ptr;   // old perm pointer
    size_t              size;   // old perm size
#ifdef __AXP__
    unsigned            pad;    // padding to get quadword aligned size
#endif
} mem_blk;

/*  variables used:
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
    mem_blk *curr, *next;

    for( curr = Blks; curr != NULL; curr = next ) {
        next = curr->next;
        free( curr );
    }
    Blks = NULL;
}

static void addMCBtoFreeList( MCB *p, size_t len )
/************************************************/
{
    MCB         *pprev;
    MCB         *pnext;

    p->len = len;
    pprev = &CFreeList;
    for( ;; ) {         /* insert in sorted order */
        pnext = pprev;
        pprev = pprev->prev;
        if( pprev == &CFreeList )
            break;
        if( pprev < p ) {
            break;
        }
    }
    pnext->prev = p;
    pprev->next = p;
    p->prev = pprev;
    p->next = pnext;
}

static void removeMCBfromFreeList( MCB *p )
/*****************************************/
{
    MCB     *pprev;
    MCB     *pnext;

    pnext = p->next;
    pprev = p->prev;
    pprev->next = pnext;
    pnext->prev = pprev;
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

static void Ccoalesce( MCB *p1 )
/******************************/
{
    MCB *p2;

    for( ;; ) {
        p2 = NEXT_MCB( p1 );
        /*
         * quit if next block not free
         * or if no more blocks follow in permanet block
         */
        if( p2->len & ALLOC_FLAG )
            break;
        /* coalesce p1 and p2 and remove p2 from free list */
        p1->len += p2->len;
        removeMCBfromFreeList( p2 );
    }
}

static void *CFastAlloc( size_t size )
/************************************/
{
    size_t      amount;
    MCB         *p1;

    amount = _RoundUp( size + MCB_SHIFT, MEM_ALIGN );
    if( amount < sizeof( MCB ) )
        amount = sizeof( MCB );

/*      search free list before getting memory from permanent area */

    for( p1 = CFreeList.prev; p1 != &CFreeList; p1 = p1->prev ) {
        Ccoalesce( p1 );
        if( p1->len >= amount ) {
            if( p1->len - amount > sizeof( MCB ) ) {
                /* block is big enough to split it */
                p1->len -= amount;
                p1 = NEXT_MCB( p1 );
                p1->len = amount;
            } else {
                removeMCBfromFreeList( p1 );
            }
            p1->len |= ALLOC_FLAG;      /* indicate block allocated */
            return( MCB2PTR( p1 ) );
        }
    }
    if( amount > PermAvail )
        return( NULL );
    PermAvail -= amount;
    p1 = (MCB *)( PermPtr + PermAvail );
    p1->len = amount | ALLOC_FLAG;
    return( MCB2PTR( p1 ) );
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
    /* make sure pointer is properly aligned */
    assert( ((unsigned)(pointer_uint)p & (MEM_ALIGN - 1)) == 0 );

    return( memset( p, 0, size ) );
}


void *CMemRealloc( void *loc, size_t size )
/*****************************************/
{
    void            *p;
    MCB             *p1;
    size_t          len;

    if( loc == NULL )
        return( CMemAlloc( size ) );

    p = loc;
    p1 = PTR2MCB( loc );
    len = (p1->len & SIZE_MASK) - MCB_SHIFT;
    if( size > len ) {
        p = CMemAlloc( size );
        memcpy( p, loc, len );
        CMemFree( loc );
#if 0
    } else {
        /* the current block is big enough -- nothing to do (very lazy realloc) */
#endif
    }
    return( p );
}

static enum cmem_kind CMemKind( void *loc )
/*****************************************/
{
    char            *ptr;
    size_t          size;
    mem_blk         *blk;

    ptr = PermPtr;
    size = PermSize;
    for( blk = Blks; blk != NULL; blk = blk->next ) {
        if( (mem_blk *)loc > blk ) {
            /* check if permanent memory (from beginning of block) */
            if( (char *)loc < ptr ) {
                return( CMEM_PERM );
            }
            /* check if dynamic memory (from end of block) */
            if( (char *)loc < (char *)blk + sizeof( mem_blk ) + size ) {
                return( CMEM_MEM );
            }
        }
        ptr = blk->ptr;
        size = blk->size;
    }
    return( CMEM_NONE );
}

void CMemFree( void *loc )
/************************/
{
    size_t      len;
    MCB         *p1;

    if( loc == NULL ) { //Should try and get rid of these error cases
        return;
    }
    if( ((char *)loc >= PCH_Start) && ((char *)loc < PCH_End) ) {
        return;
    }
    switch( CMemKind( loc ) ) {
    case CMEM_PERM:
        return;
    case CMEM_MEM:
        p1 = PTR2MCB( loc );
        len = p1->len & SIZE_MASK;
        if( (char *)p1 == PermPtr + PermAvail ) {
            PermAvail += len;
            if( CFreeList.next != &CFreeList ) {
                Ccoalesce( CFreeList.next );
                p1 = CFreeList.next;
                if( (char *)p1 == PermPtr + PermAvail ) {
                    len = p1->len;
                    PermAvail += len;
                    removeMCBfromFreeList( p1 );
                }
            }
        } else {
            addMCBtoFreeList( p1, len );
            Ccoalesce( p1 );
        }
        break;
    case CMEM_NONE:
        len = 0;
        return;
    }
}


void *CPermAlloc( size_t amount )
/*******************************/
{
    char        *p;

    amount = _RoundUp( amount, MEM_ALIGN );
    if( amount > PermAvail ) {
    	if( amount <= MAX_PERM_SIZE ) {
            /* allocate another permanent area block */
            AllocPermArea();
    	}
        if( amount > PermAvail ) {
            CErr1( ERR_OUT_OF_MEMORY );
            CSuicide();
        }
    }
    PermAvail -= amount;
    p = PermPtr;
    PermPtr += amount;
    return( memset( p, 0, amount ) );
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
