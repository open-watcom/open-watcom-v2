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
#include <string>
#include <climits>
#if defined( __UNIX__ ) || defined( __APPLE__ )
#else
    #include <mbctype.h>
#endif
#include "uniutil.hpp"
#include "errors.hpp"


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

void def_wtomb_string( const std::wstring& input, std::string& output )
/*********************************************************************/
{
    for( std::size_t index = 0; index < input.size(); ++index ) {
        char ch[ MB_LEN_MAX + 1 ];
        int  bytes( std::wctomb( &ch[ 0 ], input[ index ] ) );
        if( bytes == -1 )
            throw FatalError( ERR_T_CONV );
        ch[ bytes ] = '\0';
        output += ch;
    }
}

void def_mbtow_string( const std::string& input, std::wstring& output )
/*********************************************************************/
{
    int consumed;

    for( std::size_t index = 0; index < input.size(); index += consumed ) {
        wchar_t wch;
        consumed = std::mbtowc( &wch, input.data() + index, MB_CUR_MAX );
        if( consumed == -1 )
            throw FatalError( ERR_T_CONV );
        output += wch;
    }
}
