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
* Description:  Process acviewport tag
*
*   :ddf
*       res=[0-9]+
*
****************************************************************************/

#ifndef DDF_INCLUDED
#define DDF_INCLUDED

#include "config.hpp"
#include "element.hpp"

class Ddf : public Element {
public:
    Ddf( Document* d, Element *p, const std::wstring* f, unsigned int r, unsigned int c ) :
        Element( d, p, f, r, c ), res( 0 ) { };
    ~Ddf() { };
    Lexer::Token parse( Lexer* lexer );
    void buildText( Cell* cell );
private:
    Ddf( const Ddf& rhs );              //no copy
    Ddf& operator=( const Ddf& rhs );   //no assignment
    STD1::uint16_t res;
    Lexer::Token parseAttributes( Lexer* lexer );
};

#endif //DDF_INCLUDED
