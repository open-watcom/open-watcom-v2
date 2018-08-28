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


#include "wipfc.hpp"
#include <algorithm>
#include "btmpblk.hpp"
#include "errors.hpp"
#ifdef CHECKCOMP
    #include <iostream>
#endif

BitmapBlock::BitmapBlock( word b, byte t ) :
    _bitBuffer( 0L ), _bitCount( 0 ), _bitsPerCode( INITBITS ), _hashingShift( MAXBITS - 8 ),
    _maxCode( maxVal( INITBITS ) ), _checkCount( 100 ), _blockSize( b ),
    _type( t )
{
}
/***************************************************************************/
void BitmapBlock::write( std::FILE* bmfpo ) const
{
    if( std::fwrite( &_size, sizeof( word ), 1, bmfpo ) != 1 )
        throw FatalError( ERR_WRITE );
    if( std::fputc( _type, bmfpo ) == EOF )
        throw FatalError( ERR_WRITE );
    if( std::fwrite( &_data[0], sizeof( byte ), _data.size(), bmfpo ) != _data.size() ) {
        throw FatalError( ERR_WRITE );
    }
}
/***************************************************************************/
/* This is an LZW/LZC compressor. It is based on code in 2 articles from
   Dr. Dobb's Journal:
    1. Mark R. Nelson, October 1989
    2. Shawn M. Regan, January 1990
*/
dword BitmapBlock::compress( std::FILE* bmfpi )
{
    std::size_t block;
    if( _type ) {
        std::vector< byte > buffer( _blockSize );
        block = std::fread( &buffer[0], sizeof( byte ), _blockSize, bmfpi );
        if( !block )
            throw FatalError( ERR_READ );       //should have seen EOF elsewhere
        if( block < _blockSize ) {
            _blockSize = static_cast< word >( block );
            buffer.resize( _blockSize );
        }
        std::vector< word > code( TABLESIZE, UNDEFINED );
        std::vector< word > prefix( TABLESIZE );
        std::vector< byte >  append( TABLESIZE );
        std::vector< byte >::const_iterator itr( buffer.begin() );
        std::size_t     bytesIn( 1 );
        std::size_t     bytesOut( 0 );
//        std::size_t     checkPoint( 0 );
//        std::size_t     oldCompRatio( 100 );
        std::size_t     newCompRatio;
        word  nextCode( FIRST );
        word  codeIndex;
        word  stringCode( *itr );
        word  character;
        while( ++itr != buffer.end() ) {
            character = static_cast< word >( *itr );
            ++bytesIn;
            codeIndex = findMatch( code, prefix, append, stringCode, character );
            if( code[codeIndex] != UNDEFINED ) {
                stringCode = code[codeIndex];
            } else {
                if( nextCode <= _maxCode ) {
                    code[codeIndex] = nextCode++;
                    prefix[codeIndex] = stringCode;
                    append[codeIndex] = static_cast< byte >( character );
                }
                bytesOut += outputCode( stringCode );
                stringCode = character;
                if( nextCode > _maxCode ) {                      // if table full
                    if( _bitsPerCode < MAXBITS ) {               // if more bits available
                        _maxCode = maxVal( static_cast< unsigned int >( ++_bitsPerCode ) );  // use 'em
                    /* Temp change: The decompressor table must *never* grow past 4096 entries! */
                    } else if( bytesIn > 0/*checkPoint*/ ) {    // else if check time
                        if( _bitsPerCode == MAXBITS ) {
                            newCompRatio = bytesOut * 100 / bytesIn;
                            if( newCompRatio > 0/*oldCompRatio*/ ) { //check for compression loss
                                outputCode( CLEAR );
                                _bitsPerCode = INITBITS;
                                nextCode = FIRST;
                                _maxCode = maxVal( INITBITS );
                                bytesIn = 0;
                                bytesOut = 0;
//                                oldCompRatio = 100;
                                std::fill( code.begin(), code.end(), UNDEFINED );
//                            } else {
//                                oldCompRatio = newCompRatio;
                            }
                        }
//                        checkPoint = bytesIn + checkCount;    // Set new checkpoint
                    }
                }
            }
        }
        bytesOut += outputCode( stringCode );
        if( nextCode == _maxCode )               // Special case: increment on EOF
            ++_bitsPerCode;
        bytesOut += outputCode( TERMINATE );
        bytesOut += flushCode();
        _size = static_cast< word >( ( _data.size() + 1 ) * sizeof( byte ) );
#ifdef CHECKCOMP
        std::vector< byte > buffer2( _blockSize );
        expand( buffer2 );
        if( buffer2.size() != buffer.size() )
            std::cout<< "    Expanded data size is not equal to the original" << std::endl;
        std::cout << "    Index Original Restored" << std::endl;
        std::size_t index( 0 );
        InputIter iitr( buffer.begin() );
        OutputIter oitr( buffer2.begin() );
        while( iitr != buffer.end() ) {
            if( *iitr != *oitr )
                std::printf( "    %5u %8x %8x\n", index, *iitr, *oitr );
            ++iitr;
            ++oitr;
            ++index;
        }
#endif
    } else {
        _data.resize( _blockSize );
        block = std::fread( &_data[0], sizeof( byte ), _blockSize, bmfpi );
        if( !block )
            throw FatalError( ERR_READ );       //should have seen EOF elsewhere
        if( block < _blockSize )
            _data.resize( block );
        _size = static_cast< word >( block + 1 );
    }
    return( static_cast< dword >( block ) );
}
/***************************************************************************/
sword BitmapBlock::findMatch( std::vector< word >& code,
    std::vector< word >& prefix, std::vector< byte >& append,
    sword hashPrefix, word character ) const
{
    sword index( ( character << _hashingShift ) ^ hashPrefix );
    sword offset( ( index == 0 ) ? 1 : TABLESIZE - index );
    for( ;; ) {
        if( code[index] == UNDEFINED )
            return index;
        if( prefix[index] == hashPrefix && append[index] == character)
            return index;
        index -= offset;
        if( index < 0 ) {
            index += TABLESIZE;
        }
    }
}
/***************************************************************************/
word BitmapBlock::outputCode( word code )
{
    word bytesOut( 0 );
    _bitBuffer |= static_cast< dword >( code ) << ( 32 - _bitsPerCode - _bitCount );
    _bitCount += _bitsPerCode;
    while( _bitCount >= 8 ) {
        //debugging
        //byte byte( static_cast< byte >( _bitBuffer >> 24 ) );
        //std::size_t bytesWritten( _data.size() );
        //_data.push_back( byte );
        _data.push_back( static_cast< byte >( _bitBuffer >> 24 ) );
        _bitBuffer <<= 8;
        _bitCount -= 8;
        ++bytesOut;
    }
    return bytesOut;
}
/***************************************************************************/
word BitmapBlock::flushCode( void )
{
    word bytesOut( 0 );
    _bitCount += _bitsPerCode;
    while( _bitCount >= 8 && _bitBuffer ) {
        _data.push_back( static_cast< byte >( _bitBuffer >> 24 ) );
        _bitBuffer <<= 8;
        _bitCount -= 8;
        ++bytesOut;
    }
    return bytesOut;
}
/***************************************************************************/
#ifdef CHECKCOMP
// Modified from the references above to follow what NewView does
void BitmapBlock::expand( std::vector< byte >& output )
{
    std::vector< word > prefix( TABLESIZE );
    std::vector< byte >  append( TABLESIZE );
    std::vector< byte >  decode( TABLESIZE );
    DecodeIter string;
    OutputIter out( output.begin() );
    //OutputIter outEnd( output.end() );  //debugging
    InputIter in( _data.begin() );
    //InputIter inEnd( _data.end() );      //debugging
    word nextCode( FIRST );
    word newCode;
    word oldCode;
    word character;
    byte  lastCode;
    bool clear( true );
    _bitBuffer = 0L;
    _bitCount = 0;
    _bitsPerCode = INITBITS;
    _maxCode = maxVal( INITBITS );
    try {
        while( ( newCode = getCode( in ) ) != TERMINATE ) {
            if( clear ) {               //(re-)initialize
                clear = false;
                oldCode = newCode;
                character = newCode;
                lastCode = static_cast< byte >( newCode );//NewView
                *out = static_cast< byte >( newCode );
                ++out;
                continue;
            }
            if( newCode == CLEAR ) {    //Clear string table
                clear = true;
                _bitsPerCode = INITBITS;
                nextCode = FIRST;
                _maxCode = maxVal( INITBITS );
                continue;
            }
            if( newCode >= nextCode) {  //Check for string+char+string
                decode[0] = static_cast< byte >( character );
                string = decodeString( prefix, append, decode.begin() + 1, oldCode );
            } else {
                string = decodeString( prefix, append, decode.begin(), newCode );
            }
            character = *string;        //Output decoded string in reverse
            while( string >= decode.begin() ) {
                if( out == output.end() )
                    throw std::out_of_range( "    Destination limit exceeded" );
                *out = *string;
                ++out;
                --string;
            }
            lastCode = *( ++string );
            if( nextCode <= _maxCode ) { //Add to string table if not full
                prefix[nextCode] = oldCode;
                append[nextCode] = static_cast< byte >( character );
                ++nextCode;
                if( nextCode == _maxCode && _bitsPerCode < MAXBITS ) {
                    _maxCode = maxVal( ++_bitsPerCode );
                }
            }
            oldCode = newCode;
        }
        if( out != output.end() )
            std::cout << "    Adding fill bytes..." << std::endl;
        while( out != output.end() ) {
            *out = lastCode;
            ++out;
        }
    }
    catch( std::out_of_range& e ) {
        std::cerr << e.what() << std::endl;
    }
}
/***************************************************************************/
word BitmapBlock::getCode( InputIter& in )
{
    word retval;
    while( _bitCount <= 24 ) {
        if( in != _data.end() ) {
            _bitBuffer |= static_cast< dword >( *in ) << ( 24 - _bitCount );
            ++in;
        } else {
            //_bitBuffer |= 0;
            _bitBuffer |= static_cast< dword >( EOF ) << ( 24 - _bitCount );
        }
        _bitCount += 8;
    }
    if( in != _data.end() ) {
        retval = static_cast< word >( _bitBuffer >> ( 32 - _bitsPerCode ) );
    } else {
        retval = TERMINATE;
    }
    //retval = static_cast< word >( _bitBuffer >> ( 32 - _bitsPerCode ) );
    _bitBuffer <<= _bitsPerCode;
    _bitCount -= _bitsPerCode;
    return retval;
}
/*******************************************************************************************/
BitmapBlock::DecodeIter BitmapBlock::decodeString( std::vector< word >& prefix,
    std::vector< byte >& append, DecodeIter decode, word code )
{
    std::size_t index = 0;
    while( code > 255 ) {
        *decode = append[code];
        ++decode;
        code = prefix[code];
        if( index >= TABLESIZE )
            throw std::out_of_range( "    Decode stack size limit exceeded" );
        ++index;
    }
    *decode = static_cast< byte >( code );
    return decode;
}

#endif //CHECKCOMP
