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
#include <wradbttn.hpp>

#include "wbrdefs.h"
#include "assure.h"
#include "searchop.h"
#include "regexp.h"
#include "mem.h"
#include "util.h"

extern "C" char near META[];    // this from regexp.c

enum {
    bttnLeft = 6,
    radHeight = 18,
    radWidth = 120,
    groupLeft = 3,
    pushTop = 168,
    pushHeight = 24,
    pushWidth = 96,
};

char *  SearchOptions::userMagicString = NULL;
bool    SearchOptions::noMagic = FALSE;
bool    SearchOptions::anchorRegExp = TRUE;

SearchOptions::SearchOptions( int x, int y )
    : WDialog( topWindow, WRect(x,y,308,230), "Search Options" )
//----------------------------------------------
{
}

void SearchOptions::initialize( void )
//------------------------------------
{
    WPushButton *       bttn;
    WGroupBox *         box;

    _allowAll = new WRadioButton( this, WRect(bttnLeft,28,radWidth,radHeight),
                                  "&Allow All", RStyleGroupFirst );
    _allowAll->show();
    _disallowAll = new WRadioButton( this,WRect(bttnLeft,50,radWidth,radHeight),
                                     "&Disallow All" );
    _disallowAll->show();
    _disallowSome = new WRadioButton(this,WRect(bttnLeft,72,radWidth,radHeight),
                                     "Disallow &Some", RStyleGroupLast );
    _disallowSome->show();
    _magicBox = new WEditBox( this, WRect(132,66,160,28), MagicString );
    _magicBox->show();
    box = new WGroupBox( this, WRect(groupLeft,3,294,96),
                         "Regular Expression Characters" );
    box->show();
    _startsWith = new WRadioButton( this, WRect(bttnLeft,120,radWidth,radHeight),
                                    "Starts &With", RStyleGroupFirst );
    _startsWith->show();
    _contains = new WRadioButton( this, WRect(164,120,radWidth,radHeight),
                                  "&Contains", RStyleGroupLast );
    _contains->show();
    box = new WGroupBox( this, WRect(groupLeft,100,294,40),
                         "Search String Meaning" );
    box->show();
    _case = new WCheckBox( this, WRect(bttnLeft,146,radWidth,radHeight),
                           "&Ignore Case" );
    _case->show();
    bttn = new WPushButton( this, WRect(groupLeft,pushTop,pushWidth,pushHeight),
                            "&OK" );
    bttn->onClick( this, (cbw)&SearchOptions::okButton );
    bttn->show();
    bttn = new WPushButton( this, WRect(103,pushTop,pushWidth,pushHeight),
                            "S&et Default" );
    bttn->onClick( this, (cbw)&SearchOptions::SetDefaultSelected );
    bttn->show();
    bttn = new WPushButton( this, WRect(203,pushTop,pushWidth,pushHeight),
                            "Ca&ncel" );
    bttn->onClick( this, (cbw)&SearchOptions::cancelButton );
    bttn->show();
    setWindowState();
    show();
}

static void SearchOptions::InitSearchOptions()
//--------------------------------------------
/* this sets all of the search globals to their default values */
{
    if( userMagicString != NULL ) WBRFree( userMagicString );
    userMagicString = WBRStrDup( META );
    noMagic = FALSE;
    anchorRegExp = TRUE;
    MagicString = META;
    CaseIgnore = TRUE;
    MagicFlag = TRUE;
}

static void SearchOptions::FiniSearchOptions()
//--------------------------------------------
/* just cleans up memory pointed at by static items */
{
    WBRFree( userMagicString );
}

void SearchOptions::setWindowState()
//----------------------------------
{
    bool        bttn1, bttn2, bttn3;

    bttn1 = bttn2 = bttn3 = FALSE;
    if( MagicFlag ) {
        bttn1 = TRUE;
    } else if( noMagic ) {
        bttn2 = TRUE;
    } else {
        bttn3 = TRUE;
    }
    _allowAll->setCheck( bttn1 );
    _disallowAll->setCheck( bttn2 );
    _disallowSome->setCheck( bttn3 );
    _startsWith->setCheck( anchorRegExp );
    _contains->setCheck( !anchorRegExp );
    _case->setCheck( CaseIgnore );
    _magicBox->setText( userMagicString );
}

void SearchOptions::okButton( WWindow* )
//--------------------------------------
{
    int len;

    if( _allowAll->checked() ) {
        MagicFlag = TRUE;
    } else {
        MagicFlag = FALSE;
        if( _disallowAll->checked() ) {
            noMagic = TRUE;
        } else {
            noMagic = FALSE;
        }
    }
    WBRFree( userMagicString );
    len = _magicBox->getTextLength() + 1;
    userMagicString = (char *) WBRAlloc( len );
    _magicBox->getText( userMagicString, len );
    anchorRegExp = _startsWith->checked();
    CaseIgnore = _case->checked();
    quit( TRUE );
}

void SearchOptions::cancelButton( WWindow* )
//------------------------------------------
{
    quit( FALSE );
}

void SearchOptions::SetDefaultSelected( WWindow* )
//------------------------------------------------
{
    InitSearchOptions();
    setWindowState();
}
