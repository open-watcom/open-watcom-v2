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
* Description:  An abstract class for a tag element
*
****************************************************************************/

#ifndef TAG_INCLUDED
#define TAG_INCLUDED

#include <list>
#include "element.hpp"

class Tag : public Element {
public:
    enum WsHandling {
        NONE,
        SPACES,
        LITERAL
    };
    Tag( Document* d, Element* p, const std::wstring* f, unsigned int r,
         unsigned int c, WsHandling ws = NONE ) :
         Element( d, p, f, r, c ), whiteSpace( ws ) { };
    ~Tag();
    //add an element to the container of elements within this tag
    void appendChild( Element* e ) { children.push_back( e ); };
    Lexer::Token parse( Lexer* lexer );
    void buildIndex();
    void linearize( Page* page );
    void linearizeChildren( Page* page );
protected:
    std::list< Element* > children;
    typedef std::list< Element* >::iterator ChildrenIter;
    typedef std::list< Element* >::const_iterator ConstChildrenIter;
    WsHandling whiteSpace;
    virtual
    Lexer::Token parseAttributes( Lexer* lexer );
    bool parseInline( Lexer* lexer, Lexer::Token& tok );
    bool parseBlock( Lexer* lexer, Lexer::Token& tok );
    bool parseListBlock( Lexer* lexer, Lexer::Token& tok );
    void parseCleanup( Lexer* lexer, Lexer::Token& tok );
private:
    Tag( const Tag& rhs );              //no copy
    Tag& operator=( const Tag& rhs );   //no assignment
};

#endif //TAG_INCLUDED
