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
                    _document->lexerLine(), _document->lexerCol(), colWidth, rules, frame );
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
                    colWidth.push_back(
                        static_cast< unsigned char >(
                            std::wcstoul( value.substr( idx1, idx2 - idx1 ).c_str(), 0, 10 ) ) );
                    idx1 = idx2 == std::wstring::npos ? std::wstring::npos : idx2 + 1;
                    idx2 = value.find( L' ', idx1 );
                }
            } else if( key == L"rules" ) {
                if( value == L"none" ) {
                    rules = NO_RULES;
                } else if( value == L"horiz" ) {
                    rules = HORIZONTAL;
                } else if( value == L"vert" ) {
                    rules = VERTICAL;
                } else if( value == L"both" ) {
                    rules = BOTH;
                } else {
                    _document->printError( ERR2_VALUE );
                }
            } else if( key == L"frame" ) {
                if( value == L"none" ) {
                    frame = NO_FRAME;
                } else if( value == L"rules" ) {
                    frame = RULES;
                } else if( value == L"box" ) {
                    frame = BOX;
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
    if( colWidth.empty() )
        _document->printError( ERR1_NOCOLS );
#if 0
    unsigned int total( 0 );
    for( ConstColWidthIter itr = colWidth.begin(); itr != colWidth.end(); ++itr )
        total += *itr;
    if( total > 250 )
        _document->printError( ERR1_TABLEWIDTH );
#else
    //this could overflow
    if( std::accumulate( colWidth.begin(), colWidth.end(), 0 ) > 250 )
        _document->printError( ERR1_TABLEWIDTH );
#endif
    return _document->getNextToken();    //consume TAGEND
}
/***************************************************************************/
void Table::tbBorder( bool bottom )
{
    if( frame  || ( rules & VERTICAL ) ) {
        std::wstring txt1;
        std::wstring txt2;
        std::wstring txt3;
        unsigned int count1( 0 );
        wchar_t      ch( L' ' );
        try {
            if( frame ) {
                ch = _document->entityChar( L"&bxh." );
                if( frame == BOX ) {
                    if( bottom ) {
                        txt1.push_back( _document->entityChar( L"&bxll." ) );
                        txt3.push_back( _document->entityChar( L"&bxlr." ) );
                        if( rules & VERTICAL ) {
                            txt2.push_back( _document->entityChar( L"&bxas." ) );
                        } else {
                            txt2.push_back( ch );
                        }
                    } else {
                        txt1.push_back( _document->entityChar( L"&bxul." ) );
                        txt3.push_back( _document->entityChar( L"&bxur." ) );
                        if( rules & VERTICAL ) {
                            txt2.push_back( _document->entityChar( L"&bxde." ) );
                        } else {
                            txt2.push_back( ch );
                        }
                    }
                } else {
                    txt1.push_back( ch );
                    txt3.push_back( ch );
                    if( rules & VERTICAL ) {
                        if( bottom ) {
                            txt2.push_back( _document->entityChar( L"&bxas." ) );
                        } else {
                            txt2.push_back( _document->entityChar( L"&bxde." ) );
                        }
                    } else {
                        txt2.push_back( ch );
                    }
                }
            } else {
                txt1.push_back( L' ' );
                txt3.push_back( L' ' );
                if( rules & VERTICAL ) {
                    txt2.push_back( _document->entityChar( L"&bxv." ) );
                }
            }
        }
        catch( Class2Error& e ) {
            printError( e.code );
        }
        //left frame edge
        appendChild( new Word( _document, this, _document->dataName(),
            _document->lexerLine(), _document->lexerCol(), txt1) );
        //the columns
        if( !colWidth.empty() ) {
            for( count1 = 0; count1 < colWidth.size() - 1; ++count1 ) {
                std::wstring txt( colWidth[ count1 ], ch );
                appendChild( new Word( _document, this, _document->dataName(),
                    _document->lexerLine(), _document->lexerCol(), txt) );
                appendChild( new Word( _document, this, _document->dataName(),
                    _document->lexerLine(), _document->lexerCol(), txt2) );
            }
            std::wstring txt( colWidth[ count1 ], ch );
            appendChild( new Word( _document, this, _document->dataName(),
                _document->lexerLine(), _document->lexerCol(), txt) );
        }
        //right frame edge
        if( frame ) {
            appendChild( new Word( _document, this, _document->dataName(),
                _document->lexerLine(), _document->lexerCol(), txt3) );
        }
    }
    appendChild( new BrCmd( _document, this, _document->dataName(),
            _document->lexerLine(), _document->lexerCol() ) );
}
/***************************************************************************/
void Table::rowRule()
{
    if( rules || frame == BOX ) {
        std::wstring txt1;
        std::wstring txt2;
        std::wstring txt3;
        unsigned int count1( 0 );
        wchar_t      ch( L' ' );
        try {
            if( frame == BOX ) {
                if( rules & HORIZONTAL ) {
                    txt1.push_back( _document->entityChar( L"&bxlj." ) );
                    txt3.push_back( _document->entityChar( L"&bxrj." ) );
                } else {
                    txt1.push_back( _document->entityChar( L"&bxv." ) );
                    txt3.push_back( _document->entityChar( L"&bxv." ) );
                }
            } else {
                txt1.push_back( L' ' );
                txt3.push_back( L' ' );
            }
            if( rules & HORIZONTAL ) {
                ch = _document->entityChar( L"&bxh." );
                if( rules & VERTICAL ) {
                    txt2.push_back( _document->entityChar( L"&bxcr." ) );
                } else {
                    txt2.push_back( ch );
                }
            } else if( rules & VERTICAL ) {
                txt2.push_back( _document->entityChar( L"&bxv." ) );
            } else {
                txt2.push_back( L' ' );
            }
        }
        catch( Class2Error& e ) {
            printError( e.code );
        }
        //left frame edge
        appendChild( new Word( _document, this, _document->dataName(),
            _document->lexerLine(), _document->lexerCol(), txt1) );
        //the columns
        if( !colWidth.empty() ) {
            for( count1 = 0; count1 < colWidth.size() - 1; ++count1 ) {
                std::wstring txt( colWidth[ count1 ], ch );
                appendChild( new Word( _document, this, _document->dataName(),
                    _document->lexerLine(), _document->lexerCol(), txt) );
                appendChild( new Word( _document, this, _document->dataName(),
                    _document->lexerLine(), _document->lexerCol(), txt2) );
            }
            std::wstring txt( colWidth[ count1 ], ch );
            appendChild( new Word( _document, this, _document->dataName(),
                _document->lexerLine(), _document->lexerCol(), txt) );
        }
        //right frame edge
        appendChild( new Word( _document, this, _document->dataName(),
            _document->lexerLine(), _document->lexerCol(), txt3) );
    }
    appendChild( new BrCmd( _document, this, _document->dataName(),
            _document->lexerLine(), _document->lexerCol() ) );
}
/***************************************************************************/
void Table::buildText( Cell* cell )
{
    cell->addByte( 0xFF );  //esc
    cell->addByte( 0x02 );  //size
    cell->addByte( 0x0B );  //begin monospaced
    if( cell->textFull() ) {
        printError( ERR1_LARGEPAGE );
    }
}
/*****************************************************************************/
void ETable::buildText( Cell* cell )
{
    cell->addByte( 0xFF );  //esc
    cell->addByte( 0x02 );  //size
    cell->addByte( 0x0C );  //end monospaced
    if( cell->textFull() ) {
        printError( ERR1_LARGEPAGE );
    }
}
/*****************************************************************************/
TableRow::~TableRow()
{
    for( ColIter itr = columns.begin(); itr != columns.end(); ++itr ) {
        delete *itr;
    }
}
/*****************************************************************************/
Lexer::Token TableRow::parse( Lexer* lexer )
{
    Lexer::Token tok( parseAttributes( lexer ) );
    unsigned int colCount( 0 );
    unsigned int rows( 0 );
    std::wstring txt1( 1, frame == Table::BOX ? _document->entityChar( L"&bxv." ) : L' ' );
    std::wstring txt2( 1, rules & Table::VERTICAL ? _document->entityChar( L"&bxv." ) : L' ' );
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
                unsigned char width( 0 );
                if( colCount < colWidth.size() ) {
                    width = colWidth[ colCount ];
                } else {
                    _document->printError( ERR1_TABLECELLCOUNTHIGH );
                }
                TableCol* tablecol( new TableCol( _document, this, _document->dataName(),
                    _document->lexerLine(), _document->lexerCol(), width ) );
                columns.push_back( tablecol );
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
    if( colCount < colWidth.size() ) {    //check for unspecified columns
        _document->printError( ERR1_TABLECELLCOUNTLOW );
        while( colCount < colWidth.size() ) {
            unsigned char width( colWidth[ colCount ] );
            TableCol* tablecol( new TableCol( _document, this, _document->dataName(),
                _document->lexerLine(), _document->lexerCol(), width ) );
            columns.push_back( tablecol );
            ++colCount;
        }
    }
    for( unsigned int count1 = 0; count1 < rows; ++count1 ) {
        appendChild( new Word( _document, this, _document->dataName(),
            _document->lexerLine(), _document->lexerCol(), txt1) );
        for( unsigned int count2 = 0; count2 < colWidth.size(); ++count2 ) {
            if( count1 < columns[ count2 ]->rows() &&
                columns[ count2 ]->rowData( count1 ).size() ) {
                std::list< Element* >& data = columns[ count2 ]->rowData( count1 );
                for( ChildrenIter itr = data.begin(); itr != data.end(); ++itr ) {
                    appendChild( *itr );
                }
            } else {  //it's blank
                std::wstring blank( colWidth[ count2 ], L' ' );
                appendChild( new WhiteSpace( _document, this, _document->dataName(),
                    _document->lexerLine(), _document->lexerCol(), blank, _whiteSpace ) );
            }
            if( count2 < colWidth.size() - 1 ) {
                appendChild( new Word( _document, this, _document->dataName(),
                    _document->lexerLine(), _document->lexerCol(), txt2 ) );
            }
        }
        if( frame == Table::BOX )
            appendChild( new Word( _document, this, _document->dataName(),
                _document->lexerLine(), _document->lexerCol(), txt1) );
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
    std::size_t spaces( colWidth );
    unsigned int cellLine( 0 );
    unsigned int currentLine( _document->dataLine() );
    while( !doneF && tok != Lexer::END ) {
        if( tok == Lexer::WORD ) {
            if( inLines && currentLine < _document->dataLine() ) {
                if( spaces > 0 ) {
                    std::wstring txt( spaces, L' ' );
                    WhiteSpace* ws( new WhiteSpace( _document, this, _document->dataName(),
                        _document->lexerLine(), _document->lexerCol(), txt, _whiteSpace ) );
                    appendData( cellLine, ws );
                }
                currentLine = _document->dataLine();
                std::list< Element* > lst;
                data.push_back( lst );
                ++cellLine;
                spaces = colWidth;
            }
            bool nextIsPunct( false );
            std::wstring txt( lexer->text() );  //get text from lexer
            tok = _document->getNextToken();
            while( tok == Lexer::WORD || tok == Lexer::ENTITY ) {
                if( tok == Lexer::WORD ) {
                    txt += lexer->text();       //part of a compound ...-word-entity-word-...
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
                            txt += entityChar;
                        }
                        catch( Class2Error& e ) {
                            _document->printError( e.code );
                            break;
                        }
                    }
                }
                tok = _document->getNextToken();
            }
            std::size_t txtSize( txt.size() );
            if( inLines ) {
                if( txt.size() > spaces ) {
                    txt.erase( spaces );  //trim text
                    txtSize = spaces;
                    _document->printError( ERR1_TABLECELLTEXTWIDTH );
                }
                if( txtSize > 0 ) {
                    Word* word( new Word( _document, this, _document->dataName(),
                        _document->lexerLine(), _document->lexerCol(), txt ) );
                    appendData( cellLine, word );
                    spaces -= txtSize;
                }
            } else {
                if( txt.size() > colWidth ) {
                    txt.erase( colWidth );  //trim text
                    txtSize = colWidth;
                    _document->printError( ERR1_TABLECELLTEXTWIDTH );
                }
                Word* word( new Word( _document, this, _document->dataName(),
                    _document->lexerLine(), _document->lexerCol(), txt ) );
                if( txtSize < colWidth && ( nextIsPunct || tok == Lexer::PUNCTUATION ) )
                    ++txtSize;              //keep puctuation together with word if possible
                if( txtSize > spaces ) {    //need new line
                    if( spaces > 0 ) {
                        std::wstring spacetxt( spaces, L' ' );
                        WhiteSpace* ws( new WhiteSpace( _document, this, _document->dataName(),
                            _document->lexerLine(), _document->lexerCol(), spacetxt, _whiteSpace ) );
                        appendData( cellLine, ws );
                    }
                    std::list< Element* > lst;
                    data.push_back( lst );
                    ++cellLine;
                    spaces = colWidth;
                }
                appendData( cellLine, word );
                spaces -= txtSize;
                if( txtSize < colWidth && ( nextIsPunct || tok == Lexer::PUNCTUATION ) ) {
                    ++spaces;
                }
            }
        } else if( tok == Lexer::ENTITY ) {
            const std::wstring* nameittxt( _document->nameit( lexer->text() ) ); //lookup nameit
            if( nameittxt ) {
                std::wstring* name( _document->prepNameitName( lexer->text() ) );
                IpfBuffer* buffer( new IpfBuffer( name, _document->dataLine(), _document->dataCol(), *nameittxt ) );
                _document->pushInput( buffer );
                tok = _document->getNextToken();
                continue;
            }
            if( inLines && currentLine < _document->dataLine() ) {
                if( spaces > 0 ) {
                    std::wstring spacetxt( spaces, L' ' );
                    WhiteSpace* ws( new WhiteSpace( _document, this, _document->dataName(),
                        _document->lexerLine(), _document->lexerCol(), spacetxt, _whiteSpace ) );
                    appendData( cellLine, ws );
                }
                currentLine = _document->dataLine();
                std::list< Element* > lst;
                data.push_back( lst );
                ++cellLine;
                spaces = colWidth;
            }
            bool nextIsPunct( false );
            try {
                wchar_t entityChar( _document->entityChar( lexer->text() ) );    //lookup entity
                std::wstring entitytxt( 1, entityChar );
                tok = _document->getNextToken();
                if( !std::iswpunct( entityChar ) ) {
                    while( tok == Lexer::WORD || tok == Lexer::ENTITY ) {
                        if( tok == Lexer::WORD ) {
                            entitytxt += lexer->text();       //part of a compound ...-word-entity-word-...
                        } else if( tok == Lexer::ENTITY ) {
                            entityChar = _document->entityChar( lexer->text() );
                            if( std::iswpunct( entityChar ) ) {
                                nextIsPunct = true;
                                break;
                            }
                            entitytxt += entityChar;
                        }
                        tok = _document->getNextToken();
                    }
                }
                std::size_t txtSize( entitytxt.size() );
                if( inLines ) {
                    if( entitytxt.size() > spaces ) {
                        entitytxt.erase( spaces );  //trim text
                        txtSize = spaces;
                        _document->printError( ERR1_TABLECELLTEXTWIDTH );
                    }
                    if( txtSize > 0 ) {
                        Entity *entity = new Entity( _document, this, _document->dataName(),
                            _document->lexerLine(), _document->lexerCol(), entitytxt );
                        appendData( cellLine, entity );
                        spaces -= txtSize;
                    }
                } else {
                    if( entitytxt.size() > colWidth ) {
                        entitytxt.erase( colWidth );  //trim text
                        txtSize = colWidth;
                        _document->printError( ERR1_TABLECELLTEXTWIDTH );
                    }
                    Entity *entity = new Entity( _document, this, _document->dataName(),
                        _document->lexerLine(), _document->lexerCol(), entitytxt );
                    if( txtSize < colWidth && ( nextIsPunct || tok == Lexer::PUNCTUATION ) )
                        ++txtSize;              //keep text and punct on same line if possible
                    if( txtSize > spaces ) {    //need new line
                        if( spaces > 0 ) {
                            std::wstring spacetxt( spaces, L' ' );
                            WhiteSpace* ws( new WhiteSpace( _document, this, _document->dataName(),
                                _document->lexerLine(), _document->lexerCol(), spacetxt, _whiteSpace ) );
                            appendData( cellLine, ws );
                        }
                        std::list< Element* > lst;
                        data.push_back( lst );
                        ++cellLine;
                        spaces = colWidth;
                    }
                    appendData( cellLine, entity );
                    spaces -= txtSize;
                    if( txtSize < colWidth && ( nextIsPunct || tok == Lexer::PUNCTUATION ) ) {
                        ++spaces;
                    }
                }
            }
            catch( Class2Error& e ) {
                _document->printError( e.code );
                tok = _document->getNextToken();
            }
        } else if( tok == Lexer::PUNCTUATION ) {
            if( inLines ) {
                if( currentLine < _document->dataLine() ) {
                    currentLine = _document->dataLine();
                    if( spaces > 0 ) {
                        std::wstring spacetxt( spaces, L' ' );
                        WhiteSpace* ws( new WhiteSpace( _document, this, _document->dataName(),
                            _document->lexerLine(), _document->lexerCol(), spacetxt, _whiteSpace ) );
                        appendData( cellLine, ws );
                    }
                    std::list< Element* > lst;
                    data.push_back( lst );
                    ++cellLine;
                    spaces = colWidth;
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
                    data.push_back( lst );
                    ++cellLine;
                    spaces = colWidth;
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
                        std::wstring spacetxt( spaces, L' ' );
                        WhiteSpace* ws( new WhiteSpace( _document, this, _document->dataName(),
                            _document->lexerLine(), _document->lexerCol(), spacetxt, _whiteSpace ) );
                        appendData( cellLine, ws );
                    }
                    std::list< Element* > lst;
                    data.push_back( lst );
                    ++cellLine;
                    spaces = colWidth;
                }
                if( spaces > 0 )  {
                    std::wstring whitespacetxt( lexer->text() );
                    if( whitespacetxt.size() > spaces ) {
                        whitespacetxt.erase( spaces );        //trim text
                    }
                    WhiteSpace* ws( new WhiteSpace( _document, this, _document->dataName(),
                        _document->lexerLine(), _document->lexerCol(), whitespacetxt, _whiteSpace ) );
                    appendData( cellLine, ws );
                    spaces -= whitespacetxt.size();
                }
                tok = _document->getNextToken();
            } else {
                std::wstring whitespacetxt( lexer->text() );  //get text from lexer
                tok = _document->getNextToken();
                while( tok == Lexer::WHITESPACE ) { //accumulate whitespace
                    if( lexer->text()[0] != L'\n' ) //ignore \n's
                        whitespacetxt += lexer->text();
                    tok = _document->getNextToken();
                }
                if( spaces > 0 ) {
                    if( whitespacetxt.size() > spaces )
                        whitespacetxt.erase( spaces );            //trim text
                    WhiteSpace* ws( new WhiteSpace( _document, this, _document->dataName(),
                        _document->lexerLine(), _document->lexerCol(), whitespacetxt, _whiteSpace ) );
                    appendData( cellLine, ws );
                    spaces -= whitespacetxt.size();
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
                    std::wstring spacetxt( spaces, L' ' );
                    WhiteSpace* ws( new WhiteSpace( _document, this, _document->dataName(),
                        _document->lexerLine(), _document->lexerCol(), spacetxt, _whiteSpace ) );
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
