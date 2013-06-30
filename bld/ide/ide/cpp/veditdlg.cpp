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


#include "veditdlg.hpp"
#include "wpshbttn.hpp"
#include "wtext.hpp"
#include "weditbox.hpp"
#include "wpickcbx.hpp"
#include "wmsgdlg.hpp"
#include "wmetrics.hpp"

#include "mconfig.hpp"
#include "mproject.hpp"
#include "mcompon.hpp"
#include "mrule.hpp"
#include "mtarget.hpp"
#include "wfiledlg.hpp"
#include "wfilenam.hpp"
#include "wradbttn.hpp"
#include "wgroupbx.hpp"

extern char _viperError[];


VEditDlg::VEditDlg( WWindow* parent, WFileName &fn, WString &parms, bool isdll )
    : WDialog( parent, WRect( 100, 50, 400, 150 ), "Set Text Editor" )
{
    _isDll = isdll;
    _fn = fn;
    _parms = parms;
    _browseDialog = new WFileDialog( this, "executables\0*.exe\0"
                                           "DLL's\0*.dll\0"
                                           "All Files\0*.*\0" );
}

void VEditDlg::initialize()
{

    int                 sx;
    int                 sy;
    WPoint              average;
    WPoint              max;
    WGroupBox           *gbox;
    WText               *t1;
    WText               *t3;
    WText               *t4;
    WText               *t5;
    WText               *t6;
    WText               *t7;

    const int   rb_hite = 10;  //height of a radio button
    const int   space = 10;
    const int   button_width = 50;
    const int   button_hite = 14;

    setSystemFont( FALSE );
    this->textMetrics( average, max );
    sx = average.x() / 4;
    sy = average.y() / 8;

    t1 = new WText( this, WRect( 5*sx, 5*sy, 132*sx, 9*sy ),
                    "Enter editor file name:" );
    t1->show();

    _editorName = new WEditBox( this, WRect( 5*sx, 17*sy, 139*sx, 13*sy ) );
    _editorName->select();
    _editorName->show();

    _parmBoxText = new WText( this, WRect( 5*sx, 33*sy, 132*sx, 9*sy ),
                    "Enter editor parameters:" );
    _parmBoxText->show();

    _editorParms = new WEditBox( this, WRect( 5*sx, 45*sy, 139*sx, 13*sy ) );
    _editorParms->select();
    _editorParms->show();

    WPushButton* bBrowse = new WPushButton( this,
                WRect( 155*sx, 16*sy, button_width * sx, button_hite * sy ),
                "&Browse..." );
    bBrowse->onClick( this, (cbw)&VEditDlg::browseButton );
    bBrowse->show();

    WDefPushButton*     bOk;
    bOk = new WDefPushButton( this,
                WRect( 26*sx, 117*sy, button_width * sx, button_hite * sy ),
                "&OK" );
    bOk->onClick( this, (cbw)&VEditDlg::okButton );
    bOk->show();

    WPushButton* bCancel = new WPushButton( this,
                WRect( 82*sx, 117*sy, button_width * sx, button_hite * sy ),
                "&Cancel" );
    bCancel->onClick( this, (cbw)&VEditDlg::cancelButton );
    bCancel->show();

    WPushButton* bDefault = new WPushButton( this,
                WRect( 138*sx, 117*sy, button_width * sx, button_hite * sy ),
                "&Default" );
    bDefault->onClick( this, (cbw)&VEditDlg::defaultButton );
    bDefault->show();

    _exeButton = new WRadioButton( this,
           WRect( 13*sx, 74*sy, 60*sx, rb_hite * sy ),
           "Executable", RStyleGroupFirst );
    _exeButton->show();
    _exeButton->onClick( this, (cbw)&VEditDlg::exeButton );

    _dllButton = new WRadioButton( this,
           WRect( 13*sx, 87*sy, 60*sx, rb_hite * sy ),
           "DLL", RStyleGroupFirst );
    _dllButton->show();
    _dllButton->onClick( this, (cbw)&VEditDlg::dllButton );

    gbox = new WGroupBox( this,
                          WRect( 5*sx, 61*sy, 139*sx, 42 * sy ),
                          "Editor type: " );
    gbox->show();

    t3 = new WText( this, WRect( 155*sx, 45*sy, 65*sx, 9*sy ),
                    "Parameter" );
    t3->show();
    t4 = new WText( this, WRect( 155*sx, 54*sy, 65*sx, 9*sy ),
                    "Macros:" );
    t4->show();
    t5 = new WText( this, WRect( 155*sx, 67*sy, 65*sx, 9*sy ),
                    "%f - filename" );
    t5->show();
    t6 = new WText( this, WRect( 155*sx, 76*sy, 65*sx, 9*sy ),
                    "%r - row" );
    t6->show();
    t7 = new WText( this, WRect( 155*sx, 85*sy, 65*sx, 9*sy ),
                    "%c - column" );
    t7->show();

    _editorName->setText( _fn );
    _editorParms->setText( _parms );
    if( _isDll ) {
        _dllButton->setCheck( TRUE );
        _editorParms->setText( "" );
        _editorParms->enable( FALSE );
        _parmBoxText->enable( FALSE );
    } else {
        _exeButton->setCheck( TRUE );
    }

    size( (WOrdinal)225*sx, (WOrdinal)155*sy );
    centre();
    show();
    _editorName->setFocus();
}

VEditDlg::~VEditDlg()
{
    delete _browseDialog;
}

void VEditDlg::okButton( WWindow* ) {
    _editorName->getText( _fn );
    _editorParms->getText( _parms );
    if( !_fn.legal() ) {
        WMessageDialog::messagef( this, MsgError, MsgOk, _viperError,
                                  "'%s' is not a valid editor name",
                                  _fn.gets() );
    }
//    _fn.toLower();
    if( _exeButton->checked() ) {
        _isDll = FALSE;
    } else {
        _isDll = TRUE;
    }
    quit( TRUE );
}

void VEditDlg::cancelButton( WWindow* ) {
    quit( FALSE );
}

void VEditDlg::browseButton( WWindow* ) {
    WWindow *old;
    WFileName fn;

    old = WWindow::hasFocus();
    fn = _browseDialog->getOpenFileName( NULL, "Enter target filename",
                                         WFOpenExisting );
    if( fn.size() > 0 ) {
        fn.setExt( "" );
        _editorName->setText( fn );
    }
    old->setFocus();
}

void VEditDlg::setDefault( void ) {
    _editorName->setText( _config->editor() );
    if( _config->editorIsDLL() ) {
        _dllButton->setCheck( TRUE );
    } else {
        _exeButton->setCheck( TRUE );
    }
    _editorParms->setText( _config->editorParms() );
}

void VEditDlg::dllButton( WWindow* ) {
    _editorParms->setText( "" );
    _editorParms->enable( FALSE );
    _parmBoxText->enable( FALSE );
}

void VEditDlg::exeButton( WWindow* ) {
    _editorParms->setText( _parms );
    _editorParms->enable( TRUE );
    _parmBoxText->enable( TRUE );
}

void VEditDlg::defaultButton( WWindow* ) {
    setDefault();
}

bool VEditDlg::process( WFileName &editor, WString &parms, bool &isdll ) {
    int         ret;

    ret = WDialog::process();
    if( ret ) {
        isdll = _isDll;
        editor = _fn;
        parms = _parms;
    }
    return( ret != 0 );
}

