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


#include <wcheckbx.hpp>
#include <wcolrdlg.hpp>
#include <wgroupbx.hpp>
#include <wpshbttn.hpp>
#include <wname.hpp>
#include <wstring.hpp>

#include "gtdrawop.h"
#include "gtsetop.h"
#include "screendv.h"
#include "paintinf.h"

#define DialogWidth 358
#define EmptyHeight 75
#define GroupSep    6

#define ObjSetX         263
#define ObjSetH         24
#define ObjSetW         70

#define ObjTextX        17
#define ObjTextYOff     3
#define ObjTextW        88
#define ObjTextH        16

#define ObjLineX        118
#define ObjLineW        135
#define ObjLineYOff     10

#define ObjBoxX         118
#define ObjBoxH         24
#define ObjBoxW         70

#define GroupX          6
#define GroupW          335
#define GroupEmpty      52
#define ObjHeight       30

#define OptionSep       6

#define GroupBoxYOff    25  // y-offset from groupbox to checkboxes
#define GroupBoxH       16  // height of checkboxes

#define GroupColourX    57
#define GroupColourW    61
#define GroupStyleX     125
#define GroupStyleW     90
#define GroupThickX     221
#define GroupThickW     118

#define okX         6
#define okW         63
#define okH         24

#define cancelX     78
#define cancelW     63
#define cancelH     24

#define defaultX    157
#define defaultW    63
#define defaultH    24

#define makeX       226
#define makeW       114
#define makeH       24

TreeDrawOptions::TreeDrawOptions( WWindow * parent, TreeOptionCollection * coll,
                                  int x, int y )
                    : WDialog( parent, WRect(x,y,DialogWidth,EmptyHeight),
                               coll->name() )
                    , _nextY( GroupSep )
                    , _rect(x,y,DialogWidth,EmptyHeight)
                    , _coll( coll )
{
}

TreeDrawOptions::~TreeDrawOptions()
{
    show( WWinStateHide );

    for( int i = _groups.count(); i > 0; i -= 1 ) {
        delete _groups[ i - 1 ];
    }
}

void TreeDrawOptions::initialize()
/********************************/
{
    for( int i = 0; i < _coll->numGroups(); i += 1 ) {
        addGroup( _coll->getGroup( i ) );
    }
    prepare();
}

void TreeDrawOptions::applyChanges( void )
/****************************************/
{
    for( int i = _groups.count(); i > 0; i -= 1 ) {
        _groups[ i - 1 ]->applyChanges();
    }
}

bool TreeDrawOptions::paint()
/***************************/
{
    ScreenDev   dev;

    dev.open( this );
    for( int i = _groups.count(); i > 0; i -= 1 ) {
        _groups[ i - 1 ]->paint( &dev );
    }
    dev.close();
    return TRUE;
}

void TreeDrawOptions::prepare( void )       // prepare to be shown
/***********************************/
{
    drawButtons();

    size( DialogWidth, _rect.h() );
}

void TreeDrawOptions::drawButtons( void )
/***************************************/
{
    WPushButton * bttn;

    _nextY += GroupSep;

    WRect okRect(okX,_nextY,okW,okH);
    WRect cancelRect(cancelX,_nextY,cancelW,cancelH);
    WRect defaultRect(defaultX,_nextY,defaultW,defaultH);
    WRect makeRect(makeX,_nextY,makeW,makeH);

    bttn = new WPushButton( this, okRect, "OK" );
    bttn->onClick( this, (cbw) &TreeDrawOptions::okButton );
    bttn->show();

    bttn = new WPushButton( this, cancelRect, "Cancel" );
    bttn->onClick( this, (cbw) &TreeDrawOptions::cancelButton );
    bttn->show();

    bttn = new WPushButton( this, defaultRect, "&Default" );
    bttn->onClick( this, (cbw) &TreeDrawOptions::defaultButton );
    bttn->show();

    bttn = new WPushButton( this, makeRect, "&Make Default" );
    bttn->onClick( this, (cbw) &TreeDrawOptions::makeDefaultButton );
    bttn->show();
}

void TreeDrawOptions::okButton( WWindow * )
/*****************************************/
{
    quit( TRUE );
}

void TreeDrawOptions::cancelButton( WWindow * )
/*********************************************/
{
    quit( FALSE );
}

void TreeDrawOptions::defaultButton( WWindow * )
/**********************************************/
{
    for( int i = _groups.count(); i > 0; i -= 1 ) {
        _groups[ i - 1 ]->useDefault();
    }
    invalidate();
}

void TreeDrawOptions::makeDefaultButton( WWindow * )
/**************************************************/
{
    for( int i = _groups.count(); i > 0; i -= 1 ) {
        _groups[ i - 1 ]->makeDefault();
    }
}

