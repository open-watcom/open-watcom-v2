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
*   :acviewport
*       dll=[a-zA-z][a-zA-z0-9]*
*       objectname=[a-zA-z][a-zA-z0-9]*
*       objectinfo=[a-zA-z][a-zA-z0-9]*
*       objectid=[0-9]*
*       vpx=([0-9]+[c|x|p|%]) | (left|center|right)
*       vpy=([0-9]+[c|x|p|%]) | (top|center|bottom)
*       vpcx=([0-9]+[c|x|p|%])
*       vpcy=([0-9]+[c|x|p|%])
*
****************************************************************************/

#ifndef ACVIEWPORT_INCLUDED
#define ACVIEWPORT_INCLUDED

#include "config.hpp"
#include "element.hpp"
#include "toc.hpp"

class AcViewport : public Element {
public:
    AcViewport( Document* d, Element *p, const std::wstring* f, unsigned int r, unsigned int c ) :
        Element( d, p, f, r, c ), objectId( 0 ), doOrigin( false ), doSize( false ) { };
    ~AcViewport() { };
    Lexer::Token parse( Lexer* lexer );
    void buildText( Cell* cell );
private:
    AcViewport( const AcViewport& rhs );              //no copy
    AcViewport& operator=( const AcViewport& rhs );   //no assignment
    std::wstring dll;
    std::wstring objectName;
    std::wstring objectInfo;
    STD1::uint16_t objectId;
    PageOrigin origin;
    PageSize size;
    bool doOrigin;
    bool doSize;
    Lexer::Token parseAttributes( Lexer* lexer );
};

#endif //ACVIEWPORT_INCLUDED
