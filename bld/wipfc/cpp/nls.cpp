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
* Description:  NLS data
*
****************************************************************************/

#if defined(__unix__) && !defined(__UNIX__)
    #define __UNIX__ __unix__
#endif
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

extern class Env Environment;

Nls::Nls( const char *loc ) : bytes( 0 ), useDBCS( false )
{
    sbcsG.type = GRAPHIC;
    dbcsG.type = GRAPHIC;
    sbcsT.setDefaultBits( TEXT );
    sbcsG.setDefaultBits( GRAPHIC );
    setLocalization( loc );
}

std::string Nls::readNlsConfFile( std::FILE *nlsconf, const char *loc )
{
    char        buffer[256];
    char        *p;
    char        *fn;

    while( std::fgets( buffer, sizeof( buffer ), nlsconf ) ) {
        std::size_t len = std::strlen( buffer );
        killEOL( buffer + len - 1 );
        p = skipWS( buffer );
        if( p[0] == '\0' )
            continue;                       // skip blank lines
        if( p[0] == '#' )
            continue;                       // skip comment lines
        p = std::strtok( buffer, " \t" );   // get locale
        if( p == NULL || std::strcmp( p, loc ) != 0 )
            continue;
        p = std::strtok( NULL, " \t" );     // get nls file
        if( p == NULL )
            continue;                       // skip incorrect lines
        fn = skipWS( p );
        p = std::strtok( NULL, " \t" );     // get country
        if( p == NULL )
            continue;                       // skip incorrect lines
        p = skipWS( p );
        country.country = static_cast< STD1::uint16_t >( std::strtoul( p, NULL, 10 ) );
        p = std::strtok( NULL, " \t" );     // get codepage
        if( p == NULL )
            continue;                       // skip incorrect lines
        p = skipWS( p );
        country.codePage = static_cast< STD1::uint16_t >( std::strtoul( p, NULL, 10 ) );
        std::fclose( nlsconf );
        return( std::string( fn ) );
    }
    // if error or locale not found then set default US
    std::fclose( nlsconf );
    country.country = 1;
    country.codePage = 850;
    return( std::string( "en_US.nls" ) );
}

std::string Nls::getNlsFileName( const char *loc )
{
    std::string path( Environment.value( "WIPFC" ) );

    if( path.length() )
#if !defined( __UNIX__ ) && !defined( __APPLE__ )
        path += '\\';
#else
        path += '/';
#endif
    path += "nlsconf.txt";
    std::FILE *nlsconf = std::fopen( path.c_str(), "r" );
    if( nlsconf == NULL )
        throw FatalError( ERR_NLSCONF );
    return( readNlsConfFile( nlsconf, loc ) );
}

