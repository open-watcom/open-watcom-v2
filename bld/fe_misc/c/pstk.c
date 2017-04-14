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


#ifdef __header1
#include __header1
#endif
#include "carve.h"
#include "initdefs.h"
#include "pstk.h"

#ifndef NDEBUG
#define PSTK_BLOCK_SIZE         (4-1)   // stress the code
#else
#define PSTK_BLOCK_SIZE         (64-1)  // so each block is 64 pointers
#endif

struct pstk_blk {
    PSTK_BLK    *next;
    elem_type   data[PSTK_BLOCK_SIZE];
};

#define BLOCK_PSTK_BLK         4
static carve_t carvePSTK_BLK;

static elem_type *addBlock( PSTK_CTL *h )
{
    PSTK_BLK    *new_block;
    elem_type   *top;

    new_block = h->popped_blk;
    if( new_block == NULL ) {
        new_block = CarveAlloc( carvePSTK_BLK );
    } else {
        h->popped_blk = NULL;
    }
    new_block->next = h->top_blk;
    h->top_blk = new_block;
    top = new_block->data + PSTK_BLOCK_SIZE;
    return( top );
}

static elem_type *removeBlock( PSTK_CTL *h )
{
    elem_type   *top;
    PSTK_BLK    *old_block;
    PSTK_BLK    *last_block;
    PSTK_BLK    *curr_block;

    old_block = h->top_blk;
    curr_block = old_block->next;
    h->top_blk = curr_block;
    last_block = h->popped_blk;
    h->popped_blk = old_block;
    CarveFree( carvePSTK_BLK, last_block );
    top = NULL;
    if( curr_block != NULL ) {
        top = curr_block->data;
    }
    return( top );
}

void PstkPush( PSTK_CTL *h, elem_type e )
/***************************************/
{
    PSTK_BLK    *top_block;
    elem_type   *top;

    top = h->top_item;
    if( top == NULL ) {
        top = addBlock( h );
    } else {
        top_block = h->top_blk;
        if( top == top_block->data ) {
            top = addBlock( h );
        }
    }
    --top;
    h->top_item = top;
    *top = e;
}

elem_type *PstkPop( PSTK_CTL *h )
/*******************************/
{
    PSTK_BLK    *top_block;
    elem_type   *top;
    elem_type   *return_top;

    return_top = h->top_item;
    if( return_top != NULL ) {
        top = return_top;
        ++top;
        top_block = h->top_blk;
        if( top == top_block->data + PSTK_BLOCK_SIZE ) {
            top = removeBlock( h );
        }
        h->top_item = top;
    }
    return( return_top );
}

elem_type PstkPopElement( PSTK_CTL *h )
/*************************************/
{
    elem_type   *a_element;
    elem_type   element;

    a_element = PstkPop( h );
    if( NULL == a_element ) {
        element = NULL;
    } else {
        element = *a_element;
    }
    return element;
}

elem_type PstkTopElement( PSTK_CTL *h )
/*************************************/
{
    elem_type *a_element = PstkTop( h );
    elem_type element;
    if( NULL == a_element ) {
        element = NULL;
    } else {
        element = *a_element;
    }
    return element;
}

static int isElementPresent( const elem_type *start, const elem_type *stop, elem_type e )
{
    DbgAssert( start != stop );
    do {
        if( *start == e ) {
            return( 1 );
        }
        ++start;
    } while( start != stop );
    return( 0 );
}

int PstkContainsElement( PSTK_CTL *h, elem_type e )
/*************************************************/
{
    const elem_type *curr;
    PSTK_BLK *block;

    curr = h->top_item;
    if( curr == NULL ) {
        return( 0 );
    }
    block = h->top_blk;
    if( isElementPresent( curr, block->data + PSTK_BLOCK_SIZE, e ) ) {
        return( 1 );
    }
    for( block = block->next; block != NULL; block = block->next ) {
        if( isElementPresent( block->data + 0, block->data + PSTK_BLOCK_SIZE, e ) ) {
            return( 1 );
        }
    }
    return( 0 );
}

void PstkOpen( PSTK_CTL *h )
/**************************/
{
    h->top_blk = NULL;
    h->popped_blk = NULL;
    h->top_item = NULL;
}

void PstkClose( PSTK_CTL *h )
/***************************/
{
    PSTK_BLK *next;
    PSTK_BLK *curr;

    for( curr = h->top_blk; curr != NULL; curr = next ) {
        next = curr->next;
        CarveFree( carvePSTK_BLK, curr );
    }
    CarveFree( carvePSTK_BLK, h->popped_blk );
    DbgStmt( h->popped_blk = (PSTK_BLK *)-1 );
    DbgStmt( h->top_blk = (PSTK_BLK *)-1 );
    DbgStmt( h->top_item = (elem_type *)-1 );
}

void PstkPopAll( PSTK_CTL *h )
/****************************/
{
    PstkClose( h );
    h->popped_blk = NULL;
    h->top_blk = NULL;
    h->top_item = NULL;
}

