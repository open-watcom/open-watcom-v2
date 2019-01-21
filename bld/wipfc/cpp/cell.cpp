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
* Description:  A single cell (part of a page)
*
* Cells are written in document order
* The local dictionary can be empty, but not text
*
****************************************************************************/


#include "wipfc.hpp"
#include <functional>
#include <algorithm>
#include "cell.hpp"
#include "errors.hpp"
#include "outfile.hpp"


//for each element, call buildText() to fill text buffer
void Cell::build( OutFile* out )
{
    _out = out;
    for( ElementIter itr = _elements.begin(); itr != _elements.end(); ++itr )
        ( *itr )->buildText( this );
    if( empty() ) {
        addByte( Cell::SPACE );
    }
}
/***************************************************************************/
void Cell::addTextToLD( word index )
{
    if( !std::binary_search( _localDictionary.begin(), _localDictionary.end(), index ) ) {
        LDIter itr(
            //std::lower_bound( _localDictionary.begin(), _localDictionary.end(), index );
            std::find_if( _localDictionary.begin(), _localDictionary.end(),
                std::bind2nd( std::greater< word >(), index ) ) );
        _localDictionary.insert( itr, index );
    }
}
/***************************************************************************/
byte Cell::LDIndex( word index )
{
    LDIter itr( std::find( _localDictionary.begin(), _localDictionary.end(), index ) );
        //std::lower_bound( _localDictionary.begin(), _localDictionary.end(), index );
    return( static_cast< byte >( itr - _localDictionary.begin() ) );
}
/***************************************************************************/

//struct cellData {
//    byte           zero;               //=0
//    dword          dictOffset;         //file offset to word dict[dictCount] array
//    byte           dictCount;          // <=254 unique words
//    word           textCount;
//    //variable length data follows:
//    //byte           text[textCount];  //encoded text (indexes into dict)
//    //word           dict[dictCount];  //index to global dictionary
//};

dword Cell::write( OutFile* out ) const
{
    dword offset( out->tell() );
    // zero
    if( out->put( static_cast< byte >( 0 ) ) )
        throw FatalError( ERR_WRITE );
    // dictOffset
    if( out->put( static_cast< dword >( offset + sizeof( byte ) + sizeof( dword ) + sizeof( byte )
            + sizeof( word ) + _data.size() * sizeof( byte ) ) ) )
        throw FatalError( ERR_WRITE );
    if( out->put( static_cast< byte >( _localDictionary.size() ) ) )
        throw FatalError( ERR_WRITE );
    if( out->put( static_cast< word >( _data.size() ) ) )
        throw FatalError( ERR_WRITE );
    if( out->put( _data ) )
        throw FatalError( ERR_WRITE );
    if( !_localDictionary.empty() ) {
        if( out->put( _localDictionary ) ) {
            throw FatalError( ERR_WRITE );
        }
    }
    return offset;
}
