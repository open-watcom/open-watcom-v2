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
#include <sys/stat.h>
#include <sys/types.h>

#include "ThredOut.h"

namespace iothread {

ThredOut::ThredOut                      // Ctor
    ( unsigned buf_size )               // - buffer size
    : _handle( -1 )
    , _buf_size( buf_size )
    , _disk_addr( 0 )
    , _buffer( 0 )
    , _output( 0 )
    , _error_code( 0 )
    , _factory( buf_size )
    , _cur_buff( NULL )
{
    _cur_buff = _factory.getBuffer();
    _buffer = _cur_buff->_data;
    _output = _buffer;
}


ThredOut::~ThredOut                     // Dtor
    ( void )
{
    // prepare last buffer.
    _cur_buff->_handle = _handle;
    _cur_buff->_size = (char*)_output - (char*)_buffer;
    //pass last buffer
    _factory.bufferFilled(_cur_buff);
}


bool ThredOut::setError                 // Set error indications
    ( void )
{
    _error_code = errno;
    return false;
}

void ThredOut::flush                            // write any remaining buffered data
    ( void )
{
    // prepare last buffer.
    _cur_buff->_handle = _handle;
    _cur_buff->_size = (char*)_output - (char*)_buffer;
    _disk_addr += _cur_buff->_size;
    //pass last buffer
    //_factory.bufferFilled(_cur_buff);
    //_cur_buff = _factory.getBuffer(); // get new buffer.

    _cur_buff = _factory.swapBuffer(_cur_buff); // write this buffer, grab the new one.


    _buffer = _cur_buff->_data;
    _output = _buffer;

}

bool ThredOut::write                    // Write a segment
    ( void const* segment               // - segment
    , unsigned size )                   // - size to write
{
    bool retn;
    if(_handle == -1) return false;
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


bool ThredOut::writeChar                        // Write a character
    ( char chr )                        // - the character
{
    bool retn;
    if(_handle == -1) return false;
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


bool ThredOut::writeBuffer              // Write a buffer
    ( unsigned size )                   // - size to write
{
    // prepare buffer.
    _cur_buff->_handle = _handle;
    _cur_buff->_size = size;
    //_factory.bufferFilled(_cur_buff); // return buffer to factory.
    //_cur_buff = _factory.getBuffer(); // get new buffer.
    _cur_buff = _factory.swapBuffer(_cur_buff); // write this buffer, grab the new one.

    _buffer = _cur_buff->_data;
    _output = _buffer;

    _disk_addr += size;

    return 1;
}

void ThredOut::startHandle              // sets the file handle for all subsequent writes
    ( int newHandle )           // automatically flushes if necessary.
{
    _handle = newHandle;
}

void ThredOut::endHandle                        // flushes the buffer if necessary.
    ( void )
{
    // prepare last buffer.
    _cur_buff->_handle = _handle;
    _cur_buff->_size = (char*)_output - (char*)_buffer;
    _disk_addr += _cur_buff->_size;

    //pass last buffer
    _factory.bufferFilled(_cur_buff);

    // send a 'close this file' buffer
    _factory.closeFile(_handle);

    // get a new buffer for next file.
    _cur_buff = _factory.getBuffer(); // get new buffer.
    _buffer = _cur_buff->_data;
    _output = _buffer;

    _handle = -1;
}



} //end of namespace iothread

