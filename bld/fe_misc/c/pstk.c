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


#include header
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
    void const  *data[PSTK_BLOCK_SIZE];
};

#define BLOCK_PSTK_BLK         4
static carve_t carvePSTK_BLK;

static void const **addBlock( PSTK_CTL *h )
{
    PSTK_BLK *new_block;
    void const **top;

    new_block = h->popped_blk;
    if( new_block == NULL ) {
        new_block = CarveAlloc( carvePSTK_BLK );
    } else {
        h->popped_blk = NULL;
    }
    new_block->next = h->top_blk;
    h->top_blk = new_block;
    top = &(new_block->data[PSTK_BLOCK_SIZE]);
    return( top );
}

static void const **removeBlock( PSTK_CTL *h )
{
    void const **top;
    PSTK_BLK *old_block;
    PSTK_BLK *last_block;
    PSTK_BLK *curr_block;

    old_block = h->top_blk;
    curr_block = old_block->next;
    h->top_blk = curr_block;
    last_block = h->popped_blk;
    h->popped_blk = old_block;
    CarveFree( carvePSTK_BLK, last_block );
    top = NULL;
    if( curr_block != NULL ) {
        top = &(curr_block->data[0]);
    }
    return( top );
}

void PstkPush( PSTK_CTL *h, void const *e )
/*****************************************/
{
    PSTK_BLK *top_block;
    void const **top;

    top = h->top_item;
    if( top == NULL ) {
        top = addBlock( h );
    } else {
        top_block = h->top_blk;
        if( top == (void const **) &(top_block->data[0]) ) {
            top = addBlock( h );
        }
    }
    --top;
    h->top_item = top;
    *top = e;
}

void *PstkPop( PSTK_CTL *h )
/**************************/
{
    PSTK_BLK *top_block;
    void const **top;
    void const **return_top;

    return_top = h->top_item;
    if( return_top != NULL ) {
        top = return_top;
        ++top;
        top_block = h->top_blk;
        if( top == (void const **) &(top_block->data[PSTK_BLOCK_SIZE]) ) {
            top = removeBlock( h );
        }
        h->top_item = top;
    }
    return( return_top );
}

void* PstkPopElement( PSTK_CTL *h )
/*********************************/
{
    void **a_element = PstkPop( h );
    void *element;
    if( NULL == a_element ) {
        element = NULL;
    } else {
        element = *a_element;
    }
    return element;
}

void *PstkTopElement( PSTK_CTL *h )
/*********************************/
{
    void const **a_element = PstkTop( h );
    void *element;
    if( NULL == a_element ) {
        element = NULL;
    } else {
        element = (void*) *a_element;
    }
    return element;
}

static void const *isElementPresent( void const **start, void const **stop, void const *e )
{
    DbgAssert( start != stop );
    do {
        if( *start == e ) {
            return( e );
        }
        ++start;
    } while( start != stop );
    return( NULL );
}

void *PstkContainsElement( PSTK_CTL *h, void const *e )
/*****************************************************/
{
    void const **curr;
    void const **stop;
    PSTK_BLK *block;

    curr = h->top_item;
    if( curr == NULL ) {
        return( NULL );
    }
    block = h->top_blk;
    stop = (const void **) &(block->data[PSTK_BLOCK_SIZE]);
    if( isElementPresent( curr, stop, e ) != NULL ) {
        return( (void*) e );
    }
    for( block = block->next; block != NULL; block = block->next ) {
        curr = (const void **) &(block->data[0]);
        stop = (const void **) &(block->data[PSTK_BLOCK_SIZE]);
        if( isElementPresent( curr, stop, e ) != NULL ) {
            return( (void*) e );
        }
    }
    return( NULL );
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
    DbgStmt( h->popped_blk = (void*)-1 );
    DbgStmt( h->top_blk = (void*)-1 );
    DbgStmt( h->top_item = (void*)-1 );
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
    defn = defn;
    carvePSTK_BLK = CarveCreate( sizeof( PSTK_BLK ), BLOCK_PSTK_BLK );
}

