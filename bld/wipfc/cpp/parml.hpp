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
* Description:  Process parml/eparml tags
*
*   :parml / :eparml
*       tsize=[0-9]+  (default: 10; width of terms and term headers)
*       break=none (term and description on same line)
*             fit  (description on line below if term > tsize)
*             all  (description on line below, default)
*       compact (no blank line between each item)
*   Two columns (term, description)
*   Must contain :pt, :pd (matched)
*   Can be nested in a :pd tag
*
****************************************************************************/

#ifndef PARML_INCLUDED
#define PARML_INCLUDED

#include "tag.hpp"

class Parml : public Tag {
public:
    enum Break {
        NONE,
        FIT,
        ALL
    };
    Parml( Document* d, Element *p, const std::wstring* f, unsigned int r,
        unsigned int c, byte n, byte i ) : Tag( d, p, f, r, c ),
        _nestLevel( n ), _indent( i ), _tabSize( 10 ), _breakage( ALL ), _compact( false ) { };
    ~Parml() { };
    Lexer::Token parse( Lexer* lexer );
    void linearize( Page* page ) { linearizeChildren( page ); };
    void buildText( Cell* cell ) { (void)cell; };
protected:
    Lexer::Token parseAttributes( Lexer* lexer );
private:
    Parml( const Parml& rhs );              //no copy
    Parml& operator=( const Parml& rhs );   //no assignment

    byte                _nestLevel;     //counts from 0
    byte                _indent;        //in character spaces
    byte                _tabSize;       //in character spaces
    Break               _breakage;
    bool                _compact;
};

class EParml : public Tag {
public:
    EParml( Document* d, Element *p, const std::wstring* f, unsigned int r,
        unsigned int c ) : Tag( d, p, f, r, c ) { };
    ~EParml() { };
    void buildText( Cell* cell );
private:
    EParml( const EParml& rhs );            //no copy
    EParml& operator=( const EParml& rhs ); //no assignment
};

class Pt : public Tag {
public:
    Pt( Document* d, Element *p, const std::wstring* f, unsigned int r,
        unsigned int c, byte i, byte t, Parml::Break brk, bool cp ) :
        Tag( d, p, f, r, c ), _indent( i ), _tabSize( t ), _textLength( 0 ),
        _breakage( brk ), _compact( cp ) { };
    ~Pt() { };
    Lexer::Token parse( Lexer* lexer );
    void linearize( Page* page ) { linearizeChildren( page ); };
    void buildText( Cell* cell ) { (void)cell; };
private:
    Pt( const Pt& rhs );                //no copy
    Pt& operator=( const Pt& rhs );     //no assignment

    byte                _indent;
    byte                _tabSize;
    byte                _textLength;
    Parml::Break        _breakage;
    bool                _compact;
};

class Pd : public Tag {
public:
    Pd( Document* d, Element *p, const std::wstring* f, unsigned int r,
        unsigned int c, byte i, byte t, bool brk ) :
        Tag( d, p, f, r, c ), _indent( i ), _tabSize( t ), _doBreak( brk ) { };
    ~Pd() { };
    Lexer::Token parse( Lexer* lexer );
    void linearize( Page* page ) { linearizeChildren( page ); };
    void buildText( Cell* cell ) { (void)cell; };
private:
    Pd( const Pd& rhs );                //no copy
    Pd& operator=( const Pd& rhs );     //no assignment

    byte                _indent;
    byte                _tabSize;
    bool                _doBreak;
};

#endif //PARML_INCLUDED
