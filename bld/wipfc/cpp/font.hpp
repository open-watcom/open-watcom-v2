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
* Description: font tag
*
*   :font
*       facename=default|Courier|Helv|Tms Rmn|System Monospaced|System Proportional
*       size=HxW (in points)
*       codepage= [0-9]*
*   Resets to default at each new 'page'
*   facename=default and/or size=0x0 resets to default font
*
****************************************************************************/

#ifndef FONT_INCLUDED
#define FONT_INCLUDED

#include "tag.hpp"

class Font : public Element {
public:
    Font( Document* d, Element *p, const std::wstring* f, unsigned int r, unsigned int c ) :
        Element( d, p, f, r, c ), index( 0 ) { };
    ~Font() { };
    Lexer::Token parse( Lexer* lexer );
    void buildText( Cell* cell );
private:
    Font( const Font& rhs );            //no copy
    Font& operator=( const Font& rhs ); //no assignment
    unsigned char index;                //index of this item in the font collection
};

#endif
