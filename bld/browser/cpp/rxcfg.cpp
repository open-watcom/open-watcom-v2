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
#include <wmsgdlg.hpp>

#include "assure.h"
#include "rxcfg.h"
#include "util.h"
#include "wbrwin.h"
#include "brwhelp.h"

static const char * MagicStrs[] = {
    "^", "$","[]","()","~","@", "\\", ".", "|", "?", "+", "*"
};

RegExpCfg::RegExpCfg( WWindow * parent, const RXOptions & def,
                      RXOptions & curr )
            : RegExpCfgDlg( this )
            , WDialog( parent, frame().r, frame().t )
            , _default( def )
            , _current( curr )
//----------------------------------------------------
{
}

RegExpCfg::~RegExpCfg()
//---------------------
{
}

void RegExpCfg::initialize()
//--------------------------
{
    rescale();
    move( frame().r );
    centre();

    _anchorGroup =  new WGroupBox( this, _meanTextR.r, _meanTextR.t );
    _startsWith =   new WRadioButton( this, _startsWithR.r, _startsWithR.t, RStyleGroupFirst );
    _contains   =   new WRadioButton( this, _containsR.r, _containsR.t, RStyleGroupLast );

    _anchorGroup->show();
    _startsWith->show();
    _contains->show();

    _characterGroup =   new WGroupBox(  this, _characterGroupR.r,_characterGroupR.t );
    _characterGroup->show();

    _chbx._anchorChar =     new WCheckBox(      this, _anchorCharR.r,   _anchorCharR.t );
    _chbx._eolChar =        new WCheckBox(      this, _eolCharR.r,      _eolCharR.t );
    _chbx._charGroupChar =  new WCheckBox(      this, _charGroupCharR.r,_charGroupCharR.t );
    _chbx._exprGroupChar =  new WCheckBox(      this, _exprGroupCharR.r,_exprGroupCharR.t );
    _chbx._ignoreCaseChar = new WCheckBox(      this, _ignoreCaseCharR.r,_ignoreCaseCharR.t );
    _chbx._respectCaseChar =new WCheckBox(      this, _respectCaseCharR.r,_respectCaseCharR.t );
    _chbx._escapeChar =     new WCheckBox(      this, _escapeCharR.r,   _escapeCharR.t );
    _chbx._dotChar =        new WCheckBox(      this, _dotCharR.r,      _dotCharR.t );
    _chbx._alternateChar =  new WCheckBox(      this, _alternateCharR.r,_alternateCharR.t );
    _chbx._questionChar =   new WCheckBox(      this, _questionCharR.r, _questionCharR.t );
    _chbx._plusChar =       new WCheckBox(      this, _plusCharR.r,     _plusCharR.t );
    _chbx._starChar =       new WCheckBox(      this, _starCharR.r,     _starCharR.t );

    _chbx._anchorChar->show();
    _chbx._eolChar->show();
    _chbx._charGroupChar->show();
    _chbx._exprGroupChar->show();
    _chbx._ignoreCaseChar->show();
    _chbx._respectCaseChar->show();
    _chbx._escapeChar->show();
    _chbx._dotChar->show();
    _chbx._alternateChar->show();
    _chbx._questionChar->show();
    _chbx._plusChar->show();
    _chbx._starChar->show();

    _anchorText =       new WText(      this, _anchorTextR.r,   _anchorTextR.t );
    _eolText =          new WText(      this, _eolTextR.r,      _eolTextR.t );
    _charGroupText =    new WText(      this, _charGroupTextR.r, _charGroupTextR.t );
    _exprGroupText =    new WText(      this, _exprGroupTextR.r,_exprGroupTextR.t );
    _ignoreCaseText =   new WText(      this, _ignoreCaseTextR.r,_ignoreCaseTextR.t );
    _respectCaseText =  new WText(      this, _respectCaseTextR.r,_respectCaseTextR.t );
    _escapeText =       new WText(      this, _escapeTextR.r,   _escapeTextR.t );
    _dotText =          new WText(      this, _dotTextR.r,      _dotTextR.t );
    _alternateText =    new WText(      this, _alternateTextR.r,_alternateTextR.t );
    _questionText =     new WText(      this, _questionTextR.r, _questionTextR.t );
    _plusText =         new WText(      this, _plusTextR.r,     _plusTextR.t );
    _starText =         new WText(      this, _starTextR.r,     _starTextR.t );

    _anchorText->show();
    _eolText->show();
    _charGroupText->show();
    _exprGroupText->show();
    _ignoreCaseText->show();
    _respectCaseText->show();
    _escapeText->show();
    _dotText->show();
    _alternateText->show();
    _questionText->show();
    _plusText->show();
    _starText->show();

    _okButton =         new WDefPushButton( this, _okButtonR.r,     _okButtonR.t );
    _defaultsButton =   new WPushButton( this, _defaultsButtonR.r,  _defaultsButtonR.t );
    _cancelButton =     new WPushButton( this, _cancelButtonR.r,    _cancelButtonR.t );
    _enableAllButton =  new WPushButton( this, _enableAllButtonR.r, _enableAllButtonR.t );
    _clearAllButton =   new WPushButton( this, _clearAllButtonR.r,  _clearAllButtonR.t );
    _helpButton =       new WPushButton( this, _helpButtonR.r,      _helpButtonR.t );

    _okButton->onClick(         this, (cbw) &RegExpCfg::okButton );
    _defaultsButton->onClick(   this, (cbw) &RegExpCfg::defaultsButton );
    _cancelButton->onClick(     this, (cbw) &RegExpCfg::cancelButton );
    _enableAllButton->onClick(  this, (cbw) &RegExpCfg::enableAllButton );
    _clearAllButton->onClick(   this, (cbw) &RegExpCfg::clearAllButton );
    _helpButton->onClick(       this, (cbw) &RegExpCfg::helpButton );

    _okButton->show();
    _defaultsButton->show();
    _cancelButton->show();
    _enableAllButton->show();
    _clearAllButton->show();
    _helpButton->show();

    _startsWith->setFocus();

    setValues( _current );

    show();
}

