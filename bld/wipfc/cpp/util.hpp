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

#define ERROR_CNV   static_cast<std::size_t>( -1 )

extern void         killQuotes( char *text );
extern void         killQuotes( wchar_t *text );
extern void         killQuotes( std::string& val );
extern void         killQuotes( std::wstring& val );
extern void         splitAttribute( const std::wstring& text, std::wstring& key, std::wstring& value);
extern bool         killEOL( char *text, bool kill );
extern bool         killEOL( wchar_t *text, bool kill );
extern std::string  canonicalPath( char *arg );
extern char         *skipWS( char *text );
extern wchar_t      *skipWS( wchar_t *text );

extern std::string  def_wtomb_string( const std::wstring& input );
extern void         def_mbtow_string( const std::string& input, std::wstring& output );

#endif //UTIL_INCLUDED
