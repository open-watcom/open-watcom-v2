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
* Description:  Process artwork tag
*
*   :artwork
*       name='' (bitmap)
*       align=left|center|right
*       linkfile='' (see :artlink above)
*       runin (inline, else it's a block)
*       fit
*
****************************************************************************/

#ifndef ARTWORK_INCLUDED
#define ARTWORK_INCLUDED

#include "element.hpp"

class Artwork : public Element {
public:
    Artwork( Document* d, Element *p, const std::wstring* f, unsigned int r, unsigned int c ) :
        Element( d, p, f, r, c ), _linkfile( 0 ), _flags( 0 ), _hypergraphic( false ) { };
    ~Artwork() { };
    void setHypergraphic() { _hypergraphic = true; };
    bool isHypergraphic() const { return _hypergraphic; };
    Lexer::Token parse( Lexer* lexer );
    void buildText( Cell* cell );
private:
    Artwork( const Artwork& rhs );              //no copy
    Artwork& operator=( const Artwork& rhs );   //no assignment
    Lexer::Token parseAttributes( Lexer* lexer );
    enum Flags {
        LEFT = 0x01,
        RIGHT = 0x02,
        CENTER = 0x04,
        FIT = 0x08,
        RUNIN = 0x10,
        ALIGNMASK = 0xF8
    };
    std::wstring    _name;          //the bitmap file name
    std::wstring*   _linkfile;      //the link file name
    byte            _flags;
    bool            _hypergraphic;  //bitmap is a link
};

#endif //ARTWORK_INCLUDED
