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
* Description:  Process sl/esl tags
*
*   :sl / :esl
*       compact (else blank line between items)
*       verycompact (no extra lines at all; a wipfc extension)
*   If nested, indent 4 spaces
*
****************************************************************************/

#include "sl.hpp"
#include "brcmd.hpp"
#include "dl.hpp"
#include "cell.hpp"
#include "document.hpp"
#include "errors.hpp"
#include "lexer.hpp"
#include "lp.hpp"
#include "parml.hpp"
#include "ol.hpp"
#include "p.hpp"
#include "ul.hpp"

Lexer::Token Sl::parse( Lexer* lexer )
{
    Lexer::Token tok( parseAttributes( lexer ) );
    unsigned int itemCount( 0 );
    bool needLine( true );
    while( tok != Lexer::END && !( tok == Lexer::TAG && lexer->tagId() == Lexer::EUSERDOC ) ) {
        if( parseInline( lexer, tok ) ) {
            switch( lexer->tagId() ) {
            case Lexer::DL:
                {
                    Element* elt( new Dl( document, this, document->dataName(),
                        document->dataLine(), document->dataCol(), nestLevel + 1,
                        indent == 1 ? 4 : indent + 4 ) );
                    appendChild( elt );
                    tok = elt->parse( lexer );
                    needLine = true;
                }
                break;
            case Lexer::OL:
                {
                    Element* elt( new Ol( document, this, document->dataName(),
                        document->dataLine(), document->dataCol(),
                        nestLevel + 1, indent == 1 ? 4 : indent + 4 ) );
                    appendChild( elt );
                    tok = elt->parse( lexer );
                    needLine = true;
                }
                break;
            case Lexer::LI:
                {
                    Element* elt( new SlLi( document, this, document->dataName(),
                        document->dataLine(), document->dataCol(),
                        itemCount++, nestLevel, indent, veryCompact ||
                        ( compact && !needLine ) ) );
                    appendChild( elt );
                    tok = elt->parse( lexer );
                    needLine = false;
                }
                break;
            case Lexer::LP:
                {
                    Element* elt( new Lp( document, this, document->dataName(),
                        document->dataLine(), document->dataCol(), indent ) );
                    appendChild( elt );
                    tok = elt->parse( lexer );
                }
                break;
            case Lexer::PARML:
                {
                    Element* elt( new Parml( document, this, document->dataName(),
                        document->dataLine(), document->dataCol(), nestLevel + 1,
                        indent == 1 ? 4 : indent + 4 ) );
                    appendChild( elt );
                    tok = elt->parse( lexer );
                    needLine = true;
                }
                break;
            case Lexer::SL:
                {
                    Element* elt( new Sl( document, this, document->dataName(),
                        document->dataLine(), document->dataCol(),
                        nestLevel + 1, indent == 1 ? 4 : indent + 4 ) );
                    appendChild( elt );
                    tok = elt->parse( lexer );
                    needLine = true;
                }
                break;
            case Lexer::ESL:
                {
                    Element* elt( new ESl( document, this, document->dataName(),
                        document->dataLine(), document->dataCol() ) );
                    appendChild( elt );
                    tok = elt->parse( lexer );
                    if( !nestLevel )
                        appendChild( new BrCmd( document, this, document->dataName(),
                            document->dataLine(), document->dataCol() ) );
                    return tok;
                }
            case Lexer::UL:
                {
                    Element* elt( new Ul( document, this, document->dataName(),
                        document->dataLine(), document->dataCol(),
                        nestLevel + 1, indent == 1 ? 4 : indent + 4 ) );
                    appendChild( elt );
                    tok = elt->parse( lexer );
                    needLine = true;
                }
                break;
            default:
                document->printError( ERR1_NOENDLIST );
                return tok;
            }
        }
    }
    return tok;
}
/***************************************************************************/
Lexer::Token Sl::parseAttributes( Lexer* lexer )
{
    Lexer::Token tok( document->getNextToken() );
    lexer = lexer;
    while( tok != Lexer::TAGEND ) {
        if( tok == Lexer::ATTRIBUTE )
            document->printError( ERR1_ATTRNOTDEF );
        else if( tok == Lexer::FLAG ) {
            if( lexer->text() == L"compact" )
                compact = true;
            else if( lexer->text() == L"verycompact" )
                veryCompact = true;
            else
                document->printError( ERR1_ATTRNOTDEF );
        }
        else if( tok == Lexer::ERROR_TAG )
            throw FatalError( ERR_SYNTAX );
        else if( tok == Lexer::END )
            throw FatalError( ERR_EOF );
        else
            document->printError( ERR1_TAGSYNTAX );
        tok = document->getNextToken();
    }
    return document->getNextToken();    //consume TAGEND
}
/***************************************************************************/
void ESl::buildText( Cell* cell )
{
    cell->addByte( 0xFF );  //esc
    cell->addByte( 0x03 );  //size
    cell->addByte( 0x02 );  //set left margin
    cell->addByte( 1 );
    if( cell->textFull() )
        printError( ERR1_LARGEPAGE );
}
/***************************************************************************/
Lexer::Token SlLi::parse( Lexer* lexer )
{
    Lexer::Token tok( parseAttributes( lexer ) );
    while( tok != Lexer::END && !( tok == Lexer::TAG && lexer->tagId() == Lexer::EUSERDOC ) ) {
        if( parseInline( lexer, tok ) ) {
            if( lexer->tagId() == Lexer::LI )
                break;
            else if( lexer->tagId() == Lexer::LP ) {
                Element* elt( new P( document, this, document->dataName(),
                    document->dataLine(), document->dataCol() ) );
                appendChild( elt );
                tok = elt->parse( lexer );
            }
            else if( parseBlock( lexer, tok ) )
                break;
        }
    }
    return tok;
}
/***************************************************************************/
void SlLi::buildText( Cell* cell )
{
    cell->addByte( 0xFF );  //esc
    cell->addByte( 0x03 );  //size
    cell->addByte( 0x02 );  //set left margin
    cell->addByte( indent );
    if( compact )
        cell->addByte( 0xFD );
    else
        cell->addByte( 0xFA );
    if( cell->textFull() )
        printError( ERR1_LARGEPAGE );
}