void RegExpCfg::cancelButton( WWindow * )
//---------------------------------------
{
    quit( 0 );
}
void RegExpCfg::okButton( WWindow * )
//-----------------------------------
// this builds a string of all RX characters
// which are _not_ to be considered special
// (ie, not checked on the dialog)
{
    int i;

    _current._anchored = _startsWith->checked();

    _current._magicString[ 0 ] = '\0';
    for( i = 0; i < NUMCHECKBOXES; i += 1 ) {
        if( !_chbxArray[ i ]->checked() ) {
            strcat( _current._magicString, MagicStrs[ i ] );
        }
    }

    quit( 1 );
}

void RegExpCfg::defaultsButton( WWindow * )
//-----------------------------------------
{
    setValues( _default );
}

void RegExpCfg::enableAllButton( WWindow * )
//------------------------------------------
{
    int i;

    for( i = 0; i < NUMCHECKBOXES; i += 1 ) {
        _chbxArray[ i ]->setCheck( TRUE );
    }
}

void RegExpCfg::clearAllButton( WWindow * )
//-----------------------------------------
{
    int i;

    for( i = 0; i < NUMCHECKBOXES; i += 1 ) {
        _chbxArray[ i ]->setCheck( FALSE );
    }
}

void RegExpCfg::helpButton( WWindow * )
//-------------------------------------
{
    WBRWinBase::helpInfo()->sysHelpId( BRH_REGULAR_EXPRESSIONS );
}

bool RegExpCfg::contextHelp( bool is_active_win )
//-----------------------------------------------
{
    if( is_active_win ) {
        WBRWinBase::helpInfo()->sysHelpId( BRH_REGULAR_EXPRESSIONS );
    }
    return( TRUE );
}

void RegExpCfg::setValues( const RXOptions & opt )
//------------------------------------------------
{
    int i;

    _startsWith->setCheck( opt._anchored );
    _contains->setCheck( !opt._anchored );

    for( i = 0; i < NUMCHECKBOXES; i += 1 ) {
        _chbxArray[ i ]->setCheck( TRUE );
    }

    for( i = strlen( opt._magicString ); i > 0; i -= 1 ) {
        switch( opt._magicString[ i - 1 ] ) {
        case '^':
            _chbx._anchorChar->setCheck( FALSE );
            break;
        case '$':
            _chbx._eolChar->setCheck( FALSE );
            break;
        case '[':
        case ']':
            _chbx._charGroupChar->setCheck( FALSE );
            break;
        case '(':
        case ')':
            _chbx._exprGroupChar->setCheck( FALSE );
            break;
        case '~':
            _chbx._ignoreCaseChar->setCheck( FALSE );
            break;
        case '@':
            _chbx._respectCaseChar->setCheck( FALSE );
            break;
        case '\\':
            _chbx._escapeChar->setCheck( FALSE );
            break;
        case '.':
            _chbx._dotChar->setCheck( FALSE );
            break;
        case '|':
            _chbx._alternateChar->setCheck( FALSE );
            break;
        case '?':
            _chbx._questionChar->setCheck( FALSE );
            break;
        case '+':
            _chbx._plusChar->setCheck( FALSE );
            break;
        case '*':
            _chbx._starChar->setCheck( FALSE );
            break;
        default:
            NODEFAULT;
        }
    }
}
