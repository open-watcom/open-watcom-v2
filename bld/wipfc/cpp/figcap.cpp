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
* Description:  Process figcap tag
*
*   :figcap
*   Must be a child of :fig
*   Text cannot contain tags or ';'
*   Insert blank line before contents
****************************************************************************/

#include "figcap.hpp"
#include "brcmd.hpp"
#include "cell.hpp"
#include "document.hpp"
#include "entity.hpp"
#include "punct.hpp"
#include "util.hpp"
#include "whtspc.hpp"
#include "word.hpp"

Lexer::Token Figcap::parse( Lexer* lexer )
{
    Lexer::Token tok( parseAttributes( lexer) );
    while( tok != Lexer::END ) {
        //may contain inline, not block
        if( tok == Lexer::WORD ) {
            Word* word( new Word( document, this, document->dataName(),
                document->dataLine(), document->dataCol() ) );
            appendChild( word );
            tok = word->parse( lexer );
        }
        else if( tok == Lexer::ENTITY ) {
            Entity* entity( new Entity( document, this, document->dataName(),
                document->dataLine(), document->dataCol() ) );
            appendChild( entity );
            tok = entity->parse( lexer );
        }
        else if( tok == Lexer::PUNCTUATION ) {
            Punctuation* punct( new Punctuation( document, this, document->dataName(),
                document->dataLine(), document->dataCol() ) );
            appendChild( punct );
            tok = punct->parse( lexer );
        }
        else if( tok == Lexer::WHITESPACE ) {
            WhiteSpace* ws( new WhiteSpace( document, this, document->dataName(),
                document->dataLine(), document->dataCol(), Tag::LITERAL ) );
            appendChild( ws );
            tok = ws->parse( lexer );
        }
        else if( tok == Lexer::COMMAND )
            tok = document->processCommand( lexer, this );
        else if( tok == Lexer::TAG ) {
            if( lexer->tagId() == Lexer::EUSERDOC || lexer->tagId() == Lexer::EFIG )
                break;
            else
                parseCleanup( lexer, tok );
        }
        else if( tok == Lexer::ERROR_TAG ) {
            document->printError( ERR1_TAGNOTDEF );
            tok = document->getNextToken();
        }
        else if( tok == Lexer::ERROR_ENTITY ) {
            document->printError( ERR1_TAGNOTDEF );
            tok = document->getNextToken();
        }
    }
    return tok;
}

