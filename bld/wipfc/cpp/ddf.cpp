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
* Description:  Process acviewport tag
*
*   :ddf
*       res=[0-9]+
*
****************************************************************************/


#include "wipfc.hpp"
#include "ddf.hpp"
#include "cell.hpp"
#include "document.hpp"
#include "errors.hpp"
#include "util.hpp"
#include "xref.hpp"

Lexer::Token Ddf::parse( Lexer* lexer )
{
    Lexer::Token tok( parseAttributes( lexer ) );
    if( !_res )
        printError( ERR1_NODDFRES );
    return tok;
}
/***************************************************************************/
Lexer::Token Ddf::parseAttributes( Lexer* lexer )
{
    Lexer::Token tok;

    while( (tok = _document->getNextToken()) != Lexer::TAGEND ) {
        if( tok == Lexer::ATTRIBUTE ) {
            std::wstring key;
            std::wstring value;
            splitAttribute( lexer->text(), key, value );
            if( key == L"res" ) {
                _res = static_cast< word >( std::wcstoul( value.c_str(), 0, 10 ) );
                if( _res < 1 || _res > 64000 ) {
                    _document->printError( ERR2_VALUE );
                }
            } else {
                _document->printError( ERR1_ATTRNOTDEF );
            }
        } else if( tok == Lexer::FLAG ) {
            _document->printError( ERR1_ATTRNOTDEF );
        } else if( tok == Lexer::ERROR_TAG ) {
            throw FatalError( ERR_SYNTAX );
        } else if( tok == Lexer::END ) {
            throw FatalError( ERR_EOF );
        }
    }
    return _document->getNextToken(); //consume TAGEND;
}
/***************************************************************************/
void Ddf::buildText( Cell* cell )
{
    try {
//        word tocIndex( _document->tocIndexByRes( _res ) );
        XRef xref( _fileName, _row );
        _document->addXRef( _res, xref );
        cell->addByte( Cell::ESCAPE );  //ESC
        cell->addByte( 0x04 );          //size
        cell->addByte( 0x20 );          //ddf
        cell->add( _res );
        if( cell->textFull() ) {
            printError( ERR1_LARGEPAGE );
        }
    }
    catch( Class1Error& e ) {
        printError( e._code );
    }
}


