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

Hn::~Hn()
{
    if( document->isInf() ) {
        delete id;
        delete name;
    }
}
/***************************************************************************/
Lexer::Token Hn::parse( Lexer* lexer )
{
    Lexer::Token tok( parseAttributes( lexer ) );
    if( tok == Lexer::WHITESPACE )  //current line is empty
        tok = document->getNextToken();
    //get the rest of the line as the title
    unsigned int startLine( document->dataLine() );
    std::wstring tmp;
    while(  document->dataLine() == startLine ) {
        if( tok == Lexer::WHITESPACE ||
            tok == Lexer::WORD ||
            tok == Lexer::PUNCTUATION ) {
            tmp += lexer->text();
        }
        else if( tok == Lexer::ENTITY ) {
            const std::wstring* exp( document->nameit( lexer->text() ) );
            if( exp )
                tmp += *exp;
            else {
                try {
                    wchar_t ch( document->entity( lexer->text() ) );
                    tmp += ch;
                }
                catch( Class2Error& e ) {
                    document->printError( e.code );
                }
            }
        }
        else if( tok == Lexer::END )
            throw FatalError( ERR_EOF );
        tok = document->getNextToken();
    }
    //convert to mbs, max 255 char
    wtombstring( tmp, title );
    if( title.size() > 255 )
        title.erase( 255 );
    tok = document->getNextToken();
    while( tok != Lexer::END && !( tok == Lexer::TAG && lexer->tagId() == Lexer::EUSERDOC ) ) {
        if( parseInline( lexer, tok ) ) {
            switch( lexer->tagId() ) {
            case Lexer::H1:
                return tok;
            case Lexer::H2:
                if( toc.nestLevel >= 2 )
                    return tok;;
                if( toc.nestLevel == 1 )
                    toc.hasChildren = 1;
                if( document->headerCutOff() >= 2 ) {
                    Hn* h2( new Hn( document, NULL, document->dataName(),
                        document->lexerLine(), document->lexerCol(), 2 ) );
                    Page* pg( new Page( document, h2 ) );
                    document->addPage( pg );
                    tok = h2->parse( lexer );
                }
                else {
                    Hn* h2( new Hn( document, this, document->dataName(),
                        document->lexerLine(), document->lexerCol(), 2 ) );
                    appendChild( h2 );
                    tok = h2->parse( lexer );
                }
                break;
            case Lexer::H3:
                if( toc.nestLevel >= 3 )
                    return tok;
                if( toc.nestLevel == 2 )
                    toc.hasChildren = 1;
                if( toc.nestLevel < 2 )
                    document->printError( ERR1_HEADNEST );
                if( document->headerCutOff() >= 3 ) {
                    Hn* h3( new Hn( document, NULL, document->dataName(),
                        document->lexerLine(), document->lexerCol(), 3 ) );
                    Page* pg( new Page( document, h3 ) );
                    document->addPage( pg );
                    tok = h3->parse( lexer );
                }
                else {
                    Hn* h3( new Hn( document, this, document->dataName(),
                        document->lexerLine(), document->lexerCol(), 3 ) );
                    appendChild( h3 );
                    tok = h3->parse( lexer );
                }
                break;
            case Lexer::H4:
                if( toc.nestLevel >= 4 )
                    return tok;
                if( toc.nestLevel == 3 )
                    toc.hasChildren = 1;
                if( toc.nestLevel < 3 )
                    document->printError( ERR1_HEADNEST );
                if( document->headerCutOff() >= 4 ) {
                    Hn* h4( new Hn( document, NULL, document->dataName(),
                        document->lexerLine(), document->lexerCol(), 4 ) );
                    Page* pg( new Page( document, h4 ) );
                    document->addPage( pg );
                    tok = h4->parse( lexer );
                }
                else {
                    Hn* h4( new Hn( document, this, document->dataName(),
                        document->lexerLine(), document->lexerCol(), 4 ) );
                    appendChild( h4 );
                    tok = h4->parse( lexer );
                }
                break;
            case Lexer::H5:
                if( toc.nestLevel >= 5 )
                    return tok;
                if( toc.nestLevel == 4 )
                    toc.hasChildren = 1;
                if( toc.nestLevel < 4 )
                    document->printError( ERR1_HEADNEST );
                if( document->headerCutOff() >= 5 ) {
                    Hn* h5( new Hn( document, NULL, document->dataName(),
                        document->lexerLine(), document->lexerCol(), 5 ) );
                    Page* pg( new Page( document, h5 ) );
                    document->addPage( pg );
                    tok = h5->parse( lexer );
                }
                else {
                    Hn* h5( new Hn( document, this, document->dataName(),
                        document->lexerLine(), document->lexerCol(), 5 ) );
                    appendChild( h5 );
                    tok = h5->parse( lexer );
                }
                break;
            case Lexer::H6:
                if( toc.nestLevel >= 6 )
                    return tok;
                if( toc.nestLevel == 5 )
                    toc.hasChildren = 1;
                if( toc.nestLevel < 5 )
                    document->printError( ERR1_HEADNEST );
                if( document->headerCutOff() >= 6 ) {
                    Hn* h6( new Hn( document, NULL, document->dataName(),
                        document->lexerLine(), document->lexerCol(), 6 ) );
                    Page* pg( new Page( document, h6 ) );
                    document->addPage( pg );
                    tok = h6->parse( lexer );
                }
                else {
                    Hn* h6( new Hn( document, this, document->dataName(),
                        document->lexerLine(), document->lexerCol(), 6 ) );
                    appendChild( h6 );
                    tok = h6->parse( lexer );
                }
                break;
            case Lexer::FN:
                return tok;
            case Lexer::I1:
                {
                    I1* i1( new I1( document, NULL, document->dataName(),
                        document->lexerLine(), document->lexerCol() ) );
                    if( res )
                        i1->setRes( res );
                    else if( id )
                        i1->setIdOrName( id );
                    else if( name )
                        i1->setIdOrName( name );
                    appendChild( i1 );
                    tok = i1->parse( lexer );
                }
                break;
            case Lexer::I2:
                {
                    I2* i2( new I2( document, NULL, document->dataName(),
                        document->lexerLine(), document->lexerCol() ) );
                    if( res )
                        i2->setRes( res );
                    else if( id )
                        i2->setIdOrName( id );
                    else if( name )
                        i2->setIdOrName( name );
                    appendChild( i2 );
                    tok = i2->parse( lexer );
                }
                break;
            case Lexer::ICMD:
                {
                    ICmd* icmd( new ICmd( document, NULL, document->dataName(),
                        document->lexerLine(), document->lexerCol() ) );
                    if( res )
                        icmd->setRes( res );
                    else if( id )
                        icmd->setIdOrName( id );
                    else if( name )
                        icmd->setIdOrName( name );
                    appendChild( icmd );
                    tok = icmd->parse( lexer );
                }
                break;
            case Lexer::ISYN:
                {
                    Element* elt( new ISyn( document, NULL, document->dataName(),
                        document->lexerLine(), document->lexerCol() ) );
                    appendChild( elt );
                    tok = elt->parse( lexer );
                }
                break;
            default:
                if( parseBlock( lexer, tok ) ) {
                    if( parseListBlock( lexer, tok ) )
                        parseCleanup( lexer, tok );
                }
            }
        }
    }
    return tok;
}
/***************************************************************************/
Lexer::Token Hn::parseAttributes( Lexer* lexer )
{
    Lexer::Token tok( document->getNextToken() );
    bool xorg( false );
    bool yorg( false );
    bool dx( false );
    bool dy( false );
    while( tok != Lexer::TAGEND ) {
        //parse attributes
        if( tok == Lexer::ATTRIBUTE ) {
            std::wstring key;
            std::wstring value;
            splitAttribute( lexer->text(), key, value );
            if( key == L"res" ) {
                res = static_cast< STD1::uint16_t >( std::wcstoul( value.c_str(), 0, 10 ) );
                if( res < 1 || res > 64000 )
                    document->printError( ERR2_VALUE );
                if( Hide::hiding() )
                    document->printError( ERR1_HIDERES );
            }
            else if( key == L"id" ) {
                id = new GlobalDictionaryWord( value );
                id->toUpper();              //convert to upper case
                if( !document->isInf() )
                    id = document->addWord( id );
            }
            else if( key == L"name" ) {
                name = new GlobalDictionaryWord( value );
                name->toUpper();            //convert to upper case
                if( !document->isInf() )
                    name = document->addWord( name );
            }
            else if( key == L"tutorial" ) {
                toc.extended = 1;
                etoc.setTutor = 1;
                tutorial = value;
            }
            else if( key == L"x" ) {
                xorg = true;
                toc.extended = 1;
                if( value == L"left" ) {
                    origin.xPosType = ExtTocEntry::DYNAMIC;
                    origin.xpos = ExtTocEntry::DYNAMIC_LEFT;
                }
                else if( value == L"center" ) {
                    origin.xPosType = ExtTocEntry::DYNAMIC;
                    origin.xpos = ExtTocEntry::DYNAMIC_CENTER;
                }
                else if( value == L"right" ) {
                    origin.xPosType = ExtTocEntry::DYNAMIC;
                    origin.xpos = ExtTocEntry::DYNAMIC_RIGHT;
                }
                else if( value == L"top" || value == L"bottom" )
                    document->printError( ERR2_VALUE );
                else {
                    wchar_t *end;
                    unsigned long int x( std::wcstoul( value.c_str(), &end, 10 ) );
                    origin.xpos = static_cast< unsigned short >( x );
                    if( *end == L'c' )
                        origin.xPosType = ExtTocEntry::ABSOLUTE_CHAR;
                    else if( *end == L'%' )
                        origin.xPosType = ExtTocEntry::RELATIVE_PERCENT;
                    else if( *end == L'x' )
                        origin.xPosType = ExtTocEntry::ABSOLUTE_PIXEL;
                    else if( *end == L'p' )
                        origin.xPosType = ExtTocEntry::ABSOLUTE_POINTS;
                    else
                        document->printError( ERR2_VALUE );
                }
                if( dx && origin.xPosType == ExtTocEntry::DYNAMIC && size.widthType != ExtTocEntry::RELATIVE_PERCENT )
                    document->printError( ERR3_MIXEDUNITS );
            }
            else if( key == L"y" ) {
                yorg = true;
                toc.extended = 1;
                if( value == L"top" ) {
                    origin.yPosType = ExtTocEntry::DYNAMIC;
                    origin.ypos = ExtTocEntry::DYNAMIC_TOP;
                }
                else if( value == L"center" ) {
                    origin.yPosType = ExtTocEntry::DYNAMIC;
                    origin.ypos = ExtTocEntry::DYNAMIC_CENTER;
                }
                else if( value == L"bottom" ) {
                    origin.yPosType = ExtTocEntry::DYNAMIC;
                    origin.ypos = ExtTocEntry::DYNAMIC_BOTTOM;
                }
                else if( value == L"left" || value == L"right" )
                    document->printError( ERR2_VALUE );
                else {
                    wchar_t *end;
                    unsigned long int y( std::wcstoul( value.c_str(), &end, 10 ) );
                    origin.ypos = static_cast< unsigned short >( y );
                    if( *end == L'c' )
                        origin.yPosType = ExtTocEntry::ABSOLUTE_CHAR;
                    else if( *end == L'%' )
                        origin.yPosType = ExtTocEntry::RELATIVE_PERCENT;
                    else if( *end == L'x' )
                        origin.yPosType = ExtTocEntry::ABSOLUTE_PIXEL;
                    else if( *end == L'p' )
                        origin.yPosType = ExtTocEntry::ABSOLUTE_POINTS;
                    else
                        document->printError( ERR2_VALUE );
                }
                if( dy && origin.yPosType == ExtTocEntry::DYNAMIC && size.heightType != ExtTocEntry::RELATIVE_PERCENT )
                    document->printError( ERR3_MIXEDUNITS );
            }
            else if( key == L"width" ) {
                dx = true;
                toc.extended = 1;
                if( value == L"left" ||
                    value == L"center" ||
                    value == L"right" ||
                    value == L"top" ||
                    value == L"bottom" )
                    document->printError( ERR2_VALUE );
                else {
                    wchar_t *end;
                    unsigned long int width = std::wcstoul( value.c_str(), &end, 10 );
                    size.width = static_cast< unsigned short >( width );
                    if( *end == L'c' )
                        size.widthType = ExtTocEntry::ABSOLUTE_CHAR;
                    else if( *end == L'%' )
                        size.widthType = ExtTocEntry::RELATIVE_PERCENT;
                    else if( *end == L'x' )
                        size.widthType = ExtTocEntry::ABSOLUTE_PIXEL;
                    else if( *end == L'p' )
                        size.widthType = ExtTocEntry::ABSOLUTE_POINTS;
                    else
                        document->printError( ERR2_VALUE );
                }
                if( xorg && origin.xPosType == ExtTocEntry::DYNAMIC && size.widthType != ExtTocEntry::RELATIVE_PERCENT )
                    document->printError( ERR3_MIXEDUNITS );
            }
            else if( key == L"height" ) {
                dy = true;
                toc.extended = 1;
                if( value == L"left" ||
                    value == L"center" ||
                    value == L"right" ||
                    value == L"top" ||
                    value == L"bottom" )
                    document->printError( ERR2_VALUE );
                else {
                    wchar_t *end;
                    unsigned long int height = std::wcstoul( value.c_str(), &end, 10 );
                    size.height = static_cast< unsigned short >( height );
                    if( *end == L'c' )
                        size.heightType = ExtTocEntry::ABSOLUTE_CHAR;
                    else if( *end == L'%' )
                        size.heightType = ExtTocEntry::RELATIVE_PERCENT;
                    else if( *end == L'x' )
                        size.heightType = ExtTocEntry::ABSOLUTE_PIXEL;
                    else if( *end == L'p' )
                        size.heightType = ExtTocEntry::ABSOLUTE_POINTS;
                    else
                        document->printError( ERR2_VALUE );
                }
                if( yorg && origin.yPosType == ExtTocEntry::DYNAMIC && size.heightType != ExtTocEntry::RELATIVE_PERCENT )
                    document->printError( ERR3_MIXEDUNITS );
            }
            else if( key == L"group" ) {
                toc.extended = 1;
                group.id = static_cast< STD1::uint16_t >( std::wcstoul( value.c_str(), 0, 10 ) );
            }
            else if( key == L"titlebar" ) {
                toc.extended = 1;
                if( value == L"yes" )
                    style.word |= PageStyle::TITLEBAR;
                else if( value == L"sysmenu" ) {
                    style.word |= PageStyle::TITLEBAR;
                    style.word |= PageStyle::SYSMENU;
                }
                else if( value == L"minmax" ) {
                    style.word |= PageStyle::TITLEBAR;
                    style.word |= PageStyle::MINMAX;
                }
                else if( value == L"both" ) {
                    style.word |= PageStyle::TITLEBAR;
                    style.word |= PageStyle::SYSMENU;
                    style.word |= PageStyle::MINMAX;
                }
                else if( value != L"none" )
                    document->printError( ERR2_VALUE );
            }
            else if( key == L"scroll" ) {
                toc.extended = 1;
                if( value == L"horizontal" )
                    style.word |= PageStyle::HSCROLL;
                else if( value == L"vertical" )
                    style.word |= PageStyle::VSCROLL;
                else if( value == L"both" ) {
                    style.word |= PageStyle::HSCROLL;
                    style.word |= PageStyle::VSCROLL;
                }
                else if( value != L"none" )
                    document->printError( ERR2_VALUE );
            }
            else if( key == L"rules" ) {
                toc.extended = 1;
                if( value == L"border" )
                    style.word |= PageStyle::BORDER;
                else if( value == L"sizeborder" )
                    style.word |= PageStyle::SIZEBORDER;
                else if( value != L"none" )
                    document->printError( ERR2_VALUE );
            }
            else if( key == L"toc" ) {
                wchar_t ch[2];
                ch[0] = value[ value.size() - 1 ];//last number is critical value
                ch[1] = L'\0';
                int tmp( static_cast< int >( std::wcstol( ch, 0, 10 ) ) );
                if( tmp < 1 || tmp > 6 )
                    document->printError( ERR2_VALUE );
                else
                    document->setHeaderCutOff( static_cast< unsigned int >( tmp ) );
            }
            else if( key == L"ctrlarea" ) {
                if( value == L"page" ) {
                    toc.extended = 1;
                    etoc.setCtrl = 1;
                }
                else
                    etoc.setCtrl = 0;
            }
            else if( key == L"ctrlrefid" ) {
                toc.extended = 1;
                std::transform( value.begin(), value.end(), value.begin(), std::towupper );
                controls.word |= document->getGroupById( value );
            }
            else
                document->printError( ERR1_ATTRNOTDEF );
        }
        else if( tok == Lexer::FLAG ) {
            if( lexer->text() == L"global" )
                global = true;
            else if( lexer->text() == L"viewport" ) {
                toc.extended = 1;
                etoc.setView = 1;
            }
            else if( lexer->text() == L"clear" ) {
                toc.extended = 1;
                etoc.clear = 1;
            }
            else if( lexer->text() == L"nosearch" ) {
                toc.extended = 1;
                etoc.noSearch = 1;
            }
            else if( lexer->text() == L"noprint" ) {
                toc.extended = 1;
                etoc.noPrint = 1;
            }
            else if( lexer->text() == L"hide" )
                toc.hidden = 1;
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
    return document->getNextToken(); //consume TAGEND
}
/***************************************************************************/
void Hn::buildTOC( Page* page )
{
    if( !parent ) {     //root element of a page
        page->setChildren( childTOCs );
        page->setTitle( title );
        page->setTOC( toc );
        page->setETOC( etoc );
        page->setOrigin( origin );
        page->setSize( size );
        page->setStyle( style );
        page->setGroup( group );
        page->SetControls( controls );
        //FIXME: need the index of the parent?
        //page->setSearchable( !nosearch );
        TocRef tr( fileName, row, page->index() );
        if( res || !document->isInf() ) {
            try {
                document->addRes( res, tr );
            }
            catch ( Class3Error& e ) {
                printError( e.code );
            }
        }
        if( id ) {
            try {
                document->addNameOrId( id, tr );
            }
            catch( Class3Error& e ) {
                printError( e.code );
            }
        }
        if( name ) {
            try {
                document->addNameOrId( name, tr );
            }
            catch( Class3Error& e ) {
                printError( e.code );
            }
        }
        if( global && !document->isInf() ) {
            if( id ) {
                try {
                    document->addGNameOrId( id, page->index() );
                }
                catch( Class3Error& e ) {
                    printError( e.code );
                }
            }
            if( name ) {
                try {
                    document->addGNameOrId( name, page->index() );
                }
                catch( Class3Error& e ) {
                    printError( e.code );
                }
            }
        }
    }
}
/***************************************************************************/
void Hn::buildText( Cell* cell )
{
    if( etoc.setTutor ) {
        std::string tmp;
        wtombstring( tutorial, tmp );
        size_t size( tmp.size() );
        if( size > 253 ) {
            tmp.erase( 253 );
            size = 253;
        }
        std::vector< STD1::uint8_t > esc;
        esc.reserve( size + 3 );
        esc.push_back( 0xFF );  //esc
        esc.push_back( 0x02 );  //size
        esc.push_back( 0x15 );  //begin hide
        for( unsigned int count1 = 0; count1 < size; count1++ )
            esc.push_back( static_cast< STD1::uint8_t >( tmp[ count1 ] ) );
        esc[1] = static_cast< STD1::uint8_t >( esc.size() - 1 );
        cell->addEsc( esc );
        if( cell->textFull() )
            printError( ERR1_LARGEPAGE );
    }
}
/***************************************************************************/
void Hn::linearize( Page* page )
{
    if( document->headerCutOff() < toc.nestLevel )
        page->addElement( this );
    for( ConstChildrenIter iter = children.begin(); iter != children.end(); ++iter )
        ( *iter )->linearize( page );
}
