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
* Description:  Process ol/eol tags
*
*   :ol / :eol
*       compact (else blank line between items)
*       verycompact (no extra lines at all; a wipfc extension)
*   If nested, indent 4 spaces
*
****************************************************************************/

#ifndef OL_INCLUDED
#define OL_INCLUDED

#include "tag.hpp"
#include "li.hpp"

class Ol : public Tag {
public:
    Ol( Document* d, Element *p, const std::wstring* f, unsigned int r,
        unsigned int c, unsigned char l, unsigned char i ) : Tag( d, p, f, r, c ),
        nestLevel( l ), indent( i ), compact( false ), veryCompact( false ) { };
    ~Ol() { };
    Lexer::Token parse( Lexer* lexer );
    void linearize( Page* page ) { linearizeChildren( page ); };
    void buildText( Cell* cell ) { cell = cell; };
protected:
    Lexer::Token parseAttributes( Lexer* lexer );
private:
    Ol( const Ol& rhs );            //no copy
    Ol& operator=( const Ol& rhs ); //no assignment
    unsigned char nestLevel;    //counts from 0
    unsigned char indent;       //in character spaces
    bool compact;
    bool veryCompact;
};

class EOl : public Tag {
public:
    EOl( Document* d, Element *p, const std::wstring* f, unsigned int r,
        unsigned int c ) : Tag( d, p, f, r, c ) { };
    ~EOl() { };
    void buildText( Cell* cell );
private:
    EOl( const EOl& rhs );              //no copy
    EOl& operator=( const EOl& rhs );   //no assignment
};

class OlLi : public Li {
public:
    OlLi( Document* d, Element *p, const std::wstring* f, unsigned int r,
        unsigned int c, unsigned int n, unsigned char l, unsigned char i, bool cmp ) :
        Li( d, p, f, r, c, n, l, i, cmp ) { };
    ~OlLi() { };
    Lexer::Token parse( Lexer* lexer );
    void linearize( Page* page ) { linearizeChildren( page ); };
    void buildText( Cell* cell ) { cell = cell; };
private:
    OlLi( const OlLi& rhs );            //no copy
    OlLi& operator=( const OlLi& rhs ); //no assignment
};

#endif //OL_INCLUDED
