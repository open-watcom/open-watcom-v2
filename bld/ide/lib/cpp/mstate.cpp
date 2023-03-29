/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2023-2023 The Open Watcom Contributors. All Rights Reserved.
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

Define( MState )

MState::MState( MTool* tool, SwMode mode, MSwitch* sw, bool state )
        : _toolTag( tool->tag() )
        , _tool( tool )
        , _switchTag( "" )
        , _switch( sw )
        , _mode( mode )
        , _state( state )
{
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
    _config->kludgeMask( _switchTag );
    //
    // fix _switchTag for current version of configuration files
    // it use various hacks in dependency on project files version
    //
    _switch = _tool->findSwitch( _switchTag, p.version() );
    if( p.version() > 27 ) {
        p.readObject( &_mode );
    }
    if( p.version() >= 50 ) {
        p.readObject( &_state );
    }
}

void WEXPORT MState::readState( WObjectFile& p )
{
    p.readObject( &_state );
}

void WEXPORT MState::writeSelf( WObjectFile& p )
{
    p.writeObject( &_toolTag );
    p.writeObject( &_switchTag );
    p.writeObject( _mode );
  #if IDE_CFG_VERSION_MAJOR > 4
    p.writeObject( _state );
  #endif
}

  #if IDE_CFG_VERSION_MAJOR < 5
void WEXPORT MState::writeState( WObjectFile& p )
{
    p.writeObject( _state );
}
  #endif
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
