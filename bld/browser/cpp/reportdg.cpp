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
#include <wcheckbx.hpp>
#include <wradbttn.hpp>
#include <wtext.hpp>
#include <weditbox.hpp>
#include <wmsgdlg.hpp>

#include "reportdg.h"
#include "util.h"
#include "wbrwin.h"
#include "brwhelp.h"


ReportDialog::ReportDialog( WWindow * parent )
            : ReportDlg( this )
            , WDialog( parent, frame().r, frame().t )
//---------------------------------------------------
{
}

ReportDialog::~ReportDialog()
//---------------------------
{
}

void ReportDialog::initialize()
//-----------------------------
{
    rescale();
    move( frame().r );
    centre();

    _destinationGroup = new WGroupBox(      this, _destinationGroupR.r, _destinationGroupR.t );
    _destinationEdit =  new WEditBox(       this, _destinationEditR.r,  _destinationEditR.t );
    _filesButton =      new WPushButton(    this, _filesButtonR.r,      _filesButtonR.t );

    _destinationGroup->show();
    _destinationEdit->show();
    _filesButton->show();

    _contentsGroup =    new WGroupBox(      this, _contentsGroupR.r,    _contentsGroupR.t );
    _classes =          new WCheckBox(      this, _classesR.r,          _classesR.t );
    _variables =        new WCheckBox(      this, _variablesR.r,        _variablesR.t );
    _typedefs =         new WCheckBox(      this, _typedefsR.r,         _typedefsR.t );
    _functions =        new WCheckBox(      this, _functionsR.r,        _functionsR.t );
    _enums =            new WCheckBox(      this, _enumsR.r,            _enumsR.t );

    _contentsGroup->show();
    _classes->show();
    _variables->show();
    _typedefs->show();
    _functions->show();
    _enums->show();

    _saveButton =       new WDefPushButton( this, _saveButtonR.r,       _saveButtonR.t );
    _cancelButton =     new WPushButton(    this, _cancelButtonR.r,     _cancelButtonR.t );
    _layoutButton =     new WPushButton(    this, _layoutButtonR.r,     _layoutButtonR.t );
    _helpButton =       new WPushButton(    this, _helpButtonR.r,       _helpButtonR.t );

    _saveButton->show();
    _cancelButton->show();
    _layoutButton->show();
    _helpButton->show();

    _saveButton->onClick(   this, (cbw) ReportDialog::saveButton );
    _cancelButton->onClick( this, (cbw) ReportDialog::cancelButton );
    _layoutButton->onClick( this, (cbw) ReportDialog::layoutButton );
    _helpButton->onClick(   this, (cbw) ReportDialog::helpButton );

    _classes->setFocus();

    show();
}

void ReportDialog::cancelButton( WWindow * )
//------------------------------------------
{
    quit( 0 );
}
void ReportDialog::saveButton( WWindow * )
//--------------------------------------
{
    quit( 1 );

    // NYI -- copy results
}

void ReportDialog::layoutButton( WWindow * )
//------------------------------------------
{
    // NYI
}

void ReportDialog::filesButton( WWindow * )
//-----------------------------------------
{
    // NYI
}

void ReportDialog::helpButton( WWindow * )
//----------------------------------------
{
    WBRWinBase::helpInfo()->sysHelpId( BRH_REPORT_OPTIONS );
}

bool ReportDialog::contextHelp( bool is_active_win )
//--------------------------------------------------
{
    if( is_active_win ) {
        WBRWinBase::helpInfo()->sysHelpId( BRH_REPORT_OPTIONS );
    }
    return( TRUE );
}
