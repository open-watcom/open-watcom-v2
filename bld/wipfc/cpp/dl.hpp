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
        unsigned int c, byte n, byte i ) : Tag( d, p, f, r, c ),
        _nestLevel( n ), _indent( i ), _tabSize( 10 ), _breakage( NONE ), _compact( false ) { };
    ~Dl() { };
    Lexer::Token parse( Lexer* lexer );
    void linearize( Page* page ) { linearizeChildren( page ); };
    void buildText( Cell* cell ) { (void)cell; };
protected:
    Lexer::Token parseAttributes( Lexer* lexer );
private:
    Dl( const Dl& rhs );            //no copy
    Dl& operator=( const Dl& rhs ); //no assignment

    byte            _nestLevel;     //counts from 0
    byte            _indent;        //in character spaces
    byte            _tabSize;       //in character spaces
    Break           _breakage;
    bool            _compact;
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
        unsigned int c, byte i ) : Tag( d, p, f, r, c ), _indent( i ),
        _textLength( 0 ) { };
    ~DtHd() { };
    byte length() const { return _textLength; };
    Lexer::Token parse( Lexer* lexer );
    void linearize( Page* page ) { linearizeChildren( page ); };
    void buildText( Cell* cell ) { (void)cell; };
private:
    DtHd( const DtHd& rhs );            //no copy
    DtHd& operator=( const DtHd& rhs ); //no assignment

    byte                _indent;
    byte                _textLength;
};

class DdHd : public Tag {
public:
    DdHd( Document* d, Element *p, const std::wstring* f, unsigned int r,
        unsigned int c, byte i, byte t ) : Tag( d, p, f, r, c ),
        _indent( i ), _tabSize( t ) { };
    ~DdHd() { };
    Lexer::Token parse( Lexer* lexer );
    void linearize( Page* page ) { linearizeChildren( page ); };
    void buildText( Cell* cell ) { (void)cell; };
private:
    DdHd( const DdHd& rhs );            //no copy
    DdHd& operator=( const DdHd& rhs ); //no assignment

    byte                _indent;
    byte                _tabSize;
};

class Dt : public Tag {
public:
    Dt( Document* d, Element *p, const std::wstring* f, unsigned int r,
        unsigned int c, byte i, byte t, Dl::Break brk, bool cp ) :
        Tag( d, p, f, r, c ), _indent( i ), _tabSize( t ), _textLength( 0 ),
        _breakage( brk ), _compact( cp ) { };
    ~Dt() { };
    Lexer::Token parse( Lexer* lexer );
    void linearize( Page* page ) { linearizeChildren( page ); };
    void buildText( Cell* cell ) { (void)cell; };
private:
    Dt( const Dt& rhs );            //no copy
    Dt& operator=( const Dt& rhs ); //no assignment

    byte                _indent;
    byte                _tabSize;
    byte                _textLength;
    Dl::Break           _breakage;
    bool                _compact;
};

class Dd : public Tag {
public:
    Dd( Document* d, Element *p, const std::wstring* f, unsigned int r,
        unsigned int c, byte i, byte t, bool brk ) :
        Tag( d, p, f, r, c ), _indent( i ), _tabSize( t ), _doBreak( brk ) { };
    ~Dd() { };
    Lexer::Token parse( Lexer* lexer );
    void linearize( Page* page ) { linearizeChildren( page ); };
    void buildText( Cell* cell ) { (void)cell; };
private:
    Dd( const Dd& rhs );            //no copy
    Dd& operator=( const Dd& rhs ); //no assignment

    byte                _indent;
    byte                _tabSize;
    bool                _doBreak;
};

#endif //DL_INCLUDED
