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
* Description:  Process hpn/ehpn tags
*
*   :hp1-:hp9 / :ehp1-:ehp9
*   Cannot nest
*   Note that the treatment here is an extension to allow nesting
*
****************************************************************************/

#include "hpn.hpp"
#include "cell.hpp"
#include "document.hpp"
#include "errors.hpp"
#include "util.hpp"


std::vector< STD1::uint8_t > Hpn::levelStack;

Hpn::Hpn( Document* d, Element *p, const std::wstring* f, unsigned int r,
          unsigned int c, unsigned int l ) : Element( d, p, f, r, c ),
          level( static_cast< STD1::uint8_t >( l ) ), previousLevel( 0 )
{
    if( !levelStack.empty() ) {
        previousLevel = levelStack[ levelStack.size() - 1 ];
        d->printError( ERR2_NEST );
    }
    levelStack.push_back( level );
}
/***************************************************************************/
Lexer::Token Hpn::parse( Lexer* lexer )
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
void Hpn::buildText( Cell* cell )
{
    if( previousLevel ) {
        //kill the previous level
        cell->addByte( 0xFF );      //esc
        cell->addByte( 0x03 );      //size
        if( previousLevel != 4 && previousLevel < 8 )
            cell->addByte( 0x04 );  //change style
        else
            cell->addByte( 0x0D );  //special text color
        cell->addByte( 0x00 );      //default
    }
    cell->addByte( 0xFF );          //esc
    cell->addByte( 0x03 );          //size
    if( level != 4 && level < 8 ) {
        cell->addByte( 0x04 );      //change style
        if( level < 4 )
            cell->addByte( static_cast< STD1::uint8_t >( level ) );
        else
            cell->addByte( static_cast< STD1::uint8_t >( level - 1) );
    }
    else {
        cell->addByte( 0x0D );      //special text color
        if( level == 4 )
            cell->addByte( 0x01 );
        else
            cell->addByte( static_cast< STD1::uint8_t >( level - 6) );
    }
    if( cell->textFull() )
        printError( ERR1_LARGEPAGE );
}
/***************************************************************************/
EHpn::EHpn( Document* d, Element *p, const std::wstring* f, unsigned int r,
            unsigned int c, unsigned int l ) : Element ( d, p, f, r, c ),
            level( static_cast< STD1::uint8_t >( l ) ), previousLevel( 0 )
{
    std::vector< STD1::uint8_t >& levelStack( Hpn::levels() );
    if( levelStack[ levelStack.size() - 1 ] != l )
        d->printError( ERR2_NEST );
    levelStack.pop_back();
    if( !levelStack.empty() ) {
        d->printError( ERR1_TAGCONTEXT );
        previousLevel = levelStack[ levelStack.size() - 1 ];
    }
}
/***************************************************************************/
Lexer::Token EHpn::parse( Lexer* lexer )
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
void EHpn::buildText( Cell* cell )
{
    cell->addByte( 0xFF );          //esc
    cell->addByte( 0x03 );          //size
    if( level != 4 && level < 8 )
        cell->addByte( 0x04 );      //change style
    else
        cell->addByte( 0x0D );      //special text color
    cell->addByte( 0x00 );          //default
    if( previousLevel ) {
        cell->addByte( 0xFF );      //esc
        cell->addByte( 0x03 );      //size
        if( previousLevel != 4 && previousLevel < 8 ) {
            cell->addByte( 0x04 );  //change style
            if( previousLevel < 4 )
                cell->addByte( static_cast< STD1::uint8_t >( previousLevel ) );
            else
                cell->addByte( static_cast< STD1::uint8_t >( previousLevel - 1) );
        }
        else {
            cell->addByte( 0x0D );  //special text color
            if( previousLevel == 4 )
                cell->addByte( 0x01 );
            else
                cell->addByte( static_cast< STD1::uint8_t >( previousLevel - 6) );
        }
    }
    if( cell->textFull() )
        printError( ERR1_LARGEPAGE );
}
