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


#include <wcvector.h>

#include <wpshbttn.hpp>

#include "hotlist.h"
#include "edmodlst.h"

EditModuleList::EditModuleList( WWindow * parent, const char * text,
                                bool addRemove )
        : EditModuleListDlg( this )
        , WDialog( parent, frame().r, text )
        , _addRemove( addRemove )
//------------------------------------------------------------------
{
    if( _addRemove ) {
        _removedModuleItems = new WCPtrOrderedVector<ModuleItem>;
    }
}

EditModuleList::~EditModuleList()
//-------------------------------
{
    if( _addRemove ) {
        _removedModuleItems->clearAndDestroy();
        delete _removedModuleItems;
    }
}

void EditModuleList::initialize()
//-------------------------------
{
    rescale();
    move( frame().r );
    centre();

    _moduleBox =        new ModuleList( this, _moduleBoxR.r );
    _moduleBox->show();

    _okButton =         new WDefPushButton( this, _okButtonR.r, _okButtonR.t );
    _cancelButton =     new WPushButton( this, _cancelButtonR.r, _cancelButtonR.t );
    _enableButton =     new WPushButton( this, _enableButtonR.r, _enableButtonR.t );
    _disableButton =    new WPushButton( this, _disableButtonR.r, _disableButtonR.t );
    _helpButton =       new WPushButton( this, _helpButtonR.r, _helpButtonR.t );

    _okButton->show();
    _cancelButton->show();
    _enableButton->show();
    _disableButton->show();
    _helpButton->show();

    _okButton->onClick(         this, (cbw) okButton );
    _cancelButton->onClick(     this, (cbw) cancelButton );
    _enableButton->onClick(     this, (cbw) enableButton );
    _disableButton->onClick(    this, (cbw) disableButton );
    _helpButton->onClick(       this, (cbw) helpButton );

    if( _addRemove ) {
        _addButton =    new WPushButton( this, _addButtonR.r, _addButtonR.t );
        _removeButton = new WPushButton( this, _removeButtonR.r, _removeButtonR.t );

        _addButton->show();
        _removeButton->show();

        _addButton->onClick(    this, (cbw) addButton );
        _removeButton->onClick( this, (cbw) removeButton );
    }

    loadBox();
    _moduleBox->reset();
    _moduleBox->setFocus();

    show();
}

void EditModuleList::removeButton( WWindow * )
//--------------------------------------------
{
    ModuleItem * item;

    item = _moduleBox->remove( _moduleBox->selected() );
    _removedModuleItems->append( item );
}

static void ChangeAllEnabled( ModuleList & lst, bool nEnable )
//------------------------------------------------------------
{
    int i;

    for( i = 0; i < lst.count(); i += 1 ) {
        lst[ i ]->_enabled = nEnable;
    }
    lst.reset();
}

void EditModuleList::disableButton( WWindow * )
//---------------------------------------------
{
    ChangeAllEnabled( *_moduleBox, false );
}

void EditModuleList::enableButton( WWindow * )
//--------------------------------------------
{
    ChangeAllEnabled( *_moduleBox, true );
}
