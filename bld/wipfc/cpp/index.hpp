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
* Description:  An index entry
*
****************************************************************************/

#ifndef INDEX_INCLUDED
#define INDEX_INCLUDED

#include "config.hpp"
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>

class IndexItem {
public:
    enum Type {
        CMD,
        PRIMARY,
        SECONDARY
    };
    IndexItem( Type t );
    ~IndexItem() { };
    void setGlobal() { hdr.global = 1; };
    bool isGlobal() const { return hdr.global; };
    void setSortKey( std::wstring& key ) { hdr.sortKey = 1; sortKey = key; };
    void setText( std::wstring& t ) { text = t; };
    void setTOC( STD1::uint16_t t ) { hdr.tocPanelIndex = t; };
    void addSynonym( STD1::uint32_t t ) { synonyms.push_back( t ); };
    size_t write( std::FILE* out );
    bool operator==( const IndexItem& rhs ) const;
    bool operator==( const std::wstring& rhs ) const;
    bool operator<( const IndexItem& rhs ) const;
private:
    IndexItem( const IndexItem& rhs );              //no copy
    IndexItem& operator=( const IndexItem& rhs );   //no assignment
#pragma pack(push, 1)
    struct IndexHeader {
        STD1::uint8_t   size;               // size of item text
        STD1::uint8_t   primary  :1;        // bit 0 set: i1
        STD1::uint8_t   secondary:1;        // bit 1 set: i2 (both clear if icmd)
        STD1::uint8_t   unknown  :4;
        STD1::uint8_t   global   :1;        // bit 6 set: global entry
        STD1::uint8_t   sortKey  :1;        // bit 7 set: sort key
        STD1::uint8_t   synonymCount;       // number synonym entries following
        STD1::uint16_t  tocPanelIndex;      // toc entry number of panel
        IndexHeader() { std::memset( this, 0, sizeof( IndexItem ) ); };
    };
#pragma pack(pop)
    IndexHeader hdr;
    std::wstring sortKey;
    std::wstring text;
    std::vector< STD1::uint32_t > synonyms;
    typedef std::vector< STD1::uint32_t >::iterator SynIter;
    typedef std::vector< STD1::uint32_t >::const_iterator ConstSynIter;
    int wstricmp( const wchar_t *s, const wchar_t *t ) const;
};

#endif //INDEX_INCLUDED
