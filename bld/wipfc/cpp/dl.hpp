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
* Description:  Process dl/edl tags
*
*   :dl / :edl
*       compact (no blank line between each item)
*       tsize=[0-9]+  (default: 10; width of terms and term headers)
*       break=none (term and description on same line)
*             fit  (description on line below if term > tsize)
*             all  (description on line below)
*   May contain :dthd, :ddhd
*   Must contain :dt, :dd which must follow :dthd, :ddhd
*   Multiple :dt's are allowed, followed by a single :dd
****************************************************************************/

#ifndef DL_INCLUDED
#define DL_INCLUDED

#include "tag.hpp"

class Dl : public Tag {
public:
    enum Break {
        NONE,
        FIT,
        ALL
    };
    Dl( Document* d, Element *p, const std::wstring* f, unsigned int r,
        unsigned int c, unsigned char n, unsigned char i ) : Tag( d, p, f, r, c ),
        nestLevel( n ), indent( i ), tabSize( 10 ), breakage( NONE ), compact( false ) { };
    ~Dl() { };
    Lexer::Token parse( Lexer* lexer );
    void linearize( Page* page ) { linearizeChildren( page ); };
    void buildText( Cell* cell ) { cell = cell; };
protected:
    Lexer::Token parseAttributes( Lexer* lexer );
private:
    Dl( const Dl& rhs );            //no copy
    Dl& operator=( const Dl& rhs ); //no assignment
    unsigned char nestLevel;        //counts from 0
    unsigned char indent;           //in character spaces
    unsigned char tabSize;          //in character spaces
    Break breakage;
    bool compact;
};

class EDl : public Tag {
public:
    EDl( Document* d, Element *p, const std::wstring* f, unsigned int r,
        unsigned int c ) : Tag( d, p, f, r, c ) { };
    ~EDl() { };
    void buildText( Cell* cell );
private:
    EDl( const EDl& rhs );              //no copy
    EDl& operator=( const EDl& rhs );   //no assignment
};

class DtHd : public Tag {
public:
    DtHd( Document* d, Element *p, const std::wstring* f, unsigned int r,
        unsigned int c, unsigned char i ) : Tag( d, p, f, r, c ), indent( i ),
        textLength( 0 ) { };
    ~DtHd() { };
    unsigned char length() const { return textLength; };
    Lexer::Token parse( Lexer* lexer );
    void linearize( Page* page ) { linearizeChildren( page ); };
    void buildText( Cell* cell ) { cell = cell; };
private:
    DtHd( const DtHd& rhs );            //no copy
    DtHd& operator=( const DtHd& rhs ); //no assignment
    unsigned char indent;
    unsigned char textLength;
};

class DdHd : public Tag {
public:
    DdHd( Document* d, Element *p, const std::wstring* f, unsigned int r,
        unsigned int c, unsigned char i, unsigned char t ) : Tag( d, p, f, r, c ),
        indent( i ), tabSize( t ) { };
    ~DdHd() { };
    Lexer::Token parse( Lexer* lexer );
    void linearize( Page* page ) { linearizeChildren( page ); };
    void buildText( Cell* cell ) { cell = cell; };
private:
    DdHd( const DdHd& rhs );            //no copy
    DdHd& operator=( const DdHd& rhs ); //no assignment
    unsigned char indent;
    unsigned char tabSize;
};

class Dt : public Tag {
public:
    Dt( Document* d, Element *p, const std::wstring* f, unsigned int r,
        unsigned int c, unsigned char i, unsigned char t, Dl::Break brk, bool cp ) :
        Tag( d, p, f, r, c ), indent( i ), tabSize( t ), textLength( 0 ),
        breakage( brk ), compact( cp ) { };
    ~Dt() { };
    Lexer::Token parse( Lexer* lexer );
    void linearize( Page* page ) { linearizeChildren( page ); };
    void buildText( Cell* cell ) { cell = cell; };
private:
    Dt( const Dt& rhs );            //no copy
    Dt& operator=( const Dt& rhs ); //no assignment
    unsigned char indent;
    unsigned char tabSize;
    unsigned char textLength;
    Dl::Break breakage;
    bool compact;
};

class Dd : public Tag {
public:
    Dd( Document* d, Element *p, const std::wstring* f, unsigned int r,
        unsigned int c, unsigned char i, unsigned char t, bool brk ) :
        Tag( d, p, f, r, c ), indent( i ), tabSize( t ), doBreak( brk ) { };
    ~Dd() { };
    Lexer::Token parse( Lexer* lexer );
    void linearize( Page* page ) { linearizeChildren( page ); };
    void buildText( Cell* cell ) { cell = cell; };
private:
    Dd( const Dd& rhs );            //no copy
    Dd& operator=( const Dd& rhs ); //no assignment
    unsigned char indent;
    unsigned char tabSize;
    bool doBreak;
};

#endif //DL_INCLUDED
