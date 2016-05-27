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


#include <wgroupbx.hpp>
#include <wpshbttn.hpp>
#include <wradbttn.hpp>
#include <wcheckbx.hpp>
#include <wmsgdlg.hpp>
#include <wfiledlg.hpp>

#include "memfilt.h"
#include "wbrwin.h"
#include "brwhelp.h"

MemberFilterDlg::MemberFilterDlg( WWindow * parent,
                                  MemberFilter def,
                                  MemberFilter & curr )
            : MemberFiltDlg( this )
            , WDialog( parent, frame().r, frame().t )
            , _default( def )
            , _current( curr )
//---------------------------------------------------
{
}

bool MemberFilterDlg::contextHelp( bool is_active_win )
//-----------------------------------------------------
{
    if( is_active_win ) {
        WBRWinBase::helpInfo()->sysHelpId( BRH_MEMBER_FILTERS );
    }
    return( true );
}

void MemberFilterDlg::initialize()
//--------------------------------
{
    setSystemFont( false );
    rescale();
    move( frame().r );
    centre();

    _inheritGroup = new WGroupBox(      this, _inheritGroupR.r,  _inheritGroupR.t );
    _bttns._none = new WRadioButton(    this, _noneR.r,          _noneR.t, RStyleGroupFirst );
    _bttns._visible = new WRadioButton( this, _visibleR.r,       _visibleR.t );
    _bttns._all = new WRadioButton(     this, _allR.r,           _allR.t, RStyleGroupLast );

    _accessGroup = new WGroupBox(       this, _accessGroupR.r,   _accessGroupR.t );
    _public = new WCheckBox(            this, _publicR.r,        _publicR.t );
    _protected = new WCheckBox(         this, _protectedR.r,     _protectedR.t );
    _private = new WCheckBox(           this, _privateR.r,       _privateR.t );

    _memberGroup = new WGroupBox(       this, _memberGroupR.r,   _memberGroupR.t );
    _variables = new WCheckBox(         this, _variablesR.r,     _variablesR.t );
    _varStatic = new WCheckBox(         this, _varStaticR.r,    _varStaticR.t );
    _functions = new WCheckBox(         this, _functionsR.r,     _functionsR.t );
    _virtual = new WCheckBox(           this, _virtualR.r,       _virtualR.t );
    _funcStatic = new WCheckBox(        this, _funcStaticR.r,    _funcStaticR.t );

    _variables->onClick( this, (cbw) &MemberFilterDlg::varOrFuncPushed );
    _functions->onClick( this, (cbw) &MemberFilterDlg::varOrFuncPushed );

    _okButton = new WDefPushButton( this, _okButtonR.r,      _okButtonR.t );
    _defaultButton = new WPushButton(   this, _defaultButtonR.r, _defaultButtonR.t );
    _cancelButton = new WPushButton(    this, _cancelButtonR.r,  _cancelButtonR.t );


    _inheritGroup->show();
    _bttns._none->show();
    _bttns._visible->show();
    _bttns._all->show();
    _accessGroup->show();
    _public->show();
    _protected->show();
    _private->show();
    _memberGroup->show();
    _variables->show();
    _varStatic->show();
    _functions->show();
    _virtual->show();
    _funcStatic->show();
    _okButton->show();
    _defaultButton->show();
    _cancelButton->show();

    _okButton->onClick(      this, (cbw) MemberFilterDlg::okButton );
    _defaultButton->onClick( this, (cbw) MemberFilterDlg::defaultButton );
    _cancelButton->onClick(  this, (cbw) MemberFilterDlg::cancelButton );

    _bttns._none->setFocus();

    setValues( _current );
    show();
}

void MemberFilterDlg::setValues( const MemberFilter & filt )
//----------------------------------------------------------
{
    _bttnRay[ filt._inheritLevel ]->setCheck( true );

    _public->setCheck( (filt._accessLevel & MemberFilter::AccPublic) != 0 );
    _protected->setCheck( (filt._accessLevel & MemberFilter::AccProtected) != 0 );
    _private->setCheck( (filt._accessLevel & MemberFilter::AccPrivate) != 0 );

    _variables->setCheck( (filt._members & MemberFilter::MemVariables) != 0 );
    _varStatic->setCheck( (filt._members & MemberFilter::MemVarStatic) != 0 );
    _functions->setCheck( (filt._members & MemberFilter::MemFunctions) != 0 );
    _virtual->setCheck( (filt._members & MemberFilter::MemVirtual) != 0 );
    _funcStatic->setCheck( (filt._members & MemberFilter::MemFuncStatic) != 0 );

    varOrFuncPushed( this );
}

void MemberFilterDlg::cancelButton( WWindow * )
//---------------------------------------------
{
    quit( 0 );
}

void MemberFilterDlg::okButton( WWindow * )
//-----------------------------------------
{
    uint_8 i;

    _current._inheritLevel = -1;
    for( i = 0; i < 3; i += 1 ) {
        if( _bttnRay[ i ]->checked() ) {
            _current._inheritLevel = i;
            break;
        }
    }

    _current._accessLevel = 0;
    if( _public->checked() ) {
        _current._accessLevel |= MemberFilter::AccPublic;
    }
    if( _protected->checked() ) {
        _current._accessLevel |= MemberFilter::AccProtected;
    }
    if( _private->checked() ) {
        _current._accessLevel |= MemberFilter::AccPrivate;
    }

    _current._members = 0;
    if( _variables->checked() ) {
        _current._members |= MemberFilter::MemVariables;
    }
    if( _varStatic->checked() ) {
        _current._members |= MemberFilter::MemVarStatic;
    }
    if( _functions->checked() ) {
        _current._members |= MemberFilter::MemFunctions;
    }
    if( _virtual->checked() ) {
        _current._members |= MemberFilter::MemVirtual;
    }
    if( _funcStatic->checked() ) {
        _current._members |= MemberFilter::MemFuncStatic;
    }

    quit( 1 );
}

void MemberFilterDlg::varOrFuncPushed( WWindow * )
//------------------------------------------------
{
    _varStatic->enable( _variables->checked() );
    _virtual->enable( _functions->checked() );
    _funcStatic->enable( _functions->checked() );
}

void MemberFilterDlg::defaultButton( WWindow * )
//----------------------------------------------
{
    setValues( _default );
}

// Complain about defining trivial destructor inside class
// definition only for warning levels above 8 
#pragma warning 656 9

MemberFilterDlg::~MemberFilterDlg()
//---------------------------------
{
}