TreeDrawOptions::OptionGroup * TreeDrawOptions::addGroup( TreeOptionGroup * grp )
/*******************************************************************************/
{
    OptionGroup * group;
    WRect         r;

    group = new OptionGroup( this, _nextY, grp );
    _groups.add( group );

    for( int i = _groups.count() - 1; i > 0; i -= 1 ) {
        if( _groups[ i - 1 ]->getType() == grp->getType() ) {
            group->enableBox( _groups[ i - 1 ]->getUses(), FALSE );
            _groups[ i - 1 ]->enableBox( grp->getUses(), FALSE );
        }
    }

    group->getRect( r );
    _nextY += r.h() + GroupSep;
    _rect.h( _rect.h() + r.h() + GroupSep );

    return group;
}

void TreeDrawOptions::checkNoContend( uint_16 uses, TreeGroupType typ,
                                      bool state )
/**************************************************************************/
{
    for( int i = _groups.count(); i > 0; i -= 1 ) {
        if( _groups[ i - 1 ]->getType() == typ ) {
            _groups[ i - 1 ]->enableBox( uses, state );
        }
    }
}

//////////////////////// Option ////////////////////////////////////

TreeDrawOptions::Option::Option( OptionGroup * parentGrp, int y,
                                 TreeOption * clone )
        : _parentGroup( parentGrp )
        , _y(y)
        , _colour( clone->getColour() )
        , _penStyle( clone->getStyle() )
        , _penThickness( clone->getThickness() )
        , _clone( clone )
{
    WPushButton * bttn;
    WName *       name;

    bttn = new WPushButton( _parentGroup->getParent(),
                            WRect(ObjSetX,y,ObjSetW,ObjSetH), "Change..." );
    bttn->onClick( this, (cbw) &Option::setSelected );
    bttn->show();
    _set = bttn;
    _set->enable( _parentGroup->getUses() != TreeGroupNone );

    name = new WName( _parentGroup->getParent(),
                     WRect(ObjTextX,_y+ObjTextYOff,ObjTextW,ObjTextH),
                     bttn, clone->name() );
    name->show();
}

TreeDrawOptions::Option::~Option()
{
}

void TreeDrawOptions::Option::enableSet( bool state )
/***************************************************/
{
    _set->enable( state );
}

void TreeDrawOptions::Option::useDefault( void )
/**********************************************/
{
    TreeOption newOp( _clone->getDefault() );
    _colour = newOp.getColour();
    _penStyle = newOp.getStyle();
    _penThickness = newOp.getThickness();
}

void TreeDrawOptions::Option::applyChanges( void )
/************************************************/
{
    _clone->setOption( _colour, _penStyle, _penThickness );
}

void TreeDrawOptions::Option::paint( OutputDevice *dev )
/******************************************************/
{
    uint_16 uses = _parentGroup->getUses();

    Color       clr = (uses & TreeGroupColour) ? _colour : ColorBlack;
    WLineStyle  stl = (uses & TreeGroupStyle)  ? _penStyle : LS_PEN_SOLID;
    int         thk = (uses & TreeGroupThickness) ? _penThickness : 1;

    PaintInfo   pn( clr, thk, stl );
    dev->setPaintInfo( &pn );

    if( _parentGroup->getType() == TreeGroupLine ) {
        dev->moveTo( ObjLineX, _y + ObjLineYOff );
        dev->lineTo( ObjLineX + ObjLineW, _y + ObjLineYOff );
    } else {
        WRect r( ObjBoxX, _y, ObjBoxW, ObjBoxH );
        dev->rectangle( r );
    }
}

void TreeDrawOptions::Option::getRect( WRect & r ) const
/******************************************************/
{
    r.x( ObjTextX );
    r.y( _y );
    r.w( ObjSetX + ObjSetW - ObjTextX );
    r.h( ObjSetH );
}

void TreeDrawOptions::Option::setSelected( WWindow * )
/****************************************************/
{
    WString str;

    // NYI - get rid of &'s
    str.printf( "%s: %s", _parentGroup->name(), name() );

    //NYI - get x / y coords right

    TreeSetOption * setOp = new TreeSetOption( _parentGroup->getParent(),
                                    str, 50, 50,
                                    _colour, _penStyle, _penThickness,
                                    _parentGroup->getUses(),
                                    _parentGroup->getType() == TreeGroupLine,
                                    _clone->getDefault() );

    if( setOp->process() ) {
        setOp->readOpts( _colour, _penStyle, _penThickness );
    }

    setOp->show( WWinStateHide );
    delete setOp;

    _parentGroup->getParent()->invalidate();
}

//////////////////////// OptionGroup ////////////////////////////////////

