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
#include <wgroupbx.hpp>
#include <wcombox.hpp>

#include "assure.h"
#include "methfilt.h"
#include "projmodl.h"

static const DefW = 312;
static const DefH = 270;

static const SelectFirst = 0;   // first button changed by [Select All]
static const SelectLast = 2;

struct checkBoxInfo {
    short       x;
    short       y;
    char *      name;
    FilterFlags value;
} checkBoxTable[NUM_FILT_BTTNS] = {
    {  8, 55,  "&Public", FILT_PUBLIC },
    {  8, 75,  "P&rotected", FILT_PROTECTED },
    {  8, 95,  "Pri&vate", FILT_PRIVATE },
    {  8,138,  "&Static", FILT_STATIC },
    {108,138,  "&Non-Static", FILT_NONSTATIC },
    {  8,178,  "&Virtual", FILT_VIRTUAL },
    {108,178,  "Non-Vir&tual", FILT_NONVIRTUAL }
};

static WRect ComboBoxWRect( 4, 4, -4, 85 );
#define NUM_COMBOX_ITEMS 3

struct comboBoxInfo {
    char *      name;
    FilterFlags value;
} comboBoxTable[NUM_COMBOX_ITEMS] = {
    { "Local Members", FILT_NO_INHERITED },
    { "Visible Inherited Members", FILT_INHERITED },
    { "All Inherited Members", FILT_ALL_INHERITED }
};

MethodFilter::MethodFilter( int x, int y, bool hasvirt )
    : WDialog( topWindow, WRect( x, y, DefW, DefH ),
                "Method Filters" )
    , _hasvirt( hasvirt )
//----------------------------------------------------------
{
    _flags = activeProject->getFiltFlags(_hasvirt ? Filt_Methods : Filt_Members);
}

void MethodFilter::initialize()
//-----------------------------
{
    WRect       rect(0,0,100,15);
    WPushButton *bttn;
    WGroupBox * box;
    unsigned    limit = NUM_FILT_BTTNS;
    int         i;

    if( !_hasvirt ) {
        limit -= 2;             // don't include virtuals
    }
    _filterBox = new WComboBox( this, ComboBoxWRect, NULL );
    _filterBox->show();
    for( i = 0; i < NUM_COMBOX_ITEMS; i++ ) {
        _filterBox->insertString( comboBoxTable[i].name );
    }
    for( i = 0; i < NUM_COMBOX_ITEMS; i++ ) {
        if( comboBoxTable[i].value & _flags ) {
            _filterBox->select( i );
            break;
        }
    }

    for( i = 0; i < limit; i++ ) {
        rect.x( checkBoxTable[i].x );
        rect.y( checkBoxTable[i].y );
        _filterBttns[i] = new WCheckBox( this, rect, checkBoxTable[i].name );
        _filterBttns[i]->setCheck( (_flags & checkBoxTable[i].value) != 0 );
        _filterBttns[i]->show();
    }
    box = new WGroupBox( this, WRect(4,30,-4,87), "Access Level" );
    box->show();
    box = new WGroupBox( this, WRect(4,117,-4,42), "Storage Class" );
    box->show();
    if( _hasvirt ) {
        box = new WGroupBox( this, WRect(4,159,-4,42), "Virtuality" );
        box->show();
    } else {
        _filterBttns[NUM_FILT_BTTNS - 2] = NULL;
        _filterBttns[NUM_FILT_BTTNS - 1] = NULL;
    }
    limit = checkBoxTable[limit - 1].y;         // get end Y value;
    limit += 30;                                // for start of buttons
    bttn = new WPushButton( this, WRect( 6, limit, 92, 26 ),"&OK" );
//  bttn->setFont(stdFont);
    bttn->onClick( this, (cbw)&MethodFilter::okButton );
    bttn->show();

    bttn = new WPushButton( this, WRect( 106, limit, 92, 26 ), "&Make Default" );
    bttn->onClick( this, (cbw)&MethodFilter::makeDefault );
    bttn->show();

    bttn = new WPushButton( this, WRect( 206, limit, 92, 26 ), "&Cancel" );
//  bttn->setFont(stdFont);
    bttn->onClick( this, (cbw)&MethodFilter::cancelButton );
    bttn->show();


    bttn = new WPushButton( this, WRect( 211, 70, 82, 26 ), "Set &All" );
//      bttn->setFont(stdFont);
    bttn->onClick( this, (cbw)&MethodFilter::SetAllSelected );
    bttn->show();

    if( !_hasvirt ) {
        size( DefW, limit + 62 );
    }
    show();
}

FilterFlags MethodFilter::readCurrentFlags( void )
//------------------------------------------------
// read the current flags from the controls on the dialog.  Since the controls are deleted during
// the quit processing, it is important to not call this method after quitting.
{
    int limit;
    int i;
    FilterFlags flags;

    if(( _filterBttns[NUM_FILT_BTTNS - 2] != NULL ) &&
       ( _filterBttns[NUM_FILT_BTTNS - 1] != NULL )) {
        limit = NUM_FILT_BTTNS;
        flags = FILT_BLANK;
    } else {
        limit = NUM_FILT_BTTNS - 2;
        flags = FILT_ALL_VIRTUAL;
    }
    flags |= comboBoxTable[ _filterBox->selected() ].value;

    for( i = 0; i < limit; i++ ) {
        if( _filterBttns[i]->checked() ) {
            flags |= checkBoxTable[i].value;
        }
    }

    return( flags );
}

void MethodFilter::makeDefault( WWindow * )
//-----------------------------------------
{
    FilterFlags tmpFlags;

    tmpFlags = getCurrentFlags();

    if(( _filterBttns[NUM_FILT_BTTNS - 2] == NULL ) &&
       ( _filterBttns[NUM_FILT_BTTNS - 1] == NULL )) {
        activeProject->setFiltFlags( Filt_Members, tmpFlags );
    } else {
        activeProject->setFiltFlags( Filt_Methods, tmpFlags );
    }
}

void MethodFilter::okButton( WWindow* )
//-------------------------------------
{
    _flags = readCurrentFlags();
    quit( TRUE );
}

void MethodFilter::cancelButton( WWindow* )
//-----------------------------------------
{
    quit( FALSE );
}

void MethodFilter::SetAllSelected( WWindow* )
//-------------------------------------------
{
    for( int i = SelectFirst; i <= SelectLast; i++ ) {
        _filterBttns[i]->setCheck( TRUE );
    }
}
