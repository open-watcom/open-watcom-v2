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
#include <wgroupbx.hpp>
#include <wtext.hpp>
#include <wname.hpp>

#include "util.h"
#include "gtsetop.h"
#include "screendv.h"

#define DialogWidth     435
#define DialogHeight    230

const static WRect ExampleNode(80,11,64,20);
const static WRect ExampleLine(80,11,252,20);
const static WRect ContainLine(78,9,258,24);
const static WRect AllStyleLines(14,64,173,94);     // used for erasing
const static WRect AllThickLines(239,64,173,94);

#define NumStyles 5
const static WLineStyle PenStyle[ NumStyles ] = {
                    LS_PEN_SOLID,
                    LS_PEN_DASH,
                    LS_PEN_DOT,
                    LS_PEN_DASHDOT,
                    LS_PEN_DASHDOTDOT
                };
const static WRect StyleLine[ NumStyles ] = {
                    WRect(16,66,167,10),
                    WRect(16,81,167,10),
                    WRect(16,102,167,10),
                    WRect(16,122,167,10),
                    WRect(16,144,167,10)
                };

const static WRect OKRect       (9,170,69,25);
const static WRect CancelRect   (86,170,69,25);
const static WRect ColourRect   (350,9,69,25);
const static WRect DefaultRect  (167,170,69,25);
const static WRect MakeRect     (244,170,98,25);
const static WRect HelpRect     (350,170,69,25);
const static WRect StyleBox     (9,40,189,120);
const static WRect ThickBox     (230,40,189,120);

#define NumThick 5
const static int PenThickness[ NumThick ] = { 1, 2, 3, 4, 5 };
const static WRect ThickLine[ NumThick ] = {
                    WRect(239,66,167,10),
                    WRect(239,81,167,10),
                    WRect(239,102,167,10),
                    WRect(239,122,167,10),
                    WRect(239,144,167,10)
                };

const static WRect ExampleRect  (9,14,55,14);


TreeSetOption::TreeSetOption( WWindow * parent, const char * title, int x,int y,
                              Color colr, WLineStyle pStyle, int thickness,
                              uint_16 uses, bool isLine, TreeOption * def )
        : WDialog( parent, WRect( x, y, DialogWidth, DialogHeight ), title )
        , _parent( parent )
        , _colour( colr )
        , _penStyle( pStyle )
        , _thickness( thickness )
        , _isLine( isLine )
        , _uses( uses )
        , _default( def )
{
}

void TreeSetOption::initialize()
//------------------------------
{
    WPushButton * bttn;
    WGroupBox *   group;
    WText *       text;

    text = new WText( this, ExampleRect, "Example:", TextStyleLeftNoWrap );
    text->show();

    bttn = new WPushButton( this, ColourRect, "&Colour..." );
    bttn->onClick( this, (cbw) & TreeSetOption::colourSelected );
    bttn->enable( _uses & TreeGroupColour );
    bttn->show();

    group = new WGroupBox( this, StyleBox, "Line Style" );
    group->enable( _uses & TreeGroupStyle );
    group->show();

    group = new WGroupBox( this, ThickBox, "Thickness" );
    group->enable( _uses & TreeGroupThickness );
    group->show();

    bttn = new WPushButton( this, OKRect, "OK" );
    bttn->onClick( this, (cbw) & TreeSetOption::okButton );
    bttn->show();

    bttn = new WPushButton( this, CancelRect, "Cancel" );
    bttn->onClick( this, (cbw) & TreeSetOption::cancelButton );
    bttn->show();

    bttn = new WPushButton( this, MakeRect, "&Reset Default" );
    bttn->onClick( this, (cbw) & TreeSetOption::makeDefault );
    bttn->show();

    bttn = new WPushButton( this, DefaultRect, "&Default" );
    bttn->onClick( this, (cbw) & TreeSetOption::useDefault );
    bttn->show();

    bttn = new WPushButton( this, HelpRect, "&Help" );
    bttn->onClick( this, (cbw) & TreeSetOption::help );
    bttn->show();
}

