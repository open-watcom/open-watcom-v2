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
* Description:  A abstract class for a tag element
*
****************************************************************************/

#include <algorithm>
#include "tag.hpp"
#include "document.hpp"
#include "page.hpp"

#include "acvwport.hpp"
#include "artlink.hpp"
#include "artwork.hpp"
#include "caution.hpp"
#include "cgraphic.hpp"
#include "color.hpp"
#include "ddf.hpp"
#include "dl.hpp"
#include "document.hpp"
#include "entity.hpp"
#include "fig.hpp"
#include "font.hpp"
#include "hdref.hpp"
#include "hide.hpp"
#include "hn.hpp"
#include "hpn.hpp"
#include "lines.hpp"
#include "link.hpp"
#include "lm.hpp"
#include "note.hpp"
#include "nt.hpp"
#include "ol.hpp"
#include "p.hpp"
#include "parml.hpp"
#include "punct.hpp"
#include "rm.hpp"
#include "sl.hpp"
#include "table.hpp"
#include "ul.hpp"
#include "warning.hpp"
#include "whtspc.hpp"
#include "word.hpp"
#include "xmp.hpp"

Tag::~Tag()
{
    for( ChildrenIter itr = children.begin(); itr != children.end(); ++itr ) {
        delete *itr;
    }
}
/***************************************************************************/
Lexer::Token Tag::parse( Lexer* lexer )
{
    return parseAttributes( lexer );
}
/***************************************************************************/
Lexer::Token Tag::parseAttributes( Lexer* lexer )
{
    Lexer::Token tok( document->getNextToken() );
    lexer = lexer;
    while( tok != Lexer::TAGEND ) {
        if( tok == Lexer::ATTRIBUTE )
            document->printError( ERR1_NOATTR );
        else if( tok == Lexer::FLAG )
            document->printError( ERR1_NOATTR );
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
bool Tag::parseInline( Lexer* lexer, Lexer::Token& tok )
{
    bool notHandled( false );
    if( tok == Lexer::WORD ) {
        Word* word( new Word( document, this, document->dataName(),
            document->lexerLine(), document->lexerCol(), whiteSpace ) );
        appendChild( word );
        tok = word->parse( lexer );
    }
    else if( tok == Lexer::ENTITY ) {
        Entity* entity( new Entity( document, this, document->dataName(),
            document->lexerLine(), document->lexerCol(), whiteSpace ) );
        appendChild( entity );
        tok = entity->parse( lexer );
    }
    else if( tok == Lexer::PUNCTUATION ) {
        Punctuation* punct( new Punctuation( document, this, document->dataName(),
            document->lexerLine(), document->lexerCol(), whiteSpace ) );
        appendChild( punct );
        tok = punct->parse( lexer );
    }
    else if( tok == Lexer::WHITESPACE ) {
        WhiteSpace* ws( new WhiteSpace( document, this, document->dataName(),
        document->lexerLine(), document->lexerCol(), whiteSpace ) );
        appendChild( ws );
        tok = ws->parse( lexer );
    }
    else if( tok == Lexer::COMMAND )
        tok = document->processCommand( lexer, this );
    else if( tok == Lexer::TAG ) {
        switch( lexer->tagId() ) {
        //make new tag
        //append pointer to this tag's list
        case Lexer::ARTLINK:
            {
                Element* elt( new Artlink( document, this, document->dataName(),
                    document->lexerLine(), document->lexerCol() ) );
                appendChild( elt );
                tok = elt->parse( lexer );
            }
            break;
        case Lexer::EARTLINK:
            {
                Element* elt( new EArtlink( document, this, document->dataName(),
                    document->lexerLine(), document->lexerCol() ) );
                appendChild( elt );
                tok = elt->parse( lexer );
            }
            break;
        case Lexer::ARTWORK:
            {
                Element* elt( new Artwork( document, this, document->dataName(),
                    document->lexerLine(), document->lexerCol() ) );
                appendChild( elt );
                tok = elt->parse( lexer );
            }
            break;
        case Lexer::COLOR:
            {
                Element* elt( new Color( document, this, document->dataName(),
                    document->lexerLine(), document->lexerCol() ) );
                appendChild( elt );
                tok = elt->parse( lexer );
            }
            break;
        case Lexer::FONT:
            {
                Element* elt( new Font( document, this, document->dataName(),
                    document->lexerLine(), document->lexerCol() ) );
                appendChild( elt );
                tok = elt->parse( lexer );
            }
            break;
        case Lexer::HDREF:
            {
                Element* elt( new Hdref( document, this, document->dataName(),
                    document->lexerLine(), document->lexerCol() ) );
                appendChild( elt );
                tok = elt->parse( lexer );
            }
            break;
        case Lexer::HIDE:
            {
                Element* elt( new Hide( document, this, document->dataName(),
                    document->lexerLine(), document->lexerCol() ) );
                appendChild( elt );
                tok = elt->parse( lexer );
            }
            break;
        case Lexer::EHIDE:
            {
                Element* elt( new EHide( document, this, document->dataName(),
                    document->lexerLine(), document->lexerCol() ) );
                appendChild( elt );
                tok = elt->parse( lexer );
            }
            break;
        case Lexer::HP1:
            {
                Element* elt( new Hpn( document, this, document->dataName(),
                    document->lexerLine(), document->lexerCol(), 1 ) );
                appendChild( elt );
                tok = elt->parse( lexer );
            }
            break;
        case Lexer::HP2:
            {
                Element* elt( new Hpn( document, this, document->dataName(),
                    document->lexerLine(), document->lexerCol(), 2 ) );
                appendChild( elt );
                tok = elt->parse( lexer );
            }
            break;
        case Lexer::HP3:
            {
                Element* elt( new Hpn( document, this, document->dataName(),
                    document->lexerLine(), document->lexerCol(), 3 ) );
                appendChild( elt );
                tok = elt->parse( lexer );
            }
            break;
        case Lexer::HP4:
            {
                Element* elt( new Hpn( document, this, document->dataName(),
                    document->lexerLine(), document->lexerCol(), 4 ) );
                appendChild( elt );
                tok = elt->parse( lexer );
            }
            break;
        case Lexer::HP5:
            {
                Element* elt( new Hpn( document, this, document->dataName(),
                    document->lexerLine(), document->lexerCol(), 5 ) );
                appendChild( elt );
                tok = elt->parse( lexer );
            }
            break;
        case Lexer::HP6:
            {
                Element* elt( new Hpn( document, this, document->dataName(),
                    document->lexerLine(), document->lexerCol(), 6 ) );
                appendChild( elt );
                tok = elt->parse( lexer );
            }
            break;
        case Lexer::HP7:
            {
                Element* elt( new Hpn( document, this, document->dataName(),
                    document->lexerLine(), document->lexerCol(), 7 ) );
                appendChild( elt );
                tok = elt->parse( lexer );
            }
            break;
        case Lexer::HP8:
            {
                Element* elt( new Hpn( document, this, document->dataName(),
                    document->lexerLine(), document->lexerCol(), 8 ) );
                appendChild( elt );
                tok = elt->parse( lexer );
            }
            break;
        case Lexer::HP9:
            {
                Element* elt( new Hpn( document, this, document->dataName(),
                    document->lexerLine(), document->lexerCol(), 9 ) );
                appendChild( elt );
                tok = elt->parse( lexer );
            }
            break;
        case Lexer::EHP1:
            {
                Element* elt( new EHpn( document, this, document->dataName(),
                    document->lexerLine(), document->lexerCol(), 1 ) );
                appendChild( elt );
                tok = elt->parse( lexer );
            }
            break;
        case Lexer::EHP2:
            {
                Element* elt( new EHpn( document, this, document->dataName(),
                    document->lexerLine(), document->lexerCol(), 2 ) );
                appendChild( elt );
                tok = elt->parse( lexer );
            }
            break;
        case Lexer::EHP3:
            {
                Element* elt( new EHpn( document, this, document->dataName(),
                    document->lexerLine(), document->lexerCol(), 3 ) );
                appendChild( elt );
                tok = elt->parse( lexer );
            }
            break;
        case Lexer::EHP4:
            {
                Element* elt( new EHpn( document, this, document->dataName(),
                    document->lexerLine(), document->lexerCol(), 4 ) );
                appendChild( elt );
                tok = elt->parse( lexer );
            }
            break;
        case Lexer::EHP5:
            {
                Element* elt( new EHpn( document, this, document->dataName(),
                    document->lexerLine(), document->lexerCol(), 5 ) );
                appendChild( elt );
                tok = elt->parse( lexer );
            }
            break;
        case Lexer::EHP6:
            {
                Element* elt( new EHpn( document, this, document->dataName(),
                    document->lexerLine(), document->lexerCol(), 6 ) );
                appendChild( elt );
                tok = elt->parse( lexer );
            }
            break;
        case Lexer::EHP7:
            {
                Element* elt( new EHpn( document, this, document->dataName(),
                    document->lexerLine(), document->lexerCol(), 7 ) );
                appendChild( elt );
                tok = elt->parse( lexer );
            }
            break;
        case Lexer::EHP8:
            {
                Element* elt( new EHpn( document, this, document->dataName(),
                    document->lexerLine(), document->lexerCol(), 8 ) );
                appendChild( elt );
                tok = elt->parse( lexer );
            }
            break;
        case Lexer::EHP9:
            {
                Element* elt( new EHpn( document, this, document->dataName(),
                    document->lexerLine(), document->lexerCol(), 9 ) );
                appendChild( elt );
                tok = elt->parse( lexer );
            }
            break;
        case Lexer::LINK:
            {
                Element* elt( new Link( document, this, document->dataName(),
                    document->lexerLine(), document->lexerCol(), whiteSpace ) );
                appendChild( elt );
                tok = elt->parse( lexer );
            }
            break;
        case Lexer::ELINK:
            {
                Element* elt( new ELink( document, this, document->dataName(),
                    document->lexerLine(), document->lexerCol() ) );
                appendChild( elt );
                tok = elt->parse( lexer );
            }
            break;
        case Lexer::LM:
            {
                Element* elt( new Lm( document, this, document->dataName(),
                    document->lexerLine(), document->lexerCol() ) );
                appendChild( elt );
                tok = elt->parse( lexer );
            }
            break;
        case Lexer::RM:
            {
                Element* elt( new Rm( document, this, document->dataName(),
                    document->lexerLine(), document->lexerCol() ) );
                appendChild( elt );
                tok = elt->parse( lexer );
            }
            break;
        case Lexer::EUSERDOC:
            break;
        default:
            notHandled = true;
            break;
        }
    }
    else if( tok == Lexer::ERROR_TAG ) {
        document->printError( ERR1_TAGNOTDEF );
        tok = document->getNextToken();
    }
    else if( tok == Lexer::ERROR_ENTITY ) {
        document->printError( ERR1_TAGNOTDEF );
        tok = document->getNextToken();
    }
    return notHandled;
}
/***************************************************************************/
bool Tag::parseBlock( Lexer* lexer, Lexer::Token& tok )
{
    bool notHandled( false );
    if( !document->autoSpacing() ) {    //tag came after a word-punct combo
        document->toggleAutoSpacing();
        document->lastText()->setToggleSpacing();
        document->setLastPrintable( Lexer::TAG, 0 );
    }
    if( tok == Lexer::TAG ) {
        switch( lexer->tagId() ) {
        //make new tag
        //append pointer to this tag's list
        case Lexer::ACVIEWPORT:
            {
                Element* elt( new AcViewport( document, this, document->dataName(),
                    document->lexerLine(), document->lexerCol() ) );
                appendChild( elt );
                tok = elt->parse( lexer );
            }
            break;
        case Lexer::CAUTION:
            {
                Element* elt( new Caution( document, this, document->dataName(),
                    document->lexerLine(), document->lexerCol() ) );
                appendChild( elt );
                tok = elt->parse( lexer );
            }
            break;
        case Lexer::ECAUTION:
            {
                Element* elt( new ECaution( document, this, document->dataName(),
                    document->lexerLine(), document->lexerCol() ) );
                appendChild( elt );
                tok = elt->parse( lexer );
            }
            break;
        case Lexer::CGRAPHIC:
            {
                Element* elt( new CGraphic( document, this, document->dataName(),
                    document->lexerLine(), document->lexerCol() ) );
                appendChild( elt );
                tok = elt->parse( lexer );
            }
            break;
        case Lexer::ECGRAPHIC:
            {
                Element* elt( new ECGraphic( document, this, document->dataName(),
                    document->lexerLine(), document->lexerCol() ) );
                appendChild( elt );
                tok = elt->parse( lexer );
            }
            break;
        case Lexer::DDF:
            {
                Element* elt( new Ddf( document, this, document->dataName(),
                    document->lexerLine(), document->lexerCol() ) );
                appendChild( elt );
                tok = elt->parse( lexer );
            }
            break;
        case Lexer::FIG:
            {
                Element* elt( new Fig( document, this, document->dataName(),
                    document->lexerLine(), document->lexerCol() ) );
                appendChild( elt );
                tok = elt->parse( lexer );
            }
            break;
        case Lexer::EFIG:
            {
                Element* elt( new EFig( document, this, document->dataName(),
                    document->lexerLine(), document->lexerCol() ) );
                appendChild( elt );
                tok = elt->parse( lexer );
            }
            break;
        case Lexer::LINES:
            {
                Element* elt( new Lines( document, this, document->dataName(),
                    document->lexerLine(), document->lexerCol() ) );
                appendChild( elt );
                tok = elt->parse( lexer );
            }
            break;
        case Lexer::ELINES:
            {
                Element* elt( new ELines( document, this, document->dataName(),
                    document->lexerLine(), document->lexerCol() ) );
                appendChild( elt );
                tok = elt->parse( lexer );
            }
            break;
        case Lexer::NOTE:
            {
                Element* elt( new Note( document, this, document->dataName(),
                    document->lexerLine(), document->lexerCol() ) );
                appendChild( elt );
                tok = elt->parse( lexer );
            }
            break;
        case Lexer::NT:
            {
                Element* elt( new Nt( document, this, document->dataName(),
                    document->lexerLine(), document->lexerCol() ) );
                appendChild( elt );
                tok = elt->parse( lexer );
            }
            break;
        case Lexer::ENT:
            {
                Element* elt( new ENt( document, this, document->dataName(),
                    document->lexerLine(), document->lexerCol() ) );
                appendChild( elt );
                tok = elt->parse( lexer );
            }
            break;
        case Lexer::LI:
        case Lexer::LP:
            document->printError( ERR1_NOLIST );
            while( tok != Lexer::TAGEND )
                tok = document->getNextToken();
            tok = document->getNextToken();
            break;
        case Lexer::P:
            {
                Element* elt( new P( document, this, document->dataName(),
                    document->lexerLine(), document->lexerCol() ) );
                appendChild( elt );
                tok = elt->parse( lexer );
            }
            break;
        case Lexer::TABLE:
            {
                Element* elt( new Table( document, this, document->dataName(),
                    document->lexerLine(), document->lexerCol() ) );
                appendChild( elt );
                tok = elt->parse( lexer );
            }
            break;
        case Lexer::ETABLE:
            {
                Element* elt( new ETable( document, this, document->dataName(),
                    document->lexerLine(), document->lexerCol() ) );
                appendChild( elt );
                tok = elt->parse( lexer );
            }
            break;
        case Lexer::WARNING:
            {
                Element* elt( new Warning( document, this, document->dataName(),
                    document->lexerLine(), document->lexerCol() ) );
                appendChild( elt );
                tok = elt->parse( lexer );
            }
            break;
        case Lexer::EWARNING:
            {
                Element* elt( new EWarning( document, this, document->dataName(),
                    document->lexerLine(), document->lexerCol() ) );
                appendChild( elt );
                tok = elt->parse( lexer );
            }
            break;
        case Lexer::XMP:
            {
                Element* elt( new Xmp( document, this, document->dataName(),
                    document->lexerLine(), document->lexerCol() ) );
                appendChild( elt );
                tok = elt->parse( lexer );
            }
            break;
        case Lexer::EXMP:
            {
                Element* elt( new EXmp( document, this, document->dataName(),
                    document->lexerLine(), document->lexerCol() ) );
                appendChild( elt );
                tok = elt->parse( lexer );
            }
            break;
        case Lexer::EUSERDOC:
            break;
        default:
            notHandled = true;
            break;
        }
    }
    return notHandled;
}
/***************************************************************************/
//Nestable list elements
bool Tag::parseListBlock( Lexer* lexer, Lexer::Token& tok )
{
    bool notHandled( false );
    if( tok == Lexer::TAG ) {
        switch( lexer->tagId() ) {
        //make new tag
        //append pointer to this tag's list
        case Lexer::DL:
            {
                Element* elt( new Dl( document, this, document->dataName(),
                    document->lexerLine(), document->lexerCol(), 0, document->leftMargin() ) );
                appendChild( elt );
                tok = elt->parse( lexer );
            }
            break;
        case Lexer::OL:
            {
                Element* elt( new Ol( document, this, document->dataName(),
                    document->dataLine(), document->dataCol(), 0, document->leftMargin() ) );
                appendChild( elt );
                tok = elt->parse( lexer );
            }
            break;
        case Lexer::PARML:
                {
                    Element* elt( new Parml( document, this, document->dataName(),
                        document->dataLine(), document->dataCol(), 0, document->leftMargin() ) );
                    appendChild( elt );
                    tok = elt->parse( lexer );
                }
            break;
        case Lexer::SL:
            {
                Element* elt( new Sl( document, this, document->dataName(),
                    document->dataLine(), document->dataCol(), 0, document->leftMargin() ) );
                appendChild( elt );
                tok = elt->parse( lexer );
            }
            break;
        case Lexer::UL:
            {
                Element* elt( new Ul( document, this, document->dataName(),
                    document->dataLine(), document->dataCol(), 0, document->leftMargin() ) );
                appendChild( elt );
                tok = elt->parse( lexer );
            }
            break;
        default:
            notHandled = true;
            break;
        }
    }
    return notHandled;
}
/***************************************************************************/
void Tag::parseCleanup( Lexer* lexer, Lexer::Token& tok )
{
    if( lexer->tagId() == Lexer::BADTAG )
        document->printError( ERR1_TAGNOTDEF );
    else
        document->printError( ERR1_TAGCONTEXT );
    while( tok != Lexer::TAGEND )
        tok = document->getNextToken();
    tok = document->getNextToken();
}
/***************************************************************************/
void Tag::buildIndex()
{
    std::for_each( children.begin(), children.end(), std::mem_fun( &Element::buildIndex ) );
}
/***************************************************************************/
void Tag::linearize( Page* page )
{
    page->addElement( this );
    for( ConstChildrenIter iter = children.begin(); iter != children.end(); ++iter )
        ( *iter )->linearize( page );
}
/***************************************************************************/
void Tag::linearizeChildren( Page* page )
{
    for( ConstChildrenIter iter = children.begin(); iter != children.end(); ++iter )
        ( *iter )->linearize( page );
}
