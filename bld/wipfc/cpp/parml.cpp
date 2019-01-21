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


#include "wipfc.hpp"
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
                        Dl *dl = new Dl( _document, this, _document->dataName(),
                            _document->dataLine(), _document->dataCol(), _nestLevel + 1,
                            _indent == 1 ? 4 : _indent + 3 );
                        appendChild( dl );
                        tok = dl->parse( lexer );
                    }
                    break;
                case Lexer::OL:
                    {
                        Ol *ol = new Ol( _document, this, _document->dataName(),
                            _document->dataLine(), _document->dataCol(),
                            _nestLevel + 1, _indent == 1 ? 4 : _indent + 3 );
                        appendChild( ol );
                        tok = ol->parse( lexer );
                    }
                    break;
                case Lexer::PD:
                    _document->printError( ERR1_DLDTDDMATCH );
                    while( tok != Lexer::TAGEND )
                        tok = _document->getNextToken();
                    tok = _document->getNextToken();
                    break;
                case Lexer::PT:
                    {
                        Pt *pt = new Pt( _document, this, _document->dataName(),
                            _document->lexerLine(), _document->lexerCol(), _indent,
                            _tabSize, _breakage, _compact && notFirst );
                        appendChild( pt );
                        tok = pt->parse( lexer );
                        notFirst = true;
                    }
                    break;
                case Lexer::EPARML:
                    {
                        EParml *eparml = new EParml( _document, this, _document->dataName(),
                            _document->lexerLine(), _document->lexerCol() );
                        appendChild( eparml );
                        tok = eparml->parse( lexer );
                        if( !_nestLevel ) {
                            appendChild( new BrCmd( _document, this, _document->dataName(),
                                _document->dataLine(), _document->dataCol() ) );
                        }
                        return tok;
                    }
                case Lexer::SL:
                    {
                        Sl *sl = new Sl( _document, this, _document->dataName(),
                            _document->dataLine(), _document->dataCol(),
                            0, _indent == 1 ? 4 : _indent + 3 );
                        appendChild( sl );
                        tok = sl->parse( lexer );
                    }
                    break;
                case Lexer::UL:
                    {
                        Ul *ul = new Ul( _document, this, _document->dataName(),
                            _document->dataLine(), _document->dataCol(),
                            _nestLevel + 1, _indent == 1 ? 4 : _indent + 3 );
                        appendChild( ul );
                        tok = ul->parse( lexer );
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
Lexer::Token Parml::parseAttributes( Lexer* lexer )
{
    Lexer::Token tok;

    while( (tok = _document->getNextToken()) != Lexer::TAGEND ) {
        if( tok == Lexer::ATTRIBUTE ) {
            std::wstring key;
            std::wstring value;
            splitAttribute( lexer->text(), key, value );
            if( key == L"tsize" ) {
                _tabSize = static_cast< byte >( std::wcstoul( value.c_str(), 0, 10 ) );
            } else if( key == L"break" ) {
                if( value == L"none" ) {
                    _breakage = NONE;
                } else if( value == L"fit" ) {
                    _breakage = FIT;
                } else if( value == L"all" ) {
                    _breakage = ALL;
                } else {
                    _document->printError( ERR2_VALUE );
                }
            } else {
                _document->printError( ERR1_ATTRNOTDEF );
            }
        } else if( tok == Lexer::FLAG ) {
            if( lexer->text() == L"compact" ) {
                _compact = true;
            } else {
                _document->printError( ERR1_ATTRNOTDEF );
            }
        } else if( tok == Lexer::ERROR_TAG ) {
            throw FatalError( ERR_SYNTAX );
        } else if( tok == Lexer::END ) {
            throw FatalError( ERR_EOF );
        } else {
            _document->printError( ERR1_TAGSYNTAX );
        }
    }
    return _document->getNextToken();    //consume TAGEND
}
/***************************************************************************/
void EParml::buildText( Cell* cell )
{
    cell->addByte( Cell::ESCAPE );  //esc
    cell->addByte( 0x03 );          //size
    cell->addByte( 0x02 );          //set left margin
    cell->addByte( 1 );
    if( cell->textFull() ) {
        printError( ERR1_LARGEPAGE );
    }
}
/***************************************************************************/
Lexer::Token Pt::parse( Lexer* lexer )
{
    Lexer::Token tok( parseAttributes( lexer ) );
    appendChild( new Lm( _document, this, _document->dataName(),
        _document->lexerLine(), _document->lexerCol(), _indent ) );
    if( _compact ) {
        appendChild( new BrCmd( _document, this, _document->dataName(),
            _document->lexerLine(), _document->lexerCol() ) );
    } else {
        appendChild( new P( _document, this, _document->dataName(),
            _document->lexerLine(), _document->lexerCol() ) );
    }
    while( tok != Lexer::END && !( tok == Lexer::TAG && lexer->tagId() == Lexer::EUSERDOC ) ) {
        switch( tok ) {
        case Lexer::WORD:
            _textLength += static_cast< byte >( lexer->text().size() );
            break;
        case Lexer::ENTITY:
            {
                const std::wstring* text( _document->nameit( lexer->text() ) );
                if( text ) {
                    std::wstring* name( _document->prepNameitName( lexer->text() ) );
                    IpfBuffer* buffer( new IpfBuffer( name, _document->dataLine(), _document->dataCol(), *text ) );
                    _document->pushInput( buffer );
                    tok = _document->getNextToken();
                } else {
                    ++_textLength;
                }
            }
            break;
        case Lexer::PUNCTUATION:
            ++_textLength;
            break;
        case Lexer::WHITESPACE:
            if( lexer->text()[0] != L'\n' )
                ++_textLength;
            break;
        default:
            break;
        }
        if( parseInline( lexer, tok ) ) {
            if( lexer->tagId() == Lexer::PD ) {
                Pd *pd;
                if( _breakage == Parml::NONE ) { //keep on same line
                    pd = new Pd( _document, this, _document->dataName(),
                        _document->lexerLine(), _document->lexerCol(), _indent, _tabSize, false );
                } else if( _breakage == Parml::FIT ) {
                    pd = new Pd( _document, this, _document->dataName(),
                        _document->lexerLine(), _document->lexerCol(), _indent, _tabSize,
                        _textLength >= _tabSize );
                } else {                        //place on next line
                    pd = new Pd( _document, this, _document->dataName(),
                        _document->lexerLine(), _document->lexerCol(), _indent, _tabSize, true );
                }
                appendChild( pd );
                tok = pd->parse( lexer );
            } else {
                break;
            }
        }
    }
    return tok;
}
/***************************************************************************/
Lexer::Token Pd::parse( Lexer* lexer )
{
    Lexer::Token tok( parseAttributes( lexer ) );
    appendChild( new Lm( _document, this, _document->dataName(),
        _document->lexerLine(), _document->lexerCol(), _indent + _tabSize ) );
    if( _doBreak )
        appendChild( new BrCmd( _document, this, _document->dataName(),
            _document->lexerLine(), _document->lexerCol() ) );
    while( tok != Lexer::END && !( tok == Lexer::TAG && lexer->tagId() == Lexer::EUSERDOC ) ) {
        if( parseInline( lexer, tok ) ) {
            if( lexer->tagId() == Lexer::PD )
                parseCleanup( lexer, tok );
            break;
        }
    }
    return tok;
}

