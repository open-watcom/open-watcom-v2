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


#include <gdefn.h>


_WCRTLINK short _WCI86FAR _CGRAPH _setwindow( short invert, double left, double top,
/*==========================*/ double right, double bottom )

/* This function maps the current viewport to the window given by
   the corners ( left, top ) and ( right, bottom ). If the "invert" flag
   is set, the limits "top" and "bottom" are inverted.  */

{
    float               l, r;
    float               t, b;
    float               tmp;

    if( !_GraphMode() ) {
        return( 0 );
    }
    l = left;       // convert to float's
    r = right;
    t = top;
    b = bottom;
    if( l == r || t == b ) {
        _ErrorStatus = _GRINVALIDPARAMETER;
        return( 0 );
    }
    if( t < b ) {        // ensure bottom < top
        tmp = t;
        t = b;
        b = tmp;
    }
    _Window.xleft = l;
    _Window.xright = r;
    _Window.invert = invert;
    if( invert ) {
        _Window.ytop = t;
        _Window.ybottom = b;
    } else {
        _Window.ytop = b;
        _Window.ybottom = t;
    }
    _resetscalefactor();
    _moveto_w( l, _Window.ybottom );
    return( 1 );
}

Entry( _SETWINDOW, _setwindow ) // alternate entry-point


_WCRTLINK short _WCI86FAR _CGRAPH _setwindow_87( short invert, double left, double top,
/*===========================================*/ double right, double bottom )

{
    return( _setwindow( invert, left, top, right, bottom ) );
}

Entry( _SETWINDOW_87, _setwindow_87 ) // alternate entry-point
