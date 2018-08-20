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
* Description:  Bitmap graphics data
*   Supported input types are Win16, Win32, OS/2-16, and OS2-32
*
****************************************************************************/

#ifndef BITMAP_INCLUDED
#define BITMAP_INCLUDED

#include <cstdio>
#include <string>
#include "btmpblk.hpp"

class Bitmap {
    typedef STD1::uint8_t   byte;
    typedef STD1::uint16_t  word;
    typedef STD1::uint32_t  dword;

public:
    Bitmap( std::string& f );
    ~Bitmap() { };
    dword write( std::FILE* bmfpo ) const;
private:
    Bitmap( const Bitmap& rhs );            //no copy
    Bitmap& operator=( const Bitmap& rhs ); //no assignment
#pragma pack(push,1)
    struct BitmapFileHeader {
        STD1::uint8_t  type[ 2 ];       //'BM' for input, 'bM' for output
        STD1::uint32_t size;            //including this header, before lzw compression
        STD1::int16_t  xHotspot;
        STD1::int16_t  yHotspot;
        STD1::uint32_t bitsOffset;      //offset to bitmap data
        STD1::uint32_t bmihSize;        //size of BitmapInfoHeader16 + this entry
        void read( std::FILE* bmfpi );
        void write( std::FILE* bmfpo ) const;
        //followed by BitmapInfoHeaderXX
    };
    // win16 or os/2 1.x
    // used for both input and output
    struct BitmapInfoHeader16 {
        STD1::uint16_t width;
        STD1::uint16_t height;
        STD1::uint16_t planes;
        STD1::uint16_t bitsPerPixel;
        void read( std::FILE* bmfpi );
        void write( std::FILE* bmfpo ) const;
        //followed by rgb triples if <= 8bpp
    };
    struct RGBA {
        STD1::uint8_t   blue;
        STD1::uint8_t   green;
        STD1::uint8_t   red;
        STD1::uint8_t   reserved;
        void read( std::FILE* bmfpi );
    };
    struct RGB {
        STD1::uint8_t   blue;
        STD1::uint8_t   green;
        STD1::uint8_t   red;
        RGB() : blue( 0 ), green( 0 ), red( 0 ) { };
        RGB( RGBA& rhs ) : blue( rhs.blue ), green( rhs.green ), red( rhs.red ) { };
        RGB& operator=( RGBA& rhs ) { blue = rhs.blue; green = rhs.green; red = rhs.red; return *this; };
        void read( std::FILE* bmfpi );
        void write( std::FILE* bmfpo ) const;
    };
#pragma pack(pop)
    void readHeader16( std::FILE* bmfpi );
    void readHeaderW32( std::FILE* bmfpi );
    void readHeaderOS2( std::FILE* bmfpi );
    void findBlockSize( std::size_t width, std::size_t height, std::size_t bitsPerPixel );
    void compress( std::FILE* bmfpi );
    unsigned rgbByteSize() { return 1 << _bmih.bitsPerPixel; };

    BitmapFileHeader            _bmfh;          //read BitmapFileHeader
    BitmapInfoHeader16          _bmih;
    std::vector< RGB >          _rgb;
    dword                       _bytesPerRow;
    dword                       _dataSize;      //size of the compressed data
    word                        _blockSize;     //including this word
    std::vector< BitmapBlock >  _data;          //and all of the data blocks
    typedef std::vector< BitmapBlock >::iterator DataIter;
    typedef std::vector< BitmapBlock >::const_iterator ConstDataIter;
};

#endif //BITMAP_INCLUDED

