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

Link::~Link()
{
    if( document->isInf() )
        delete refid;
}
/***************************************************************************/
Lexer::Token Link::parse( Lexer* lexer )
{
    Lexer::Token tok( parseAttributes( lexer ) );
    if( !noElink ) {
        while( tok != Lexer::END && !( tok == Lexer::TAG && lexer->tagId() == Lexer::EUSERDOC) ) {
            //inline except: elink, artlink, eartlink, artwork, hdref
            if( tok == Lexer::TAG ) {
                if( lexer->tagId() == Lexer::ELINK )
                    break;
                else if( lexer->tagId() == Lexer::ARTLINK ||
                         lexer->tagId() == Lexer::EARTLINK ||
                         lexer->tagId() == Lexer::ARTWORK ||
                         lexer->tagId() == Lexer::HDREF ) {
                    parseCleanup( lexer, tok );
                }
                else if( parseInline( lexer, tok ) )
                    parseCleanup( lexer, tok );
            }
            else if( parseInline( lexer, tok ) )
                parseCleanup( lexer, tok );
        }
    }
    return tok;
}
/***************************************************************************/
Lexer::Token Link::parseAttributes( Lexer* lexer )
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
            if( key == L"reftype" ) {
                if( value == L"hd" )
                    type = TOPIC;
                else if( value == L"fn" ) {
                    Hn* root( static_cast< Hn* >( rootElement() ) );
                    if( root->isSplit() )
                        document->printError( ERR3_FNNOSPLIT );
                    type = FOOTNOTE;
                }
                else if( value == L"launch" )
                    type = LAUNCH;
                else if( value == L"inform" ) {
                    type = INFORM;
                    noElink = true;
                }
                else
                    document->printError( ERR2_VALUE );
            }
            else if( key == L"res" )
                res = static_cast< STD1::uint16_t >( std::wcstoul( value.c_str(), 0, 10 ) );
            else if( key == L"refid" ) {
                refid = new GlobalDictionaryWord( value );
                refid->toUpper();           //to uppercase
                if( !document->isInf() )
                    refid = document->addWord( refid );
            }
            else if( key == L"database" ) {
                database = value;
                try {
                    document->addExtFile( value );
                }
                catch( Class1Error& e ) {
                    document->printError( e.code );
                }
            }
            else if( key == L"object" )
                object = value;
            else if( key == L"data" )
                data = value;
            else if( key == L"group" ) {
                group.id = static_cast< STD1::uint16_t >( std::wcstoul( value.c_str(), 0, 10 ) );
                doGroup = true;
            }
            else if( key == L"vpx" ) {
                doOrigin = true;
                xorg = true;
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
                    origin.xpos = static_cast< STD1::uint16_t >( x );
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
            else if( key == L"vpy" ) {
                doOrigin = true;
                yorg = true;
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
                    origin.ypos = static_cast< STD1::uint16_t >( y );
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
            else if( key == L"vpcx" ) {
                doSize = true;
                dx = true;
                if( value == L"left" ||
                    value == L"center" ||
                    value == L"right" ||
                    value == L"top" ||
                    value == L"bottom" )
                    document->printError( ERR2_VALUE );
                else {
                    wchar_t *end;
                    unsigned long int width( std::wcstoul( value.c_str(), &end, 10 ) );
                    size.width = static_cast< STD1::uint16_t >( width );
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
            else if( key == L"vpcy" ) {
                doSize = true;
                dy = true;
                if( value == L"left" ||
                    value == L"center" ||
                    value == L"right" ||
                    value == L"top" ||
                    value == L"bottom" )
                    document->printError( ERR2_VALUE );
                else {
                    wchar_t *end;
                    unsigned long int height( std::wcstoul( value.c_str(), &end, 10 ) );
                    size.height = static_cast< STD1::uint16_t >( height );
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
            else if( key == L"x" ) {
                hypergraphic = true;
                x = static_cast< STD1::uint16_t >( std::wcstoul( value.c_str(), 0, 10 ) );
            }
            else if( key == L"y" ) {
                hypergraphic = true;
                y = static_cast< STD1::uint16_t >( std::wcstoul( value.c_str(), 0, 10 ) );
            }
            else if( key == L"cx" ) {
                hypergraphic = true;
                cx = static_cast< STD1::uint16_t >( std::wcstoul( value.c_str(), 0, 10 ) );
            }
            else if( key == L"cy" ) {
                hypergraphic = true;
                cy = static_cast< STD1::uint16_t >( std::wcstoul( value.c_str(), 0, 10 ) );
            }
            else if( key == L"titlebar" ) {
                doStyle = true;
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
                doStyle = true;
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
                doStyle = true;
                if( value == L"border" )
                    style.word |= PageStyle::BORDER;
                else if( value == L"sizeborder" )
                    style.word |= PageStyle::SIZEBORDER;
                else if( value != L"none" )
                    document->printError( ERR2_VALUE );
            }
            else
                document->printError( ERR1_ATTRNOTDEF );
        }
        else if( tok == Lexer::FLAG ) {
            if( lexer->text() == L"auto" ) {
                if( type == FOOTNOTE )
                    document->printError( ERR3_FNNOAUTO );
                else {
                    automatic = true;
                    noElink = true;
                }
            }
            else if( lexer->text() == L"viewport" )
                viewport = true;
            else if( lexer->text() == L"dependent" )
                dependent = true;
            else if( lexer->text() == L"split" )
                split = true;
            else if( lexer->text() == L"child" )
                child = true;
            else
                document->printError( ERR1_ATTRNOTDEF );
        }
        else if( tok == Lexer::ERROR_TAG )
            throw FatalError( ERR_SYNTAX );
        else if( tok == Lexer::END )
            throw FatalError( ERR_EOF );
        tok = document->getNextToken();
    }
    return document->getNextToken();    //consume TAGEND
}
/***************************************************************************/
void Link::buildText( Cell* cell )
{
    switch( type ) {
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
    if( refid || res ) {                    //either refid or res is required
        if( database.empty() ) {            //jump to internal link
            try {
                XRef xref( fileName, row );
                STD1::uint16_t tocIndex;
                if( refid ) {
                    tocIndex = document->tocIndexById( refid );
                    document->addXRef( refid, xref );
                }
                else {
                    tocIndex = document->tocIndexByRes( res );
                    document->addXRef( res, xref );
                }
                std::vector< STD1::uint8_t > esc;
                esc.reserve( 7 + sizeof( PageOrigin ) + sizeof( PageSize ) + \
                    sizeof( PageStyle ) + sizeof( PageGroup ) );
                esc.push_back( 0xFF );      //ESC
                esc.push_back( 4 );         //size
                if( !hypergraphic )
                    esc.push_back( 0x05 );  //text link
                else {
                    if( x || y || cx || cy )
                        esc.push_back( 0x01 );  //partial bitmap
                    else
                        esc.push_back( 0x04 );  //full bitmap
                }
                esc.push_back( static_cast< STD1::uint8_t >( tocIndex ) );
                esc.push_back( static_cast< STD1::uint8_t >( tocIndex >> 8 ) );
                //this may need to be last
                if( hypergraphic && ( x || y || cx || cy ) ) {
                    esc.push_back( static_cast< STD1::uint8_t >( x ) );
                    esc.push_back( static_cast< STD1::uint8_t >( x >> 8 ) );
                    esc.push_back( static_cast< STD1::uint8_t >( y ) );
                    esc.push_back( static_cast< STD1::uint8_t >( y >> 8 ) );
                    esc.push_back( static_cast< STD1::uint8_t >( cx ) );
                    esc.push_back( static_cast< STD1::uint8_t >( cx >> 8 ) );
                    esc.push_back( static_cast< STD1::uint8_t >( cy ) );
                    esc.push_back( static_cast< STD1::uint8_t >( cy >> 8 ) );
                }
                if( viewport || doStyle || automatic || split || doOrigin || doSize ||
                    dependent || doGroup ) {
                    STD1::uint8_t flag1( 0 );
                    STD1::uint8_t flag2( 0 );
                    esc[ 1 ] += 2;
                    if( doOrigin )
                        flag1 |= 0x01;
                    if( doSize )
                        flag1 |= 0x02;
                    if( viewport )
                        flag1 |= 0x04;
                    if( doStyle )
                        flag1 |= 0x08;
                    if( automatic )
                        flag1 |= 0x40;
                    if( split ) {
                        flag1 |= 0x80;
                        Hn* root( static_cast< Hn* >( rootElement() ) );
                        root->setIsParent();
                        root->addChild( tocIndex );
                    }
                    if( dependent )
                        flag2 |= 0x02;
                    if( doGroup )
                        flag2 |= 0x04;
                    esc.push_back( flag1 );
                    esc.push_back( flag2 );
                }
                if( doOrigin ) {
                    esc[ 1 ] += sizeof( PageOrigin );
                    STD1::uint8_t* src = reinterpret_cast< STD1::uint8_t* >( &origin );
                    for( size_t count1 = 0; count1 < sizeof( PageOrigin ); ++count1, ++src)
                        esc.push_back( *src );
                }
                if( doSize ) {
                    esc[ 1 ] += sizeof( PageSize );
                    STD1::uint8_t* src = reinterpret_cast< STD1::uint8_t* >( &size );
                    for( size_t count1 = 0; count1 < sizeof( PageSize ); ++count1, ++src)
                        esc.push_back( *src );
                }
                if( doStyle ) {
                    esc[ 1 ] += sizeof( PageStyle );
                    esc.push_back( static_cast< STD1::uint8_t >( style.word ) );
                    esc.push_back( static_cast< STD1::uint8_t >( style.word >> 8 ) );
                }
                if( doGroup ) {
                    esc[ 1 ] += sizeof( PageGroup );
                    esc.push_back( static_cast< STD1::uint8_t >( group.id ) );
                    esc.push_back( static_cast< STD1::uint8_t >( group.id >> 8 ) );
                }
                esc[ 1 ] = static_cast< STD1::uint8_t >( esc.size() - 1 );
                cell->addEsc( esc );
                if( cell->textFull() )
                    printError( ERR1_LARGEPAGE );
            }
            catch( Class1Error& e ) {
                printError( e.code );
            }
        }
        else {                              //jump to external link
            std::vector< STD1::uint8_t > esc;
            esc.reserve( 7 );
            esc.push_back( 0xFF );          //ESC
            esc.push_back( 4 );             //size
            if( !hypergraphic )
                esc.push_back( 0x1F );      //text link
            else {
                esc.push_back( 0x0F );      //hypergraphic link
                if( x || y || cx || cy )
                    esc.push_back( 0x17 );  //partial bitmap
                else
                    esc.push_back( 0x16 );  //full bitmap
            }
            STD1::uint16_t index( document->extFileIndex( database ) );
            esc.push_back( static_cast< STD1::uint8_t >( index ) );
            //esc.push_back( static_cast< STD1::uint8_t >( index >> 8 ) );
            std::string tmp;
            wtombstring( refid->getText(), tmp );
            size_t size( tmp.size() );
            esc.push_back( static_cast< STD1::uint8_t >( size ) );
            if( hypergraphic && ( x || y || cx || cy ) ) {
                esc.push_back( static_cast< STD1::uint8_t >( x ) );
                esc.push_back( static_cast< STD1::uint8_t >( x >> 8 ) );
                esc.push_back( static_cast< STD1::uint8_t >( y ) );
                esc.push_back( static_cast< STD1::uint8_t >( y >> 8 ) );
                esc.push_back( static_cast< STD1::uint8_t >( cx ) );
                esc.push_back( static_cast< STD1::uint8_t >( cx >> 8 ) );
                esc.push_back( static_cast< STD1::uint8_t >( cy ) );
                esc.push_back( static_cast< STD1::uint8_t >( cy >> 8 ) );
            }
            if( size > 255 - esc.size() + 1 ) {
                size = 255 - esc.size() + 1;
                tmp.erase( size );
            }
            for( size_t count1 = 0; count1 < size; count1++ )
                esc.push_back( static_cast< STD1::uint8_t >( tmp[ count1 ] ) );
            esc[ 1 ] = static_cast< STD1::uint8_t >( esc.size() - 1 );
            cell->addEsc( esc );
            if( cell->textFull() )
                printError( ERR1_LARGEPAGE );
        }
    }
    else
        printError( ERR1_NOREFID );
}
/***************************************************************************/
void Link::doFootnote( Cell* cell )
{
    if( refid || res ) {                    //refid is required
        try {
            XRef xref( fileName, row );
            size_t tocIndex;
            if( refid ) {
                tocIndex = document->tocIndexById( refid );
                document->addXRef( refid, xref );
            }
            else {
                tocIndex = document->tocIndexByRes( res );
                document->addXRef( res, xref );
            }
            std::vector< STD1::uint8_t > esc;
            esc.reserve( 5 );
            esc.push_back( 0xFF );          //ESC
            esc.push_back( 4 );             //size
            if( !hypergraphic )
                esc.push_back( 0x07 );      //text link
            else {
                if( x || y || cx || cy )
                    esc.push_back( 0x02 );  //partial bitmap
                else
                    esc.push_back( 0x05 );  //full bitmap
            }
            esc.push_back( static_cast< STD1::uint8_t >( tocIndex ) );
            esc.push_back( static_cast< STD1::uint8_t >( tocIndex >> 8 ) );
            if( hypergraphic && ( x || y || cx || cy ) ) {
                esc.push_back( static_cast< STD1::uint8_t >( x ) );
                esc.push_back( static_cast< STD1::uint8_t >( x >> 8 ) );
                esc.push_back( static_cast< STD1::uint8_t >( y ) );
                esc.push_back( static_cast< STD1::uint8_t >( y >> 8 ) );
                esc.push_back( static_cast< STD1::uint8_t >( cx ) );
                esc.push_back( static_cast< STD1::uint8_t >( cx >> 8 ) );
                esc.push_back( static_cast< STD1::uint8_t >( cy ) );
                esc.push_back( static_cast< STD1::uint8_t >( cy >> 8 ) );
            }
            esc[ 1 ] = static_cast< STD1::uint8_t >( esc.size() - 1 );
            cell->addEsc( esc );
            if( cell->textFull() )
                printError( ERR1_LARGEPAGE );
        }
        catch( Class1Error& e ) {
            printError( e.code );
        }
    }
    else
        printError( ERR1_NOREFID );
}
/***************************************************************************/
void Link::doLaunch( Cell* cell )
{
    if( object.size() && data.size() ) {//both are required
        std::vector< STD1::uint8_t > esc;
        esc.reserve( 6 );
        esc.push_back( 0xFF );          //ESC
        esc.push_back( 3 );             //size
        if ( !hypergraphic )
            esc.push_back( 0x10 );      //text link
        else {
            esc.push_back( 0x0F );      //hypergraphic link
            if( x || y || cx || cy )
                esc.push_back( 0x08 );  //partial bitmap
            else
                esc.push_back( 0x07 );  //full bitmap
        }
        esc.push_back( 0x00 );          //blank byte
        if( hypergraphic && ( x || y || cx || cy ) ) {
            esc.push_back( static_cast< STD1::uint8_t >( x ) );
            esc.push_back( static_cast< STD1::uint8_t >( x >> 8 ) );
            esc.push_back( static_cast< STD1::uint8_t >( y ) );
            esc.push_back( static_cast< STD1::uint8_t >( y >> 8 ) );
            esc.push_back( static_cast< STD1::uint8_t >( cx ) );
            esc.push_back( static_cast< STD1::uint8_t >( cx >> 8 ) );
            esc.push_back( static_cast< STD1::uint8_t >( cy ) );
            esc.push_back( static_cast< STD1::uint8_t >( cy >> 8 ) );
        }
        std::string buffer;
        wtombstring( object, buffer );
        buffer += ' ';
        wtombstring( data, buffer );
        size_t size( buffer.size() );
        if( size > 255 - esc.size() + 1 ) {
            size = 255 - esc.size() + 1;
            buffer.erase( size );
        }
        for( size_t count1 = 0; count1 < size; ++count1 )
            esc.push_back( static_cast< STD1::uint8_t >( buffer[ count1 ] ) );
        esc[ 1 ] = static_cast< STD1::uint8_t >( esc.size() - 1 );
        cell->addEsc( esc );
        if( cell->textFull() )
            printError( ERR1_LARGEPAGE );
    }
    else
        printError( ERR2_VALUE );
}
/***************************************************************************/
void Link::doInform( Cell* cell )
{
   if( res ) {                          //res is required
        std::vector< STD1::uint8_t > esc;
        esc.reserve( 5 );
        esc.push_back( 0xFF );          //ESC
        esc.push_back( 4 );             //size
        if( !hypergraphic )
            esc.push_back( 0x16 );      //text link
        else {
            esc.push_back( 0x0F );      //hypergraphic link
            if( x || y || cx || cy )
                esc.push_back( 0x10 );  //partial bitmap
            else
                esc.push_back( 0x09 );  //full bitmap
        }
        esc.push_back( static_cast< STD1::uint8_t >( res ) );
        esc.push_back( static_cast< STD1::uint8_t >( res >> 8 ) );
        if ( hypergraphic && ( x || y || cx || cy ) ) {
            esc.push_back( static_cast< STD1::uint8_t >( x ) );
            esc.push_back( static_cast< STD1::uint8_t >( x >> 8 ) );
            esc.push_back( static_cast< STD1::uint8_t >( y ) );
            esc.push_back( static_cast< STD1::uint8_t >( y >> 8 ) );
            esc.push_back( static_cast< STD1::uint8_t >( cx ) );
            esc.push_back( static_cast< STD1::uint8_t >( cx >> 8 ) );
            esc.push_back( static_cast< STD1::uint8_t >( cy ) );
            esc.push_back( static_cast< STD1::uint8_t >( cy >> 8 ) );
        }
        esc[ 1 ] = static_cast< STD1::uint8_t >( esc.size() - 1 );
        cell->addEsc( esc );
        if( cell->textFull() )
            printError( ERR1_LARGEPAGE );
    }
    else
        printError( ERR2_VALUE );
}
/***************************************************************************/
void ELink::buildText( Cell* cell )
{
    cell->addByte( 0xFF );
    cell->addByte( 0x02 );
    cell->addByte( 0x08 );
    if( cell->textFull() )
        printError( ERR1_LARGEPAGE );
}
