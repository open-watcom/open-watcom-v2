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
* Description:  A heading tag
*   :h1-:h6
*       res=[0-9]+
*       id=[a-zA-z][a-zA-z0-9]*
*       name=[a-zA-z][a-zA-z0-9]*
*       global
*       tutorial=''
*       x=([0-9]+[c|x|p|%]) | (left|center|right)
*       y=([0-9]+[c|x|p|%]) | (top|center|bottom)
*       width=([0-9]+[c|x|p|%])
*       height=([0-9]+[c|x|p|%])
*       group=[0-9]+
*       viewport (force new window)
*       clear (close open window, force new)
*       titlebar=yes|sysmenu|minmax|both|none (default: both)
*       scroll=horizontal|vertical|both|none (default: both)
*       rules=border|sizeborder|none (default: sizeborder)
*       nosearch (do not return this heading, return parent)
*       noprint
*       hide (do not include in TOC)
*       toc=[1-6] (valid until eof or next occurance)
*       ctrlarea=page|none
*       ctrlrefid=
*   Cannot skip levels
*
****************************************************************************/

#ifndef HN_INCLUDED
#define HN_INCLUDED

#include <vector>
#include "tag.hpp"
#include "toc.hpp"

class GlobalDictionaryWord; //forward references

class Hn : public Tag {
public:
    Hn( Document* d, Element *p, const std::wstring* f, unsigned int r, unsigned int c, unsigned int l ) :
        Tag( d, p, f, r, c ), _id( 0 ), _name( 0 ), _res( 0 ), _global( false )
        { _toc.flags.s.nestLevel = static_cast< byte >( l ); };
    ~Hn();
    Lexer::Token parse( Lexer* lexer );
    void buildTOC( Page* page );
    void buildText( Cell* cell );
    //this header is at level l
    void setLevel( unsigned int l ) { _toc.flags.s.nestLevel = static_cast< byte >( l ); };
    unsigned int level() const { return _toc.flags.s.nestLevel; };
    //get the resource id
    unsigned int resourceNumber() const { return _res; };
    //there are other headers under this one
    void setHasChildren() { _toc.flags.s.hasChildren = 1; };
    //has child windows that it controls
    void setIsParent() { _toc.flags.s.extended = 1; _etoc.flags.s.isParent = 1; };
    void addChild( word toc1 ) { _childTOCs.push_back( toc1 ); };
    //this is a header on a split window
    bool isSplit() const { return (_style.attrs & PageStyle::SPLIT) == PageStyle::SPLIT; };
    void linearize( Page* page );
protected:
    Lexer::Token parseAttributes( Lexer* lexer );
private:
    Hn( const Hn& rhs );                //no copy
    Hn& operator=( const Hn& rhs );     //no assignment

    TocEntry                _toc;
    ExtTocEntry             _etoc;
    PageOrigin              _origin;
    PageSize                _size;
    PageStyle               _style;
    PageGroup               _group;
    PageControl             _control;
    std::wstring            _title;
    std::wstring            _tutorial;
    std::vector< word >     _childTOCs;
    GlobalDictionaryWord*   _id;
    GlobalDictionaryWord*   _name;
    word                    _res;
    bool                    _global;
};

#endif //HN_INCLUDED
