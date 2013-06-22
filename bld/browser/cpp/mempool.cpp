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


#include <string.h>

#include "mempool.h"
#include "assure.h"

#if DEBUG
    static  DebuggingLog MemoryPool::_log( "mempool" );
#endif

#define ALLOCSIG 0xa5
#define FREESIG  0xbd

MemoryPool::MemoryPool( const char * owner )
    : _elemSize( 0 )
    , _elemsPerBlock( 0 )
    , _lastElement( NULL )
    , _currElement( NULL )
    , _currBlock( NULL )
    , _freeList( NULL )

    #if DEBUG
    , _poolOwner( owner )
    , _numAllocs( 0 )
    , _numFrees( 0 )
    #endif

//------------------------------------------
{
    owner = owner;  // disable warning if DEBUG not enabled
}


MemoryPool::MemoryPool( size_t elemSize, const char * owner,
                        size_t elemsPerBlock )
    : _elemSize( 0 )
    , _elemsPerBlock( 0 )
    , _currBlock( NULL )
    , _lastElement( NULL )
    , _currElement( NULL )
    , _freeList( NULL )

    #if DEBUG
    , _poolOwner( owner )
    , _numAllocs( 0 )
    , _numFrees( 0 )
    #endif

//----------------------------------------------------------
{
    owner = owner;  // disable warning if DEBUG not enabled

    setSize( elemSize, elemsPerBlock );
}

MemoryPool::~MemoryPool()
//-----------------------
{
    #if DEBUG
        if( _numAllocs > 0 && _numAllocs != _numFrees ) {
            _log.printf( "    <%d unfreed>\n", _poolOwner, _numAllocs - _numFrees );
            // NYI -- something to walk through and print like trmem
        }
    #endif

    ragnarok();
}

void MemoryPool::setSize( size_t elemSize, int elemsPerBlock )
//------------------------------------------------------------
{
    ASSERTION( _elemSize == 0 || _elemSize == elemSize );

    _elemSize = elemSize;
    _elemsPerBlock = elemsPerBlock;

    if( _elemSize < sizeof( void * )) {
        _elemSize = sizeof( void * );
    }

    // Size of hunk of memory including "next-block" ptr
    _blockSize = elemSize * elemsPerBlock + sizeof( void * );
}

void MemoryPool::ragnarok()
//-------------------------
{
    void * next;
    int    count;

    for( count = 0; _currBlock != NULL; count += 1 ) {
        next = * (( void ** ) _currBlock );
        delete [] _currBlock;
        _currBlock = ( char * )next;
    }

    #if DEBUG
        if( _poolOwner && _numAllocs >= 0 ) {
            _log.printf( "%s: %d allocated; ", _poolOwner, _numAllocs );
            _log.printf( "%d %d-byte elms/block\n", _elemsPerBlock, _elemSize );
            _log.printf( "    %d blocks of size %d giving %d\n", count, _blockSize, _blockSize * count );
        }
        _numAllocs = -1;
        _numFrees = 0;
    #endif

    _currBlock = NULL;
    _lastElement = NULL;
    _currElement = NULL;
    _freeList = NULL;
}

void * MemoryPool::alloc()
//------------------------
{
    ASSERTION( _elemSize > 0 && _blockSize > 0 );

    #if DEBUG
        _numAllocs += 1;
    #endif

    if( _freeList != NULL ) {
        char * tmp;

        tmp = ( char * )_freeList;
        _freeList = * ((void **) _freeList );
        return( tmp );
    }

    if( _currElement == _lastElement ) {
        grow();
    }

    _currElement -= _elemSize;

    #if DEBUG
        memset( _currElement, ALLOCSIG, _elemSize );
    #endif

    return( _currElement );
}

void MemoryPool::free( void * mem )
//---------------------------------
{
    if( mem == NULL ) {
        return;
    }

    #if DEBUG
        memset( mem, FREESIG, _elemSize );
        _numFrees += 1;
    #endif


    *( void ** ) mem = _freeList;
    _freeList = mem;
}

void MemoryPool::grow()
//---------------------
// private
{
    char * newBlock;

    newBlock = new char[ _blockSize ];
    *(( void ** )newBlock ) = _currBlock;
    _currBlock = newBlock;
    _lastElement = _currBlock + sizeof( void * );

    // Actually this is one element past the block size so that
    // alloc() does the right thing (this is because _currElement
    // is the last element returned by alloc)
    _currElement = _currBlock + _blockSize;
}

