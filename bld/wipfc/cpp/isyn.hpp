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
* Description:  Process isyn tag
*
*   :isyn
*       root= (single word)
*   Followed by space separated list of synonyms
*
****************************************************************************/

#ifndef ISYN_INCLUDED
#define ISYN_INCLUDED

#include <memory>
#include "element.hpp"
#include "synonym.hpp"

class ISyn : public Element {
public:
    ISyn( Document* d, Element* p, const std::wstring* f, unsigned int r, unsigned int c ) :
        Element( d, p, f, r, c ), syn( new Synonym() ) { };
    ~ISyn() { };
    Lexer::Token parse( Lexer* lexer );
    void buildText( Cell* cell ) { cell = cell; };
private:
    ISyn( const ISyn& rhs );                //no copy
    ISyn& operator=( const ISyn& rhs );     //no assignment
    std::wstring root;
    std::auto_ptr< Synonym > syn;
};

#endif //ISYN_INCLUDED
