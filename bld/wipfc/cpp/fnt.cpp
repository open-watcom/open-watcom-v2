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
* Description:  Font data
*
****************************************************************************/


#include "wipfc.hpp"
#include "errors.hpp"
#include "fnt.hpp"
#include "uniutil.hpp"


STD1::uint32_t FontEntry::write( std::FILE *out ) const
{
    char            faceName[MAX_FACENAME_SIZE];    //null terminated
    std::string     buffer;

    wtomb_string( _faceName, buffer );
    std::strncpy( faceName, buffer.c_str(), MAX_FACENAME_SIZE );
    faceName[MAX_FACENAME_SIZE - 1] = '\0';
    if( std::fwrite( faceName, sizeof( faceName ), 1, out) != 1 )
        throw FatalError( ERR_WRITE );
    if( std::fwrite( &_height, sizeof( _height ), 1, out) != 1 )
        throw FatalError( ERR_WRITE );
    if( std::fwrite( &_width, sizeof( _width ), 1, out) != 1 )
        throw FatalError( ERR_WRITE );
    if( std::fwrite( &_codePage, sizeof( _codePage ), 1, out) != 1 )
        throw FatalError( ERR_WRITE );
    return( sizeof( faceName ) + sizeof( _height ) + sizeof( _width ) + sizeof( _codePage ) );
}

bool FontEntry::operator==( const FontEntry &rhs ) const
{
    const wchar_t *s1 = _faceName.c_str();
    const wchar_t *s2 = rhs._faceName.c_str();
    while( *s1 == *s2 && *s1 != L'\0' ) {
        s1++;
        s2++;
    }
    return *s1 == *s2 &&
        _height == rhs._height &&
        _width == rhs._width &&
        _codePage == rhs._codePage;
}
