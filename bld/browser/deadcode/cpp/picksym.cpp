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
#include <weditbox.hpp>
#include <wcmdlist.hpp>
#include <wcombox.hpp>
#include <wpshbttn.hpp>
#include <winflist.hpp>
#include <wvlist.hpp>
#include <wtext.hpp>
#include <wname.hpp>
#include "wbrdefs.h"
#include "assure.h"
#include "util.h"
#include "projmodl.h"
#include "symbol.h"
#include "viewsym.h"
#include "vwcreate.h"
#include "keysym.h"
#include "picksym.h"

const int startW = 245;
const int startH = 300;

static char* symTypeNames[] = { "ALL"
                                ,"Functions"
                                ,"Classes"
                                ,"Types"
                                ,"Variables"
                                ,NULL
                                ,NULL
                                ,"Macros"
                                ,NULL
                                };

bool PickSymbol::_showDetails = FALSE;

PickSymbol::PickSymbol( ProjectModel *project, int x, int y, WBRWindow * prnt,
                        dr_search symType, bool /*showPin*/ )
    : WBRWindow( prnt, WRect(x,y,startW,startH), NULL )
    , _project( project )
    , _symbolName(new WCommandList( this, WRect(112,20,-5,-5), NULL))
    , _fileName(new WCommandList( this, WRect(112,62,-5,-5), NULL))
    , _baseClass(new WCommandList( this, WRect(112,104,-5,-5), NULL))
    , _containingFunc(new WCommandList( this, WRect(112,146,-5,-5), NULL))
    , _symbolTypeSelect(new WComboBox( this, WRect(5,20,98,124), NULL ))
    , _locateButton(new WPushButton( this, WRect(10,58,80,28), "&Locate" ))
    , _detailButton(new WPushButton( this, WRect(10,100,80,28),"Show &Info"))
    , _okButton(new WPushButton( this, WRect(10, 144, 80, 28),"&OK"))
    , _list(new WInfiniteList( this, WRect(6,178,-5,-2-22), "Matching Symbols",
                               this, (ocbloi)&PickSymbol::findSymbols,
                               (ccbo)&PickSymbol::getName ))
    , _symbolDescription(new WText( this, WRect(6,-22,-5,-5), "",
                                    TextStyleLeftNoWrap|TextStyleAmpersands))
    , _readyToLoad( FALSE )
{
    setTitle(symType);

    WName * tb;
    tb = new WName( this, WRect(112,4,-5,16), _symbolName, "Symbol &Name" );
    tb->show();

    tb = new WName( this, WRect(112,46,-5,16), _fileName, "&File Name" );
    tb->show();

    tb = new WName( this, WRect(112,88,-5,16), _baseClass, "&Base Class" );
    tb->show();

    tb = new WName( this, WRect(112,130,-5,16), _containingFunc,
                    "&Containing Function" );
    tb->show();

    #if 0
    tb = new WName( this, WRect(8,178,128,16),
                    _list, "Matching &Symbols:" );
    tb->show();
    #endif

    _symbolName->show();

    _fileName->show();

    _baseClass->show();

    _containingFunc->show();

    for( int i=0; symTypeNames[ i ] != NULL; i++ ) {
        _symbolTypeSelect->insertString( symTypeNames[i] );
    }
    _symbolTypeSelect->select( symType );
    _symbolTypeSelect->onChanged( this, (cbw)&PickSymbol::symbolTypeSet );
    _symbolTypeSelect->show();
    if( symType != DR_SEARCH_ALL ) {
        _filter.setSymType( symType );
    }

    _detailButton->onClick( this, (cbw)&PickSymbol::detailSelected );
    _detailButton->show();

    _okButton->onClick( this, (cbw)&PickSymbol::okSelected );
    _okButton->show();

    _locateButton->onClick( this, (cbw)&PickSymbol::locateSymbols );
    _locateButton->show();

    _list->onChanged( this, (cbw)&PickSymbol::select );
    _list->onDblClick( this, (cbw)&PickSymbol::dblSelect );
    _list->show();

//  _symbolDescription(stdFont);
    _symbolDescription->show();

    show();
    _symbolName->setFocus();
}

