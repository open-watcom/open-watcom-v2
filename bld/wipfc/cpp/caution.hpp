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
* Description:  Process caution/ecaution tags
*
*   :caution / :ecaution
*       text='' (in place of data from nls file)
*   Prompt appears on its own line, contents not indented
*
****************************************************************************/

#ifndef CAUTION_INCLUDED
#define CAUTION_INCLUDED

#include "tag.hpp"

class Caution : public Tag {
public:
    Caution( Document* d, Element *p, const std::wstring* f, unsigned int r,
        unsigned int c ) : Tag( d, p, f, r, c ) { };
    ~Caution() { };
    Lexer::Token parse( Lexer* lexer );
    void linearize( Page* page ) { linearizeChildren( page ); };
    void buildText( Cell* cell ) { cell = cell; };
private:
    Caution( const Caution& rhs );              //no copy
    Caution& operator=( const Caution& rhs );   //no assignment
    static void prepBufferName( std::wstring* buffer, const std::wstring& fname );
};

class ECaution : public Tag {
public:
    ECaution( Document* d, Element *p, const std::wstring* f, unsigned int r,
        unsigned int c ) : Tag( d, p, f, r, c ) { };
    ~ECaution() { };
    void buildText( Cell* cell );
private:
    ECaution( const ECaution& rhs );            //no copy
    ECaution& operator=( const ECaution& rhs ); //no assignment
};

#endif //CAUTION_INCLUDED
