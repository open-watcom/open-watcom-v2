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
* Description:  link and elink tags
*   :link / :elink
*       reftype=hd (to header, requires refid, may use database)
*               fn (to footnote, requires refid)
*               launch (exec program, requires object and data attributes)
*               inform (send a message, requires res and not :elink)
*       res=[0-9]+
*       refid=[a-zA-z][a-zA-z0-9]*
*       database='' (name of external ipf/hlp file)
*       object='' (name of executable)
*       data='' (parameters to pass to executable)
*       auto (automatically open this link)
*       viewport
*       dependent
*       split
*       child
*       group=[0-9]+
*       vpx=([0-9]+[c|x|p|%]) | (left|center|right|top|bottom)
*       vpy=([0-9]+[c|x|p|%]) | (left|center|right|top|bottom)
*       vpcx=([0-9]+[c|x|p|%])
*       vpcy=([0-9]+[c|x|p|%])
*       titlebar=yes|sysmenu|minmax|both|none (default: both)
*       scroll=horizontal|vertical|both|none (default: both)
*       rules=border|sizeborder|none (default: sizeborder)
*       x=[0-9]+
*       y=[0-9]+
*       cx=[0-9]+
*       cy=[0-9]+
*   Terminated by :elink unless it is a child of :artlink, or type == inform
*
****************************************************************************/


#include "wipfc.hpp"
#include "link.hpp"
#include "brcmd.hpp"
#include "cell.hpp"
#include "document.hpp"
#include "entity.hpp"
#include "errors.hpp"
#include "gdword.hpp"
#include "hn.hpp"
#include "punct.hpp"
#include "util.hpp"
#include "whtspc.hpp"
#include "word.hpp"
#include "outfile.hpp"


