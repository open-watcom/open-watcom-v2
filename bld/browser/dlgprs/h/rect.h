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



#ifndef __RECT_H__

class Rect {
public:
            Rect();
            Rect( const Rect & other );
            Rect( int x, int y, int w, int h );

    bool    operator == ( const Rect & other ) const;
    Rect &  operator = ( const Rect & other );


    int     _x;
    int     _y;
    int     _w;
    int     _h;
};

inline Rect::Rect()
        : _x( 0 )
        , _y( 0 )
        , _w( 0 )
        , _h( 0 )
//-----------------
{
}

inline Rect::Rect( const Rect & o )
        : _x( o._x )
        , _y( o._y )
        , _w( o._w )
        , _h( o._h )
//-------------------------------------
{
}

inline Rect::Rect( int x, int y, int w, int h )
        : _x( x )
        , _y( y )
        , _w( w )
        , _h( h )
//---------------------------------------------
{
}


inline bool Rect::operator == ( const Rect & o ) const
//----------------------------------------------------
{
    return( (_x == o._x) && (_y == o._y) && (_w == o._w) && (_h == o._h) );
}

inline Rect & Rect::operator = ( const Rect & o )
//-----------------------------------------------
{
    _x = o._x;
    _y = o._y;
    _w = o._w;
    _h = o._h;

    return *this;
}


#define __RECT_H__
#endif
