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

short _WCI86FAR _CGRAPH _pie( short fill, short x1, short y1, short x2, short y2,
/*====================*/ short x3, short y3, short x4, short y4 )

{
    short               success;

    if( _GrProlog() ) {
        success = _L2pie( fill, _VtoPhysX( x1 ), _VtoPhysY( y1 ),
                                _VtoPhysX( x2 ), _VtoPhysY( y2 ),
                                _VtoPhysX( x3 ), _VtoPhysY( y3 ),
                                _VtoPhysX( x4 ), _VtoPhysY( y4 ) );
        _GrEpilog();
    } else {
        success = 0;
    }
    return( success );
}

Entry( _PIE, _pie ) // alternate entry-point


short _WCI86FAR _L2pie( short fill, short x1, short y1, short x2, short y2,
/*==============*/ short x3, short y3, short x4, short y4 )

{
#if defined( _DEFAULT_WINDOWS )
    short               clip1;
    short               clip2;
    WPI_PRES            dc;
    HBITMAP             bm;
    HBRUSH              brush;
    HBRUSH              old_brush;
    HPEN                pen;
    HPEN                old_pen;
    WPI_COLOUR          color;
#if defined( __WINDOWS__ )
    WPI_RECT            clip_rect;
    HRGN                temprgn;
    short               clipy1, clipy2;
    short               tmpx3, tmpy3, tmpx4, tmpy4;
    float               angle, cosval, sinval;
    short               a, b, xc, yc;
#endif

    clip1 = _L1OutCode( x1, y1 );       // check for clipping
    clip2 = _L1OutCode( x2, y2 );
    if( clip1 & clip2 ) {
        _ErrorStatus = _GRNOOUTPUT;
        return ( 0 );                         // trivially outside so quit
    }

    dc = _Mem_dc;
    color  = _Col2RGB( _CurrColor ) ;

#if defined( __WINDOWS__ )
// do the clipping
    temprgn = _ClipRgn;
    clipy1 = _wpi_cvth_y( _CurrState->clip_def.ymin, _GetPresHeight() );
    clipy2 = _wpi_cvth_y( _CurrState->clip_def.ymax + 1, _GetPresHeight() );
    _wpi_setintwrectvalues( &clip_rect,
                           _CurrState->clip_def.xmin,
                           clipy1,
                           _CurrState->clip_def.xmax + 1,
                           clipy2 );
    _ClipRgn = _wpi_createrectrgn( dc, &clip_rect );
    _wpi_getclipbox( dc, &clip_rect);
    _wpi_selectcliprgn( dc, _ClipRgn );
    _wpi_deletecliprgn( dc, temprgn );

    y1 = _wpi_cvth_y( y1, _GetPresHeight() );
    y2 = _wpi_cvth_y( y2, _GetPresHeight() );
    y3 = _wpi_cvth_y( y3, _GetPresHeight() );
    y4 = _wpi_cvth_y( y4, _GetPresHeight() );

    a = ( x2 - x1 + 1 ) / 2;
    b = ( y2 - y1 + 1 ) / 2;
    xc = x1 + a - 1;
    yc = y1 + b - 1;

    tmpx3 = x3 - xc;
    tmpy3 = y3 - yc;
    tmpx4 = x4 - xc;
    tmpy4 = y4 - yc;

    angle = atan2( tmpy3, tmpx3 );

//  sinval = sin( angle );
    sinval = angle;
    _GR_sin( &sinval );
//  cosval = cos( angle );
    cosval = angle;
    _GR_cos( &cosval );

    tmpx3 = xc + a * cosval;
    tmpy3 = yc + b * sinval;

    angle = atan2( tmpy4, tmpx4 );

//  sinval = sin( angle );
    sinval = angle;
    _GR_sin( &sinval );
//  cosval = cos( angle );
    cosval = angle;
    _GR_cos( &cosval );

    tmpx4 = xc + a * cosval;
    tmpy4 = yc + b * sinval;

    if( ( tmpx3 == tmpx4 ) && ( tmpy3 == tmpy4 ) ) {
        fill = _GBORDER;
    }
#else
    if( fill == _GBORDER ) {
        fill = _GPIEBORDER;
    }
#endif

// setup before drawing
// Check for a fillmask
    if( fill == _GFILLINTERIOR ) {
        pen = _wpi_createpen( PS_NULL, 0, color );
        if( _HaveMask == 0 ) {
            brush = _wpi_createsolidbrush( color );
        } else {
            // if a mask is defined, convert it to bitmap
            bm = _Mask2Bitmap( dc, &_FillMask );
            brush = _wpi_createpatternbrush( bm );
        }
    } else {
        pen = _MyCreatePen( color );
        brush = _wpi_createnullbrush();
    }

    old_pen = _wpi_selectpen( dc, pen );
    old_brush = _wpi_selectbrush( dc, brush );

#if defined( __WINDOWS__ )
// We want the same behaviour as dos in windows
    if( ( tmpx3 == tmpx4 ) && ( tmpy3 == tmpy4 ) ) {
        MoveTo( dc, xc, yc );
        LineTo( dc, tmpx3, tmpy3 );
    } else {
        _wpi_pie( dc, x1, y1, x2, y2, x3, y3, x4, y4 );
    }
#else
    _L1Arc( fill, x1, y1, x2, y2, x3, y3, x4, y4 );
#endif
// Clean up after drawing
    _wpi_getoldbrush( dc, old_brush );
    if( fill == _GFILLINTERIOR ) {
        _wpi_deletebrush( brush );
    }

    _wpi_getoldpen( dc, old_pen );
    _wpi_deletepen( pen );

    if( _HaveMask != 0 ) {
        _wpi_deletebitmap( bm );
    }
#if defined( __WINDOWS__ )
    temprgn = _ClipRgn;
    _ClipRgn = _wpi_createrectrgn( dc, &clip_rect );
    _wpi_selectcliprgn( dc, _ClipRgn );
    _wpi_deletecliprgn( dc, temprgn );
#endif

// Update the window
    _MyInvalidate( x1, y1, x2, y2 );
    _RefreshWindow();
#else
    _L1Arc( fill, x1, y1, x2, y2, x3, y3, x4, y4 );
    if( fill != _GFILLINTERIOR ) {      // border only
        if( _ErrorStatus == _GROK || _ErrorStatus == _GRCLIPPED ) {
            _L2line( _ArcInfo.centre.xcoord, _ArcInfo.centre.ycoord,
                     _ArcInfo.start.xcoord, _ArcInfo.start.ycoord );
            _L2line( _ArcInfo.centre.xcoord, _ArcInfo.centre.ycoord,
                     _ArcInfo.end.xcoord, _ArcInfo.end.ycoord );
        }
    }
#endif
    return( _ErrorStatus == _GROK );
}
