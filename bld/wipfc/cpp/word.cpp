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
* Description:  A WORD element
*
****************************************************************************/


#include <cwctype>
#include "word.hpp"
#include "document.hpp"
#include "gdword.hpp"
#include "ipfbuff.hpp"

Lexer::Token Word::parse( Lexer* lexer )
{
    std::wstring txt( lexer->text() );  //get text from lexer
    Lexer::Token tok( document->getNextToken() );
    while( tok != Lexer::END && !( tok == Lexer::TAG && lexer->tagId() == Lexer::EUSERDOC) ) {
        if( tok == Lexer::WORD )
            txt += lexer->text();       //part of a compound ...-word-entity-word-...
        else if( tok == Lexer::ENTITY ) {
            const std::wstring* exp( document->nameit( lexer->text() ) );
            if( exp ) {
                std::wstring* name( document->prepNameitName( lexer->text() ) );
                IpfBuffer* buffer( new IpfBuffer( name, document->dataLine(), document->dataCol(), *exp ) );
                document->pushInput( buffer );
            }
            else {
                try {
                    wchar_t entity( document->entity( lexer->text() ) );
                    if ( std::iswpunct( entity ) )
                        break;
                    else
                        txt += entity;
                }
                catch( Class2Error& e ) {
                    document->printError( e.code );
                    break;
                }
            }
        }
        else
            break;
        tok = document->getNextToken();
    }
    if( whiteSpace != Tag::SPACES && document->autoSpacing() ) {
        Lexer::Token t( document->lastToken() );
        if( t == Lexer::WORD || t == Lexer::ENTITY || t == Lexer::PUNCTUATION ) {
            document->toggleAutoSpacing();
            document->lastText()->setToggleSpacing();
        }
    }
    GlobalDictionaryWord* word( new GlobalDictionaryWord( txt ) );
    text = document->addWord( word );   //insert into global dictionary
    document->setLastPrintable( Lexer::WORD, this );
    return tok;
}

