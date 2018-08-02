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
* Description:  Links to external files
* The database table is an array of filenames, each preceded by a length byte
*
****************************************************************************/


#include "wipfc.hpp"
#include <cstdlib>
#include "extfiles.hpp"
#include "errors.hpp"
#include "document.hpp"


void ExternalFiles::addFile( std::wstring& str )
{
    if( _table.find( str ) == _table.end() ) {
        _table.insert( std::map< std::wstring, STD1::uint16_t >::value_type( str, 0 ) );
        if( _table.size() >= 256 ) {
            throw Class1Error( ERR1_EXTFILESLARGE );
        }
    }
}
/***************************************************************************/
void ExternalFiles::convert()
{
    STD1::uint16_t count1( 0 );
    for( TableIter itr = _table.begin(); itr != _table.end(); ++itr, ++count1 ) {
        itr->second = count1;
    }
}
/***************************************************************************/
STD1::uint32_t ExternalFiles::write( std::FILE *out, Document *document )
{
    if( _table.empty() )
        return 0;
    STD1::uint32_t start( std::ftell( out ) );
    for( ConstTableIter itr = _table.begin(); itr != _table.end(); ++itr ) {
        std::string buffer;
        document->wtomb_string( itr->first, buffer );
        std::size_t length( buffer.size() );
        if( length > 255 ) {
            buffer.erase( 255 );
            length = 255;
        }
        std::size_t written;
        if( std::fputc( static_cast< STD1::uint8_t >( length + 1 ), out ) == EOF ||
            ( written = std::fwrite( buffer.data(), sizeof( char ), length, out ) ) != length )
            throw FatalError( ERR_WRITE );
        _bytes += static_cast< STD1::uint32_t >( written + 1 );
    }
    return start;
}

