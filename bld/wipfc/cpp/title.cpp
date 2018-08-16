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
* Description:  Process title tag
*   :title
*       Maximum 47 characters + null terminator
*
****************************************************************************/


#include "wipfc.hpp"
#include <cstdlib>
#include <string>
#include "title.hpp"
#include "lexer.hpp"
#include "document.hpp"
#include "outfile.hpp"


Lexer::Token Title::parse( Lexer* lexer )
{
    Lexer::Token tok;

    while( (tok = _document->getNextToken()) != Lexer::TAGEND ) {
        if( tok == Lexer::ATTRIBUTE ) {
            _document->printError( ERR1_ATTRNOTDEF );
        } else if( tok == Lexer::FLAG ) {
            _document->printError( ERR1_ATTRNOTDEF );
        } else if( tok == Lexer::END ) {
            throw FatalError( ERR_EOF );
        } else {
            _document->printError( ERR1_TAGSYNTAX );
        }
    }
    _fileName = _document->dataName();
    _row = _document->lexerLine();
    _col = _document->lexerCol();
    unsigned int startLine( _document->dataLine() );
    tok = _document->getNextToken();
    while(  _document->dataLine() == startLine ) {
        if( tok == Lexer::WHITESPACE ||
            tok == Lexer::WORD ||
            tok == Lexer::PUNCTUATION ) {
            _text += lexer->text();
        } else if( tok == Lexer::ENTITY ) {
            const std::wstring* exp( _document->nameit( lexer->text() ) );
            if( exp ) {
                _text += *exp;
            } else {
                try {
                    wchar_t entityChar( _document->entityChar( lexer->text() ) );
                    _text += entityChar;
                }
                catch( Class2Error& e ) {
                    _document->printError( e._code );
                }
            }
        } else if( tok == Lexer::END ) {
            throw FatalError( ERR_EOF );
        } else {
            break;
        }
        tok = _document->getNextToken();
    }
    return tok;
}

void Title::build( OutFile *out )
{
    //build Title
    std::string buffer( out->wtomb_string( _text ) );
    if( buffer.size() > TITLE_SIZE - 1 )
        _document->printError( ERR2_TEXTTOOLONG );
    _document->setTitle( buffer );
}
