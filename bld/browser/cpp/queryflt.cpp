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
#include <weditbox.hpp>
#include <wgroupbx.hpp>
#include <wmsgdlg.hpp>

#include "brwhelp.h"
#include "fileedit.h"
#include "filefilt.h"
#include "keysym.h"
#include "optmgr.h"
#include "queryflt.h"
#include "util.h"
#include "wbrwin.h"


QueryFilter::QueryFilter( WWindow * prt, const char * txt )
        : QueryFilterDlg( this )
        , WDialog( prt, frame().r, txt )
        , _filter( NULL )
//--------------------------------------------------------
{
    _fileEdit = new FileEdit( this );
}

QueryFilter::~QueryFilter()
//-------------------------
{
    delete _fileEdit;
}

int QueryFilter::editFilter( KeySymbol * filter )
//-----------------------------------------------
{
    _filter = filter;
    return process();
}

void QueryFilter::initialize()
//----------------------------
{
    setSystemFont( FALSE );
    rescale();
    move( frame().r );
    centre();

    _searchForGroup =   new WGroupBox(    this, _searchForGroupR.r, _searchForGroupR.t );
    _chbx._searchClasses =      new WCheckBox( this, _searchClassesR.r, _searchClassesR.t );
    _chbx._searchEnums =        new WCheckBox( this, _searchEnumsR.r, _searchEnumsR.t );
    _chbx._searchFunctions =    new WCheckBox( this, _searchFunctionsR.r, _searchFunctionsR.t );
    _chbx._searchTypedefs =     new WCheckBox( this, _searchTypedefsR.r, _searchTypedefsR.t );
    _chbx._searchVariables =    new WCheckBox( this, _searchVariablesR.r, _searchVariablesR.t );
    _searchAll =                new WPushButton( this, _searchAllR.r, _searchAllR.t );

    _searchForGroup->show();
    _chbx._searchClasses->show();
    _chbx._searchEnums->show();
    _chbx._searchFunctions->show();
    _chbx._searchTypedefs->show();
    _chbx._searchVariables->show();
    _searchAll->show();

    _searchAll->onClick( this, (cbw) QueryFilter::searchAll );

    _searchInGroup =    new WGroupBox(    this, _searchInGroupR.r, _searchInGroupR.t );
    _classEdit =        new WEditBox(     this, _classEditR.r, _classEditR.t );
    _classText =        new WText(        this, _classTextR.r, _classTextR.t );
    _functionEdit =     new WEditBox(     this, _functionEditR.r, _functionEditR.t );
    _functionText =     new WText(        this, _functionTextR.r, _functionTextR.t );

    _searchInGroup->show();
    _classEdit->show();
    _classText->show();
    _functionEdit->show();
    _functionText->show();

    _symAttribs =   new WGroupBox( this, _attributesGroupR.r, _attributesGroupR.t );
    _attribArt =    new WCheckBox( this, _attribArtR.r, _attribArtR.t );
    _attribAnon =   new WCheckBox( this, _attribAnonR.r, _attribAnonR.t );
    _attribDecl =   new WCheckBox( this, _attribDeclR.r, _attribDeclR.t );

    _symAttribs->show();
    _attribAnon->show();
    _attribArt->show();
    _attribDecl->show();

    _okButton =     new WDefPushButton( this, _okButtonR.r, _okButtonR.t );
    _cancelButton = new WPushButton( this, _cancelButtonR.r, _cancelButtonR.t );
    _fileButton =   new WPushButton(  this, _fileButtonR.r, _fileButtonR.t );
    _helpButton =   new WPushButton( this, _helpButtonR.r, _helpButtonR.t );

    _okButton->show();
    _cancelButton->show();
    _fileButton->show();
    _helpButton->show();

    _okButton->onClick(     this, (cbw) QueryFilter::okButton );
    _cancelButton->onClick( this, (cbw) QueryFilter::cancelButton );
    _fileButton->onClick(   this, (cbw) QueryFilter::fileButton );
    _helpButton->onClick(   this, (cbw) QueryFilter::helpButton );

    setValues();

    _classEdit->setFocus();
    show();
}

void QueryFilter::cancelButton( WWindow * )
//-----------------------------------------
{
    quit( 0 );
}

// search types corresponding to each checkbox
// must match order of the checkboxes in queryflt.h
static const KeySymType SearchType[] = {
    KSTClasses, KSTEnums, KSTFunctions, KSTTypedefs, KSTVariables
};

void QueryFilter::okButton( WWindow * )
//-------------------------------------
{
    int     i;
    WString str;
    int     searchType = KSTNoSyms;

    _classEdit->getText( str );
    _filter->setContClass( str.gets() );

    _functionEdit->getText( str );
    _filter->setContFunction( str.gets() );

    for( i = 0; i < NUMQFLTCHECKBOXES; i += 1 ) {
        if( _chkbxArray[ i ]->checked() ) {
            searchType |= SearchType[ i ];
        }
    }
    _filter->setSymType( searchType );

    _filter->setAnonymous( _attribAnon->checked() );
    _filter->setArtificial( _attribArt->checked() );
    _filter->setDeclaration( _attribDecl->checked() );

    quit( 1 );
}

void QueryFilter::setValues()
//---------------------------
{
    int i;
    int searchType;

    _classEdit->setText( _filter->contClass() );
    _functionEdit->setText( _filter->contFunction() );

    searchType = _filter->symType();
    for( i = 0; i < NUMQFLTCHECKBOXES; i += 1 ) {
        _chkbxArray[ i ]->setCheck( (searchType & SearchType[ i ]) != 0 );
    }

    _attribAnon->setCheck( _filter->anonymous() );
    _attribArt->setCheck( _filter->artificial() );
    _attribDecl->setCheck( _filter->declaration() );
}

void QueryFilter::fileButton( WWindow * )
//---------------------------------------
{
    _fileEdit->edit( _filter->fileFilter() );
}

void QueryFilter::helpButton( WWindow * )
//---------------------------------------
{
    WBRWinBase::helpInfo()->sysHelpId( BRH_QUERY_FILTER );
}

bool QueryFilter::contextHelp( bool is_active_win )
//-------------------------------------------------
{
    if( is_active_win ) {
        WBRWinBase::helpInfo()->sysHelpId( BRH_QUERY_FILTER );
    }
    return( TRUE );
}

void QueryFilter::searchAll( WWindow * )
//---------------------------------------
{
    int i;

    for( i = 0; i < NUMQFLTCHECKBOXES; i += 1 ) {
        _chkbxArray[ i ]->setCheck( TRUE );
    }
}
