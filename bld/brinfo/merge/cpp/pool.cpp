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


#include "wpch.hpp"
#include "pool.hpp"
#include "debugprt.hpp"


#if 0
#ifdef DEBUG_PRT
static char const *PoolNames[] = {      // for debugging
    "AvlLinkPool",
    "AvlStackPool",
    "DeclPool",
    "DefnPool",
    "HashLinkPool",
    "LListBlockPool",
    "U32PairPool",
    "ScopePool",
    "StrLinkPool",
    "TypePool",
    "UsagePool"
};
#endif
#endif


#define NEXT(p,type) (*((type **) (p)))

//
// Pool::Pool   --Default constructor.
//

static const unsigned Pool::BLOCK_LEN = 512;

Pool::Pool( PoolIds id, size_t size, unsigned b_len )
        : _size( size > sizeof(void*) ? size : sizeof(void*) )
        , _blockLen( b_len ? b_len : BLOCK_LEN )
        , _idNum( id )
/************************************************************/
{
    _array = NULL;
    _freeBlocks = NULL;
    _pfree = NULL;
    _blockCount = 0;
}


//
// Pool::~Pool  --Destructor.
//

Pool::~Pool()
/***********/
{
    uint_8      *temp;

    while( _array != NULL ){
        temp = _array;
        _array = NEXT( _array + _blockLen*_size, uint_8 );
        delete[] temp;
    }
    while( _freeBlocks != NULL ){
        temp = _freeBlocks;
        _freeBlocks = NEXT( _freeBlocks + _blockLen*_size, uint_8 );
        delete[] temp;
    }
}


//
// Pool::Get    --General purpose allocator.
//

void *Pool::Get()
/***************/
{
    void        *result;

    result = _pfree;
    if( result == NULL ){
        uint_8  *index;
        unsigned limit;
        uint_8  *temp;

        if( _freeBlocks != NULL ){
            temp = _freeBlocks;
            _freeBlocks = NEXT( _freeBlocks + _blockLen*_size, uint_8 );
            NEXT( temp + _blockLen*_size, uint_8 ) = _array;
        } else {
            limit = _blockLen * _size;
            temp = new uint_8[ limit + sizeof(uint_8 *) ];
            limit -= _size;
            for( index=temp; index<temp+limit; index+=_size ){
                NEXT( index, void ) = (void *) (index + _size);
            }
            NEXT( index, void ) = NULL;
            NEXT( index+_size, uint_8 ) = _array;
        }

        result = (void *) (_array = temp);
        _blockCount++;
    }
    _pfree = NEXT( result, void );
    return result;
}


//
//  Pool::Release       --General purpose de-allocator.
//

void Pool::Release( void * p )
/****************************/
{
    NEXT( p, void ) = _pfree;
    _pfree = p;
}


//
//  partition   --Partition a linked list.  Used by sortList.
//

static void partition( void *&start, void *pivot, void *& after)
{
    void        *left, *right, *current;

    left = NULL;
    right = NULL;
    current = start;
    start = NULL;
    after = NULL;
    while( current != NULL ){
        if( current < pivot ){
            if( left == NULL ){
                start = current;
                left = current;
             } else {
                 NEXT( left, void ) = current;
                 left = current;
             }
        } else {
            if( right == NULL ){
                after = current;
                right = current;
            } else {
                NEXT( right, void ) = current;
                right = current;
            }
        }
        current = NEXT( current, void );
    }

    if( left != NULL ){
        NEXT( left, void ) = NULL;
    }
    if( right != NULL ){
        NEXT( right, void ) = NULL;
    }
    return;
}


//
//  sortList    --QuickSort a linked list.  (No, I couldn't use an array.)
//

