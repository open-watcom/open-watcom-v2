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


// System includes --------------------------------------------------------

#include <string.h>

// Project includes -------------------------------------------------------

#include "memstrm.h"
#include "errors.h"

int MemoryStream::_minIncrement = 256;

MemoryStream::MemoryStream()
/**************************/
    : _memory( NULL )
    , _size( 0 )
    , _length( 0 )
    , _offset( 0 )
    , _currPtr( NULL )
{
    growBlock( 0 );
}

MemoryStream::~MemoryStream()
/***************************/
{
    if( _memory ) {
        delete [] _memory;
    }
}

void MemoryStream::growBlock( int increment )
/*******************************************/
{
    int numChunks;
    unsigned long newSize;
    char * newBlock;

    numChunks = increment / _minIncrement + 1;

    newSize = _size + ( numChunks * _minIncrement );

    newBlock = new char[ newSize ];
    if( newBlock == NULL ) {
        throw ErrOutOfMemory;
    }

    memset( newBlock, 0, newSize );

    memcpy( newBlock, _memory, _size );

    _currPtr = newBlock + _offset;

    if( _memory != NULL ) {
        delete [] _memory;
    }

    _size = newSize;
    _memory = newBlock;
}

unsigned long MemoryStream::read( void * buffer, int length )
/***********************************************************/
{
    if( _offset + length <= _length ) {
        memcpy( buffer, _currPtr, length );
        _currPtr += length;
        _offset += length;
    } else {
        memcpy( buffer, _currPtr, _length - _offset );
        _currPtr = _currPtr + _length - _offset;
        _offset = _length;
    }

    return _offset;
}

unsigned long MemoryStream::size()
/********************************/
{
    return _length;
}

unsigned long MemoryStream::write( void * buffer, int length )
/************************************************************/
{
    if( _offset + length > _size ) {
        growBlock( _offset + length - _size );
    }

    if( _offset + length > _length ) {
        _length = _offset + length;
    }

    memcpy( _currPtr, buffer, length );

    _currPtr += length;
    _offset += length;

    return _offset;
}

unsigned long MemoryStream::tell()
/********************************/
{
    return _offset;
}

unsigned long MemoryStream::length()
/**********************************/
{
    return _length;
}

unsigned long MemoryStream::seek( long pos, MemSeekType seekType )
/****************************************************************/
// Moves to the absolute position specified by pos.
// If pos is past end of buffer, move to end of buffer
// If pos is before beginning of buffer, move to beginning of buffer
// (Note difference between this seek and file seek)
{
    if( seekType == MemSeekFromCurrent ) {
        pos = _offset + pos;
    } else if( seekType == MemSeekFromEnd ) {
        pos = _length + pos;
    }

    if( pos > _length ) {
        if( pos > _size ) {
            growBlock( pos - _size );
        }
        _length = pos;
        _currPtr = _memory + pos;
        _offset = pos;

    } else if( pos < 0 ){
        _currPtr = _memory;
        _offset = 0;
    } else {
        _currPtr = _memory + pos;
        _offset = pos;
    }

    return _offset;
}

unsigned long MemoryStream::setSize( unsigned long size )
/*******************************************************/
// Expand or truncate the memory block.
// If truncate, offset may be moved to new end of memory if offset was
// previously past the new block size.
//
// Returns the offset.
{
    if( size < _length ) {
        _length = size;
        if( _offset > _length ) {
            _offset = _length;
        }
    } else if( size > _length ) {
        if( size > _size ) {
            growBlock( size - _size );
        }

        _length = size;
    }

    return _offset;
}

void MemoryStream::writeByte( char value )
/****************************************/
{
    if( _offset + 1 > _size ) {
        growBlock( _offset + 1 - _size );
    }

    if( _offset + 1 > _length ) {
        _length = _offset + 1;
    }

    *_currPtr = value;
    _currPtr++;

    _offset++;
}

void MemoryStream::writeULEB128( uint_32 value )
/**********************************************/
{
    uint_8                      b;

    for(;;) {
        b = (uint_8) (value & 0x7f);
        value >>= 7;
        if( value == 0 ) break;
        writeByte( b | 0x80 );
    }
    writeByte( b );
}

void MemoryStream::writeLEB128( int_32 value )
/*********************************************/
{
    uint_8                      b;

    /* we can only handle an arithmetic right shift */
    if( value >= 0 ) {
        for(;;) {
            b = (uint_8) (value & 0x7f);
            value >>= 7;
            if( value == 0 && ( b & 0x40 ) == 0 ) break;
            writeByte( b | 0x80 );
        }
    } else {
        for(;;) {
            b = (uint_8) (value & 0x7f);
            value >>= 7;
            if( value == -1 && ( b & 0x40 ) ) break;
            writeByte( b | 0x80 );
        }
    }
    writeByte( b );
}

char * MemoryStream::getPtr()
/***************************/
{
    return _memory;
}
