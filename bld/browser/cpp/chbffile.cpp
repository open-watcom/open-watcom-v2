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
#include <errno.h>
#include <assert.h>

#include "chbffile.h"

#define OFFSET_SHIFT 12
#define PAGEMASK    ( 0xffffffffl << OFFSET_SHIFT )
#define BUFFERSIZE  ( 0x1 << OFFSET_SHIFT )     // 4K
#define BUFFMASK    ( BUFFERSIZE - 1 )

MemoryPool CheckedBufferedFile::_bufferPool( BUFFERSIZE, "CheckedBufferedFile", 1 );

inline int BuffOffset( int off ) {
    return( (int)(off & BUFFMASK) );
}

inline long PageBound( long off ) {
    return( off & PAGEMASK );
}

CheckedBufferedFile::CheckedBufferedFile( const char * fileName )
            : CheckedFile( fileName )
            , _buffer( NULL )
            , _bufLen( -1 )
            , _startOffset( -1 )
            , _buffOffset( -1 )
//---------------------------------------------------------------
{
}

CheckedBufferedFile::~CheckedBufferedFile()
//-----------------------------------------
{
    if( _logOpen ) {
        close();
    }
}

void CheckedBufferedFile::open( int access, int permission )
//----------------------------------------------------------
{
    if( access & O_RDWR ) {
        FileExcept oops( FileExcept::Open, EINVAL, _fileName,
                        "Can't open buffered file for Read / Write." );
        throw( oops );
    }

    CheckedFile::open( access, permission );
    _buffer = (char *) _bufferPool.alloc();

    if( access & O_WRONLY ) {
        _startOffset = 0;
        _buffOffset = 0;
        _bufLen = 0;
    } else {
        reloadBuffer();
    }
}

int CheckedBufferedFile::read( void * buffer, int len )
//-----------------------------------------------------
{
    unsigned amt;
    int      lenRead = 0;

    if( !_isOpen ) {
        reOpen();
    }

    // if this is the last page in the file and it doesn't
    // contain <len> bytes, return 0.
    if( _bufLen < BUFFERSIZE && _buffOffset + len >= _bufLen ) {
        lenRead = _bufLen - _buffOffset;
        if( lenRead > 0 ) {
            memcpy( buffer, _buffer + _buffOffset, lenRead );
            _buffOffset = _bufLen;
            return lenRead;
        } else {
            return 0;
        }
    }

    for( ;; ) {
        if( _buffOffset + len <= _bufLen ) break;
        amt = _bufLen - _buffOffset;
        memcpy( buffer, _buffer + _buffOffset, amt );
        _buffOffset += amt;
        len -= amt;
        lenRead += amt;
        buffer = (char *) buffer + amt;

        if( _bufLen == BUFFERSIZE ) {
            reloadBuffer();
        } else {
            break;
        }
    }
    memcpy( buffer, _buffer + _buffOffset, len );
    _buffOffset += len;
    lenRead += len;

    return lenRead;
}

void CheckedBufferedFile::write( const void * buffer, int len )
//-------------------------------------------------------------
{
    unsigned amt;

    if( !_isOpen ) {
        reOpen();
    }

    for( ;; ) {
        if( _buffOffset + len <= BUFFERSIZE ) break;
        amt = BUFFERSIZE - _buffOffset;
        memcpy( _buffer + _buffOffset, buffer, amt );
        _buffOffset += amt;
        len -= amt;
        buffer = (char *) buffer + amt;
        flushBuffer();
    }
    memcpy( _buffer + _buffOffset, buffer, len );
    _buffOffset += len;
}

long CheckedBufferedFile::seek( long offset, int whence )
//-------------------------------------------------------
{
    long newOff;

    if( !_isOpen ) {
        reOpen();
    }

    if( _openAccess & O_WRONLY ) {
        flushBuffer();
        _startOffset = CheckedFile::seek( offset, whence );
        _buffOffset = 0;
        return _startOffset;
    } else {
        switch( whence ) {
        case SEEK_SET:
            newOff = offset;
            break;
        case SEEK_CUR:
            newOff = _startOffset + _buffOffset + offset;
            break;
        case SEEK_END:
            newOff = st_size() + offset;
            break;
        default:
            FileExcept oops( FileExcept::Seek, EINVAL, _fileName, BadWhenceMessage );
            throw( oops );
        }

        if( newOff >= _startOffset + _bufLen || newOff < _startOffset ) {
            long tmp;

            tmp = CheckedFile::seek( PageBound( newOff ), SEEK_SET );
            if( tmp != PageBound( newOff ) ) {
                FileExcept oops( FileExcept::Seek, errno, _fileName );
                throw( oops );
            }
            reloadBuffer();
            _buffOffset = BuffOffset( newOff );
        } else {
            _buffOffset = BuffOffset( newOff );
        }

        return _startOffset + _buffOffset;
    }
}

long CheckedBufferedFile::tell() const
//------------------------------------
{
    return _startOffset + _buffOffset;
}


void CheckedBufferedFile::reOpen()
//--------------------------------
{
    int BuffOffset = _buffOffset;

    _buffer = (char *) _bufferPool.alloc();

    assert( !(_openAccess & O_WRONLY) );

    _currOffset = _startOffset;
    CheckedFile::reOpen();

    reloadBuffer();
    _buffOffset = BuffOffset;
}

void CheckedBufferedFile::privClose()
//-----------------------------------
{
    if( _openAccess & O_WRONLY ) {
        flushBuffer();
    }
    _bufferPool.free( _buffer );
    _buffer = NULL;
    CheckedFile::privClose();
}

void CheckedBufferedFile::reloadBuffer()
//--------------------------------------
// assume the file is physically positioned at the start of
// the next page
{
    _startOffset = _currOffset;
    _buffOffset = 0;
    _bufLen = CheckedFile::read( _buffer, BUFFERSIZE );
}

void CheckedBufferedFile::flushBuffer()
//-------------------------------------
// assume the file is positioned at the start of the page
{
    if( _buffOffset > 0 ) {
        CheckedFile::write( _buffer, _buffOffset );
    }

    _buffOffset = 0;
    _startOffset = _currOffset;
}

