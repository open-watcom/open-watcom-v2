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
* Description:  Country configuration data
*
****************************************************************************/


#include "wipfc.hpp"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "env.hpp"
#include "cntrydef.hpp"
#include "util.hpp"
#include "errors.hpp"
#include "outfile.hpp"


void CountryDef::nlsConfig( const char *loc )
/*******************************************/
{
    char        buffer[256];
    const char  *fn1;
    const char  *fn2;

    std::string path( Environment.value( "WIPFC" ) );
    if( path.length() )
        path += PATH_SEPARATOR;
    std::string fname( path + "nlsconf.txt" );
    std::FILE *nlsconf = std::fopen( fname.c_str(), "r" );
    if( nlsconf == NULL )
        throw FatalError( ERR_NLSCONF );
    while( (fn1 = fn2 = std::fgets( buffer, sizeof( buffer ), nlsconf )) != NULL ) {
        std::size_t len = std::strlen( buffer );
        killEOL( buffer + len - 1 );
        char *p = skipWS( buffer );
        if( p[0] == '\0' )
            continue;                       // skip blank lines
        if( p[0] == '#' )
            continue;                       // skip comment lines
        p = std::strtok( buffer, " \t" );   // get locale
        if( p == NULL || std::strcmp( p, loc ) != 0 )
            continue;
        p = std::strtok( NULL, " \t" );     // get nls file
        if( p == NULL )
            continue;                       // skip incorrect lines
        fn1 = skipWS( p );
        p = std::strtok( NULL, " \t" );     // get entity file
        if( p == NULL )
            continue;                       // skip incorrect lines
        fn2 = skipWS( p );
        p = std::strtok( NULL, " \t" );     // get country
        if( p == NULL )
            continue;                       // skip incorrect lines
        _country = static_cast< word >( std::strtoul( skipWS( p ), NULL, 10 ) );
        p = std::strtok( NULL, " \t" );     // get codepage
        if( p == NULL )
            continue;                       // skip incorrect lines
        _codePage = static_cast< word >( std::strtoul( skipWS( p ), NULL, 10 ) );
        _useDBCS = false;
        p = std::strtok( NULL, " \t" );     // get dbcs flag
        if( p != NULL ) {
            _useDBCS = ( std::strtoul( skipWS( p ), NULL, 10 ) != 0 );
        }
        break;
    }
    std::fclose( nlsconf );
    if( fn2 == NULL ) {
        // if error or locale not found then set default US
        _country = 1;
        _codePage = 437;
        _useDBCS = false;
        fn1 = "en_US.nls";
        fn2 = "entity.txt";
    }
    _nlsFileName = path + std::string( fn1 );
    _entityFileName = path + std::string( fn2 );
}

CountryDef::dword CountryDef::write( OutFile *out ) const
/*********************************************************/
{
    dword start = out->tell();
    if( out->put( _size ) )
        throw FatalError( ERR_WRITE );
    if( out->put( static_cast< byte >( _type ) ) )
        throw FatalError( ERR_WRITE );
    if( out->put( _format ) )
        throw FatalError( ERR_WRITE );
    if( out->put( _value ) )
        throw FatalError( ERR_WRITE );
    if( out->put( _country ) )
        throw FatalError( ERR_WRITE );
    if( out->put( _codePage ) )
        throw FatalError( ERR_WRITE );
    if( out->put( _reserved ) )
        throw FatalError( ERR_WRITE );
    return( start );
}
