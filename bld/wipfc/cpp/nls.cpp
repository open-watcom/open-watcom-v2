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
* Description:  NLS data
*
****************************************************************************/


#include "wipfc.hpp"
#include <cstdlib>
#include <cstring>
#if defined( __UNIX__ ) || defined( __APPLE__ )
    #include <clocale>
#else
    #include <mbctype.h>
#endif
#include "errors.hpp"
#include "env.hpp"
#include "nls.hpp"
#include "util.hpp"
#include "uniutil.hpp"


Nls::Nls( const char *loc ) : _bytes( 0 )
{
    _sbcsG._type = WIPFC::GRAPHIC;
    _dbcsG._type = WIPFC::GRAPHIC;
    _sbcsT.setDefaultBits( WIPFC::TEXT );
    _sbcsG.setDefaultBits( WIPFC::GRAPHIC );
    setLocalization( loc );
}

/*****************************************************************************/
void Nls::readEntityFile( const std::string& sfname )
{
    char        buffer[256 * 2];
    wchar_t     text[256];

    std::FILE* entty( std::fopen( sfname.c_str(), "r" ) );
    if( entty == NULL )
        throw FatalError( ERR_COUNTRY );
    while( std::fgets( buffer, sizeof( buffer ), entty ) ) {
        std::size_t len = std::strlen( buffer );
        killEOL( buffer + len - 1 );
        len = mbtow_cstring( text, buffer, sizeof( text ) / sizeof( wchar_t ) - 1 );
        if( len == static_cast< std::size_t >( -1 ) ) {
            std::fclose( entty );
            throw FatalError( ERR_T_CONV );
        }
        _entityMap.insert( std::map< std::wstring, wchar_t >::value_type( std::wstring( text + 1 ), text[0] ) );
    }
    std::fclose( entty );
}
/*****************************************************************************/
void Nls::readNLSFile( const std::string& sfname )
{
    char        sbuffer[256 * 2];
    wchar_t     keyword[256];
    wchar_t     *value;
    bool        doGrammar( false );

    std::FILE *nls = std::fopen( sfname.c_str(), "r" );
    if( nls == NULL )
        throw FatalError( ERR_LANG );
    _cgraphicFont.setCodePage( _country.codePage() );
    while( std::fgets( sbuffer, sizeof( sbuffer ), nls ) ) {
        std::size_t len( std::strlen( sbuffer ) );
        killEOL( sbuffer + len - 1 );
        len = mbtow_cstring( keyword, sbuffer, sizeof( keyword ) / sizeof( wchar_t ) - 1 );
        if( len == static_cast< std::size_t >( -1 ) )
            throw FatalError( ERR_T_CONV );
        if( keyword[0] == L'\0' )
            continue;               //skip blank lines
        if( keyword[0] == L'#' )
            continue;               //skip comments
        if( (value = std::wcschr( keyword, L'=' )) != NULL ) {
            *value++ = L'\0';
            if( doGrammar ) {
                if( std::wcscmp( keyword, L"Words" ) == 0 ) {
                    processGrammar( value );
                } else if ( std::wcscmp( keyword, L"RemoveNL" ) == 0 ) {
                    //FIXME: exclude these values from sbcs/dbcs table?
                }
            } else if( std::wcscmp( keyword, L"Note" ) == 0 ) {
                killQuotes( value );
                _noteText = std::wstring( value );
            } else if( std::wcscmp( keyword, L"Caution" ) == 0 ) {
                killQuotes( value );
                _cautionText = std::wstring( value );
            } else if( std::wcscmp( keyword, L"Warning" ) == 0 ) {
                killQuotes( value );
                _warningText = std::wstring( value );
            } else if( std::wcscmp( keyword, L"Reference" ) == 0 ) {
                killQuotes( value );
                _referenceText = std::wstring( value );
            } else if( std::wcscmp( keyword, L"olChars" ) == 0 ) {
                _olCh = std::wstring( value );
            } else if( std::wcscmp( keyword, L"olClose1" ) == 0 ) {
                _olClosers[0] = std::wstring( value );
            } else if( std::wcscmp( keyword, L"olClose2" ) == 0 ) {
                _olClosers[1] = std::wstring( value );
            } else if( std::wcscmp( keyword, L"ulItemId1" ) == 0 ) {
                _ulBul[0] = std::wstring( value );
            } else if( std::wcscmp( keyword, L"ulItemId2" ) == 0 ) {
                _ulBul[1] = std::wstring( value );
            } else if( std::wcscmp( keyword, L"ulItemId3" ) == 0 ) {
                _ulBul[2] = std::wstring( value );
            } else if( std::wcscmp( keyword, L"cgraphicFontFaceName" ) == 0 ) {
                killQuotes( value );
                _cgraphicFont.setFaceName( std::wstring( value ) );
            } else if( std::wcscmp( keyword, L"cgraphicFontWidth" ) == 0 ) {
                _cgraphicFont.setWidth( static_cast< word >( std::wcstol( value, 0, 10 ) ) );
            } else if( std::wcscmp( keyword, L"cgraphicFontHeight" ) == 0 ) {
                _cgraphicFont.setHeight( static_cast< word >( std::wcstol( value, 0, 10 ) ) );
            } else {
                // error: unknown keyword
            }
        } else if( std::wcscmp( keyword, L"Grammar" ) == 0 ) {
            doGrammar = true;
        } else if( std::wcscmp( keyword, L"eGrammar" ) == 0 ) {
            doGrammar = false;
        } else {
            // error: unknown keyword
        }
    }
    std::fclose( nls );
}
/*****************************************************************************/
void Nls::setLocalization( const char *loc)
{
    // TODO! MBCS<->UNICODE conversion for mbtow_char and wtomb_char must be setup
    // instead of existing code which rely on host OS locale support.
    // By example proper characters encoding for US INF Documentation files is
    // DOS codepage 850, but on Linux it is handled as ISO-8859-1 or UTF-8 in
    // dependency how host locale are configured. It is wrong!
    // Correct solution is to use DOS codepage 850 for US on any host OS.
    // It requires to define appropriate MBCS<->UNICODE conversion tables as part of WIPFC.
    _country.nlsConfig( loc );
    // TODO! Following code must be replaced by setup MBCS<->UNICODE conversion tables
    // for mbtow_char and wtomb_char
#if defined( __UNIX__ ) || defined( __APPLE__ )
    std::setlocale( LC_ALL, loc );  //this doesn't really do anything in OW either
#endif
#if !defined( __UNIX__ ) && !defined( __APPLE__ )
    _setmbcp( _country.codePage() ); //doesn't do much of anything in OW
#endif
    readNLSFile( _country.nlsFileName() );
    readEntityFile( _country.entityFileName() );
}
/*****************************************************************************/
void Nls::processGrammar( wchar_t *buffer )
{
    if( _grammarChars.empty() ) {
        _grammarChars.reserve( 26 + 26 + 10 );
    }
#if defined( _MSC_VER ) && ( _MSC_VER < 1910 ) && !defined( _WCSTOK_DEPRECATED )
    wchar_t* tok( std::wcstok( buffer, L"+" ) );
#else
    wchar_t* p;
    wchar_t* tok( std::wcstok( buffer, L"+", &p ) );
#endif
    while( tok ) {
        if( std::wcslen( tok ) > 1 ) {
            // characters range "chr1-chr2"
            // change this loop if we use RegExp
            wchar_t chr1( tok[0] );
            wchar_t chr2( tok[2] );
            for( wchar_t c = chr1; c <= chr2; ++c )
                _grammarChars += c;
            _dbcsT._ranges.push_back( static_cast< word >( chr1 ) );
            _dbcsT._ranges.push_back( static_cast< word >( chr2 ) );
        } else {
            // single character "chr"
            wchar_t chr( tok[0] );
            _grammarChars += chr;
            _dbcsT._ranges.push_back( static_cast< word >( chr ) );
            _dbcsT._ranges.push_back( static_cast< word >( chr ) );
        }
#if defined( _MSC_VER ) && ( _MSC_VER < 1910 ) && !defined( _WCSTOK_DEPRECATED )
        tok = std::wcstok( 0, L"+" );
#else
        tok = std::wcstok( 0, L"+", &p );
#endif
    }
}
/*****************************************************************************/
wchar_t Nls::entity( const std::wstring& key )
{
    EntityIter pos( _entityMap.find( key ) );
    if( pos == _entityMap.end() )
        throw Class2Error( ERR2_SYMBOL );
    return pos->second;
}
/*****************************************************************************/
STD1::uint32_t Nls::write( std::FILE *out )
{
    _bytes = _country.size();
    dword start = _country.write( out );
    if( _country.useDBCS() ) {
        _dbcsT.write( out );
        _bytes += _dbcsT._size;
        _dbcsG.write( out );
        _bytes += _dbcsG._size;
    } else {
        _sbcsT.write( out );
        _bytes += _sbcsT._size;
        _sbcsG.write( out );
        _bytes += _sbcsG._size;
    }
    return( start );
}
/*****************************************************************************/
void Nls::SbcsGrammarDef::setDefaultBits( WIPFC::NLSRecType rectype )
{
    static const unsigned char defbits[2][32] = {
        { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xc0,
          0x7f, 0xff, 0xff, 0xe0, 0x7f, 0xff, 0xff, 0xe0,
          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
        { 0x7f, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00,
          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
          0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
          0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff }
    };
    std::memcpy( this->_bits, &defbits[rectype - WIPFC::TEXT][0], 32 );
}
/*****************************************************************************/
STD1::uint32_t Nls::SbcsGrammarDef::write( std::FILE *out ) const
{
    dword start = std::ftell( out );
    if( std::fwrite( &_size, sizeof( _size ), 1, out ) != 1 )
        throw FatalError( ERR_WRITE );
    byte type = static_cast< byte >( _type );
    if( std::fwrite( &type, sizeof( type ), 1, out ) != 1 )
        throw FatalError( ERR_WRITE );
    if( std::fwrite( &_format, sizeof( _format ), 1, out ) != 1 )
        throw FatalError( ERR_WRITE );
    if( std::fwrite( _bits, sizeof( _bits[0] ), sizeof( _bits ) / sizeof( _bits[0] ), out ) != sizeof( _bits ) / sizeof( _bits[0] ) )
        throw FatalError( ERR_WRITE );
    return( start );
}
/*****************************************************************************/
STD1::uint32_t Nls::DbcsGrammarDef::write( std::FILE *out )
{
    dword start = std::ftell( out );
    _size = static_cast< word >( sizeof( word ) + 2 * sizeof( byte ) + _ranges.size() * sizeof( word ) );
    if( std::fwrite( &_size, sizeof( _size ), 1, out ) != 1 )
        throw FatalError( ERR_WRITE );
    byte type = static_cast< byte >( _type );
    if( std::fwrite( &type, sizeof( type ), 1, out ) != 1 )
        throw FatalError( ERR_WRITE );
    if( std::fwrite( &_format, sizeof( _format ), 1, out ) != 1 )
        throw FatalError( ERR_WRITE );
    for( std::vector< word >::const_iterator itr = _ranges.begin(); itr != _ranges.end(); ++itr ) {
        if( std::fwrite( &(*itr), sizeof( word ), 1, out ) != 1 ) {
            throw FatalError( ERR_WRITE );
        }
    }
    return( start );
}
