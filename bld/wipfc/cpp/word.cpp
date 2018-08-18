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
* Description:  A TextWord element
*
****************************************************************************/


#include "wipfc.hpp"
#include <cwctype>
#include "word.hpp"
#include "document.hpp"
#include "gdword.hpp"
#include "ipfbuff.hpp"

Lexer::Token TextWord::parse( Lexer* lexer )
{
    std::wstring text( lexer->text() );  //get text from lexer
    Lexer::Token tok( _document->getNextToken() );
    while( tok != Lexer::END && !( tok == Lexer::TAG && lexer->tagId() == Lexer::EUSERDOC) ) {
        if( tok == Lexer::WORD ) {
            text += lexer->text();       //part of a compound ...-word-entity-word-...
        } else if( tok == Lexer::ENTITY ) {
            const std::wstring* exp( _document->nameit( lexer->text() ) );
            if( exp ) {
                std::wstring* name( _document->prepNameitName( lexer->text() ) );
                IpfBuffer* buffer( new IpfBuffer( name, _document->dataLine(), _document->dataCol(), *exp ) );
                _document->pushInput( buffer );
            } else {
                try {
                    wchar_t entityChar( _document->entityChar( lexer->text() ) );
                    if( std::iswpunct( entityChar ) )
                        break;
                    text += entityChar;
                }
                catch( Class2Error& e ) {
                    _document->printError( e._code );
                    break;
                }
            }
        } else {
            break;
        }
        tok = _document->getNextToken();
    }
    if( _whiteSpace != Tag::SPACES && _document->autoSpacing() ) {
        Lexer::Token t( _document->lastToken() );
        if( t == Lexer::WORD || t == Lexer::ENTITY || t == Lexer::PUNCTUATION ) {
            _document->toggleAutoSpacing();
            _document->lastText()->setToggleSpacing();
        }
    }
    _text = _document->addTextToGD( new GlobalDictionaryWord( text ) );   //insert into global dictionary
    _document->setLastPrintable( Lexer::WORD, this );
    return tok;
}

