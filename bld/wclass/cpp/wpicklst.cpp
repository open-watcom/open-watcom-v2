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


#include "wpicklst.hpp"
#include "wobjfile.hpp"

Define( WPickList )

#ifndef NOPERSIST
WPickList* WEXPORT WPickList::createSelf( WObjectFile& )
{
    return new WPickList();
}

void WEXPORT WPickList::readSelf( WObjectFile& p )
{
    setUpdates( FALSE );
    WVList::readSelf( p );
    setUpdates();
}

void WEXPORT WPickList::writeSelf( WObjectFile& p )
{
        WVList::writeSelf( p );
}
#endif

WObject* WEXPORT WPickList::add( WObject* obj )
{
        obj = WVList::add( obj );
        updateAllViews();
        return obj;
}

WObject* WPickList::replaceAt( int index, WObject* obj )
{
    obj = WVList::replaceAt( index, obj );
    updateAllViews();
    return obj;
}

WObject* WPickList::insertAt( int index, WObject* obj )
{
    obj = WVList::insertAt( index, obj );
    updateAllViews();
    return obj;
}

WObject* WEXPORT WPickList::removeAt( int index )
{
        WObject* obj = WVList::removeAt( index );
        updateAllViews();
        return obj;
}
