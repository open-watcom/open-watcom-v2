/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2009-2021 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  A heading tag
*   :h1-:h6
*       res=[0-9]+
*       id=[a-zA-z][a-zA-z0-9]*
*       name=[a-zA-z][a-zA-z0-9]*
*       global
*       tutorial=''
*       x=([0-9]+[c|x|p|%]) | (left|center|right)
*       y=([0-9]+[c|x|p|%]) | (top|center|bottom)
*       width=([0-9]+[c|x|p|%])
*       height=([0-9]+[c|x|p|%])
*       group=[0-9]+
*       viewport (force new window)
*       clear (close open window, force new)
*       titlebar=yes|sysmenu|minmax|both|none (default: both)
*       scroll=horizontal|vertical|both|none (default: both)
*       rules=border|sizeborder|none (default: sizeborder)
*       nosearch (do not return this heading, return parent)
*       noprint
*       hide (do not include in TOC)
*       toc=[1-6] (valid until eof or next occurance)
*       ctrlarea=page|none
*       ctrlrefid=
*   Cannot skip levels
*
****************************************************************************/


#include "wipfc.hpp"
#include <algorithm>
#include <cwctype>
#include "hn.hpp"
#include "cell.hpp"
#include "document.hpp"
#include "hide.hpp"
#include "i1.hpp"
#include "i2.hpp"
#include "icmd.hpp"
#include "isyn.hpp"
#include "page.hpp"
#include "tocref.hpp"
#include "util.hpp"
#include "outfile.hpp"


