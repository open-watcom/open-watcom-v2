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
#include "hashtbl.hpp"

static Pool LListBase::Block::blockPool( LListBlockPool, sizeof(LListBase::Block), 64 );


HashTableBase::HashTableBase(int num_buckets)
/*******************************************/
    : _numBuckets( num_buckets )
{
    _table = new Hashable *[num_buckets];
    memset( _table, 0, num_buckets*sizeof(Hashable *));
    _numEntries = 0;
}


HashTableBase::~HashTableBase()
/*****************************/
{
    Hashable *  prev;
    Hashable *  curr;
    int         i;

    for( i=0; i<_numBuckets; i++ ){
        curr = _table[i];
        while( curr != NULL ){
            prev = curr;
            curr = curr->next;
            delete prev;
        }
    }
    delete[] _table;
}


void HashTableBase::Clear()
/*************************/
{
    memset( _table, 0, _numBuckets*sizeof(Hashable *) );
    _numEntries = 0;
}


void HashTableBase::ClearAndDelete()
/**********************************/
{
    Hashable *  prev;
    Hashable *  curr;
    int         i;

    for( i=0; i<_numBuckets; i++ ){
        curr = _table[i];
        while( curr != NULL ){
            prev = curr;
            curr = curr->next;
            delete prev;
        }
        _table[i] = NULL;
    }
    _numEntries = 0;
}


int HashTableBase::Hash( uint_32 id )
/***********************************/
{
    //return ((int) (id << 8)) % _numBuckets;

    uint_32 s = id;
    uint_32 c;
    uint_32 g;
    uint_32 h;

    h = 0x4;
    c = 0x4;
    c += s;
    h = ( h << 4 ) + c;
    g = h & ~0x0ffffff;
    h ^= g;
    h ^= g >> (4+4+4+4+4);
    g = h & ~0x0fff;
    h ^= g;
    h ^= g >> (4+4+4);
    h ^= h >> (2+4);
    WAssert( ( h & ~0x0fff ) == 0 );
    return( (int) (h%_numBuckets) );
}


void HashTableBase::Insert( Hashable *hashdata )
/************************************************/
{
    Hashable *  current;
    Hashable *  prev;
    int         bucket;

    bucket = Hash( hashdata->index );
    current = _table[ bucket ];
    prev = NULL;
    while( current != NULL && current->index < hashdata->index ){
        prev = current;
        current = current->next;
    }
    if( current != NULL ){
        hashdata->next = current;
    } else {
        hashdata->next = NULL;
    }
    if( prev != NULL ){
        prev->next = hashdata;
    } else {
        _table[ bucket ] = hashdata;
    }
    _numEntries++;
}


Hashable * HashTableBase::Lookup( uint_32 id )
/**********************************************/
{
    Hashable *  result;
    Hashable *  current;

    current = _table[ Hash( id ) ];
    result = NULL;
    for( ;; ){
        if( current == NULL ){
            break;
        } else if( current->index > id ){
            break;
        } else if( current->index == id ){
            result = current;
            break;
        }
        current = current->next;
    }
    return result;
}


Hashable * HashTableBase::Remove( uint_32 id )
/**********************************************/
{
    Hashable            *current;
    Hashable            *prev;
    int                 index;

    prev = NULL;
    index = Hash( id );
    current = _table[ index ];
    for( ;; ){
        if( current == NULL ){
            break;
        } else if( current->index > id ){
            break;
        } else if( current->index == id ){
            if( prev != NULL ){
                prev->next = current->next;
            } else {
                _table[index] = current->next;
            }
            break;
        }
        prev = current;
        current = current->next;
    }
    return current;
}


LListBase::LListBase()
/********************/
{
    _tail = _current = 32-1;
    _headBlock = _tailBlock = _currentBlock = NULL;
    _numEntries = 0;
}


LListBase::~LListBase()
/*********************/
{
    Block *     prev;
    Block *     curr;

    curr = _headBlock;
    while( curr != NULL ){
        prev = curr;
        curr = curr->next;
        delete prev;
    }
}


void LListBase::Append( void *data )
/**********************************/
{
    Block       *block;
    int         tail;

    tail = _tail+1;
    if( tail == 32 ){
        block = new Block;
        block->data[0] = data;
        block->next = NULL;
        block->prev = _tailBlock;
        if( _headBlock == NULL ){
            _headBlock = block;
        } else {
            _tailBlock->next = block;
        }
        _tailBlock = block;
        _tail = 0;
    } else {
        _tail = tail;
        _tailBlock->data[tail] = data;
    }
    _numEntries++;
}


void *LListBase::Pop()
/********************/
{
    void        *result;
    Block       *topBlock;
    int         top;

    topBlock = _tailBlock;
    if( topBlock == NULL ){
        return NULL;
    }
    top = _tail;
    result = topBlock->data[top];
    top--;
    if( top == -1 ){
        _tailBlock = topBlock->prev;
        if( _tailBlock == NULL ){
            _headBlock = NULL;
        }
        _tail = 32-1;
        delete topBlock;
    } else {
        _tail = top;
    }
    _numEntries--;
    return result;
}


void * LListBase::First()
/***********************/
{
    _current = 0;
    _currentBlock = _headBlock;
    return (_currentBlock!=NULL)?_currentBlock->data[0]:NULL;
}


void * LListBase::Next()
/**********************/
{
    Block       *currentBlock;
    int         current;

    currentBlock = _currentBlock;
    if( currentBlock == NULL ){
        return NULL;
    } else {
        current = _current+1;
        if( currentBlock == _tailBlock ){
            if( current > _tail ){
                _currentBlock = NULL;
                return NULL;
            } else {
                _current = current;
                return currentBlock->data[current];
            }
        } else {
            if( current < 32 ){
                _current = current;
                return currentBlock->data[current];
            } else {
                _current = 0;
                WAssert( currentBlock->next != NULL );
                _currentBlock = currentBlock->next;
                return _currentBlock->data[0];
            }
        }
    }
}


void LListBase::Clear()
/*********************/
{
    Block *     prev;
    Block *     curr;

    curr = _headBlock;
    while( curr != NULL ){
        prev = curr;
        curr = curr->next;
        delete prev;
    }
    _headBlock = NULL;
    _tailBlock = NULL;
    _currentBlock = NULL;
    _tail = 32-1;
    _current = 32-1;
    _numEntries = 0;

    Block::blockPool.CleanUp( Pool::Medium );
}
