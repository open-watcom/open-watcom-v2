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


#ifndef __THREADOUT_H__
#define __THREADOUT_H__

#ifdef __cplusplus

#include "thrdbuff.h"

namespace iothread {

class ThredOut {                                // Output File Bufferer
    int _handle;                        // - handle for i/o
    unsigned _buf_size;                 // - size of a buffer
    void* _buffer;                      // - current buffer
    void* _output;                      // - next output location
    unsigned long _disk_addr;           // - disk offset for current buffer
    unsigned _error_code;               // - error code
    BufferFactory _factory;
    Buffer *_cur_buff;

    bool setError                       // Set error indication
        ( void )
    ;
    bool writeBuffer                    // Write a buffer
        ( unsigned size )               // - size to be written
    ;
    void flush                          // write any remaining buffered data
        ( void )
    ;


public:
    ThredOut                            // Ctor
        ( unsigned buf_size )           // - buffer size
    ;
    ~ThredOut                           // Dtor
        ( void )
    ;
    unsigned errorCode                  // Get error code
        ( void )
        const
    {
        return _error_code;
    }

    unsigned long tell                  // Tell the disk address
        ( void )
        const
    {
        return _disk_addr + (char*)_output - (char*)_buffer;
    }

    bool write                          // Write a segment
        ( void const* segment           // - segment
        , unsigned size )               // - size to be written
    ;
    bool writeChar                      // Write a character
        ( char chr )                    // - the character
    ;

    void startHandle                    // sets the file handle for all subsequent writes
        ( int newHandle )               // automatically flushes if necessary.
    ;
    void endHandle                      // flushes the buffer if necessary.
        ( void )
    ;
};

} // end of namespace

#else
#       error fileout.h can only be used with c++
#endif // __cplusplus

#endif // __THREADOUT_H__
