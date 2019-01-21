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
* Description:  A .ce command
*
****************************************************************************/


#include "wipfc.hpp"
#include "cecmd.hpp"
#include "cell.hpp"
#include "brcmd.hpp"
#include "entity.hpp"
#include "document.hpp"
#include "ipfbuff.hpp"
#include "punct.hpp"
#include "whtspc.hpp"
#include "word.hpp"

void CeCmd::parseCommand( Lexer* lexer )
{
    std::wstring* fname( new std::wstring() );
    prepBufferName( fname, *( _document->dataName() ) );
    fname = _document->addFileName( fname );
    bool oldBlockParsing( _document->blockParsing() );
    IpfBuffer* buffer( new IpfBuffer( fname, _document->dataLine(), _document->dataCol(), lexer->text() ) );
    _document->pushInput( buffer );
    _document->setBlockParsing( true );
    Lexer::Token tok( _document->getNextToken() );
    while( tok != Lexer::END ) {
        if( tok == Lexer::WHITESPACE ) {
            WhiteSpace* ws( new WhiteSpace( _document, this,
                _document->dataName(), _document->dataLine(), _document->dataCol(), _whiteSpace ) );
            appendChild( ws );
            tok = ws->parse( lexer );
        } else if( tok == Lexer::WORD ) {
            TextWord* w( new TextWord( _document, this,
                    _document->dataName(), _document->dataLine(), _document->dataCol() ) );
            appendChild( w );
            tok = w->parse( lexer );
        } else if( tok == Lexer::ENTITY ) {
            Entity* entity( new Entity( _document, this,
                    _document->dataName(), _document->dataLine(), _document->dataCol() ) );
            appendChild( entity );
            tok = entity->parse( lexer );
        } else if( tok == Lexer::PUNCTUATION ) {
            Punctuation* punct( new Punctuation( _document, this,
                    _document->dataName(), _document->dataLine(), _document->dataCol() ) );
            appendChild( punct );
            tok = punct->parse( lexer );
        } else {
            _document->printError( ERR1_TAGCONTEXT );
            tok = _document->getNextToken();
        }
    }
    appendChild( new BrCmd( _document, this,
        _document->dataName(), _document->dataLine(), _document->dataCol() ) );
    _document->setBlockParsing( oldBlockParsing );
    _document->popInput();
}
/*****************************************************************************/
void CeCmd::buildText( Cell* cell )
{
    cell->addByte( Cell::CENTER );
    if( cell->textFull() ) {
        printError( ERR1_LARGEPAGE );
    }
}
/*****************************************************************************/
void CeCmd::prepBufferName( std::wstring* buffer, const std::wstring& fname )
{
    buffer->assign( L"Centered text from " );
    buffer->append( fname );
}

