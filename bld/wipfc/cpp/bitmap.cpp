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
* Description:  Bitmapped graphics data
*
****************************************************************************/


#include "wipfc.hpp"
#include <vector>
#include "bitmap.hpp"
#include "errors.hpp"


//used for input
struct BitmapInfoHeaderWin32 {
    dword   width;           //pixels
    dword   height;          //pixels
    word    planes;
    word    bitsPerPixel;    //1,4,8,16,24,32
    dword   compression;     //0,1,2,3
    dword   imageSize;       //rounded up to next dword
    dword   xResolution;
    dword   yResolution;
    dword   usedColors;
    dword   importantColors;
    void read( std::FILE* bmfpi );
    static unsigned SIZE() { return( 2 * sizeof( dword ) + 2 * sizeof( word ) + 6 * sizeof( dword ) ); };
    //possibly followed by rgb quads
};

//used for input
struct BitmapInfoHeaderOS22x {
    dword   width;           //pixels
    dword   height;          //pixels
    word    planes;
    word    bitsPerPixel;    //1,4,8,16,24,32
    dword   compression;     //0,1,2,3
    dword   imageSize;       //rounded up to next dword
    dword   xResolution;
    dword   yResolution;
    dword   usedColors;
    dword   importantColors;
    word    units;
    word    reserved;
    word    recording;
    word    rendering;
    dword   size1;
    dword   size2;
    dword   colorEncoding;
    dword   identifier;
    void read( std::FILE* bmfpi );
    static unsigned SIZE() { return( 2 * sizeof( dword ) + 2 * sizeof( word ) + 6 * sizeof( dword ) + 4 * sizeof( word ) + 4 * sizeof( dword ) ); };
    //possibly followed by rgb triples
};

