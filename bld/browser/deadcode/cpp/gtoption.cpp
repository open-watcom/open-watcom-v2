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


#include "projmodl.h"
#include "gtoption.h"

/////////////////////////// TreeOptionCollection ///////////////////////////

void TreeOptionCollection::makeDefault( void ) const
/**************************************************/
{
    for( int i = numGroups(); i > 0; i -= 1 ) {
        getGroup( i - 1 )->makeDefault();
    }
}

void TreeOptionCollection::useDefault( void )
/*******************************************/
{
    for( int i = numGroups(); i > 0; i -= 1 ) {
        getGroup( i - 1 )->useDefault();
    }
}

uint_16 TreeOptionCollection::getUses( int grp )
/**********************************************/
{
    return getGroup( grp )->getUses();
}

Color TreeOptionCollection::getColour( int grp, int ix )
/******************************************************/
{
    return getGroup( grp )->getOption(ix)->getColour();
}

WLineStyle TreeOptionCollection::getStyle( int grp, int ix )
/**********************************************************/
{
    return getGroup( grp )->getOption(ix)->getStyle();
}

int TreeOptionCollection::getThickness( int grp, int ix )
/*******************************************************/
{
    return getGroup( grp )->getOption(ix)->getThickness();
}

/////////////////////////// TreeOptionGroup ///////////////////////////

TreeOptionGroup::TreeOptionGroup( char * name, TreeGroupType type,
                                  TreeGroupUses uses )
                    : _name( name )
                    , _type( type )
                    , _uses( uses )
                    , _default( NULL )
{
}

TreeOptionGroup::TreeOptionGroup( TreeOptionGroup * def )
                    : _name( def->name() )
                    , _type( def->getType() )
                    , _uses( def->getUses() )
                    , _default( def )
{
    for( int i = 0; i < def->numOpts(); i += 1 ) {
        addOption( def->getOption( i ) );
    }
}

TreeOptionGroup::~TreeOptionGroup()
{
    for( int i = _options.count(); i > 0; i -= 1 ) {
        delete _options[ i - 1 ];
    }
}

int  TreeOptionGroup::addOption( TreeOption * def )
/*************************************************/
{
    _options.add( new TreeOption( def ) );
    return _options.count() - 1;
}

int  TreeOptionGroup::addOption( TreeOption const & opt, TreeOption * def )
/**************************************************************************/
{
    _options.add( new TreeOption( opt, def ) );
    return _options.count() - 1;
}

int  TreeOptionGroup::addOption(char * name, Color clr, WLineStyle stl, int thk)
/******************************************************************************/
{
    _options.add( new TreeOption( name, clr, stl, thk ) );
    return _options.count() - 1;
}

void TreeOptionGroup::makeDefault( void )
/***************************************/
{
    for( int i = _options.count(); i > 0; i -= 1 ) {
        _options[ i - 1 ]->makeDefault();
    }

    if( _default != NULL ) {
        _default->_uses = _uses;
    }
}

void TreeOptionGroup::useDefault( void )
/**************************************/
{
    for( int i = _options.count(); i > 0; i -= 1 ) {
        _options[ i - 1 ]->useDefault();
    }
    if( _default != NULL ) {
        _uses = _default->_uses;
    }
}

////////////////////////////// TreeOption /////////////////////////////

TreeOption::TreeOption( char * name, Color clr, WLineStyle stl, int thk )
                : _name( name )
                , _colour( clr )
                , _penStyle( stl )
                , _penThickness( thk )
                , _default( NULL )
{
}

TreeOption::TreeOption( TreeOption * def )
                : _name( def->name() )
                , _colour( def->_colour )
                , _penStyle( def->_penStyle )
                , _penThickness( def->_penThickness )
                , _default( def )
{
}

TreeOption::TreeOption( TreeOption const & clone, TreeOption * def )
                : _default( def )
{
    setOption( clone );
}

void TreeOption::readOption( TreeOption & opt ) const
/***************************************************/
{
    opt._colour         = _colour;
    opt._penStyle       = _penStyle;
    opt._penThickness   = _penThickness;
    opt._name           = _name;
}

void TreeOption::setOption( const TreeOption & opt )
/**************************************************/
{
    _colour         = opt._colour;
    _penStyle       = opt._penStyle;
    _penThickness   = opt._penThickness;
    _name           = opt._name;
}

void TreeOption::setOption( Color clr, WLineStyle stl, int thk )
/**************************************************************/
{
    _colour         = clr;
    _penStyle       = stl;
    _penThickness   = thk;
}

void TreeOption::makeDefault( void ) const
/****************************************/
{
    if( _default != NULL ) {
        readOption( *_default );
    }
}

void TreeOption::useDefault( void )
/*********************************/
{
    if( _default != NULL ) {
        setOption( *_default );
    }
}
