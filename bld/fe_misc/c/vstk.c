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

#ifndef TRUE
#   define TRUE  1
#   define FALSE 0
#endif

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


static int vstkInBlk(           // TEST IF ENTRY IS WITHIN A BLOCK
    VSTK_BLK const *blk,        // - the block
    void *curr,                 // - current entry
    unsigned block_size )       // - size of a block
{
    int retn;                   // - TRUE ==> is within the block

    if( curr == NULL ) {
        retn = FALSE;
    } else if( curr < (void *)&blk->data[0] ) {
        retn = FALSE;
    } else if( curr >= (void *)&blk->data[ block_size ] ) {
        retn = FALSE;
    } else {
        retn = TRUE;
    }
    return retn ;
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
void _VstkIntegrity( VSTK_CTL const *stack )
{
    if( stack->current == NULL ) return;
    if( !vstkInBlk( stack->top, stack->current, vstkDataSize( stack ) ) ) {
        _FatalAbort( "vstk: curr is not in top blk" );
    }
}
void _VstkPushZapPop( VSTK_CTL *stack )
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
    int index )                 // - the index
{
    unsigned reqd;              // - required number of blocks
    unsigned blks;              // - number of blocks
    VSTK_BLK *blk;              // - current block
    unsigned per;               // - # elements per block
    void *retn;                 // - item indexed
    int blk_pushed;             // - TRUE ==> a block was added
    int on_top;                 // - TRUE ==> retn in top block

    _VstkIntegrity( stack );
    if( index < 0 ) {
        retn = NULL;
    } else {
        blks = 0;
        for( blk = stack->top; blk != NULL; ++blks, blk = blk->last );
        per = stack->per_block;
        reqd = index / per + 1;
        blk_pushed = FALSE;
        for( ; blks < reqd; ++blks ) {
            blk_pushed = TRUE;
            vstkPushBlk( stack );
        }
        index = blks * per - index;
        for( blk = stack->top, on_top = TRUE;
             index > per;
             index -= per, blk = blk->last, on_top = FALSE );
        retn = &blk->data[ ( index - 1 ) * stack->size ];
        if( ( blk_pushed )
          ||( stack->current == NULL )
          ||( on_top && ( retn < stack->current ) ) ) {
            stack->current = retn;
        }
        _VstkIntegrity( stack );
    }
    return( retn );
}


int VstkDimension(              // GET UPPER DIMENSION OF VIRTUAL STACK
    VSTK_CTL const *stack )     // - stack
{
    VSTK_BLK *blk;              // - current block
    int dimension;              // - dimension
    int per_block;              // - # entries per block

    _VstkIntegrity( stack );
    blk = stack->top;
    if( blk == NULL ) {
        dimension = 0;
    } else {
        dimension = - ( (char*)stack->current - blk->data ) / stack->size;
        per_block = stack->per_block;
#ifndef NDEBUG
        if( dimension > per_block ) {
            _FatalAbort( "vstk: dimension > per_block" );
        }
#endif
        for( ; blk != NULL; blk = blk->last ) {
            dimension += per_block;
        }
        _VstkIntegrity( stack );
    }
    return( dimension - 1 );
}


void *VstkNext(                 // GET NEXT ITEM IN STACK
    VSTK_CTL const *stack,      // - stack being walked
    void *cur )                 // - current item
{
    VSTK_BLK *blk;              // - current block
    unsigned blk_size;          // - size of a block

    _VstkIntegrity( stack );
    blk_size = vstkDataSize( stack );
    for( blk = stack->top; ; blk = blk->last ) {
        if( blk == NULL ) {
            blk = stack->top;
            if( blk == NULL ) {
                cur = NULL;
            } else {
                cur = &blk->data;
            }
            break;
        }
        if( vstkInBlk( blk, cur, blk_size ) ) {
            cur = (char *)cur + stack->size;
            if( cur >= (void *)&blk->data[blk_size] ) {
                blk = blk->last;
                if( blk == NULL ) {
                    cur = NULL;
                } else {
                    cur = blk->data;
                }
            }
            break;
        }
    }
    return( cur );
}


void *VstkBase(                 // GET BASE ELEMENT
    VSTK_CTL *stack,            // - stack to be based
    int base )                  // - the base index
{
    void *cur;                  // - current element

    --base;
    if( base >= VstkDimension( stack ) ) {
        cur = VstkTop( stack );
    } else {
        cur = VstkIndex( stack, base );
    }
    return( cur );
}


void VstkTruncate(              // TRUNCATE A VSTK
    VSTK_CTL *stack,            // - stack to be truncated
    int base )                  // - the truncation index
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
    for(;;) {
        last = stack->top;
        if( last == NULL ) break;
        if( vstkInBlk( last, curr, blk_size ) ) break;
        freeVstkBlk( stack );
    }
    stack->current = curr;
    _VstkIntegrity( stack );
}
