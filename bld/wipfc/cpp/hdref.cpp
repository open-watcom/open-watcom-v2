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
* Description: hdref tag
*
*   :hdref
*       res=[0-9]+
*       refid=[A-Za-z0-9]+
*
****************************************************************************/

#include "hdref.hpp"
#include "cell.hpp"
#include "document.hpp"
#include "errors.hpp"
#include "ipfbuff.hpp"
#include "lexer.hpp"
#include "page.hpp"
#include "util.hpp"

Lexer::Token Hdref::parse( Lexer* lexer )
{
    std::wstring refid;
    std::wstring res;
    Lexer::Token tok( document->getNextToken() );
    while( tok != Lexer::TAGEND ) {
        //parse attributes
        if( tok == Lexer::ATTRIBUTE ) {
            std::wstring key;
            std::wstring value;
            splitAttribute( lexer->text(), key, value );
            if( key == L"res" )
                res = value;
            else if( key == L"refid" )
                refid = value;
            else
                document->printError( ERR1_ATTRNOTDEF );
        }
        else if( tok == Lexer::FLAG )
            document->printError( ERR1_ATTRNOTDEF );
        else if( tok == Lexer::ERROR_TAG )
            throw FatalError( ERR_SYNTAX );
        else if( tok == Lexer::END )
            throw FatalError( ERR_EOF );
        else
            document->printError( ERR1_TAGSYNTAX );
        tok = document->getNextToken();
    }
    std::wstring temp( L":link reftype=hd" );
    if( !refid.empty() ) {
        temp += L" refid=";
        temp += refid;
    }
    if( !res.empty() ) {
        temp += L" res=";
        temp += res;
    }
    temp += L'.';
    temp += document->reference();
    temp += L":elink.";
    if( !refid.empty() || !res.empty() ) {
        std::wstring* fname( new std::wstring() );
        prepBufferName( fname, *( document->dataName() ) );
        fname = document->addFileName( fname );
        document->pushInput( new IpfBuffer( fname, document->dataLine(),
            document->dataCol(), temp ) );
        bool oldBlockParsing( document->blockParsing() );
        document->setBlockParsing( true );
        tok = document->getNextToken(); //first token from buffer
        while( tok != Lexer::END ) {
            if( parseInline( lexer, tok ) )
                parseCleanup( lexer, tok );
        }
        document->setBlockParsing( oldBlockParsing );
        document->popInput();
    }
    return document->getNextToken();    //next token from stream
}
/*****************************************************************************/
void Hdref::prepBufferName( std::wstring* buffer, const std::wstring& fname )
{
    buffer->assign( L"hdref text from " );
    buffer->append( fname );
}

