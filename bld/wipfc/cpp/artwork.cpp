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
* Description:  Process artwork tag
*
*   :artwork
*       name='' (bitmap)
*       align=left|center|right
*       linkfile='' (see :artlink above)
*       runin (inline, else it's a block)
*       fit
*
****************************************************************************/

#include <vector>
#include "artwork.hpp"
#include "cell.hpp"
#include "document.hpp"
#include "errors.hpp"
#include "page.hpp"
#include "util.hpp"

Lexer::Token Artwork::parse( Lexer* lexer )
{
    Lexer::Token tok( parseAttributes( lexer ) );
    if( name.empty() )
        document->printError( ERR1_NOFILENAME );
    else
        document->addBitmap( name );
    if( linkfile && !linkfile->empty() ) {
        //push the file on the stack and parse
        linkfile = document->addFileName( linkfile );
        document->pushInput( new IpfFile( linkfile ) );
        //tok = document->getNextToken();
    }
    while( tok != Lexer::END && !( tok == Lexer::TAG && lexer->tagId() == Lexer::EUSERDOC)) {
        if( tok == Lexer::WHITESPACE )
            tok = document->getNextToken();
        else if( tok == Lexer::COMMAND ) {
            if( lexer->cmdId() != Lexer::COMMENT )
                break;
            tok = document->getNextToken();
        }
        else if( tok == Lexer::TAG ) {
            if( lexer->tagId() == Lexer::ARTLINK )
                hypergraphic = true;
            break;
        }
        else if( tok == Lexer::ERROR_TAG ) {
            document->printError( ERR1_TAGNOTDEF );
            tok = document->getNextToken();
        }
        else if( tok == Lexer::ERROR_ENTITY ) {
            document->printError( ERR1_TAGNOTDEF );
            tok = document->getNextToken();
        }
        else
            break;
    }
    return tok;
}
/***************************************************************************/
Lexer::Token Artwork::parseAttributes( Lexer* lexer )
{
    Lexer::Token tok( document->getNextToken() );
    while( tok != Lexer::TAGEND ) {
        if( tok == Lexer::ATTRIBUTE ) {
            std::wstring key;
            std::wstring value;
            splitAttribute( lexer->text(), key, value );
            if( key == L"name" )
                name = value;
            else if( key == L"align" ) {
                if( value == L"left" ) {
                    flags &= Artwork::ALIGNMASK;
                    flags |= Artwork::LEFT;
                }
                else if( value == L"right" ) {
                    flags &= Artwork::ALIGNMASK;
                    flags |= Artwork::RIGHT;
                }
                else if( value == L"center" ) {
                    flags &= Artwork::ALIGNMASK;
                    flags |= Artwork::CENTER;
                }
                else
                    document->printError( ERR2_VALUE );
            }
            else if( key == L"linkfile" ) {
                linkfile = new std::wstring( value );
                hypergraphic = true;
            }
            else
                document->printError( ERR1_ATTRNOTDEF );
        }
        else if( tok == Lexer::FLAG ) {
            if( lexer->text() == L"fit" )
                flags |= Artwork::FIT;
            else if( lexer->text() == L"runin" )
                flags |= Artwork::RUNIN;
            else
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
void Artwork::buildText( Cell* cell )
{
    STD1::uint32_t index( document->bitmapByName( name ) );  //get file offset of graphic
    std::vector< STD1::uint8_t > esc;
    esc.push_back( 0xFF );      //esc
    if( !hypergraphic ) {
        esc.push_back( 0x07 );  //size
        esc.push_back( 0x0E );  //bitmap image
    }
    else {
        esc.push_back( 0x08 );  //size
        esc.push_back( 0x0F );  //image map
        esc.push_back( 0x00 );  //define hypergraphic
    }
    esc.push_back( flags );
    esc.push_back( static_cast< STD1::uint8_t >( index ) );
    esc.push_back( static_cast< STD1::uint8_t >( index >> 8) );
    esc.push_back( static_cast< STD1::uint8_t >( index >> 16) );
    esc.push_back( static_cast< STD1::uint8_t >( index >> 24) );
    esc[1] = static_cast< STD1::uint8_t >( esc.size() - 1 );
    cell->addEsc( esc );
    if( cell->textFull() )
        printError( ERR1_LARGEPAGE );
}


