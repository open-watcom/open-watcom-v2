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


#include <wname.hpp>
#include <wlistbox.hpp>
#include <wpshbttn.hpp>
#include <wlistbox.hpp>
#include <wmsgdlg.hpp>

#include "gtrootsl.h"
#include "wbrwin.h"
#include "brwhelp.h"


TreeRootSelect::TreeRootSelect( WWindow * prt, int, int,
                                TreeRootList * visible,
                                TreeRootList * hidden )
                : GTRootSelectDlg( this )
                , TwoList()
                , WDialog( prt, frame().r, frame().t )
                , _visible(visible)
                , _hidden(hidden)
//----------------------------------------------------------
{
}

void TreeRootSelect::initialize()
//-------------------------------
{
    rescale();
    size( frame().r.w(), frame().r.h() );
    centre();

    _toRightButton = new WPushButton( this, _toRightButtonR.r, _toRightButtonR.t );
    _toRightButton->onClick( this,  (cbw) &TreeRootSelect::disablePressed );
    _toRightButton->show();

    _allRightButton = new WPushButton( this, _allRightButtonR.r, _allRightButtonR.t );
    _allRightButton->onClick( this, (cbw) &TreeRootSelect::disableAllPressed );
    _allRightButton->show();

    _toLeftButton = new WPushButton( this, _toLeftButtonR.r, _toLeftButtonR.t );
    _toLeftButton->onClick( this,   (cbw) &TreeRootSelect::enablePressed );
    _toLeftButton->show();

    _allLeftButton = new WPushButton( this, _allLeftButtonR.r, _allLeftButtonR.t );
    _allLeftButton->onClick( this,  (cbw) &TreeRootSelect::enableAllPressed );
    _allLeftButton->show();

    _okButton = new WDefPushButton( this, _okButtonR.r, _okButtonR.t );
    _okButton->onClick( this,       (cbw) &TreeRootSelect::okButton );
    _okButton->show();

    _cancelButton = new WPushButton( this, _cancelButtonR.r, _cancelButtonR.t );
    _cancelButton->onClick( this,   (cbw) &TreeRootSelect::cancelButton );
    _cancelButton->show();

    _helpButton = new WPushButton( this, _helpButtonR.r, _helpButtonR.t );
    _helpButton->onClick( this,     (cbw) &TreeRootSelect::help );
    _helpButton->show();

    _leftBox = new WListBox( this, _leftBoxR.r );
    _leftBox->show();

    _rightBox = new WListBox( this, _rightBoxR.r );
    _rightBox->show();

    _leftText = new WName( this, _leftTextR.r, _leftBox, (char *) _leftTextR.t );
    _leftText->show();

    _rightText = new WName( this, _rightTextR.r, _rightBox, (char *) _rightTextR.t );
    _rightText->show();

    #if 0 // NYI -- responding to double-clicks, changing
    _leftBox->onChanged( this,      (cbw) &TreeRootSelect::listBoxChanged );
    _leftBox->onDblClick( this,     (cbw) &TreeRootSelect::doubleClick );

    _rightBox->onChanged( this,     (cbw) &TreeRootSelect::listBoxChanged );
    _rightBox->onDblClick( this,    (cbw) &TreeRootSelect::doubleClick );
    #endif

    _leftBox->setFocus();

    fillBoxes();
}

void TreeRootSelect::help( WWindow * )
//------------------------------------
{
    WBRWinBase::helpInfo()->sysHelpId( BRH_SELECT_ROOTS );
}

void TreeRootSelect::moveElement( WhichList fromList, int index )
//---------------------------------------------------------------
{
    if( fromList == LeftList ) {
        _hidden->add( _visible->removeAt( index ) );
    } else {
        _visible->add( _hidden->removeAt( index ) );
    }
}

void TreeRootSelect::moveAll( WhichList fromList )
//------------------------------------------------
{
    if( fromList == LeftList ) {
        for( int i = _visible->count(); i > 0; i -= 1 ) {
            _hidden->add( (*_visible)[ i - 1 ] );
        }
        _visible->reset();
    } else {
        for( int i = _hidden->count(); i > 0; i -= 1 ) {
            _visible->add( (*_hidden)[ i - 1 ] );
        }
        _hidden->reset();
    }
}

void TreeRootSelect::fillBoxes( void )
//------------------------------------
{
    int i;

    _leftBox->reset();
    _rightBox->reset();

    for( i = 0; i < _visible->count(); i += 1 ) {
        _leftBox->insertString( (*_visible)[ i ]->node()->name(), i );
    }

    for( i = 0; i < _hidden->count(); i += 1 ) {
        _rightBox->insertString( (*_hidden)[ i ]->node()->name(), i );
    }
}

