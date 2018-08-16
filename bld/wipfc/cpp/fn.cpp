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
* Description:  Process fn/efn tags
*
*   :fn / :efn
*       id=[a-zA-z][a-zA-z0-9]*
*   Index entries invalid
*   Cannot be linked to from a child window
*   Not searchable
*
****************************************************************************/


#include "wipfc.hpp"
#include "fn.hpp"
#include "cell.hpp"
#include "document.hpp"
#include "lexer.hpp"
#include "page.hpp"
#include "tocref.hpp"
#include "util.hpp"

Lexer::Token Fn::parse( Lexer* lexer )
{
    Lexer::Token tok( parseAttributes( lexer ) );
//    bool done( false );
    while( tok != Lexer::END && !( tok == Lexer::TAG && lexer->tagId() == Lexer::EUSERDOC)) {
        if( lexer->tagId() == Lexer::EFN ) {
            tok = Tag::parseAttributes( lexer );
            break;
        } else if( parseInline( lexer, tok ) ) {
            if( parseBlock( lexer, tok ) ) {
                if( parseListBlock( lexer, tok ) ) {
                    parseCleanup( lexer, tok );
                }
            }
        }
    }
    return tok;
}
/***************************************************************************/
Lexer::Token Fn::parseAttributes( Lexer* lexer )
{
    Lexer::Token tok;

    while( (tok = _document->getNextToken()) != Lexer::TAGEND ) {
        if( tok == Lexer::ATTRIBUTE ) {
            std::wstring key;
            std::wstring value;
            splitAttribute( lexer->text(), key, value );
            if( key == L"id" ) {
                _id = new GlobalDictionaryWord( value );
                _id->toUpper();          //to uppercase
                if( !_document->isInf() ) {
                    _id = _document->addTextToGD( _id );
                }
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
    if( !_id )
        _document->printError( ERR1_NOFNID );
    return _document->getNextToken();    //consume TAGEND
}
/***************************************************************************/
void Fn::buildTOC( Page* page )
{
    page->setTOC( _toc );
    if( _id ) {
        TocRef tr( _fileName, _row, page->index() );
        _document->addNameOrId( _id, tr );
    }
}

