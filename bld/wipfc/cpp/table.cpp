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
* Description:  Process table/etable tags
*
*   :table / :etable
*       cols='' (space separated list of widths in characters)
*       rules=both|horiz|vert|none (default: both)
*       frame=rules|box|none (default: box)
*   Supplying more column data than columns is an error
*   Text is formatted at compile time
*
****************************************************************************/


#include "wipfc.hpp"
#include <cstdlib>
#include <numeric>
#include <string>
#include <cwctype>
#include "table.hpp"
#include "brcmd.hpp"
#include "cell.hpp"
#include "color.hpp"
#include "document.hpp"
#include "entity.hpp"
#include "font.hpp"
#include "hpn.hpp"
#include "ipfbuff.hpp"
#include "link.hpp"
#include "punct.hpp"
#include "util.hpp"
#include "whtspc.hpp"
#include "word.hpp"

Lexer::Token Table::parse( Lexer* lexer )
{
    Lexer::Token tok( parseAttributes( lexer ) );
    unsigned int rowCount( 0 );
    tbBorder( false );
    while( tok != Lexer::END && !( tok == Lexer::TAG && lexer->tagId() == Lexer::EUSERDOC ) ) {
        //allowed: '\n', command, :row.
        if( tok == Lexer::WHITESPACE && lexer->text()[ 0 ] == L'\n' ) {
            tok = _document->getNextToken();     //ignore \n's
        } else if( tok == Lexer::COMMAND ) {
            _document->parseCommand( lexer, this );
            tok = _document->getNextToken();
        } else if( tok == Lexer::TAG ) {
            if( lexer->tagId() == Lexer::ETABLE ) {
                break;
            } else if( lexer->tagId() == Lexer::TROW ) {
                if( rowCount )
                    rowRule();
                TableRow *tablerow = new TableRow( _document, this, _document->dataName(),
                    _document->lexerLine(), _document->lexerCol(), _colWidth, _rules, _frame );
                appendChild( tablerow );
                tok = tablerow->parse( lexer );
                ++rowCount;
            } else {
                _document->printError( ERR1_TAGCONTEXT );
                parseCleanup( lexer, tok );
            }
        } else {
            _document->printError( ERR1_TABLETEXT );
            tok = _document->getNextToken();
        }
    }
    tbBorder( true );
    return tok;
}
/***************************************************************************/
Lexer::Token Table::parseAttributes( Lexer* lexer )
{
    Lexer::Token tok;

    while( (tok = _document->getNextToken()) != Lexer::TAGEND ) {
        if( tok == Lexer::ATTRIBUTE ) {
            std::wstring key;
            std::wstring value;
            splitAttribute( lexer->text(), key, value );
            if( key == L"cols" ) {
                std::wstring::size_type idx1( 0 );
                std::wstring::size_type idx2( value.find( L' ' ) );
                while( idx1 != std::wstring::npos ) { //split value on ' '
                    _colWidth.push_back(
                        static_cast< byte >( std::wcstoul( value.substr( idx1, idx2 - idx1 ).c_str(), 0, 10 ) ) );
                    idx1 = idx2 == std::wstring::npos ? std::wstring::npos : idx2 + 1;
                    idx2 = value.find( L' ', idx1 );
                }
            } else if( key == L"rules" ) {
                if( value == L"none" ) {
                    _rules = NO_RULES;
                } else if( value == L"horiz" ) {
                    _rules = HORIZONTAL;
                } else if( value == L"vert" ) {
                    _rules = VERTICAL;
                } else if( value == L"both" ) {
                    _rules = BOTH;
                } else {
                    _document->printError( ERR2_VALUE );
                }
            } else if( key == L"frame" ) {
                if( value == L"none" ) {
                    _frame = NO_FRAME;
                } else if( value == L"rules" ) {
                    _frame = RULES;
                } else if( value == L"box" ) {
                    _frame = BOX;
                } else {
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
        } else {
            _document->printError( ERR1_TAGSYNTAX );
        }
    }
    if( _colWidth.empty() )
        _document->printError( ERR1_NOCOLS );
#if 0
    unsigned int total( 0 );
    for( ConstColWidthIter itr = _colWidth.begin(); itr != _colWidth.end(); ++itr )
        total += *itr;
    if( total > 250 )
        _document->printError( ERR1_TABLEWIDTH );
#else
    //this could overflow
    if( std::accumulate( _colWidth.begin(), _colWidth.end(), 0 ) > 250 )
        _document->printError( ERR1_TABLEWIDTH );
#endif
    return _document->getNextToken();    //consume TAGEND
}
/***************************************************************************/
void Table::tbBorder( bool bottom )
{
    if( _frame || ( _rules & VERTICAL ) ) {
        std::wstring text1;
        std::wstring text2;
        std::wstring text3;
        unsigned int count1( 0 );
        wchar_t      ch( L' ' );
        try {
            if( _frame ) {
                ch = _document->entityChar( L"&bxh." );
                if( _frame == BOX ) {
                    if( bottom ) {
                        text1.push_back( _document->entityChar( L"&bxll." ) );
                        text3.push_back( _document->entityChar( L"&bxlr." ) );
                        if( _rules & VERTICAL ) {
                            text2.push_back( _document->entityChar( L"&bxas." ) );
                        } else {
                            text2.push_back( ch );
                        }
                    } else {
                        text1.push_back( _document->entityChar( L"&bxul." ) );
                        text3.push_back( _document->entityChar( L"&bxur." ) );
                        if( _rules & VERTICAL ) {
                            text2.push_back( _document->entityChar( L"&bxde." ) );
                        } else {
                            text2.push_back( ch );
                        }
                    }
                } else {
                    text1.push_back( ch );
                    text3.push_back( ch );
                    if( _rules & VERTICAL ) {
                        if( bottom ) {
                            text2.push_back( _document->entityChar( L"&bxas." ) );
                        } else {
                            text2.push_back( _document->entityChar( L"&bxde." ) );
                        }
                    } else {
                        text2.push_back( ch );
                    }
                }
            } else {
                text1.push_back( L' ' );
                text3.push_back( L' ' );
                if( _rules & VERTICAL ) {
                    text2.push_back( _document->entityChar( L"&bxv." ) );
                }
            }
        }
        catch( Class2Error& e ) {
            printError( e._code );
        }
        //left frame edge
        appendChild( new TextWord( _document, this, _document->dataName(),
            _document->lexerLine(), _document->lexerCol(), text1) );
        //the columns
        if( !_colWidth.empty() ) {
            for( count1 = 0; count1 < _colWidth.size() - 1; ++count1 ) {
                std::wstring text( _colWidth[ count1 ], ch );
                appendChild( new TextWord( _document, this, _document->dataName(),
                    _document->lexerLine(), _document->lexerCol(), text) );
                appendChild( new TextWord( _document, this, _document->dataName(),
                    _document->lexerLine(), _document->lexerCol(), text2) );
            }
            std::wstring text( _colWidth[ count1 ], ch );
            appendChild( new TextWord( _document, this, _document->dataName(),
                _document->lexerLine(), _document->lexerCol(), text) );
        }
        //right frame edge
        if( _frame ) {
            appendChild( new TextWord( _document, this, _document->dataName(),
                _document->lexerLine(), _document->lexerCol(), text3) );
        }
    }
    appendChild( new BrCmd( _document, this, _document->dataName(),
            _document->lexerLine(), _document->lexerCol() ) );
}
/***************************************************************************/
void Table::rowRule()
{
    if( _rules || _frame == BOX ) {
        std::wstring text1;
        std::wstring text2;
        std::wstring text3;
        unsigned int count1( 0 );
        wchar_t      ch( L' ' );
        try {
            if( _frame == BOX ) {
                if( _rules & HORIZONTAL ) {
                    text1.push_back( _document->entityChar( L"&bxlj." ) );
                    text3.push_back( _document->entityChar( L"&bxrj." ) );
                } else {
                    text1.push_back( _document->entityChar( L"&bxv." ) );
                    text3.push_back( _document->entityChar( L"&bxv." ) );
                }
            } else {
                text1.push_back( L' ' );
                text3.push_back( L' ' );
            }
            if( _rules & HORIZONTAL ) {
                ch = _document->entityChar( L"&bxh." );
                if( _rules & VERTICAL ) {
                    text2.push_back( _document->entityChar( L"&bxcr." ) );
                } else {
                    text2.push_back( ch );
                }
            } else if( _rules & VERTICAL ) {
                text2.push_back( _document->entityChar( L"&bxv." ) );
            } else {
                text2.push_back( L' ' );
            }
        }
        catch( Class2Error& e ) {
            printError( e._code );
        }
        //left frame edge
        appendChild( new TextWord( _document, this, _document->dataName(),
            _document->lexerLine(), _document->lexerCol(), text1 ) );
        //the columns
        if( !_colWidth.empty() ) {
            for( count1 = 0; count1 < _colWidth.size() - 1; ++count1 ) {
                std::wstring text( _colWidth[ count1 ], ch );
                appendChild( new TextWord( _document, this, _document->dataName(),
                    _document->lexerLine(), _document->lexerCol(), text ) );
                appendChild( new TextWord( _document, this, _document->dataName(),
                    _document->lexerLine(), _document->lexerCol(), text2 ) );
            }
            std::wstring text( _colWidth[ count1 ], ch );
            appendChild( new TextWord( _document, this, _document->dataName(),
                _document->lexerLine(), _document->lexerCol(), text ) );
        }
        //right frame edge
        appendChild( new TextWord( _document, this, _document->dataName(),
            _document->lexerLine(), _document->lexerCol(), text3 ) );
    }
    appendChild( new BrCmd( _document, this, _document->dataName(),
            _document->lexerLine(), _document->lexerCol() ) );
}
/***************************************************************************/
void Table::buildText( Cell* cell )
{
    cell->addByte( Cell::ESCAPE );  //esc
    cell->addByte( 0x02 );          //size
    cell->addByte( 0x0B );          //begin monospaced
    if( cell->textFull() ) {
        printError( ERR1_LARGEPAGE );
    }
}
/*****************************************************************************/
void ETable::buildText( Cell* cell )
{
    cell->addByte( Cell::ESCAPE );  //esc
    cell->addByte( 0x02 );          //size
    cell->addByte( 0x0C );          //end monospaced
    if( cell->textFull() ) {
        printError( ERR1_LARGEPAGE );
    }
}
/*****************************************************************************/
TableRow::~TableRow()
{
    for( ColIter itr = _columns.begin(); itr != _columns.end(); ++itr ) {
        delete *itr;
    }
}
/*****************************************************************************/
Lexer::Token TableRow::parse( Lexer* lexer )
{
    Lexer::Token tok( parseAttributes( lexer ) );
    unsigned int colCount( 0 );
    unsigned int rows( 0 );
    std::wstring text1( 1, _frame == Table::BOX ? _document->entityChar( L"&bxv." ) : L' ' );
    std::wstring text2( 1, (_rules & Table::VERTICAL) ? _document->entityChar( L"&bxv." ) : L' ' );
    while( tok != Lexer::END && !( tok == Lexer::TAG && lexer->tagId() == Lexer::EUSERDOC ) ) {
        //allowed: '\n', command, :c.
        if( tok == Lexer::WHITESPACE && lexer->text()[ 0 ] == L'\n' ) {
            tok = _document->getNextToken();     //ignore \n's
        } else if( tok == Lexer::COMMAND ) {
            _document->parseCommand( lexer, this );
            tok = _document->getNextToken();
        } else if( tok == Lexer::TAG ) {
            if( lexer->tagId() == Lexer::ETABLE || lexer->tagId() == Lexer::TROW ) {
                break;
            } else if( lexer->tagId() == Lexer::TCOL ) {
                byte width( 0 );
                if( colCount < _colWidth.size() ) {
                    width = _colWidth[ colCount ];
                } else {
                    _document->printError( ERR1_TABLECELLCOUNTHIGH );
                }
                TableCol* tablecol( new TableCol( _document, this, _document->dataName(),
                    _document->lexerLine(), _document->lexerCol(), width ) );
                _columns.push_back( tablecol );
                ++colCount;
                tok = tablecol->parse( lexer );
                if( tablecol->rows() > rows ) {
                    rows = tablecol->rows();
                }
            } else {
                _document->printError( ERR1_TAGCONTEXT );
                parseCleanup( lexer, tok );
            }
        } else {
            _document->printError( ERR1_TABLETEXT );
            tok = _document->getNextToken();
        }
    }
    if( colCount < _colWidth.size() ) {    //check for unspecified columns
        _document->printError( ERR1_TABLECELLCOUNTLOW );
        while( colCount < _colWidth.size() ) {
            byte width( _colWidth[ colCount ] );
            TableCol* tablecol( new TableCol( _document, this, _document->dataName(),
                _document->lexerLine(), _document->lexerCol(), width ) );
            _columns.push_back( tablecol );
            ++colCount;
        }
    }
    for( unsigned int count1 = 0; count1 < rows; ++count1 ) {
        appendChild( new TextWord( _document, this, _document->dataName(),
            _document->lexerLine(), _document->lexerCol(), text1) );
        for( unsigned int count2 = 0; count2 < _colWidth.size(); ++count2 ) {
            if( count1 < _columns[ count2 ]->rows() &&
                _columns[ count2 ]->rowData( count1 ).size() ) {
                std::list< Element* >& _data = _columns[ count2 ]->rowData( count1 );
                for( ChildrenIter itr = _data.begin(); itr != _data.end(); ++itr ) {
                    appendChild( *itr );
                }
            } else {  //it's blank
                std::wstring blank( _colWidth[ count2 ], L' ' );
                appendChild( new WhiteSpace( _document, this, _document->dataName(),
                    _document->lexerLine(), _document->lexerCol(), blank, _whiteSpace ) );
            }
            if( count2 < _colWidth.size() - 1 ) {
                appendChild( new TextWord( _document, this, _document->dataName(),
                    _document->lexerLine(), _document->lexerCol(), text2 ) );
            }
        }
        if( _frame == Table::BOX )
            appendChild( new TextWord( _document, this, _document->dataName(),
                _document->lexerLine(), _document->lexerCol(), text1) );
        appendChild( new BrCmd( _document, this, _document->dataName(),
            _document->lexerLine(), _document->lexerCol() ) );
    }
    return tok;
}
/*****************************************************************************/
Lexer::Token TableCol::parse( Lexer* lexer )
{
    Lexer::Token tok( parseAttributes( lexer ) );
    bool doneF( false );
    bool inLink( false );
    bool inLines( false );
    std::size_t spaces( _colWidth );
    unsigned int cellLine( 0 );
    unsigned int currentLine( _document->dataLine() );
    while( !doneF && tok != Lexer::END ) {
        if( tok == Lexer::WORD ) {
            if( inLines && currentLine < _document->dataLine() ) {
                if( spaces > 0 ) {
                    std::wstring text( spaces, L' ' );
                    WhiteSpace* ws( new WhiteSpace( _document, this, _document->dataName(),
                        _document->lexerLine(), _document->lexerCol(), text, _whiteSpace ) );
                    appendData( cellLine, ws );
                }
                currentLine = _document->dataLine();
                std::list< Element* > lst;
                _data.push_back( lst );
                ++cellLine;
                spaces = _colWidth;
            }
            bool nextIsPunct( false );
            std::wstring text( lexer->text() );  //get text from lexer
            tok = _document->getNextToken();
            while( tok == Lexer::WORD || tok == Lexer::ENTITY ) {
                if( tok == Lexer::WORD ) {
                    text += lexer->text();       //part of a compound ...-word-entity-word-...
                } else if( tok == Lexer::ENTITY ) {
                    const std::wstring* exp( _document->nameit( lexer->text() ) );
                    if( exp ) {
                        std::wstring* name( _document->prepNameitName( lexer->text() ) );
                        IpfBuffer* buffer( new IpfBuffer( name, _document->dataLine(), _document->dataCol(), *exp ) );
                        _document->pushInput( buffer );
                    } else {
                        try {
                            wchar_t entityChar( _document->entityChar( lexer->text() ) );
                            if( std::iswpunct( entityChar ) ) {
                                nextIsPunct = true;
                                break;
                            }
                            text += entityChar;
                        }
                        catch( Class2Error& e ) {
                            _document->printError( e._code );
                            break;
                        }
                    }
                }
                tok = _document->getNextToken();
            }
            std::size_t textSize( text.size() );
            if( inLines ) {
                if( text.size() > spaces ) {
                    text.erase( spaces );  //trim text
                    textSize = spaces;
                    _document->printError( ERR1_TABLECELLTEXTWIDTH );
                }
                if( textSize > 0 ) {
                    appendData( cellLine, new TextWord( _document, this, _document->dataName(),
                        _document->lexerLine(), _document->lexerCol(), text ) );
                    spaces -= textSize;
                }
            } else {
                if( text.size() > _colWidth ) {
                    text.erase( _colWidth );  //trim text
                    textSize = _colWidth;
                    _document->printError( ERR1_TABLECELLTEXTWIDTH );
                }
                if( textSize < _colWidth && ( nextIsPunct || tok == Lexer::PUNCTUATION ) )
                    ++textSize;              //keep puctuation together with word if possible
                if( textSize > spaces ) {    //need new line
                    if( spaces > 0 ) {
                        std::wstring spaceText( spaces, L' ' );
                        appendData( cellLine, new WhiteSpace( _document, this, _document->dataName(),
                            _document->lexerLine(), _document->lexerCol(), spaceText, _whiteSpace ) );
                    }
                    std::list< Element* > lst;
                    _data.push_back( lst );
                    ++cellLine;
                    spaces = _colWidth;
                }
                appendData( cellLine, new TextWord( _document, this, _document->dataName(),
                    _document->lexerLine(), _document->lexerCol(), text ) );
                spaces -= textSize;
                if( textSize < _colWidth && ( nextIsPunct || tok == Lexer::PUNCTUATION ) ) {
                    ++spaces;
                }
            }
        } else if( tok == Lexer::ENTITY ) {
            const std::wstring* nameitText( _document->nameit( lexer->text() ) ); //lookup nameit
            if( nameitText ) {
                std::wstring* name( _document->prepNameitName( lexer->text() ) );
                _document->pushInput( new IpfBuffer( name, _document->dataLine(), _document->dataCol(), *nameitText ) );
                tok = _document->getNextToken();
                continue;
            }
            if( inLines && currentLine < _document->dataLine() ) {
                if( spaces > 0 ) {
                    std::wstring spaceText( spaces, L' ' );
                    appendData( cellLine, new WhiteSpace( _document, this, _document->dataName(),
                        _document->lexerLine(), _document->lexerCol(), spaceText, _whiteSpace ) );
                }
                currentLine = _document->dataLine();
                std::list< Element* > lst;
                _data.push_back( lst );
                ++cellLine;
                spaces = _colWidth;
            }
            bool nextIsPunct( false );
            try {
                wchar_t entityChar( _document->entityChar( lexer->text() ) );    //lookup entity
                std::wstring entityText( 1, entityChar );
                tok = _document->getNextToken();
                if( !std::iswpunct( entityChar ) ) {
                    while( tok == Lexer::WORD || tok == Lexer::ENTITY ) {
                        if( tok == Lexer::WORD ) {
                            entityText += lexer->text();       //part of a compound ...-word-entity-word-...
                        } else if( tok == Lexer::ENTITY ) {
                            entityChar = _document->entityChar( lexer->text() );
                            if( std::iswpunct( entityChar ) ) {
                                nextIsPunct = true;
                                break;
                            }
                            entityText += entityChar;
                        }
                        tok = _document->getNextToken();
                    }
                }
                std::size_t textSize( entityText.size() );
                if( inLines ) {
                    if( entityText.size() > spaces ) {
                        entityText.erase( spaces );  //trim text
                        textSize = spaces;
                        _document->printError( ERR1_TABLECELLTEXTWIDTH );
                    }
                    if( textSize > 0 ) {
                        Entity *entity = new Entity( _document, this, _document->dataName(),
                            _document->lexerLine(), _document->lexerCol(), entityText );
                        appendData( cellLine, entity );
                        spaces -= textSize;
                    }
                } else {
                    if( entityText.size() > _colWidth ) {
                        entityText.erase( _colWidth );  //trim text
                        textSize = _colWidth;
                        _document->printError( ERR1_TABLECELLTEXTWIDTH );
                    }
                    Entity *entity = new Entity( _document, this, _document->dataName(),
                        _document->lexerLine(), _document->lexerCol(), entityText );
                    if( textSize < _colWidth && ( nextIsPunct || tok == Lexer::PUNCTUATION ) )
                        ++textSize;              //keep text and punct on same line if possible
                    if( textSize > spaces ) {    //need new line
                        if( spaces > 0 ) {
                            std::wstring spaceText( spaces, L' ' );
                            WhiteSpace* ws( new WhiteSpace( _document, this, _document->dataName(),
                                _document->lexerLine(), _document->lexerCol(), spaceText, _whiteSpace ) );
                            appendData( cellLine, ws );
                        }
                        std::list< Element* > lst;
                        _data.push_back( lst );
                        ++cellLine;
                        spaces = _colWidth;
                    }
                    appendData( cellLine, entity );
                    spaces -= textSize;
                    if( textSize < _colWidth && ( nextIsPunct || tok == Lexer::PUNCTUATION ) ) {
                        ++spaces;
                    }
                }
            }
            catch( Class2Error& e ) {
                _document->printError( e._code );
                tok = _document->getNextToken();
            }
        } else if( tok == Lexer::PUNCTUATION ) {
            if( inLines ) {
                if( currentLine < _document->dataLine() ) {
                    currentLine = _document->dataLine();
                    if( spaces > 0 ) {
                        std::wstring spaceText( spaces, L' ' );
                        WhiteSpace* ws( new WhiteSpace( _document, this, _document->dataName(),
                            _document->lexerLine(), _document->lexerCol(), spaceText, _whiteSpace ) );
                        appendData( cellLine, ws );
                    }
                    std::list< Element* > lst;
                    _data.push_back( lst );
                    ++cellLine;
                    spaces = _colWidth;
                }
                if( spaces > 0 ) {
                    Punctuation* punct( new Punctuation( _document, this, _document->dataName(),
                        _document->lexerLine(), _document->lexerCol(), lexer->text(), false ) );
                    appendData( cellLine, punct );
                    --spaces;
                }
            } else {
                if( spaces == 0 ) {
                    std::list< Element* > lst;
                    _data.push_back( lst );
                    ++cellLine;
                    spaces = _colWidth;
                }
                Punctuation* punct( new Punctuation( _document, this, _document->dataName(),
                    _document->lexerLine(), _document->lexerCol(), lexer->text(), false ) );
                appendData( cellLine, punct );
                --spaces;
            }
            tok = _document->getNextToken();
        } else if( tok == Lexer::WHITESPACE ) {
            if( lexer->text()[0] == L'\n' ) {   //ignore \n's
                tok = _document->getNextToken();
            } else if( inLines ) {
                if( currentLine < _document->dataLine() ) {
                    currentLine = _document->dataLine();
                    if( spaces > 0 ) {
                        std::wstring spaceText( spaces, L' ' );
                        WhiteSpace* ws( new WhiteSpace( _document, this, _document->dataName(),
                            _document->lexerLine(), _document->lexerCol(), spaceText, _whiteSpace ) );
                        appendData( cellLine, ws );
                    }
                    std::list< Element* > lst;
                    _data.push_back( lst );
                    ++cellLine;
                    spaces = _colWidth;
                }
                if( spaces > 0 )  {
                    std::wstring whitespaceText( lexer->text() );
                    if( whitespaceText.size() > spaces ) {
                        whitespaceText.erase( spaces );        //trim text
                    }
                    WhiteSpace* ws( new WhiteSpace( _document, this, _document->dataName(),
                        _document->lexerLine(), _document->lexerCol(), whitespaceText, _whiteSpace ) );
                    appendData( cellLine, ws );
                    spaces -= whitespaceText.size();
                }
                tok = _document->getNextToken();
            } else {
                std::wstring whitespaceText( lexer->text() );  //get text from lexer
                tok = _document->getNextToken();
                while( tok == Lexer::WHITESPACE ) { //accumulate whitespace
                    if( lexer->text()[0] != L'\n' ) //ignore \n's
                        whitespaceText += lexer->text();
                    tok = _document->getNextToken();
                }
                if( spaces > 0 ) {
                    if( whitespaceText.size() > spaces )
                        whitespaceText.erase( spaces );            //trim text
                    WhiteSpace* ws( new WhiteSpace( _document, this, _document->dataName(),
                        _document->lexerLine(), _document->lexerCol(), whitespaceText, _whiteSpace ) );
                    appendData( cellLine, ws );
                    spaces -= whitespaceText.size();
                }
            }
        } else if( tok == Lexer::COMMAND ) {
            _document->parseCommand( lexer, this );
            tok = _document->getNextToken();
        } else if( tok == Lexer::TAG ) {
            switch( lexer->tagId() ) {
            case Lexer::COLOR:
                {
                    Color *color = new Color( _document, this, _document->dataName(),
                        _document->lexerLine(), _document->lexerCol() );
                    appendData( cellLine, color );
                    tok = color->parse( lexer );
                }
                break;
            case Lexer::FONT:
                {
                    Font *font = new Font( _document, this, _document->dataName(),
                        _document->lexerLine(), _document->lexerCol() );
                    appendData( cellLine, font );
                    tok = font->parse( lexer );
                }
                break;
            case Lexer::HP1:
                {
                    Hpn *hpn = new Hpn( _document, this, _document->dataName(),
                        _document->lexerLine(), _document->lexerCol(), 1 );
                    appendData( cellLine, hpn );
                    tok = hpn->parse( lexer );
                }
                break;
            case Lexer::HP2:
                {
                    Hpn *hpn = new Hpn( _document, this, _document->dataName(),
                        _document->lexerLine(), _document->lexerCol(), 2 );
                    appendData( cellLine, hpn );
                    tok = hpn->parse( lexer );
                }
                break;
            case Lexer::HP3:
                {
                    Hpn *hpn = new Hpn( _document, this, _document->dataName(),
                        _document->lexerLine(), _document->lexerCol(), 3 );
                    appendData( cellLine, hpn );
                    tok = hpn->parse( lexer );
                }
                break;
            case Lexer::HP4:
                {
                    Hpn *hpn = new Hpn( _document, this, _document->dataName(),
                        _document->lexerLine(), _document->lexerCol(), 4 );
                    appendData( cellLine, hpn );
                    tok = hpn->parse( lexer );
                }
                break;
            case Lexer::HP5:
                {
                    Hpn *hpn = new Hpn( _document, this, _document->dataName(),
                        _document->lexerLine(), _document->lexerCol(), 5 );
                    appendData( cellLine, hpn );
                    tok = hpn->parse( lexer );
                }
                break;
            case Lexer::HP6:
                {
                    Hpn *hpn = new Hpn( _document, this, _document->dataName(),
                        _document->lexerLine(), _document->lexerCol(), 6 );
                    appendData( cellLine, hpn );
                    tok = hpn->parse( lexer );
                }
                break;
            case Lexer::HP7:
                {
                    Hpn *hpn = new Hpn( _document, this, _document->dataName(),
                        _document->lexerLine(), _document->lexerCol(), 7 );
                    appendData( cellLine, hpn );
                    tok = hpn->parse( lexer );
                }
                break;
            case Lexer::HP8:
                {
                    Hpn *hpn = new Hpn( _document, this, _document->dataName(),
                        _document->lexerLine(), _document->lexerCol(), 8 );
                    appendData( cellLine, hpn );
                    tok = hpn->parse( lexer );
                }
                break;
            case Lexer::HP9:
                {
                    Hpn *hpn = new Hpn( _document, this, _document->dataName(),
                        _document->lexerLine(), _document->lexerCol(), 9 );
                    appendData( cellLine, hpn );
                    tok = hpn->parse( lexer );
                }
                break;
            case Lexer::EHP1:
                {
                    EHpn *ehpn = new EHpn( _document, this, _document->dataName(),
                        _document->lexerLine(), _document->lexerCol(), 1 );
                    appendData( cellLine, ehpn );
                    tok = ehpn->parse( lexer );
                }
                break;
            case Lexer::EHP2:
                {
                    EHpn *ehpn = new EHpn( _document, this, _document->dataName(),
                        _document->lexerLine(), _document->lexerCol(), 2 );
                    appendData( cellLine, ehpn );
                    tok = ehpn->parse( lexer );
                }
                break;
            case Lexer::EHP3:
                {
                    EHpn *ehpn = new EHpn( _document, this, _document->dataName(),
                        _document->lexerLine(), _document->lexerCol(), 3 );
                    appendData( cellLine, ehpn );
                    tok = ehpn->parse( lexer );
                }
                break;
            case Lexer::EHP4:
                {
                    EHpn *ehpn = new EHpn( _document, this, _document->dataName(),
                        _document->lexerLine(), _document->lexerCol(), 4 );
                    appendData( cellLine, ehpn );
                    tok = ehpn->parse( lexer );
                }
                break;
            case Lexer::EHP5:
                {
                    EHpn *ehpn = new EHpn( _document, this, _document->dataName(),
                        _document->lexerLine(), _document->lexerCol(), 5 );
                    appendData( cellLine, ehpn );
                    tok = ehpn->parse( lexer );
                }
                break;
            case Lexer::EHP6:
                {
                    EHpn *ehpn = new EHpn( _document, this, _document->dataName(),
                        _document->lexerLine(), _document->lexerCol(), 6 );
                    appendData( cellLine, ehpn );
                    tok = ehpn->parse( lexer );
                }
                break;
            case Lexer::EHP7:
                {
                    EHpn *ehpn = new EHpn( _document, this, _document->dataName(),
                        _document->lexerLine(), _document->lexerCol(), 7 );
                    appendData( cellLine, ehpn );
                    tok = ehpn->parse( lexer );
                }
                break;
            case Lexer::EHP8:
                {
                    EHpn *ehpn = new EHpn( _document, this, _document->dataName(),
                        _document->lexerLine(), _document->lexerCol(), 8 );
                    appendData( cellLine, ehpn );
                    tok = ehpn->parse( lexer );
                }
                break;
            case Lexer::EHP9:
                {
                    EHpn *ehpn = new EHpn( _document, this, _document->dataName(),
                        _document->lexerLine(), _document->lexerCol(), 9 );
                    appendData( cellLine, ehpn );
                    tok = ehpn->parse( lexer );
                }
                break;
            case Lexer::LINES:
                inLines = true;
                while( tok != Lexer::TAGEND )
                    tok = _document->getNextToken();
                tok = _document->getNextToken();
                if( tok == Lexer::WHITESPACE && lexer->text()[0] == L'\n' )
                    tok = _document->getNextToken(); //consume '\n'
                currentLine = _document->dataLine();
                break;
            case Lexer::ELINES:
                inLines = false;
                while( tok != Lexer::TAGEND )
                    tok = _document->getNextToken();
                tok = _document->getNextToken();
                break;
            case Lexer::LINK:
                {
                    Link* link( new Link( _document, this, _document->dataName(),
                        _document->lexerLine(), _document->lexerCol() ) );
                    appendData( cellLine, link );
                    link->setNoEndTag();
                    inLink = true;
                    tok = link->parse( lexer );
                }
                break;
            case Lexer::ELINK:
                {
                    ELink *elink = new ELink( _document, this, _document->dataName(),
                        _document->lexerLine(), _document->lexerCol() );
                    appendData( cellLine, elink );
                    tok = elink->parse( lexer );
                    inLink = false;
                }
                break;
            case Lexer::TROW:
            case Lexer::TCOL:
            case Lexer::ETABLE:
            case Lexer::EUSERDOC:
                doneF = true;
                if( inLink ) {
                    ELink *elink = new ELink( _document, this, _document->dataName(),
                        _document->lexerLine(), _document->lexerCol() );
                    appendData( cellLine, elink );
                    inLink = false;
                    printError( ERR1_TABLEELINK );
                }
                if( spaces > 0 ) {
                    std::wstring spaceText( spaces, L' ' );
                    WhiteSpace* ws( new WhiteSpace( _document, this, _document->dataName(),
                        _document->lexerLine(), _document->lexerCol(), spaceText, _whiteSpace ) );
                    appendData( cellLine, ws );
                }
                break;
            default:
                _document->printError( ERR1_TABLECELLTAG );
                while( tok != Lexer::TAGEND )
                    tok = _document->getNextToken();
                tok = _document->getNextToken();
                break;
            }
        } else if( tok == Lexer::ERROR_TAG ) {
            _document->printError( ERR1_TAGNOTDEF );
            tok = _document->getNextToken();
        } else if( tok == Lexer::ERROR_ENTITY ) {
            _document->printError( ERR1_TAGNOTDEF );
            tok = _document->getNextToken();
        }
    }
    return tok;
}
