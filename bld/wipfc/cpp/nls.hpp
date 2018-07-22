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

#ifndef NLS_INCLUDED
#define NLS_INCLUDED

#include "config.hpp"
#include <cstdio>
#include <map>
#include <string>
#include <vector>

class Nls {
public:
    enum NlsRecType {
        CONTROL,
        TEXT,
        GRAPHIC
    };
    Nls( const char* loc );
    //set the locale
    void setLocalization( const char *loc );
    STD1::uint16_t codePage() const { return country.codePage; };
    //get localized text strings
    const std::wstring& note() const { return noteText; };
    const std::wstring& warning() const { return warningText; };
    const std::wstring& caution() const { return cautionText; };
    const std::wstring& reference() const { return referenceText; };
    const std::wstring& grammar() const { return grammarChars; };
    //graphics font characteristics
    const std::wstring& cgraphicFontFaceName() const { return cgraphicFontFace; };
    int cgraphicFontWidth() const { return cgraphicFontW; };
    int cgraphicFontHeight() const { return cgraphicFontH; };
    //localized bullets
    const std::wstring& olChars() const { return olCh; };
    const std::wstring* olClose() const { return olClosers; };
    const std::wstring* ulBullets() const { return ulBul; };
    //resolve an entity reference
    wchar_t entity( const std::wstring& key );
    bool isEntity( const std::wstring& key ) { return entityMap.find( key ) != entityMap.end(); };
    //number of bytes written
    STD1::uint32_t length() { return bytes; };
    STD1::uint32_t write( std::FILE* out );
private:
    Nls( const Nls& rhs );              //no copy
    Nls& operator=( const Nls& rhs );   //no assignment
    struct CountryDef {
        STD1::uint16_t size;        //12
        STD1::uint8_t  type;        //NLSRecType.CONTROL
        STD1::uint8_t  format;      //0
        STD1::uint16_t value;       //256
        STD1::uint16_t country;
        STD1::uint16_t codePage;
        STD1::uint16_t reserved;    //0
        CountryDef() : size( sizeof( STD1::uint16_t ) + 2 * sizeof( STD1::uint8_t ) + 4 * sizeof( STD1::uint16_t ) ),
            type( Nls::CONTROL ), format( 0 ), value( 256 ), country( 1 ), codePage( 850 ), reserved( 0 ) {};
        STD1::uint32_t write( std::FILE* out ) const;
    };

    struct SbcsGrammarDef {         //Single-byte character set
        STD1::uint16_t size;        //36
        STD1::uint8_t  type;        //NLSRecType.TEXT, NLSRecType.GRAPHIC
        STD1::uint8_t  format;      //0
        STD1::uint8_t  bits[32];    //high-order bit first
        SbcsGrammarDef() : size( sizeof( STD1::uint16_t ) + (2 + sizeof( bits ) / sizeof( bits[0] )) * sizeof( STD1::uint8_t ) ),
            type( Nls::TEXT ), format( 0 ) {};
        void setDefaultBits( NlsRecType rectype );
        STD1::uint32_t write( std::FILE* out ) const;
    };
    struct DbcsGrammarDef {         //Double-byte character set
        STD1::uint16_t size;        //4 + (# ranges * 4)
        STD1::uint8_t  type;        //NLSRecType.TEXT, NLSRecType.GRAPHIC
        STD1::uint8_t  format;      //1
        std::vector<STD1::uint16_t> ranges;
        DbcsGrammarDef() : size( 4 ), type( Nls::TEXT ), format( 1 ) {};
        STD1::uint32_t write( std::FILE* out );
    };

    CountryDef country;
    SbcsGrammarDef sbcsT;
    SbcsGrammarDef sbcsG;
    DbcsGrammarDef dbcsT;
    DbcsGrammarDef dbcsG;
    std::map< std::wstring, wchar_t > entityMap; //stuff from entity file
    typedef std::map< std::wstring, wchar_t >::iterator EntityIter;
    typedef std::map< std::wstring, wchar_t >::const_iterator ConstEntityIter;
    std::wstring noteText;                  //stuff from nls file
    std::wstring cautionText;
    std::wstring warningText;
    std::wstring referenceText;
    std::wstring grammarChars;
    std::wstring cgraphicFontFace;
    STD1::uint32_t bytes;
    int cgraphicFontW;
    int cgraphicFontH;
    std::wstring olCh;
    std::wstring olClosers[ 2 ];
    std::wstring ulBul[ 3 ];
    bool useDBCS;
    void setCodePage( STD1::uint16_t cp );
    void readEntityFile( std::FILE* aps );
    void readNLS( std::FILE* nls );
    void processGrammar( wchar_t* value );
    std::string readNlsConfFile( std::FILE *nlsconf, const char *loc );
    std::string getNlsFileName( const char *loc );
};

#endif //NLS_INCLUDED