static void pstkInit( INITFINI* defn )
{
    /* unused parameters */ (void)defn;

    carvePSTK_BLK = CarveCreate( sizeof( PSTK_BLK ), BLOCK_PSTK_BLK );
}

static void pstkFini( INITFINI* defn )
{
    /* unused parameters */ (void)defn;

#ifndef NDEBUG
    CarveVerifyAllGone( carvePSTK_BLK, "PSTK_BLK" );
#endif
    CarveDestroy( carvePSTK_BLK );
}

INITDEFN( pstk, pstkInit, pstkFini )


// PSTK_ITER SUPPORT
//
// Assumes that the PSTK is not changed while iterating
//
// usage:   PSTK_ITER iter;
//          PSTK_CTL pstk;
//          ...
//          if( PstkIterUpOpen( &iter, &pstk ) ) {
//              for( ; ; ) {
//                  void *item = PstkIterUpNext( &iter );
//                  if( NULL == item )
//                      break;
//                  ... process "item"
//              }
//          }
//
// or:      PSTK_ITER iter;
//          PSTK_CTL pstk;
//          ...
//          if( PstkIterDnOpen( &iter, &pstk ) ) {
//              for( ; ; ) {
//                  void *item = PstkIterDnNext( &iter );
//                  if( NULL == item )
//                      break;
//                  ... process "item"
//              }
//          }


int PstkIterDnOpen                  // OPEN THE DOWN ITERATOR
    ( PSTK_ITER *iter               // - the iterator
    , PSTK_CTL const *pstk )        // - the pstk
{
    elem_type *a_item;              // - addr[ next item ]

    iter->pstk = pstk;
    a_item = pstk->top_item;
    if( NULL == a_item ) {
        iter->item = NULL;
        iter->block = NULL;
        iter->top = NULL;
        iter->bot = NULL;
    } else {
        const PSTK_BLK *blk = pstk->top_blk;
        iter->block = blk;
        if( NULL == blk->next ) {
            iter->bot = blk->data + PSTK_BLOCK_SIZE - 1;
        } else {
            iter->bot = 0;
        }
        iter->top = a_item;
        iter->item = a_item - 1;
    }
    return NULL == iter->item;
}


elem_type PstkIterDnNext            // GET NEXT ITEM
    ( PSTK_ITER *iter )             // - the iterator
{
    elem_type retn;                 // - return: next item
    const elem_type *a_item;        // - addr[ next item ]

    a_item = iter->item;
    if( NULL == a_item ) {
        retn = NULL;
    } else {
        const PSTK_BLK *blk = iter->block;
        if( a_item == blk->data + PSTK_BLOCK_SIZE - 1 ) {
            if( blk->next == NULL ) {
                retn = NULL;
            } else {
                blk = blk->next;
                iter->block = blk;
                if( NULL == blk->next ) {
                    iter->bot = blk->data + PSTK_BLOCK_SIZE - 1;
                }
                a_item = blk->data;
                iter->item = a_item;
                retn = *a_item;
            }
        } else {
            ++a_item;
            iter->item = a_item;
            retn = *a_item;
        }
    }
    return retn;
}


static void setUpBlock              // SETUP UP-BLOCK
    ( PSTK_ITER *iter               // - the iterator
    , PSTK_BLK const *pred )        // - preceding block
{
    PSTK_CTL const *pstk;           // - pstk being used
    PSTK_BLK const *curr;           // - used to search blocks
    PSTK_BLK const *next;           // - next block (from the top)

    pstk = iter->pstk;
    curr = pstk->top_blk;
    if( pred == curr ) {
        iter->item = NULL;
    } else {
        iter->top = pstk->top_item;
        for( ;; ) {
            DbgAssert( curr != NULL );
            next = curr->next;
            if( next == pred )
                break;
            curr = next;
        }
        iter->item = curr->data + PSTK_BLOCK_SIZE;
        if( NULL == pred ) {
            iter->bot = curr->data + PSTK_BLOCK_SIZE - 1;
        }
        iter->block = curr;
    }
}


int PstkIterUpOpen                  // OPEN THE UP ITERATOR
    ( PSTK_ITER *iter               // - the iterator
    , PSTK_CTL const *pstk )        // - the pstk
{
    iter->pstk = pstk;
    setUpBlock( iter, 0 );
    return NULL == iter->item;
}


elem_type PstkIterUpNext            // GET NEXT ITEM
    ( PSTK_ITER *iter )             // - the iterator
{
    elem_type retn;                 // - return: next item
    const elem_type *a_item;        // - addr[ next item ]

    a_item = iter->item;
    if( NULL == a_item ) {
        retn = NULL;
    } else {
        if( a_item == iter->top ) {
            retn = NULL;
        } else {
            const PSTK_BLK * blk = iter->block;
            if( a_item == blk->data ) {
                setUpBlock( iter, blk );
                a_item = iter->item;
            }
            -- a_item;
            iter->item = a_item;
            retn = *a_item;
        }
    }
    return retn;
}
