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
* Description:  Process icmd tag
*
*   :icmd
*   Must follow :hn or :in tag
*   Contents must be unique
*
****************************************************************************/

#ifndef ICMD_INCLUDED
#define ICMD_INCLUDED

#include "config.hpp"
#include <cstdio>
#include <memory>
#include "element.hpp"
#include "index.hpp"

class GlobalDictionaryWord; //forward reference

class ICmd : public Element {
public:
    ICmd( Document* d, Element* p, const std::wstring* f, unsigned int r, unsigned int c );
    ~ICmd() { };
    Lexer::Token parse( Lexer* lexer );
    void buildIndex();
    void buildText( Cell* cell ) { cell = cell; };
    void setRes( STD1::uint16_t r ) { parentRes = r; };
    void setIdOrName( GlobalDictionaryWord* w ) { parentId = w; };
    size_t write( std::FILE* out ) { return  index->write( out ); };
    bool operator==( const ICmd& rhs ) const{ return *index == *rhs.index; };
    bool operator==( const std::wstring& rhs ) const { return *index == rhs; };
    bool operator<( const ICmd& rhs ) const { return *index < *rhs.index; };
private:
    ICmd( const ICmd& rhs );                //no copy
    ICmd& operator=( const ICmd& rhs );     //no assignment
    std::auto_ptr< IndexItem > index;
    GlobalDictionaryWord* parentId;
    STD1::uint16_t parentRes;
};
#endif //ICMD_INCLUDED
