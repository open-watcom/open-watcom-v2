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


// vwcreate.cpp - implementation of class ViewCreator


#include <wvlist.hpp>
#include "assure.h"
#include "symbol.h"
#include "viewsym.h"
#include "strcview.h"
#include "texttree.h"
#include "vwcreate.h"

WVList * ViewCreator::_activeViews[]={ NULL, NULL, NULL };
createFn createTable[] = { (createFn) &ViewSymbol::createView,
                           (createFn) &TextTree::createView,
                           (createFn) &StrucView::createView
                         };

WBRWindow * ViewCreator::findOrCreate( ViewType type, int x, int y,
                                       WBRWindow *prnt, Symbol *info )
/********************************************************************/
{
    WBRWindow * view = NULL;

    REQUIRE( type >= 0 && type < CreateLastView, "vwcreate@findorcreate - invalid type!" );

    if( _activeViews[ type ] ) {
        // look for equivalent view in the active set
        view = find( type, info );
        if( view ) {
            view->show();
            // ITB NYI broken: view->bringToTop();
            return view;
        }
    }
    return create( type, x, y, prnt, info );
}

WBRWindow * ViewCreator::find( ViewType type, Symbol *info )
/**********************************************************/
{
    REQUIRE( type >= 0 && type < CreateLastView, "vwcreate@find - invalid type!" );

    return (WBRWindow *)_activeViews[ type ]->find( info );
}

WBRWindow * ViewCreator::create( ViewType type, int x, int y,
                                 WBRWindow *prnt, const Symbol *sym )
/********************************************************************/
{
    REQUIRE( type >= 0 && type < CreateLastView, "vwcreate@create - invalid type!" );

    WBRWindow * view = NULL;

    view = (createTable[type])( x, y, prnt, sym );
    if( _activeViews[ type ] == NULL ) {
        _activeViews[ type ] = new WVList;
    }
    _activeViews[ type ]->add( view );
    return view;
}

void ViewCreator::viewDying( ViewType type, WBRWindow * view )
/************************************************************/
{
    if( _activeViews[ type ] ) {
        _activeViews[ type ]->removeSame( view );
        if( _activeViews[ type ]->count() == 0 ) {
            delete _activeViews[ type ];
            _activeViews[ type ]=NULL;
        }
    }
}
