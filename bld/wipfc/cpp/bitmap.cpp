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
* Description:  Bitmapped graphics data
*
****************************************************************************/

#include <vector>
#include "bitmap.hpp"
#include "errors.hpp"

#pragma pack(push,1)

//used for input
struct BitmapInfoHeaderWin32 {
    STD1::uint32_t width;           //pixels
    STD1::uint32_t height;          //pixels
    STD1::uint16_t planes;
    STD1::uint16_t bitsPerPixel;    //1,4,8,16,24,32
    STD1::uint32_t compression;     //0,1,2,3
    STD1::uint32_t imageSize;       //rounded up to next dword
    STD1::uint32_t xResolution;
    STD1::uint32_t yResolution;
    STD1::uint32_t usedColors;
    STD1::uint32_t importantColors;
    void read( std::FILE* in );
    //possibly followed by rgb quads
};

//used for input
struct BitmapInfoHeaderOS22x {
    STD1::uint32_t width;           //pixels
    STD1::uint32_t height;          //pixels
    STD1::uint16_t planes;
    STD1::uint16_t bitsPerPixel;    //1,4,8,16,24,32
    STD1::uint32_t compression;     //0,1,2,3
    STD1::uint32_t imageSize;       //rounded up to next dword
    STD1::uint32_t xResolution;
    STD1::uint32_t yResolution;
    STD1::uint32_t usedColors;
    STD1::uint32_t importantColors;
    STD1::uint16_t units;
    STD1::uint16_t reserved;
    STD1::uint16_t recording;
    STD1::uint16_t rendering;
    STD1::uint32_t size1;
    STD1::uint32_t size2;
    STD1::uint32_t colorEncoding;
    STD1::uint32_t identifier;
    void read( std::FILE* in );
    //possibly followed by rgb triples
};

#pragma pack(pop)

