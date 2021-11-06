/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2009-2021 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  A searchable word entry in the Dictionary (vocabulary) data
*
****************************************************************************/


#include "wipfc.hpp"
#include <algorithm>
#include <cstdlib>
#include "fts.hpp"
#include "errors.hpp"
#include "outfile.hpp"


//struct FTS8Header {
//    byte            size;
//    byte            compression;
//    //variable length data follows
//    //bitstring: 1 bit per panel
//};
//struct FTS16Header {
//    word            size;
//    byte            compression;
//    //variable length data follows
//    //bitstring: 1 bit per panel
//};

void FTSElement::setPages( std::size_t count )
/********************************************/
{
    std::size_t elements( ( count + 7 ) / 8 );
    _pages.resize( elements );
    _maxPage = count;
}

void FTSElement::onPage( std::size_t i )
/**************************************/
{
    std::size_t index( i / 8 );
    byte mask( 0x80 >> ( i % 8 ) );
    if( !(_pages[index] & mask) ) {
        ++_pageCount;
        _pages[index] |= mask;
    }
}

std::size_t FTSElement::getPages( std::vector< word >& pg, bool absent ) const
/****************************************************************************/
{
    word index = 0;

    if( absent ) {
        pg.reserve( _maxPage - _pageCount );
    } else {
        pg.reserve( _pageCount );
    }
    for( ConstPageIter itr = _pages.begin(); itr != _pages.end(); ++itr ) {
        for( byte mask = 0x80; mask != 0; mask >>= 1 ) {
            if( ( absent && (*itr & mask) == 0 && index < _maxPage )
              || ( !absent && (*itr & mask) ) ) {
                pg.push_back( index );
            }
            ++index;
        }
    }
    return( pg.size() * sizeof( word ) );
}

void FTSElement::build( OutFile* out )
/************************************/
{
    (void)out;

    if( _pageCount == 0 ) {
        _comp = NONE;
    } else if( _pageCount == _maxPage ) {
        _comp = ALL;
    } else {
        PageIter lnz;
        PageIter fnz;
        std::size_t score[5];
        score[0] = _pageCount * sizeof( word );
        score[1] = ( _maxPage - _pageCount ) * sizeof( word );
        //find the last non-zero byte
        for( lnz = _pages.end() - 1; lnz != _pages.begin() && *lnz == 0; --lnz )
            ;
        _pages.erase( lnz + 1, _pages.end() );    //remove bytes containing 0's from end
        score[2] = _pages.size() * sizeof( byte );
        //find first non-zero byte
        for( fnz = _pages.begin(); fnz != _pages.end() && *fnz == 0; ++fnz ) {
            ++_firstPage;
        }
        score[3] = ( score[2] - _firstPage ) * sizeof( byte ) + sizeof( word );
        std::vector< byte > rle;
        if( _pages.size() > 3 ) {
            //run length encode the truncated bitstring
            //but only if data > 3 bytes because minimum size of
            //rle encoding is 3 bytes
            encode( rle );
            score[4] = ( rle.size() + 1 ) * sizeof( byte );
        } else {
            score[4] = static_cast< std::size_t >( -1 );
        }
        std::size_t index = 0;
        _dataSize = score[0];
        for( std::size_t count = 1; count < sizeof( score ) / sizeof( score[0] ); ++count ) {
            if( score[count] < _dataSize ) {
                _dataSize = score[count];
                index = count;
            }
        }
        if( index == 0 ) {  //in order of preference (complexity)
            _comp = PRESENT;
        } else if( index == 1 ) {
            _comp = ABSENT;
        } else if( index == 2 ) {
            _comp = TRUNC;
        } else if( index == 3 ) {
            _comp = DBL_TRUNC;
            _pages.erase( _pages.begin(), fnz );  //truncate front
        } else if( index == 4 ) {
            _comp = RLE;
            _pages = rle;
        }
    }
}

