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
#include "ipffile.hpp"
#include "errors.hpp"
#include "util.hpp"
#include "outfile.hpp"
#include "nls.hpp"
#include "iculoadr.hpp"


IpfFile::IpfFile( const std::wstring* wfname, Nls *nls ) : IpfData(),
    _fileName( wfname ), _ungottenChar( WEOF ), _ungotten( false ), _pos( 0 )
{
    UErrorCode err = U_ZERO_ERROR;

    (void)nls;

    std::string sfname( def_wtomb_string( *_fileName ) );
    if( (_stream = std::fopen( sfname.c_str(), "rb" )) == 0 ) {
        throw FatalIOError( ERR_OPEN, *_fileName );
    }
    _icu = nls->getICU();
    _converter = _icu->clone( &err );
}

IpfFile::IpfFile( const std::string& sfname, const std::wstring* wfname, Nls *nls ) : IpfData(),
    _fileName( wfname ), _ungottenChar( WEOF ), _ungotten( false ), _pos( 0 )
{
    UErrorCode err = U_ZERO_ERROR;

    (void)nls;

    if( (_stream = std::fopen( sfname.c_str(), "rb" )) == 0 ) {
        throw FatalIOError( ERR_OPEN, *_fileName );
    }
    _icu = nls->getICU();
    _converter = _icu->clone( &err );
}

IpfFile::~IpfFile()
{
    if( _stream ) {
        std::fclose( _stream );
    }
    _wbuffer.erase();
    _icu->close( _converter );
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

std::wint_t IpfFile::getwc()
/**************************/
{
    // read UNICODE character from internal buffer
    if( _pos >= _wbuffer.size() ) {
        _pos = 0;
        if( gets( false ) == NULL ) {
            return( WEOF );
        }
    }
    return _wbuffer[_pos++];
}

const std::wstring * IpfFile::gets( bool removeEOL )
/**************************************************/
{
    UErrorCode  err = U_ZERO_ERROR;
    char        sbuffer[512];
    bool        eol;
    std::string buffer;

    _wbuffer = L"";
    eol = false;
    // read MBCS/SBCS/UTF8 input line from file
    while( !eol && std::fgets( sbuffer, sizeof( sbuffer ), _stream ) != NULL ) {
        std::size_t len = std::strlen( sbuffer );
        eol = killEOL( sbuffer + len - 1, removeEOL );
        buffer += sbuffer;
    }
    if( buffer.size() > 0 || eol ) {
        // input line conversion from MBCS/SBCS/UTF8 to UNICODE (by ICU converter)
        const char *start = buffer.c_str();
        const char *end = start + buffer.size();
        while( start < end ) {
            UChar32 uc;
            uc = _icu->getNextUChar( _converter, &start, end, &err );
            _wbuffer += static_cast< wchar_t >( uc );
        }
        return( &_wbuffer );
    }
    return( NULL );
}
