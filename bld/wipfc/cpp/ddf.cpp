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
* Description:  Process acviewport tag
*
*   :ddf
*       res=[0-9]+
*
****************************************************************************/

#include "ddf.hpp"
#include "cell.hpp"
#include "document.hpp"
#include "errors.hpp"
#include "util.hpp"
#include "xref.hpp"

Lexer::Token Ddf::parse( Lexer* lexer )
{
    Lexer::Token tok( parseAttributes( lexer ) );
    if( !res )
        printError( ERR1_NODDFRES );
    return tok;
}
/***************************************************************************/
Lexer::Token Ddf::parseAttributes( Lexer* lexer )
{
    Lexer::Token tok( document->getNextToken() );
    while( tok != Lexer::TAGEND ) {
        if( tok == Lexer::ATTRIBUTE ) {
            std::wstring key;
            std::wstring value;
            splitAttribute( lexer->text(), key, value );
            if( key == L"res" ) {
                res = static_cast< STD1::uint16_t >( std::wcstoul( value.c_str(), 0, 10 ) );
                if( res < 1 || res > 64000 )
                    document->printError( ERR2_VALUE );
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
        tok = document->getNextToken();
    }
    return document->getNextToken(); //consume TAGEND;
}
/***************************************************************************/
void Ddf::buildText( Cell* cell )
{
    try {
        STD1::uint16_t tocIndex( document->tocIndexByRes( res ) );
        XRef xref( fileName, row );
        document->addXRef( res, xref );
        cell->addByte( 0xFF );  //ESC
        cell->addByte( 0x04 );  //size
        cell->addByte( 0x20 );  //ddf
        cell->addByte( static_cast< STD1::uint8_t >( res ) );
        cell->addByte( static_cast< STD1::uint8_t >( res >> 8 ) );
        if( cell->textFull() )
            printError( ERR1_LARGEPAGE );
    }
    catch( Class1Error& e ) {
        printError( e.code );
    }
}


