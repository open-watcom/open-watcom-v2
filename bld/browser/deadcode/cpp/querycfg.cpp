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


#include <wdialog.hpp>
#include <wcmdlist.hpp>
#include <wpshbttn.hpp>
#include <wcheckbx.hpp>
#include <wtext.hpp>
#include <wcmdlist.hpp>
#include <wmsgdlg.hpp>
#include <wvlist.hpp>

#include "keysym.h"
#include "optmgr.h"
#include "querycfg.h"
#include "queryflt.h"
#include "wbrwin.h"
#include "brwhelp.h"


QueryConfig::QueryConfig( KeySymbol * key, WWindow * prt, const char * txt )
        : QueryConfigDlg( this )
        , WDialog( prt, frame().r, txt )
        , _filter( key )
//----------------------------------------------------------------------
{
    WString filttxt;

    _searchStrings = new WVList;
    _searchStrings->add( new WString( key->name() ) );

    filttxt.printf( "%s Filters", txt );
    _queryFilter = new QueryFilter( this, filttxt );
}

QueryConfig::QueryConfig( WWindow * prt, const char * txt )
        : QueryConfigDlg( this )
        , WDialog( prt, frame().r, txt  )
        , _filter( NULL )
//----------------------------------------------------------------------
{
    WString filttxt;

    _searchStrings = new WVList;

   filttxt.printf( "%s Filters", txt );
    _queryFilter = new QueryFilter( this, txt );
}

QueryConfig::~QueryConfig()
//-------------------------
{
    _searchStrings->deleteContents();
    delete _searchStrings;
    delete _queryFilter;
}

int QueryConfig::editFilter( KeySymbol * filter )
//-----------------------------------------------
{
    _filter = filter;
    return process();
}

void QueryConfig::initialize()
//----------------------------
{
    int i;
    OptionManager * optMgr = WBRWinBase::optManager();

    rescale();
    move( frame().r );
    centre();

    _symText =      new WText( this, _symTextR.r, _symTextR.t );
    _symName =      new WCommandList( this, _symNameR.r, NULL );

    _symText->show();
    _symName->show();

    _matchCase =    new WCheckBox( this, _matchCaseR.r, _matchCaseR.t );
    _wholeWord =    new WCheckBox( this, _anchoredR.r, _anchoredR.t );
    _useRX =        new WCheckBox( this, _useRXR.r, _useRXR.t );
    _cfgRegexp =    new WPushButton( this, _cfgRegexpR.r, _cfgRegexpR.t );

    _matchCase->show();
    _wholeWord->show();
    _useRX->show();

    _matchCase->setCheck( !optMgr->getIgnoreCase() );
    _wholeWord->setCheck( optMgr->getWholeWord() );
    _useRX->setCheck( optMgr->getUseRX() );
    _useRX->onClick( this, (cbw) &QueryConfig::useRXClicked );
    _wholeWord->enable( !optMgr->getUseRX() );

    _ok =           new WDefPushButton( this, _searchR.r, _searchR.t );
    _cancel =       new WPushButton( this, _cancelR.r, _cancelR.t );
    _filters =      new WPushButton( this, _filtersR.r, _filtersR.t );
    _help =         new WPushButton( this, _helpR.r, _helpR.t );

    _ok->show();
    _cancel->show();
    _filters->show();
    _cfgRegexp->show();
    _help->show();

    _cfgRegexp->enable( optMgr->getUseRX() );

    _ok->onClick(           this, (cbw) &QueryConfig::okButton );
    _cancel->onClick(       this, (cbw) &WDialog::cancelButton );
    _filters->onClick(      this, (cbw) &QueryConfig::filtersButton );
    _cfgRegexp->onClick(    this, (cbw) &QueryConfig::cfgRXButton );
    _help->onClick(         this, (cbw) &QueryConfig::helpButton );

    for( i = 0; i < _searchStrings->count(); i +=1 ) {
        _symName->insertString( ((WString *)(*_searchStrings)[ i ] )->gets(),
                                0 );
    }

    _symName->setFocus();

    show();
}

void QueryConfig::okButton( WWindow * )
//-------------------------------------
{
    WString str;
    _symName->getCommand( str );

    if( str.size() > 0 ) {
        WString * addStr = new WString( str );
        if( !_searchStrings->find( addStr ) ) {
            _searchStrings->insertAt( 0, addStr );
        }
    }

    WBRWinBase::optManager()->setIgnoreCase( !_matchCase->checked() );
    WBRWinBase::optManager()->setWholeWord( _wholeWord->checked() );
    WBRWinBase::optManager()->setUseRX(_useRX->checked() );

    _filter->setName( str.gets() );     //  setName _after_ options set!

    // NYI -- file stuff

    quit( TRUE );
}

void QueryConfig::filtersButton( WWindow * )
//------------------------------------------
{
    _queryFilter->editFilter( _filter );
}

void QueryConfig::cfgRXButton( WWindow * )
//----------------------------------------
{
    WBRWinBase::optManager()->editRegExp();
}

void QueryConfig::helpButton( WWindow * )
//---------------------------------------
{
    WBRWinBase::helpInfo()->sysHelpId( BRH_QUERY_CONFIGURATION );
}

void QueryConfig::useRXClicked( WWindow * )
//-----------------------------------------
{
    _wholeWord->enable( !_useRX->checked() );
    _cfgRegexp->enable( _useRX->checked() );
}
