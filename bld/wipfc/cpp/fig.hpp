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
* Description:  Process fig/efig tags
*
*   :fig / :efig
*   Insert blank line before contents
*   Proportional font
*   Honor whitespace, including '\n'
****************************************************************************/

#ifndef FIG_INCLUDED
#define FIG_INCLUDED

#include "tag.hpp"

class Fig : public Tag {
public:
    Fig( Document* d, Element *p, const std::wstring* f, unsigned int r, \
        unsigned int c ) : Tag( d, p, f, r, c, Tag::LITERAL ) { };
    ~Fig() { };
    Lexer::Token parse( Lexer* lexer );
    void buildText( Cell* cell );
private:
    Fig( const Fig& rhs );              //no copy
    Fig& operator=( const Fig& rhs );   //no assignment
};

class EFig : public Tag {
public:
    EFig( Document* d, Element *p, const std::wstring* f, unsigned int r, \
        unsigned int c ) : Tag( d, p, f, r, c ) { };
    ~EFig() { };
    void buildText( Cell* cell );
private:
    EFig( const EFig& rhs );            //no copy
    EFig& operator=( const EFig& rhs ); //no assignment
};

#endif //FIG_INCLUDED
