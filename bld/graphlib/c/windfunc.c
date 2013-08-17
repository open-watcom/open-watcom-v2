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


static float            XScale1;
static float            XScale2;
static float            YScale1;
static float            YScale2;


short _WtoPhysX( double x )
/*=========================

   Map x from window coordinates to physical coordinates.   */

{
    float               phys_x;

    phys_x = ( (float) x - _Window.xleft ) * XScale1;
    phys_x += roundoff( phys_x );
    return( (short) phys_x + _CurrState->clip.xmin );
}


short _WtoPhysY( double y )
/*=========================

   Map y from window coordinates to physical coordinates.   */

{
    float               phys_y;

    if( _Window.invert ) {
        phys_y = ( _Window.ytop - (float) y ) * YScale1;
    } else {
        phys_y = ( (float) y - _Window.ytop ) * YScale1;
    }
    phys_y += roundoff( phys_y );
    return( (short) phys_y + _CurrState->clip.ymin );
}


short _WtoScaleX( double x )
/*==========================

   Return length of x in physical coordinates. */

{
    return( _CurrState->vc.numxpixels * ( (float) x ) * XScale2 );
}


short _WtoScaleY( double y )
/*==========================

   Return length of y in physical coordinates. */

{
    return( _CurrState->vc.numypixels * ( (float) y ) * YScale2 );
}


_WCRTLINK struct _wxycoord _WCI86FAR _CGRAPH _getwindowcoord( short x, short y )
/*===============================================================

   Map (x,y) from viewport coordinates to window coordinates.   */

{
    float               y1;
    struct _wxycoord    pt;

    if( _GraphMode() ) {
        pt.wx = _Window.xleft + ( _Window.xright - _Window.xleft ) *
                ( _VtoPhysX( x ) - _CurrState->clip.xmin ) /
                ( _CurrState->clip.xmax - _CurrState->clip.xmin );
        y1 = (float) ( _VtoPhysY( y ) - _CurrState->clip.ymin ) /
                     ( _CurrState->clip.ymax - _CurrState->clip.ymin );
        if( _Window.invert ) {
            pt.wy = _Window.ytop - y1 * ( _Window.ytop - _Window.ybottom );
        } else {
            pt.wy = y1 * ( _Window.ybottom - _Window.ytop ) + _Window.ytop;
        }
    } else {
        pt.wx = 0;
        pt.wy = 0;
    }
    return( pt );
}

Entry1( _GETWINDOWCOORD, _getwindowcoord ) // alternate entry-point


_WCRTLINK struct xycoord _WCI86FAR _CGRAPH _getviewcoord_w( double x, double y )
/*===============================================================

   Map (x,y) from window coordinates to viewport coordinates.   */

{
    struct xycoord      pt;

    if( _GraphMode() ) {
        pt.xcoord = _GetLogX( _WtoPhysX( x ) );
        pt.ycoord = _GetLogY( _WtoPhysY( y ) );
    } else {
        pt.xcoord = 0;
        pt.ycoord = 0;
    }
    return( pt );
}

Entry1( _GETVIEWCOORD_W, _getviewcoord_w ) // alternate entry-point


_WCRTLINK struct xycoord _WCI86FAR _CGRAPH _getviewcoord_w_87( double x, double y )
/*================================================================*/

{
    return( _getviewcoord_w( x, y ) );
}

Entry1( _GETVIEWCOORD_W_87, _getviewcoord_w_87 ) // alternate entry-point


_WCRTLINK struct xycoord _WCI86FAR _CGRAPH _getviewcoord_wxy( struct _wxycoord _WCI86FAR * pt )
/*=========================================================================

   Map (x,y) from window coordinates to viewport coordinates.   */

{
    return( _getviewcoord_w( pt->wx, pt->wy ) );
}

Entry1( _GETVIEWCOORD_WXY, _getviewcoord_wxy ) // alternate entry-point


_WCRTLINK struct _wxycoord _WCI86FAR _CGRAPH _getcurrentposition_w( void )
/*=========================================================

   This function returns the current output position in window coordinates. */

{
    return( _CurrPos_w );
}

Entry1( _GETCURRENTPOSITION_W, _getcurrentposition_w ) // alternate entry-point


void _resetscalefactor( void )
//======================

{
    float               x_range;
    float               y_range;

    x_range = _Window.xright - _Window.xleft;
    XScale1 = ( _CurrState->clip.xmax - _CurrState->clip.xmin ) / x_range;
    XScale2 = 1.0f / x_range;
    if( _Window.invert ) {
        y_range = _Window.ytop - _Window.ybottom;
    } else {
        y_range = _Window.ybottom - _Window.ytop;
    }
    YScale1 = ( _CurrState->clip.ymax - _CurrState->clip.ymin ) / y_range;
    YScale2 = 1.0f / y_range;
}
