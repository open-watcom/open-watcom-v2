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

#include <cstdio>
#include <map>
#include <string>
#include <vector>
#include "fnt.hpp"
#include "nlsrecty.hpp"

class Nls {
    typedef STD1::uint8_t   byte;
    typedef STD1::uint16_t  word;
    typedef STD1::uint32_t  dword;

public:
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
    const FontEntry& cgraphicFont() const { return _cgraphicFont; };
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
    Nls( const Nls& rhs );              // no copy
    Nls& operator=( const Nls& rhs );   // no assignment
    struct CountryDef {
        word                size;       // 12
        WIPFC::NLSRecType   type;       // NLSRecType.CONTROL
        byte                format;     // 0
        word                value;      // 256
        word                country;
        word                codePage;
        word                reserved;   // 0
        CountryDef() : size( sizeof( word ) + 2 * sizeof( byte ) + 4 * sizeof( word ) ),
            type( WIPFC::CONTROL ), format( 0 ), value( 256 ), country( 1 ), codePage( 850 ), reserved( 0 ) {};
        STD1::uint32_t write( std::FILE* out ) const;
    };

    struct SbcsGrammarDef {         // Single-byte character set
        word                size;       // 36
        WIPFC::NLSRecType   type;       // NLSRecType.TEXT, NLSRecType.GRAPHIC
        byte                format;     // 0
        byte                bits[32];   // high-order bit first
        SbcsGrammarDef() : size( sizeof( word ) + 2 * sizeof( byte ) + (sizeof( bits ) / sizeof( bits[0] )) * sizeof( byte ) ),
            type( WIPFC::TEXT ), format( 0 ) {};
        void setDefaultBits( WIPFC::NLSRecType rectype );
        STD1::uint32_t write( std::FILE* out ) const;
    };
    struct DbcsGrammarDef {         // Double-byte character set
        word                size;       // 4 + (# ranges * 4)
        WIPFC::NLSRecType   type;       // NLSRecType.TEXT, NLSRecType.GRAPHIC
        byte                format;     // 1
        std::vector< word > ranges;
        DbcsGrammarDef() : size( sizeof( word ) + 2 * sizeof( byte ) ),
            type( WIPFC::TEXT ), format( 1 ) {};
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
    FontEntry _cgraphicFont;
    dword bytes;
    std::wstring olCh;
    std::wstring olClosers[2];
    std::wstring ulBul[3];
    bool useDBCS;
    void setCodePage( word cp );
    void readEntityFile( std::FILE* aps );
    void readNLS( std::FILE* nls );
    void processGrammar( wchar_t* value );
    std::string readNlsConfFile( std::FILE *nlsconf, const char *loc );
    std::string getNlsFileName( const char *loc );
};

#endif //NLS_INCLUDED

