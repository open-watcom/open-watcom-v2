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
* Description:  Process artlink tag
*
*   :artlink / :eartlink
*       must follow :artwork on the next line, or in file referenced from :artwork
*       must contain one or more :link
*
****************************************************************************/

#include "artlink.hpp"
#include "document.hpp"
#include "lexer.hpp"
#include "link.hpp"

Lexer::Token Artlink::parse( Lexer* lexer )
{
    Lexer::Token tok( parseAttributes( lexer ) );
    while( tok != Lexer::END ) {
        if( tok == Lexer::TAG ) {
            if( lexer->tagId() == Lexer::LINK ) {
                Link* link( new Link( document, this, document->dataName(),
                            document->lexerLine(), document->lexerCol() ) );
                appendChild( link );
                link->setNoEndTag();
                link->setHypergraphic();
                tok = link->parse( lexer );
            }
            else if( lexer->tagId() == Lexer::EARTLINK || lexer->tagId() == Lexer::EUSERDOC )
                break;
            else
                parseCleanup( lexer, tok );
        }
        else {
            if( tok != Lexer::WHITESPACE )
                printError( ERR1_TAGCONTEXT );
            tok = document->getNextToken();
        }
    }
    return tok;
}

