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
* Description:  Process parml/eparml tags
*
*   :parml / :eparml
*       tsize=[0-9]+  (default: 10; width of terms and term headers)
*       break=none (term and description on same line)
*             fit  (description on line below if term > tsize)
*             all  (description on line below, default)
*       compact (no blank line between each item)
*   Two columns (term, description)
*   Must contain :pt, :pd (matched)
*   Can be nested in a :pd tag
*
****************************************************************************/

#include "parml.hpp"
#include "dl.hpp"
#include "brcmd.hpp"
#include "cell.hpp"
#include "document.hpp"
#include "errors.hpp"
#include "ipfbuff.hpp"
#include "lexer.hpp"
#include "lm.hpp"
#include "ol.hpp"
#include "sl.hpp"
#include "ul.hpp"
#include "p.hpp"
#include "util.hpp"

Lexer::Token Parml::parse( Lexer* lexer )
{
    Lexer::Token tok( parseAttributes( lexer ) );
    bool notFirst( false );
    while( tok != Lexer::END && !( tok == Lexer::TAG && lexer->tagId() == Lexer::EUSERDOC ) ) {
        if( parseInline( lexer, tok ) ) {
            if( parseBlock( lexer, tok ) ) {
                switch( lexer->tagId() ) {
                case Lexer::DL:
                    {
                        Element* elt( new Dl( document, this, document->dataName(),
                            document->dataLine(), document->dataCol(), nestLevel + 1,
                            indent == 1 ? 4 : indent + 3 ) );
                        appendChild( elt );
                        tok = elt->parse( lexer );
                    }
                    break;
                case Lexer::OL:
                    {
                        Element* elt( new Ol( document, this, document->dataName(),
                            document->dataLine(), document->dataCol(),
                            nestLevel + 1, indent == 1 ? 4 : indent + 3 ) );
                        appendChild( elt );
                        tok = elt->parse( lexer );
                    }
                    break;
                case Lexer::PD:
                    document->printError( ERR1_DLDTDDMATCH );
                    while( tok != Lexer::TAGEND )
                        tok = document->getNextToken();
                    tok = document->getNextToken();
                    break;
                case Lexer::PT:
                    {
                        Element* elt( new Pt( document, this, document->dataName(),
                            document->lexerLine(), document->lexerCol(), indent,
                            tabSize, breakage, compact && notFirst ) );
                        appendChild( elt );
                        tok = elt->parse( lexer );
                        notFirst = true;
                    }
                    break;
                case Lexer::EPARML:
                    {
                        Element* elt( new EParml( document, this, document->dataName(),
                            document->lexerLine(), document->lexerCol() ) );
                        appendChild( elt );
                        tok = elt->parse( lexer );
                        if( !nestLevel )
                            appendChild( new BrCmd( document, this, document->dataName(),
                                document->dataLine(), document->dataCol() ) );
                        return tok;
                    }
                case Lexer::SL:
                    {
                        Element* elt( new Sl( document, this, document->dataName(),
                            document->dataLine(), document->dataCol(),
                            0, indent == 1 ? 4 : indent + 3 ) );
                        appendChild( elt );
                        tok = elt->parse( lexer );
                    }
                    break;
                case Lexer::UL:
                    {
                        Element* elt( new Ul( document, this, document->dataName(),
                            document->dataLine(), document->dataCol(),
                            nestLevel + 1, indent == 1 ? 4 : indent + 3 ) );
                        appendChild( elt );
                        tok = elt->parse( lexer );
                    }
                    break;
                default:
                    document->printError( ERR1_NOENDLIST );
                    return tok;
                }
            }
        }
    }
    return tok;
}
/***************************************************************************/
Lexer::Token Parml::parseAttributes( Lexer* lexer )
{
    Lexer::Token tok( document->getNextToken() );
    while( tok != Lexer::TAGEND ) {
        if( tok == Lexer::ATTRIBUTE ) {
            std::wstring key;
            std::wstring value;
            splitAttribute( lexer->text(), key, value );
            if( key == L"tsize" ) {
                tabSize = static_cast< unsigned char >( std::wcstoul( value.c_str(), 0, 10 ) );
            }
            else if( key == L"break" ) {
                if( value == L"none" )
                    breakage = NONE;
                else if( value == L"fit" )
                    breakage = FIT;
                else if( value == L"all" )
                    breakage = ALL;
                else
                    document->printError( ERR2_VALUE );
            }
            else
                document->printError( ERR1_ATTRNOTDEF );
        }
        else if( tok == Lexer::FLAG ) {
            if( lexer->text() == L"compact" )
                compact = true;
            else
                document->printError( ERR1_ATTRNOTDEF );
        }
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
void EParml::buildText( Cell* cell )
{
    cell->addByte( 0xFF );  //esc
    cell->addByte( 0x03 );  //size
    cell->addByte( 0x02 );  //set left margin
    cell->addByte( 1 );
    if( cell->textFull() )
        printError( ERR1_LARGEPAGE );
}
/***************************************************************************/
Lexer::Token Pt::parse( Lexer* lexer )
{
    Lexer::Token tok( parseAttributes( lexer ) );
    appendChild( new Lm( document, this, document->dataName(),
        document->lexerLine(), document->lexerCol(), indent ) );
    if( compact )
        appendChild( new BrCmd( document, this, document->dataName(),
            document->lexerLine(), document->lexerCol() ) );
    else
        appendChild( new P( document, this, document->dataName(),
            document->lexerLine(), document->lexerCol() ) );
    while( tok != Lexer::END && !( tok == Lexer::TAG && lexer->tagId() == Lexer::EUSERDOC ) ) {
        switch( tok ) {
        case Lexer::WORD:
            textLength += static_cast< unsigned char >( lexer->text().size() );
            break;
        case Lexer::ENTITY:
            {
                const std::wstring* txt( document->nameit( lexer->text() ) );
                if( txt ) {
                    std::wstring* name( document->prepNameitName( lexer->text() ) );
                    IpfBuffer* buffer( new IpfBuffer( name, document->dataLine(), document->dataCol(), *txt ) );
                    document->pushInput( buffer );
                    tok = document->getNextToken();
                }
                else
                    ++textLength;
            }
            break;
        case Lexer::PUNCTUATION:
            ++textLength;
            break;
        case Lexer::WHITESPACE:
            if( lexer->text()[0] != L'\n' )
                ++textLength;
            break;
        default:
            break;
        }
        if( parseInline( lexer, tok ) ) {
            if( lexer->tagId() == Lexer::PD ) {
                Element* ent;
                if( breakage == Parml::NONE )  //keep on same line
                    ent = new Pd( document, this, document->dataName(),
                        document->lexerLine(), document->lexerCol(), indent, tabSize, false );
                else if( breakage == Parml::FIT )
                    ent = new Pd( document, this, document->dataName(),
                        document->lexerLine(), document->lexerCol(), indent, tabSize,
                        textLength >= tabSize );
                else                        //place on next line
                    ent = new Pd( document, this, document->dataName(),
                        document->lexerLine(), document->lexerCol(), indent, tabSize, true );
                appendChild( ent );
                tok = ent->parse( lexer );
            }
            else
                break;
        }
    }
    return tok;
}
/***************************************************************************/
Lexer::Token Pd::parse( Lexer* lexer )
{
    Lexer::Token tok( parseAttributes( lexer ) );
    appendChild( new Lm( document, this, document->dataName(),
        document->lexerLine(), document->lexerCol(), indent + tabSize ) );
    if( doBreak )
        appendChild( new BrCmd( document, this, document->dataName(),
            document->lexerLine(), document->lexerCol() ) );
    while( tok != Lexer::END && !( tok == Lexer::TAG && lexer->tagId() == Lexer::EUSERDOC ) ) {
        if( parseInline( lexer, tok ) ) {
            if( lexer->tagId() == Lexer::PD )
                parseCleanup( lexer, tok );
            break;
        }
    }
    return tok;
}