TreeDrawOptions::OptionGroup::OptionGroup( TreeDrawOptions * parent, int y,
                                           TreeOptionGroup * clone )
            : _parent( parent )
            , _rect( GroupX, y, GroupW, GroupEmpty + ObjHeight * clone->numOpts() )
            , _clone( clone )
            , _uses( clone->getUses() )
{
    WGroupBox * box;

    box = new WGroupBox( parent, _rect, clone->name() );
    box->show();

    _colour = new WCheckBox( parent, WRect(GroupColourX,y+GroupBoxYOff,GroupColourW,GroupBoxH),
                           "Colour" );
    _colour->onClick( this, (cbw) &OptionGroup::boxSelected );

    _style = new WCheckBox( parent, WRect(GroupStyleX,y+GroupBoxYOff,GroupStyleW,GroupBoxH),
                           "Line Style" );
    _style->onClick( this, (cbw) &OptionGroup::boxSelected );

    _thickness = new WCheckBox( parent, WRect(GroupThickX,y+GroupBoxYOff,GroupThickW,GroupBoxH),
                           "Line Thickness" );
    _thickness->onClick( this, (cbw) &OptionGroup::boxSelected );

    _colour->setCheck( _uses & TreeGroupColour );
    _colour->show();
    _style->setCheck( _uses & TreeGroupStyle );
    _style->show();
    _thickness->setCheck( _uses & TreeGroupThickness );
    _thickness->show();

    _nextY = y + GroupBoxYOff + GroupBoxH + OptionSep;

    for( int i = 0; i < clone->numOpts(); i += 1 ) {
        addOption( clone->getOption( i ) );
    }
}

TreeDrawOptions::OptionGroup::~OptionGroup()
{
    for( int i = _options.count(); i > 0; i -= 1 ) {
        delete _options[ i - 1 ];
    }
}

void TreeDrawOptions::OptionGroup::applyChanges( void )
/*****************************************************/
{
    _clone->setUses( _uses );
    for( int i = _options.count(); i > 0; i -= 1 ) {
        _options[ i - 1 ]->applyChanges();
    }
}

void TreeDrawOptions::OptionGroup::makeDefault( void )
/****************************************************/
{
    applyChanges();
    _clone->makeDefault();
}

void TreeDrawOptions::OptionGroup::useDefault( void )
/***************************************************/
{
    for( int i = _options.count(); i > 0; i -= 1 ) {
        _options[ i - 1 ]->useDefault();
    }
    _uses = _clone->getDefault()->getUses();

    _colour->setCheck( _uses & TreeGroupColour );
    _style->setCheck( _uses & TreeGroupStyle );
    _thickness->setCheck( _uses & TreeGroupThickness );

    for( i = _options.count(); i > 0; i -= 1 ) {
        _options[ i - 1 ]->enableSet( _uses != TreeGroupNone );
    }
}

void TreeDrawOptions::OptionGroup::addOption( TreeOption * op )
/*************************************************************/
{
    WRect    r;
    Option * obj = new Option( this, _nextY, op );

    _options.add( obj );

    obj->getRect( r );
    _nextY += r.h() + OptionSep;
}

void TreeDrawOptions::OptionGroup::paint( OutputDevice *dev )
/***********************************************************/
{
    for( int i = _options.count(); i > 0; i -= 1 ) {
        _options[ i - 1 ]->paint( dev );
    }
}

void TreeDrawOptions::OptionGroup::enableBox( uint_8 uses, bool state )
/*********************************************************************/
{
    if( uses & TreeGroupColour ) {
        _colour->enable( state );
    }

    if( uses & TreeGroupStyle ) {
        _style->enable( state );
    }

    if( uses & TreeGroupThickness ) {
        _thickness->enable( state );
    }
}

void TreeDrawOptions::OptionGroup::boxSelected( WWindow * win )
/*************************************************************/
{
    uint_16         newUse;
    uint_16         oldUse = getUses();
    WCheckBox *     box = (WCheckBox *) win;
    bool            state = box->checked();

    newUse = (box == _colour) ? TreeGroupColour
                              : (box == _style) ? TreeGroupStyle
                                                : TreeGroupThickness;

    _parent->checkNoContend( newUse, getType(), !state );
    if( state ) {
        enableBox( newUse, TRUE );
    }

    if( state ) {
        _uses |= newUse;
    } else {
        _uses &= (uint_8) ~newUse;
    }

    if( _uses > TreeGroupNone && oldUse == TreeGroupNone ) {
        for( int i = _options.count(); i > 0; i -= 1 ) {
            _options[ i - 1 ]->enableSet( TRUE );
        }
    } else {
        if( _uses == TreeGroupNone && oldUse > TreeGroupNone ) {
            for( int i = _options.count(); i > 0; i -= 1 ) {
                _options[ i - 1 ]->enableSet( FALSE );
            }
        }
    }
    _parent->invalidate();
}
