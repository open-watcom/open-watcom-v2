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


#include <wtext.hpp>
#include <wpshbttn.hpp>
#include <wradbttn.hpp>
#include <weditbox.hpp>
#include <wmsgdlg.hpp>
#include <wfiledlg.hpp>
#include <wgroupbx.hpp>

#include "eddlldg.h"
#include "brwhelp.h"
#include "wbrwin.h"

static char * DLLFilter = "Editor DLL (*.dll)\0" "*.dll\0"
                          "All Files(*.*)\0" "*.*\0";
static char * EXEFilter = "Editor Executable (*.exe)\0" "*.exe\0"
                          "All Files(*.*)\0" "*.*\0";

NewEditDLL::NewEditDLL( WWindow * parent,
                        const char * curName,
                        bool curIsDLL,
                        const char * curParms,
                        const char * def,
                        bool defIsDLL,
                        const char * defParms )
            : EditDLLDlg( this )
            , WDialog( parent, frame().r, frame().t )
            , _newDLLName( curName )
            , _parms( curParms )
            , _isDLL( curIsDLL )
            , _default( def )
            , _defaultIsDLL( defIsDLL )
            , _defaultParms( defParms )
//----------------------------------------------------
{
}

NewEditDLL::~NewEditDLL()
//-----------------------
{
}

void NewEditDLL::initialize()
//--------------------------
{
    rescale();
    move( frame().r );
    centre();

    _dllText =      new WText(          this, _dllNameTextR.r,  _dllNameTextR.t );
    _dllEdit =      new WEditBox(       this, _dllNameEditR.r,  _dllNameEditR.t );
    _parmBoxText =  new WText(          this, _parmBoxTextR.r,  _parmBoxTextR.t );
    _editorParms =  new WEditBox(       this, _editorParmsR.r,  _editorParmsR.t );
    _okButton =     new WDefPushButton( this, _okButtonR.r,     _okButtonR.t );
    _defaultButton =new WPushButton(    this, _defaultButtonR.r,_defaultButtonR.t );
    _filesButton =  new WPushButton(    this, _fileButtonR.r,   _fileButtonR.t );
    _cancelButton = new WPushButton(    this, _cancelButtonR.r, _cancelButtonR.t );
    _helpButton =   new WPushButton(    this, _helpButtonR.r,   _helpButtonR.t );
    _exeButton =    new WRadioButton(   this, _exeButtonR.r,    _exeButtonR.t, RStyleGroupFirst );
    _dllButton =    new WRadioButton(   this, _dllButtonR.r,    _dllButtonR.t, RStyleGroupFirst );
    _gbox =         new WGroupBox(      this, _gboxR.r,         _gboxR.t );

    _parmMsg =  new WText(              this, _parmMsgR.r,      _parmMsgR.t );
    _macroMsg = new WText(              this, _macroMsgR.r,     _macroMsgR.t );
    _fMsg =     new WText(              this, _fMsgR.r,         _fMsgR.t );
    _rMsg =     new WText(              this, _rMsgR.r,         _rMsgR.t );
    _cMsg =     new WText(              this, _cMsgR.r,         _cMsgR.t );

    _dllText->show();
    _dllEdit->show();
    _editorParms->show();
    _parmBoxText->show();
    _okButton->show();
    _defaultButton->show();
    _filesButton->show();
    _cancelButton->show();
    _helpButton->show();
    _exeButton->show();
    _dllButton->show();
    _parmMsg->show();
    _macroMsg->show();
    _fMsg->show();
    _rMsg->show();
    _cMsg->show();
    _gbox->show();

    _okButton->onClick(          this, (cbw) NewEditDLL::okButton );
    _defaultButton->onClick(     this, (cbw) NewEditDLL::defaultButton );
    _filesButton->onClick(       this, (cbw) NewEditDLL::filesButton );
    _cancelButton->onClick(      this, (cbw) NewEditDLL::cancelButton );
    _helpButton->onClick(        this, (cbw) NewEditDLL::helpButton );
    _exeButton->onClick(         this, (cbw) NewEditDLL::exeButton );
    _dllButton->onClick(         this, (cbw) NewEditDLL::dllButton );

    _dllEdit->setText( _newDLLName );
    _dllEdit->setFocus();

    if( _isDLL ) {
        _dllButton->setCheck( TRUE );
        _editorParms->setText( "" );
        _editorParms->enable( FALSE );
        _parmBoxText->enable( FALSE );
    } else {
        _exeButton->setCheck( TRUE );
        _editorParms->setText( _parms );
        _editorParms->enable( TRUE );
        _parmBoxText->enable( TRUE );
    }

    show();
}

void NewEditDLL::cancelButton( WWindow * )
//---------------------------------------
{
    quit( 0 );
}

void NewEditDLL::okButton( WWindow * )
//-----------------------------------
{
    _dllEdit->getText( _newDLLName );
    _editorParms->getText( _parms );
    if( _exeButton->checked() ) {
        _isDLL = FALSE;
    } else {
        _isDLL = TRUE;
    }

    quit( 1 );
}

void NewEditDLL::filesButton( WWindow * )
//---------------------------------------
{
    const char * result;
    if( _exeButton->checked() ) {
        WFileDialog fileSelect( this, EXEFilter );
        result = fileSelect.getOpenFileName( NULL,
            "New Browser Editor EXE", WFOpenNew );
        if( result && *result ) {
            _dllEdit->setText( result );
        }
    } else {
        WFileDialog fileSelect( this, DLLFilter );
        result = fileSelect.getOpenFileName( NULL,
            "New Browser Editor DLL", WFOpenNew );
        if( result && *result ) {
            _dllEdit->setText( result );
        }
    }
}

void NewEditDLL::defaultButton( WWindow * )
//-----------------------------------------
{
    _dllEdit->setText( _default );
    if( _defaultIsDLL ) {
        _dllButton->setCheck( TRUE );
        _editorParms->setText( "" );
        _editorParms->enable( FALSE );
        _parmBoxText->enable( FALSE );
    } else {
        _exeButton->setCheck( TRUE );
        _editorParms->setText( _defaultParms );
        _editorParms->enable( TRUE );
        _parmBoxText->enable( TRUE );
    }
}


void NewEditDLL::helpButton( WWindow * )
//--------------------------------------
{
    WBRWinBase::helpInfo()->sysHelpId( BRH_FILE_FILTER );
}

void NewEditDLL::dllButton( WWindow * )
//--------------------------------------
{
    _editorParms->setText( "" );
    _editorParms->enable( FALSE );
    _parmBoxText->enable( FALSE );
}

void NewEditDLL::exeButton( WWindow * )
//--------------------------------------
{
    _editorParms->setText( _parms );
    _editorParms->enable( TRUE );
    _parmBoxText->enable( TRUE );
}



bool NewEditDLL::contextHelp( bool is_active_win )
//------------------------------------------------
{
    if( is_active_win ) {
        WBRWinBase::helpInfo()->sysHelpId( BRH_FILE_FILTER );
    }
    return( TRUE );
}
