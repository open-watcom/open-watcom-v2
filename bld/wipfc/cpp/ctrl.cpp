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
* Description:  Process ctrl tag
*   :ctrl
*       ctrlid=[::alphanum::]+
*       controls='' (Search, Print, Index, Contents, Esc, Back, Forward, id)
*       page
*       coverpage
*   Must be a child of :ctrldef
*   Must follow :pbutton
*
****************************************************************************/


#include "wipfc.hpp"
#include <algorithm>
#include <cwctype>
#include "ctrl.hpp"
#include "controls.hpp"
#include "document.hpp"
#include "util.hpp"

Lexer::Token Ctrl::parse( Lexer* lexer )
{
    Lexer::Token tok;

    while( (tok = _document->getNextToken()) != Lexer::TAGEND ) {
        if( tok == Lexer::ATTRIBUTE ) {
            std::wstring key;
            std::wstring value;
            splitAttribute( lexer->text(), key, value );
            if( key == L"ctrlid" ) {
                _ctrlid = value;
                std::transform( _ctrlid.begin(), _ctrlid.end(), _ctrlid.begin(), std::towupper );
            } else if( key == L"controls" ) {
                _controls = value;
                std::transform( _controls.begin(), _controls.end(), _controls.begin(), std::towupper );
            } else {
                _document->printError( ERR1_ATTRNOTDEF );
            }
        } else if( tok == Lexer::FLAG ) {
            if( lexer->text() == L"page" ) {
                _page = true;
            } else if( lexer->text() == L"coverpage" ) {
                _coverpage = true;
            } else {
                _document->printError( ERR1_ATTRNOTDEF );
            }
        } else if( tok == Lexer::ERROR_TAG ) {
            throw FatalError( ERR_SYNTAX );
        } else if( tok == Lexer::END ) {
            throw FatalError( ERR_EOF );
        } else {
            _document->printError( ERR1_TAGSYNTAX );
        }
    }
    return _document->getNextToken();
}
/***************************************************************************/
void Ctrl::build( Controls* ctrls)
{
    if( ctrls->getGroupById( _ctrlid ) == 0 ) { //no duplicate group ids
        ControlGroup grp( _ctrlid );
        if( !_controls.empty() ) {
            std::wstring::size_type p1( 0 );
            while( p1 < std::wstring::npos ) {
                std::wstring::size_type p2( _controls.find( L' ', p1 ) );
                std::wstring temp( _controls.substr( p1, p2 - p1 ) );
                ControlButton* btn( ctrls->getButtonById( temp ) ); //check if button is present
                if( btn ) {
                    grp.addButtonIndex( btn->index() );
                } else {
                    if( temp == L"ESC" ) {
                        grp.addButtonIndex( 0 );
                    } else if( temp == L"SEARCH" ) {
                        grp.addButtonIndex( 1 );
                    } else if( temp == L"PRINT" ) {
                        grp.addButtonIndex( 2 );
                    } else if( temp == L"INDEX" ) {
                        grp.addButtonIndex( 3 );
                    } else if( temp == L"CONTENTS" ) {
                        grp.addButtonIndex( 4 );
                    } else if( temp == L"BACK" ) {
                        grp.addButtonIndex( 5 );
                    } else if( temp == L"FORWARD" ) {
                        grp.addButtonIndex( 6 );
                    } else {
                        printError( ERR3_NOBUTTON, temp );
                    }
                }
                p1 = p2 == std::wstring::npos ? std::wstring::npos : p2 + 1;
            }
            ctrls->addGroup( grp );
            if( _coverpage ) {
                ctrls->setCover( ctrls->group()->index() + 1);
            }
        }
    } else {
        printError( ERR3_DUPID, _ctrlid );
    }
}
