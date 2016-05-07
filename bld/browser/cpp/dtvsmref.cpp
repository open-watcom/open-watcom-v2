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


#include "busyntc.h"
#include "dtvsmref.h"
#include "dbmgr.h"
#include "icons.h"
#include "module.h"
#include "symbol.h"
#include "viewmgr.h"
#include "brwhelp.h"

DTVSymbolRefs::DTVSymbolRefs( const Symbol * sym )
        : HotWindowList( "" )
//------------------------------------------------
{
    WString str;
    _symbol = new Symbol( *sym );

    viewManager()->registerForEvents( this );
    str.printf( "Symbols Using %s", _symbol->name() );
    setText( str );

    BusyNotice busy( "Loading..." );
    dbManager()->module()->findRefSyms( &_users, _symbol );

    onDblClick( this, (cbw) &DTVSymbolRefs::toggleDetail );
    onHotPress( this, (cbw) &DTVSymbolRefs::toggleDetail );
    reset();

    setIcon( DTSymbolRefs );
}

DTVSymbolRefs::~DTVSymbolRefs()
//---------------------------
{
    delete _symbol;
    _users.deleteContents();
}

bool DTVSymbolRefs::contextHelp( bool is_active_win )
//---------------------------------------------------
{
    if( is_active_win ) {
        WBRWinBase::helpInfo()->sysHelpId( BRH_SYMBOL_REFERENCES );
    }
    return( true );
}

int DTVSymbolRefs::count()
//------------------------
{
    return _users.count();
}

const char * DTVSymbolRefs::getString( int index )
//------------------------------------------------
{
    return ((Symbol *)_users[ index ])->scopedName();
}

int DTVSymbolRefs::getHotSpot( int index, bool pressed )
//------------------------------------------------------
{
    return ((Symbol *)_users[ index ])->getHotSpot( pressed );
}

void DTVSymbolRefs::toggleDetail( WWindow * )
//--------------------------------------------
{
    int             sel = selected();
    Symbol *        sym;
    DetailView *    dtv;

    if( 0 <= sel && sel < _users.count() ) {
        sym = (Symbol *) _users[ sel ];
        dtv = viewManager()->findDetailView( sym, DetailView::ViewDetail );

        if( !dtv ) {
            popDetail( sym );
        } else {
            dtv->aboutToClose();
            viewManager()->kill();
        }
    }
}

void DTVSymbolRefs::event( ViewEvent ve, View * view )
//----------------------------------------------------
{
    if( ve == VEClose || ve == VECreate ) {
        if( view->identity() == VIDetailView ) {
            int             i;
            int             maxRows = getRows() + _topIndex;
            DetailView *    dtv = (DetailView *) view;
            drmem_hdl       drhdl = dtv->symHandle();

            if( _users.count() < maxRows ) {
                maxRows = _users.count();
            }

            for( i = _topIndex; i < maxRows; i += 1 ) {
                Symbol * sym = (Symbol *) _users[ i ];

                if( sym == NULL ) {
                    break;
                }
                if( drhdl == sym->getHandle() ) {
                    invalidateRow( i - _topIndex );
                    break;
                }
            }
        }
    } else {
        DetailView::event( ve, view );
    }
}

ViewEvent DTVSymbolRefs::wantEvents()
//-----------------------------------
{
    return( VECreate | VEClose | DetailView::wantEvents() );
};
