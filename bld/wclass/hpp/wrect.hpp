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


#ifndef wrect_class
#define wrect_class

#include "wregion.hpp"
#include "wordinal.hpp"


WCLASS WRect : public WRegion {
    Declare( WRect )
    public:
        WEXPORT WRect( WOrdinal x=0, WOrdinal y=0, WOrdinal w=0, WOrdinal h=0 )
                : _x( x ), _y( y ), _w( w ), _h( h ) {}
        WEXPORT ~WRect() {}
        WOrdinal WEXPORT x() const { return _x; }
        WOrdinal WEXPORT y() const { return _y; }
        WOrdinal WEXPORT w() const { return _w; }
        WOrdinal WEXPORT h() const { return _h; }
        void WEXPORT x( WOrdinal x ) { _x = x; }
        void WEXPORT y( WOrdinal y ) { _y = y; }
        void WEXPORT w( WOrdinal w ) { _w = w; }
        void WEXPORT h( WOrdinal h ) { _h = h; }

        void WEXPORT fitInside( const WRect& r );
        bool WEXPORT contains( WOrdinal x, WOrdinal y ) const;
    private:
        WOrdinal        _x, _y, _w, _h;
};

#endif
