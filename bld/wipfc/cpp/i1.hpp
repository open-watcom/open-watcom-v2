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
* Description:  Process i1 tags
*
*   :i1
*       id=[a-zA-z][a-zA-z0-9]*
*       global
*       roots='' (space separated list of words defined by :isyn tags)
*       sortkey='key-text' (place index-text where key-text would sort in index)
*   a single line of index-text follows
*   Cannot be child of :fn
*
****************************************************************************/

#ifndef I1_INCLUDED
#define I1_INCLUDED

#include <cstdio>
#include <memory>
#include "element.hpp"
#include "index.hpp"

class GlobalDictionaryWord; //forward reference

class I1 : public Element {
public:
    I1( Document* d, Element* p, const std::wstring* f, unsigned int r, unsigned int c );
    ~I1() { };
    Lexer::Token parse( Lexer* lexer );
    void buildIndex();
    void buildText( Cell* cell ) { (void)cell; };
    void setRes( word r ) { _parentRes = r; };
    void setIdOrName( GlobalDictionaryWord* w ) { _parentId = w; };
    void addSecondary( IndexItem* i ) { _secondary.push_back( i ); };
    word secondaryCount() const
        { return static_cast< word >( _secondary.size() ); };
    bool isGlobal() const { return _primary->isGlobal(); };
    dword write( OutFile* out );
    bool operator==( const I1& rhs ) const{ return *_primary == *rhs._primary; };
    bool operator==( const std::wstring& rhs ) const { return *_primary == rhs; };
    bool operator<( const I1& rhs ) const { return *_primary < *rhs._primary; };
private:
    I1( const I1& rhs );                //no copy
    I1& operator=( const I1& rhs );     //no assignment
    Lexer::Token parseAttributes( Lexer* lexer );

    std::auto_ptr< IndexItem >  _primary;
    std::vector< IndexItem* >   _secondary;
    typedef std::vector< IndexItem* >::iterator IndexIter;
    typedef std::vector< IndexItem* >::const_iterator ConstIndexIter;
    std::vector< std::wstring > _synRoots;
    typedef std::vector< std::wstring >::iterator SynIter;
    typedef std::vector< std::wstring >::const_iterator ConstSynIter;
    std::wstring                _id;
    GlobalDictionaryWord*       _parentId;
    word                        _parentRes;
};

#endif //I1_INCLUDED
