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
* Description:  A single word in the Dictionary (vocabulary) data
*
****************************************************************************/

#ifndef GDWORD_INCLUDED
#define GDWORD_INCLUDED

#include "config.hpp"
#include <cstdio>
#include <string>
#include <vector>
#include "fts.hpp"

class GlobalDictionaryWord {
public:
    GlobalDictionaryWord() : idx( 0 ) { };
    GlobalDictionaryWord( const std::wstring& t ) : idx( 0 ), text( t ) { };
    //convert to upper case
    void toUpper();
    //set the index postion in the collection
    void setIndex( STD1::uint16_t i ) { idx = i; };
    STD1::uint16_t index() const { return idx; };
    const std::wstring& getText() const { return text; };
    size_t writeWord( std::FILE* out ) const;
    bool operator==( const GlobalDictionaryWord& rhs ) const { return text == rhs.text; };
    bool operator==( const std::wstring& rhs ) const { return text == rhs; };
    bool operator<( const GlobalDictionaryWord& rhs ) const;
    //Functions forwarded to FTSElement
    void setPages( size_t count ) { fts.setPages( count ); };
    void onPage( size_t i ) { fts.onPage( i ); };
    void buildFTS() { fts.build(); };
    bool bigFTS() { return fts.bigFTS(); };
    size_t writeFTS( std::FILE* out, bool big ) { return fts.write( out, big ); };
private:
    GlobalDictionaryWord( const GlobalDictionaryWord& rhs );            //no copy
    GlobalDictionaryWord& operator=( const GlobalDictionaryWord& rhs ); //no assignment
    STD1::uint16_t idx;             //index in Dictionary
    std::wstring text;              //the actual text
    FTSElement fts;
    int wstricmp( const wchar_t *s, const wchar_t *t ) const;
};

#endif //GDWORD_INCLUDED