Bitmap::Bitmap( std::string& fname ) : dataSize( sizeof( STD1::uint16_t ) ), blockSize( 0 )
{
    std::FILE* in( std::fopen( fname.c_str(), "rb" ) );
    if( !in )
        throw FatalError( ERR_OPENIMG );
    try {
        bmfh.read( in );
        if( bmfh.type[0] != 'B' || bmfh.type[1] != 'M' )
            throw Class1Error( ERR1_BADFMT );
        bmfh.type[0] = 'b';
        if ( bmfh.bmihSize == sizeof( BitmapInfoHeader16 ) + sizeof( STD1::uint32_t ) )
            readHeader16( in );
        else if (bmfh.bmihSize == sizeof( BitmapInfoHeaderWin32 ) + sizeof( STD1::uint32_t ) )
            readHeaderW32( in );
        else if (bmfh.bmihSize == sizeof( BitmapInfoHeaderOS22x ) + sizeof( STD1::uint32_t ) )
            readHeaderOS2( in );
        else
            throw Class1Error( ERR1_BADFMT );
        findBlockSize( bmih.width, bmih.height, bmih.bitsPerPixel );
        compress( in );
        for( DataIter itr = data.begin(); itr != data.end(); ++itr ) {
            dataSize += itr->totalSize();
        }
    }
    catch( FatalError& e ) {
        std::fclose( in );
        throw e;
    }
    catch( Class1Error& e ) {
        std::fclose( in );
        throw e;
    }
    std::fclose( in );
}
/***************************************************************************/
/*
BitmapFileHeader hdr;
variable length data follows:
unsigned char rgb[(1 << hdr.info.bitsPerPixel) * 3]
unsigned long size;         //starting with next field, used to SEEK_CUR to next bitmap
STD1::uint16_t blockSize;
BitmapBlock[];
*/
STD1::uint32_t Bitmap::write( std::FILE* out ) const
{
    STD1::uint32_t offset( std::ftell( out ) );
    bmfh.write( out );
    bmih.write( out );
    if( !rgb.empty() ) {
        if( std::fwrite( &rgb[0], sizeof( RGB ), rgb.size(), out ) != rgb.size() )
            throw FatalError( ERR_WRITE );
    }
    if( std::fwrite( &dataSize, sizeof( STD1::uint32_t ), 1, out ) != 1 )
        throw FatalError( ERR_WRITE );
    if( std::fwrite( &blockSize, sizeof( STD1::uint16_t ), 1, out ) != 1 )
        throw FatalError( ERR_WRITE );
    for( ConstDataIter itr = data.begin(); itr != data.end(); ++itr )
        itr->write( out );
    return offset;
}
/***************************************************************************/
void Bitmap::BitmapFileHeader::read( std::FILE* in )
{
    if( std::fread( this, sizeof( BitmapFileHeader ), 1, in ) != 1 )
        throw FatalError( ERR_READ );
}
/***************************************************************************/
void Bitmap::BitmapFileHeader::write( std::FILE* out ) const
{
    if( std::fwrite( this, sizeof( BitmapFileHeader ), 1, out ) != 1 )
        throw FatalError( ERR_WRITE );
}
/***************************************************************************/
void Bitmap::BitmapInfoHeader16::read( std::FILE* in )
{
    if( std::fread( this, sizeof( BitmapInfoHeader16 ), 1, in ) != 1 )
        throw FatalError( ERR_READ );
}
/***************************************************************************/
void Bitmap::BitmapInfoHeader16::write( std::FILE* out ) const
{
    if( std::fwrite( this, sizeof( BitmapInfoHeader16 ), 1, out ) != 1 )
        throw FatalError( ERR_WRITE );
}
/***************************************************************************/
void BitmapInfoHeaderWin32::read( std::FILE* in )
{
    if( std::fread( this, sizeof( BitmapInfoHeaderWin32 ), 1, in ) != 1 )
        throw FatalError( ERR_READ );
}
/***************************************************************************/
void BitmapInfoHeaderOS22x::read( std::FILE* in )
{
    if( std::fread( this, sizeof( BitmapInfoHeaderOS22x ), 1, in ) != 1 )
        throw FatalError( ERR_READ );
}
/***************************************************************************/
void Bitmap::RGB::read( std::FILE* in )
{
    if( std::fread( this, sizeof( RGB ), 1, in ) != 1 )
        throw FatalError( ERR_READ );
}
/***************************************************************************/
void Bitmap::RGB::write( std::FILE* out ) const
{
    if( std::fwrite( this, sizeof( RGB ), 1, out ) != 1 )
        throw FatalError( ERR_WRITE );
}
/***************************************************************************/
void Bitmap::RGBA::read( std::FILE* in )
{
    if( std::fread( this, sizeof( RGBA ), 1, in ) != 1 )
        throw FatalError( ERR_READ );
}
/***************************************************************************/
void Bitmap::readHeader16( std::FILE* in )
{
    bmih.read( in );
    if( bmih.bitsPerPixel <= 8 ) {
        size_t rgbSize( 1 << bmih.bitsPerPixel );
        rgb.resize( rgbSize );
        if( std::fread( &rgb[0], sizeof( RGB ), rgbSize, in ) != rgbSize )
            throw FatalError( ERR_READ );
    }
}
/***************************************************************************/
void Bitmap::readHeaderW32( std::FILE* in )
{
    BitmapInfoHeaderWin32 bmihW32;
    bmihW32.read( in );
    bmih.width = static_cast< STD1::uint16_t >( bmihW32.width );
    bmih.height = static_cast< STD1::uint16_t >( bmihW32.height );
    bmih.planes = bmihW32.planes;
    bmih.bitsPerPixel = bmihW32.bitsPerPixel;
    bmfh.bmihSize = sizeof( BitmapInfoHeader16 ) + sizeof( STD1::uint32_t );
    //it is not necessary to adjust the size or offset, but doesn't hurt
    bmfh.size -= sizeof( BitmapInfoHeaderWin32 ) - sizeof( BitmapInfoHeader16 );
    bmfh.bitsOffset -= sizeof( BitmapInfoHeaderWin32 ) - sizeof( BitmapInfoHeader16 );
    if( bmih.bitsPerPixel <= 8 ) {
        size_t rgbSize( 1 << bmih.bitsPerPixel );
        rgb.reserve( rgbSize );
        for( size_t count1 = 0; count1 < rgbSize; ++count1 ) {
            RGBA tmp1;
            tmp1.read( in );
            RGB tmp2( tmp1 );
            rgb.push_back( tmp2 );
        }
        bmfh.size -= rgbSize * ( sizeof( RGBA ) - sizeof( RGB ) );
        bmfh.bitsOffset -= rgbSize * ( sizeof( RGBA ) - sizeof( RGB ) );
    }
    else if (bmihW32.compression == 3) {
        //read and discard 3 items
        RGBA tmp1;
        tmp1.read( in );
        tmp1.read( in );
        tmp1.read( in );
        bmfh.size -= 3 * sizeof( RGBA );
        bmfh.bitsOffset -= 3 * sizeof( RGBA );
    }
}
/***************************************************************************/
void Bitmap::readHeaderOS2( std::FILE* in )
{
    BitmapInfoHeaderOS22x bmihOS22x;
    bmihOS22x.read( in );
    bmih.width = static_cast< STD1::uint16_t >( bmihOS22x.width );
    bmih.height = static_cast< STD1::uint16_t >( bmihOS22x.height );
    bmih.planes = bmihOS22x.planes;
    bmih.bitsPerPixel = bmihOS22x.bitsPerPixel;
    bmfh.bmihSize = sizeof( BitmapInfoHeader16 ) + sizeof( STD1::uint32_t );
    //it is not necessary to adjust the size or offset, but doesn't hurt
    bmfh.size -= sizeof( BitmapInfoHeaderOS22x ) - sizeof( BitmapInfoHeader16 );
    bmfh.bitsOffset -= sizeof( BitmapInfoHeaderOS22x ) - sizeof( BitmapInfoHeader16 );
    if( bmih.bitsPerPixel <= 8 ) {
        size_t rgbSize( 1 << bmih.bitsPerPixel );
        rgb.reserve( rgbSize );
        if( bmihOS22x.usedColors ) {
            for( size_t count1 = 0; count1 < bmihOS22x.usedColors; ++count1 ) {
                RGBA tmp1;
                tmp1.read( in );
                RGB tmp2( tmp1 );
                rgb.push_back( tmp2 );
            }
            RGB tmp;
            for( size_t count1 = bmihOS22x.usedColors; count1 < rgbSize; ++count1 )
                rgb.push_back( tmp );
        }
        else {
            for( size_t count1 = 0; count1 < rgbSize; ++count1 ) {
                RGBA tmp1;
                tmp1.read( in );
                RGB tmp2( tmp1 );
                rgb.push_back( tmp2 );
            }
        }
        bmfh.bitsOffset = sizeof( BitmapFileHeader ) + sizeof( BitmapInfoHeader16 ) + 3 * rgb.size();
        bmfh.size = bmfh.bitsOffset + bmihOS22x.imageSize;
    }
    else if (bmihOS22x.compression == 3) {
        //read and discard 3 items
        RGBA tmp1;
        tmp1.read( in );
        tmp1.read( in );
        tmp1.read( in );
        bmfh.size -= 3 * sizeof( RGBA );
        bmfh.bitsOffset -= 3 * sizeof( RGBA );
    }
}
/***************************************************************************/
// We pretend that the bitmap is unencoded and that each bitmap row is padded
// to a dword boundary
void Bitmap::findBlockSize( size_t width, size_t height, size_t bitsPerPixel )
{
    switch( bitsPerPixel ) {
    case 1:
        bytesPerRow = (( width / 8 ) & 3 ) ? (( width / 8 ) & ~3 ) + 4 : width / 8;
        break;
    case 4:
        bytesPerRow = (( (width + 1) / 2 ) & 3 ) ? (( (width + 1) / 2 ) & ~3 ) + 4 : (width + 1) / 2;
        break;
    case 8:
        bytesPerRow = ( width & 3 ) ? ( width & ~3 ) + 4 : width;
        break;
    case 15:
    case 16:
        bytesPerRow = (( width * 2 ) & 3 ) ? (( width * 2 ) & ~3 ) + 4 : width * 2;
        break;
    case 24:
        bytesPerRow = (( width * 3 ) & 3 ) ? (( width * 3 ) & ~3 ) + 4 : width * 3;
        break;
    case 32:
        bytesPerRow = width * 4;
        break;
    default:
        throw Class1Error( ERR1_BADFMT );
    }
    STD1::uint32_t totalSize( bytesPerRow * height );
    blockSize = static_cast< STD1::uint16_t >( ( ( UINT16_MAX - 256 ) / bytesPerRow - 1 ) * bytesPerRow );
#ifdef CHECKCOMP
    std::printf( "  width=%u bitsPerPixel=%u, bytesPerRow=%u\n", width, bitsPerPixel, bytesPerRow );
    std::printf( "  calculated blockSize=%u\n", blockSize );
#endif
    if( totalSize < static_cast< STD1::uint32_t >( blockSize ))
        blockSize = static_cast< STD1::uint16_t >( totalSize );
}
/***************************************************************************/
void Bitmap::compress( std::FILE* in )
{
#ifdef CHECKCOMP
    unsigned int    count( 1 );
#endif
    STD1::uint32_t   bytesToRead( bytesPerRow * bmih.height );
    STD1::uint32_t   bytes( 0 );
    while( bytes < bytesToRead ) {
#ifdef CHECKCOMP
        std::printf( "  Block %u\n", count++ );
#endif
#define COMPRESSION
#ifdef COMPRESSION
        data.push_back( BitmapBlock( blockSize, 2 ) );
#else
        data.push_back( BitmapBlock( blockSize, 0 ) );
#endif
        bytes += data[ data.size() - 1 ].compress( in );
    }
}

