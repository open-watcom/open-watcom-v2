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
* Description:  A .ce command
*
****************************************************************************/

#include "cecmd.hpp"
#include "cell.hpp"
#include "brcmd.hpp"
#include "entity.hpp"
#include "document.hpp"
#include "ipfbuff.hpp"
#include "punct.hpp"
#include "whtspc.hpp"
#include "word.hpp"

Lexer::Token CeCmd::parse( Lexer* lexer )
{
    std::wstring temp;
    std::wstring* fname( new std::wstring() );
    prepBufferName( fname, *( document->dataName() ) );
    fname = document->addFileName( fname );
    bool oldBlockParsing( document->blockParsing() );
    IpfBuffer* buffer( new IpfBuffer( fname, document->dataLine(), document->dataCol(), lexer->text() ) );
    document->pushInput( buffer );
    document->setBlockParsing( true );
    Lexer::Token tok( document->getNextToken() );
    while( tok != Lexer::END ) {
        if( tok == Lexer::WHITESPACE ) {
            WhiteSpace* ws( new WhiteSpace( document, this,
                document->dataName(), document->dataLine(), document->dataCol(), whiteSpace ) );
            appendChild( ws );
            tok = ws->parse( lexer );
        }
        else if( tok == Lexer::WORD ) {
            Word* word( new Word( document, this,
                    document->dataName(), document->dataLine(), document->dataCol() ) );
            appendChild( word );
            tok = word->parse( lexer );
        }
        else if( tok == Lexer::ENTITY ) {
            Entity* entity( new Entity( document, this,
                    document->dataName(), document->dataLine(), document->dataCol() ) );
            appendChild( entity );
            tok = entity->parse( lexer );
        }
        else if (tok == Lexer::PUNCTUATION ) {
            Punctuation* punct( new Punctuation( document, this,
                    document->dataName(), document->dataLine(), document->dataCol() ) );
            appendChild( punct );
            tok = punct->parse( lexer );
        }
        else if( tok != Lexer::END ) {
            document->printError( ERR1_TAGCONTEXT );
            tok = document->getNextToken();
        }
    }
    appendChild( new BrCmd( document, this,
        document->dataName(), document->dataLine(), document->dataCol() ) );
    document->setBlockParsing( oldBlockParsing );
    document->popInput();
    return document->getNextToken();
}
/*****************************************************************************/
void CeCmd::buildText( Cell* cell )
{
    cell->addByte( 0xFB );
    if( cell->textFull() )
        printError( ERR1_LARGEPAGE );
}
/*****************************************************************************/
void CeCmd::prepBufferName( std::wstring* buffer, const std::wstring& fname )
{
    buffer->assign( L"Centered text from " );
    buffer->append( fname );
}

