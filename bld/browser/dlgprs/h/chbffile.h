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


#ifndef __CHBFFILE_H__

#ifdef __WATCOMC__
// disable "integral value may be truncated" warning
#pragma warning 389 9
#endif

#include "chfile.h"
#include "mempool.h"

class CheckedBufferedFile : public CheckedFile {
public:

                    CheckedBufferedFile( const char * fileName );
    virtual         ~CheckedBufferedFile();

    virtual void    open( int access, int permission );
    virtual int     read( void * buffer, int len );
    virtual void    write( const void * buffer, int len );
    virtual long    seek( long offset, int whence );
    virtual long    tell() const;

protected:
    virtual void    reOpen();
    virtual void    privClose();
            void    flushBuffer();      // empty write buffer
            void    reloadBuffer();

            char *      _buffer;
            int         _bufLen;        // the number of bytes in the buffer
            long        _startOffset;   // offset for start of buffer in file
            int         _buffOffset;    // offset within buffer

    static  MemoryPool  _bufferPool;
};

#define __CHBFFILE_H__
#endif