void FTSElement::encode( std::vector< byte >& rle )
/*************************************************/
//The number of pages can never exceed 65535 because the count is stored in
//an word (unsigned short)
//only runs of 3 or more are considered to be "same"
{
    std::vector< byte > dif;
    ConstPageIter tst( _pages.begin() );
    ConstPageIter itr( _pages.begin() + 1 );
    bool same( *itr == *tst && *( itr + 1 ) == *tst );
    std::size_t sameCount( 2 );
    for( ; itr != _pages.end(); ++itr ) {
        if( same ) {
            if( *itr != *tst ) {
                --sameCount;
                if( sameCount < 0x80 ) {
                    rle.push_back( static_cast< byte >( sameCount ) );
                    rle.push_back( *tst );
                } else {
                    //sameCount will never exceed 65536 because the number of pages
                    //must be less than 65536 (it's stored in a word)
                    rle.push_back( 0x80 );
                    rle.push_back( *tst );
                    rle.push_back( static_cast< byte >( sameCount ) );
                    rle.push_back( static_cast< byte >( sameCount >> 8 ) );
                }
                tst = itr;
                same = false;
            } else {
                ++sameCount;
            }
        } else {
            if( *itr == *tst && itr + 1 != _pages.end() && *( itr + 1 ) == *tst ) {
                std::vector< byte >::const_iterator datab( dif.begin() );
                std::size_t difSize;
                byte code = 0xFF;
                for( difSize = dif.size(); difSize > 128; difSize -= 128 ) {
                    rle.push_back( code );
                    for( std::size_t count = 0; count <= 128; ++count ) {
                        rle.push_back( *datab++ );
                    }
                }
                if( difSize > 0 ) {
                    if( difSize > 1 ) {
                        code = static_cast< byte >( difSize - 1 ) | 0x80;
                    } else {
                        code = 0;
                    }
                    rle.push_back( code );
                    for( std::size_t count = 0; count < difSize; ++count ) {
                        rle.push_back( *datab++ );
                    }
                    dif.clear();
                }
                same = true;
                sameCount = 2;
            } else {
                dif.push_back( *tst );
                tst = itr;
            }
        }
    }
    if( same ) {
        --sameCount;
        if( sameCount < 0x80 ) {
            rle.push_back( static_cast< byte >( sameCount ) );
            rle.push_back( *tst );
        } else {
            rle.push_back( 0x80 );
            rle.push_back( *tst );
            rle.push_back( static_cast< byte >( sameCount ) );
            rle.push_back( static_cast< byte >( sameCount >> 8 ) );
        }
    } else {
        dif.push_back( *tst );
        std::vector< byte >::const_iterator datab( dif.begin() );
        std::size_t difSize;
        byte code = 0xFF;
        for( difSize = dif.size(); difSize > 128; difSize -= 128 ) {
            rle.push_back( code );
            for( std::size_t count = 0; count <= 128; ++count ) {
                rle.push_back( *datab++ );
            }
        }
        if( difSize > 1 ) {
            code = static_cast< byte >( difSize - 1 ) | 0x80;
        } else {
            code = 0;
        }
        rle.push_back( code );
        for( std::size_t count = 0; count < difSize; ++count ) {
            rle.push_back( *datab++ );
        }
    }
}

dword FTSElement::write( OutFile* out, bool big ) const
/*****************************************************/
{
    std::vector< word > pg;
    std::size_t size;

    // calculate FTS data size
    switch( _comp ) {
    case PRESENT:
    case ABSENT:
        size = getPages( pg, ( _comp == ABSENT ) );
        break;
    case RLE:
    case DBL_TRUNC:
    case TRUNC:
        size = _pages.size() * sizeof( byte );
        if( _comp == RLE ) {
            size += sizeof( byte );
        } else if( _comp == DBL_TRUNC ) {
            size += sizeof( word );
        }
        break;
    case NONE:
    case ALL:
    default:
        size = 0;
        break;
    }
    // output FTS data, add header size and round it
    if( big ) {
        size = static_cast< word >( size + sizeof( word ) + sizeof( byte ) );
        if( out->put( static_cast< word >( size ) ) ) {
            throw FatalError( ERR_WRITE );
        }
    } else {
        size = static_cast< byte >( size + sizeof( byte ) + sizeof( byte ) );
        if( out->put( static_cast< byte >( size ) ) ) {
            throw FatalError( ERR_WRITE );
        }
    }
    if( out->put( static_cast< byte >( _comp ) ) )
        throw FatalError( ERR_WRITE );
    switch( _comp ) {
    case PRESENT:
    case ABSENT:
        if( out->put( pg ) )
            throw FatalError( ERR_WRITE );
        break;
    case RLE:
    case DBL_TRUNC:
    case TRUNC:
        if( _comp == RLE ) {
            if( out->put( static_cast< byte >( 1 ) ) ) {
                throw FatalError( ERR_WRITE );
            }
        } else if( _comp == DBL_TRUNC ) {
            if( out->put( _firstPage ) ) {
                throw FatalError( ERR_WRITE );
            }
        }
        if( out->put( _pages ) )
            throw FatalError( ERR_WRITE );
        break;
    case NONE:
    case ALL:
    default:
        break;
    }
    return( static_cast< dword >( size ) );
}
