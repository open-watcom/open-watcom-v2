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
* Description:  Implement an indexed stack of items.
*
****************************************************************************/


#ifdef __header1
#include __header1
#endif
#include <stddef.h>
#include "vstk.h"


static VSTK_BLK *freeVstkBlk(   // FREE A VSTK_BLK
    VSTK_CTL *stack )           // - control for stack
{
    VSTK_BLK *blk;              // - block to be freed
    VSTK_BLK *last;             // - block following freed block

    blk = stack->top;
    last = blk->last;
    blk->last = stack->freed;
    stack->freed = blk;
    stack->top = last;
    return( last );
}


static unsigned vstkDataSize(   // COMPUTE SIZE OF DATA AREA IN BLOCK
    VSTK_CTL const *stack )     // - control for stack
{
    return( stack->per_block * stack->size );
}


static bool vstkInBlk(          // TEST IF ENTRY IS WITHIN A BLOCK
    VSTK_BLK const *blk,        // - the block
    void *curr,                 // - current entry
    unsigned block_size )       // - size of a block
{
    bool    retb;               // - true ==> is within the block

    if( curr == NULL ) {
        retb = false;
    } else if( curr < (void *)&blk->data[0] ) {
        retb = false;
    } else if( curr >= (void *)&blk->data[ block_size ] ) {
        retb = false;
    } else {
        retb = true;
    }
    return( retb );
}

static void *vstkPushBlk(       // PUSH THE BLOCK
    VSTK_CTL *stack )           // - stack to be pushed
{
    VSTK_BLK *blk;              // - current block
    void *cur;                  // - current entry
    unsigned size;              // - size of stacking area

    blk = stack->freed;
    size = vstkDataSize( stack );
    if( blk == NULL ) {
        blk = _MemoryAllocate( size + offsetof( VSTK_BLK, data ) );
    } else {
        stack->freed = blk->last;
    }
    blk->last = stack->top;
    stack->top = blk;
    cur = &blk->data[ size ];
    return( cur );
}

#ifndef NDEBUG
static void _VstkIntegrity( VSTK_CTL const *stack )
{
    if( stack->current != NULL ) {
        if( !vstkInBlk( stack->top, stack->current, vstkDataSize( stack ) ) ) {
            _FatalAbort( "vstk: curr is not in top blk" );
        }
    }
}
static void _VstkPushZapPop( VSTK_CTL *stack )
{
    VSTK_BLK *blk;              // - current block
    void *cur;                  // - current entry
    unsigned size;              // - block size

    // NYI: zap on block transitions
    blk = stack->top;
    if( blk != NULL ) {
        cur = stack->current;
        if( cur != NULL ) {
            if( cur != blk->data ) {
                size = stack->size;
                cur = (char *)cur - size;
                DbgZapFreed( cur, size );
            }
        }
    }
}
#else
#define _VstkIntegrity(__stack)
#define _VstkPushZapPop(__stack)
#endif


void *VstkPush(                 // PUSH THE STACK
    VSTK_CTL *stack )           // - stack to be pushed
{
    VSTK_BLK *blk;              // - current block
    void *cur;                  // - current entry

    _VstkIntegrity( stack );
    if( stack->top == NULL ) {
        cur = vstkPushBlk( stack );
    } else {
        cur = stack->current;
        if( cur == NULL ) {
            cur = vstkPushBlk( stack );
        } else {
            blk = stack->top;
            if( cur == blk->data ) {
                cur = vstkPushBlk( stack );
            }
        }
    }
    cur = (char *)cur - stack->size;
    stack->current = cur;
    _VstkIntegrity( stack );
    DbgZapAlloc( cur, stack->size );
    return( cur );
}


void *VstkTop(                  // POINT AT TOP OF STACK
    VSTK_CTL const *stack )     // - stack to be used
{
    _VstkIntegrity( stack );
    return( stack->current );
}


void *VstkPop(                  // POP THE STACK
    VSTK_CTL *stack )           // - stack to be used
{
    void *retn;                 // - stacked entry
    void *cur;                  // - current entry
    VSTK_BLK *blk;              // - current block
    VSTK_BLK *lst;              // - last block

    _VstkIntegrity( stack );
    _VstkPushZapPop( stack );
    retn = stack->current;
    if( retn != NULL ) {
        cur = retn;
        blk = stack->top;
        if( cur == &blk->data[ vstkDataSize( stack ) - stack->size ] ) {
            lst = freeVstkBlk( stack );
            if( lst == NULL ) {
                cur = NULL;
            } else {
                cur = lst->data;
            }
        } else {
            cur = (char *)cur + stack->size;
        }
        stack->current = cur;
        _VstkIntegrity( stack );
    }
    return( retn );
}


void VstkOpen(                  // OPEN THE VIRTUAL STACK
    VSTK_CTL *stack,            // - stack to be opened
    unsigned size,              // - size of an element
    unsigned count )            // - number per block
{
    stack->current = NULL;
    stack->top = NULL;
    stack->freed = NULL;
    stack->per_block = count;
    stack->size = size;
    _VstkIntegrity( stack );
}


