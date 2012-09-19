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


#include "mactset.hpp"
#include "maction.hpp"
#include "wobjfile.hpp"

Define( ActionStates )

ActionStates::ActionStates( MAction* action )
    : _action( action )
{
    if( action ) {
        action->name( _actionTag );
    }
}


ActionStates::~ActionStates()
{
    _states.deleteContents();
}

#ifndef NOPERSIST
ActionStates* WEXPORT ActionStates::createSelf( WObjectFile& )
{
    return new ActionStates( NULL );
}

void WEXPORT ActionStates::readSelf( WObjectFile& p )
{
    WObject::readSelf( p );
    p.readObject( &_actionTag );
    p.readObject( &_states );
}

void WEXPORT ActionStates::writeSelf( WObjectFile& p )
{
    WObject::writeSelf( p );
    p.writeObject( &_actionTag );
    p.writeObject( &_states );
}
#endif

bool ActionStates::linkup( WVList& actions )
{
    for( int i=0; i<actions.count(); i++ ) {
        MAction* action = (MAction*)actions[i];
        WString n;
        action->name( n );
        if( n == _actionTag ) {
            _action = action;
            return TRUE;
        }
    }
    return FALSE;
}
