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


#include <wstring.hpp>
#include <wlistbox.hpp>
#include <wvlist.hpp>

#include "dtvfunc.h"
#include "funcsym.h"
#include "util.h"
#include "menuids.h"
#include "viewmgr.h"

DTViewFunction::DTViewFunction( const Symbol * sym )
                : DTViewSymbol( sym, TRUE )
//--------------------------------------------------
{
    _locals = new WVList;

    _symbolBox->onDblClick( this, (cbw) &DTViewFunction::elementClicked );

    setup();
}


DTViewFunction::~DTViewFunction()
//-------------------------------
{
    _locals->deleteContents();
    delete _locals;
}

void DTViewFunction::setup()
//--------------------------
{
    int             i;
    WString         str;
    FunctionSym *   func = (FunctionSym *) _symbol;
    Symbol *        sym;


    func->localVars( *_locals );

    for( i = 0; i < _locals->count(); i += 1 ) {
        sym = (Symbol *) (*_locals)[ i ];
        _symbolBox->insertString( sym->scopedName() );
    }
}

void DTViewFunction::setMenus( MenuManager * mgr )
//--------------------------------------------
{
    DTViewSymbol::setMenus( mgr );

#ifdef DETAIL_STUFF_IMPLEMENTED
    mgr->registerForMenu( this, MIMenuID( MMDetail, DMCalls ) );
#endif
}

void DTViewFunction::unsetMenus( MenuManager * mgr )
//----------------------------------------------
{
    DTViewSymbol::unsetMenus( mgr );

#ifdef DETAIL_STUFF_IMPLEMENTED
    mgr->unRegister( MIMenuID( MMDetail, DMCalls ) );
#endif
}

void DTViewFunction::menuSelected( const MIMenuID & id )
//------------------------------------------------------
{
#ifdef DETAIL_STUFF_IMPLEMENTED
    switch( id.mainID() ) {
    case MMDetail:
        switch( id.subID() ) {
        case DMCalls:
            viewManager()->showDetailView( _symbol, ViewCalls );
            break;
        }
    }
#endif

    DTViewSymbol::menuSelected( id );
}

void DTViewFunction::elementClicked( WWindow * )
//----------------------------------------------
{
    int             sel = _symbolBox->selected();
    const Symbol *  sym;

    if( sel < 0 ) {
        return;
    }

    sym = (Symbol *) (*_locals)[ sel ];

    if( sym ) {
        popDetail( sym );
    }
}
