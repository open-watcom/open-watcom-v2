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
* Description:  Process lines/elines tags
*
*   :lines / :elines
*       align=left|center|right
*   Honor whitespace (becomes literal whitespace), including '\n' (as .br)
*
****************************************************************************/

#ifndef LINES_INCLUDED
#define LINES_INCLUDED

#include "tag.hpp"

class Lines : public Tag {
public:
    Lines( Document* d, Element *p, const std::wstring* f, unsigned int r,
            unsigned int c ) : Tag( d, p, f, r, c, Tag::LITERAL ), alignment( LEFT ) { }
    ~Lines() { };
    Lexer::Token parse( Lexer* lexer );
    void buildText( Cell* cell );
protected:
    Lexer::Token parseAttributes( Lexer* lexer );
private:
    Lines( const Lines& rhs );              //no copy
    Lines& operator=( const Lines& rhs );   //no assignment
    enum Align {
        LEFT = 0x01,
        RIGHT = 0x02,
        CENTER = 0x04
    };
    Align alignment;
};

class ELines : public Tag {
public:
    ELines( Document* d, Element *p, const std::wstring* f, unsigned int r, 
            unsigned int c ) : Tag( d, p, f, r, c ) { }
    ~ELines() { };
    void buildText( Cell* cell );
private:
    ELines( const ELines& rhs );            //no copy
    ELines& operator=( const ELines& rhs ); //no assignment
};

#endif //LINES_INCLUDED
