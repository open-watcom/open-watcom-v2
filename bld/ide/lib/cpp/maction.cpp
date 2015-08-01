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


#include "mconfig.hpp"
#include "maction.hpp"
#include "wobjfile.hpp"
#include "wfilenam.hpp"

Define( MAction )

WEXPORT MAction::MAction( WTokenFile& fil, WString& tok )
    : _defAction( false )
    , _button( false )
    , _okForMask( false )
{
    _accel = NULL;
    fil.token( _name );
    if( fil.eol() ) {
        _tool = _config->nilTool();
    } else {
        _tool = _config->findTool( fil.token( tok ) ); //this must work
        if( !_tool ) {
            _tool = _config->nilTool();
        }
        if( !fil.eol() ) {
            _okForMask = ( fil.token( tok ) == "OKFORMASK" );
        }
    }
    fil.token( tok );   //read ahead
    for(;;) {
        if( tok == "Command" ) {
            _commands.add( new MCommand( fil, tok ) );
        } else if( tok == "Hint" ) {
            fil.token( _hint );
            fil.token( tok );
        } else if( tok == "AccelKey" ) {
            fil.token( _accelString );
            fil.token( tok );
        } else if( tok == "Button" ) {
            _button = true;
            fil.token( tok );
        } else if( tok == "Default" ) {
            _defAction = true;
            fil.token( tok );
        } else if( tok == "Help" ) {
            fil.token( _help );
            fil.token( tok );
        } else if( tok == "HotKey" ) {
            if( _accel == NULL ) {
                _accel = new MAccelerator( fil, tok );
            }
        } else if( tok == "rem" ) {
            fil.flushLine( tok );
            fil.token( tok );
        } else {
            break;
        }
    }
}

MAction::MAction( const char* name )
    : _name( name )
{
    _accel = NULL;
}

MAction::~MAction()
{
    _commands.deleteContents();
    if( _accel != NULL ) {
        delete _accel;
        _accel = NULL;
    }
}

void MAction::menuName( WString *mname ) {

    mname->truncate( 0 );
    mname->concat( _name );
    if( _accel != NULL ) {
        mname->concat( "\t" );
        mname->concat( _accel->description() );
    }
}

int MAction::expand( WString& command, WFileName* target, const WString& mask, WVList* states, SwMode mode )
{
    MCommand s;
    for( int j=0; j<_commands.count(); j++ ) {
        MCommand* x = (MCommand*)_commands[j];
        const WString& xm = x->mask();
        if( xm.size() == 0 || xm.match( mask ) ) {
            if( j > 0 ) s.concat( '\n' );
            s.concat( *x );
        }
    }
    return( s.expand( command, target, _tool, mask, states, mode ) );
}

int MAction::accelKey()
{
    int accel = 0;
    if( _accelString.size() > 1 ) {
        if( toupper( *_accelString ) == 'F' ) {
            WString x( &_accelString[(size_t)1] );
            accel = 0x006F + (int)x;
        }
    }
    return( accel );
}

#ifndef NOPERSIST
MAction* WEXPORT MAction::createSelf( WObjectFile& )
{
    return( new MAction() );
}

void WEXPORT MAction::readSelf( WObjectFile& p )
{
    WObject::readSelf( p );
    _tool = (MTool*)p.readObject();
//  p.readObject( &_tagMask );
    p.readObject( &_name );
    p.readObject( &_commands );
    p.readObject( &_hint );
}

void WEXPORT MAction::writeSelf( WObjectFile& p )
{
    WObject::writeSelf( p );
    p.writeObject( _tool );
//  p.writeObject( &_tagMask );
    p.writeObject( &_name );
    p.writeObject( &_commands );
    p.writeObject( &_hint );
}
#endif

void MAction::text( WString& t )
{
    size_t icount = _name.size();
    for( size_t i=0; i<icount; i++ ) {
        if( _name[i] != '&' ) {
            t.concat( _name[i] );
        }
    }
}

WKeyCode MAction::menuAccel() {
    if( _accel == NULL ) {
        return( WKeyNone );
    } else {
        return( _accel->keyStroke() );
    }
}