Hn::~Hn()
{
    if( _document->isInf() ) {
        delete _id;
        delete _name;
    }
}
/***************************************************************************/
Lexer::Token Hn::parse( Lexer* lexer )
{
    Lexer::Token tok( parseAttributes( lexer ) );
    if( tok == Lexer::WHITESPACE )  //current line is empty
        tok = _document->getNextToken();
    //get the rest of the line as the title
    unsigned int startLine( _document->dataLine() );
    std::wstring tmp;
    while(  _document->dataLine() == startLine ) {
        if( tok == Lexer::WHITESPACE ||
            tok == Lexer::WORD ||
            tok == Lexer::PUNCTUATION ) {
            tmp += lexer->text();
        } else if( tok == Lexer::ENTITY ) {
            const std::wstring* exp( _document->nameit( lexer->text() ) );
            if( exp ) {
                tmp += *exp;
            } else {
                try {
                    wchar_t entityChar( _document->entityChar( lexer->text() ) );
                    tmp += entityChar;
                }
                catch( Class2Error& e ) {
                    _document->printError( e._code );
                }
            }
        } else if( tok == Lexer::END ) {
            throw FatalError( ERR_EOF );
        }
        tok = _document->getNextToken();
    }
    _title = tmp;
    tok = _document->getNextToken();
    while( tok != Lexer::END && !( tok == Lexer::TAG && lexer->tagId() == Lexer::EUSERDOC ) ) {
        if( parseInline( lexer, tok ) ) {
            switch( lexer->tagId() ) {
            case Lexer::H1:
                return tok;
            case Lexer::H2:
                if( _toc.flags.s.nestLevel >= 2 )
                    return tok;;
                if( _toc.flags.s.nestLevel == 1 )
                    _toc.flags.s.hasChildren = 1;
                if( _document->headerCutOff() >= 2 ) {
                    Hn* h2( new Hn( _document, NULL, _document->dataName(),
                        _document->lexerLine(), _document->lexerCol(), 2 ) );
                    Page* pg( new Page( _document, h2 ) );
                    _document->addPage( pg );
                    tok = h2->parse( lexer );
                } else {
                    Hn* h2( new Hn( _document, this, _document->dataName(),
                        _document->lexerLine(), _document->lexerCol(), 2 ) );
                    appendChild( h2 );
                    tok = h2->parse( lexer );
                }
                break;
            case Lexer::H3:
                if( _toc.flags.s.nestLevel >= 3 )
                    return tok;
                if( _toc.flags.s.nestLevel == 2 )
                    _toc.flags.s.hasChildren = 1;
                if( _toc.flags.s.nestLevel < 2 )
                    _document->printError( ERR1_HEADNEST );
                if( _document->headerCutOff() >= 3 ) {
                    Hn* h3( new Hn( _document, NULL, _document->dataName(),
                        _document->lexerLine(), _document->lexerCol(), 3 ) );
                    Page* pg( new Page( _document, h3 ) );
                    _document->addPage( pg );
                    tok = h3->parse( lexer );
                } else {
                    Hn* h3( new Hn( _document, this, _document->dataName(),
                        _document->lexerLine(), _document->lexerCol(), 3 ) );
                    appendChild( h3 );
                    tok = h3->parse( lexer );
                }
                break;
            case Lexer::H4:
                if( _toc.flags.s.nestLevel >= 4 )
                    return tok;
                if( _toc.flags.s.nestLevel == 3 )
                    _toc.flags.s.hasChildren = 1;
                if( _toc.flags.s.nestLevel < 3 )
                    _document->printError( ERR1_HEADNEST );
                if( _document->headerCutOff() >= 4 ) {
                    Hn* h4( new Hn( _document, NULL, _document->dataName(),
                        _document->lexerLine(), _document->lexerCol(), 4 ) );
                    Page* pg( new Page( _document, h4 ) );
                    _document->addPage( pg );
                    tok = h4->parse( lexer );
                } else {
                    Hn* h4( new Hn( _document, this, _document->dataName(),
                        _document->lexerLine(), _document->lexerCol(), 4 ) );
                    appendChild( h4 );
                    tok = h4->parse( lexer );
                }
                break;
            case Lexer::H5:
                if( _toc.flags.s.nestLevel >= 5 )
                    return tok;
                if( _toc.flags.s.nestLevel == 4 )
                    _toc.flags.s.hasChildren = 1;
                if( _toc.flags.s.nestLevel < 4 )
                    _document->printError( ERR1_HEADNEST );
                if( _document->headerCutOff() >= 5 ) {
                    Hn* h5( new Hn( _document, NULL, _document->dataName(),
                        _document->lexerLine(), _document->lexerCol(), 5 ) );
                    Page* pg( new Page( _document, h5 ) );
                    _document->addPage( pg );
                    tok = h5->parse( lexer );
                } else {
                    Hn* h5( new Hn( _document, this, _document->dataName(),
                        _document->lexerLine(), _document->lexerCol(), 5 ) );
                    appendChild( h5 );
                    tok = h5->parse( lexer );
                }
                break;
            case Lexer::H6:
                if( _toc.flags.s.nestLevel >= 6 )
                    return tok;
                if( _toc.flags.s.nestLevel == 5 )
                    _toc.flags.s.hasChildren = 1;
                if( _toc.flags.s.nestLevel < 5 )
                    _document->printError( ERR1_HEADNEST );
                if( _document->headerCutOff() >= 6 ) {
                    Hn* h6( new Hn( _document, NULL, _document->dataName(),
                        _document->lexerLine(), _document->lexerCol(), 6 ) );
                    Page* pg( new Page( _document, h6 ) );
                    _document->addPage( pg );
                    tok = h6->parse( lexer );
                } else {
                    Hn* h6( new Hn( _document, this, _document->dataName(),
                        _document->lexerLine(), _document->lexerCol(), 6 ) );
                    appendChild( h6 );
                    tok = h6->parse( lexer );
                }
                break;
            case Lexer::FN:
                return tok;
            case Lexer::I1:
                {
                    I1* i1( new I1( _document, NULL, _document->dataName(),
                        _document->lexerLine(), _document->lexerCol() ) );
                    if( _res ) {
                        i1->setRes( _res );
                    } else if( _id ) {
                        i1->setIdOrName( _id );
                    } else if( _name ) {
                        i1->setIdOrName( _name );
                    }
                    appendChild( i1 );
                    tok = i1->parse( lexer );
                }
                break;
            case Lexer::I2:
                {
                    I2* i2( new I2( _document, NULL, _document->dataName(),
                        _document->lexerLine(), _document->lexerCol() ) );
                    if( _res ) {
                        i2->setRes( _res );
                    } else if( _id ) {
                        i2->setIdOrName( _id );
                    } else if( _name ) {
                        i2->setIdOrName( _name );
                    }
                    appendChild( i2 );
                    tok = i2->parse( lexer );
                }
                break;
            case Lexer::ICMD:
                {
                    ICmd* icmd( new ICmd( _document, NULL, _document->dataName(),
                        _document->lexerLine(), _document->lexerCol() ) );
                    if( _res ) {
                        icmd->setRes( _res );
                    } else if( _id ) {
                        icmd->setIdOrName( _id );
                    } else if( _name ) {
                        icmd->setIdOrName( _name );
                    }
                    appendChild( icmd );
                    tok = icmd->parse( lexer );
                }
                break;
            case Lexer::ISYN:
                {
                    ISyn *isyn = new ISyn( _document, NULL, _document->dataName(),
                        _document->lexerLine(), _document->lexerCol() );
                    appendChild( isyn );
                    tok = isyn->parse( lexer );
                }
                break;
            default:
                if( parseBlock( lexer, tok ) ) {
                    if( parseListBlock( lexer, tok ) ) {
                        parseCleanup( lexer, tok );
                    }
                }
            }
        }
    }
    return tok;
}
/***************************************************************************/
Lexer::Token Hn::parseAttributes( Lexer* lexer )
{
    Lexer::Token tok;
    bool xorg( false );
    bool yorg( false );
    bool dx( false );
    bool dy( false );

    while( (tok = _document->getNextToken()) != Lexer::TAGEND ) {
        //parse attributes
        if( tok == Lexer::ATTRIBUTE ) {
            std::wstring key;
            std::wstring value;
            splitAttribute( lexer->text(), key, value );
            if( key == L"res" ) {
                _res = static_cast< word >( std::wcstoul( value.c_str(), 0, 10 ) );
                if( _res < 1 || _res > 64000 )
                    _document->printError( ERR2_VALUE );
                if( Hide::hiding() ) {
                    _document->printError( ERR1_HIDERES );
                }
            } else if( key == L"id" ) {
                _id = new GlobalDictionaryWord( value );
                _id->toUpper();              //convert to upper case
                if( !_document->isInf() ) {
                    _id = _document->addTextToGD( _id );
                }
            } else if( key == L"name" ) {
                _name = new GlobalDictionaryWord( value );
                _name->toUpper();            //convert to upper case
                if( !_document->isInf() ) {
                    _name = _document->addTextToGD( _name );
                }
            } else if( key == L"tutorial" ) {
                _toc.flags.s.extended = 1;
                _etoc.flags.s.setTutor = 1;
                _tutorial = value;
            } else if( key == L"x" ) {
                xorg = true;
                _toc.flags.s.extended = 1;
                if( value == L"left" ) {
                    _origin.xPosType = ExtTocEntry::DYNAMIC;
                    _origin.xpos = ExtTocEntry::DYNAMIC_LEFT;
                } else if( value == L"center" ) {
                    _origin.xPosType = ExtTocEntry::DYNAMIC;
                    _origin.xpos = ExtTocEntry::DYNAMIC_CENTER;
                } else if( value == L"right" ) {
                    _origin.xPosType = ExtTocEntry::DYNAMIC;
                    _origin.xpos = ExtTocEntry::DYNAMIC_RIGHT;
                } else if( value == L"top" || value == L"bottom" ) {
                    _document->printError( ERR2_VALUE );
                } else {
                    wchar_t *end;
                    unsigned long x( std::wcstoul( value.c_str(), &end, 10 ) );
                    _origin.xpos = static_cast< word >( x );
                    if( *end == L'c' ) {
                        _origin.xPosType = ExtTocEntry::ABSOLUTE_CHAR;
                    } else if( *end == L'%' ) {
                        _origin.xPosType = ExtTocEntry::RELATIVE_PERCENT;
                    } else if( *end == L'x' ) {
                        _origin.xPosType = ExtTocEntry::ABSOLUTE_PIXEL;
                    } else if( *end == L'p' ) {
                        _origin.xPosType = ExtTocEntry::ABSOLUTE_POINTS;
                    } else {
                        _document->printError( ERR2_VALUE );
                    }
                }
                if( dx && _origin.xPosType == ExtTocEntry::DYNAMIC && _size.widthType != ExtTocEntry::RELATIVE_PERCENT ) {
                    _document->printError( ERR3_MIXEDUNITS );
                }
            } else if( key == L"y" ) {
                yorg = true;
                _toc.flags.s.extended = 1;
                if( value == L"top" ) {
                    _origin.yPosType = ExtTocEntry::DYNAMIC;
                    _origin.ypos = ExtTocEntry::DYNAMIC_TOP;
                } else if( value == L"center" ) {
                    _origin.yPosType = ExtTocEntry::DYNAMIC;
                    _origin.ypos = ExtTocEntry::DYNAMIC_CENTER;
                } else if( value == L"bottom" ) {
                    _origin.yPosType = ExtTocEntry::DYNAMIC;
                    _origin.ypos = ExtTocEntry::DYNAMIC_BOTTOM;
                } else if( value == L"left" || value == L"right" ) {
                    _document->printError( ERR2_VALUE );
                } else {
                    wchar_t *end;
                    unsigned long y( std::wcstoul( value.c_str(), &end, 10 ) );
                    _origin.ypos = static_cast< word >( y );
                    if( *end == L'c' ) {
                        _origin.yPosType = ExtTocEntry::ABSOLUTE_CHAR;
                    } else if( *end == L'%' ) {
                        _origin.yPosType = ExtTocEntry::RELATIVE_PERCENT;
                    } else if( *end == L'x' ) {
                        _origin.yPosType = ExtTocEntry::ABSOLUTE_PIXEL;
                    } else if( *end == L'p' ) {
                        _origin.yPosType = ExtTocEntry::ABSOLUTE_POINTS;
                    } else {
                        _document->printError( ERR2_VALUE );
                    }
                }
                if( dy && _origin.yPosType == ExtTocEntry::DYNAMIC && _size.heightType != ExtTocEntry::RELATIVE_PERCENT ) {
                    _document->printError( ERR3_MIXEDUNITS );
                }
            } else if( key == L"width" ) {
                dx = true;
                _toc.flags.s.extended = 1;
                if( value == L"left" ||
                    value == L"center" ||
                    value == L"right" ||
                    value == L"top" ||
                    value == L"bottom" ) {
                    _document->printError( ERR2_VALUE );
                } else {
                    wchar_t *end;
                    unsigned long width = std::wcstoul( value.c_str(), &end, 10 );
                    _size.width = static_cast< word >( width );
                    if( *end == L'c' ) {
                        _size.widthType = ExtTocEntry::ABSOLUTE_CHAR;
                    } else if( *end == L'%' ) {
                        _size.widthType = ExtTocEntry::RELATIVE_PERCENT;
                    } else if( *end == L'x' ) {
                        _size.widthType = ExtTocEntry::ABSOLUTE_PIXEL;
                    } else if( *end == L'p' ) {
                        _size.widthType = ExtTocEntry::ABSOLUTE_POINTS;
                    } else {
                        _document->printError( ERR2_VALUE );
                    }
                }
                if( xorg && _origin.xPosType == ExtTocEntry::DYNAMIC && _size.widthType != ExtTocEntry::RELATIVE_PERCENT ) {
                    _document->printError( ERR3_MIXEDUNITS );
                }
            } else if( key == L"height" ) {
                dy = true;
                _toc.flags.s.extended = 1;
                if( value == L"left" ||
                    value == L"center" ||
                    value == L"right" ||
                    value == L"top" ||
                    value == L"bottom" ) {
                    _document->printError( ERR2_VALUE );
                } else {
                    wchar_t *end;
                    unsigned long height = std::wcstoul( value.c_str(), &end, 10 );
                    _size.height = static_cast< word >( height );
                    if( *end == L'c' ) {
                        _size.heightType = ExtTocEntry::ABSOLUTE_CHAR;
                    } else if( *end == L'%' ) {
                        _size.heightType = ExtTocEntry::RELATIVE_PERCENT;
                    } else if( *end == L'x' ) {
                        _size.heightType = ExtTocEntry::ABSOLUTE_PIXEL;
                    } else if( *end == L'p' ) {
                        _size.heightType = ExtTocEntry::ABSOLUTE_POINTS;
                    } else {
                        _document->printError( ERR2_VALUE );
                    }
                }
                if( yorg && _origin.yPosType == ExtTocEntry::DYNAMIC && _size.heightType != ExtTocEntry::RELATIVE_PERCENT ) {
                    _document->printError( ERR3_MIXEDUNITS );
                }
            } else if( key == L"group" ) {
                _toc.flags.s.extended = 1;
                _group.id = static_cast< word >( std::wcstoul( value.c_str(), 0, 10 ) );
            } else if( key == L"titlebar" ) {
                _toc.flags.s.extended = 1;
                if( value == L"yes" ) {
                    _style.attrs |= PageStyle::TITLEBAR;
                } else if( value == L"sysmenu" ) {
                    _style.attrs |= PageStyle::TITLEBAR;
                    _style.attrs |= PageStyle::SYSMENU;
                } else if( value == L"minmax" ) {
                    _style.attrs |= PageStyle::TITLEBAR;
                    _style.attrs |= PageStyle::MINMAX;
                } else if( value == L"both" ) {
                    _style.attrs |= PageStyle::TITLEBAR;
                    _style.attrs |= PageStyle::SYSMENU;
                    _style.attrs |= PageStyle::MINMAX;
                } else if( value != L"none" ) {
                    _document->printError( ERR2_VALUE );
                }
            } else if( key == L"scroll" ) {
                _toc.flags.s.extended = 1;
                if( value == L"horizontal" ) {
                    _style.attrs |= PageStyle::HSCROLL;
                } else if( value == L"vertical" ) {
                    _style.attrs |= PageStyle::VSCROLL;
                } else if( value == L"both" ) {
                    _style.attrs |= PageStyle::HSCROLL;
                    _style.attrs |= PageStyle::VSCROLL;
                } else if( value != L"none" ) {
                    _document->printError( ERR2_VALUE );
                }
            } else if( key == L"rules" ) {
                _toc.flags.s.extended = 1;
                if( value == L"border" ) {
                    _style.attrs |= PageStyle::BORDER;
                } else if( value == L"sizeborder" ) {
                    _style.attrs |= PageStyle::SIZEBORDER;
                } else if( value != L"none" ) {
                    _document->printError( ERR2_VALUE );
                }
            } else if( key == L"toc" ) {
                wchar_t ch[2];
                ch[0] = value[value.size() - 1];    //last number is critical value
                ch[1] = L'\0';
                int tmp( static_cast< int >( std::wcstol( ch, 0, 10 ) ) );
                if( tmp < 1 || tmp > 6 ) {
                    _document->printError( ERR2_VALUE );
                } else {
                    _document->setHeaderCutOff( static_cast< unsigned int >( tmp ) );
                }
            } else if( key == L"ctrlarea" ) {
                if( value == L"page" ) {
                    _toc.flags.s.extended = 1;
                    _etoc.flags.s.setCtrl = 1;
                } else {
                    _etoc.flags.s.setCtrl = 0;
                }
            } else if( key == L"ctrlrefid" ) {
                _toc.flags.s.extended = 1;
                std::transform( value.begin(), value.end(), value.begin(), std::towupper );
                _control.refid = _document->getGroupIndexById( value );
            } else {
                _document->printError( ERR1_ATTRNOTDEF );
            }
        } else if( tok == Lexer::FLAG ) {
            if( lexer->text() == L"global" ) {
                _global = true;
            } else if( lexer->text() == L"viewport" ) {
                _toc.flags.s.extended = 1;
                _etoc.flags.s.setView = 1;
            } else if( lexer->text() == L"clear" ) {
                _toc.flags.s.extended = 1;
                _etoc.flags.s.clear = 1;
            } else if( lexer->text() == L"nosearch" ) {
                _toc.flags.s.extended = 1;
                _etoc.flags.s.noSearch = 1;
            } else if( lexer->text() == L"noprint" ) {
                _toc.flags.s.extended = 1;
                _etoc.flags.s.noPrint = 1;
            } else if( lexer->text() == L"hide" ) {
                _toc.flags.s.hidden = 1;
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
    return _document->getNextToken(); //consume TAGEND
}
/***************************************************************************/
void Hn::buildTOC( Page* page )
{
    if( !_parent ) {     //root element of a page
        page->setChildren( _childTOCs );
        page->setTitle( _title );
        page->setTOC( _toc );
        page->setETOC( _etoc );
        page->setOrigin( _origin );
        page->setSize( _size );
        page->setStyle( _style );
        page->setGroup( _group );
        page->SetControl( _control );
        //FIXME: need the index of the parent?
        //page->setSearchable( !nosearch );
        TocRef tr( _fileName, _row, page->index() );
        if( _res || !_document->isInf() ) {
            try {
                _document->addRes( _res, tr );
            }
            catch ( Class3Error& e ) {
                printError( e._code );
            }
        }
        if( _id ) {
            try {
                _document->addNameOrId( _id, tr );
            }
            catch( Class3Error& e ) {
                printError( e._code );
            }
        }
        if( _name ) {
            try {
                _document->addNameOrId( _name, tr );
            }
            catch( Class3Error& e ) {
                printError( e._code );
            }
        }
        if( _global && !_document->isInf() ) {
            if( _id ) {
                try {
                    _document->addGNameOrId( _id, page->index() );
                }
                catch( Class3Error& e ) {
                    printError( e._code );
                }
            }
            if( _name ) {
                try {
                    _document->addGNameOrId( _name, page->index() );
                }
                catch( Class3Error& e ) {
                    printError( e._code );
                }
            }
        }
    }
}
/***************************************************************************/
void Hn::buildText( Cell* cell )
{
    if( _etoc.flags.s.setTutor ) {
        std::string buffer( cell->out()->wtomb_string( _tutorial ) );
        if( buffer.size() > ( 255 - 2 ) )
            buffer.erase( 255 - 2 );
        std::size_t start( cell->getPos() );
        cell->reserve( buffer.size() + 3 );
        cell->addByte( Cell::ESCAPE );  //esc
        cell->addByte( 0x02 );          //size
        cell->addByte( 0x15 );          //begin hide
        cell->add( buffer );
        cell->updateByte( start + 1, static_cast< byte >( cell->getPos( start ) - 1 ) );
        if( cell->textFull() ) {
            printError( ERR1_LARGEPAGE );
        }
    }
}
/***************************************************************************/
void Hn::linearize( Page* page )
{
    if( _document->headerCutOff() < _toc.flags.s.nestLevel )
        page->addElement( this );
    for( ConstChildrenIter iter = _children.begin(); iter != _children.end(); ++iter ) {
        ( *iter )->linearize( page );
    }
}