static void pstkFini( INITFINI* defn )
{
    defn = defn;
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
//                  void* item = PstkIterUpNext( &iter );
//                  if( NULL == item ) break;
//                  ... process "item"
//              }
//          }
//
// or:      PSTK_ITER iter;
//          PSTK_CTL pstk;
//          ...
//          if( PstkIterDnOpen( &iter, &pstk ) ) {
//              for( ; ; ) {
//                  void* item = PstkIterDnNext( &iter );
//                  if( NULL == item ) break;
//                  ... process "item"
//              }
//          }


int PstkIterDnOpen                  // OPEN THE DOWN ITERATOR
    ( PSTK_ITER* iter               // - the iterator
    , PSTK_CTL const *pstk )        // - the pstk
{
    void const * const* a_item;     // - addr[ next item ]

    iter->pstk = pstk;
    a_item = pstk->top_item;
    if( NULL == a_item ) {
        iter->item = NULL;
        iter->block = NULL;
        iter->top = NULL;
        iter->bot = NULL;
    } else {
        PSTK_BLK const * blk = pstk->top_blk;
        iter->block = blk;
        if( NULL == blk->next ) {
            iter->bot = &blk->data[ PSTK_BLOCK_SIZE - 1 ];
        } else {
            iter->bot = 0;
        }
        iter->top = a_item;
        iter->item = a_item - 1;
    }
    return NULL == iter->item;
}


void* PstkIterDnNext                // GET NEXT ITEM
    ( PSTK_ITER* iter )             // - the iterator
{
    void* retn;                     // - return: next item
    void const * const* a_item;     // - addr[ next item ]

    a_item = iter->item;
    if( NULL == a_item ) {
        retn = NULL;
    } else {
        PSTK_BLK const * blk = iter->block;
        if( a_item == &blk->data[ PSTK_BLOCK_SIZE - 1 ] ) {
            if( blk->next == NULL ) {
                retn = NULL;
            } else {
                blk = blk->next;
                iter->block = blk;
                if( NULL == blk->next ) {
                    iter->bot = &blk->data[ PSTK_BLOCK_SIZE - 1 ];
                }
                a_item = &blk->data[0];
                iter->item = a_item;
                retn = (void*) *a_item;
            }
        } else {
            ++ a_item;
            iter->item = a_item;
            retn = (void*) *a_item;
        }
    }
    return retn;
}


static void setUpBlock              // SETUP UP-BLOCK
    ( PSTK_ITER* iter               // - the iterator
    , PSTK_BLK const * pred )       // - preceding block
{
    PSTK_CTL const * pstk;          // - pstk being used
    PSTK_BLK const * curr;          // - used to search blocks
    PSTK_BLK const * next;          // - next block (from the top)

    pstk = iter->pstk;
    curr = pstk->top_blk;
    if( pred == curr ) {
        iter->item = NULL;
    } else {
        iter->top = pstk->top_item;
        for( ; ; ) {
            DbgAssert( curr != NULL );
            next = curr->next;
            if( next == pred ) break;
            curr = next;
        }
        iter->item = &curr->data[ PSTK_BLOCK_SIZE ];
        if( NULL == pred ) {
            iter->bot = &curr->data[ PSTK_BLOCK_SIZE - 1 ];
        }
        iter->block = curr;
    }
}


int PstkIterUpOpen                  // OPEN THE UP ITERATOR
    ( PSTK_ITER* iter               // - the iterator
    , PSTK_CTL const *pstk )        // - the pstk
{
    iter->pstk = pstk;
    setUpBlock( iter, 0 );
    return NULL == iter->item;
}


void* PstkIterUpNext                // GET NEXT ITEM
    ( PSTK_ITER* iter )             // - the iterator
{
    void* retn;                     // - return: next item
    void const * const* a_item;     // - addr[ next item ]

    a_item = iter->item;
    if( NULL == a_item ) {
        retn = NULL;
    } else {
        if( a_item == iter->top ) {
            retn = NULL;
        } else {
            PSTK_BLK const * blk = iter->block;
            if( a_item == &blk->data[ 0 ] ) {
                setUpBlock( iter, blk );
                a_item = iter->item;
            }
            -- a_item;
            iter->item = a_item;
            retn = (void*) *a_item;
        }
    }
    return retn;
}
