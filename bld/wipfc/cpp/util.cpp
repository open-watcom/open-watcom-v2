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
* Description:  Some utility functions
*
****************************************************************************/

#if defined( __unix__ ) && !defined( __UNIX__ )
    #define __UNIX__ __unix__
#endif
#include <cstdlib>
#include <cstring>
#include <memory>
#include <string>
#ifdef __UNIX__
    #include <unistd.h>
#else
    #include <direct.h>
#endif
#include "errors.hpp"

void killQuotes( char * text )
{
    if( *text == '"' || *text == '\'' ) {
        char quote = *text;
        std::memmove( text, text + 1, std::strlen( text ) * sizeof( char ) );
        char *end( std::strrchr( text, quote ) );
        if ( end )
            *end = '\0';
    }
}
/*****************************************************************************/
void killQuotes( wchar_t * text )
{
    if( *text == L'"' || *text == '\'' ) {
        wchar_t quote = *text;
        std::memmove( text, text + 1, std::wcslen( text ) * sizeof( wchar_t ) );
        wchar_t *end( std::wcsrchr( text, quote ) );
        if ( end )
            *end = L'\0';
    }
}
/*****************************************************************************/
void killQuotes( std::string& val )
{
    if ( val[0] == '"' || val[0] == '\'' )
        {
        val.erase( 0, 1 );
        if( val[ val.size() - 1 ] == '"' || val[ val.size() - 1 ] == '\'' )
            val.erase( val.size() - 1, 1 );
        }
}
/*****************************************************************************/
void killQuotes( std::wstring& val )
{
    if ( val[0] == L'"' || val[0] == L'\'' )
        {
        val.erase( 0, 1 );
        if( val[ val.size() - 1 ] == L'"' || val[ val.size() - 1 ] == L'\'' )
            val.erase( val.size() - 1, 1 );
        }
}
/*****************************************************************************/
void splitAttribute( const std::wstring& text, std::wstring& key, std::wstring& value)
{
    std::wstring::size_type index( text.find( '=', 0 ) );
    key = text.substr( 0, index );
    value = text.substr( index + 1 );
    killQuotes( value );
}
/*****************************************************************************/
//Allow for files with non-native end-of-lines
void killEOL( char * text )
{
    if( *text == '\n' || *text == '\r' ) {  //CR or LF
        *text = '\0';
        --text;
    }
    if( *text == '\n' || *text == '\r' ) {  //CRLF (or LFCR)
        *text = '\0';
    }
}
/*****************************************************************************/
//Allow for files with non-native end-of-lines
void killEOL( wchar_t * text )
{
    if( *text == L'\n' || *text == L'\r' ) {    //CR or LF
        *text = L'\0';
        --text;
    }
    if( *text == L'\n' || *text == L'\r' ) {    //CRLF (or LFCR)
        *text = L'\0';
    }
}
/*****************************************************************************/
std::string canonicalPath( char* arg )
{
    std::auto_ptr< char > cwd( ::getcwd( 0, 0 ) );
    std::string fullpath( cwd.get() );
    std::string inFile( arg );
#ifdef __UNIX__
    const char* srchstr = "../";
    char sep = '/';
#else
    const char* srchstr = "..\\";
    char sep = '\\';
#endif
    std::string::size_type idx1( inFile.find( srchstr ) );
    if( idx1 == 0 ) {
        while( idx1 == 0 ) {                    //must be at start of line
            std::string::size_type idx2( fullpath.rfind( sep ) );
            if( idx2 != std::string::npos ) {
                fullpath.erase( idx2 );
                inFile.erase( idx1, 3 );
            }
            else if( !fullpath.empty() ) {
#ifdef __UNIX__
                idx2 = 0;
#else
                idx2 = fullpath.find( ':' );    //don't kill drive
                if( idx2 != std::string::npos )
                    ++idx2;
#endif
                fullpath.erase( idx2 );
                inFile.erase( 0, 3 );
                break;
            }
            idx1 = inFile.find( srchstr );
        }
        fullpath += sep;
        fullpath += inFile;
    }
    else
        fullpath = inFile;
#ifndef __UNIX__
    if( fullpath.size() > PATH_MAX )
        throw FatalError( ERR_PATH_MAX );
#endif
    return fullpath;
}
/*****************************************************************************/
void wtombstring( const std::wstring& input, std::string& output )
{
    for( size_t index = 0; index < input.size(); ++index ) {
        char ch[ MB_CUR_MAX + 1 ];
        int  bytes( std::wctomb( &ch[ 0 ], input[ index ] ) );
        if( bytes == -1 )
            throw FatalError( ERR_T_CONV );
        ch[ bytes ] = '\0';
        output += ch;
    }
}
/*****************************************************************************/
void mbtowstring( const std::string& input, std::wstring& output )
{
    size_t index( 0 );
    while( index < input.size() ) {
        wchar_t wch;
        int consumed( std::mbtowc( &wch, input.data() + index, MB_CUR_MAX ) );
        if( consumed == -1 )
            throw FatalError( ERR_T_CONV );
        output += wch;
        index += consumed;
    }
}
