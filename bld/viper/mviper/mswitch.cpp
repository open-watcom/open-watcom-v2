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


#include "mswitch.hpp"
#include "wobjfile.hpp"
#include "mstate.hpp"

Define( MSwitch )

MSwitch::MSwitch( WTokenFile& fil, WString& tok )
{
    _panel = (int)fil.token( tok );
    fil.token( _mask );
    fil.token( _text );
    _compatibleText = _text;
    for( int i = 0; i < _compatibleText.size(); i++ ) {
        // Prior to Open Watcom 1.9, the project files used spaces instead of dashes in a
        // few of the switch names.  These old names need to be recognized for compatibility.
        if( _compatibleText[i] == '-' ) {
            _compatibleText.setChar( i, ' ' );
        }
    }
}

#ifndef NOPERSIST
MSwitch* WEXPORT MSwitch::createSelf( WObjectFile& )
{
    return NULL;
}

void WEXPORT MSwitch::readSelf( WObjectFile& p )
{
    p.readObject( &_panel );
    p.readObject( &_mask );
    p.readObject( &_text );
}

void WEXPORT MSwitch::writeSelf( WObjectFile& p )
{
    p.writeObject( _panel );
    p.writeObject( &_mask );
    p.writeObject( &_text );
}
#endif

void MSwitch::displayText( WString& s )
{
    s.puts( text() );
    if( on().size() > 0 ) {
        s.concat( ' ' );
        s.concat( '[' );
        const char* c = on();
        int i;
        for( i=strlen( c ); i>0; i-- ) {
            if( c[i-1] == '\\' ) break;
        }
        s.concat( &c[i] );
        s.concat( ']' );
    }
}

void MSwitch::findStates( WVList* states, WVList& found )
{
    if( states ) {
        int icount = states->count();
        for( int i=0; i<icount; i++ ) {
            MState* st = (MState*)(*states)[i];
            if( st->sw() == this ) {
                found.add( st );
            }
        }
    }
}

void MSwitch::getTag( WString& tag )
{
    tag = _mask;
    tag.concat( _text );
}

void MSwitch::getCompatibleTag( WString& tag )
{
    tag = _mask;
    tag.concat( _compatibleText );
}

MSwitch* MSwitch::addSwitch( WVList& list, const char* mask )
{
    if( _mask.match( mask ) ) {
        return (MSwitch*)list.add( this );
    }
    return NULL;
}
