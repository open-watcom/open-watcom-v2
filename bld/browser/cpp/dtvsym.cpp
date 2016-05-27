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


#include <stdlib.h>
#include <wpshbttn.hpp>
#include <wlistbox.hpp>
#include <wmetrics.hpp>

#include "wbrdefs.h"
#include "dtvsym.h"
#include "dtvclass.h"
#include "dtvenum.h"
#include "dtvfunc.h"
#include "symbol.h"
#include "icons.h"
#include "descpnt.h"
#include "util.h"
#include "menuids.h"
#include "browse.h"
#include "viewmgr.h"
#include "brwhelp.h"

static char * Defined = "Defined";
static char * Declared = "Declared";

const int CaptionGadgetSize = 150;   // FIXME -- this is a guess, should be system metrics

static WResource DetailIcons[ DR_SYM_NOT_SYM ] = {
    DTFuncIcon,
    DTClassIcon,
    DTEnumIcon,
    DTTypedefIcon,
    DTVarIcon,
    DTMacroIcon,
    DTLabelIcon
};

static char * SymbolTitleText[ DR_SYM_NOT_SYM ] = {
    "Function",
    "Class",
    "Enum",
    "Typedef",
    "Variable",
    "Macro",
    "Label"
};

DTViewSymbol::DTViewSymbol( const Symbol * sym, bool box )
                : WBRWindow( "" )
                , ViewSymbolDlg( topWindow )
                , _useBox( box )
//----------------------------------------------------------------
{
    _symbol = Symbol::defineSymbol( sym ); // my own copy
    initialize();

    setIcon( DetailIcons[ _symbol->symtype() ] );
    menuManager()->trackPopup( this, MMDetail );
}

DTViewSymbol::~DTViewSymbol()
//---------------------------
{
    delete _descPaint;
    delete _symbol;
}

bool DTViewSymbol::contextHelp( bool is_active_win )
//--------------------------------------------------
{
    if( is_active_win ) {
        WBRWinBase::helpInfo()->sysHelpId( BRH_DETAIL_VIEW );
    }
    return( true );
}

void DTViewSymbol::initialize()
//-----------------------------
// create the detail view of the symbol and size it.
{
    WString         titleText;
    WRect           tmpRect;
    char            fName[_MAX_PATH];
    int             width;
    int             tmpWidth;
    int             height;

    rescale();

    //----------------- "Function initialize" title ----------------

    titleText.printf( "%s %s", SymbolTitleText[ _symbol->symtype() ],
                _symbol->name() );
    setText( titleText.gets() );
    width = getTextExtentX( titleText.gets() ) + CaptionGadgetSize;

    //----------  "Defined: D:\dev\browser\cpp\dtvsym.cpp" line ----------

    if( _symbol->defSourceFile( fName ) ) {
        _fileText.printf( "%s: %s", (_symbol->isDefined()) ? Defined : Declared, fName );
        height = _symbolLineR.r.y();
    } else {
        height = _fileNameR.r.y();
    }
    tmpWidth = _fileNameR.r.x() + abs( _fileNameR.r.w() ) +
                getTextExtentX( _fileText.gets() );

    width = maxInt( width, tmpWidth );

    //------- "void DTViewSymbol::initialize()" line -------

    tmpRect = _symbolLineR.r;
    tmpRect.y( height );

    _descPaint = new DescriptionPaint( this, tmpRect, _symbol );

    height += _symbolBoxR.r.y() - _symbolLineR.r.y();
    tmpWidth = _symbolLineR.r.x() + _descPaint->rect().w() + abs( _symbolLineR.r.w() );
    width = maxInt( width, tmpWidth );

    //------------------ member listbox --------------------
    if( _useBox ) {
        tmpRect = _symbolBoxR.r;
        tmpRect.y( height );

        _symbolBox = new WListBox( this, tmpRect,
                        LStyleNoIntegral | WStyleHScroll | WStyleVScroll );
        _symbolBox->show();
        _symbolBox->setFocus();

        height += __frame.r.h() - _symbolBoxR.r.y();    // this include 2 * dialogFrameHeight
    }


    width += 2 * WSystemMetrics::dialogFrameWidth();
    height += WSystemMetrics::captionSize();

    size( width, height );
    show();
}

