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
* Description:  Process cgraphic/ecgraphic tags
*
*   :cgraphic / :ecgraphic
*   Honors white space (as spaces), including '\n' (as .br)
*   Rendered monospaced, indented 2 spaces left
*   Cannot nest
****************************************************************************/

#include "cgraphic.hpp"
#include "brcmd.hpp"
#include "cell.hpp"
#include "document.hpp"
#include "p.hpp"
#include "page.hpp"
#include "util.hpp"

Lexer::Token CGraphic::parse( Lexer* lexer )
{
    Lexer::Token tok( parseAttributes( lexer ) );
    if( tok == Lexer::WHITESPACE && lexer->text()[0] == L'\n' )
        tok = document->getNextToken(); //consume '\n' if just after tag end
    while( tok != Lexer::END && !( tok == Lexer::TAG && lexer->tagId() == Lexer::EUSERDOC)) {
        if( parseInline( lexer, tok ) ) {
            if( lexer->tagId() == Lexer::ECGRAPHIC )
                break;
            else
                parseCleanup( lexer, tok );
        }
    }
    return tok;
}
/*****************************************************************************/
void CGraphic::buildText( Cell* cell )
{
    cell->addByte( 0xFF );  //esc
    cell->addByte( 0x02 );  //size
    cell->addByte( 0x0B );  //begin monospaced
    if( cell->textFull() )
        printError( ERR1_LARGEPAGE );
}
/*****************************************************************************/
void ECGraphic::buildText( Cell* cell )
{
    cell->addByte( 0xFF );  //esc
    cell->addByte( 0x02 );  //size
    cell->addByte( 0x0C );  //end monospaced
    if( cell->textFull() )
        printError( ERR1_LARGEPAGE );
}

