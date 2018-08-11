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
* Description:  Process i2 tags
*
*   :i2
*       refid=[a-zA-z][a-zA-z0-9]*
*       global
*       sortkey='key-text'.index-text
*   Must follow :i1
*
****************************************************************************/


#include "wipfc.hpp"
#include <algorithm>
#include "i2.hpp"
#include "document.hpp"
#include "errors.hpp"
#include "gdword.hpp"
#include "i1.hpp"
#include "ptrops.hpp"
#include "util.hpp"
#include "xref.hpp"

Lexer::Token I2::parse( Lexer* lexer )
{
    Lexer::Token tok( parseAttributes( lexer ) );
    std::wstring text;
    while( tok != Lexer::END && !( tok == Lexer::TAG && lexer->tagId() == Lexer::EUSERDOC)) {
        if( tok == Lexer::WORD ) {
            text += lexer->text();
        } else if( tok == Lexer::ENTITY ) {
            const std::wstring* exp( _document->nameit( lexer->text() ) );
            if( exp ) {
                text += *exp;
            } else {
                try {
                    wchar_t entityChar( _document->entityChar( lexer->text() ) );
                    text += entityChar;
                }
                catch( Class2Error& e ) {
                    _document->printError( e._code );
                }
            }
        } else if( tok == Lexer::PUNCTUATION ) {
            text += lexer->text();
        } else if( tok == Lexer::WHITESPACE ) {
            if( lexer->text()[0] == L'\n' ) {
                tok = _document->getNextToken();
                break;
            }
            text += lexer->text();
        } else {
            break;
        }
        tok = _document->getNextToken();
    }
    if( text.empty() )
        _document->printError( ERR2_INOTEXT );
    _index->setText( text );
    return tok;
}
/*****************************************************************************/
Lexer::Token I2::parseAttributes( Lexer* lexer )
{
    Lexer::Token tok;

    while( (tok = _document->getNextToken()) != Lexer::TAGEND ) {
        if( tok == Lexer::ATTRIBUTE ) {
            std::wstring key;
            std::wstring value;
            splitAttribute( lexer->text(), key, value );
            if( key == L"refid" ) {
                _refid = value;
            } else if( key == L"sortkey" ) {
                _index->setSortKey( value );
            } else {
                _document->printError( ERR1_ATTRNOTDEF );
            }
        } else if( tok == Lexer::FLAG ) {
            if( lexer->text() == L"global" ) {
                if( !_document->isInf() )    //only for hlp files
                    _index->setGlobal();
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
    if( _refid.empty() )
        _document->printError( ERR1_NOREFID );
    return _document->getNextToken(); //consume TAGEND
}
/*****************************************************************************/
void I2::buildIndex()
{
    try {
        XRef xref( _fileName, _row );
        if( _parentRes ) {
            _index->setTOC( _document->tocIndexByRes( _parentRes ) );
            _document->addXRef( _parentRes, xref );
        } else if( _parentId ) {
            _index->setTOC( _document->tocIndexById( _parentId ) );
            _document->addXRef( _parentId, xref );
        }
        I1* i1( _document->indexById( _refid ) );
        i1->addSecondary( _index.get() );
    }
    catch( Class1Error& e ) {
        printError( e._code );
    }
}