Bitmap::Bitmap( std::string& fname ) : _dataSize( sizeof( word ) ), _blockSize( 0 )
{
    std::FILE* bmfpi( std::fopen( fname.c_str(), "rb" ) );
    if( !bmfpi )
        throw FatalError( ERR_OPENIMG );
    try {
        _bmfh.read( bmfpi );
        if( _bmfh.type[0] != 'B' || _bmfh.type[1] != 'M' )
            throw Class1Error( ERR1_BADFMT );
        _bmfh.type[0] = 'b';
        if( _bmfh.bmihSize == Bitmap::BitmapInfoHeader16::SIZE() + sizeof( _bmfh.bmihSize ) ) {
            readHeader16( bmfpi );
        } else if( _bmfh.bmihSize == BitmapInfoHeaderWin32::SIZE() + sizeof( _bmfh.bmihSize ) ) {
            readHeaderW32( bmfpi );
        } else if( _bmfh.bmihSize == BitmapInfoHeaderOS22x::SIZE() + sizeof( _bmfh.bmihSize ) ) {
            readHeaderOS2( bmfpi );
        } else {
            throw Class1Error( ERR1_BADFMT );
        }
        findBlockSize( _bmih.width, _bmih.height, _bmih.bitsPerPixel );
        compress( bmfpi );
        for( DataIter itr = _data.begin(); itr != _data.end(); ++itr ) {
            _dataSize += itr->totalSize();
        }
    }
    catch( FatalError& e ) {
        std::fclose( bmfpi );
        throw e;
    }
    catch( Class1Error& e ) {
        std::fclose( bmfpi );
        throw e;
    }
    std::fclose( bmfpi );
}
/***************************************************************************/
/*
BitmapFileHeader hdr;
variable length data follows:
unsigned char rgb[(1 << hdr.info.bitsPerPixel) * 3]
unsigned long size;         //starting with next field, used to SEEK_CUR to next bitmap
word          blockSize;
BitmapBlock[];
*/
dword Bitmap::write( std::FILE* bmfpo ) const
{
    dword offset( std::ftell( bmfpo ) );
    _bmfh.write( bmfpo );
    _bmih.write( bmfpo );
    for( ConstRGBIter itr = _rgb.begin(); itr != _rgb.end(); ++itr )
        itr->write( bmfpo );
    if( std::fwrite( &_dataSize, sizeof( dword ), 1, bmfpo ) != 1 )
        throw FatalError( ERR_WRITE );
    if( std::fwrite( &_blockSize, sizeof( word ), 1, bmfpo ) != 1 )
        throw FatalError( ERR_WRITE );
    for( ConstDataIter itr = _data.begin(); itr != _data.end(); ++itr )
        itr->write( bmfpo );
    return offset;
}
/***************************************************************************/
void Bitmap::BitmapFileHeader::read( std::FILE* bmfpi )
{
    if( std::fread( type, sizeof( type ), 1, bmfpi ) != 1 ) {
        throw FatalError( ERR_READ );
    }
    if( std::fread( &size, sizeof( size ), 1, bmfpi ) != 1 ) {
        throw FatalError( ERR_READ );
    }
    if( std::fread( &xHotspot, sizeof( xHotspot ), 1, bmfpi ) != 1 ) {
        throw FatalError( ERR_READ );
    }
    if( std::fread( &yHotspot, sizeof( yHotspot ), 1, bmfpi ) != 1 ) {
        throw FatalError( ERR_READ );
    }
    if( std::fread( &bitsOffset, sizeof( bitsOffset ), 1, bmfpi ) != 1 ) {
        throw FatalError( ERR_READ );
    }
    if( std::fread( &bmihSize, sizeof( bmihSize ), 1, bmfpi ) != 1 ) {
        throw FatalError( ERR_READ );
    }
}
/***************************************************************************/
void Bitmap::BitmapFileHeader::write( std::FILE* bmfpo ) const
{
    if( std::fwrite( type, sizeof( type ), 1, bmfpo ) != 1 ) {
        throw FatalError( ERR_WRITE );
    }
    if( std::fwrite( &size, sizeof( size ), 1, bmfpo ) != 1 ) {
        throw FatalError( ERR_WRITE );
    }
    if( std::fwrite( &xHotspot, sizeof( xHotspot ), 1, bmfpo ) != 1 ) {
        throw FatalError( ERR_WRITE );
    }
    if( std::fwrite( &yHotspot, sizeof( yHotspot ), 1, bmfpo ) != 1 ) {
        throw FatalError( ERR_WRITE );
    }
    if( std::fwrite( &bitsOffset, sizeof( bitsOffset ), 1, bmfpo ) != 1 ) {
        throw FatalError( ERR_WRITE );
    }
    if( std::fwrite( &bmihSize, sizeof( bmihSize ), 1, bmfpo ) != 1 ) {
        throw FatalError( ERR_WRITE );
    }
}
/***************************************************************************/
void Bitmap::BitmapInfoHeader16::read( std::FILE* bmfpi )
{
    if( std::fread( &width, sizeof( width ), 1, bmfpi ) != 1 ) {
        throw FatalError( ERR_READ );
    }
    if( std::fread( &height, sizeof( height ), 1, bmfpi ) != 1 ) {
        throw FatalError( ERR_READ );
    }
    if( std::fread( &planes, sizeof( planes ), 1, bmfpi ) != 1 ) {
        throw FatalError( ERR_READ );
    }
    if( std::fread( &bitsPerPixel, sizeof( bitsPerPixel ), 1, bmfpi ) != 1 ) {
        throw FatalError( ERR_READ );
    }
}
/***************************************************************************/
void Bitmap::BitmapInfoHeader16::write( std::FILE* bmfpo ) const
{
    if( std::fwrite( &width, sizeof( width ), 1, bmfpo ) != 1 ) {
        throw FatalError( ERR_WRITE );
    }
    if( std::fwrite( &height, sizeof( height ), 1, bmfpo ) != 1 ) {
        throw FatalError( ERR_WRITE );
    }
    if( std::fwrite( &planes, sizeof( planes ), 1, bmfpo ) != 1 ) {
        throw FatalError( ERR_WRITE );
    }
    if( std::fwrite( &bitsPerPixel, sizeof( bitsPerPixel ), 1, bmfpo ) != 1 ) {
        throw FatalError( ERR_WRITE );
    }
}
/***************************************************************************/
void BitmapInfoHeaderWin32::read( std::FILE* bmfpi )
{
    if( std::fread( &width, sizeof( width ), 1, bmfpi ) != 1 ) {
        throw FatalError( ERR_READ );
    }
    if( std::fread( &height, sizeof( height ), 1, bmfpi ) != 1 ) {
        throw FatalError( ERR_READ );
    }
    if( std::fread( &planes, sizeof( planes ), 1, bmfpi ) != 1 ) {
        throw FatalError( ERR_READ );
    }
    if( std::fread( &bitsPerPixel, sizeof( bitsPerPixel ), 1, bmfpi ) != 1 ) {
        throw FatalError( ERR_READ );
    }
    if( std::fread( &compression, sizeof( compression ), 1, bmfpi ) != 1 ) {
        throw FatalError( ERR_READ );
    }
    if( std::fread( &imageSize, sizeof( imageSize ), 1, bmfpi ) != 1 ) {
        throw FatalError( ERR_READ );
    }
    if( std::fread( &xResolution, sizeof( xResolution ), 1, bmfpi ) != 1 ) {
        throw FatalError( ERR_READ );
    }
    if( std::fread( &yResolution, sizeof( yResolution ), 1, bmfpi ) != 1 ) {
        throw FatalError( ERR_READ );
    }
    if( std::fread( &usedColors, sizeof( usedColors ), 1, bmfpi ) != 1 ) {
        throw FatalError( ERR_READ );
    }
    if( std::fread( &importantColors, sizeof( importantColors ), 1, bmfpi ) != 1 ) {
        throw FatalError( ERR_READ );
    }
}
/***************************************************************************/
void BitmapInfoHeaderOS22x::read( std::FILE* bmfpi )
{
    if( std::fread( &width, sizeof( width ), 1, bmfpi ) != 1 ) {
        throw FatalError( ERR_READ );
    }
    if( std::fread( &height, sizeof( height ), 1, bmfpi ) != 1 ) {
        throw FatalError( ERR_READ );
    }
    if( std::fread( &planes, sizeof( planes ), 1, bmfpi ) != 1 ) {
        throw FatalError( ERR_READ );
    }
    if( std::fread( &bitsPerPixel, sizeof( bitsPerPixel ), 1, bmfpi ) != 1 ) {
        throw FatalError( ERR_READ );
    }
    if( std::fread( &compression, sizeof( compression ), 1, bmfpi ) != 1 ) {
        throw FatalError( ERR_READ );
    }
    if( std::fread( &imageSize, sizeof( imageSize ), 1, bmfpi ) != 1 ) {
        throw FatalError( ERR_READ );
    }
    if( std::fread( &xResolution, sizeof( xResolution ), 1, bmfpi ) != 1 ) {
        throw FatalError( ERR_READ );
    }
    if( std::fread( &yResolution, sizeof( yResolution ), 1, bmfpi ) != 1 ) {
        throw FatalError( ERR_READ );
    }
    if( std::fread( &usedColors, sizeof( usedColors ), 1, bmfpi ) != 1 ) {
        throw FatalError( ERR_READ );
    }
    if( std::fread( &importantColors, sizeof( importantColors ), 1, bmfpi ) != 1 ) {
        throw FatalError( ERR_READ );
    }
    if( std::fread( &units, sizeof( units ), 1, bmfpi ) != 1 ) {
        throw FatalError( ERR_READ );
    }
    if( std::fread( &reserved, sizeof( reserved ), 1, bmfpi ) != 1 ) {
        throw FatalError( ERR_READ );
    }
    if( std::fread( &recording, sizeof( recording ), 1, bmfpi ) != 1 ) {
        throw FatalError( ERR_READ );
    }
    if( std::fread( &rendering, sizeof( rendering ), 1, bmfpi ) != 1 ) {
        throw FatalError( ERR_READ );
    }
    if( std::fread( &size1, sizeof( size1 ), 1, bmfpi ) != 1 ) {
        throw FatalError( ERR_READ );
    }
    if( std::fread( &size2, sizeof( size2 ), 1, bmfpi ) != 1 ) {
        throw FatalError( ERR_READ );
    }
    if( std::fread( &colorEncoding, sizeof( colorEncoding ), 1, bmfpi ) != 1 ) {
        throw FatalError( ERR_READ );
    }
    if( std::fread( &identifier, sizeof( identifier ), 1, bmfpi ) != 1 ) {
        throw FatalError( ERR_READ );
    }
}

