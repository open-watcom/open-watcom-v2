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


#include "gdefn.h"
#include "rotate.h"


_WCRTLINK short _WCI86FAR _CGRAPH _lineto( short x, short y )
/*============================================

   This function draws a line from the current position to the specified
   position using the current colour and linestyle.  It returns a
   success flag. */

{
    short               success;        /* line successfully drawn */

    if( _GrProlog() ) {
        success = _L2line( _VtoPhysX( _CurrPos.xcoord ),
                           _VtoPhysY( _CurrPos.ycoord ),
                           _VtoPhysX( x ), _VtoPhysY( y ) );
        _RefreshWindow();
        _moveto( x, y );
        _GrEpilog();
    } else {
        success = 0;
    }
    return( success );
}

Entry1( _LINETO, _lineto ) // alternate entry-point


short _WCI86FAR _L2line( short x1, short y1, short x2, short y2 )
/*==========================================================

   This function draws a line from the current position to the specified
   position using the current colour and linestyle.  It returns a
   success flag. */

{
    short               success;        /* line successfully drawn */
    short               dx, dy;

    success = _L1Line( x1, y1, x2, y2 );
    if( ( _LineStyle != SOLID_LINE ) && _StyleWrap ) {
        dx = abs( x2 - x1 );
        dy = abs( y2 - y1 );
        if( dx < dy ) {
            dx = dy;
        }
        _LineStyle = _wrol( _LineStyle, dx & 15 );
    }
    return( success );
}