static void  sortList( void *left, void *& first, void *& last )
{
    void        *right, *m1, *m2, *pivot;

    pivot = left;
    m1 = NULL;

    // We choose the first out-of-order element from the front of the
    // list as the pivot.  This lets us check to see if the list is
    // in fact sorted to begin with.
    while( pivot != NULL ){
        if( pivot > NEXT( pivot, void ) ){
            break;
        }
        m1 = pivot;
        pivot = NEXT( pivot, void );
    }
    if( pivot == NULL || NEXT( pivot, void ) == NULL ){
        first = left; last = pivot;
        return;
    }

    // Remove the pivot element from the list before partitioning.
    // This guarantees that the algorithm will eventually terminate.
    if( m1 != NULL ){
        NEXT( m1, void ) = NEXT( pivot, void );
    } else {
        left = NEXT( pivot, void );
    }

    // Partition the lists, sort the sublists, and join everything back
    // together in the proper order.
    partition( left, pivot, right );
    if( left == NULL ){
        sortList( right, first, last );
        NEXT( pivot, void ) = first;
        first = pivot;
    } else if( right == NULL ){
        sortList( left, first, last );
        NEXT( last, void ) = pivot;
        NEXT( pivot, void ) = NULL;
        last = pivot;
    } else {
        sortList( left, first, m1 );
        sortList( right, m2, last );
        NEXT( m1, void ) = pivot;
        NEXT( pivot, void ) = m2;
    }
    return;
}


static inline void * voidInc( void *p, unsigned val )
{
    return (void *) ((uint_8 *) p + val);
}


void Pool::CleanUp( Severity s )
/******************************/
{
    // Step 1: organize the _pfree list
    {
        void    *dummy;

        sortList( _pfree, _pfree, dummy );
    }
    if( s == Low ){
        goto done;
    }

    // Step 2: collect unused blocks in the _freeBlocks list.
    {
        void    *current, *prev, *start, *finish;
        uint_8  *curBlock, *prevBlock;

        #if 0
        IF_DEBUG_PRT(
            DebugPrinter.DebugMsg("%s collecting blocks.",PoolNames[_idNum]);
        )
        #endif

        // A "chain" of entries in the sorted _pfree list is a sequence
        // of >1 consecutive entries which are adjacent in memory.
        // A chain of length _blockLen must correspond to an entry in the
        // _array list.  Find all such chains in the _pfree list and
        // move the corresponding entries from _array to _freeBlocks.

        prev = NULL;
        current = _pfree;
        while( current != NULL && NEXT( current, void ) != NULL ){

            if( NEXT( current, void ) != voidInc( current, _size ) ){

                // This is not the start of a chain, so continue.

                prev = current;
                current = NEXT( current, void );

            } else {

                // This is the start of a chain.  So find the end.

                start = current;
                current = NEXT( current, void );
                while( NEXT( current, void ) != NULL ){
                    if( NEXT( current, void ) != voidInc( current, _size ) ){
                        break;
                    }
                    current = NEXT( current, void );
                }
                finish = current;
                current = NEXT( current, void );

                // If the chain is _blockLen units long, it must correspond
                // to an entry in the _array list.

                if( finish == voidInc( start, (_blockLen-1)*_size ) ){

                    // Remove the entries from _pfree.

                    NEXT( finish, void ) = NULL;
                    if( prev != NULL ){
                        NEXT( prev, void ) = current;
                    } else {
                        _pfree = current;
                    }

                    // Find the corresponding entry in _array.

                    prevBlock = NULL;
                    curBlock = _array;
                    while( curBlock != NULL ){
                        if( (void *) curBlock == start ){
                            break;
                        }
                        prevBlock = curBlock;
                        curBlock = NEXT( curBlock + _blockLen*_size, uint_8 );
                    }

                    // Move the entry from _array to _freeBlocks.

                    WAssert( curBlock != NULL );
                    if( prevBlock != NULL ){
                        NEXT( prevBlock+_blockLen*_size, uint_8 ) =
                            NEXT( curBlock + _blockLen*_size, uint_8 );
                    } else {
                        _array = NEXT( curBlock + _blockLen*_size, uint_8 );
                    }
                    NEXT( curBlock + _blockLen*_size, uint_8 ) = _freeBlocks;
                    _freeBlocks = curBlock;
                    _blockCount--;

                } else {

                    // This chain was too short, keep searching.

                    prev = finish;

                }
            }
        }
    }
    if( s == Medium ){
        goto done;
    }

    // Step 3: delete the unused blocks
    {
        uint_8  *current, *prev;

        prev = NULL;
        current = _freeBlocks;
        while( current != NULL ){
            prev = current;
            current = NEXT( current + _blockLen*_size, uint_8 );
            delete[] prev;
        }
        _freeBlocks = NULL;
    }
done:
    return;
}


unsigned Pool::MemUsed()
/**********************/
{
    return (_blockLen*_size+sizeof(uint_8))*_blockCount;
}
