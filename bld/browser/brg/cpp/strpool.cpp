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


#include "assure.h"
#include "strpool.h"

StringPool::StringPool( unsigned size, const char * owner )
                : _currBlock( NULL )
                , _currPos( NULL )
                , _endOfCurrBlock( NULL )
                , _firstBlock( NULL )
                , _blockSize( size )

                #if DEBUG
                , _owner( owner )
                , _numAllocs( 0 )
                , _numGrows( 0 )
                #endif
//--------------------------------------------------------
{
    owner = owner;  // disable warning if DEBUG is off
}

StringPool::~StringPool()
//-----------------------
{
    #if INSTRUMENTS
        Log.printf( "strpool: %s: %d allocated, %d grows, %d bytes used\n",
                    _owner, _numAllocs, _numGrows,
                    _numGrows * (_blockSize + sizeof( StringBlock ) - 1) );
    #endif

    ragnarok();
}

char * StringPool::alloc( unsigned len )
//--------------------------------------
{
    char * ret;

    #if DEBUG
        _numAllocs += 1;
    #endif

    if( _currPos + len >= _endOfCurrBlock ) {
        grow();
    }
    ASSERTION( _currPos + len < _endOfCurrBlock );

    ret = _currPos;
    _currPos += len;

    return ret;
}

void StringPool::grow()
//---------------------
{
    #if DEBUG
        _numGrows += 1;
    #endif

    _currBlock = (StringBlock *) new char[ _blockSize + sizeof( StringBlock ) ];
    _endOfCurrBlock = (char *) _currBlock + _blockSize
                        + sizeof( StringBlock ) - 1;
    _currBlock->nextBlock = _firstBlock;
    _firstBlock = _currBlock;
    _currPos = _currBlock->data;
}

void StringPool::ragnarok()
//-------------------------
{
    StringBlock * curr;
    StringBlock * prev;

    curr = _firstBlock;
    while( curr != NULL ) {
        prev = curr;
        curr = curr->nextBlock;
        delete [] (char *) prev;
    }

    _currBlock = NULL;
    _currPos = NULL;
    _endOfCurrBlock = NULL;
    _firstBlock = NULL;
}
