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
* Description:  Strings data
*
****************************************************************************/

#include <cstdlib>
#include "strings.hpp"
#include "errors.hpp"

STD1::uint32_t StringTable::write( std::FILE *out )
{
    if( table.empty() )
        return 0L;
    STD1::uint32_t start( std::ftell( out ) );
    for( ConstTableIter itr = table.begin(); itr != table.end(); ++itr ) {
        char buffer[ 256 ];
        size_t written;
        size_t length( std::wcstombs( buffer, itr->c_str(), sizeof( buffer ) / sizeof( char ) ) );
        if( length == static_cast< size_t >( -1 ) )
            throw FatalError( ERR_T_CONV );
        if( std::fputc( static_cast< STD1::uint8_t >( length + 1 ), out ) == EOF ||
            ( written = std::fwrite( buffer, sizeof( char ), length, out ) ) != length)
            throw FatalError( ERR_WRITE );
        bytes += written + 1;
    }
    return start;
}

