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
#include <wcheckbx.hpp>
#include <wtext.hpp>
#include "wbrdefs.h"
#include "assure.h"
#include "clstxttr.h"
#include "symbol.h"
#include "opicklst.h"
#include "vwcreate.h"
#include "classtyp.h"
#include "strcview.h"
#include "vwclass.h"
#include "projmodl.h"

bool ViewClassType::_dataMembersOn=FALSE;
bool ViewClassType::_methodsOn=FALSE;
bool ViewClassType::_friendsOn=FALSE;

ViewClassType::ViewClassType()
    :_showDataMembers(NULL)
    ,_showMethods(NULL)
    ,_showFriends(NULL)
    ,_dataMembersList(NULL)
    ,_methodsList(NULL)
    ,_friendsList(NULL)
    ,_dataMembersDesc(NULL)
    ,_methodsDesc(NULL)
    ,_friendsDesc(NULL)
{
}

ViewClassType::ViewClassType( int x, int y, WBRWindow *prnt, const Symbol *info,
                              bool derived )
/******************************************************************************/
    :ViewSymbol( x, y, prnt, info, TRUE /*noSetup*/ )
    ,_showDataMembers(NULL)
    ,_showMethods(NULL)
    ,_showFriends(NULL)
    ,_dataMembersList(NULL)
    ,_methodsList(NULL)
    ,_friendsList(NULL)
    ,_dataMembersDesc(NULL)
    ,_methodsDesc(NULL)
    ,_friendsDesc(NULL)
    ,_dataFlags( activeProject->getFiltFlags( Filt_Members ))
    ,_methodFlags( activeProject->getFiltFlags( Filt_Methods ))
{
    if( !derived ) {
        refresh();
        setup();
    } // else derived class will call these
}

ViewClassType::~ViewClassType()
{
    _dataMembers.deleteContents();
    _methods.deleteContents();
    _friends.deleteContents();
}

void ViewClassType::setup()
/*************************/
{
    WPushButton *  bttn;

    WVList & kids = children();
    ulong startIndex = kids.count();

    ViewSymbol::setup();

    bttn = new WPushButton( this, WRect(-70,0,65,viewHeight), "Filters...");
//  bttn->setFont(stdFont);
    bttn->onClick( this, (cbw)&ViewClassType::dataFilterProc );
    bttn->show();

    _showDataMembers=new WCheckBox( this, WRect(viewLeft,0,160,viewHeight),
                                    "Show &Data Members" );
//  _showDataMembers->setFont(stdFont);
    _showDataMembers->onClick( this, (cbw)&ViewClassType::showDataMembers );
    _showDataMembers->setCheck( _dataMembersOn );
    _showDataMembers->show();
    showDataMembers( NULL );

    bttn = new WPushButton( this, WRect(-70,0,65,viewHeight), "Filters..." );
//  bttn->setFont(stdFont);
    bttn->onClick( this, (cbw)&ViewClassType::methodFilterProc );
    bttn->show();

    _showMethods=new WCheckBox( this, WRect(viewLeft,0,160,viewHeight),
                                "Show &Methods" );
//  _showMethods->setFont(stdFont);
    _showMethods->onClick( this, (cbw)&ViewClassType::showMethods );
    _showMethods->setCheck( _methodsOn );
    _showMethods->show();
    showMethods( NULL );

    _showFriends=new WCheckBox( this, WRect(viewLeft,0,160,viewHeight),
                                "Show &Friends" );
//  _showFriends->setFont(stdFont);
    _showFriends->onClick( this, (cbw)&ViewClassType::showFriends );
    _showFriends->setCheck( _friendsOn );
    _showFriends->show();
    showFriends( NULL );

    bttn = new WPushButton( this, WRect(107,0,143,viewHeight),
                            "&Show Decomposition" );
//  bttn->setFont(stdFont);
    bttn->onClick( this, (cbw)&ViewClassType::createStructure );
    bttn->show();

    bttn = new WPushButton( this, WRect(viewLeft,0,97,viewHeight),
                            "Show &Inherit" );
//  bttn->setFont(stdFont);
    bttn->onClick( this, (cbw)&ViewClassType::createInherit );
    bttn->show();

    arrangeKids( startIndex );
}

void ViewClassType::refresh()
/***************************/
{
    ViewSymbol::refresh();
    if( _dataMembersOn ) {
        loadDataMembers();
    }
    if( _methodsOn ) {
        loadMethods();
    }
    if( _friendsOn ) {
        loadFriends();
    }
}

ViewSymbol * ViewClassType::create( int x, int y, WBRWindow * prnt,
                                    const Symbol *info )
