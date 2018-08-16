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
* Description:  IPF Input file reader
*
****************************************************************************/

#include "wipfc.hpp"
#include <cstdlib>
#include <cstring>
#include <climits>
#if defined( __UNIX__ ) || defined( __APPLE__ )
    #include <clocale>
#else
    #include <mbctype.h>
#endif
#include "ipffile.hpp"
#include "errors.hpp"
#include "util.hpp"
#include "outfile.hpp"
#include "nls.hpp"


IpfFile::IpfFile( const std::wstring* wfname, Nls *nls ) : IpfData(),
    _fileName( wfname ), _ungottenChar( WEOF ), _ungotten( false )
{
    (void)nls;

    std::string sfname( def_wtomb_string( *_fileName ) );
    if( (_stream = std::fopen( sfname.c_str(), "rb" )) == 0 ) {
        throw FatalIOError( ERR_OPEN, *_fileName );
    }
}

IpfFile::IpfFile( const std::string& sfname, const std::wstring* wfname, Nls *nls ) : IpfData(),
    _fileName( wfname ), _ungottenChar( WEOF ), _ungotten( false )
{
    (void)nls;

    if( (_stream = std::fopen( sfname.c_str(), "rb" )) == 0 ) {
        throw FatalIOError( ERR_OPEN, *_fileName );
    }
}
/*****************************************************************************/
//Read a character
//Returns EOB if end-of-file reached
std::wint_t IpfFile::get()
{
    std::wint_t     ch;

    if( _ungotten ) {
        ch = _ungottenChar;
        _ungotten = false;
    } else {
        ch = getwc();
    }
    if( ch == L'\r' ) {
        ch = getwc();
    }
    incCol();
    if( ch == L'\n' ) {
        incLine();
        resetCol();
    } else if( ch == WEOF ) {
        ch = EOB;
        if( !std::feof( _stream ) ) {
            throw FatalIOError( ERR_READ, *_fileName );
        }
    }
    return( ch );
}
/*****************************************************************************/
void IpfFile::unget( wchar_t ch )
{
    //std::ungetwc( ch, _stream );
    _ungottenChar = ch;
    _ungotten = true;
    decCol();
    if( ch == L'\n' ) {
        decLine();
    }
}

static int mbtow_char( wchar_t *wc, const char *mbc, std::size_t len )
/********************************************************************/
{
    // TODO! must be converted by selected MBCS->UNICODE conversion table
    // which is independent from the host user locale
    return( std::mbtowc( wc, mbc, len ) );
}

std::wint_t IpfFile::getwc()
/**************************/
{
    wchar_t ch;

#if defined( __UNIX__ ) || defined( __APPLE__ )
    // TODO! read MBCS character and convert it to UNICODE by mbtow_char
    ch = std::fgetwc( _stream );
#else
    char    mbc[ MB_LEN_MAX ];
    if( std::fread( &mbc[0], sizeof( char ), 1, _stream ) != 1 )
        return( WEOF );
    if( _ismbblead( mbc[0] ) ) {
        if( std::fread( &mbc[1], sizeof( char ), 1, _stream ) != 1 ) {
            return( WEOF );
        }
    }
    if( mbtow_char( &ch, mbc, MB_CUR_MAX ) < 0 ) {
        throw FatalError( ERR_T_CONV );
    }
#endif
    return( ch );
}

void IpfFile::mbtow_string( const std::string& input, std::wstring& output )
/**************************************************************************/
{
    int consumed;

    output.clear();
    for( std::size_t index = 0; index < input.size(); index += consumed ) {
        wchar_t wch;
        consumed = mbtow_char( &wch, input.data() + index, MB_CUR_MAX );
        if( consumed == -1 )
            throw FatalError( ERR_T_CONV );
        output += wch;
    }
}

const wchar_t * IpfFile::gets( std::wstring& wbuffer )
/****************************************************/
{
    char    sbuffer[512];

    if( std::fgets( sbuffer, sizeof( sbuffer ), _stream ) != NULL ) {
        std::size_t len = std::strlen( sbuffer );
        killEOL( sbuffer + len - 1 );
        std::string buffer( sbuffer );
        mbtow_string( sbuffer, wbuffer );
        return( wbuffer.c_str() );
    }
    return( NULL );
}
