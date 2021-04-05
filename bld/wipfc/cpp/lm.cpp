/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2009-2021 The Open Watcom Contributors. All Rights Reserved.
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
* Description: lm tag
*
*   :lm
*       margin=[0-9]+ (default: 1)
*   If current position is past new margin, margin begins on next line
*
****************************************************************************/


#include "wipfc.hpp"
#include <cstdlib>
#include "lm.hpp"
#include "cell.hpp"
#include "document.hpp"
#include "errors.hpp"
#include "lexer.hpp"
#include "page.hpp"
#include "util.hpp"

Lexer::Token Lm::parse( Lexer* lexer )
{
    Lexer::Token tok;

    while( (tok = _document->getNextToken()) != Lexer::TAGEND ) {
        //parse attributes
        if( tok == Lexer::ATTRIBUTE ) {
            std::wstring key;
            std::wstring value;
            splitAttribute( lexer->text(), key, value );
            if( key == L"margin" ) {
                long tmp( std::wcstol( value.c_str(), 0, 10 ) );
                if( tmp < 1 )
                    tmp = 1;
                if( tmp > 255 )
                    tmp = 255;
                _margin = static_cast< byte >( tmp );
                _document->setLeftMargin( _margin );
            } else {
                _document->printError( ERR1_ATTRNOTDEF );
            }
        } else if( tok == Lexer::FLAG ) {
            _document->printError( ERR1_ATTRNOTDEF );
        } else if( tok == Lexer::ERROR_TAG ) {
            throw FatalError( ERR_SYNTAX );
        } else if( tok == Lexer::END ) {
            throw FatalError( ERR_EOF );
        } else {
            _document->printError( ERR1_TAGSYNTAX );
        }
    }
    return _document->getNextToken();    //consume TAGEND
}
/***************************************************************************/
void Lm::buildText( Cell* cell )
{
    cell->addByte( Cell::ESCAPE );  //esc
    cell->addByte( 0x03 );          //size
    cell->addByte( 0x02 );          //set left margin
    cell->add( _margin );
    if( cell->textFull() ) {
        printError( ERR1_LARGEPAGE );
    }
}

