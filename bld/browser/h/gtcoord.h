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


#ifndef __GTRECT_H__
#define __GTRECT_H__

#include <wrect.hpp>

typedef int_32 TreeCoord;

class TreeRect {
public:
        TreeRect( void )
            : _x( 0 ), _y( 0 ), _w( 0 ), _h( 0 ) {}
        TreeRect( TreeCoord nx, TreeCoord ny, TreeCoord nw, TreeCoord nh )
            : _x( nx ), _y( ny ), _w( nw ), _h( nh ) {}
        TreeRect( short nx, short ny, short nw, short nh )
            : _x( nx ), _y( ny ), _w( nw ), _h( nh ) {}

        ~TreeRect() {}

        TreeCoord x( void ) const { return _x; }
        TreeCoord y( void ) const { return _y; }
        TreeCoord w( void ) const { return _w; }
        TreeCoord h( void ) const { return _h; }

        TreeCoord left( void ) const { return _x; }
        TreeCoord right( void ) const { return _x + _w; }
        TreeCoord top( void ) const { return _y; }
        TreeCoord bottom( void ) const { return _y + _h; }

        void x( const TreeCoord nx ) { _x = nx; }
        void y( const TreeCoord ny ) { _y = ny; }
        void w( const TreeCoord nw ) { _w = nw; }
        void h( const TreeCoord nh ) { _h = nh; }

        bool contains( const TreeCoord x, const TreeCoord y ) const
        { return ( ( x >= _x && x <= _x + _w ) && ( y >= _y && y <= _y + _h )); }

        WRect makeWRect() { return WRect( (short)_x, (short)_y, (short)_w, (short)_h ); }

private:

        TreeCoord   _x, _y, _w, _h;
};

#endif // __GTRECT_H__
