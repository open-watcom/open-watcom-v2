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
* Description: hdref tag
*
*   :hdref
*       res=[0-9]+
*       refid=[A-Za-z0-9]+
*
****************************************************************************/

#ifndef HDREF_INCLUDED
#define HDREF_INCLUDED

#include "config.hpp"
#include "tag.hpp"

class Hdref : public Tag {
public:
    Hdref( Document* d, Element *p, const std::wstring* f, unsigned int r, unsigned int c ) :
        Tag( d, p, f, r, c ) { };
    ~Hdref() { };
    Lexer::Token parse( Lexer* lexer );
    void linearize( Page* page ) { linearizeChildren( page ); };
    void buildText( Cell* cell ) { cell = cell; };
private:
    Hdref( const Hdref& rhs );                //no copy
    Hdref& operator=( const Hdref& rhs );     //no assignment
    static void prepBufferName( std::wstring* buffer, const std::wstring& fname );
};

#endif //HDREF_INCLUDED