PickSymbol::~PickSymbol()
{
    delete _symbolName;
    delete _fileName;
    delete _baseClass;
    delete _containingFunc;
    delete _symbolTypeSelect;
    delete _locateButton;
    delete _detailButton;
    delete _okButton;
    delete _list;
    delete _symbolDescription;
}


void PickSymbol::locateSymbols( WWindow * )
/*****************************************/
{
    WString query;

    _fileName->getCommand( query );
    _filter.setFile( query );

    _baseClass->getCommand( query );
    _filter.setBaseClass( query );

    _symbolName->getCommand( query );
    _filter.setName( query );

    _containingFunc->getCommand( query );
    _filter.setFunction( query );

    _readyToLoad = TRUE;

    _list->reset();
    if( _list->count() == 0 ) {
        dr_search filt = _filter.getSymType();

        REQUIRE(filt != DR_SEARCH_NOT_SYM,"pickSymbols::findSymbols barf!");

        errMessage( "Found no matching %s.",
                    (filt == DR_SEARCH_ALL) ? "symbols" : symTypeNames[filt]);
        _symbolDescription->setText( "" );
        _fileName->setFocus();
    }
}

char const * PickSymbol::getName( Symbol * sym )
/**********************************************/
{
    REQUIRE( sym != NULL, "symbol is NULL in PickSymbol::getName" );
    static char buff[ MAX_DISP_NAME_LEN ];
    char * s;

    s = sym->copyBaseName( buff );
    return s;
}

WObject * PickSymbol::findSymbols( WVList * lst, ProjSearchCtxt * ctxt,
                                   int n )
/**********************************************************************/
{
    ProjSearchCtxt * ret;

    if( !_readyToLoad ) {
        return NULL;
    }

    try {
        ret = _project->findSymbols( lst, &_filter, ctxt, n );
    } catch( CauseOfDeath cause ) {
        IdentifyAssassin( cause );
    }

    return ret;
}

void PickSymbol::enterHit()
/*************************/
{
#if 0    // NYI
    if (getChildFocus() == &_listBox) {
        popDetail(_listBox->selectedItem());
    } else {
        if( changed() ) {       // NYI: changed() is bogus. trap onChanged
            findSymbols();      // and keep track of it ourself.
        }
    }
#endif
}

void PickSymbol::select( WWindow * )
/**********************************/
// Called when the user made a selection from the list box.
{
    // NYI: possible communication to pinned dialogs
    Symbol * sym = (Symbol *) _list->selected();

    if( sym ) {
        _symbolDescription->setText( sym->scopedName() );
    }
}

void PickSymbol::dblSelect( WWindow * )
/*************************************/
//    Called when the user double clicked on item in the list box.
{
    Symbol * sym = (Symbol *) _list->selected();
    popDetail( sym );
}

void PickSymbol::symbolTypeSet( WWindow * choice )
/************************************************/
//    Called when the user made a selection from symbol type combobox
{
    dr_search index = (dr_search) ((WComboBox *)choice)->selected();
    _filter.setSymType( index );
    setTitle( index );
}

void PickSymbol::setTitle( dr_search symType )
/**********************************************/
{
    WString title("Locate ");
    if( symType==DR_SEARCH_ALL ) {
        title.concat( "Symbols" );
    } else {
        title.concat( symTypeNames[symType] );
    }
    setText( title );
}

void PickSymbol::detailSelected( WWindow * )
/******************************************/
{
    Symbol * sym = (Symbol *) _list->selected();
    if( sym ) {
        popDetail( sym );
    }
}

void PickSymbol::okSelected( WWindow * )
/**************************************/
{
    close();
}