bool TreeSetOption::paint()
//-------------------------
{
    ScreenDev   dev;
    Color       clr = (_uses & TreeGroupColour) ? _colour : ColorBlack;
    WLineStyle  stl = (_uses & TreeGroupStyle)  ? _penStyle : LS_PEN_SOLID;
    int         thk = (_uses & TreeGroupThickness) ? _thickness : 1;
    PaintInfo   customPen(clr,thk,stl);

    dev.open( this );

    dev.setPaintInfo( &customPen );
    if( _isLine ) {
        dev.moveTo( ExampleLine.x(), ExampleLine.y() + ExampleLine.h() / 2 );
        dev.lineTo( ExampleLine.x() + ExampleLine.w(),
                     ExampleLine.y() + ExampleLine.h() / 2 );
    } else {
        dev.rectangle( ExampleNode );
    }

    for( int i = 0; i < NumStyles; i += 1 ) {
        PaintInfo stylePen(_colour,_thickness,PenStyle[ i ]);
        dev.setPaintInfo( &stylePen );

        dev.moveTo( StyleLine[i].x(), StyleLine[i].y() + StyleLine[i].h() / 2 );
        dev.lineTo( StyleLine[i].x() + StyleLine[i].w(),
                     StyleLine[i].y() + StyleLine[i].h() / 2 );
    }

    for( i = 0; i < NumThick; i += 1 ) {
        PaintInfo thickPen(_colour,PenThickness[ i ],_penStyle);

        dev.setPaintInfo( &thickPen );

        dev.moveTo( ThickLine[i].x(), ThickLine[i].y() + ThickLine[i].h() / 2 );
        dev.lineTo( ThickLine[i].x() + ThickLine[i].w(),
                    ThickLine[i].y() + ThickLine[i].h() / 2 );
    }

    dev.close();
    return TRUE;
}

void TreeSetOption::readOpts( Color & clr, WLineStyle & penst, int & wid )
//------------------------------------------------------------------------
{
    clr = _colour;
    penst = _penStyle;
    wid = _thickness;
}

void TreeSetOption::colourSelected( WWindow * )
//---------------------------------------------
{
    Color clr = _colour;
    WColorDialog * dlg = new WColorDialog( this );

    dlg->show();

    if( dlg->chooseColor( clr, &_colour ) ) {
        invalidate();
    }

    dlg->show( WWinStateHide );
    delete dlg;
}

bool TreeSetOption::leftBttnDn( int x, int y, WMouseKeyFlags )
//------------------------------------------------------------
{
    if( _uses & TreeGroupStyle ) {
        for( int i = 0; i < NumStyles; i += 1 ) {
            if( StyleLine[ i ].contains( x, y ) ) {
                _penStyle = PenStyle[ i ];
                invalidate();
                break;
            }
        }
    }

    if( _uses & TreeGroupThickness ) {
        for( int i = 0; i < NumThick; i += 1 ) {
            if( ThickLine[ i ].contains( x, y ) ) {
                _thickness = PenThickness[ i ];
                invalidate();
                break;
            }
        }
    }

    return TRUE;
}

void TreeSetOption::makeDefault( WWindow * )
//------------------------------------------
{
    _default->setOption( _colour, _penStyle, _thickness );
}

void TreeSetOption::useDefault( WWindow * )
//-----------------------------------------
{
    _colour = _default->getColour();
    _penStyle = _default->getStyle();
    _thickness = _default->getThickness();
    invalidate();
}

void TreeSetOption::okButton( WWindow * )
//---------------------------------------
{
    quit( TRUE );
}

void TreeSetOption::cancelButton( WWindow * )
//-------------------------------------------
{
    quit( FALSE );
}

void TreeSetOption::help( WWindow * )
//-----------------------------------
{
    noHelpAvailable();
}
