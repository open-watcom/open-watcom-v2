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
#include "cntrydef.hpp"

class Nls {
    typedef STD1::uint8_t   byte;
    typedef STD1::uint16_t  word;
    typedef STD1::uint32_t  dword;

public:
    Nls( const char* loc );
    //set the locale
    void setLocalization( const char *loc );
    STD1::uint16_t codePage() const { return _country.getCodePage(); };
    //get localized text strings
    const std::wstring& note() const { return _noteText; };
    const std::wstring& warning() const { return _warningText; };
    const std::wstring& caution() const { return _cautionText; };
    const std::wstring& reference() const { return _referenceText; };
    const std::wstring& grammar() const { return _grammarChars; };
    //graphics font characteristics
    const FontEntry& cgraphicFont() const { return _cgraphicFont; };
    //localized bullets
    const std::wstring& olChars() const { return _olCh; };
    const std::wstring* olClose() const { return _olClosers; };
    const std::wstring* ulBullets() const { return _ulBul; };
    //resolve an entity reference
    wchar_t entity( const std::wstring& key );
    bool isEntity( const std::wstring& key ) { return _entityMap.find( key ) != _entityMap.end(); };
    //number of bytes written
    STD1::uint32_t length() { return _bytes; };
    STD1::uint32_t write( std::FILE* out );
private:
    typedef std::map< std::wstring, wchar_t >::iterator EntityIter;
    typedef std::map< std::wstring, wchar_t >::const_iterator ConstEntityIter;

    Nls( const Nls& rhs );              // no copy
    Nls& operator=( const Nls& rhs );   // no assignment

    void setCodePage( word cp );
    void readEntityFile( word cp );
    void readNLS( std::FILE* nls );
    void processGrammar( wchar_t* value );

    struct SbcsGrammarDef {             // Single-byte character set
        word                _size;          // 36
        WIPFC::NLSRecType   _type;          // NLSRecType.TEXT, NLSRecType.GRAPHIC
        byte                _format;        // 0
        byte                _bits[32];      // high-order bit first
        SbcsGrammarDef() : _size( sizeof( word ) + 2 * sizeof( byte ) + (sizeof( _bits ) / sizeof( _bits[0] )) * sizeof( byte ) ),
            _type( WIPFC::TEXT ), _format( 0 ) {};
        void setDefaultBits( WIPFC::NLSRecType rectype );
        STD1::uint32_t write( std::FILE* out ) const;
    };
    struct DbcsGrammarDef {             // Double-byte character set
        word                _size;          // 4 + (# ranges * 4)
        WIPFC::NLSRecType   _type;          // NLSRecType.TEXT, NLSRecType.GRAPHIC
        byte                _format;        // 1
        std::vector< word > _ranges;        // variable
        DbcsGrammarDef() : _size( sizeof( word ) + 2 * sizeof( byte ) ),
            _type( WIPFC::TEXT ), _format( 1 ) {};
        STD1::uint32_t write( std::FILE* out );
    };

    CountryDef _country;
    SbcsGrammarDef _sbcsT;
    SbcsGrammarDef _sbcsG;
    DbcsGrammarDef _dbcsT;
    DbcsGrammarDef _dbcsG;
    std::map< std::wstring, wchar_t > _entityMap; //stuff from entity file
    std::wstring _noteText;                  //stuff from nls file
    std::wstring _cautionText;
    std::wstring _warningText;
    std::wstring _referenceText;
    std::wstring _grammarChars;
    FontEntry _cgraphicFont;
    dword _bytes;
    std::wstring _olCh;
    std::wstring _olClosers[2];
    std::wstring _ulBul[3];
    bool _useDBCS;
};

#endif //NLS_INCLUDED
