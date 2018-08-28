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
* Description: lm tag
*
*   :lm
*       margin=[0-9]+ (default: 1)
*   If current position is past new margin, margin begins on next line
*
****************************************************************************/

#ifndef LM_INCLUDED
#define LM_INCLUDED

#include "tag.hpp"

class Lm : public Element {
public:
    Lm( Document* d, Element *p, const std::wstring* f, unsigned int r, unsigned int c ) :
        Element( d, p, f, r, c ), _margin( 1 ) { };
    Lm( Document* d, Element *p, const std::wstring* f, unsigned int r, unsigned int c, byte m ) :
        Element( d, p, f, r, c ), _margin( m ) { };
    ~Lm() { };
    Lexer::Token parse( Lexer* lexer );
    void buildText( Cell* cell );
private:
    Lm( const Lm& rhs );                //no copy
    Lm& operator=( const Lm& rhs );     //no assignment

    byte                _margin;
};

#endif //LM_INCLUDED
