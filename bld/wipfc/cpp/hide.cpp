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
* Description:  Process hide/ehide tags
*
*   :hide / :ehide
*       key='[0-9][a-z][A-Z]*'+
*   Multiple single-quoted keys separated by '+'
*   Cannot nest
*   Cannot contain :h with res attribute (link target)
*
****************************************************************************/

#include <vector>
#include "hide.hpp"
#include "cell.hpp"
#include "document.hpp"
#include "errors.hpp"
#include "util.hpp"

bool Hide::hide( false );

Hide::Hide( Document* d, Element *p, const std::wstring* f, unsigned int r,
          unsigned int c ) : Element( d, p, f, r, c )
{
    if( hide )
        d->printError( ERR2_NEST );
    else
        hide = true;
}
/***************************************************************************/
Lexer::Token Hide::parse( Lexer* lexer )
{
    Lexer::Token tok( document->getNextToken() );
    while( tok != Lexer::TAGEND ) {
        //parse attributes
        if( tok == Lexer::ATTRIBUTE ) {
            std::wstring key;
            std::wstring value;
            splitAttribute( lexer->text(), key, value );
            if( key == L"key" ) {
                //kill of all other "'"
                std::wstring::size_type index( value.find( L'\'', 0 ) );
                while( index != std::wstring::npos ) {
                    value.erase( index, 1 );
                    index = value.find( L'\'', index );
                }
                keyPhrase = value;
            }
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
    return document->getNextToken(); //consume TAGEND
}
/***************************************************************************/
//How are multiple pass-phrases encoded? Assuming + left in place...
void Hide::buildText( Cell* cell )
{
    std::string tmp;
    wtombstring( keyPhrase, tmp );
    size_t size( tmp.size() );
    if( size > 253 ) {
        tmp.erase( 253 );
        size = 253;
    }
    std::vector< STD1::uint8_t > esc;
    esc.reserve( size + 3 );
    esc.push_back( 0xFF );  //esc
    esc.push_back( 0x02 );  //size
    esc.push_back( 0x17 );  //begin hide
    for( unsigned int count1 = 0; count1 < size; count1++ )
        esc.push_back( static_cast< STD1::uint8_t >( tmp[ count1 ] ) );
    esc[1] = static_cast< STD1::uint8_t >( esc.size() - 1 );
    cell->addEsc( esc );
    if( cell->textFull() )
        printError( ERR1_LARGEPAGE );
}
/***************************************************************************/
EHide::EHide( Document* d, Element *p, const std::wstring* f, unsigned int r,
            unsigned int c ) : Element ( d, p, f, r, c )
{
    if( Hide::hiding() )
        Hide::clear();
    else
        d->printError( ERR2_NEST );
}
/***************************************************************************/
Lexer::Token EHide::parse( Lexer* lexer )
{
    Lexer::Token tok( document->getNextToken() );
    lexer = lexer;
    while( tok != Lexer::TAGEND ) {
        if( tok == Lexer::ATTRIBUTE )
            document->printError( ERR1_ATTRNOTDEF );
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
    return document->getNextToken();    //consume TAGEND
}
/***************************************************************************/
void EHide::buildText( Cell* cell )
{
    cell->addByte( 0xFF );  //esc
    cell->addByte( 0x02 );  //size
    cell->addByte( 0x18 );  //end hide
    if( cell->textFull() )
        printError( ERR1_LARGEPAGE );
}

