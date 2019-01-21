/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2009-2018 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Process pbutton tag
*   :pbutton
*       id=[a-zA-z][a-zA-z0-9]*
*       res=[0-9]+
*       text=''
*
****************************************************************************/


#include "wipfc.hpp"
#include <algorithm>
#include <cwctype>
#include "pbutton.hpp"
#include "controls.hpp"
#include "document.hpp"
#include "util.hpp"

Lexer::Token PButton::parse( Lexer* lexer )
{
    Lexer::Token tok;

    while( (tok = _document->getNextToken()) != Lexer::TAGEND ) {
        if( tok == Lexer::ATTRIBUTE ) {
            std::wstring key;
            std::wstring value;
            splitAttribute( lexer->text(), key, value );
            if( key == L"id" ) {
                _id = value;
                std::transform( _id.begin(), _id.end(), _id.begin(), std::towupper );
                if( _id == L"ESC" ||
                    _id == L"SEARCH" ||
                    _id == L"PRINT" ||
                    _id == L"INDEX" ||
                    _id == L"CONTENTS" ||
                    _id == L"BACK" ||
                    _id == L"FORWARD" )
                    _document->printError( ERR3_DUPID, _id );
            } else if( key == L"res" ) {
                _res = std::wcstoul( value.c_str(), 0, 10 );
            } else if( key == L"text" ) {
                _text = value;
            } else {
                _document->printError( ERR1_ATTRNOTDEF );
            }
        } else if( tok == Lexer::FLAG ) {
            _document->printError( ERR1_ATTRNOTDEF );
        } else if( tok == Lexer::END ) {
            throw FatalError( ERR_EOF );
        } else {
            _document->printError( ERR1_TAGSYNTAX );
        }
    }
    return _document->getNextToken();
}
/***************************************************************************/
void PButton::build( Controls* ctrls)
{
    ControlButton btn( _id, static_cast< word >( _res ), _text );
    //don't allow duplicates of predefined buttons
    if( _id != L"ESC" &&
        _id != L"SEARCH" &&
        _id != L"PRINT" &&
        _id != L"INDEX" &&
        _id != L"CONTENTS" &&
        _id != L"BACK" &&
        _id != L"FORWARD" )
        ctrls->addButton( btn );
}
