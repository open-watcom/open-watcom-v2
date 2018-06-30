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
* Description:  Some utility functions
*
****************************************************************************/


#include "wipfc.hpp"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <string>
#include <climits>
#include <cctype>
#include <cwctype>
#if defined( __UNIX__ ) || defined( __APPLE__ )
    #include <unistd.h>
#else
    #include <direct.h>
    #include <mbctype.h>
#endif
#include "util.hpp"
#include "errors.hpp"

#ifndef HAVE_CONFIG_H
#include "clibext.h"
#endif


void killQuotes( char * text )
/****************************/
{
    if( *text == '"' || *text == '\'' ) {
        char quote = *text;
        std::memmove( text, text + 1, std::strlen( text ) * sizeof( char ) );
        char *end( std::strrchr( text, quote ) );
        if ( end )
            *end = '\0';
    }
}

void killQuotes( wchar_t * text )
/*******************************/
{
    if( *text == L'"' || *text == '\'' ) {
        wchar_t quote = *text;
        std::memmove( text, text + 1, std::wcslen( text ) * sizeof( wchar_t ) );
        wchar_t *end( std::wcsrchr( text, quote ) );
        if ( end )
            *end = L'\0';
    }
}

void killQuotes( std::string& val )
/*********************************/
{
    if ( val[0] == '"' || val[0] == '\'' )
        {
        val.erase( 0, 1 );
        if( val[ val.size() - 1 ] == '"' || val[ val.size() - 1 ] == '\'' )
            val.erase( val.size() - 1, 1 );
        }
}

void killQuotes( std::wstring& val )
/**********************************/
{
    if ( val[0] == L'"' || val[0] == L'\'' )
        {
        val.erase( 0, 1 );
        if( val[ val.size() - 1 ] == L'"' || val[ val.size() - 1 ] == L'\'' )
            val.erase( val.size() - 1, 1 );
        }
}

void splitAttribute( const std::wstring& text, std::wstring& key, std::wstring& value )
/*************************************************************************************/
{
    std::wstring::size_type index( text.find( '=', 0 ) );
    key = text.substr( 0, index );
    value = text.substr( index + 1 );
    killQuotes( value );
}

void killEOL( char *text )
/************************/
//Allow for files with non-native end-of-lines
{
    if( *text == '\n' || *text == '\r' ) {  //CR or LF
        *text-- = '\0';
    }
    if( *text == '\n' || *text == '\r' ) {  //CRLF (or LFCR)
        *text = '\0';
    }
}

void killEOL( wchar_t *text )
/***************************/
//Allow for files with non-native end-of-lines
{
    if( *text == L'\n' || *text == L'\r' ) {    //CR or LF
        *text-- = L'\0';
    }
    if( *text == L'\n' || *text == L'\r' ) {    //CRLF (or LFCR)
        *text = L'\0';
    }
}

std::string canonicalPath( char* arg )
/************************************/
{
    std::auto_ptr< char > cwd( ::getcwd( 0, 0 ) );
    std::string fullpath( cwd.get() );
    std::string inFile( arg );
    bool no_parent_ref = true;
    // parent reference "../" must be on the begining then remove all levels
    for( ; inFile.find( PATH_PARENT_REF ) == 0; inFile.erase( 0, 3 ) ) {
        no_parent_ref = false;
        std::string::size_type idx2( fullpath.rfind( PATH_SEPARATOR ) );
        if( idx2 != std::string::npos ) {
            fullpath.erase( idx2 );
        } else if( !fullpath.empty() ) {
#if !defined( __UNIX__ ) && !defined( __APPLE__ )
            if( fullpath[fullpath.size() - 1] == ':' )
                continue;                   // don't kill drive
            idx2 = fullpath.find( ':' );    // don't kill drive
            if( idx2 != std::string::npos ) {
                fullpath.erase( idx2 + 1 );
                continue;
            }
#endif
            fullpath.erase();
        }
    }
    if( no_parent_ref ) {
        fullpath = inFile;
    } else {
        fullpath += PATH_SEPARATOR;
        fullpath += inFile;
    }
#if !defined( __UNIX__ ) && !defined( __APPLE__ )
    if( fullpath.size() > PATH_MAX )
        throw FatalError( ERR_PATH_MAX );
#endif
    return( fullpath );
}

