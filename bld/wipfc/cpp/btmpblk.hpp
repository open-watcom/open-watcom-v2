/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2009-2018 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  A (possibly compressed) block of bitmap graphics data
*
****************************************************************************/

#ifndef BITMAPBLOCK_INCLUDED
#define BITMAPBLOCK_INCLUDED

#include <cstdio>
#include <vector>

// BitMapBlock
class BitmapBlock {
public:
    BitmapBlock() { };
    BitmapBlock( word b, byte t );
    void write( std::FILE* bmfpo ) const;
    dword compress( std::FILE* bmfpi );
    unsigned int totalSize() const
    { return static_cast< unsigned int >( sizeof( word ) + ( _data.size() + 1 ) * sizeof( byte ) ); };
private:
    std::size_t maxVal( unsigned int n ) { return ( 1 << n ) - 1; };
    //check for a matching code
    sword findMatch( std::vector< word >& code,
        std::vector< word >& prefix,
        std::vector< byte >& append,
        sword hashPrefix, word character ) const;
    word outputCode( word );
    word flushCode( void );
//#define CHECKCOMP
#ifdef CHECKCOMP
    typedef std::vector< byte >::iterator DecodeIter;
    typedef std::vector< byte >::iterator OutputIter;
    typedef std::vector< byte >::iterator InputIter;
    void expand( std::vector< byte >& output );
    word getCode( InputIter& index );
    DecodeIter decodeString( std::vector< word >& prefix,
        std::vector< byte >& append, DecodeIter buffer, word code );
#endif //CHECKCOMP

    enum lzwBits {
        INITBITS = 9,
        MAXBITS = 12
    };
    enum lzwTSize {
        TABLESIZE = 5021
    };
    enum lzwCodes {
        CLEAR = 256,
        TERMINATE,
        FIRST,
        UNDEFINED = 0xFFFF
    };
    dword               _bitBuffer;     //buffer for variable length codes
    std::size_t         _bitCount;      //bits in the buffer
    std::size_t         _bitsPerCode;   //code size
    std::size_t         _hashingShift;
    std::size_t         _maxCode;
    std::size_t         _checkCount;    //when to check for degradation
    word                _blockSize;     //uncompressed data size
    word                _size;          //starting with next field
    byte                _type;          //0 == uncompressed, 2 == LZW compressed
    std::vector< byte > _data;
};

#endif //BITMAPBLOCK_INCLUDED
