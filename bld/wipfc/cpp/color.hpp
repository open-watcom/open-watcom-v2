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
* Description:  Process color tag
*
*   :color fc=name bc=name
*   Valid names are: default, black, blue, red, pink, green, cyan, yellow, neutral,
*   brown, darkgray, darkblue, darkred, darkpink, darkgreen darkcyan, palegray
*   Remains active until changed, or the next header
*
****************************************************************************/

#ifndef COLOR_INCLUDED
#define COLOR_INCLUDED

#include "element.hpp"

class Color : public Element {
public:
    Color( Document* d, Element *p, const std::wstring* f, unsigned int r, unsigned int c ) :
        Element( d, p, f, r, c ), foreground( DEFAULT ), background( DEFAULT ), setForeground( false ),
        setBackground( false ) { };
    ~Color() { };
    Lexer::Token parse( Lexer* lexer );
    void buildText( Cell* cell );
private:
    Color( const Color& rhs );              //no copy
    Color& operator=( const Color& rhs );   //no assignment
    enum ColorName {
        DEFAULT,
        BLUE,
        RED,
        PINK,
        GREEN,
        CYAN,
        YELLOW,
        NEUTRAL,
        BROWN = NEUTRAL,
        DARKGRAY,
        DARKBLUE,
        DARKRED,
        DARKPINK,
        DARKGREEN,
        DARKCYAN,
        BLACK,
        PALEGRAY
    };
    ColorName foreground;
    ColorName background;
    bool setForeground;
    bool setBackground;
    ColorName parseColor( std::wstring& name );
};

#endif //COLOR_INCLUDED