static void vstkFreeBlk(        // FREE LIST OF BLOCKS
    VSTK_BLK *cur )             // - list to be freed
{
    VSTK_BLK *lst;              // - last block

    for( ; cur != NULL; ) {
        lst = cur;
        cur = cur->last;
        _MemoryFree( lst );
    }
}


void VstkClose(                 // CLOSE THE VIRTUAL STACK
    VSTK_CTL *stack )           // - stack to be closed
{
    _VstkIntegrity( stack );
    vstkFreeBlk( stack->top );
    vstkFreeBlk( stack->freed );
}


void *VstkIndex(                // INDEX INTO A VIRTUAL STACK
    VSTK_CTL *stack,            // - stack to be indexed
    unsigned index )            // - the index
{
    unsigned reqd;              // - required number of blocks
    unsigned blks;              // - number of blocks
    VSTK_BLK *blk;              // - current block
    unsigned per;               // - # elements per block
    void *retn;                 // - item indexed
    bool blk_pushed;            // - true ==> a block was added
    bool on_top;                // - true ==> retn in top block

    _VstkIntegrity( stack );
    blks = 0;
    for( blk = stack->top; blk != NULL; blk = blk->last )
        ++blks;
    per = stack->per_block;
    reqd = index / per + 1;
    blk_pushed = false;
    for( ; blks < reqd; ++blks ) {
        blk_pushed = true;
        vstkPushBlk( stack );
    }
    on_top = true;
    blk = stack->top;
    for( index = blks * per - index; index > per; index -= per ) {
        on_top = false;
        blk = blk->last;
    }
    retn = &blk->data[( index - 1 ) * stack->size];
    if( blk_pushed || ( stack->current == NULL ) || ( on_top && ( retn < stack->current ) ) ) {
        stack->current = retn;
    }
    _VstkIntegrity( stack );
    return( retn );
}


unsigned VstkDimension(         // GET UPPER DIMENSION OF VIRTUAL STACK
    VSTK_CTL const *stack )     // - stack
{
    VSTK_BLK *blk;              // - current block
    unsigned dimension;         // - dimension
    unsigned per_block;         // - # entries per block

    _VstkIntegrity( stack );
    blk = stack->top;
    dimension = 0;
    if( blk != NULL ) {
        dimension -= ( (char*)stack->current - blk->data ) / stack->size;
        per_block = stack->per_block;
#ifndef NDEBUG
        if( dimension + per_block > per_block ) {
            _FatalAbort( "vstk: dimension > per_block" );
        }
#endif
        for( ; blk != NULL; blk = blk->last ) {
            dimension += per_block;
        }
        _VstkIntegrity( stack );
    }
    return( dimension );
}


void *VstkNext(                 // GET NEXT ITEM IN STACK
    VSTK_CTL const *stack,      // - stack being walked
    void *cur )                 // - current item
{
    VSTK_BLK *blk;              // - current block
    unsigned blk_size;          // - size of a block

    _VstkIntegrity( stack );
    blk_size = vstkDataSize( stack );
    for( blk = stack->top; blk != NULL; blk = blk->last ) {
        if( vstkInBlk( blk, cur, blk_size ) ) {
            // current item is found
            cur = (char *)cur + stack->size;
            if( cur >= (void *)&blk->data[blk_size] ) {
                blk = blk->last;
                if( blk == NULL ) {
                    cur = NULL;
                } else {
                    cur = blk->data;
                }
            }
            return( cur );
        }
    }
    // current item is not found
    if( stack->top == NULL )
        return( NULL );
    return( stack->top->data );
}


void *VstkBase(                 // GET BASE ELEMENT
    VSTK_CTL *stack,            // - stack to be based
    unsigned base )             // - the base index
{
    void *cur;                  // - current element

    if( base == 0 ) {
        cur = NULL;
    } else if( base >= VstkDimension( stack ) ) {
        cur = VstkTop( stack );
    } else {
        cur = VstkIndex( stack, base - 1 );
    }
    return( cur );
}


void VstkTruncate(              // TRUNCATE A VSTK
    VSTK_CTL *stack,            // - stack to be truncated
    unsigned base )             // - the truncation index
{
    void *curr;                 // - current item
    VSTK_BLK *last;             // - last allocated block
    unsigned blk_size;          // - # data bytes in a block

    _VstkIntegrity( stack );
    if( base == 0 ) {
        curr = NULL;
    } else {
        curr = VstkIndex( stack, base - 1 );
    }
    blk_size = vstkDataSize( stack );
    while( (last = stack->top) != NULL ) {
        if( vstkInBlk( last, curr, blk_size ) )
            break;
        freeVstkBlk( stack );
    }
    stack->current = curr;
    _VstkIntegrity( stack );
}
