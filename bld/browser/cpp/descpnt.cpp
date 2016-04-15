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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#include <stdlib.h>
#include <wcvector.h>
#include <wstring.hpp>

#include "symbol.h"
#include "descrip.h"
#include "descpnt.h"


DescriptionPart::DescriptionPart( const char * txt, Symbol * sym,
                                  const WRect & r )
        : _text( txt )
        , _symbol( sym )
        , _rect( r )
//---------------------------------------------------------------------
{
    _rect.h( _rect.h() * 2 );
}

DescriptionPart::~DescriptionPart()
//---------------------------------
{
    delete _symbol;
}

void DescriptionPart::paint( WBRWindow * prnt, bool sel )
//-------------------------------------------------------
{
    Color fg;
    Color bg;

    if( _symbol ) {     // user defined
        if( sel ) {
            fg = ColorBlue;
            bg = ColorBlack;
        } else {
            fg = ColorBlue;
            bg = prnt->backgroundColour();
        }
    } else {
        fg = ColorBlack;
        bg = prnt->backgroundColour();
    }

    prnt->drawText( WPoint( _rect.x(), _rect.y() ),
                    _text.gets(), fg, bg );
}


//-------------------------- DescriptionPaint ------------------------


DescriptionPaint::DescriptionPaint( WBRWindow * prnt, const WRect & r,
                                    Symbol * sym )
                    : _parent( prnt )
                    , _rect( r )
                    , _current( -1 )
//--------------------------------------------------------------------
{
    int             i;
    WVList          desc;
    Description *   entry;
    WString         buf;
    const char *    uDefSymName;
    int             x = r.x();
    int             w;
    int             h;

    _parts = new WCPtrOrderedVector<DescriptionPart>;

    sym->description( desc );

    for( i = 0; i < desc.count(); i += 1 ) {
        entry = (Description *) desc[i];
        if( entry->symbol() ) {
            if( sym->isEqual( entry->symbol() ) ) {

                // don't hilight the symbol we're describing
                buf.concat( entry->name() );
                delete entry->symbol();

            } else {

                if( buf != "" ) { // flush buf
                    w = prnt->getTextExtentX( buf );
                    h = prnt->getTextExtentY( buf );
                    _parts->append( new DescriptionPart( buf.gets(), NULL,
                                        WRect(x,r.y(),w, h ) ) );

                    buf="";
                    x+=w;
                }

                uDefSymName = entry->name();
                w = prnt->getTextExtentX( uDefSymName );
                h = prnt->getTextExtentY( uDefSymName );
                _parts->append( new DescriptionPart( uDefSymName,
                                                     entry->symbol(),
                                                     WRect(x,r.y(),w, h ) ) );
                x+=w;
            }
        } else {
            buf.concat( entry->name() );
        }
    }

    desc.deleteContents();


    if( buf != "" ) { // flush buf
        w = prnt->getTextExtentX( buf );
        h = prnt->getTextExtentY( buf );
        _parts->append( new DescriptionPart( buf, NULL,
                                WRect(x,r.y(),w, h ) ) );

        buf="";
        x+=w;
    }

    _rect.w( x - abs( _rect.x() ) );
}

DescriptionPaint::~DescriptionPaint()
//-----------------------------------
{
    _parts->clearAndDestroy();
    delete _parts;
}

DescriptionPart * DescriptionPaint::findPart( int x, int y, int &idx )
//--------------------------------------------------------------------
{
    int i;
    DescriptionPart * part;

    for( i = 0; i < _parts->entries(); i += 1 ) {
        part = (*_parts)[ i ];

        if( x > part->_rect.x() && x < part->_rect.x() + part->_rect.w() ) {
            if( y > part->_rect.y() && y < part->_rect.y() + part->_rect.h() ) {
                idx = i;
                return part;
            }
        }
    }

    return NULL;
}

bool DescriptionPaint::select( int x, int y )
//-------------------------------------------
{
    bool                ret = false;
    DescriptionPart *   part;
    int                 idx;

    if( x < _rect.x() || x > _rect.x() + _rect.w() )
        return false;

    if( y < _rect.y() || y > _rect.y() + _rect.h() )
        return false;

    part = findPart( x, y, idx );

    if( part && part->_symbol ) {
        _current = idx;
        return true;
    }

    return false;
}

bool DescriptionPaint::tabHit( bool shift )
//-----------------------------------------
{
    int i;
    int count;
    int oCurrent = _current;
    DescriptionPart * part = NULL;

    count = _parts->entries();
    for( i = 0; i < count && !part; i += 1 ) {
        _current += (shift) ? (count - 1) : 1;
        _current %= count;
        if( (*_parts)[ _current ]->_symbol != NULL ) {
            part = (*_parts)[ _current ];
        }
    }
    if( part == NULL ) {
        _current = oCurrent;
    }

    return( oCurrent != _current );
}

bool DescriptionPaint::enterHit()
//-------------------------------
{
    if( _current < 0 )
        return false;

    _parent->popDetail( (*_parts)[ _current ]->_symbol );
    return true;
}

bool DescriptionPaint::paint()
//----------------------------
{
    int                 i;
    DescriptionPart *   entry;

    for( i = 0; i < _parts->entries(); i += 1 ) {
        entry = (*_parts)[ i ];
        entry->paint( _parent, (i == _current) );
    }

    return true;
}

const WRect & DescriptionPaint::partRect( int idx )
//-------------------------------------------------
{
    return( (*_parts)[ idx ]->_rect );
}