char *skipWS( char *text )
/************************/
{
    while( std::isspace( *(unsigned char *)text ) )
        text++;
    return( text );
}

wchar_t *skipWS( wchar_t *text )
/******************************/
{
    while( std::iswspace( *text ) )
        text++;
    return( text );
}

int wtomb_char( char *mbc, wchar_t wc )
/*************************************/
{
    // TODO! must be converted by selected UNICODE->MBCS conversion table
    // which is independent from the host OS locale
    // conversion must be selected by Nls::setLocalization
    return( std::wctomb( mbc, wc ) );
}

int mbtow_char( wchar_t *wc, const char *mbc, std::size_t len )
/*************************************************************/
{
    // TODO! must be converted by selected MBCS->UNICODE conversion table
    // which in independent from the host OS locale
    // conversion must be selected by Nls::setLocalization
    return( std::mbtowc( wc, mbc, len ) );
}

std::wint_t read_wchar( std::FILE *fp )
/*************************************/
{
    wchar_t ch;

#if defined( __UNIX__ ) || defined( __APPLE__ )
    // TODO! read MBCS character and convert it to UNICODE by mbtow_char
    ch = std::fgetwc( fp );
#else
    char    mbc[ MB_LEN_MAX ];
    if( std::fread( &mbc[0], sizeof( char ), 1, fp ) != 1 )
        return( WEOF );
    if( _ismbblead( mbc[0] ) ) {
        if( std::fread( &mbc[1], sizeof( char ), 1, fp ) != 1 ) {
            return( WEOF );
        }
    }
    if( mbtow_char( &ch, mbc, MB_CUR_MAX ) < 0 ) {
        throw FatalError( ERR_T_CONV );
    }
#endif
    return( ch );
}

std::size_t wtomb_cstring( char *dst_mbc, const wchar_t *src_wc, std::size_t len )
/********************************************************************************/
{
    std::size_t dst_len = 0;
    char        mbc[MB_LEN_MAX + 1];
    int         bytes;

    while( len > 0 && *src_wc != L'\0' ) {
        bytes = wtomb_char( mbc, *src_wc );
        if( bytes == -1 || bytes > len )
            return( static_cast<std::size_t>( -1 ) );
        std::memcpy( dst_mbc, mbc, bytes );
        dst_mbc += bytes;
        dst_len += bytes;
        len -= bytes;
        src_wc++;
    }
    *dst_mbc = '\0';
    return( dst_len );
}

std::size_t mbtow_cstring( wchar_t *dst_wc, const char *src_mbc, std::size_t len )
/********************************************************************************/
{
    std::size_t dst_len = 0;
    int         bytes;

    while( len > 0 && *src_mbc != '\0' ) {
        bytes = mbtow_char( dst_wc, src_mbc, MB_LEN_MAX );
        if( bytes == -1 )
            return( static_cast<std::size_t>( -1 ) );
        dst_wc++;
        dst_len++;
        len--;
        src_mbc += bytes;
    }
    *dst_wc = L'\0';
    return( dst_len );
}

void wtomb_string( const std::wstring& input, std::string& output )
/*****************************************************************/
{
    for( std::size_t index = 0; index < input.size(); ++index ) {
        char ch[ MB_LEN_MAX + 1 ];
        int  bytes( wtomb_char( &ch[ 0 ], input[ index ] ) );
        if( bytes == -1 )
            throw FatalError( ERR_T_CONV );
        ch[ bytes ] = '\0';
        output += ch;
    }
}

void mbtow_string( const std::string& input, std::wstring& output )
/*****************************************************************/
{
    int consumed;

    for( std::size_t index = 0; index < input.size(); index += consumed ) {
        wchar_t wch;
        consumed = mbtow_char( &wch, input.data() + index, MB_CUR_MAX );
        if( consumed == -1 )
            throw FatalError( ERR_T_CONV );
        output += wch;
    }
}
