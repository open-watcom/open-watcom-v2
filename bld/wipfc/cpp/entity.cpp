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
* Description:  An ENTITY element
*
****************************************************************************/


#include "wipfc.hpp"
#include <cwctype>
#include "entity.hpp"
#include "document.hpp"
#include "ipfbuff.hpp"
#include "lexer.hpp"

Lexer::Token Entity::parse( Lexer* lexer )
{
    Lexer::Token tok;
    const std::wstring* nameitTxt( _document->nameit( lexer->text() ) ); //lookup nameit
    if( nameitTxt ) {
        std::wstring* name( _document->prepNameitName( lexer->text() ) );
        IpfBuffer* buffer( new IpfBuffer( name, _document->dataLine(), _document->dataCol(), *nameitTxt ) );
        _document->pushInput( buffer );
        return _document->getNextToken();
    }
    try {
        wchar_t entity( _document->entity( lexer->text() ) );    //lookup entity
        std::wstring txt(1, entity );
        tok = _document->getNextToken();
        if( !std::iswpunct( entity ) ) {
            while( tok != Lexer::END && !( tok == Lexer::TAG && lexer->tagId() == Lexer::EUSERDOC) ) {
                if( tok == Lexer::WORD )
                    txt += lexer->text();       //part of a compound ...-word-entity-word-...
                else if( tok == Lexer::ENTITY ) {
                    entity = _document->entity( lexer->text() );
                    if ( std::iswpunct( entity ) )
                        break;
                    else
                        txt+= entity;
                }
                else
                    break;
                tok = _document->getNextToken();
            }
        }
        if( whiteSpace != Tag::SPACES && _document->autoSpacing() ) {
            Lexer::Token t( _document->lastToken() );
            if( t == Lexer::WORD || t == Lexer::ENTITY || t == Lexer::PUNCTUATION ) {
                _document->toggleAutoSpacing();
                _document->lastText()->setToggleSpacing();
            }
        }
        text = _document->addWord( new GlobalDictionaryWord( txt ) );   //insert into global dictionary
    }
    catch( Class2Error& e ) {
        _document->printError( e.code );
        tok = _document->getNextToken();
    }
    _document->setLastPrintable( Lexer::ENTITY, this );
    return tok;
}

