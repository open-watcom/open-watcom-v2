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
* Description:  Process nt/ent tags
*
*   :nt / :ent
*       text=[a-zA-z0-9]* (in place of data from nls file)
*   New paragraph, contents follows 'Note:  ' on line
*   Text left aligns on column after 'text'
*
****************************************************************************/

#ifndef NT_INCLUDED
#define NT_INCLUDED

#include "tag.hpp"

class Nt : public Tag {
public:
    Nt( Document* d, Element *p, const std::wstring* f, unsigned int r, unsigned int c ) :
        Tag( d, p, f, r, c ) { };
    ~Nt() { };
    Lexer::Token parse( Lexer* lexer );
    void linearize( Page* page ) { linearizeChildren( page ); };
    void buildText( Cell* cell ) { cell = cell; };
private:
    Nt( const Nt& rhs );                //no copy
    Nt& operator=( const Nt& rhs );     //no assignment
    static void prepBufferName( std::wstring* buffer, const std::wstring& fname );
};

class ENt : public Tag {
public:
    ENt( Document* d, Element *p, const std::wstring* f, unsigned int r, unsigned int c ) :
        Tag( d, p, f, r, c ) { };
    ~ENt() { };
    void buildText( Cell* cell );
private:
    ENt( const ENt& rhs );              //no copy
    ENt& operator=( const ENt& rhs );   //no assignment
};

class NtLm : public Element {
public:
    NtLm( Document* d, Element *p, const std::wstring* f, unsigned int r, unsigned int c ) :
        Element( d, p, f, r, c ) { };
    ~NtLm() { };
    Lexer::Token parse( Lexer* lexer ) { lexer = lexer; return Lexer::END; };
    void buildText( Cell* cell );
private:
    NtLm( const NtLm& rhs );            //no copy
    NtLm& operator=( const NtLm& rhs ); //no assignment
};

#endif //NT_INCLUDED