/*****************************************************************************/
void Nls::setCodePage( STD1::uint16_t cp )
{
#if !defined( __UNIX__ ) && !defined( __APPLE__ )
    _setmbcp( cp ); //doesn't do much of anything in OW
#endif
    std::string path( Environment.value( "WIPFC" ) );
    if( path.length() )
#if !defined( __UNIX__ ) && !defined( __APPLE__ )
        path += '\\';
#else
        path += '/';
#endif
    path += "enti";
    if( cp == 850 || cp == 437 ) {
        path += "ty";
    } else {
        char code[6];
        std::sprintf( code, "%4.4d", cp );
        path.append( code, 4 );
    }
    path += ".txt";
    std::FILE* entty( std::fopen( path.c_str(), "r" ) );
    if( entty == NULL )
        throw FatalError( ERR_COUNTRY );
    readEntityFile( entty );
    std::fclose( entty );
    country.codePage = cp;
}
/*****************************************************************************/
void Nls::readEntityFile( std::FILE *entty )
{
    char    buffer[256 * 2];
    wchar_t text[256];
    int     offset;
    wchar_t c;
    while( std::fgets( buffer, sizeof( buffer ), entty ) ) {
        std::size_t len = std::strlen( buffer );
        killEOL( buffer + len - 1 );
        offset = mbtow_char( &c, buffer, len );
        if( offset == -1 )
            throw FatalError( ERR_T_CONV );
        if( offset > 1 )
            useDBCS = true;
        len = mbtow_cstring( text, buffer + offset, sizeof( text ) / sizeof( wchar_t ) - 1 );
        if( len == static_cast< std::size_t >( -1 ) )
            throw FatalError( ERR_T_CONV );
        text[len] = L'\0';
        entityMap.insert( std::map< std::wstring, wchar_t >::value_type( text, c ) );
    }
}
/*****************************************************************************/
void Nls::setLocalization( const char *loc)
{
    std::string path( Environment.value( "WIPFC" ) );
    if( path.length() )
#if !defined( __UNIX__ ) && !defined( __APPLE__ )
        path += '\\';
#else
        path += '/';
#endif
    path += getNlsFileName( loc );
    std::FILE *nls = std::fopen( path.c_str(), "r" );
    if( nls == NULL )
        throw FatalError( ERR_LANG );
    readNLS( nls );
    std::fclose( nls );
#if defined( __UNIX__ ) || defined( __APPLE__ )
    std::setlocale( LC_ALL, loc );  //this doesn't really do anything in OW either
#endif
    setCodePage( country.codePage );
}
/*****************************************************************************/
void Nls::readNLS( std::FILE *nls )
{
    char        sbuffer[256 * 2];
    wchar_t     value[256];
    char        *p;
    bool        doGrammar( false );
    while( std::fgets( sbuffer, sizeof( sbuffer ), nls ) ) {
        std::size_t len( std::strlen( sbuffer ) );
        killEOL( sbuffer + len - 1 );
        if( sbuffer[0] == '\0' )
            continue;               //skip blank lines
        if( sbuffer[0] == '#' )
            continue;               //skip comments
        if( (p = std::strchr( sbuffer, '=' )) != NULL ) {
            *p++ = '\0';
            mbtow_cstring( value, p, sizeof( value ) / sizeof( wchar_t ) - 1 );
            if( doGrammar ) {
                if( std::strcmp( sbuffer, "Words" ) == 0 ) {
                    processGrammar( value );
                } else if ( std::strcmp( sbuffer, "RemoveNL" ) == 0 ) {
                    //FIXME: exclude these values from s/dbcs table?
                }
            } else if( std::strcmp( sbuffer, "Note" ) == 0 ) {
                std::wstring text( value );
                killQuotes( text );
                noteText = text;
            } else if( std::strcmp( sbuffer, "Caution" ) == 0 ) {
                std::wstring text( value );
                killQuotes( text );
                cautionText = text;
            } else if( std::strcmp( sbuffer, "Warning" ) == 0 ) {
                std::wstring text( value );
                killQuotes( text );
                warningText = text;
            } else if( std::strcmp( sbuffer, "Reference" ) == 0 ) {
                std::wstring text( value );
                killQuotes( text );
                referenceText = text;
            } else if( std::strcmp( sbuffer, "olChars" ) == 0 ) {
                std::wstring text( value );
                olCh = text;
            } else if( std::strcmp( sbuffer, "olClose1" ) == 0 ) {
                std::wstring text( value );
                olClosers[0] = text;
            } else if( std::strcmp( sbuffer, "olClose2" ) == 0 ) {
                std::wstring text( value );
                olClosers[1] = text;
            } else if( std::strcmp( sbuffer, "ulItemId1" ) == 0 ) {
                std::wstring text( value );
                ulBul[0] = text;
            } else if( std::strcmp( sbuffer, "ulItemId2" ) == 0 ) {
                std::wstring text( value );
                ulBul[1] = text;
            } else if( std::strcmp( sbuffer, "ulItemId3" ) == 0 ) {
                std::wstring text( value );
                ulBul[2] = text;
            } else if( std::strcmp( sbuffer, "cgraphicFontFaceName" ) == 0 ) {
                std::wstring text( value );
                killQuotes( text );
                cgraphicFontFace = text;
            } else if( std::strcmp( sbuffer, "cgraphicFontWidth" ) == 0 ) {
                cgraphicFontW = static_cast< int >( std::wcstol( value, 0, 10 ) );
            } else if( std::strcmp( sbuffer, "cgraphicFontHeight" ) == 0 ) {
                cgraphicFontH = static_cast< int >( std::wcstol( value, 0, 10 ) );
            } else {
                // error: unknown keyword
            }
        } else if( std::strcmp( sbuffer, "Grammar" ) == 0 ) {
            doGrammar = true;
        } else if( std::strcmp( sbuffer, "eGrammar" ) == 0 ) {
            doGrammar = false;
        } else {
            // error: unknown keyword
        }
    }
}
/*****************************************************************************/
void Nls::processGrammar( wchar_t *buffer )
{
    if( grammarChars.empty() ) {
        grammarChars.reserve( 26 + 26 + 10 );
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
                grammarChars += c;
            dbcsT.ranges.push_back( static_cast< STD1::uint16_t >( chr1 ) );
            dbcsT.ranges.push_back( static_cast< STD1::uint16_t >( chr2 ) );
            if( chr1 > 255 || chr2 > 255 ) {
                useDBCS = true;
            }
        } else {
            // single character "chr"
            wchar_t chr( tok[0] );
            grammarChars += chr;
            dbcsT.ranges.push_back( static_cast< STD1::uint16_t >( chr ) );
            dbcsT.ranges.push_back( static_cast< STD1::uint16_t >( chr ) );
            if( chr > 255 ) {
                useDBCS = true;
            }
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
    EntityIter pos( entityMap.find( key ) );
    if( pos == entityMap.end() )
        throw Class2Error( ERR2_SYMBOL );
    return pos->second;
}
/*****************************************************************************/
STD1::uint32_t Nls::write( std::FILE *out )
{
    bytes = country.size;
    STD1::uint32_t start( country.write( out ) );
    if( useDBCS ) {
        dbcsT.write( out );
        bytes += dbcsT.size;
        dbcsG.write( out );
        bytes += dbcsG.size;
    } else {
        sbcsT.write( out );
        bytes += sbcsT.size;
        sbcsG.write( out );
        bytes += sbcsG.size;
    }
    return( start );
}
/*****************************************************************************/
STD1::uint32_t Nls::CountryDef::write( std::FILE *out ) const
{
    STD1::uint32_t start( std::ftell( out ) );
    if( std::fwrite( this, sizeof( CountryDef ), 1, out ) != 1 )
        throw FatalError( ERR_WRITE );
    return( start );
}
/*****************************************************************************/
void Nls::SbcsGrammarDef::setDefaultBits( NlsRecType rectype )
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
    std::memcpy( this->bits, &defbits[rectype - 1][0], 32 );
}
/*****************************************************************************/
STD1::uint32_t Nls::SbcsGrammarDef::write( std::FILE *out ) const
{
    STD1::uint32_t start( std::ftell( out ) );
    if( std::fwrite( this, sizeof( SbcsGrammarDef ), 1, out) != 1 )
        throw FatalError( ERR_WRITE );
    return( start );
}
/*****************************************************************************/
STD1::uint32_t Nls::DbcsGrammarDef::write( std::FILE *out )
{
    STD1::uint32_t start( std::ftell( out ) );
    size = 4 + static_cast< STD1::uint16_t >( ranges.size() * sizeof( STD1::uint16_t ) );
    if( std::fwrite( &size, sizeof( STD1::uint16_t ), 1, out) != 1 )
        throw FatalError( ERR_WRITE );
    if( std::fwrite( &type, sizeof( STD1::uint8_t ), 1, out) != 1 )
        throw FatalError( ERR_WRITE );
    if( std::fwrite( &format, sizeof( STD1::uint8_t ), 1, out) != 1 )
        throw FatalError( ERR_WRITE );
    for( std::vector< STD1::uint16_t >::const_iterator itr = ranges.begin(); itr != ranges.end(); ++itr ) {
        if( std::fwrite( &(*itr), sizeof( STD1::uint16_t), 1, out) != 1 ) {
            throw FatalError( ERR_WRITE );
        }
    }
    return start;
}
/*
The following table lists the 3-digit country code for the /COUNTRY or -d: nnn
parameter, the numeric identifiers of code pages, and the APS filename of the
IPFC command supported.

+-------------------------------------------------------------------------+
|Country               |Country Code|Code Pages        |APS File          |
|----------------------+------------+------------------+------------------|
|Arabic                |785         |0864              |APSY0864.APS      |
|----------------------+------------+------------------+------------------|
|Australia             |061         |0437, 0850        |APSYMBOL.APS      |
|----------------------+------------+------------------+------------------|
|Belgium               |032         |0437, 0850        |APSYMBOL.APS      |
|----------------------+------------+------------------+------------------|
|Brazil                |055         |0850, 0437        |APSYMBOL.APS      |
|----------------------+------------+------------------+------------------|
|Bulgaria              |359         |0915              |APSY0915.APS      |
|----------------------+------------+------------------+------------------|
|Canadian English      |001         |0437, 0850        |APSYMBOL.APS      |
|----------------------+------------+------------------+------------------|
|Canadian French       |002         |0863, 0850        |APSYMBOL.APS      |
|----------------------+------------+------------------+------------------|
|Catalan               |034         |0850, 0437        |APSYMBOL.APS      |
|----------------------+------------+------------------+------------------|
|Chinese (Simplified)  |086         |1381              |APSY1381.APS      |
|----------------------+------------+------------------+------------------|
|Chinese (Simplified)  |086         |1386              |APSY1386.APS      |
|----------------------+------------+------------------+------------------|
|Chinese (Traditional) |088         |0950              |APSY0950.APS      |
|----------------------+------------+------------------+------------------|
|Czech                 |421         |0852              |APSY0852.APS      |
|----------------------+------------+------------------+------------------|
|Denmark               |045         |0865, 0850        |APSYMBOL.APS      |
|----------------------+------------+------------------+------------------|
|Finland               |358         |0437, 0850        |APSYMBOL.APS      |
|----------------------+------------+------------------+------------------|
|France                |033         |0437, 0850        |APSYMBOL.APS      |
|----------------------+------------+------------------+------------------|
|Germany               |049         |0437, 0850        |APSYMBOL.APS      |
|----------------------+------------+------------------+------------------|
|Greece                |030         |0869              |APSY0869.APS      |
|----------------------+------------+------------------+------------------|
|Greece                |030         |0813              |APSY0813.APS      |
|----------------------+------------+------------------+------------------|
|Hebrew                |972         |0862              |APSY0862.APS      |
|----------------------+------------+------------------+------------------|
|Hungary               |036         |0852              |APSY0852.APS      |
|----------------------+------------+------------------+------------------|
|Italy                 |039         |0437, 0850        |APSYMBOL.APS      |
|----------------------+------------+------------------+------------------|
|Japan                 |081         |0932, 0437, 0850  |APSY0932.APS      |
|----------------------+------------+------------------+------------------|
|Korea                 |082         |0949, 0934        |APSY0949.APS      |
|----------------------+------------+------------------+------------------|
|Latin America         |003         |0437, 0850        |APSYMBOL.APS      |
|----------------------+------------+------------------+------------------|
|Lithuania             |370         |0921              |APSY0921.APS      |
|----------------------+------------+------------------+------------------|
|Netherlands           |031         |0437, 0850        |APSYMBOL.APS      |
|----------------------+------------+------------------+------------------|
|Norway                |047         |0865, 0850        |APSYMBOL.APS      |
|----------------------+------------+------------------+------------------|
|Poland                |048         |0852              |APSY0852.APS      |
|----------------------+------------+------------------+------------------|
|Portugal              |351         |0860, 0850        |APSYMBOL.APS      |
|----------------------+------------+------------------+------------------|
|Russia                |007         |0866              |APSY0866.APS      |
|----------------------+------------+------------------+------------------|
|Slovenia              |386         |0852              |APSY0852.APS      |
|----------------------+------------+------------------+------------------|
|Spain                 |034         |0437, 0850        |APSYMBOL.APS      |
|----------------------+------------+------------------+------------------|
|Sweden                |046         |0437, 0850        |APSYMBOL.APS      |
|----------------------+------------+------------------+------------------|
|Switzerland           |041         |0437, 0850        |APSYMBOL.APS      |
|----------------------+------------+------------------+------------------|
|Thailand              |066         |0874              |APSY0874.APS      |
|----------------------+------------+------------------+------------------|
|Turkey                |090         |0857              |APSY0857.APS      |
|----------------------+------------+------------------+------------------|
|United Kingdom        |044         |0437, 0850        |APSYMBOL.APS      |
|----------------------+------------+------------------+------------------|
|United States         |001         |0437, 0850        |APSYMBOL.APS      |
+-------------------------------------------------------------------------+
Note:  If there is an APSYxxxx.APS file that matches the code page you are
    using to compile your IPF file (either specified or default), the IPFC
    will use that file. Otherwise, it will use APSYMBOL.APS file that is
    suitable for code page 437 or 850.

The following table lists the 3-letter identifier for the /LANGUAGE and -l: xxx
parameter of the IPFC command:

+------------------------------------------------------------------------+
|ID        |Language                      |NLS File                      |
|----------+------------------------------+------------------------------|
|ARA       |Arabic                        |IPFARA.NLS                    |
|----------+------------------------------+------------------------------|
|BGR       |Bulgarian                     |IPFBGR.NLS                    |
|----------+------------------------------+------------------------------|
|CAT       |Catalan                       |IPFCAT.NLS                    |
|----------+------------------------------+------------------------------|
|CHT       |Chinese (Traditional)         |IPFCHT.NLS                    |
|----------+------------------------------+------------------------------|
|CZE       |Czech                         |IPFCZE.NLS                    |
|----------+------------------------------+------------------------------|
|DAN       |Danish                        |IPFDAN.NLS                    |
|----------+------------------------------+------------------------------|
|DEU       |German                        |IPFDEU.NLS                    |
|----------+------------------------------+------------------------------|
|ELL       |Greek 813                     |IPFELL.NLS                    |
|----------+------------------------------+------------------------------|
|ENG       |English UP                    |IPFENG.NLS                    |
|----------+------------------------------+------------------------------|
|ENU       |English US                    |IPFENU.NLS                    |
|----------+------------------------------+------------------------------|
|ESP       |Spanish                       |IPFESP.NLS                    |
|----------+------------------------------+------------------------------|
|FIN       |Finnish                       |IPFFIN.NLS                    |
|----------+------------------------------+------------------------------|
|FRA       |French                        |IPFFRA.NLS                    |
|----------+------------------------------+------------------------------|
|FRC       |Canadian French               |IPFFRC.NLS                    |
|----------+------------------------------+------------------------------|
|GRK       |Greek 869                     |IPFGRK.NLS                    |
|----------+------------------------------+------------------------------|
|HEB       |Hebrew                        |IPFHEB.NLS                    |
|----------+------------------------------+------------------------------|
|HUN       |Hungarian                     |IPFHUN.NLS                    |
|----------+------------------------------+------------------------------|
|ITA       |Italian                       |IPFITA.NLS                    |
|----------+------------------------------+------------------------------|
|JPN       |Japanese                      |IPFJPN.NLS                    |
|----------+------------------------------+------------------------------|
|KOR       |Korean                        |IPFKOR.NLS                    |
|----------+------------------------------+------------------------------|
|LTU       |Lithuanian                    |IPFLTU.NLS                    |
|----------+------------------------------+------------------------------|
|NLD       |Dutch                         |IPFNLD.NLS                    |
|----------+------------------------------+------------------------------|
|NOR       |Norwegian                     |IPFNOR.NLS                    |
|----------+------------------------------+------------------------------|
|POL       |Polish                        |IPFPOL.NLS                    |
|----------+------------------------------+------------------------------|
|PRC       |Chinese (Simplified) 1381     |IPFPRC.NLS                    |
|----------+------------------------------+------------------------------|
|PRC       |Chinese (Simplified) 1386     |IPFGBK.NLS                    |
|----------+------------------------------+------------------------------|
|PTB       |Brazilian/Portuguese          |IPFPTB.NLS                    |
|----------+------------------------------+------------------------------|
|PTG       |Portuguese                    |IPFPTG.NLS                    |
|----------+------------------------------+------------------------------|
|RUS       |Russian                       |IPFRUS.NLS                    |
|----------+------------------------------+------------------------------|
|SLO       |Slovene                       |IPFSLO.NLS                    |
|----------+------------------------------+------------------------------|
|SVE       |Swedish                       |IPFSVE.NLS                    |
|----------+------------------------------+------------------------------|
|THI       |Thai                          |IPFTHI.NLS                    |
|----------+------------------------------+------------------------------|
|TRK       |Turkish                       |IPFTRK.NLS                    |
|----------+------------------------------+------------------------------|
|UND       |User defined                  |IPFUND.NLS                    |
+------------------------------------------------------------------------+

*/
