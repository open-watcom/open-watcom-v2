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


#include "mvswitch.hpp"
#include "wobjfile.hpp"
#include "mvstate.hpp"

Define( MVSwitch )

MVSwitch::MVSwitch( WTokenFile& fil, WString& tok )
    : MSwitch( fil, tok )
    , _quote( '\'' )
    , _optional( false )
    , _multiple( false )
{
    fil.token( _on );
    fil.token( _connector );
    _multiple = ( fil.token( tok ) == "MULTI" );
    _optional = true;
    WString value;
    bool state = false;
    for( int i=0; i<SWMODE_COUNT; i++ ) {
        if( !fil.eol() ) {
            fil.token( tok );
            if( _optional && tok == "ON" ) {
                state = true;
                fil.token( value );
            } else if( _optional && tok == "OFF" ) {
                state = false;
                fil.token( value );
            } else if( _optional && tok == "REQ" ) {
                _optional = false;
                fil.token( value );
            } else {
                value = tok;
            }
        }
        _state[i] = state;
        _value[i] = value;
    }
}

#ifndef NOPERSIST
MVSwitch* WEXPORT MVSwitch::createSelf( WObjectFile& )
{
    return( new MVSwitch() );
}

void WEXPORT MVSwitch::readSelf( WObjectFile& p )
{
    MSwitch::readSelf( p );
    p.readObject( &_on );
    if( p.version() > 30 ) {
        p.readObject( &_connector );
    }
    if( p.version() > 28 ) {
        for( int i=0; i<SWMODE_COUNT; i++ ) {
            p.readObject( &_value[i] );
            p.readObject( &_state[i] );
        }
    } else {
        p.readObject( &_value[SWMODE_RELEASE] );
        p.readObject( &_state[SWMODE_RELEASE] );
        _value[SWMODE_DEBUG] = _value[SWMODE_RELEASE];
        _state[SWMODE_DEBUG] = _state[SWMODE_RELEASE];
    }
    if( p.version() > 28 ) {
        p.readObject( &_multiple );
    }
    p.readObject( &_optional );
}

void WEXPORT MVSwitch::writeSelf( WObjectFile& p )
{
    MSwitch::writeSelf( p );
    p.writeObject( &_on );
    p.writeObject( &_connector );
    for( int i=0; i<SWMODE_COUNT; i++ ) {
        p.writeObject( &_value[i] );
        p.writeObject( _state[i] );
    }
    p.writeObject( _multiple );
    p.writeObject( _optional );
}
#endif

#if 0
static bool needsQuotes( const char* str )
{
    if( str ) {
        for( int i=0; str[i]; i++ ) {
            if( isspace( str[i] ) ) {
                return( true );
            }
        }
    }
    return( false );
}
#endif

void MVSwitch::addValues( WString& str, WStringList& values, bool& first )
{
    int count = values.count();
    for( int i=0; i<count; i++ ) {
        if( !first ) str.concat( ' ' );
        first = false;
        str.concat( _on );
#if 1
        WString& val = values.stringAt( i );
        WString fmt( _connector );
        fmt.concat( "%s" );
        str.concatf( fmt, (const char*)val, "" );
#else
        str.concat( _connector );
        WString& val = values.stringAt( i );
        if( _quote && needsQuotes( val ) ) {
            str.concat( _quote );
            str.concat( val );
            str.concat( _quote );
        } else {
            str.concat( val );
        }
#endif
    }
}

void MVSwitch::addone( WString& str, bool state, WString* value, bool& first )
{
    WStringList values;
    if( _multiple ) {
        values.parseIn( *value );
    } else if( value->size() > 0 ) {
        values.add( new WString( *value ) );
    }
    int count = values.count();
    if( _optional ) {
        if( state ) {
            if( count > 0 ) {
                addValues( str, values, first );
            } else {
                if( !first ) str.concat( ' ' );
                first = false;
                str.concat( _on );
            }
        }
    } else if( count > 0 ) {
        addValues( str, values, first );
    }
}

void MVSwitch::getText( WString& str, MState* state )
{
    bool first = true;
    MVState* st = (MVState*)state;
    addone( str, st->state(), &st->value(), first );
}

void MVSwitch::getText( WString& str, WVList* states, SwMode mode )
{
    WVList found;
    findStates( states, found );
    int icount = found.count();
    bool first = true;
    bool found_match = false;
    if( icount > 0 ) {
        // used to check if( _multiple )
        for( int i=0; i<icount; i++ ) {
            MVState* st = (MVState*)found[i];
            if( st->mode() == mode ) {
                found_match = true;
                addone( str, st->state(), &st->value(), first );
            }
        }
    }
    if( !found_match ) {
        addone( str, _state[mode], &_value[mode], first );
    }
}

