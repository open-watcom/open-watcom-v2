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


#include "wobjfile.hpp"
#include "mconfig.hpp"
#include "mworkfil.hpp"
#include "mitem.hpp"
#include "mrule.hpp"
#include "mcompon.hpp"

#ifndef PRODUCTION
    #include "mstate.hpp"
#endif

WEXPORT MWorkFile::MWorkFile( const char* name, SwMode mode, MItem* item, MComponent* comp )
    : WFileName( name )
    , _item( item )
    , _browseable( false )
    , _component( comp )
{
    _item->addStates( _states, mode );
}

void MWorkFile::insertStates( MWorkFile* w )
{
    WVList& wStates = w->states();
    for( int i=0; i<wStates.count(); i++ ) {
        _states.insertAt( i, wStates[i] );
    }
}

void MWorkFile::addStates( WVList& states )
{
    for( int i=0; i<_states.count(); i++ ) {
        states.add( _states[i] );
    }
}

void MWorkFile::makeCommand( WString& s, WVList* workFiles )
{
    WVList states; addStates( states ); //do we need to make this copy?
    MRule* rule = _item->rule();
    WString& mask = _component->mask();
    WFileName f( *this );
    f.relativeTo( _component->filename() );
    rule->makeCommand( s, &f, workFiles, mask, &states, _component->mode(), &_browseable );
}

#ifndef PRODUCTION
void MWorkFile::dump( WFile& fil )
{
    MRule* rule = _item->rule();
    WString& mask = _component->mask();

    fil.printf( "#%FP", this );
    fil.printf( ", %s:%s", (const char*)mask, (const char*)rule->tag() );
    fil.printf( ", %s", (const char*)*this );

    WString n;
    int icount = _states.count();
    if( icount > 0 ) {
        bool first = true;
        for( int i=0; i<icount; i++ ) {
            MState* st = (MState*)_states[i];
            WString temp;
            st->sw()->getText( temp, st );
            if( temp.size() > 0 ) {
                if( first ) n.concat( " (" );
                if( !first ) n.concat( ' ' );
                n.concat( temp );
                first = false;
            }
        }
        if( !first ) n.concat( ')' );
    }
    fil.printf( " %s\n", (const char*)n );
}
#endif
