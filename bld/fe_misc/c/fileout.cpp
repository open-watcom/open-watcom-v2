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


#include <errno.h>
#include <fcntl.h>
#include <io.h>
#include <stdio.h>
#include <string.h>
#include <sys\stat.h>
#include <sys\types.h>

#include "FileOut.h"


FileOut::FileOut                        // Ctor
    ( int handle                        // - file handle
    , unsigned buf_size )               // - buffer size
    : _handle( handle )
    , _buf_size( buf_size )
    , _disk_addr( 0 )
    , _buffer( 0 )
    , _output( 0 )
    , _error_code( 0 )
{
    _buffer = new char[ _buf_size ];
    _output = _buffer;
}


FileOut::~FileOut                       // Dtor
    ( void )
{
    writeBuffer( (char*)_output - (char*)_buffer );
    if( 0 != _buffer ) {
        delete[] _buffer;
    }
}


bool FileOut::seek                      // Seek in file
    ( unsigned long new_posn )          // - new position
{
    writeBuffer( (char*)_output - (char*)_buffer );
    int retn;
    if( -1 == ::lseek( _handle, new_posn, SEEK_SET ) ) {
        retn = setError();
    } else {
        _disk_addr = new_posn;
    }
    return retn;
}


bool FileOut::setError                  // Set error indications
    ( void )
{
    _error_code = errno;
    return false;
}


bool FileOut::write                     // Write a segment
    ( void const* segment               // - segment
    , unsigned size )                   // - size to write
{
    bool retn;
    for( ; ; ) {
        unsigned left = (char*)_buffer + _buf_size - (char*)_output;
        if( left >= size ) {
            ::memcpy( _output, segment, size );
            _output = (char*)_output + size;
            retn = true;
            break;
        }
        ::memcpy( _output, segment, left );
        segment = (char const*)segment + left;
        size -= left;
        writeBuffer( _buf_size );
    }
    return retn;
}


bool FileOut::writeChar                 // Write a character
    ( char chr )                        // - the character
{
    bool retn;
    unsigned left = (char*)_buffer + _buf_size - (char*)_output;
    if( left > 1 ) {
        *(char*&)_output = chr;
        _output = (char*)_output + 1;
        retn = true;
    } else {
        char buf[4];
        buf[0] = chr;
        retn = write( buf, 1 );
    }
    return retn;
}


bool FileOut::writeBuffer               // Write a buffer
    ( unsigned size )                   // - size to write
{
    bool retn = true;
    if( 0 != size ) {
        int wrote = ::write( _handle, _buffer, size );
        _output = _buffer;
        _disk_addr += size;
        if( -1 == wrote
         || wrote != size ) {
            retn = setError();
        }
    }
    return retn;
}