/***************************************************************************/
void Bitmap::RGB::read( std::FILE* bmfpi )
{
    if( std::fread( &blue, sizeof( blue ), 1, bmfpi ) != 1 ) {
        throw FatalError( ERR_READ );
    }
    if( std::fread( &green, sizeof( green ), 1, bmfpi ) != 1 ) {
        throw FatalError( ERR_READ );
    }
    if( std::fread( &red, sizeof( red ), 1, bmfpi ) != 1 ) {
        throw FatalError( ERR_READ );
    }
}
/***************************************************************************/
void Bitmap::RGB::write( std::FILE* bmfpo ) const
{
    if( std::fwrite( &blue, sizeof( blue ), 1, bmfpo ) != 1 ) {
        throw FatalError( ERR_WRITE );
    }
    if( std::fwrite( &green, sizeof( green ), 1, bmfpo ) != 1 ) {
        throw FatalError( ERR_WRITE );
    }
    if( std::fwrite( &red, sizeof( red ), 1, bmfpo ) != 1 ) {
        throw FatalError( ERR_WRITE );
    }
}
/***************************************************************************/
void Bitmap::RGBA::read( std::FILE* bmfpi )
{
    if( std::fread( &blue, sizeof( blue ), 1, bmfpi ) != 1 ) {
        throw FatalError( ERR_READ );
    }
    if( std::fread( &green, sizeof( green ), 1, bmfpi ) != 1 ) {
        throw FatalError( ERR_READ );
    }
    if( std::fread( &red, sizeof( red ), 1, bmfpi ) != 1 ) {
        throw FatalError( ERR_READ );
    }
    if( std::fread( &reserved, sizeof( reserved ), 1, bmfpi ) != 1 ) {
        throw FatalError( ERR_READ );
    }
}
/***************************************************************************/
void Bitmap::readHeader16( std::FILE* bmfpi )
{
    _bmih.read( bmfpi );
    if( _bmih.bitsPerPixel <= 8 ) {
        std::size_t rgbSize( rgbByteSize() );
        _rgb.resize( rgbSize );
        for( std::size_t count1 = 0; count1 < rgbSize; ++count1 ) {
            RGB tmp1;
            tmp1.read( bmfpi );
            _rgb.push_back( tmp1 );
        }
    }
}
/***************************************************************************/
void Bitmap::readHeaderW32( std::FILE* bmfpi )
{
    BitmapInfoHeaderWin32 bmihW32;
    bmihW32.read( bmfpi );
    _bmih.width = static_cast< word >( bmihW32.width );
    _bmih.height = static_cast< word >( bmihW32.height );
    _bmih.planes = bmihW32.planes;
    _bmih.bitsPerPixel = bmihW32.bitsPerPixel;
    _bmfh.bmihSize = BitmapInfoHeader16::SIZE() + sizeof( _bmfh.bmihSize );
    //it is not necessary to adjust the size or offset, but doesn't hurt
    _bmfh.size -= BitmapInfoHeaderWin32::SIZE() - Bitmap::BitmapInfoHeader16::SIZE();
    _bmfh.bitsOffset -= BitmapInfoHeaderWin32::SIZE() - Bitmap::BitmapInfoHeader16::SIZE();
    if( _bmih.bitsPerPixel <= 8 ) {
        std::size_t rgbSize( rgbByteSize() );
        _rgb.reserve( rgbSize );
        for( std::size_t count1 = 0; count1 < rgbSize; ++count1 ) {
            RGBA tmp1;
            tmp1.read( bmfpi );
            RGB tmp2( tmp1 );
            _rgb.push_back( tmp2 );
        }
        _bmfh.size -= static_cast< dword >( rgbSize * ( Bitmap::RGBA::SIZE() - Bitmap::RGB::SIZE() ) );
        _bmfh.bitsOffset -= static_cast< dword >( rgbSize * ( Bitmap::RGBA::SIZE() - Bitmap::RGB::SIZE() ) );
    } else if( bmihW32.compression == 3 ) {
        //read and discard 3 items
        RGBA tmp1;
        tmp1.read( bmfpi );
        tmp1.read( bmfpi );
        tmp1.read( bmfpi );
        _bmfh.size -= 3 * Bitmap::RGBA::SIZE();
        _bmfh.bitsOffset -= 3 * Bitmap::RGBA::SIZE();
    }
}
/***************************************************************************/
void Bitmap::readHeaderOS2( std::FILE* bmfpi )
{
    BitmapInfoHeaderOS22x bmihOS22x;
    bmihOS22x.read( bmfpi );
    _bmih.width = static_cast< word >( bmihOS22x.width );
    _bmih.height = static_cast< word >( bmihOS22x.height );
    _bmih.planes = bmihOS22x.planes;
    _bmih.bitsPerPixel = bmihOS22x.bitsPerPixel;
    _bmfh.bmihSize = Bitmap::BitmapInfoHeader16::SIZE() + sizeof( _bmfh.bmihSize );
    //it is not necessary to adjust the size or offset, but doesn't hurt
    _bmfh.size -= BitmapInfoHeaderOS22x::SIZE() - Bitmap::BitmapInfoHeader16::SIZE();
    _bmfh.bitsOffset -= BitmapInfoHeaderOS22x::SIZE() - Bitmap::BitmapInfoHeader16::SIZE();
    if( _bmih.bitsPerPixel <= 8 ) {
        std::size_t rgbSize( rgbByteSize() );
        _rgb.reserve( rgbSize );
        if( bmihOS22x.usedColors ) {
            for( std::size_t count1 = 0; count1 < bmihOS22x.usedColors; ++count1 ) {
                RGBA tmp1;
                tmp1.read( bmfpi );
                RGB tmp2( tmp1 );
                _rgb.push_back( tmp2 );
            }
            RGB tmp;
            for( std::size_t count1 = bmihOS22x.usedColors; count1 < rgbSize; ++count1 ) {
                _rgb.push_back( tmp );
            }
        } else {
            for( std::size_t count1 = 0; count1 < rgbSize; ++count1 ) {
                RGBA tmp1;
                tmp1.read( bmfpi );
                RGB tmp2( tmp1 );
                _rgb.push_back( tmp2 );
            }
        }
        _bmfh.bitsOffset = static_cast< dword >( Bitmap::BitmapFileHeader::SIZE() + Bitmap::BitmapInfoHeader16::SIZE() + Bitmap::RGB::SIZE() * _rgb.size() );
        _bmfh.size = _bmfh.bitsOffset + bmihOS22x.imageSize;
    } else if( bmihOS22x.compression == 3 ) {
        //read and discard 3 items
        RGBA tmp1;
        tmp1.read( bmfpi );
        tmp1.read( bmfpi );
        tmp1.read( bmfpi );
        _bmfh.size -= 3 * Bitmap::RGBA::SIZE();
        _bmfh.bitsOffset -= 3 * Bitmap::RGBA::SIZE();
    }
}
/***************************************************************************/
// We pretend that the bitmap is unencoded and that each bitmap row is padded
// to a dword boundary
void Bitmap::findBlockSize( std::size_t width, std::size_t height, std::size_t bitsPerPixel )
{
    switch( bitsPerPixel ) {
    case 1:
        _bytesPerRow = (( width / 8 ) & 3 ) ? static_cast< dword >( (( width / 8 ) & ~3 ) + 4 ) : static_cast< dword >( width / 8 );
        break;
    case 4:
        _bytesPerRow = (( (width + 1) / 2 ) & 3 ) ? static_cast< dword >( (( (width + 1) / 2 ) & ~3 ) + 4 ) : static_cast< dword >( (width + 1) / 2 );
        break;
    case 8:
        _bytesPerRow = ( width & 3 ) ? static_cast< dword >( ( width & ~3 ) + 4 ) : static_cast< dword >( width );
        break;
    case 15:
    case 16:
        _bytesPerRow = (( width * 2 ) & 3 ) ? static_cast< dword >( (( width * 2 ) & ~3 ) + 4 ) : static_cast< dword >( width * 2 );
        break;
    case 24:
        _bytesPerRow = (( width * 3 ) & 3 ) ? static_cast< dword >( (( width * 3 ) & ~3 ) + 4 ) : static_cast< dword >( width * 3 );
        break;
    case 32:
        _bytesPerRow = static_cast< dword >( width * 4 );
        break;
    default:
        throw Class1Error( ERR1_BADFMT );
    }
    dword totalSize( static_cast< dword >( _bytesPerRow * height ) );
    _blockSize = static_cast< word >( ( ( UINT16_MAX - 256 ) / _bytesPerRow - 1 ) * _bytesPerRow );
#ifdef CHECKCOMP
    std::printf( "  width=%u bitsPerPixel=%u, bytesPerRow=%u\n", width, bitsPerPixel, _bytesPerRow );
    std::printf( "  calculated blockSize=%u\n", _blockSize );
#endif
    if( static_cast< dword >( _blockSize ) > totalSize ) {
        _blockSize = static_cast< word >( totalSize );
    }
}
/***************************************************************************/
void Bitmap::compress( std::FILE* bmfpi )
{
#ifdef CHECKCOMP
    unsigned int    count( 1 );
#endif
    dword   bytesToRead( _bytesPerRow * _bmih.height );
    dword   bytes( 0 );
    while( bytes < bytesToRead ) {
#ifdef CHECKCOMP
        std::printf( "  Block %u\n", count++ );
#endif
#define COMPRESSION
#ifdef COMPRESSION
        _data.push_back( BitmapBlock( _blockSize, 2 ) );
#else
        _data.push_back( BitmapBlock( _blockSize, 0 ) );
#endif
        bytes += _data[_data.size() - 1].compress( bmfpi );
    }
}
