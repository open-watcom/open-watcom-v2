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
* Description:  Process title tag
*   :title
*       Maximum 47 characters
*
****************************************************************************/

#include <cstdlib>
#include <string>
#include "title.hpp"
#include "lexer.hpp"
#include "document.hpp"

Lexer::Token Title::parse( Lexer* lexer, IpfHeader* hdr )
{
    Lexer::Token tok( document->getNextToken() );
    while ( tok != Lexer::TAGEND ) {
        if( tok == Lexer::ATTRIBUTE )
            document->printError( ERR1_ATTRNOTDEF );
        else if( tok == Lexer::FLAG )
            document->printError( ERR1_ATTRNOTDEF );
        else if( tok == Lexer::END )
            throw FatalError( ERR_EOF );
        else
            document->printError( ERR1_TAGSYNTAX );
        tok = document->getNextToken();
    }
    std::wstring txt;
    unsigned int startLine( document->dataLine() );
    tok = document->getNextToken();
    while(  document->dataLine() == startLine ) {
        if( tok == Lexer::WHITESPACE ||
            tok == Lexer::WORD ||
            tok == Lexer::PUNCTUATION ) {
            txt += lexer->text();
        }
        else if( tok == Lexer::ENTITY ) {
            const std::wstring* exp( document->nameit( lexer->text() ) );
            if( exp )
                txt += *exp;
            else {
                try {
                    wchar_t ch( document->entity( lexer->text() ) );
                    txt += ch;
                }
                catch( Class2Error& e ) {
                    document->printError( e.code );
                }
            }
        }
        else if( tok == Lexer::END )
            throw FatalError( ERR_EOF );
        else
            break;
        tok = document->getNextToken();
    }
    if( txt.size() > 47 )
        document->printError( ERR2_TEXTTOOLONG );
    size_t index1 = 0;
    size_t index2 = 0;
    char *title( hdr->title );
    while( index1 < txt.size() && index2 < 47 ) {
        index2 += std::wctomb( title + index2, txt[ index1 ] );
        ++index1 ;
    }
    return tok;
}

