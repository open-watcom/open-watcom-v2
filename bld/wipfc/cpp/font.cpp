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
* Description: font tag
*
*   :font
*       facename=default|Courier|Helv|Tms Rmn|System Monospaced|System Proportional
*       size=HxW (in points)
*       codepage= [0-9]*
*   Resets to default at each new 'page'
*   facename=default and/or size=0x0 resets to default font
*
****************************************************************************/


#include "wipfc.hpp"
#include <cstdlib>
#include "font.hpp"
#include "fnt.hpp"
#include "cell.hpp"
#include "document.hpp"
#include "errors.hpp"
#include "lexer.hpp"
#include "page.hpp"
#include "util.hpp"

Lexer::Token Font::parse( Lexer* lexer )
{
    FontEntry fnt;
    bool isDefault( false );
    Lexer::Token tok;

    while( (tok = _document->getNextToken()) != Lexer::TAGEND ) {
        //parse attributes
        if( tok == Lexer::ATTRIBUTE ) {
            std::wstring key;
            std::wstring value;
            splitAttribute( lexer->text(), key, value );
            if( key == L"facename" ) {
                if( value == L"default" ) {
                    _index = 0;
                    fnt.setHeight( 0 );
                    fnt.setWidth( 0 );
                    isDefault = true;
                } else {
                    fnt.setFaceName( value );
                }
            } else if( key == L"size" ) {
                wchar_t *end;
                word height = static_cast< word >( std::wcstoul( value.c_str(), &end, 10 ) );
                ++end;
                word width = static_cast< word >( std::wcstoul( end, &end, 10 ) );
                if( height == 0 || width == 0 ) {
                    _index = 0;
                    height = 0;
                    width = 0;
                    isDefault = true;
                }
                fnt.setWidth( width );
                fnt.setHeight( height );
            } else if( key == L"codepage" ) {
                fnt.setCodePage( static_cast< word >( std::wcstoul( value.c_str(), 0, 10 ) ) );
            } else {
                _document->printError( ERR1_ATTRNOTDEF );
            }
        } else if( tok == Lexer::FLAG ) {
            _document->printError( ERR1_ATTRNOTDEF );
        } else if( tok == Lexer::ERROR_TAG ) {
            throw FatalError( ERR_SYNTAX );
        } else if( tok == Lexer::END ) {
            throw FatalError( ERR_EOF );
        } else {
            _document->printError( ERR1_TAGSYNTAX );
        }
    }
    if( !isDefault ) {
        try {
            _index = static_cast< byte >( _document->addFont( fnt ) );
        }
        catch( Class2Error& e ) {
            _document->printError( e._code );
        }
    }
    return _document->getNextToken();
}

/***************************************************************************/
void Font::buildText( Cell* cell )
{
    cell->addByte( Cell::ESCAPE );  //esc
    cell->addByte( 0x03 );          //size
    cell->addByte( 0x19 );          //set font
    cell->add( _index );
    if( cell->textFull() ) {
        printError( ERR1_LARGEPAGE );
    }
}

