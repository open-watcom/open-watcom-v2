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
* Description:  Process pbutton tag
*   :pbutton
*       id=[a-zA-z][a-zA-z0-9]*
*       res=[0-9]+
*       text=''
*
****************************************************************************/

#include <algorithm>
#include <cwctype>
#include "pbutton.hpp"
#include "controls.hpp"
#include "document.hpp"
#include "util.hpp"

Lexer::Token PButton::parse( Lexer* lexer )
{
    Lexer::Token tok( document->getNextToken() );
    while( tok != Lexer::TAGEND ) {
        if( tok == Lexer::ATTRIBUTE ) {
            std::wstring key;
            std::wstring value;
            splitAttribute( lexer->text(), key, value );
            if( key == L"id" ) {
                id = value;
                std::transform( id.begin(), id.end(), id.begin(), std::towupper );
                if( id == L"ESC" ||
                    id == L"SEARCH" ||
                    id == L"PRINT" ||
                    id == L"INDEX" ||
                    id == L"CONTENTS" ||
                    id == L"BACK" ||
                    id == L"FORWARD" )
                    document->printError( ERR3_DUPID, id );
            }
            else if( key == L"res" )
                res = std::wcstoul( value.c_str(), 0, 10 );
            else if( key == L"text" )
                text = value;
            else
                document->printError( ERR1_ATTRNOTDEF );
        }
        else if ( tok == Lexer::FLAG )
            document->printError( ERR1_ATTRNOTDEF );
        else if( tok == Lexer::END )
            throw FatalError( ERR_EOF );
        else
            document->printError( ERR1_TAGSYNTAX );
        tok = document->getNextToken();
    }
    return document->getNextToken();
}
/***************************************************************************/
void PButton::build( Controls* ctrls)
{
    ControlButton btn( id, static_cast< STD1::uint16_t >( res ), text);
    //don't allow duplicates of predefined buttons
    if( id != L"ESC" &&
        id != L"SEARCH" &&
        id != L"PRINT" &&
        id != L"INDEX" &&
        id != L"CONTENTS" &&
        id != L"BACK" &&
        id != L"FORWARD" )
        ctrls->addButton( btn );
}
