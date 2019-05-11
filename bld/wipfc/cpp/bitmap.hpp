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
public:
    Bitmap( std::string& f );
    ~Bitmap() { };
    dword write( std::FILE* bmfpo ) const;
private:
    Bitmap( const Bitmap& rhs );            //no copy
    Bitmap& operator=( const Bitmap& rhs ); //no assignment
    struct BitmapFileHeader {
        byte    type[2];        //'BM' for input, 'bM' for output
        dword   size;           //including this header, before lzw compression
        sword   xHotspot;
        sword   yHotspot;
        dword   bitsOffset;     //offset to bitmap data
        dword   bmihSize;       //size of BitmapInfoHeader16 + this entry
        void read( std::FILE* bmfpi );
        void write( std::FILE* bmfpo ) const;
        static unsigned SIZE() { return( 2 * sizeof( byte ) + sizeof( dword ) + 2 * sizeof( sword ) + 2 * sizeof( dword ) ); };
        //followed by BitmapInfoHeaderXX
    };
    // win16 or os/2 1.x
    // used for both input and output
    struct BitmapInfoHeader16 {
        word    width;
        word    height;
        word    planes;
        word    bitsPerPixel;
        void read( std::FILE* bmfpi );
        void write( std::FILE* bmfpo ) const;
        static unsigned SIZE() { return( 4 * sizeof( word ) ); };
        //followed by rgb triples if <= 8bpp
    };
    struct RGBA {
        byte    blue;
        byte    green;
        byte    red;
        byte    reserved;
        void read( std::FILE* bmfpi );
        static unsigned SIZE() { return( 4 * sizeof( byte ) ); };
    };
    struct RGB {
        byte    blue;
        byte    green;
        byte    red;
        RGB() : blue( 0 ), green( 0 ), red( 0 ) { };
        RGB( RGBA& rhs ) : blue( rhs.blue ), green( rhs.green ), red( rhs.red ) { };
        RGB& operator=( RGBA& rhs ) { blue = rhs.blue; green = rhs.green; red = rhs.red; return *this; };
        void read( std::FILE* bmfpi );
        void write( std::FILE* bmfpo ) const;
        static unsigned SIZE() { return( 3 * sizeof( byte ) ); };
    };
    void readHeader16( std::FILE* bmfpi );
    void readHeaderW32( std::FILE* bmfpi );
    void readHeaderOS2( std::FILE* bmfpi );
    void findBlockSize( std::size_t width, std::size_t height, std::size_t bitsPerPixel );
    void compress( std::FILE* bmfpi );
    unsigned rgbByteSize() { return 1 << _bmih.bitsPerPixel; };

    BitmapFileHeader            _bmfh;          //read BitmapFileHeader
    BitmapInfoHeader16          _bmih;
    std::vector< RGB >          _rgb;
    typedef std::vector< RGB >::iterator RGBIter;
    typedef std::vector< RGB >::const_iterator ConstRGBIter;
    dword                       _bytesPerRow;
    dword                       _dataSize;      //size of the compressed data
    word                        _blockSize;     //including this word
    std::vector< BitmapBlock >  _data;          //and all of the data blocks
    typedef std::vector< BitmapBlock >::iterator DataIter;
    typedef std::vector< BitmapBlock >::const_iterator ConstDataIter;
};

#endif //BITMAP_INCLUDED

