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
* Description:  An abstract class for any element (tag, command, text)
*
****************************************************************************/

#ifndef ELEMENT_INCLUDED
#define ELEMENT_INCLUDED

#include <string>
#include "lexer.hpp"
#include "errors.hpp"

class Document; //forward references
class Page;
class Cell;

class Element {
public:
    Element( Document* d, Element* p, const std::wstring* f, unsigned int r, unsigned int c ) :
        _document( d ), _parent( p ), _fileName( f ), _row( r ), _col( c ) { };
    virtual
    ~Element() { };
    //parse the attributes of this element, and parse allowed child elements
    virtual
    Lexer::Token parse( Lexer* lexer ) = 0;
    //add data to the TOC entry of a page
    virtual
    void buildTOC( Page* page ) { (void)page; };
    //add information from i1, i2, icmd
    virtual
    void buildIndex() { };
    //add this element and all children to page
    virtual
    void linearize( Page* page );
    //add words, punctuation to local dictionary
    virtual
    std::pair< bool, bool > buildLocalDict( Page* page ) { (void)page; return std::make_pair( false, false ); };
    //add encoded text to a cell
    virtual
    void buildText( Cell* cell ) = 0;
    //get this elements containing element
    const Element* owner() const { return _parent; };
    //get the ultimate root element
    Element* rootElement();
    //print an error message (not during parsing)
    void printError( ErrCode c ) const;
    void printError( ErrCode c, const std::wstring& text ) const;

private:
    Element( const Element& rhs );              //no copy
    Element& operator=( const Element& rhs );   //no assignment

protected:
    Document*           _document;
    Element*            _parent;
    //element was parsed from this source file
    const std::wstring* _fileName;
    //at this row
    unsigned int        _row;
    //and this column
    unsigned int        _col;
};

#endif //ELEMENT_INCLUDED
