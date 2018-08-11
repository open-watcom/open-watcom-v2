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
* Description:  An index entry
*
****************************************************************************/

#ifndef INDEX_INCLUDED
#define INDEX_INCLUDED

#include <cstdio>
#include <cstring>
#include <string>
#include <vector>


class OutFile;      // forward reference

class IndexItem {
    typedef STD1::uint8_t   byte;
    typedef STD1::uint16_t  word;
    typedef STD1::uint32_t  dword;

public:
    enum Type {
        CMD,
        PRIMARY,
        SECONDARY
    };
    IndexItem( Type t );
    ~IndexItem() { };
    void setGlobal() { _hdr.global = 1; };
    bool isGlobal() const { return _hdr.global; };
    void setSortKey( std::wstring& key ) { _hdr.sortKey = 1; _sortKey = key; };
    void setText( std::wstring& t ) { _text = t; };
    void setTOC( word t ) { _hdr.tocPanelIndex = t; };
    void addSynonym( dword t ) { _synonyms.push_back( t ); };
    dword write( OutFile* out );
    bool operator==( const IndexItem& rhs ) const;
    bool operator==( const std::wstring& rhs ) const;
    bool operator<( const IndexItem& rhs ) const;
private:
    IndexItem( const IndexItem& rhs );              //no copy
    IndexItem& operator=( const IndexItem& rhs );   //no assignment
    int wstricmp( const wchar_t *s, const wchar_t *t ) const;

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
    IndexHeader             _hdr;
    std::wstring            _sortKey;
    std::wstring            _text;
    std::vector< dword >    _synonyms;
    typedef std::vector< dword >::iterator SynIter;
    typedef std::vector< dword >::const_iterator ConstSynIter;
};

#endif //INDEX_INCLUDED
