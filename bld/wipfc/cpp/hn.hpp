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

#include "config.hpp"
#include <vector>
#include "tag.hpp"
#include "toc.hpp"

class GlobalDictionaryWord; //forward references

class Hn : public Tag {
public:
    Hn( Document* d, Element *p, const std::wstring* f, unsigned int r, unsigned int c, unsigned int l ) :
        Tag( d, p, f, r, c ), id( 0 ), name( 0 ), res( 0 ), global( false )
        { toc.nestLevel = static_cast< STD1::uint8_t >( l ); };
    ~Hn();
    Lexer::Token parse( Lexer* lexer );
    void buildTOC( Page* page );
    void buildText( Cell* cell );
    //this header is at level l
    void setLevel( unsigned int l ) { toc.nestLevel = static_cast< STD1::uint8_t >( l ); };
    unsigned int level() const { return toc.nestLevel; };
    //get the resource id
    unsigned int resourceNumber() const { return res; };
    //there are other headers under this one
    void setHasChildren() { toc.hasChildren = 1; };
    //has child windows that it controls
    void setIsParent() { toc.extended = 1; etoc.isParent = 1; };
    void addChild( STD1::uint16_t toc ) { childTOCs.push_back( toc ); };
    //this is a header on a split window
    bool isSplit() const { return ( style.word & PageStyle::SPLIT ) == PageStyle::SPLIT; };
    void linearize( Page* page );
protected:
    Lexer::Token parseAttributes( Lexer* lexer );
private:
    Hn( const Hn& rhs );                //no copy
    Hn& operator=( const Hn& rhs );     //no assignment
    TocEntry toc;
    ExtTocEntry etoc;
    PageOrigin origin;
    PageSize size;
    PageStyle style;
    PageGroup group;
    PageControls controls;
    std::string title;
    std::wstring tutorial;
    std::vector< STD1::uint16_t > childTOCs;
    GlobalDictionaryWord* id;
    GlobalDictionaryWord* name;
    STD1::uint16_t res;
    bool global;
};

#endif //HN_INCLUDED
