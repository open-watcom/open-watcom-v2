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
                case Lexer::DTHD:
                    if( first ) {
                        first = false;
                        needDdHd = true;
                        dthd = new DtHd( _document, this, _document->dataName(),
                            _document->lexerLine(), _document->lexerCol(), _indent );
                        appendChild( dthd );
                        tok = dthd->parse( lexer );
                    } else {
                        _document->printError( ERR1_TAGCONTEXT );
                        while( tok != Lexer::TAGEND )
                            tok = _document->getNextToken();
                        tok = _document->getNextToken();
                    }
                    break;
                case Lexer::DDHD:
                    if( needDdHd ) {
                        needDdHd = false;
                        DdHd *ddhd = new DdHd( _document, this, _document->dataName(),
                            _document->lexerLine(), _document->lexerCol(), _indent,
                            dthd->length() >= _tabSize ? dthd->length() : _tabSize );
                        appendChild( ddhd );
                        tok = ddhd->parse( lexer );
                    } else {
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
                    } else {
                        Dt *dt = new Dt( _document, this, _document->dataName(),
                            _document->lexerLine(), _document->lexerCol(), _indent,
                            _tabSize, _breakage, _compact && !first );
                        appendChild( dt );
                        tok = dt->parse( lexer );
                        first = false;
                    }
                    break;
                case Lexer::EDL:
                    {
                        EDl *edl = new EDl( _document, this, _document->dataName(),
                            _document->lexerLine(), _document->lexerCol() );
                        appendChild( edl );
                        tok = edl->parse( lexer );
                        if( !_nestLevel ) {
                            appendChild( new BrCmd( _document, this, _document->dataName(),
                                _document->dataLine(), _document->dataCol() ) );
                        }
                        return tok;
                    }
                case Lexer::PARML:
                    {
                        Parml *parml = new Parml( _document, this, _document->dataName(),
                            _document->dataLine(), _document->dataCol(), _nestLevel + 1,
                            _indent == 1 ? 4 : _indent + 3 );
                        appendChild( parml );
                        tok = parml->parse( lexer );
                    }
                    break;
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
Lexer::Token Dl::parseAttributes( Lexer* lexer )
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
void EDl::buildText( Cell* cell )
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
Lexer::Token DtHd::parse( Lexer* lexer )
{
    Lexer::Token tok( parseAttributes( lexer ) );
    appendChild( new Lm( _document, this, _document->dataName(),
        _document->lexerLine(), _document->lexerCol(), _indent ) );
    appendChild( new P( _document, this, _document->dataName(),
        _document->lexerLine(), _document->lexerCol() ) );
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
        case Lexer::WHITESPACE:
            ++_textLength;
        default:
            break;
        }
        if( parseInline( lexer, tok ) ) {
            break;
        }
    }
    return tok;
}
/***************************************************************************/
Lexer::Token DdHd::parse( Lexer* lexer )
{
    Lexer::Token tok( parseAttributes( lexer ) );
    appendChild( new Lm( _document, this, _document->dataName(),
        _document->lexerLine(), _document->lexerCol(), _indent + _tabSize ) );
    while( tok != Lexer::END && !( tok == Lexer::TAG && lexer->tagId() == Lexer::EUSERDOC ) ) {
        if( parseInline( lexer, tok ) ) {
            break;
        }
    }
    return tok;
}
/***************************************************************************/
Lexer::Token Dt::parse( Lexer* lexer )
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
            if( lexer->tagId() == Lexer::DD ) {
                Dd *dd;
                if( _breakage == Dl::NONE ) {    //keep on same line
                    dd = new Dd( _document, this, _document->dataName(),
                        _document->lexerLine(), _document->lexerCol(), _indent, _tabSize, false );
                } else if( _breakage == Dl::FIT ) {
                    dd = new Dd( _document, this, _document->dataName(),
                        _document->lexerLine(), _document->lexerCol(), _indent, _tabSize,
                        _textLength >= _tabSize );
                } else {                        //place on next line
                    dd = new Dd( _document, this, _document->dataName(),
                        _document->lexerLine(), _document->lexerCol(), _indent, _tabSize, true );
                }
                appendChild( dd );
                tok = dd->parse( lexer );
            } else {
                break;
            }
        }
    }
    return tok;
}
/***************************************************************************/
Lexer::Token Dd::parse( Lexer* lexer )
{
    Lexer::Token tok( parseAttributes( lexer ) );
    appendChild( new Lm( _document, this, _document->dataName(),
        _document->lexerLine(), _document->lexerCol(), _indent + _tabSize ) );
    if( _doBreak ) {
        appendChild( new BrCmd( _document, this, _document->dataName(),
            _document->lexerLine(), _document->lexerCol() ) );
    }
    while( tok != Lexer::END && !( tok == Lexer::TAG && lexer->tagId() == Lexer::EUSERDOC ) ) {
        if( parseInline( lexer, tok ) ) {
            if( lexer->tagId() == Lexer::DD )
                parseCleanup( lexer, tok );
            break;
        }
    }
    return tok;
}
