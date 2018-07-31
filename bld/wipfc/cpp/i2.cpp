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
    std::wstring txt;
    while( tok != Lexer::END && !( tok == Lexer::TAG && lexer->tagId() == Lexer::EUSERDOC)) {
        if( tok == Lexer::WORD )
            txt += lexer->text();
        else if( tok == Lexer::ENTITY ) {
            const std::wstring* exp( _document->nameit( lexer->text() ) );
            if( exp )
                txt += *exp;
            else {
                try {
                    wchar_t ch( _document->entity( lexer->text() ) );
                    txt += ch;
                }
                catch( Class2Error& e ) {
                    _document->printError( e.code );
                }
            }
        }
        else if( tok == Lexer::PUNCTUATION )
            txt += lexer->text();
        else if( tok == Lexer::WHITESPACE ) {
            if( lexer->text()[0] == L'\n' ) {
                tok = _document->getNextToken();
                break;
            }
            txt+= lexer->text();
        }
        else
            break;
        tok = _document->getNextToken();
    }
    if( txt.empty() )
        _document->printError( ERR2_INOTEXT );
    index->setText( txt );
    return tok;
}
/*****************************************************************************/
Lexer::Token I2::parseAttributes( Lexer* lexer )
{
    Lexer::Token tok( _document->getNextToken() );
    while( tok != Lexer::TAGEND ) {
        if( tok == Lexer::ATTRIBUTE ) {
            std::wstring key;
            std::wstring value;
            splitAttribute( lexer->text(), key, value );
            if( key == L"refid" )
                refid = value;
            else if( key == L"sortkey" )
                index->setSortKey( value );
            else
                _document->printError( ERR1_ATTRNOTDEF );
        }
        else if( tok == Lexer::FLAG ) {
            if( lexer->text() == L"global" ) {
                if( !_document->isInf() )    //only for hlp files
                    index->setGlobal();
            }
            else
                _document->printError( ERR1_ATTRNOTDEF );
        }
        else if( tok == Lexer::ERROR_TAG )
            throw FatalError( ERR_SYNTAX );
        else if( tok == Lexer::END )
            throw FatalError( ERR_EOF );
        else
            _document->printError( ERR1_TAGSYNTAX );
        tok = _document->getNextToken();
    }
    if( refid.empty() )
        _document->printError( ERR1_NOREFID );
    return _document->getNextToken(); //consume TAGEND
}
/*****************************************************************************/
void I2::buildIndex()
{
    try {
        XRef xref( _fileName, _row );
        if( parentRes ) {
            index->setTOC( _document->tocIndexByRes( parentRes ) );
            _document->addXRef( parentRes, xref );
        }
        else if( parentId ) {
            index->setTOC( _document->tocIndexById( parentId ) );
            _document->addXRef( parentId, xref );
        }
        I1* i1( _document->indexById( refid ) );
        i1->addSecondary( index.get() );
    }
    catch( Class1Error& e ) {
        printError( e.code );
    }
}