/***************************************************************/
{
    return new ViewClassType( x, y, prnt, info );
}

void ViewClassType::dataFilterProc( WWindow * )
/*********************************************/
{
    MethodFilter filt( this, 150, 150, FALSE );

    if( filt.process() ) {
        _dataFlags = filt.getCurrentFlags();
        loadDataMembers();
    }
}

void ViewClassType::methodFilterProc( WWindow * )
/***********************************************/
{
    MethodFilter filt( this, 150, 150, TRUE );

    if( filt.process() ) {
        _methodFlags = filt.getCurrentFlags();
        loadMethods();
    }
}

void ViewClassType::showDataMembers( WWindow * )
/**********************************************/
{
    showList( &_dataMembersList, &_dataMembersDesc, _showDataMembers,
              methodOf(ViewClassType,loadDataMembers), _dataMembersOn,
              pickCallback(&ViewClassType::selectDataMember) );
}

void ViewClassType::showMethods( WWindow * )
/******************************************/
{
    showList( &_methodsList, &_methodsDesc, _showMethods,
              methodOf(ViewClassType,loadMethods),
              _methodsOn, pickCallback(&ViewClassType::selectMethod) );
}

void ViewClassType::showFriends( WWindow * )
/******************************************/
{
    showList( &_friendsList, &_friendsDesc, _showFriends,
              methodOf(ViewClassType,loadFriends),
              _friendsOn, pickCallback(&ViewClassType::selectFriend) );
}

void ViewClassType::createInherit( WWindow * )
/********************************************/
{
    WRect rect;

    getRectangle( rect, TRUE );
    ViewCreator::findOrCreate( CreateInheritView, rect.x(), rect.y() + rect.h(),
                               (WBRWindow *)parent(), _model );
}

void ViewClassType::createStructure( WWindow * )
/**********************************************/
{
    WRect rect;
    getRectangle( rect, TRUE );

    ViewCreator::findOrCreate( CreateStructView,
                               rect.x()+rect.w(), rect.y(),
                               (WBRWindow *)parent(), _model );
}

void ViewClassType::loadDataMembers()
/***********************************/
{
    if( _dataMembersList != NULL ) {
        _dataMembers.deleteContents();
        ((ClassType *)_model)->dataMembers( _dataMembers, _dataFlags );
        _dataMembersList->setObjectList( _dataMembers,
                                         (nameMethod)&Symbol::copyBaseName );
    }
}

void ViewClassType::loadMethods()
/*******************************/
{
    if( _methodsList != NULL ) {
        _methods.deleteContents();
        ((ClassType *)_model)->memberFunctions( _methods, _methodFlags );
        _methodsList->setObjectList( _methods,
                                     (nameMethod)&Symbol::copyBaseName );
    }
}

void ViewClassType::loadFriends()
/*******************************/
{
    if( _friendsList != NULL ) {
        _friends.deleteContents();
        ((ClassType *)_model)->friendFunctions( _friends, FILT_ALL );
        _friendsList->setObjectList( _friends,
                                     (nameMethod)&Symbol::copyBaseName );
    }
}

void ViewClassType::selectDataMember( WObject * obj )
/***************************************************/
{
    Symbol *    sym = (Symbol *) obj;
    WString     buf;
    dr_access   access;

    access = DRGetAccess( sym->getHandle() );
    switch( access ) {
    case DR_ACCESS_PUBLIC:
        buf = "public: ";
        break;
    case DR_ACCESS_PROTECTED:
        buf = "protected: ";
        break;
    default:
        REQUIRE( access == DR_ACCESS_PRIVATE, "vwclass@selectdatamember has sym with unknown access!" );
        buf = "private: ";
    }

    buf.concat( sym->scopedName() );
    _dataMembersDesc->setText( buf );
}

void ViewClassType::selectMethod( WObject * obj )
/***********************************************/
{
    Symbol *    sym = (Symbol *) obj;
    WString     buf;
    dr_access   access;

    access = DRGetAccess( sym->getHandle() );
    switch( access ) {
    case DR_ACCESS_PUBLIC:
        buf = "public: ";
        break;
    case DR_ACCESS_PROTECTED:
        buf = "protected: ";
        break;
    default:
        REQUIRE( access == DR_ACCESS_PRIVATE, "vwclass@selectdatamember has sym with unknown access!" );
        buf = "private: ";
    }

    buf.concat( sym->scopedName() );
    _methodsDesc->setText( buf );
}

void ViewClassType::selectFriend( WObject * obj )
/***********************************************/
{
    Symbol *    sym = (Symbol *) obj;

    _friendsDesc->setText( sym->scopedName() );
}