static MIMenuID ViewSymMenus[] = {
    MIMenuID( MMDetail, DMDefinition ),
    MIMenuID( MMDetail, DMReferences ),
    MIMenuID( MMDetail, DMUsers )
};

#define NumViewSymMenus ( sizeof(ViewSymMenus) / sizeof(MIMenuID) )

void DTViewSymbol::setMenus( MenuManager * mgr )
//----------------------------------------------
{
    int i;

    for( i = 0; i < NumViewSymMenus; i += 1 ) {
        mgr->registerForMenu( this, ViewSymMenus[ i ] );
    }

    menuManager()->enableMenu( MIMenuID( MMDetail, DMDefinition ),
                                browseTop->canEdit() );
}

void DTViewSymbol::unsetMenus( MenuManager * mgr )
//------------------------------------------------
{
    int i;

    for( i = 0; i < NumViewSymMenus; i += 1 ) {
        mgr->unRegister( ViewSymMenus[ i ] );
    }
}

void DTViewSymbol::menuSelected( const MIMenuID & id )
//----------------------------------------------------
{
    switch( id.mainID() ) {
    case MMDetail:
        switch( id.subID() ) {
        case DMDefinition:
            browseTop->showSource( _symbol );
            break;
        case DMReferences:
            viewManager()->showDetailView( _symbol, ViewSourceRefs );
            break;
        case DMUsers:
            viewManager()->showDetailView( _symbol, ViewSymbolRefs );
            break;
        }
    }
}

bool DTViewSymbol::paint()
//------------------------
{
    WPoint loc( _fileNameR.r.x(), _fileNameR.r.y() );
    drawText( loc, _fileText.gets(), ColorBlack, backgroundColour() );

    return _descPaint->paint();
}

bool DTViewSymbol::leftBttnUp( int x, int y, WMouseKeyFlags )
//-----------------------------------------------------------
{
    int prev = _descPaint->current();
    if( _descPaint->select( x, y ) ) {
        if( prev != _descPaint->current() ) {
            if( prev >= 0 ) {
                invalidateRect( _descPaint->partRect( prev ) );
            }
            invalidateRect( _descPaint->partRect( _descPaint->current() ) );
        }
        return true;
    }
    return false;
}

bool DTViewSymbol::leftBttnDbl( int x, int y, WMouseKeyFlags )
//------------------------------------------------------------
{
    int prev = _descPaint->current();
    if( _descPaint->select( x, y ) ) {
        if( prev != _descPaint->current() ) {
            if( prev >= 0 ) {
                invalidateRect( _descPaint->partRect( prev ) );
            }
            invalidateRect( _descPaint->partRect( _descPaint->current() ) );
        }
        _descPaint->enterHit();
        return true;
    }
    return false;
}

bool DTViewSymbol::keyDown( WKeyCode kc, WKeyState )
//--------------------------------------------------
{
    bool        redraw = false;
    int         prev = _descPaint->current();

    switch( kc ) {
    case WKeyTab:
        redraw = _descPaint->tabHit( false );
        break;
    case WKeyShiftTab:
        redraw = _descPaint->tabHit( true );
        break;
    case WKeyCtrlEnter:
        _descPaint->enterHit();
        break;
    default:
        return false;
    }

    if( redraw ) {
        if( prev >= 0 ) {
            invalidateRect( _descPaint->partRect( prev ) );
        }
        invalidateRect( _descPaint->partRect( _descPaint->current() ) );
    }
    return true;
}

static DTViewSymbol * DTViewSymbol::createView( const Symbol * sym )
//------------------------------------------------------------------
{
    switch( sym->symtype() ) {
    case DR_SYM_CLASS:
        return new DTViewClass( sym );
    case DR_SYM_FUNCTION:
        return new DTViewFunction( sym );
    case DR_SYM_ENUM:
        return new DTViewEnum( sym );
    default:
        return new DTViewSymbol( sym );
    }
}
