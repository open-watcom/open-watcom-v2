/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2023      The Open Watcom Contributors. All Rights Reserved.
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


#include "idecfg.h"
#include "wobjfile.hpp"
#include "mconfig.hpp"
#include "mstate.hpp"
#include "mrule.hpp"            //temp
#include "mtypo.hpp"

Define( MState )

MState::MState( MTool* tool, SwMode mode, MSwitch* sw, bool state )
        : _toolTag( tool->tag() )
        , _tool( tool )
        , _switchTag( "" )
        , _switch( sw )
        , _mode( mode )
        , _state( state )
{
    //
    // update _switchTag to current configuration file version
    //
    if( _switch != NULL ) {
        sw->getTag( _switchTag );
    }
}

#ifndef NOPERSIST
MState* WEXPORT MState::createSelf( WObjectFile& )
{
    return( NULL );
}

void WEXPORT MState::readSelf( WObjectFile& p )
{
    //
    // Open Watcom IDE configuration/project files up to version 1.9 are buggy
    // There are many switch ID's which were changed by incompatible way
    // IDE uses various hacks to fix it later instead of proper solution
    // It is very hard to detect what was broken in each OW version because
    // there vere no change to version number of project files
    //
    p.readObject( &_toolTag );
    _tool = _config->findTool( _toolTag );
    if( _tool == NULL ) {
        MRule* r = _config->findRule( _toolTag );       //temp
        if( r ) {
            _tool = r->tool();
        } else {
            _tool = _config->nilTool();
        }
        _toolTag = _tool->tag();
    }
    p.readObject( &_switchTag );
    //
    // if necessary then update mask in _switchTag to current configuration
    // file version
    //
    _config->kludgeMask( _switchTag );
    //
    // Open Watcom IDE configuration/project files are buggy
    // There are many switch ID's which were changed by incompatible way
    // IDE uses various hacks to fix it later instead of proper solution
    // It is very hard to detect what was broken in each OW version because
    // there vere no change to version number of project files
    //
    // explicit search of _switchTag for current configuration file
    //
    _switch = _tool->findSwitch( _switchTag );
    if( _switch == NULL ) {
        if( p.version() < 42 ) {
            //
            // old project files use switch GUI description as switch tag
            // this old "tag" need to be translated to new tag/ID used by
            // new version 5 of configuration file format
            //
            // first try explicit search of _switchTag for current configuration
            // file
            //
            WString* swid;
            WString swtext = _switchTag;
            swtext.chop( MASK_SIZE );
            swid = _tool->findSwitchIdByText( swtext );
            if( swid == NULL ) {
                //
                // try un-exact search of _switchTag for current configuration
                // file
                // - ignore character case
                // - try to ignore difference between space and dash
                //
                swid = _tool->findSwitchIdByText( swtext, 1 );
            }
            if( swid != NULL ) {
                swtext = _switchTag;
                swtext.truncate( MASK_SIZE );
                swtext.concat( *swid );
                _switch = _tool->findSwitch( swtext );
            }
        }
        //
        // update _switchTag to current configuration file version
        //
        if( _switch != NULL ) {
            _switch->getTag( _switchTag );
        }
    }
    if( p.version() > 27 ) {
        p.readObject( &_mode );
    }
    if( p.version() > 40 ) {
        p.readObject( &_state );
    }
}

void WEXPORT MState::writeSelf( WObjectFile& p )
{
    p.writeObject( &_toolTag );
    p.writeObject( &_switchTag );
    p.writeObject( _mode );
    p.writeObject( _state );
}
#endif

void MState::resetRuleRefs()
{
    _tool = _config->findTool( _toolTag );
    _switch = _tool->findSwitch( _switchTag );
}

bool MState::legal()
{
    return( _switch != NULL );
}
