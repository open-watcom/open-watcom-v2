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
* Description: A ctrl tag
*   :ctrl
*       ctrlid=[::alphanum::]+
*       controls='' (Esc, Search, Print, Index, Contents, Back, Forward, id)
*       page
*       coverpage
*   Must be a child of :ctrldef
*   Must follow :pbutton
*
****************************************************************************/

#ifndef CTRL_INCLUDED
#define CTRL_INCLUDED

#include "ctrltag.hpp"

class Document; //forward reference

class Ctrl : public CtrlTag {
public:
    Ctrl( Document* d, Element* p, const std::wstring* f, unsigned int r, unsigned int c ) :
        CtrlTag( d, p, f, r, c ), page( false ), coverpage( false ) { };
    ~Ctrl() { };
    Lexer::Token parse( Lexer* lexer );
    void buildText( Cell* cell ) { cell = cell; };
    void build( Controls* ctrls );
private:
    Ctrl( const Ctrl& rhs );            //no copy
    Ctrl& operator=( const Ctrl& rhs ); //no assignment
    std::wstring ctrlid;                //identifier
    std::wstring controls;              //control type
    bool page;
    bool coverpage;
};

#endif //CTRL_INCLUDED
