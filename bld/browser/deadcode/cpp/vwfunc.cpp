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


#include <wpshbttn.hpp>
#include <wlistbox.hpp>
#include <wcheckbx.hpp>
#include <wtext.hpp>
#include "wbrdefs.h"
#include "assure.h"
#include "symbol.h"
#include "opicklst.h"
#include "vwcreate.h"
#include "funcsym.h"
#include "fnctxttr.h"
#include "vwfunc.h"

bool ViewFunction::_localsOn=FALSE;

ViewFunction::ViewFunction()
    :_showLocals(NULL)
    ,_localsList(NULL)
    ,_localsDesc(NULL)
{
}

ViewFunction::ViewFunction( int x, int y, WBRWindow * prnt, const Symbol *info,
                            bool derived )
    :ViewSymbol( x, y, prnt, info, TRUE/*derived call*/ )
    ,_showLocals(NULL)
    ,_localsList(NULL)
    ,_localsDesc(NULL)
{
    if( !derived ) {
        refresh();
        setup();
    } // else derived class will call these
}

ViewFunction::~ViewFunction()
{
    _locals.deleteContents();
}

void ViewFunction::setup()
/************************/
{
    WVList & kids = children();
    ulong startIndex = kids.count();

    ViewSymbol::setup();

    _showLocals = new WCheckBox( this, WRect(viewLeft,0,140,viewHeight),
                                 "Show &Locals" );
//    _showLocals->setFont(stdFont);
    _showLocals->onClick( this, (cbw)&ViewFunction::showLocals );
    _showLocals->setCheck( _localsOn );
    _showLocals->show();
    showLocals( NULL );

    WPushButton * bttn;
    bttn = new WPushButton( this, WRect(viewLeft,0,140,viewHeight),
                            "Show &Call Tree" );
//  bttn->setFont(stdFont);
    bttn->onClick( this, (cbw)&ViewFunction::createCallTree );
    bttn->show();

    arrangeKids( startIndex );
}

void ViewFunction::refresh()
/**************************/
{
    ViewSymbol::refresh();
    if( _localsOn ) {
        loadLocals();
    }
}

ViewSymbol * ViewFunction::create( int x, int y, WBRWindow *prnt,
                                                 const Symbol *info )
/*******************************************************************/
{
    return new ViewFunction( x, y, prnt, info );
}

void ViewFunction::showLocals( WWindow * )
/****************************************/
{
    showList( &_localsList, &_localsDesc, _showLocals,
              methodOf(ViewFunction,loadLocals),
              _localsOn, pickCallback(&ViewFunction::selectLocal) );
}

void ViewFunction::createCallTree( WWindow * )
/********************************************/
{
    WRect rect;

    getRectangle( rect, TRUE );

    ViewCreator::findOrCreate( CreateInheritView, rect.x(),
                               rect.y() + rect.h(),
                               (WBRWindow *)parent(), _model );
}

void ViewFunction::loadLocals()
/*****************************/
{
    if( _localsList ) {
        _locals.deleteContents();
        ((FunctionSym *)_model)->localVars( _locals );
        _localsList->setObjectList( _locals, (nameMethod)&Symbol::copyBaseName);
    }
}

void ViewFunction::selectLocal( WObject * obj )
/*********************************************/
// could refresh attached local view
{
    Symbol * sym = (Symbol *) obj;
    _localsDesc->setText( sym->scopedName() );
}
