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


#include <wcvector.h>

#include <whotspot.hpp>

#include "hotspots.h"
#include "modlist.h"

ModuleList::ModuleList( WWindow * prt, const WRect & r )
                    : HotControlList( prt, r )
//----------------------------------------------------------------------------
// note that this ctor doesn't do a reset, so the owner has to
{
    _modItems = new WCPtrOrderedVector<ModuleItem>;

    onHotPress( this, (cbw) toggleEnable );
}

ModuleList::~ModuleList()
//-----------------------
{
    _modItems->clearAndDestroy();
    delete _modItems;
}

int ModuleList::count()
//---------------------
{
    return _modItems->entries();
}

ModuleItem * ModuleList::operator [] ( int index )
//------------------------------------------------
{
    return (*_modItems)[ index ];
}

const char * ModuleList::getString( int index )
//---------------------------------------------
{
    return (*_modItems)[ index ]->_str->gets();
}

int ModuleList::getHotSpot( int index, bool pressed )
//---------------------------------------------------
{
    if( (*_modItems)[ index ]->_enabled ) {
        if( !pressed ) {
            return CheckBoxCheckedU;
        } else {
            return CheckBoxCheckedD;
        }
    } else {
        if( !pressed ) {
            return CheckBoxUncheckedU;
        } else {
            return CheckBoxUncheckedD;
        }
    }
}

void ModuleList::insert( ModuleItem * itm )
//-----------------------------------------
{
    _modItems->append( itm );
}

void ModuleList::toggleEnable( WWindow * )
//----------------------------------------
{
    if( _selected < 0 ) {
        return;
    }
    bool state = !(*_modItems)[ _selected ]->_enabled;
    (*_modItems)[ _selected ]->_enabled = state;

    WPoint point;
    WRect rect;
    getRowPoint( _selected - _topIndex, point );
    rect.x( point.x() + getHotOffset( _selected ) );
    rect.y( point.y() );
    GlobalHotSpots->hotSpotSize( getHotSpot( _selected, state ), point );
    rect.w( point.x() );
    rect.h( point.y() );
    invalidateRect( rect );
}

ModuleItem * ModuleList::remove( int index )
//------------------------------------------
{
    ModuleItem * item;

    item = _modItems->removeAt( index );
    _selected = index - 1;

    if( _selected < 0 ) {
        _selected = 0;
    }

    if( _selected >= count() ) {
        _selected = count() - 1;
    }

    reset();

    return item;
}
