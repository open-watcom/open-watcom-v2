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
* Description:  Process isyn tag
*
*   :isyn
*       root= (single word)
*   Followed by space separated list of synonyms
*
****************************************************************************/

#include <cstdlib>
#include "isyn.hpp"
#include "document.hpp"
#include "errors.hpp"
#include "lexer.hpp"
#include "util.hpp"

Lexer::Token ISyn::parse( Lexer* lexer )
{
    Lexer::Token tok( document->getNextToken() );
    while( tok != Lexer::TAGEND ) {
        if( tok == Lexer::ATTRIBUTE ) {
            std::wstring key;
            std::wstring value;
            splitAttribute( lexer->text(), key, value );
            if( key == L"root" ) {
                root = value;
            }
            else
                document->printError( ERR1_ATTRNOTDEF );
        }
        else if( tok == Lexer::FLAG ) {
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
    tok = document->getNextToken(); //consume TAGEND
    unsigned int currentLine = document->dataLine();
    while( tok != Lexer::END && !( tok == Lexer::TAG && lexer->tagId() == Lexer::EUSERDOC)) {
        if( tok == Lexer::WORD ) {
            char buffer[ 256 ];
            size_t length( std::wcstombs( buffer, lexer->text().c_str(), sizeof(buffer) / sizeof(char) ) );
            if( length == static_cast< size_t >( -1 ) )
                throw FatalError( ERR_T_CONV );
            std::string txt( buffer );
            syn->add( txt );
        }
        else if( tok == Lexer::WHITESPACE )
            tok = document->getNextToken();
        else
            break;
        if( document->dataLine() > currentLine )
            break;
    }
    try {
        document->addSynonym( root, syn.get() );
    }
    catch( Class3Error& e ) {
        document->printError( e.code );
        }
    return tok;
}

