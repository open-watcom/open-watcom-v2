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
* Description:  Process hide/ehide tags
*
*   :hide / :ehide
*       key=''
*   Multiple single-quoted keys separated by '+'
*   Cannot nest
*   Cannot contain :hn with res attribute (link target)
*
****************************************************************************/

#ifndef HIDE_INCLUDED
#define HIDE_INCLUDED

#include "tag.hpp"

class Hide : public Element {
public:
    Hide( Document* d, Element *p, const std::wstring* f, unsigned int r,
        unsigned int c );
    ~Hide() { };
    static
    bool hiding() { return hide; };
    static
    void clear() { hide = false; };
    Lexer::Token parse( Lexer* lexer );
    void buildText( Cell* cell );
protected:
    Lexer::Token parseAttributes( Lexer* lexer );
private:
    Hide( const Hide& rhs );            //no copy
    Hide& operator=( const Hide& rhs ); //no assignment
    static bool hide;
    std::wstring keyPhrase;
};

class EHide : public Element {
public:
    EHide( Document* d, Element *p, const std::wstring* f, unsigned int r,
        unsigned int c );
    ~EHide() { };
    Lexer::Token parse( Lexer* lexer );
    void buildText( Cell* cell );
private:
    EHide( const EHide& rhs );              //no copy
    EHide& operator=( const EHide& rhs );   //no assignment
};

#endif //HIDE_INCLUDED
