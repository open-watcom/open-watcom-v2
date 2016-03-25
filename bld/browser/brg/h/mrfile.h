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
* Description:  Merger file class declaration.
*
****************************************************************************/


#ifndef MRFILE_H_INCLUDED
#define MRFILE_H_INCLUDED

#include <dr.h>

#include <stdio.h>
#include "brmerge.h"
#include "elffile.h"
#include "chfile.h"


#define MERGEFILESTRBUF (512 * 3)

class MergeFile : public ElfFile {
public:
                            MergeFile( const char * filename );
                            ~MergeFile();

        enum MergeFileExcept { Invalid };

inline  uint_8              readByte( dr_section sect, uint_32& off );
        uint_16             readWord( dr_section sect, uint_32& off );
        uint_32             readDWord( dr_section sect, uint_32& off );
        uint_32             readULEB128( dr_section sect, uint_32& off );
        int_32              readSLEB128( dr_section sect, uint_32& off );
        uint_32             readForm( dr_section sect, uint_32& off,
                                        uint_32 form, uint_8 addrSize );
        const char *        readString( dr_section sect, uint_32& off );
inline  void                readBlock( dr_section sect, uint_32& off,
                                        void * buff, uint_32 len );
        void                skipForm( dr_section sect, uint_32& off,
                                        uint_32 form, uint_8 addrSize );
        void                copyFormTo( MergeFile& other, dr_section sect,
                                        uint_32& off, uint_32 form,
                                        uint_8 addrSize );

inline  void                writeByte( uint_8 b );
        void                writeWord( uint_16 w );
        void                writeDWord( uint_32 dw );
        void                writeULEB128( uint_32 uleb );
        void                writeSLEB128( int_32 sleb );
        void                writeString( const char * str );
inline  void                writeBlock( const void * buff, uint_32 len );
        void                writeForm( uint_32 form, uint_32 val, uint_8 addrSize );

        bool                operator== ( const MergeFile& other ) const {
                                return this == &other;
                            };

        static uint         ULEB128Len( uint_32 uleb );
        static uint         SLEB128Len( int_32 sleb );

private:
        static char         _buffer[ MERGEFILESTRBUF ];
};

inline void MergeFile::readBlock( dr_section sect, uint_32& off,
                            void * buff, uint_32 len )
//--------------------------------------------------------------
// reads a block into buf and updates the offset
{
#ifdef DEBUG
    InfoAssert( _drSizes[ sect ] >= off + len );
#endif
    _file->seek( _drSections[ sect ] + off, SEEK_SET );
    _file->read( buff, len );

    off += len;
}

inline uint_8 MergeFile::readByte( dr_section sect, uint_32& off )
//----------------------------------------------------------------
// reads byte, updates off
{
    int_8 result;
    readBlock( sect, off, &result, sizeof( uint_8 ) );
    return result;
}

inline void MergeFile::writeBlock( const void * buff, uint_32 len )
//-----------------------------------------------------------------
{
    _file->write( buff, len );
}

inline void MergeFile::writeByte( uint_8 b )
//------------------------------------------
{
    writeBlock( &b, sizeof( uint_8 ) );
}

#endif
