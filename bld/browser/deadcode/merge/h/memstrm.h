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


#ifndef _MEMORY_STREAM_H
#define _MEMORY_STREAM_H

// System includes --------------------------------------------------------

#include <wstd.h>

// Project includes -------------------------------------------------------

enum MemSeekType { MemSeekFromCurrent, MemSeekFromEnd, MemSeekFromStart };

class MemoryStream {
public:
                        MemoryStream();
                        ~MemoryStream();

    unsigned long       read( void * buffer, int length );
    unsigned long       size();
    unsigned long       write( void * buffer, int length );
    unsigned long       tell();
    unsigned long       length();
    unsigned long       seek( long pos, MemSeekType seekType = MemSeekFromStart );
    unsigned long       setSize( unsigned long size );
    char *              getPtr();
    void                writeByte( char value );
    void                writeULEB128( uint_32 value );
    void                writeLEB128( int_32 value );

protected:
    void                growBlock( int increment );

    static int          _minIncrement;
    char *              _memory;
    char *              _currPtr;
    unsigned long       _size;
    unsigned long       _length;
    unsigned long       _offset;

};

#endif
