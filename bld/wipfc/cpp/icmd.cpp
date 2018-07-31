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
* Description:  Process icmd tag
*
*   :icmd
*   Must follow :hn or :in tag
*   Contents must be unique
*
****************************************************************************/


#include "wipfc.hpp"
#include "icmd.hpp"
#include "document.hpp"
#include "errors.hpp"
#include "gdword.hpp"
#include "xref.hpp"

ICmd::ICmd( Document* d, Element* p, const std::wstring* f, unsigned int r, unsigned int c ) :
    Element( d, p, f, r, c ), index( new IndexItem( IndexItem::CMD ) ),
            parentId( 0 ), parentRes( 0 )
{
    d->addCmdIndex( this );
}
/*****************************************************************************/
Lexer::Token ICmd::parse( Lexer* lexer )
{
    Lexer::Token tok( _document->getNextToken() );
    (void)lexer;
    while( tok != Lexer::TAGEND ) {
        if( tok == Lexer::ATTRIBUTE )
            _document->printError( ERR1_ATTRNOTDEF );
        else if( tok == Lexer::FLAG )
            _document->printError( ERR1_ATTRNOTDEF );
        else if( tok == Lexer::ERROR_TAG )
            throw FatalError( ERR_SYNTAX );
        else if( tok == Lexer::END )
            throw FatalError( ERR_EOF );
        else
            _document->printError( ERR1_TAGSYNTAX );
        tok = _document->getNextToken();
    }
    tok = _document->getNextToken();    //consume TAGEND
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
void ICmd::buildIndex()
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
    }
    catch( Class1Error& e ) {
        printError( e.code );
    }
}

