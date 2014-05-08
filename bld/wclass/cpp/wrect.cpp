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


#include "wrect.hpp"
#include "wobjfile.hpp"

Define( WRect )


void WEXPORT WRect::fitInside( const WRect& r ) {
/***********************************************/

    if( _x >= r.w() ) _x = (WOrdinal)(r.w() - 2);
    if( _y >= r.h() ) _y = (WOrdinal)(r.h() - 2);
    if( _x+_w >= r.w() ) _w = (WOrdinal)(r.w() - _x - 1);
    if( _y+_h >= r.h() ) _h = (WOrdinal)(r.h() - _y - 1);
}


bool WEXPORT WRect::contains( WOrdinal x, WOrdinal y ) const {
/************************************************************/

    return( ( ( x >= _x && x <= _x + _w ) && ( y >= _y && y <= _y + _h ) ) );
}


#ifndef NOPERSIST

WRect * WEXPORT WRect::createSelf( WObjectFile & ) {
/**************************************************/

    return( new WRect() );
}


void WEXPORT WRect::readSelf( WObjectFile &p ) {
/**********************************************/

    p.readObject( &_x );
    p.readObject( &_y );
    p.readObject( &_w );
    p.readObject( &_h );
}


void WEXPORT WRect::writeSelf( WObjectFile &p ) {
/***********************************************/

    p.writeObject( _x );
    p.writeObject( _y );
    p.writeObject( _w );
    p.writeObject( _h );
}

#endif