Link::~Link()
{
    if( _document->isInf() ) {
        delete _refid;
    }
}
/***************************************************************************/
Lexer::Token Link::parse( Lexer* lexer )
{
    Lexer::Token tok( parseAttributes( lexer ) );
    if( !_noElink ) {
        while( tok != Lexer::END && !( tok == Lexer::TAG && lexer->tagId() == Lexer::EUSERDOC) ) {
            //inline except: elink, artlink, eartlink, artwork, hdref
            if( tok == Lexer::TAG ) {
                if( lexer->tagId() == Lexer::ELINK ) {
                    break;
                } else if( lexer->tagId() == Lexer::ARTLINK ||
                         lexer->tagId() == Lexer::EARTLINK ||
                         lexer->tagId() == Lexer::ARTWORK ||
                         lexer->tagId() == Lexer::HDREF ) {
                    parseCleanup( lexer, tok );
                } else if( parseInline( lexer, tok ) ) {
                    parseCleanup( lexer, tok );
                }
            } else if( parseInline( lexer, tok ) ) {
                parseCleanup( lexer, tok );
            }
        }
    }
    return tok;
}
/***************************************************************************/
Lexer::Token Link::parseAttributes( Lexer* lexer )
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
            if( key == L"reftype" ) {
                if( value == L"hd" ) {
                    _type = TOPIC;
                } else if( value == L"fn" ) {
                    Hn* root( static_cast< Hn* >( rootElement() ) );
                    if( root->isSplit() )
                        _document->printError( ERR3_FNNOSPLIT );
                    _type = FOOTNOTE;
                } else if( value == L"launch" ) {
                    _type = LAUNCH;
                } else if( value == L"inform" ) {
                    _type = INFORM;
                    _noElink = true;
                } else {
                    _document->printError( ERR2_VALUE );
                }
            } else if( key == L"res" ) {
                _res = static_cast< word >( std::wcstoul( value.c_str(), 0, 10 ) );
            } else if( key == L"refid" ) {
                _refid = new GlobalDictionaryWord( value );
                _refid->toUpper();           //to uppercase
                if( !_document->isInf() ) {
                    _refid = _document->addTextToGD( _refid );
                }
            } else if( key == L"database" ) {
                _database = value;
                try {
                    _document->addExtFile( value );
                }
                catch( Class1Error& e ) {
                    _document->printError( e._code );
                }
            } else if( key == L"object" ) {
                _object = value;
            } else if( key == L"data" ) {
                _data = value;
            } else if( key == L"group" ) {
                _group.id = static_cast< word >( std::wcstoul( value.c_str(), 0, 10 ) );
                _doGroup = true;
            } else if( key == L"vpx" ) {
                _doOrigin = true;
                xorg = true;
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
                    unsigned long xpos( std::wcstoul( value.c_str(), &end, 10 ) );
                    _origin.xpos = static_cast< word >( xpos );
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
            } else if( key == L"vpy" ) {
                _doOrigin = true;
                yorg = true;
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
                    unsigned long ypos( std::wcstoul( value.c_str(), &end, 10 ) );
                    _origin.ypos = static_cast< word >( ypos );
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
            } else if( key == L"vpcx" ) {
                _doSize = true;
                dx = true;
                if( value == L"left" ||
                    value == L"center" ||
                    value == L"right" ||
                    value == L"top" ||
                    value == L"bottom" ) {
                    _document->printError( ERR2_VALUE );
                } else {
                    wchar_t *end;
                    unsigned long width( std::wcstoul( value.c_str(), &end, 10 ) );
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
            } else if( key == L"vpcy" ) {
                _doSize = true;
                dy = true;
                if( value == L"left" ||
                    value == L"center" ||
                    value == L"right" ||
                    value == L"top" ||
                    value == L"bottom" ) {
                    _document->printError( ERR2_VALUE );
                } else {
                    wchar_t *end;
                    unsigned long height( std::wcstoul( value.c_str(), &end, 10 ) );
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
            } else if( key == L"x" ) {
                _hypergraphic = true;
                _hspot.x = static_cast< word >( std::wcstoul( value.c_str(), 0, 10 ) );
            } else if( key == L"y" ) {
                _hypergraphic = true;
                _hspot.y = static_cast< word >( std::wcstoul( value.c_str(), 0, 10 ) );
            } else if( key == L"cx" ) {
                _hypergraphic = true;
                _hspot.cx = static_cast< word >( std::wcstoul( value.c_str(), 0, 10 ) );
            } else if( key == L"cy" ) {
                _hypergraphic = true;
                _hspot.cy = static_cast< word >( std::wcstoul( value.c_str(), 0, 10 ) );
            } else if( key == L"titlebar" ) {
                _doStyle = true;
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
                _doStyle = true;
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
                _doStyle = true;
                if( value == L"border" ) {
                    _style.attrs |= PageStyle::BORDER;
                } else if( value == L"sizeborder" ) {
                    _style.attrs |= PageStyle::SIZEBORDER;
                } else if( value != L"none" ) {
                    _document->printError( ERR2_VALUE );
                }
            } else {
                _document->printError( ERR1_ATTRNOTDEF );
            }
        } else if( tok == Lexer::FLAG ) {
            if( lexer->text() == L"auto" ) {
                if( _type == FOOTNOTE ) {
                    _document->printError( ERR3_FNNOAUTO );
                } else {
                    _automatic = true;
                    _noElink = true;
                }
            } else if( lexer->text() == L"viewport" ) {
                _viewport = true;
            } else if( lexer->text() == L"dependent" ) {
                _dependent = true;
            } else if( lexer->text() == L"split" ) {
                _split = true;
            } else if( lexer->text() == L"child" ) {
                _child = true;
            } else {
                _document->printError( ERR1_ATTRNOTDEF );
            }
        } else if( tok == Lexer::ERROR_TAG ) {
            throw FatalError( ERR_SYNTAX );
        } else if( tok == Lexer::END ) {
            throw FatalError( ERR_EOF );
        }
    }
    return _document->getNextToken();    //consume TAGEND
}
/***************************************************************************/
void Link::buildText( Cell* cell )
{
    switch( _type ) {
    case TOPIC:
        doTopic( cell );
        break;
    case FOOTNOTE:
        doFootnote( cell );
        break;
    case LAUNCH:
        doLaunch( cell );
        break;
    case INFORM:
        doInform( cell );
        break;
    default:
        break;
    }
}
/***************************************************************************/
void Link::doTopic( Cell* cell )
{
    if( _refid || _res ) {                  //either refid or res is required
        if( _database.empty() ) {            //jump to internal link
            try {
                XRef xref( _fileName, _row );
                word tocIndex;
                if( _refid ) {
                    tocIndex = _document->tocIndexById( _refid );
                    _document->addXRef( _refid, xref );
                } else {
                    tocIndex = _document->tocIndexByRes( _res );
                    _document->addXRef( _res, xref );
                }
                std::size_t start( cell->getPos() );
                cell->reserve( 7 + _hspot.size( _hypergraphic ) + _origin.size() + _size.size() +
                    _style.size() + _group.size() );
                cell->addByte( Cell::ESCAPE );  //ESC
                cell->addByte( 4 );             //size
                if( !_hypergraphic ) {
                    cell->addByte( 0x05 );      //text link
                } else {
                    if( _hspot.isDef( true ) ) {
                        cell->addByte( 0x01 );  //partial bitmap
                    } else {
                        cell->addByte( 0x04 );  //full bitmap
                    }
                }
                cell->add( tocIndex );
                //this may need to be last
                if( _hspot.isDef( _hypergraphic ) )
                    cell->add( _hspot );
                if( _viewport || _doStyle || _automatic || _split || _doOrigin || _doSize ||
                    _dependent || _doGroup ) {
                    byte flag1( 0 );
                    byte flag2( 0 );
                    if( _doOrigin )
                        flag1 |= 0x01;
                    if( _doSize )
                        flag1 |= 0x02;
                    if( _viewport )
                        flag1 |= 0x04;
                    if( _doStyle )
                        flag1 |= 0x08;
                    if( _automatic )
                        flag1 |= 0x40;
                    if( _split ) {
                        flag1 |= 0x80;
                        Hn* root( static_cast< Hn* >( rootElement() ) );
                        root->setIsParent();
                        root->addChild( tocIndex );
                    }
                    if( _dependent )
                        flag2 |= 0x02;
                    if( _doGroup )
                        flag2 |= 0x04;
                    cell->add( flag1 );
                    cell->add( flag2 );
                }
                if( _doOrigin ) {
                    _origin.buildText( cell );
                }
                if( _doSize ) {
                    _size.buildText( cell );
                }
                if( _doStyle ) {
                    _style.buildText( cell );
                }
                if( _doGroup ) {
                    _group.buildText( cell );
                }
                cell->updateByte( start + 1, static_cast< byte >( cell->getPos( start ) - 1 ) );
                if( cell->textFull() ) {
                    printError( ERR1_LARGEPAGE );
                }
            }
            catch( Class1Error& e ) {
                printError( e._code );
            }
        } else {                              //jump to external link
            std::size_t start( cell->getPos() );
            cell->reserve( 7 +  _hspot.size( _hypergraphic ) );
            cell->addByte( Cell::ESCAPE );  //ESC
            cell->addByte( 4 );             //size
            if( !_hypergraphic ) {
                cell->addByte( 0x1F );      //text link
            } else {
                cell->addByte( 0x0F );      //hypergraphic link
                if( _hspot.isDef( true ) ) {
                    cell->addByte( 0x17 );  //partial bitmap
                } else {
                    cell->addByte( 0x16 );  //full bitmap
                }
            }
            cell->add( _document->extFileIndex( _database ) );
            std::string buffer( cell->out()->wtomb_string( _refid->getText() ) );
            if( buffer.size() > 255 - (( cell->getPos( start ) + _hspot.size( _hypergraphic ) + 1 ) - 1 ) )
                buffer.erase( 255 - (( cell->getPos( start ) + _hspot.size( _hypergraphic ) + 1 ) - 1 ) );
            cell->add( static_cast< byte >( buffer.size() ) );
            if( _hspot.isDef( _hypergraphic ) )
                cell->add( _hspot );
            cell->add( buffer );
            cell->updateByte( start + 1, static_cast< byte >( cell->getPos( start ) - 1 ) );
            if( cell->textFull() ) {
                printError( ERR1_LARGEPAGE );
            }
        }
    } else {
        printError( ERR1_NOREFID );
    }
}
/***************************************************************************/
void Link::doFootnote( Cell* cell )
{
    if( _refid || _res ) {                  //refid is required
        try {
            XRef xref( _fileName, _row );
            word tocIndex;
            if( _refid ) {
                tocIndex = _document->tocIndexById( _refid );
                _document->addXRef( _refid, xref );
            } else {
                tocIndex = _document->tocIndexByRes( _res );
                _document->addXRef( _res, xref );
            }
            std::size_t start( cell->getPos() );
            cell->reserve( 5 + _hspot.size( _hypergraphic ) );
            cell->addByte( Cell::ESCAPE );  //ESC
            cell->addByte( 4 );             //size
            if( !_hypergraphic ) {
                cell->addByte( 0x07 );      //text link
            } else {
                if( _hspot.isDef( true ) ) {
                    cell->addByte( 0x02 );  //partial bitmap
                } else {
                    cell->addByte( 0x05 );  //full bitmap
                }
            }
            cell->add( tocIndex );
            if( _hspot.isDef( _hypergraphic ) )
                cell->add( _hspot );
            cell->updateByte( start + 1, static_cast< byte >( cell->getPos( start ) - 1 ) );
            if( cell->textFull() ) {
                printError( ERR1_LARGEPAGE );
            }
        }
        catch( Class1Error& e ) {
            printError( e._code );
        }
    } else {
        printError( ERR1_NOREFID );
    }
}
/***************************************************************************/
void Link::doLaunch( Cell* cell )
{
    if( _object.size() && _data.size() ) {  //both are required
        std::size_t start( cell->getPos() );
        cell->reserve( 6 + _hspot.size( _hypergraphic ) );
        cell->addByte( Cell::ESCAPE );  //ESC
        cell->addByte( 3 );             //size
        if( !_hypergraphic ) {
            cell->addByte( 0x10 );      //text link
        } else {
            cell->addByte( 0x0F );      //hypergraphic link
            if( _hspot.isDef( true ) ) {
                cell->addByte( 0x08 );  //partial bitmap
            } else {
                cell->addByte( 0x07 );  //full bitmap
            }
        }
        cell->addByte( 0x00 );          //blank byte
        if( _hspot.isDef( _hypergraphic ) )
            cell->add( _hspot );
        std::string buffer( cell->out()->wtomb_string( _object ) );
        buffer += ' ';
        std::string tmp( cell->out()->wtomb_string( _data ) );
        buffer += tmp;
        if( buffer.size() > 255 - cell->getPos( start ) + 1 )
            buffer.erase( 255 - cell->getPos( start ) + 1 );
        cell->add( buffer );
        cell->updateByte( start + 1, static_cast< byte >( cell->getPos( start ) - 1 ) );
        if( cell->textFull() ) {
            printError( ERR1_LARGEPAGE );
        }
    } else {
        printError( ERR2_VALUE );
    }
}
/***************************************************************************/
void Link::doInform( Cell* cell )
{
   if( _res ) {                         //res is required
        std::size_t start( cell->getPos() );
        cell->reserve( 6 + _hspot.size( _hypergraphic ) );
        cell->addByte( Cell::ESCAPE );  //ESC
        cell->addByte( 4 );             //size
        if( !_hypergraphic ) {
            cell->addByte( 0x16 );      //text link
        } else {
            cell->addByte( 0x0F );      //hypergraphic link
            if( _hspot.isDef( true ) ) {
                cell->addByte( 0x10 );  //partial bitmap
            } else {
                cell->addByte( 0x09 );  //full bitmap
            }
        }
        cell->add( _res );
        if( _hspot.isDef( _hypergraphic ) )
            cell->add( _hspot );
        cell->updateByte( start + 1, static_cast< byte >( cell->getPos( start ) - 1 ) );
        if( cell->textFull() ) {
            printError( ERR1_LARGEPAGE );
        }
    } else {
        printError( ERR2_VALUE );
    }
}
/***************************************************************************/
void ELink::buildText( Cell* cell )
{
    cell->addByte( Cell::ESCAPE );
    cell->addByte( 0x02 );
    cell->addByte( 0x08 );
    if( cell->textFull() ) {
        printError( ERR1_LARGEPAGE );
    }
}
