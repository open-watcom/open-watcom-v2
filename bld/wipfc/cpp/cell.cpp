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
* Description:  A single cell (part of a page)
*
* Cells are written in document order
* The local dictionary can be empty, but not text
*
****************************************************************************/

#include <algorithm>
#include "cell.hpp"
#include "errors.hpp"

//for each element, call buildText() to fill text buffer
void Cell::build()
{
    for( ElementIter itr = elements.begin(); itr != elements.end(); ++itr )
        ( *itr )->buildText( this );
    if( text.empty() )
        text.push_back( 0xFE );
}
/***************************************************************************/
void Cell::addWord( STD1::uint16_t word )
{
    if( !std::binary_search( localDictionary.begin(), localDictionary.end(), word ) ) {
        LDIter itr( 
            //std::lower_bound( localDictionary.begin(), localDictionary.end(), word );
            std::find_if( localDictionary.begin(), localDictionary.end(),
                std::bind2nd( std::greater< STD1::uint16_t >(), word ) ) );
        localDictionary.insert( itr, word );
    }
}
/***************************************************************************/
void Cell::addText( STD1::uint16_t word )
{
    LDIter itr( 
        //std::lower_bound( localDictionary.begin(), localDictionary.end(), word );
        std::find( localDictionary.begin(), localDictionary.end(), word ) );
    size_t index = itr - localDictionary.begin();
    text.push_back( static_cast< STD1::uint8_t >( index ) );
}
/***************************************************************************/
void Cell::addEsc( const std::vector< STD1::uint8_t >& esc )
{
    for( ConstTextIter itr = esc.begin(); itr != esc.end(); ++itr )
        text.push_back( *itr );
}
/***************************************************************************/
#pragma pack(push, 1)
    struct cellData {
        STD1::uint8_t  zero;               //=0
        STD1::uint32_t dictOffset;         //file offset to STD1::uint16_t array
        STD1::uint8_t  dictCount;          // <=254 unique words
        STD1::uint16_t textCount;
        //variable length data follows:
        //STD1::uint8_t  text[textCount];  //encoded text (indexes into dict)
        //STD1::uint16_t  dict[dictCount];  //index to global dictionary
    };
#pragma pack(pop)

STD1::uint32_t Cell::write( std::FILE* out ) const
{
    STD1::uint32_t offset( std::ftell( out ) );
    cellData data;
    data.zero = 0;
    data.dictOffset = offset + sizeof( STD1::uint8_t ) + sizeof( STD1::uint32_t ) +
        sizeof( STD1::uint8_t ) + sizeof( STD1::uint16_t ) + text.size();
    data.dictCount = static_cast< STD1::uint8_t >( localDictionary.size() );
    data.textCount = static_cast< STD1::uint16_t >( text.size() );
    if( std::fwrite( &data, sizeof( cellData ), 1, out ) != 1 )
        throw FatalError( ERR_WRITE );
    if( std::fwrite( &text[0], sizeof( STD1::uint8_t ), text.size(), out ) != text.size() )
        throw FatalError( ERR_WRITE );
    if( !localDictionary.empty() && \
        std::fwrite( &localDictionary[0],
                     sizeof( STD1::uint16_t ),
                     localDictionary.size(),
                     out ) != localDictionary.size() )
        throw FatalError( ERR_WRITE );
    return offset;
}

