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
* Description:  A abstract class for a tag element
*
****************************************************************************/


#include "wipfc.hpp"
#include <functional>
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
    for( ChildrenIter itr = _children.begin(); itr != _children.end(); ++itr ) {
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
    Lexer::Token tok;

    (void)lexer;

    while( (tok = _document->getNextToken()) != Lexer::TAGEND ) {
        if( tok == Lexer::ATTRIBUTE ) {
            _document->printError( ERR1_NOATTR );
        } else if( tok == Lexer::FLAG ) {
            _document->printError( ERR1_NOATTR );
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
bool Tag::parseInline( Lexer* lexer, Lexer::Token& tok )
{
    bool notHandled( false );
    if( tok == Lexer::WORD ) {
        TextWord* w( new TextWord( _document, this, _document->dataName(),
            _document->lexerLine(), _document->lexerCol(), _whiteSpace ) );
        appendChild( w );
        tok = w->parse( lexer );
    } else if( tok == Lexer::ENTITY ) {
        Entity* entity( new Entity( _document, this, _document->dataName(),
            _document->lexerLine(), _document->lexerCol(), _whiteSpace ) );
        appendChild( entity );
        tok = entity->parse( lexer );
    } else if( tok == Lexer::PUNCTUATION ) {
        Punctuation* punct( new Punctuation( _document, this, _document->dataName(),
            _document->lexerLine(), _document->lexerCol(), _whiteSpace ) );
        appendChild( punct );
        tok = punct->parse( lexer );
    } else if( tok == Lexer::WHITESPACE ) {
        WhiteSpace* ws( new WhiteSpace( _document, this, _document->dataName(),
        _document->lexerLine(), _document->lexerCol(), _whiteSpace ) );
        appendChild( ws );
        tok = ws->parse( lexer );
    } else if( tok == Lexer::COMMAND ) {
        _document->parseCommand( lexer, this );
        tok = _document->getNextToken();
    } else if( tok == Lexer::TAG ) {
        switch( lexer->tagId() ) {
        //make new tag
        //append pointer to this tag's list
        case Lexer::ARTLINK:
            {
                Artlink *artlink = new Artlink( _document, this, _document->dataName(),
                    _document->lexerLine(), _document->lexerCol() );
                appendChild( artlink );
                tok = artlink->parse( lexer );
            }
            break;
        case Lexer::EARTLINK:
            {
                EArtlink *eartlink = new EArtlink( _document, this, _document->dataName(),
                    _document->lexerLine(), _document->lexerCol() );
                appendChild( eartlink );
                tok = eartlink->parse( lexer );
            }
            break;
        case Lexer::ARTWORK:
            {
                Artwork *artwork = new Artwork( _document, this, _document->dataName(),
                    _document->lexerLine(), _document->lexerCol() );
                appendChild( artwork );
                tok = artwork->parse( lexer );
            }
            break;
        case Lexer::COLOR:
            {
                Color *color = new Color( _document, this, _document->dataName(),
                    _document->lexerLine(), _document->lexerCol() );
                appendChild( color );
                tok = color->parse( lexer );
            }
            break;
        case Lexer::FONT:
            {
                Font *font = new Font( _document, this, _document->dataName(),
                    _document->lexerLine(), _document->lexerCol() );
                appendChild( font );
                tok = font->parse( lexer );
            }
            break;
        case Lexer::HDREF:
            {
                Hdref *hdref = new Hdref( _document, this, _document->dataName(),
                    _document->lexerLine(), _document->lexerCol() );
                appendChild( hdref );
                tok = hdref->parse( lexer );
            }
            break;
        case Lexer::HIDE:
            {
                Hide *hide = new Hide( _document, this, _document->dataName(),
                    _document->lexerLine(), _document->lexerCol() );
                appendChild( hide );
                tok = hide->parse( lexer );
            }
            break;
        case Lexer::EHIDE:
            {
                EHide *ehide = new EHide( _document, this, _document->dataName(),
                    _document->lexerLine(), _document->lexerCol() );
                appendChild( ehide );
                tok = ehide->parse( lexer );
            }
            break;
        case Lexer::HP1:
            {
                Hpn *hpn = new Hpn( _document, this, _document->dataName(),
                    _document->lexerLine(), _document->lexerCol(), 1 );
                appendChild( hpn );
                tok = hpn->parse( lexer );
            }
            break;
        case Lexer::HP2:
            {
                Hpn *hpn = new Hpn( _document, this, _document->dataName(),
                    _document->lexerLine(), _document->lexerCol(), 2 );
                appendChild( hpn );
                tok = hpn->parse( lexer );
            }
            break;
        case Lexer::HP3:
            {
                Hpn *hpn = new Hpn( _document, this, _document->dataName(),
                    _document->lexerLine(), _document->lexerCol(), 3 );
                appendChild( hpn );
                tok = hpn->parse( lexer );
            }
            break;
        case Lexer::HP4:
            {
                Hpn *hpn = new Hpn( _document, this, _document->dataName(),
                    _document->lexerLine(), _document->lexerCol(), 4 );
                appendChild( hpn );
                tok = hpn->parse( lexer );
            }
            break;
        case Lexer::HP5:
            {
                Hpn *hpn = new Hpn( _document, this, _document->dataName(),
                    _document->lexerLine(), _document->lexerCol(), 5 );
                appendChild( hpn );
                tok = hpn->parse( lexer );
            }
            break;
        case Lexer::HP6:
            {
                Hpn *hpn = new Hpn( _document, this, _document->dataName(),
                    _document->lexerLine(), _document->lexerCol(), 6 );
                appendChild( hpn );
                tok = hpn->parse( lexer );
            }
            break;
        case Lexer::HP7:
            {
                Hpn *hpn = new Hpn( _document, this, _document->dataName(),
                    _document->lexerLine(), _document->lexerCol(), 7 );
                appendChild( hpn );
                tok = hpn->parse( lexer );
            }
            break;
        case Lexer::HP8:
            {
                Hpn *hpn = new Hpn( _document, this, _document->dataName(),
                    _document->lexerLine(), _document->lexerCol(), 8 );
                appendChild( hpn );
                tok = hpn->parse( lexer );
            }
            break;
        case Lexer::HP9:
            {
                Hpn *hpn = new Hpn( _document, this, _document->dataName(),
                    _document->lexerLine(), _document->lexerCol(), 9 );
                appendChild( hpn );
                tok = hpn->parse( lexer );
            }
            break;
        case Lexer::EHP1:
            {
                EHpn *ehpn = new EHpn( _document, this, _document->dataName(),
                    _document->lexerLine(), _document->lexerCol(), 1 );
                appendChild( ehpn );
                tok = ehpn->parse( lexer );
            }
            break;
        case Lexer::EHP2:
            {
                EHpn *ehpn = new EHpn( _document, this, _document->dataName(),
                    _document->lexerLine(), _document->lexerCol(), 2 );
                appendChild( ehpn );
                tok = ehpn->parse( lexer );
            }
            break;
        case Lexer::EHP3:
            {
                EHpn *ehpn = new EHpn( _document, this, _document->dataName(),
                    _document->lexerLine(), _document->lexerCol(), 3 );
                appendChild( ehpn );
                tok = ehpn->parse( lexer );
            }
            break;
        case Lexer::EHP4:
            {
                EHpn *ehpn = new EHpn( _document, this, _document->dataName(),
                    _document->lexerLine(), _document->lexerCol(), 4 );
                appendChild( ehpn );
                tok = ehpn->parse( lexer );
            }
            break;
        case Lexer::EHP5:
            {
                EHpn *ehpn = new EHpn( _document, this, _document->dataName(),
                    _document->lexerLine(), _document->lexerCol(), 5 );
                appendChild( ehpn );
                tok = ehpn->parse( lexer );
            }
            break;
        case Lexer::EHP6:
            {
                EHpn *ehpn = new EHpn( _document, this, _document->dataName(),
                    _document->lexerLine(), _document->lexerCol(), 6 );
                appendChild( ehpn );
                tok = ehpn->parse( lexer );
            }
            break;
        case Lexer::EHP7:
            {
                EHpn *ehpn = new EHpn( _document, this, _document->dataName(),
                    _document->lexerLine(), _document->lexerCol(), 7 );
                appendChild( ehpn );
                tok = ehpn->parse( lexer );
            }
            break;
        case Lexer::EHP8:
            {
                EHpn *ehpn = new EHpn( _document, this, _document->dataName(),
                    _document->lexerLine(), _document->lexerCol(), 8 );
                appendChild( ehpn );
                tok = ehpn->parse( lexer );
            }
            break;
        case Lexer::EHP9:
            {
                EHpn *ehpn = new EHpn( _document, this, _document->dataName(),
                    _document->lexerLine(), _document->lexerCol(), 9 );
                appendChild( ehpn );
                tok = ehpn->parse( lexer );
            }
            break;
        case Lexer::LINK:
            {
                Link *link = new Link( _document, this, _document->dataName(),
                    _document->lexerLine(), _document->lexerCol(), _whiteSpace );
                appendChild( link );
                tok = link->parse( lexer );
            }
            break;
        case Lexer::ELINK:
            {
                ELink *elink = new ELink( _document, this, _document->dataName(),
                    _document->lexerLine(), _document->lexerCol() );
                appendChild( elink );
                tok = elink->parse( lexer );
            }
            break;
        case Lexer::LM:
            {
                Lm *lm = new Lm( _document, this, _document->dataName(),
                    _document->lexerLine(), _document->lexerCol() );
                appendChild( lm );
                tok = lm->parse( lexer );
            }
            break;
        case Lexer::RM:
            {
                Rm *rm = new Rm( _document, this, _document->dataName(),
                    _document->lexerLine(), _document->lexerCol() );
                appendChild( rm );
                tok = rm->parse( lexer );
            }
            break;
        case Lexer::EUSERDOC:
            break;
        default:
            notHandled = true;
            break;
        }
    } else if( tok == Lexer::ERROR_TAG ) {
        _document->printError( ERR1_TAGNOTDEF );
        tok = _document->getNextToken();
    } else if( tok == Lexer::ERROR_ENTITY ) {
        _document->printError( ERR1_TAGNOTDEF );
        tok = _document->getNextToken();
    }
    return notHandled;
}
/***************************************************************************/
bool Tag::parseBlock( Lexer* lexer, Lexer::Token& tok )
{
    bool notHandled( false );
    if( !_document->autoSpacing() ) {    //tag came after a word-punct combo
        _document->toggleAutoSpacing();
        _document->lastText()->setToggleSpacing();
        _document->setLastPrintable( Lexer::TAG, 0 );
    }
    if( tok == Lexer::TAG ) {
        switch( lexer->tagId() ) {
        //make new tag
        //append pointer to this tag's list
        case Lexer::ACVIEWPORT:
            {
                AcViewport *acviewport = new AcViewport( _document, this, _document->dataName(),
                    _document->lexerLine(), _document->lexerCol() );
                appendChild( acviewport );
                tok = acviewport->parse( lexer );
            }
            break;
        case Lexer::CAUTION:
            {
                Caution *caution = new Caution( _document, this, _document->dataName(),
                    _document->lexerLine(), _document->lexerCol() );
                appendChild( caution );
                tok = caution->parse( lexer );
            }
            break;
        case Lexer::ECAUTION:
            {
                ECaution *ecaution = new ECaution( _document, this, _document->dataName(),
                    _document->lexerLine(), _document->lexerCol() );
                appendChild( ecaution );
                tok = ecaution->parse( lexer );
            }
            break;
        case Lexer::CGRAPHIC:
            {
                CGraphic *cgraphic = new CGraphic( _document, this, _document->dataName(),
                    _document->lexerLine(), _document->lexerCol() );
                appendChild( cgraphic );
                tok = cgraphic->parse( lexer );
            }
            break;
        case Lexer::ECGRAPHIC:
            {
                ECGraphic *ecgraphic = new ECGraphic( _document, this, _document->dataName(),
                    _document->lexerLine(), _document->lexerCol() );
                appendChild( ecgraphic );
                tok = ecgraphic->parse( lexer );
            }
            break;
        case Lexer::DDF:
            {
                Ddf *ddf = new Ddf( _document, this, _document->dataName(),
                    _document->lexerLine(), _document->lexerCol() );
                appendChild( ddf );
                tok = ddf->parse( lexer );
            }
            break;
        case Lexer::FIG:
            {
                Fig *fig = new Fig( _document, this, _document->dataName(),
                    _document->lexerLine(), _document->lexerCol() );
                appendChild( fig );
                tok = fig->parse( lexer );
            }
            break;
        case Lexer::EFIG:
            {
                EFig *efig = new EFig( _document, this, _document->dataName(),
                    _document->lexerLine(), _document->lexerCol() );
                appendChild( efig );
                tok = efig->parse( lexer );
            }
            break;
        case Lexer::LINES:
            {
                Lines *lines = new Lines( _document, this, _document->dataName(),
                    _document->lexerLine(), _document->lexerCol() );
                appendChild( lines );
                tok = lines->parse( lexer );
            }
            break;
        case Lexer::ELINES:
            {
                ELines *elines = new ELines( _document, this, _document->dataName(),
                    _document->lexerLine(), _document->lexerCol() );
                appendChild( elines );
                tok = elines->parse( lexer );
            }
            break;
        case Lexer::NOTE:
            {
                Note *note = new Note( _document, this, _document->dataName(),
                    _document->lexerLine(), _document->lexerCol() );
                appendChild( note );
                tok = note->parse( lexer );
            }
            break;
        case Lexer::NT:
            {
                Nt *nt = new Nt( _document, this, _document->dataName(),
                    _document->lexerLine(), _document->lexerCol() );
                appendChild( nt );
                tok = nt->parse( lexer );
            }
            break;
        case Lexer::ENT:
            {
                ENt *ent = new ENt( _document, this, _document->dataName(),
                    _document->lexerLine(), _document->lexerCol() );
                appendChild( ent );
                tok = ent->parse( lexer );
            }
            break;
        case Lexer::LI:
        case Lexer::LP:
            _document->printError( ERR1_NOLIST );
            while( tok != Lexer::TAGEND )
                tok = _document->getNextToken();
            tok = _document->getNextToken();
            break;
        case Lexer::P:
            {
                P *p= new P( _document, this, _document->dataName(),
                    _document->lexerLine(), _document->lexerCol() );
                appendChild( p );
                tok = p->parse( lexer );
            }
            break;
        case Lexer::TABLE:
            {
                Table *table = new Table( _document, this, _document->dataName(),
                    _document->lexerLine(), _document->lexerCol() );
                appendChild( table );
                tok = table->parse( lexer );
            }
            break;
        case Lexer::ETABLE:
            {
                ETable *etable = new ETable( _document, this, _document->dataName(),
                    _document->lexerLine(), _document->lexerCol() );
                appendChild( etable );
                tok = etable->parse( lexer );
            }
            break;
        case Lexer::WARNING:
            {
                Warning *warning = new Warning( _document, this, _document->dataName(),
                    _document->lexerLine(), _document->lexerCol() );
                appendChild( warning );
                tok = warning->parse( lexer );
            }
            break;
        case Lexer::EWARNING:
            {
                EWarning *ewarning = new EWarning( _document, this, _document->dataName(),
                    _document->lexerLine(), _document->lexerCol() );
                appendChild( ewarning );
                tok = ewarning->parse( lexer );
            }
            break;
        case Lexer::XMP:
            {
                Xmp *xmp = new Xmp( _document, this, _document->dataName(),
                    _document->lexerLine(), _document->lexerCol() );
                appendChild( xmp );
                tok = xmp->parse( lexer );
            }
            break;
        case Lexer::EXMP:
            {
                EXmp *exmp = new EXmp( _document, this, _document->dataName(),
                    _document->lexerLine(), _document->lexerCol() );
                appendChild( exmp );
                tok = exmp->parse( lexer );
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
                Dl *dl = new Dl( _document, this, _document->dataName(),
                    _document->lexerLine(), _document->lexerCol(), 0, _document->leftMargin() );
                appendChild( dl );
                tok = dl->parse( lexer );
            }
            break;
        case Lexer::OL:
            {
                Ol *ol = new Ol( _document, this, _document->dataName(),
                    _document->dataLine(), _document->dataCol(), 0, _document->leftMargin() );
                appendChild( ol );
                tok = ol->parse( lexer );
            }
            break;
        case Lexer::PARML:
            {
                Parml *parml = new Parml( _document, this, _document->dataName(),
                    _document->dataLine(), _document->dataCol(), 0, _document->leftMargin() );
                appendChild( parml );
                tok = parml->parse( lexer );
            }
            break;
        case Lexer::SL:
            {
                Sl *sl = new Sl( _document, this, _document->dataName(),
                    _document->dataLine(), _document->dataCol(), 0, _document->leftMargin() );
                appendChild( sl );
                tok = sl->parse( lexer );
            }
            break;
        case Lexer::UL:
            {
                Ul *ul = new Ul( _document, this, _document->dataName(),
                    _document->dataLine(), _document->dataCol(), 0, _document->leftMargin() );
                appendChild( ul );
                tok = ul->parse( lexer );
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
    if( lexer->tagId() == Lexer::BADTAG ) {
        _document->printError( ERR1_TAGNOTDEF );
    } else {
        _document->printError( ERR1_TAGCONTEXT );
    }
    while( tok != Lexer::TAGEND )
        tok = _document->getNextToken();
    tok = _document->getNextToken();
}
/***************************************************************************/
void Tag::buildIndex()
{
    for( ConstChildrenIter iter = _children.begin(); iter != _children.end(); ++iter ) {
        ( *iter )->buildIndex();
    }
}

/***************************************************************************/
void Tag::linearize( Page* page )
{
    page->addElement( this );
    for( ConstChildrenIter iter = _children.begin(); iter != _children.end(); ++iter ) {
        ( *iter )->linearize( page );
    }
}
/***************************************************************************/
void Tag::linearizeChildren( Page* page )
{
    for( ConstChildrenIter iter = _children.begin(); iter != _children.end(); ++iter ) {
        ( *iter )->linearize( page );
    }
}
