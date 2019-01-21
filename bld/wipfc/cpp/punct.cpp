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
* Description:  A PUNCTUATION element.
*
****************************************************************************/


#include "wipfc.hpp"
#include <cwctype>
#include "punct.hpp"
#include "document.hpp"

Lexer::Token Punctuation::parse( Lexer* lexer )
{
    //get text from lexer and insert into global dictionary
    _text = _document->addTextToGD( new GlobalDictionaryWord( lexer->text() ) );
    Lexer::Token tok( _document->getNextToken() );
    if( _whiteSpace != Tag::SPACES && _document->autoSpacing() ) {
        Lexer::Token t( _document->lastToken() );
        if( t == Lexer::WORD || t == Lexer::ENTITY || t == Lexer::PUNCTUATION ) {
            _document->toggleAutoSpacing();
            _document->lastText()->setToggleSpacing();
        }
    }
    _document->setLastPrintable( Lexer::PUNCTUATION, this );
    return tok;
}

