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
* Description:  Process ol/eol tags
*
*   :ol / :eol
*       compact (else blank line between items)
*       verycompact (no extra lines at all; a wipfc extension)
*   If nested, indent 4 spaces
*
****************************************************************************/


#include "wipfc.hpp"
#include <limits>
#include "ol.hpp"
#include "brcmd.hpp"
#include "dl.hpp"
#include "cell.hpp"
#include "document.hpp"
#include "errors.hpp"
#include "lexer.hpp"
#include "lm.hpp"
#include "lp.hpp"
#include "p.hpp"
#include "parml.hpp"
#include "punct.hpp"
#include "sl.hpp"
#include "ul.hpp"
#include "whtspc.hpp"
#include "word.hpp"

Lexer::Token Ol::parse( Lexer* lexer )
{
    Lexer::Token tok( parseAttributes( lexer ) );
    unsigned int itemCount( 0 );
    bool needLine( true );
    while( tok != Lexer::END && !( tok == Lexer::TAG && lexer->tagId() == Lexer::EUSERDOC ) ) {
        if( parseInline( lexer, tok ) ) {
            switch( lexer->tagId() ) {
            case Lexer::DL:
                {
                    Element* elt( new Dl( _document, this, _document->dataName(),
                        _document->dataLine(), _document->dataCol(), nestLevel + 1,
                        indent == 1 ? 4 : indent + 3 ) );
                    appendChild( elt );
                    tok = elt->parse( lexer );
                    needLine = true;
                }
                break;
            case Lexer::OL:
                {
                    Element* elt( new Ol( _document, this, _document->dataName(),
                        _document->dataLine(), _document->dataCol(),
                        nestLevel + 1, indent == 1 ? 4 : indent + 3 ) );
                    appendChild( elt );
                    tok = elt->parse( lexer );
                    needLine = true;
                }
                break;
            case Lexer::LI:
                {
                    Element* elt( new OlLi( _document, this, _document->dataName(),
                        _document->dataLine(), _document->dataCol(),
                        itemCount++, nestLevel, indent, veryCompact ||
                        ( compact && !needLine ) ) );
                    appendChild( elt );
                    tok = elt->parse( lexer );
                    needLine = false;
                }
                break;
            case Lexer::LP:
                {
                    Element* elt( new Lp( _document, this, _document->dataName(),
                        _document->dataLine(), _document->dataCol(), indent + 3 ) );
                    appendChild( elt );
                    tok = elt->parse( lexer );
                    needLine = true;
                }
                break;
            case Lexer::EOL:
                {
                    Element* elt( new EOl( _document, this, _document->dataName(),
                        _document->dataLine(), _document->dataCol() ) );
                    appendChild( elt );
                    tok = elt->parse( lexer );
                    if( !nestLevel )
                        appendChild( new BrCmd( _document, this, _document->dataName(),
                            _document->dataLine(), _document->dataCol() ) );
                    return tok;
                }
            case Lexer::PARML:
                {
                    Element* elt( new Parml( _document, this, _document->dataName(),
                        _document->dataLine(), _document->dataCol(), nestLevel + 1,
                        indent == 1 ? 4 : indent + 3 ) );
                    appendChild( elt );
                    tok = elt->parse( lexer );
                    needLine = true;
                }
                break;
            case Lexer::SL:
                {
                    Element* elt( new Sl( _document, this, _document->dataName(),
                        _document->dataLine(), _document->dataCol(),
                        0, indent == 1 ? 4 : indent + 3 ) );
                    appendChild( elt );
                    tok = elt->parse( lexer );
                    needLine = true;
                }
                break;
            case Lexer::UL:
                {
                    Element* elt( new Ul( _document, this, _document->dataName(),
                        _document->dataLine(), _document->dataCol(),
                        0, indent == 1 ? 4 : indent + 3 ) );
                    appendChild( elt );
                    tok = elt->parse( lexer );
                    needLine = true;
                }
                break;
            default:
                _document->printError( ERR1_NOENDLIST );
                return tok;
            }
        }
    }
    return tok;
}
/***************************************************************************/
Lexer::Token Ol::parseAttributes( Lexer* lexer )
{
    Lexer::Token tok( _document->getNextToken() );
    (void)lexer;
    while( tok != Lexer::TAGEND ) {
        if( tok == Lexer::ATTRIBUTE )
            _document->printError( ERR1_ATTRNOTDEF );
        else if( tok == Lexer::FLAG ) {
            if( lexer->text() == L"compact" )
                compact = true;
            else if( lexer->text() == L"verycompact" )
                veryCompact = true;
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
    return _document->getNextToken();    //consume TAGEND
}
/***************************************************************************/
void EOl::buildText( Cell* cell )
{
    cell->addByte( 0xFF );  //esc
    cell->addByte( 0x03 );  //size
    cell->addByte( 0x02 );  //set left margin
    cell->addByte( 1 );
    if( cell->textFull() )
        printError( ERR1_LARGEPAGE );
}
/***************************************************************************/
Lexer::Token OlLi::parse( Lexer* lexer )
{
    Lexer::Token tok( parseAttributes( lexer ) );
    appendChild( new Lm( _document, this, _document->dataName(), _document->dataLine(),
        _document->dataCol(), indent ) );
    if( compact )
        appendChild( new BrCmd( _document, this, _document->dataName(), _document->dataLine(),
            _document->dataCol() ) );
    else
        appendChild( new P( _document, this, _document->dataName(), _document->dataLine(),
            _document->dataCol() ) );
    if( nestLevel & 1 ) {
        std::wstring txt;
        txt.push_back( _document->olChar( itemNumber ) );
        appendChild( new LiteralWhiteSpace( _document, this, _document->dataName(),
            _document->dataLine(), _document->dataCol(), true ) );
        appendChild( new Word( _document, this, _document->dataName(),
            _document->dataLine(), _document->dataCol(), txt, false ) );
        appendChild( new Punctuation( _document, this, _document->dataName(),
            _document->dataLine(), _document->dataCol(), _document->olClose( nestLevel ), true ) );
    }
    else {
        wchar_t tmp[ 4 ];
        std::swprintf( tmp, 4, L"%u", itemNumber + 1 );
        std::wstring txt( tmp );
        if( itemNumber < 9 )
            appendChild( new LiteralWhiteSpace( _document, this, _document->dataName(),
                _document->dataLine(), _document->dataCol(), true ) );
        appendChild( new Word( _document, this, _document->dataName(),
            _document->dataLine(), _document->dataCol(), txt, itemNumber >= 9 ) );
        appendChild( new Punctuation( _document, this, _document->dataName(),
            _document->dataLine(), _document->dataCol(), _document->olClose( nestLevel ), true ) );
    }
    appendChild( new Lm( _document, this, _document->dataName(), _document->dataLine(),
        _document->dataCol(), indent + 3 ) );
    while( tok != Lexer::END && !( tok == Lexer::TAG && lexer->tagId() == Lexer::EUSERDOC ) ) {
        if( parseInline( lexer, tok ) ) {
            if( lexer->tagId() == Lexer::LI )
                break;
            else if( lexer->tagId() == Lexer::LP ) {
                Element* elt( new P( _document, this, _document->dataName(),
                    _document->dataLine(), _document->dataCol() ) );
                appendChild( elt );
                tok = elt->parse( lexer );
            }
            else if( parseBlock( lexer, tok ) )
                break;
        }
    }
    return tok;
}

