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
* Description:  Process lines/elines tags
*
*   :lines / :elines
*       align=left|center|right
*   Honor whitespace (becomes literal whitespace), including '\n' (as .br)
*
****************************************************************************/

#include "lines.hpp"
#include "brcmd.hpp"
#include "cell.hpp"
#include "document.hpp"
#include "p.hpp"
#include "page.hpp"
#include "util.hpp"

Lexer::Token Lines::parse( Lexer* lexer )
{
    Lexer::Token tok( parseAttributes( lexer ) );
    if( tok == Lexer::WHITESPACE && lexer->text()[0] == L'\n' )
        tok = document->getNextToken(); //consume '\n' if just after tag end
    while( tok != Lexer::END ) {
        if( parseInline( lexer, tok ) ) {
            if( lexer->tagId() == Lexer::ELINES )
                break;
            else
                parseCleanup( lexer, tok );
        }
    }
    return tok;
}
/*****************************************************************************/
Lexer::Token Lines::parseAttributes( Lexer* lexer )
{
    Lexer::Token tok( document->getNextToken() );
    while( tok != Lexer::TAGEND ) {
        if( tok == Lexer::ATTRIBUTE ) {
            std::wstring key;
            std::wstring value;
            splitAttribute( lexer->text(), key, value );
            if( key == L"align" ) {
                if( value == L"left" )
                    alignment = LEFT;
                else if( value == L"right" )
                    alignment = RIGHT;
                else if( value == L"center" )
                    alignment = CENTER;
                else
                    document->printError( ERR2_VALUE );
            }
            else
                document->printError( ERR1_ATTRNOTDEF );
        }
        else if( tok == Lexer::FLAG )
            document->printError( ERR1_ATTRNOTDEF );
        else if( tok == Lexer::ERROR_TAG )
            throw FatalError( ERR_SYNTAX );
        else if( tok == Lexer::END )
            throw FatalError( ERR_EOF );
        else
            document->printError( ERR1_TAGSYNTAX );
        tok = document->getNextToken();
    }
    return document->getNextToken();
}
/*****************************************************************************/
void Lines::buildText( Cell* cell )
{
    cell->addByte( 0xFC );  //toggle spacing
    cell->addByte( 0xFF );  //esc
    cell->addByte( 0x03 );  //size
    cell->addByte( 0x1A );  //begin lines sequence
    cell->addByte( alignment );
    cell->addByte( 0xFC );  //toggle spacing
    if( cell->textFull() )
        printError( ERR1_LARGEPAGE );
}
/*****************************************************************************/
void ELines::buildText( Cell* cell )
{
    cell->addByte( 0xFF );  //esc
    cell->addByte( 0x02 );  //size
    cell->addByte( 0x1B );  //end lines sequence
    if( cell->textFull() )
        printError( ERR1_LARGEPAGE );
}

