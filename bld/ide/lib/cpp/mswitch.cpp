/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2023 The Open Watcom Contributors. All Rights Reserved.
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

Define( MSwitch )

MSwitch::MSwitch( WTokenFile& fil, WString& tok )
{
    _panel = (int)fil.token( tok );
    fil.token( _mask );
    fil.token( _id );
    fil.token( _on );
    fixup();
}

#ifndef NOPERSIST
MSwitch* WEXPORT MSwitch::createSelf( WObjectFile& )
{
    return( NULL );
}

void WEXPORT MSwitch::readSelf( WObjectFile& p )
{
    p.readObject( &_panel );
    p.readObject( &_mask );
    p.readObject( &_id );
    p.readObject( &_on );
    fixup();
}

void WEXPORT MSwitch::writeSelf( WObjectFile& p )
{
    p.writeObject( _panel );
    p.writeObject( &_mask );
    p.writeObject( &_id );
    p.writeObject( &_on );
}
#endif

void MSwitch::addOptText( WString& s )
{
    if( _on.size() > 0 ) {
        s.concat( ' ' );
        s.concat( '[' );
        const char* c = _on;
        size_t i;
        for( i=strlen( c ); i>0; i-- ) {
            if( c[i-1] == '\\' ) {
                break;
            }
        }
        s.concat( &c[i] );
        s.concat( ']' );
    }
}

void MSwitch::findStates( WVList* states, WVList& found )
{
    if( states != NULL ) {
        int icount = states->count();
        for( int i=0; i<icount; i++ ) {
            MState* st = (MState*)(*states)[i];
            if( st->sw() == this ) {
                found.add( st );
            }
        }
    }
}

bool MSwitch::isTagEqual( const char* swtag, int kludge ) const
{
    for( int i = 0; i < MASK_SIZE; i++ ) {
        if( _mask[i] != *swtag++ ) {
            return( false );
        }
    }
    for( int i = 0; i < _idlen; i++ ) {
        int ct = (unsigned char)_id[i];
        int cs = (unsigned char)*swtag++;
        if( cs == '\0' )
            return( false );
        if( ct == cs )
            continue;
        if( kludge == 1 ) {
            // ignore dash/space mismatch
            if( cs == '-' && ct == ' ' || cs == ' ' && ct == '-' )
                continue;
            // ignore upper/lower case mismatch
            if( toupper( cs ) == toupper( ct ) ) {
                continue;
            }
//        } else if( kludge == 2 ) {
        }
        return( false );
    }
    return( *swtag == '\0' );
}

MSwitch* MSwitch::addSwitch( WVList& list, const char* mask )
{
    if( _mask.match( mask ) ) {
        return( (MSwitch*)list.add( this ) );
    }
    return( NULL );
}
