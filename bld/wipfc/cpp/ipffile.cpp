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
#include "ipffile.hpp"
#include "errors.hpp"
#include "uniutil.hpp"


IpfFile::IpfFile( const std::wstring* wfname ) : IpfData(), _fileName( wfname ),
    _ungottenChar( WEOF ), _ungotten( false )
{
    std::string sfname;
    def_wtomb_string( *_fileName, sfname );
    if( (_stream = std::fopen( sfname.c_str(), "rb" )) == 0 ) {
        throw FatalIOError( ERR_OPEN, *_fileName );
    }
}

IpfFile::IpfFile( const std::string& sfname, const std::wstring* wfname ) : IpfData(), _fileName( wfname ),
    _ungottenChar( WEOF ), _ungotten( false )
{
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
        ch = read_wchar( _stream );
    }
    if( ch == L'\r' ) {
        ch = read_wchar( _stream );
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
