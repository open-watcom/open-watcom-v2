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
* Description:  Process p tag
*
*  :p
*  Insert blank line (0xFA) before contents
*
****************************************************************************/

#ifndef P_INCLUDED
#define P_INCLUDED

#include "tag.hpp"

class P : public Tag {
public:
    P( Document* d, Element *p, const std::wstring* f, unsigned int r, unsigned int c ) :
        Tag( d, p, f, r, c ) { };
    ~P() { };
    Lexer::Token parse( Lexer* lexer );
    void buildText( Cell* cell );
private:
    P( const P& rhs );                  //no copy
    P& operator=( const P& rhs );       //no assignment
};

#endif //P_INCLUDED
