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


#include <dr.h>         // NYI -- should be in module

#include <wgroupbx.hpp>
#include <wpshbttn.hpp>
#include <wradbttn.hpp>
#include <wmsgdlg.hpp>
#include <wfiledlg.hpp>
#include <wcmdlist.hpp>

#include "assure.h"
#include "busyntc.h"
#include "fileedit.h"
#include "filefilt.h"
#include "modlist.h"
#include "wbrwin.h"
#include "brwhelp.h"


FileEdit::FileEdit( WWindow * parent )
            : FileEditDlg( this )
            , WDialog( parent, frame().r, frame().t )
            , _fileFilter( NULL )
            , _searchPatterns( NULL )
//---------------------------------------------------
{
    _tmpFilter = new FileFilter;
}

FileEdit::~FileEdit()
//-------------------
{
    if( _searchPatterns != NULL ) {
        _searchPatterns->deleteContents();
    }
    delete _searchPatterns;
    delete _tmpFilter;
}

int FileEdit::edit( FileFilter * filt )
//-------------------------------------
{
    uint i;

    _fileFilter = filt;
    *_tmpFilter = *_fileFilter;

    if( !_searchPatterns ) {
        _searchPatterns = new WVList;

        for( i = _tmpFilter->numPatterns(); i > 0; i -= 1 ) {
            WString * str;
            str = new WString( _tmpFilter->pattern( i - 1 )->_pattern );
            _searchPatterns->add( str );
        }
    }

    return process();
}

void FileEdit::initialize()
//-------------------------
{
    int i;

    setSystemFont( false );
    rescale();
    move( frame().r );
    centre();

    _fileBox = new ModuleList( this, _fileboxR.r );

    _okButton = new WDefPushButton( this, _okButtonR.r, _okButtonR.t );
    _cancelButton = new WPushButton( this, _cancelButtonR.r,  _cancelButtonR.t );

    _setAllButton = new WPushButton( this, _setallR.r, _setallR.t );
    _clearAllButton = new WPushButton( this, _clearallR.r,  _clearallR.t );

    _helpButton = new WPushButton( this, _helpButtonR.r, _helpButtonR.t );

    _patternGroup = new WGroupBox( this, _patternGroupR.r, _patternGroupR.t );
    _patternText = new WCommandList( this, _patternTextR.r, _patternTextR.t );
    _includeButton = new WPushButton( this, _includeR.r, _includeR.t );
    _excludeButton = new WPushButton( this, _excludeR.r,  _excludeR.t );

    _fileBox->show();
    _patternGroup->show();
    _patternText->show();
    _includeButton->show();
    _excludeButton->show();
    _setAllButton->show();
    _clearAllButton->show();
    _okButton->show();
    _helpButton->show();
    _cancelButton->show();

    _includeButton->onClick(  this, (cbw) FileEdit::includeButton );
    _excludeButton->onClick(  this, (cbw) FileEdit::excludeButton );
    _setAllButton->onClick(   this, (cbw) FileEdit::setAllButton );
    _clearAllButton->onClick( this, (cbw) FileEdit::clearAllButton );
    _okButton->onClick(       this, (cbw) FileEdit::okButton );
    _helpButton->onClick(     this, (cbw) FileEdit::helpButton );
    _cancelButton->onClick(   this, (cbw) FileEdit::cancelButton );

    for( i = 0; i < _tmpFilter->numEntries(); i += 1 ) {
        FFiltEntry * entry = _tmpFilter->entry( i );
        _fileBox->insert( new ModuleItem( new WString( entry->_name ),
                            entry->_enabled ) );
    }
    _fileBox->reset();
    _fileBox->setFocus();

    for( i = 0; i < _searchPatterns->count(); i += 1 ) {
        _patternText->insertString( ((WString *)(*_searchPatterns)[ i ] )->gets(),
                                0 );
    }

    show();
}

void FileEdit::includeButton( WWindow * )
//---------------------------------------
{
    WString * pat = new WString;;

    _patternText->getCommand( *pat );

    if( !_searchPatterns->find( pat ) ) {
        _searchPatterns->insertAt( 0, pat );
    }

    _tmpFilter->include( pat->gets() );
    update();
}

void FileEdit::excludeButton( WWindow * )
//---------------------------------------
{
    WString * pat = new WString;

    _patternText->getCommand( *pat );

    if( !_searchPatterns->find( pat ) ) {
        _searchPatterns->insertAt( 0, pat );
    }

    _tmpFilter->exclude( pat->gets() );
    update();
}

void FileEdit::setAllButton( WWindow * )
//--------------------------------------
{
    _tmpFilter->includeAll();
    update();
}

void FileEdit::clearAllButton( WWindow * )
//----------------------------------------
{
    _tmpFilter->excludeAll();
    update();
}

void FileEdit::update()
//---------------------
{
    int     i;
    bool    needsInv = false;

    for( i = 0; i < _tmpFilter->numEntries(); i += 1 ) {
        FFiltEntry * entry = _tmpFilter->entry( i );

        if( (*_fileBox)[ i ]->_enabled != entry->_enabled ) {
            (*_fileBox)[ i ]->_enabled = entry->_enabled;
            needsInv = true;
        }
    }

    if( needsInv ) {
        _fileBox->invalidate();
    }
}

void FileEdit::cancelButton( WWindow * )
//--------------------------------------
{
    quit( 0 );
}

void FileEdit::okButton( WWindow * )
//----------------------------------
{
    int i;

    for( i = 0; i < _fileBox->count(); i += 1 ) {
        FFiltEntry * entry = _tmpFilter->entry( i );
        ModuleItem * item = (*_fileBox)[ i ];

        if( item->_enabled != entry->_enabled ) {
            if( item->_enabled ) {
                _tmpFilter->include( item->_str->gets() );
            } else {
                _tmpFilter->exclude( item->_str->gets() );
            }
        }
    }

    *_fileFilter = *_tmpFilter;
    quit( 1 );
}

void FileEdit::helpButton( WWindow * )
//------------------------------------
{
    WBRWinBase::helpInfo()->sysHelpId( BRH_FILE_FILTER );
}

bool FileEdit::contextHelp( bool is_active_win )
//----------------------------------------------
{
    if( is_active_win ) {
        WBRWinBase::helpInfo()->sysHelpId( BRH_FILE_FILTER );
    }
    return( true );
}
