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


#ifndef __FILEOUT_H__
#define __FILEOUT_H__

#ifdef __cplusplus

class FileOut {                         // Output File Bufferer
    int _handle;                        // - handle for i/o
    unsigned _buf_size;                 // - size of a buffer
    unsigned long _disk_addr;           // - disk offset for current buffer
    void* _buffer;                      // - current buffer
    void* _output;                      // - next output location
    unsigned _error_code;               // - error code

    bool setError                       // Set error indication
        ( void )
    ;
    bool writeBuffer                    // Write a buffer
        ( unsigned size )               // - size to be written
    ;
public:
    FileOut                             // Ctor
        ( int handle                    // - i/o handle
        , unsigned buf_size )           // - buffer size
    ;
    ~FileOut                            // Dtor
        ( void )
    ;
    unsigned errorCode                  // Get error code
        ( void )
        const
    {
        return _error_code;
    }
    bool seek                           // Seek in the file
        ( unsigned long offset )        // - offset
    ;
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
};


#else
#       error fileout.h can only be used with c++
#endif // __cplusplus

#endif // __FILEOUT_H__
