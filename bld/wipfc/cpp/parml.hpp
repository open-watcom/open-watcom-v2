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
        unsigned int c, unsigned char n, unsigned char i ) : Tag( d, p, f, r, c ),
        nestLevel( n ), indent( i ), tabSize( 10 ), breakage( ALL ), compact( false ) { };
    ~Parml() { };
    Lexer::Token parse( Lexer* lexer );
    void linearize( Page* page ) { linearizeChildren( page ); };
    void buildText( Cell* cell ) { cell = cell; };
protected:
    Lexer::Token parseAttributes( Lexer* lexer );
private:
    Parml( const Parml& rhs );              //no copy
    Parml& operator=( const Parml& rhs );   //no assignment
    unsigned char nestLevel;    //counts from 0
    unsigned char indent;       //in character spaces
    unsigned char tabSize;      //in character spaces
    Break breakage;
    bool compact;
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
        unsigned int c, unsigned char i, unsigned char t, Parml::Break brk, bool cp ) :
        Tag( d, p, f, r, c ), indent( i ), tabSize( t ), textLength( 0 ),
        breakage( brk ), compact( cp ) { };
    ~Pt() { };
    Lexer::Token parse( Lexer* lexer );
    void linearize( Page* page ) { linearizeChildren( page ); };
    void buildText( Cell* cell ) { cell = cell; };
private:
    Pt( const Pt& rhs );                //no copy
    Pt& operator=( const Pt& rhs );     //no assignment
    unsigned char indent;
    unsigned char tabSize;
    unsigned char textLength;
    Parml::Break breakage;
    bool compact;
};

class Pd : public Tag {
public:
    Pd( Document* d, Element *p, const std::wstring* f, unsigned int r,
        unsigned int c, unsigned char i, unsigned char t, bool brk ) :
        Tag( d, p, f, r, c ), indent( i ), tabSize( t ), doBreak( brk ) { };
    ~Pd() { };
    Lexer::Token parse( Lexer* lexer );
    void linearize( Page* page ) { linearizeChildren( page ); };
    void buildText( Cell* cell ) { cell = cell; };
private:
    Pd( const Pd& rhs );                //no copy
    Pd& operator=( const Pd& rhs );     //no assignment
    unsigned char indent;
    unsigned char tabSize;
    bool doBreak;
};

#endif //PARML_INCLUDED
