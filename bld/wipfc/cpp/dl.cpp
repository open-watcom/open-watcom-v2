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
* Description:  Process dl/edl tags
*
*   :dl / :edl
*       compact (no blank line between each item)
*       tsize=[0-9]+  (default: 10; width of terms and term headers)
*       break=none (term and description on same line)
*             fit  (description on line below if term > tsize)
*             all  (description on line below)
*   May contain :dthd, :ddhd
*   Must contain :dt, :dd which must follow :dthd, :ddhd
*   Multiple :dt's are allowed, followed by a single :dd
****************************************************************************/


#include "wipfc.hpp"
#include <cstdlib>
#include "dl.hpp"
#include "brcmd.hpp"
#include "cell.hpp"
#include "document.hpp"
#include "errors.hpp"
#include "ipfbuff.hpp"
#include "lexer.hpp"
#include "lm.hpp"
#include "ol.hpp"
#include "parml.hpp"
#include "sl.hpp"
#include "ul.hpp"
#include "p.hpp"
#include "util.hpp"

Lexer::Token Dl::parse( Lexer* lexer )
{
    Lexer::Token tok( parseAttributes( lexer ) );
    //dthd and ddhd, if present, must come first
    bool first( true );
    bool needDdHd( false );
    DtHd* dthd( 0 );
    while( tok != Lexer::END && !( tok == Lexer::TAG && lexer->tagId() == Lexer::EUSERDOC ) ) {
        if( parseInline( lexer, tok ) ) {
            if( parseBlock( lexer, tok ) ) {
                switch( lexer->tagId() ) {
                case Lexer::DL:
                    {
                        Element* elt( new Dl( _document, this, _document->dataName(),
                            _document->dataLine(), _document->dataCol(), nestLevel + 1,
                            indent == 1 ? 4 : indent + 3 ) );
                        appendChild( elt );
                        tok = elt->parse( lexer );
                    }
                    break;
                case Lexer::OL:
                    {
                        Element* elt( new Ol( _document, this, _document->dataName(),
                            _document->dataLine(), _document->dataCol(),
                            nestLevel + 1, indent == 1 ? 4 : indent + 3 ) );
                        appendChild( elt );
                        tok = elt->parse( lexer );
                    }
                    break;
                case Lexer::DTHD:
                    if( first ) {
                        first = false;
                        needDdHd = true;
                        dthd = new DtHd( _document, this, _document->dataName(),
                            _document->lexerLine(), _document->lexerCol(), indent );
                        appendChild( dthd );
                        tok = dthd->parse( lexer );
                    }
                    else {
                        _document->printError( ERR1_TAGCONTEXT );
                        while( tok != Lexer::TAGEND )
                            tok = _document->getNextToken();
                        tok = _document->getNextToken();
                    }
                    break;
                case Lexer::DDHD:
                    if( needDdHd ) {
                        needDdHd = false;
                        Element* elt( new DdHd( _document, this, _document->dataName(),
                            _document->lexerLine(), _document->lexerCol(), indent,
                            dthd->length() >= tabSize ? dthd->length() : tabSize ) );
                        appendChild( elt );
                        tok = elt->parse( lexer );
                    }
                    else {
                        _document->printError( ERR1_DLHEADMATCH );
                        while( tok != Lexer::TAGEND )
                            tok = _document->getNextToken();
                        tok = _document->getNextToken();
                    }
                    break;
                case Lexer::DD:
                    if( needDdHd ) {
                        _document->printError( ERR1_DLHEADMATCH );
                        needDdHd = false;
                    }
                    _document->printError( ERR1_DLDTDDMATCH );
                    while( tok != Lexer::TAGEND )
                        tok = _document->getNextToken();
                    tok = _document->getNextToken();
                    break;
                case Lexer::DT:
                    if( needDdHd ) {
                        _document->printError( ERR1_DLHEADMATCH );
                        needDdHd = false;
                    }
                    else {
                        Element* elt( new Dt( _document, this, _document->dataName(),
                            _document->lexerLine(), _document->lexerCol(), indent,
                            tabSize, breakage, compact && !first ) );
                        appendChild( elt );
                        tok = elt->parse( lexer );
                        first = false;
                    }
                    break;
                case Lexer::EDL:
                    {
                        Element* elt( new EDl( _document, this, _document->dataName(),
                            _document->lexerLine(), _document->lexerCol() ) );
                        appendChild( elt );
                        tok = elt->parse( lexer );
                        if( !nestLevel )
                            appendChild( new BrCmd( _document, this, _document->dataName(),
                                _document->dataLine(), _document->dataCol() ) );
                        return tok;
                    }
                case Lexer::PARML:
                    {
                        Element* elt( new Parml( _document, this, _document->dataName(),
                            _document->dataLine(), _document->dataCol(), nestLevel + 1,
                            indent == 1 ? 4 : indent + 3 ) );
                        appendChild( elt );
                        tok = elt->parse( lexer );
                    }
                    break;
                case Lexer::SL:
                    {
                        Element* elt( new Sl( _document, this, _document->dataName(),
                            _document->dataLine(), _document->dataCol(),
                            0, indent == 1 ? 4 : indent + 3 ) );
                        appendChild( elt );
                        tok = elt->parse( lexer );
                    }
                    break;
                case Lexer::UL:
                    {
                        Element* elt( new Ul( _document, this, _document->dataName(),
                            _document->dataLine(), _document->dataCol(),
                            nestLevel + 1, indent == 1 ? 4 : indent + 3 ) );
                        appendChild( elt );
                        tok = elt->parse( lexer );
                    }
                    break;
                default:
                    _document->printError( ERR1_NOENDLIST );
                    return tok;
                }
            }
        }
    }
    return tok;
}
/***************************************************************************/
Lexer::Token Dl::parseAttributes( Lexer* lexer )
{
    Lexer::Token tok( _document->getNextToken() );
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
                    _document->printError( ERR2_VALUE );
            }
            else
                _document->printError( ERR1_ATTRNOTDEF );
        }
        else if( tok == Lexer::FLAG ) {
            if( lexer->text() == L"compact" )
                compact = true;
            else
                _document->printError( ERR1_ATTRNOTDEF );
        }
        else if( tok == Lexer::ERROR_TAG )
            throw FatalError( ERR_SYNTAX );
        else if( tok == Lexer::END )
            throw FatalError( ERR_EOF );
        else
            _document->printError( ERR1_TAGSYNTAX );
        tok = _document->getNextToken();
    }
    return _document->getNextToken();    //consume TAGEND
}
/***************************************************************************/
void EDl::buildText( Cell* cell )
{
    cell->addByte( 0xFF );  //esc
    cell->addByte( 0x03 );  //size
    cell->addByte( 0x02 );  //set left margin
    cell->addByte( 1 );
    if( cell->textFull() )
        printError( ERR1_LARGEPAGE );
}
/***************************************************************************/
Lexer::Token DtHd::parse( Lexer* lexer )
{
    Lexer::Token tok( parseAttributes( lexer ) );
    appendChild( new Lm( _document, this, _document->dataName(),
        _document->lexerLine(), _document->lexerCol(), indent ) );
    appendChild( new P( _document, this, _document->dataName(),
        _document->lexerLine(), _document->lexerCol() ) );
    while( tok != Lexer::END && !( tok == Lexer::TAG && lexer->tagId() == Lexer::EUSERDOC ) ) {
        switch( tok ) {
        case Lexer::WORD:
            textLength += static_cast< unsigned char >( lexer->text().size() );
            break;
        case Lexer::ENTITY:
            {
                const std::wstring* txt( _document->nameit( lexer->text() ) );
                if( txt ) {
                    std::wstring* name( _document->prepNameitName( lexer->text() ) );
                    IpfBuffer* buffer( new IpfBuffer( name, _document->dataLine(), _document->dataCol(), *txt ) );
                    _document->pushInput( buffer );
                    tok = _document->getNextToken();
                }
                else
                    ++textLength;
            }
            break;
        case Lexer::PUNCTUATION:
        case Lexer::WHITESPACE:
            ++textLength;
        default:
            break;
        }
        if( parseInline( lexer, tok ) )
            break;
    }
    return tok;
}
/***************************************************************************/
Lexer::Token DdHd::parse( Lexer* lexer )
{
    Lexer::Token tok( parseAttributes( lexer ) );
    appendChild( new Lm( _document, this, _document->dataName(),
        _document->lexerLine(), _document->lexerCol(), indent + tabSize ) );
    while( tok != Lexer::END && !( tok == Lexer::TAG && lexer->tagId() == Lexer::EUSERDOC ) ) {
        if( parseInline( lexer, tok ) )
            break;
    }
    return tok;
}
/***************************************************************************/
Lexer::Token Dt::parse( Lexer* lexer )
{
    Lexer::Token tok( parseAttributes( lexer ) );
    appendChild( new Lm( _document, this, _document->dataName(),
        _document->lexerLine(), _document->lexerCol(), indent ) );
    if( compact )
        appendChild( new BrCmd( _document, this, _document->dataName(),
            _document->lexerLine(), _document->lexerCol() ) );
    else
        appendChild( new P( _document, this, _document->dataName(),
            _document->lexerLine(), _document->lexerCol() ) );
    while( tok != Lexer::END && !( tok == Lexer::TAG && lexer->tagId() == Lexer::EUSERDOC ) ) {
        switch( tok ) {
        case Lexer::WORD:
            textLength += static_cast< unsigned char >( lexer->text().size() );
            break;
        case Lexer::ENTITY:
            {
                const std::wstring* txt( _document->nameit( lexer->text() ) );
                if( txt ) {
                    std::wstring* name( _document->prepNameitName( lexer->text() ) );
                    IpfBuffer* buffer( new IpfBuffer( name, _document->dataLine(), _document->dataCol(), *txt ) );
                    _document->pushInput( buffer );
                    tok = _document->getNextToken();
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
            if( lexer->tagId() == Lexer::DD ) {
                Element* ent;
                if( breakage == Dl::NONE )  //keep on same line
                    ent = new Dd( _document, this, _document->dataName(),
                        _document->lexerLine(), _document->lexerCol(), indent, tabSize, false );
                else if( breakage == Dl::FIT )
                    ent = new Dd( _document, this, _document->dataName(),
                        _document->lexerLine(), _document->lexerCol(), indent, tabSize,
                        textLength >= tabSize );
                else                        //place on next line
                    ent = new Dd( _document, this, _document->dataName(),
                        _document->lexerLine(), _document->lexerCol(), indent, tabSize, true );
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
Lexer::Token Dd::parse( Lexer* lexer )
{
    Lexer::Token tok( parseAttributes( lexer ) );
    appendChild( new Lm( _document, this, _document->dataName(),
        _document->lexerLine(), _document->lexerCol(), indent + tabSize ) );
    if( doBreak )
        appendChild( new BrCmd( _document, this, _document->dataName(),
            _document->lexerLine(), _document->lexerCol() ) );
    while( tok != Lexer::END && !( tok == Lexer::TAG && lexer->tagId() == Lexer::EUSERDOC ) ) {
        if( parseInline( lexer, tok ) ) {
            if( lexer->tagId() == Lexer::DD )
                parseCleanup( lexer, tok );
            break;
        }
    }
    return tok;
}

