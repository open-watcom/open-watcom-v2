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
    int codePage() const { return country.codePage; };
    //get localized text strings
    const std::wstring& note() const { return noteText; };
    const std::wstring& warning() const { return warningText; };
    const std::wstring& caution() const { return cautionText; };
    const std::wstring& reference() const { return referenceText; };
    const std::wstring& grammer() const { return grammerChars; };
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
#pragma pack(push, 1)
    struct CountryDef {
        STD1::uint16_t size;        //12
        STD1::uint8_t  type;        //NLSRecType.CONTROL
        STD1::uint8_t  format;      //0
        STD1::uint16_t value;       //256
        STD1::uint16_t country;
        STD1::uint16_t codePage;
        STD1::uint16_t reserved;    //0
        CountryDef() :
            size( sizeof(CountryDef) ), type( Nls::CONTROL ), format( 0 ), value( 256 ),
            country( 1 ), codePage( 850 ), reserved( 0 ) {};
        STD1::uint32_t write( std::FILE* out ) const;
        };
    
    struct SbcsGrammerDef {         //Single-byte character set
        STD1::uint16_t size;        //36
        STD1::uint8_t  type;        //NLSRecType.WORD, NLSRecType.GRAPHIC
        STD1::uint8_t  format;      //0
        STD1::uint8_t  bits[ 32 ];  //high-order bit first
        SbcsGrammerDef() : size( sizeof( SbcsGrammerDef ) ), type( Nls::TEXT ), format( 0 ) { };
        void setDefaultBits( NlsRecType type );
        STD1::uint32_t write( std::FILE* out ) const;
        };
#pragma pack(pop)
    struct DbcsGrammerDef {         //Double-byte character set
        STD1::uint16_t size;        //4 + (# ranges * 4)
        STD1::uint8_t  type;        //NLSRecType.TEXT, NLSRecType.GRAPHIC
        STD1::uint8_t  format;      //1
        std::vector<STD1::uint16_t> ranges;
        DbcsGrammerDef() : size( 4 ), type( Nls::TEXT ) , format( 1 ) {};
        STD1::uint32_t write( std::FILE* out );
        };

    CountryDef country;
    SbcsGrammerDef sbcsT;
    SbcsGrammerDef sbcsG;
    DbcsGrammerDef dbcsT;
    DbcsGrammerDef dbcsG;
    std::map< std::wstring, wchar_t > entityMap; //stuff from entity file
    typedef std::map< std::wstring, wchar_t >::iterator EntityIter;
    typedef std::map< std::wstring, wchar_t >::const_iterator ConstEntityIter;
    std::wstring noteText;                  //stuff from nls file
    std::wstring cautionText;
    std::wstring warningText;
    std::wstring referenceText;
    std::wstring grammerChars;
    std::wstring cgraphicFontFace;
    STD1::uint32_t bytes;
    int cgraphicFontW;
    int cgraphicFontH;
    std::wstring olCh;
    std::wstring olClosers[ 2 ];
    std::wstring ulBul[ 3 ];
    bool useDBCS;
    void setCodePage( int cp );
    void readEntityFile( std::FILE* aps );
    void readNLS( std::FILE* nls );
    void processGrammer( wchar_t* value );
};

#endif //NLS_INCLUDED

