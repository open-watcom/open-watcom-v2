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
#include <wmsgdlg.hpp>
#include <wfiledlg.hpp>

#include "assure.h"
#include "enumstl.h"
#include "wbrwin.h"
#include "brwhelp.h"

EnumStyleEdit::EnumStyleEdit( WWindow * parent,
                              EnumViewStyle def,
                              EnumViewStyle & curr )
            : EnumStyleDlg( this )
            , WDialog( parent, frame().r, frame().t )
            , _default( def )
            , _current( curr )
//---------------------------------------------------
{
}

bool EnumStyleEdit::contextHelp( bool is_active_win )
//---------------------------------------------------
{
    if( is_active_win ) {
        WBRWinBase::helpInfo()->sysHelpId( BRH_ENUMERATOR_STYLES );
    }
    return( TRUE );
}

void EnumStyleEdit::initialize()
//------------------------------
{
    int i;

    setSystemFont( FALSE );
    rescale();
    move( frame().r );
    centre();

    _styleGroup = new WGroupBox( this, _styleGroupR.r, _styleGroupR.t );

    _okButton = new WDefPushButton( this, _okButtonR.r, _okButtonR.t );
    _defaultButton = new WPushButton( this, _defaultButtonR.r, _defaultButtonR.t );
    _cancelButton = new WPushButton( this, _cancelButtonR.r,  _cancelButtonR.t );

    _buttons[ EV_HexLowerCase ] = new WRadioButton( this, _lowerHexR.r, _lowerHexR.t, RStyleGroupFirst );
    _buttons[ EV_HexUpperCase ] = new WRadioButton( this, _upperHexR.r, _upperHexR.t );
    _buttons[ EV_HexMixedCase ] = new WRadioButton( this, _mixedHexR.r, _mixedHexR.t );
    _buttons[ EV_Octal ] =        new WRadioButton( this, _octalR.r,    _octalR.t );
    _buttons[ EV_Decimal ] =      new WRadioButton( this, _decimalR.r,  _decimalR.t );
    _buttons[ EV_Character ] =    new WRadioButton( this, _characterR.r,_characterR.t, RStyleGroupLast );

    _styleGroup->show();
    _okButton->show();
    _defaultButton->show();
    _cancelButton->show();

    for( i = 0; i < EV_NumStyles; i += 1 ) {
        _buttons[ i ]->show();
    }

    _okButton->onClick(      this, (cbw) EnumStyleEdit::okButton );
    _defaultButton->onClick( this, (cbw) EnumStyleEdit::defaultButton );
    _cancelButton->onClick(  this, (cbw) EnumStyleEdit::cancelButton );

    _buttons[ EV_HexLowerCase ]->setFocus();

    setValues( _current );
    show();
}

void EnumStyleEdit::setValues( EnumViewStyle filt )
//-------------------------------------------------
{
    int i;

    _buttons[ 0 ]->setCheck( TRUE );

    for( i = 0; i < EV_NumStyles; i += 1 ) {
        if( filt == i ) {
            _buttons[ i ]->setCheck( TRUE );
            break; // <--- loop exit
        }
    }
}

void EnumStyleEdit::cancelButton( WWindow * )
//-------------------------------------------
{
    quit( 0 );
}

void EnumStyleEdit::okButton( WWindow * )
//---------------------------------------
{
    int i;

    for( i = 0; i < EV_NumStyles; i += 1 ) {
        if( _buttons[ i ]->checked() ) {
            _current = (EnumViewStyle) i;
            break;
        }
    }

    ASSERTION( i < EV_NumStyles );

    quit( 1 );
}

void EnumStyleEdit::defaultButton( WWindow * )
//--------------------------------------------
{
    setValues( _default );
}

// Complain about defining trivial destructor inside class
// definition only for warning levels above 8 
#pragma warning 656 9

EnumStyleEdit::~EnumStyleEdit()
//-----------------------------
{
}
