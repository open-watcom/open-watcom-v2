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
* Description:  Process icmd tag
*
*   :icmd
*   Must follow :hn or :in tag
*   Contents must be unique
*
****************************************************************************/

#ifndef ICMD_INCLUDED
#define ICMD_INCLUDED

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
    void buildText( Cell* cell ) { (void)cell; };
    void setRes( word r ) { _parentRes = r; };
    void setIdOrName( GlobalDictionaryWord* w ) { _parentId = w; };
    dword write( OutFile* out ) { return  _index->write( out ); };
    bool operator==( const ICmd& rhs ) const{ return *_index == *rhs._index; };
    bool operator==( const std::wstring& rhs ) const { return *_index == rhs; };
    bool operator<( const ICmd& rhs ) const { return *_index < *rhs._index; };
private:
    ICmd( const ICmd& rhs );                //no copy
    ICmd& operator=( const ICmd& rhs );     //no assignment

    std::auto_ptr< IndexItem >  _index;
    GlobalDictionaryWord*       _parentId;
    word                        _parentRes;
};
#endif //ICMD_INCLUDED
