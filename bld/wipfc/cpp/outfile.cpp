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
* Description:  Output file processing
*
****************************************************************************/


#include "wipfc.hpp"
#include <cstdlib>
#include <climits>
#include <cstring>
#include "fnt.hpp"
#include "errors.hpp"
#include "outfile.hpp"
#include "util.hpp"
#include "nls.hpp"
#include "iculoadr.hpp"


OutFile::OutFile( const std::string& sfname, Nls *nls ) : _ufp( NULL ), _codePage( DEFAULT_CODEPAGE )
{
    UErrorCode err = U_ZERO_ERROR;
    std::FILE *fp;

    _codePage = nls->codePage();

    fp = std::fopen( sfname.c_str(), "wb" );
    if( fp != NULL ) {
        _ufp = new UNIFILE;
        _ufp->_fp = fp;
        _icu = nls->getICU();
        _converter = _icu->clone( &err );
    }
}

OutFile::~OutFile()
{
    if( _ufp != NULL ) {
        std::fclose( _ufp->_fp );
        delete _ufp;
        _icu->close( _converter );
    }
}

bool OutFile::codePage( word codePage )
{
    if( codePage == DEFAULT_CODEPAGE )
        codePage = _codePage;
    return( write( &codePage, sizeof( codePage ), 1 ) );
}

std::string OutFile::wtomb_string( const std::wstring& input )
/************************************************************/
{
    std::string output;
    char ch[MB_LEN_MAX + 1];
    char *target;
    UChar uc[1];
    const UChar *source;
    UErrorCode err = U_ZERO_ERROR;

    for( std::size_t index = 0; index < input.size(); ++index ) {
        target = ch;
        source = uc;
        uc[0] = input[index];
        _icu->fromUnicodeToMBCS( _converter, &target, target + MB_LEN_MAX, &source, source + 1, NULL, TRUE, &err );
        *target = '\0';
        output += ch;
    }
    return( output );
}
