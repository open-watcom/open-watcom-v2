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
* Description:  Panel Controls data
*
****************************************************************************/

#include "ctrlbtn.hpp"
#include "errors.hpp"
#include "util.hpp"

STD1::uint32_t ControlButton::write( std::FILE *out ) const
{
    STD1::uint32_t bytes( sizeof( STD1::uint16_t ) * 2 );
    STD1::uint16_t type( 1 );
    if( std::fwrite( &type, sizeof( STD1::uint16_t), 1, out) != 1 )
        throw FatalError( ERR_WRITE );
    if( std::fwrite( &res, sizeof( STD1::uint16_t), 1, out) != 1 ) 
        throw FatalError( ERR_WRITE );
    std::string buffer;
    wtombstring( txt, buffer );
    size_t length( buffer.size() );
    if( length > 255 ) {
        buffer.erase( 255 );
        length = 255;
    }
    if( std::fputc( static_cast< STD1::uint8_t >( length ), out) == EOF ||
        std::fwrite( buffer.data(), sizeof( char ), length, out ) != length )
        throw FatalError( ERR_WRITE );
    bytes += length + 1;
    return bytes;
}

