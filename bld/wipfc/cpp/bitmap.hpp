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
* Description:  Bitmap graphics data
*   Supported input types are Win16, Win32, OS/2-16, and OS2-32
*
****************************************************************************/

#ifndef BITMAP_INCLUDED
#define BITMAP_INCLUDED

#include "config.hpp"
#include <cstdio>
#include <string>
#include "btmpblk.hpp"

class Bitmap {
public:
    Bitmap( std::string& f );
    ~Bitmap() { };
    STD1::uint32_t write( std::FILE* out ) const;
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
        void read( std::FILE* in );
        void write( std::FILE* out ) const;
        //followed by BitmapInfoHeaderXX
    };
    // win16 or os/2 1.x
    // used for both input and output
    struct BitmapInfoHeader16 {
        STD1::uint16_t width;
        STD1::uint16_t height;
        STD1::uint16_t planes;
        STD1::uint16_t bitsPerPixel;
        void read( std::FILE* in );
        void write( std::FILE* out ) const;
        //followed by rgb triples if <= 8bpp
    };
    struct RGBA {
        STD1::uint8_t   blue;
        STD1::uint8_t   green;
        STD1::uint8_t   red;
        STD1::uint8_t   reserved;
        void read( std::FILE* in );
        };
    struct RGB {
        STD1::uint8_t   blue;
        STD1::uint8_t   green;
        STD1::uint8_t   red;
        RGB() : blue( 0 ), green( 0 ), red( 0 ) { };
        RGB( RGBA& rhs ) : blue( rhs.blue ), green( rhs.green ), red( rhs.red ) { };
        RGB& operator=( RGBA& rhs ) { blue = rhs.blue; green = rhs.green; red = rhs.red; return *this; };
        void read( std::FILE* in );
        void write( std::FILE* out ) const;
    };
#pragma pack(pop)
    BitmapFileHeader bmfh;              //read BitmapFileHeader
    BitmapInfoHeader16 bmih;
    std::vector< RGB > rgb;
    STD1::uint32_t bytesPerRow;
    STD1::uint32_t dataSize;            //size of the compressed data
    STD1::uint16_t blockSize;           //including this word
    std::vector< BitmapBlock > data;    //and all of the data blocks
    typedef std::vector< BitmapBlock >::iterator DataIter;
    typedef std::vector< BitmapBlock >::const_iterator ConstDataIter;
    void readHeader16( std::FILE* in );
    void readHeaderW32( std::FILE* in );
    void readHeaderOS2( std::FILE* in );
    void findBlockSize( size_t width, size_t height, size_t bitsPerPixel );
    void compress( std::FILE* in );
};

#endif //BITMAP_INCLUDED

