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
#include <wtext.hpp>
#include <wradbttn.hpp>
#include <wmsgdlg.hpp>

#include "util.h"

#include "gtlnopt.h"

GTLineOption::GTLineOption( WWindow * parent )
            : GTLineOptDlg( this )
            , WDialog( parent, frame().r, frame().t )
//---------------------------------------------------
{
}

GTLineOption::~GTLineOption()
//---------------------------
{
}

void GTLineOption::initialize()
//-----------------------------
{
    rescale();
    move( frame().r );

    _exampleText =      new WText(          this, _exampleTextR.r,      _exampleTextR.t );
    _exampleText->show();

    _styleGroup =       new WGroupBox(      this, _styleGroupR.r,       _styleGroupR.t );
    _styleGroup->show();

    _normalButton =     new WRadioButton(   this, _normalButtonR.r,     _normalButtonR.t, RStyleGroupFirst );
    _dashButton =       new WRadioButton(   this, _dashButtonR.r,       _dashButtonR.t );
    _dotButton =        new WRadioButton(   this, _dotButtonR.r,        _dotButtonR.t );
    _dashdotButton =    new WRadioButton(   this, _dashdotButtonR.r,    _dashdotButtonR.t );
    _dashdotdotButton = new WRadioButton(   this, _dashdotdotButtonR.r, _dashdotdotButtonR.t );
    _thick1Button =     new WRadioButton(   this, _thick1ButtonR.r,     _thick1ButtonR.t );
    _thick2Button =     new WRadioButton(   this, _thick2ButtonR.r,     _thick2ButtonR.t );
    _thick3Button =     new WRadioButton(   this, _thick3ButtonR.r,     _thick3ButtonR.t );
    _thick4Button =     new WRadioButton(   this, _thick4ButtonR.r,     _thick4ButtonR.t );
    _thick5Button =     new WRadioButton(   this, _thick5ButtonR.r,     _thick5ButtonR.t, RStyleGroupLast );

    _normalButton->show();
    _dashdotdotButton->show();
    _dashdotButton->show();
    _dashButton->show();
    _dotButton->show();
    _thick1Button->show();
    _thick2Button->show();
    _thick3Button->show();
    _thick4Button->show();
    _thick5Button->show();

    _okButton =         new WDefPushButton( this, _okButtonR.r,         _okButtonR.t );
    _cancelButton =     new WPushButton(    this, _cancelButtonR.r,     _cancelButtonR.t );
    _colourButton =     new WPushButton(    this, _colourButtonR.r,     _colourButtonR.t );
    _helpButton =       new WPushButton(    this, _helpButtonR.r,       _helpButtonR.t );

    _okButton->show();
    _cancelButton->show();
    _colourButton->show();
    _helpButton->show();

    _okButton->onClick(     this, (cbw) GTLineOption::okButton );
    _cancelButton->onClick( this, (cbw) GTLineOption::cancelButton );
    _colourButton->onClick( this, (cbw) GTLineOption::colourButton );
    _helpButton->onClick(   this, (cbw) GTLineOption::helpButton );


    _okButton->setFocus();

    show();
}

void GTLineOption::okButton( WWindow * )
//--------------------------------------
{
    //NYI

    quit( 1 );
}

void GTLineOption::cancelButton( WWindow * )
//------------------------------------------
{
    //NYI

    quit( 0 );
}

void GTLineOption::colourButton( WWindow * )
//------------------------------------------
{
    //NYI

    notYetImplemented();
}

void GTLineOption::helpButton( WWindow * )
//----------------------------------------
{
    //NYI

    notYetImplemented();
}
