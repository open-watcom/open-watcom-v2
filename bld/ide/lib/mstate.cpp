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


#include "mstate.hpp"
#include "mconfig.hpp"
#include "wobjfile.hpp"
#include "mtool.hpp"
#include "mrule.hpp"            //temp
#include "mtypo.hpp"

Define( MState )

MState::MState( MTool* tool, SwMode mode, MSwitch* sw )
        : _tool( tool )
        , _switch( sw )
        , _toolTag( tool->tag() )
        , _switchTag( "" )
        , _mode( mode )
{
    if( _switch ) {
        sw->getTag( _switchTag );
    }
}

#ifndef NOPERSIST
MState* WEXPORT MState::createSelf( WObjectFile& )
{
    return NULL;
}

void WEXPORT MState::readSelf( WObjectFile& p )
{
    p.readObject( &_toolTag );
    _tool = _config->findTool( _toolTag );
    if( !_tool ) {
        MRule* r = _config->findRule( _toolTag );       //temp
        if( r ) {
            _tool = r->tool();
        } else {
            _tool = _config->nilTool();
        }
        _toolTag = _tool->tag();
    }
    p.readObject( &_switchTag );
    _config->kludgeString( _switchTag );
    FixTypo( _switchTag );
    _switch = _tool->findSwitch( _switchTag );
    if( p.version() > 27 ) {
        p.readObject( &_mode );
    }
}

void WEXPORT MState::writeSelf( WObjectFile& p )
{
    p.writeObject( &_toolTag );
    p.writeObject( &_switchTag );
    p.writeObject( _mode );
}
#endif

void MState::resetRuleRefs()
{
    _tool = _config->findTool( _toolTag );
    _switch = _tool->findSwitch( _switchTag );
}

bool MState::legal()
{
    return _switch != NULL;
}
