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
#include <weditbox.hpp>
#include <wmsgdlg.hpp>
#include <wfiledlg.hpp>

#include "fileinfo.h"

#include "newdbrdg.h"
#include "wbrwin.h"
#include "brwhelp.h"

NewDBRFile::NewDBRFile( WWindow * parent, const char * filter )
            : NewDBRDlg( this )
            , WDialog( parent, frame().r, frame().t )
            , _fileName( "" )
            , _filter( filter )
//----------------------------------------------------
{
}

bool NewDBRFile::contextHelp( bool is_active_win )
//------------------------------------------------
{
    if( is_active_win ) {
        WBRWinBase::helpInfo()->sysHelpId( BRH_NEW_DATABASE_FILE );
    }
    return( TRUE );
}

void NewDBRFile::initialize()
//--------------------------
{
    setSystemFont( FALSE );
    rescale();
    move( frame().r );
    centre();

    _browsefileGroup =   new WGroupBox(      this, _browsefileGroupR.r,   _browsefileGroupR.t );
    _fileEdit =          new WEditBox(       this, _fileEditR.r,          _fileEditR.t );
    _okButton =          new WDefPushButton( this, _okButtonR.r,          _okButtonR.t );
    _browseFilesButton = new WPushButton(    this, _browseFilesButtonR.r, _browseFilesButtonR.t );
    _cancelButton =      new WPushButton(    this, _cancelButtonR.r,      _cancelButtonR.t );

    _browsefileGroup->show();
    _fileEdit->show();
    _okButton->show();
    _browseFilesButton->show();
    _cancelButton->show();

    _okButton->onClick(          this, (cbw) NewDBRFile::okButton );
    _browseFilesButton->onClick( this, (cbw) NewDBRFile::filesButton );
    _cancelButton->onClick(      this, (cbw) NewDBRFile::cancelButton );

    _fileEdit->setFocus();

    show();
}

void NewDBRFile::cancelButton( WWindow * )
//---------------------------------------
{
    _fileName = "";

    quit( 0 );
}

void NewDBRFile::okButton( WWindow * )
//-----------------------------------
{
    MsgRetType ret;

    _fileEdit->getText( _fileName );

    FileInfo inf( _fileName.gets() );
    if( inf.exists() ) {
        ret = WMessageDialog::messagef( this, MsgWarning, MsgOkCancel, "New Browser Database File",
                                        "%s exists.  Overwrite?", _fileName.gets() );
        if( ret == MsgRetOk ) {
            quit( 1 );
        } else {
            /* don't accept, stay in dialog */
        }
    } else {
        quit( 1 );
    }
}

void NewDBRFile::filesButton( WWindow * )
//---------------------------------------
{
    WFileDialog fileSelect( this, _filter );
    const char * result;

    result = fileSelect.getOpenFileName( NULL, "New Browser Database File", WFOpenNew );

    if( result && *result ) {
        _fileEdit->setText( result );
    }
}

// Complain about defining trivial destructor inside class
// definition only for warning levels above 8 
#pragma warning 656 9

NewDBRFile::~NewDBRFile()
//-----------------------
{
}
