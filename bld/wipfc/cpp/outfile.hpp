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

#ifndef OUTFILE_INCLUDED
#define OUTFILE_INCLUDED

#include <vector>
#include <string>


class Nls;     // forward reference
class ICULoader;
struct UConverter;

struct UNIFILE {
    std::FILE   *_fp;
};

class OutFile {
public:
    OutFile( const std::string& sfname, Nls *nls );
    ~OutFile();

    dword tell() { return std::ftell( _ufp->_fp ); };
    bool write( const void *buffer, std::size_t size, std::size_t cnt ) { return( std::fwrite( buffer, size, cnt, _ufp->_fp ) != cnt ); };
    bool putc( char data ) { return( std::fputc( data, _ufp->_fp ) == EOF ); };
    bool put( byte data ) { return( std::fwrite( &data, sizeof( data ), 1, _ufp->_fp ) != 1 ); };
    bool put( word data ) { return( std::fwrite( &data, sizeof( data ), 1, _ufp->_fp ) != 1 ); };
    bool put( dword data ) { return( std::fwrite( &data, sizeof( data ), 1, _ufp->_fp ) != 1 ); };
    bool put( const std::string& data ) { return( std::fwrite( &data[0], 1, data.size(), _ufp->_fp ) != data.size() ); };
    bool put( const std::vector< byte >& data ) { return( std::fwrite( &data[0], sizeof( byte ), data.size(), _ufp->_fp ) != data.size() ); };
    bool put( const std::vector< word >& data ) { return( std::fwrite( &data[0], sizeof( word ), data.size(), _ufp->_fp ) != data.size() ); };
    bool put( const std::vector< dword >& data ) { return( std::fwrite( &data[0], sizeof( dword ), data.size(), _ufp->_fp ) != data.size() ); };
    bool codePage( word codePage );
    int seek( dword offset, int where ) { return std::fseek( _ufp->_fp, offset, where ); };
    // UNICODE->MBCS conversion
    std::string wtomb_string( const std::wstring& input );

private:
    UNIFILE*            _ufp;
    word                _codePage;
    ICULoader           *_icu;
    UConverter          *_converter;
};

#endif
