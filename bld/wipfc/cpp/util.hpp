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

#ifndef UTIL_INCLUDED
#define UTIL_INCLUDED

#include <string>

#if defined( __UNIX__ ) || defined( __APPLE__ )
#define PATH_LIST_SEPARATORS    ":;"
#define PATH_SEPARATOR          '/'
#define PATH_PARENT_REF         "../"
#else
#define PATH_LIST_SEPARATORS    ";"
#define PATH_SEPARATOR          '\\'
#define PATH_PARENT_REF         "..\\"
#endif

void killQuotes( char * text );
void killQuotes( wchar_t * text );
void killQuotes( std::string& val );
void killQuotes( std::wstring& val );
void splitAttribute( const std::wstring& text, std::wstring& key, std::wstring& value);
void killEOL( char * text );
void killEOL( wchar_t * text );
std::string canonicalPath( char* arg );
int wtomb_char( char *mbc, wchar_t wc );
int mbtow_char( wchar_t *wc, const char *mbc, std::size_t len );
std::wint_t read_wchar( std::FILE *fp );
std::size_t wtomb_cstring( char *mbc, const wchar_t *wc, std::size_t len );
std::size_t mbtow_cstring( wchar_t *wc, const char *mbc, std::size_t len );
void wtomb_string( const std::wstring& input, std::string& output );
void mbtow_string( const std::string& input, std::wstring& output );
char *skipWS( char *text );
wchar_t *skipWS( wchar_t *text );

#endif //UTIL_INCLUDED
