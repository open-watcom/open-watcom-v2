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
* Description:  Process fn/efn tags
*
*   :fn / :efn
*       id=[a-zA-z][a-zA-z0-9]*
*   Index entries invalid
*   Cannot be linked to from a split window
*   Not searchable
*
****************************************************************************/

#ifndef FN_INCLUDED
#define FN_INCLUDED

#include "tag.hpp"
#include "toc.hpp"

class GlobalDictionaryWord; //forward references

class Fn : public Tag {
public:
    Fn( Document* d, Element *p, const std::wstring* f, unsigned int r, \
        unsigned int c ) : Tag( d, p, f, r, c ), id( 0 ) { };
    ~Fn() { };
    Lexer::Token parse( Lexer* lexer );
    void buildTOC( Page* page );
    void linearize( Page* page ) { linearizeChildren( page ); };
    void buildText( Cell* cell ) { cell = cell; };
protected:
    Lexer::Token parseAttributes( Lexer* lexer );
private:
    Fn( const Fn& rhs );                //no copy
    Fn& operator=( const Fn& rhs );     //no assignment
    TocEntry toc;
    GlobalDictionaryWord* id;
};

#endif //FN_INCLUDED
