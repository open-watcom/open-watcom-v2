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
* Description:  A WHITESPACE element
* Note that only single spaces are elided; all others are significant
*
****************************************************************************/

#include "whtspc.hpp"
#include "cell.hpp"
#include "document.hpp"

WhiteSpace::WhiteSpace( Document* d, Element* p, const std::wstring* f, unsigned int r,
    unsigned int c, const std::wstring& tx, Tag::WsHandling w, bool ts ) :
    Text( d, p, f, r, c, w, ts )
{
    if( tx[0] != L'\n' )
        spaces = static_cast< unsigned char >( tx.size() );
    else
        spaces = 0;
    if( w == Tag::SPACES ) {
        GlobalDictionaryWord* word( new GlobalDictionaryWord( tx ) );
        text = document->addWord( word );   //insert into global dictionary
    }
}
/***************************************************************************/
Lexer::Token WhiteSpace::parse( Lexer* lexer )
{
    if( lexer->text()[0] != L'\n' ) {
        spaces = static_cast< unsigned char >( lexer->text().size() ); //number of spaces
        if( whiteSpace == Tag::SPACES ) {
            GlobalDictionaryWord* word( new GlobalDictionaryWord( lexer->text() ) );
            text = document->addWord( word );   //insert into global dictionary
        }
    }
    if( whiteSpace != Tag::SPACES && !document->autoSpacing() ) {
        document->toggleAutoSpacing();
        Lexer::Token t( document->lastToken() );
        if( t == Lexer::WORD || t == Lexer::ENTITY || t == Lexer::PUNCTUATION )
            document->lastText()->setToggleSpacing();
    }
    document->setLastPrintable( Lexer::WHITESPACE, this );
    return document->getNextToken();
}
/***************************************************************************/
void WhiteSpace::buildText( Cell* cell )
{
    if( spaces ) {
        if( whiteSpace == Tag::SPACES && text ) {
            Text::buildText( cell );
        }
        else if( col == 1 ) {
            for( unsigned char count = 0; count < spaces / 2; ++count )
                cell->addByte( 0xFE );
            if( spaces & 1 ) {
                cell->addByte( 0xFC );
                cell->addByte( 0xFE );
                cell->addByte( 0xFC );
            }
        }
        else if( spaces > 1 ) {
            if( spaces & 1 ) {
                for( unsigned char count = 0; count < spaces / 2; ++count )
                    cell->addByte( 0xFE );
            }
            else {
                for( unsigned char count = 0; count < spaces / 2 - 1; ++count )
                    cell->addByte( 0xFE );
                cell->addByte( 0xFC );
                cell->addByte( 0xFE );
                cell->addByte( 0xFC );
            }
        }
    }
    else if( whiteSpace != Tag::NONE )  //'\n'
        cell->addByte( 0xFD );
    if( cell->textFull() )
        printError( ERR1_LARGEPAGE );
}
/***************************************************************************/
std::pair< bool, bool > LiteralWhiteSpace::buildLocalDict( Page* page )
{
    std::pair< bool, bool > retval( false, toggleSpacing );
    page = page;
    return retval;
}
/***************************************************************************/
void LiteralWhiteSpace::buildText( Cell* cell )
{
    if( toggleSpacing )
        cell->addByte( 0xFC );
    cell->addByte( 0xFE );
    if( cell->textFull() )
        printError( ERR1_LARGEPAGE );
}
