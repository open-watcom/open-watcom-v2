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
        Element( d, p, f, r, c ), linkfile( 0 ), flags( 0 ), hypergraphic( false ) { };
    ~Artwork() { };
    void setHypergraphic() { hypergraphic = true; };
    bool isHypergraphic() const { return hypergraphic; };
    Lexer::Token parse( Lexer* lexer );
    void buildText( Cell* cell );
private:
    Artwork( const Artwork& rhs );              //no copy
    Artwork& operator=( const Artwork& rhs );   //no assignment
    enum Flags {
        LEFT = 0x01,
        RIGHT = 0x02,
        CENTER = 0x04,
        FIT = 0x08,
        RUNIN = 0x10,
        ALIGNMASK = 0xF8
    };
    std::wstring name;      //the bitmap file name
    std::wstring* linkfile; //the link file name
    unsigned char flags;
    bool hypergraphic;      //bitmap is a link
    Lexer::Token parseAttributes( Lexer* lexer );
};

#endif //ARTWORK_INCLUDED
