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


#include "mtoolitm.hpp"
#include "mconfig.hpp"
#include "wobjfile.hpp"

Define( MToolItem )

WEXPORT MToolItem::MToolItem( WTokenFile& fil, WString& tok )
    : _toolType( TOOL_UNDEFINED )
{
    _toolId = (int)fil.token( tok );
    fil.token( tok );
    if( tok == "Project" ) {
        _toolType = TOOL_PROJECT_ACTION;
    } else if( tok == "Target" ) {
        _toolType = TOOL_TARGET_ACTION;
    } else if( tok == "Source" ) {
        _toolType = TOOL_ITEM_ACTION;
    }
    fil.token( _actionName );
    if( !fil.eol() ) {
        fil.token( _hint );
    }
    fil.token( tok );   //read ahead
}

#ifndef NOPERSIST
MToolItem* WEXPORT MToolItem::createSelf( WObjectFile& )
{
    return new MToolItem();
}

void WEXPORT MToolItem::readSelf( WObjectFile& p )
{
    WObject::readSelf( p );
    p.readObject( &_toolId );
    p.readObject( &_toolType );
    p.readObject( &_actionName );
    p.readObject( &_hint );
}

void WEXPORT MToolItem::writeSelf( WObjectFile& p )
{
    WObject::writeSelf( p );
    p.writeObject( _toolId );
    p.writeObject( &_toolType );
    p.writeObject( &_hint );
}
#endif
