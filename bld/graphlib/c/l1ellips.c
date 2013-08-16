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
* Description:  High level ellipse drawing routines.
*
****************************************************************************/


#include "gdefn.h"
#include "ellinfo.h"
#include "rotate.h"


#define _is_odd( q )    ( ( q & 1 ) != 0 )
#define _is_even( q )   ( ( q & 1 ) == 0 )


#if defined( _DEFAULT_WINDOWS )

extern float            sqrtf( float );
#pragma aux             sqrtf "*_";


static struct xycoord   Cal_Coord( float, float, float, float );

#else

static struct line_entry    VectA;
static struct line_entry    VectB;
static struct ellipse_info  EllInfo;


static void             FastPlot( short x, short y, short q );
static void             SlowPlot( short x, short y, short q );
static void             ArcPlot( short x, short y, short q );
static void             EllFill( short x, short y, short q );
static void             ArcFill( short x, short y, short q );
static void             InitLineMasks( short x1, short y1, short x2, short y2 );
static void             HollowArc( short x1, short y1, short x2, short y2, short a, short b );
static void             FilledArc( short x1, short y1, short x2, short y2, short a, short b );
#endif


void _L1Ellipse( short fill, short x1, short y1, short x2, short y2 )
//===================================================================

{
    short               clip1;
    short               clip2;
#if defined( _DEFAULT_WINDOWS )
    WPI_PRES            dc;
    HBITMAP             bm;
    HBRUSH              brush;
    HBRUSH              old_brush;
    HPEN                pen;
    HPEN                old_pen;
    HRGN                temprgn;
    WPI_COLOUR          color;
    WPI_RECT            clip_rect;
    short               t;
    short               clipy1, clipy2;
#endif

    clip1 = _L1OutCode( x1, y1 );       // check for clipping
    clip2 = _L1OutCode( x2, y2 );
    if( clip1 & clip2 ) {
        _ErrorStatus = _GRNOOUTPUT;
        return;                         // trivially outside so quit
    }
#if defined( _DEFAULT_WINDOWS )
    dc = _Mem_dc;

// Do the clippings
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

// Set up before drawing
    color = _Col2RGB( _CurrColor );

    if( fill == _GFILLINTERIOR ) {
        // there is not border for filled objects, so set the pen to null
        pen = _wpi_createpen( PS_NULL, 0, color );

        // Check if there is a fill mask
        if( _HaveMask == 0 ) {
            brush = _wpi_createsolidbrush( color );
        } else {
            // if a mask is defined, convert it to bitmap
            bm = _Mask2Bitmap( dc, &_FillMask );
            brush = _wpi_createpatternbrush( bm );
        }

    } else {
        // Just draw the frame
        pen = _MyCreatePen( color );

        // make sure the background can still be seen
        brush = _wpi_createnullbrush();
    }

    old_pen = _wpi_selectpen( dc, pen );
    old_brush = _wpi_selectbrush( dc, brush );
    if( y1 > y2 ){
        t = y1;
        y1 = y2;
        y2 = t;
    }
    y1 = _wpi_cvth_y( y1, _GetPresHeight() );
    y2 = _wpi_cvth_y( y2, _GetPresHeight() );
#if defined( __OS2__ )
    _wpi_ellipse( dc, x1, y1-1, x2, y2 );
#else
    _wpi_ellipse( dc, x1, y1, x2, y2 );
#endif

// Clean up afterwards
    _wpi_getoldbrush( dc, old_brush );
    if( fill == _GFILLINTERIOR ){
        _wpi_deletebrush( brush );
    } else {
        _wpi_deletenullbrush( brush );
    }

    if( _HaveMask != 0 ) {
        _wpi_deletebitmap( bm );
    }

    _wpi_getoldpen( dc, old_pen );
    _wpi_deletepen( pen );


    temprgn = _ClipRgn;
    _ClipRgn = _wpi_createrectrgn( dc, &clip_rect );
    _wpi_selectcliprgn( dc, _ClipRgn );
    _wpi_deletecliprgn( dc, temprgn );

// Make sure we update the window
    _MyInvalidate( x1, y1, x2, y2 );
    _RefreshWindow();
#else
    _StartDevice();
    EllInfo.x_reflect = x1 + x2;        // need to reflect to use symmetry
    EllInfo.y_reflect = y1 + y2;
    if( fill == _GFILLINTERIOR ) {
        EllInfo.prev_y = -1;            // no previous line
        _L0Ellipse( x1, y1, x2, y2, EllFill );
    } else {
        if( clip1 == clip2 && _LineStyle == SOLID_LINE ) {
            // no clipping and solid line
            _L0Ellipse( x1, y1, x2, y2, FastPlot );
        } else {
            InitLineMasks( x1, y1, x2, y2 );
            _L0Ellipse( x1, y1, x2, y2, SlowPlot );
        }
    }
    _ResetDevice();
#endif
}


#if !defined( _DEFAULT_WINDOWS )

static void FastPlot( short x, short y, short q )
//===============================================

// plot the ellipse point (x,y)

{
    void                (*setup )( short, short, grcolor );
    putdot_fn           *putdot;
    gr_device _FARD     *dev_ptr;

    dev_ptr = _CurrState->deviceptr;
    setup = dev_ptr->setup;
    putdot = dev_ptr->plot[ _PlotAct ];
    if( q == 4 ) {  // all quadrants
        ( *setup )( x,                     y,                     _CurrColor );
        ( *putdot )( _Screen.mem, _Screen.colour, _Screen.mask );
        ( *setup )( EllInfo.x_reflect - x, y,                     _CurrColor );
        ( *putdot )( _Screen.mem, _Screen.colour, _Screen.mask );
        ( *setup )( EllInfo.x_reflect - x, EllInfo.y_reflect - y, _CurrColor );
        ( *putdot )( _Screen.mem, _Screen.colour, _Screen.mask );
        ( *setup )( x,                     EllInfo.y_reflect - y, _CurrColor );
        ( *putdot )( _Screen.mem, _Screen.colour, _Screen.mask );
    } else {
        if( q == 1 || q == 2 ) {
            x = EllInfo.x_reflect - x;
        }
        if( q == 2 || q == 3 ) {
            y = EllInfo.y_reflect - y;
        }
        ( *setup )( x, y, _CurrColor );
        ( *putdot )( _Screen.mem, _Screen.colour, _Screen.mask );
    }
}


static void PutDot( int x, int y, int q )
//=======================================

{
    void                (*setup )( short, short, grcolor );
    putdot_fn           *putdot;
    gr_device _FARD     *dev_ptr;

    if( _L1OutCode( x, y ) == 0 ) {     // inside viewport
        if( EllInfo.line_mask[ q ] & 1 ) {  // check for line style
            dev_ptr = _CurrState->deviceptr;
            setup = dev_ptr->setup;
            putdot = dev_ptr->plot[ _PlotAct ];
            ( *setup )( x, y, _CurrColor );
            ( *putdot )( _Screen.mem, _Screen.colour, _Screen.mask );
        }
    }
}


static void SlowPlot( short x, short y, short q )
//===============================================

// plot the ellipse point (x,y) taking into account the line style
// and the clipping region

{
    short               inc;
    short               x1;
    short               y1;

    if( q == 4 ) {  // do all the quadrants
        q = 0;      // start with 0
        inc = 1;
    } else {
        inc = 4;
    }
    for( ; q <= 3; q += inc ) {
        if( q == 1 || q == 2 ) {
            x1 = EllInfo.x_reflect - x;
        } else {
            x1 = x;
        }
        if( q == 2 || q == 3 ) {
            y1 = EllInfo.y_reflect - y;
        } else {
            y1 = y;
        }
        if( q == 0 || q == 2 ) {
            EllInfo.line_mask[ q ] = _wrol( EllInfo.line_mask[ q ], 1 );
        } else {
            EllInfo.line_mask[ q ] = _wror( EllInfo.line_mask[ q ], 1 );
        }
        PutDot( x1, y1, q );
    }
}


#define NumPts  EllInfo.prev_y
#define XAxis   EllInfo.x_reflect
#define YAxis   EllInfo.y_reflect


static void CountPlot( short x, short y, short q )
//================================================

{
    x = x;
    y = y;
    if( q == 4 ) {      // count points drawn in each quadrant
        ++NumPts;
    } else if( q == 0 ) {
        XAxis = 1;
    } else if( q == 1 ) {
        YAxis = 1;
    }
}


static void DummyPlot( int x, int y, int q )
//==========================================

{
    x = x;
    y = y;
    q = q;
}


static void InitLineMasks( short x1, short y1, short x2, short y2 )
//=================================================================

// To adjust the line style mask properly, we need to know how
// many points will be drawn by the ellipse drawing algorithm.
// To determine this, call the algorithm with a dummy routine that
// counts the points.

{
    short               amount;

    NumPts = 0;
    _L0Ellipse( x1, y1, x2, y2, CountPlot );    // count the points
    amount = 2*NumPts + XAxis + YAxis;
    EllInfo.line_mask[ 0 ] = _LineStyle;
    EllInfo.line_mask[ 1 ] = _wrol( _LineStyle, ( amount + 1 ) & 0x0f );
    EllInfo.line_mask[ 2 ] = _wrol( _LineStyle, ( amount ) & 0x0f );
    EllInfo.line_mask[ 3 ] = _wrol( _LineStyle, ( 2 * amount + 1 ) & 0x0f );
    EllInfo.x_reflect = x1 + x2;    // need to be reset
    EllInfo.y_reflect = y1 + y2;
}


static void EllFill( short x, short y, short q )
//==============================================

// given the ellipse point (x,y), fill the ellipse

{
    short               xl;
    short               y2;

    if( q == 0 || q == 4 ) {    // only plot if quadrant 0 or all
        if( EllInfo.prev_y != y ) {     // and only if y has changed
            xl = EllInfo.x_reflect - x;  // reflected point is x1 + ( x2 - x )
            _L1ClipFill( xl, x, y );
            y2 = EllInfo.y_reflect - y;
            if( y2 != y ) {
                _L1ClipFill( xl, x, y2 );
            }
            EllInfo.prev_y = y;
        }
    }
}


static short Quadrant( short x, short y, short x_width, short y_width )
//=====================================================================

// return the quadrant (0..3) containing the point (x,y)
// Note: x and y cannot both be 0
// A point on the x-axis will be in either quadrant 0 or 3 depending
// on the width of the x_axis
{
    if( x == 0 ) {          // check for y_axis
        if( y > 0 ) {
            if( y_width == 0 ) {
                return( 0 );
            } else {
                return( 1 );
            }
        } else {    // y < 0
            return( 3 );
        }
    } else if( y == 0 ) {   // check for x_axis
        if( x > 0 ) {
            if( x_width == 0 ) {
                return( 3 );
            } else {
                return( 0 );
            }
        } else {    // x < 0
            return( 2 );
        }
    } else if( x > 0 ) {
        if( y > 0 ) {
            return( 0 );
        } else {
            return( 3 );
        }
    } else {    // x < 0
        if( y > 0 ) {
            return( 1 );
        } else {
            return( 2 );
        }
    }
}

#endif

#if defined( _DEFAULT_WINDOWS )
#if defined( __OS2__ )
static float GetAngle( short x, short y, short xc, short yc, short a, short b )
//===========================================================
// return the angle between the line from (xc, yc) to (x, y) and the x
// axis.

{
    float  angle;
    float  sinval, cosval;
    float  pie = 3.141592654;

    x -= xc;
    y -= yc;

    sinval = (float)y / (float)b;
    if( sinval > 1 ) {
        sinval = 1;
    } else if( sinval < -1 ) {
        sinval = -1;
    }

    cosval = (float)x / (float)a;
    if( cosval > 1 ) {
        cosval = 1;
    } else if( cosval < -1 ) {
        cosval = -1;
    }

    if( a >= b ) {
        angle = asin( sinval );
        if( x < 0 ) {
            if( sinval > 0 ) {
                angle = pie - angle;
            } else if( sinval < 0 ) {
                angle = -( pie + angle);
            } else if( sinval == 0 ) {
                angle = pie;
            }
        }
    } else {
        angle = acos( cosval );
        if( y < 0 ) {
            angle = -angle;
        }
    }
    if( angle < 0 ) {
        angle += ( 2 * pie);
    }
    return angle * 180.0 / pie;
}
#endif
static int round( float num )
//==================================================
// This function round off a float in to an interger.
{
    short       sign;
    int         floor;

    if( num < 0 ) {
        sign = -1;
        num = -num;
    } else {
        sign = 1;
    }

    floor = (int)num;
    if( num - floor < 0.5 ) {
        return sign * floor;
    } else {
        return sign * ( floor + 1 );
    }
}

#endif

void _L1Arc( short fill, short x1, short y1, short x2, short y2,
/*====================*/ short x3, short y3, short x4, short y4 )

/* This function draws an elliptical arc.  The virtual ellipse is defined by
   the rectangle whose opposite corners are ( x1, y1 ) and ( x2, y2 ). The
   intersections of the vectors from the center of the ellipse to the points
   ( x3, y3 ) and ( x4, y4 ) define the start and end points respectively
   where the arc is drawn in a counter clockwise direction. All values
   are in physical coordinates. */

{
    short               clip1;
    short               clip2;
    short               a, b;
    short               t;
#if defined( _DEFAULT_WINDOWS )
    WPI_PRES            dc;
    HPEN                pen;
    HPEN                old_pen;
    HRGN                temprgn;
    WPI_COLOUR          color;
    WPI_RECT            clip_rect;
    short               clipy1, clipy2;
#endif

    clip1 = _L1OutCode( x1, y1 );       // check for clipping
    clip2 = _L1OutCode( x2, y2 );
    if( clip1 & clip2 ) {
        _ErrorStatus = _GRNOOUTPUT;
        return;                         // trivially outside so quit
    }
    if( x1 > x2 ) {         // ensure x1 < x2
        t = x1;
        x1 = x2;
        x2 = t;
    }
    if( y1 > y2 ) {         // ensure y1 < y2
        t = y1;
        y1 = y2;
        y2 = t;
    }
    a = ( x2 - x1 + 1 ) / 2 - 1;
    b = ( y2 - y1 + 1 ) / 2 - 1;
    _ArcInfo.centre.xcoord = x1 + a + 1;       // centre of ellipse
    _ArcInfo.centre.ycoord = y1 + b + 1;
    _ArcInfo.vecta.xcoord = x3 - _ArcInfo.centre.xcoord;
    _ArcInfo.vecta.ycoord = _ArcInfo.centre.ycoord - y3;        // invert y
    _ArcInfo.vectb.xcoord = x4 - _ArcInfo.centre.xcoord;
    _ArcInfo.vectb.ycoord = _ArcInfo.centre.ycoord - y4;        // invert y
    if( _ArcInfo.vecta.xcoord == 0 && _ArcInfo.vecta.ycoord == 0 ||
                _ArcInfo.vectb.xcoord == 0 && _ArcInfo.vectb.ycoord == 0 ) {
        _ErrorStatus = _GRINVALIDPARAMETER;     // zero vector
        return;
    }

#if defined( _DEFAULT_WINDOWS )
    // Calculate end points
    _ArcInfo.start = Cal_Coord( x3, y3, a, b );
    _ArcInfo.end = Cal_Coord( x4, y4, a, b );

    fill = fill;        // used to get rid of warnings in windows
    dc = _Mem_dc;

// do the clipping

    temprgn = _ClipRgn;
    clipy1 = _wpi_cvth_y( _CurrState->clip_def.ymin, _GetPresHeight() );
    clipy2 = _wpi_cvth_y( _CurrState->clip_def.ymax + 1, _GetPresHeight() );
    _wpi_setintwrectvalues( &clip_rect, _CurrState->clip_def.xmin, clipy1,
                           _CurrState->clip_def.xmax + 1, clipy2 );
    _ClipRgn = _wpi_createrectrgn( dc, &clip_rect );
    _wpi_getclipbox( dc, &clip_rect);
    _wpi_selectcliprgn( dc, _ClipRgn );
    _wpi_deletecliprgn( dc, temprgn );

// setup
// Convert our color to RGB values
    color = _Col2RGB( _CurrColor );

// Map the line styles
#if defined( __OS2__ )
    if( fill != _GFILLINTERIOR ) {
#endif
        pen = _MyCreatePen( color );
        old_pen = _wpi_selectpen( dc, pen );
#if defined( __OS2__ )
    }

    OS2_Arc( dc, fill, a, b );
#else
    _wpi_arc( dc, x1, y1, x2, y2, x3, y3, x4, y4 );
#endif

//  Clean up
#if defined( __OS2__ )
    if( fill != _GFILLINTERIOR ) {
#endif
        _wpi_getoldpen( dc, old_pen );
        _wpi_deletepen( pen );
#if defined( __OS2__ )
    }
#endif

    temprgn = _ClipRgn;
    _ClipRgn = _wpi_createrectrgn( dc, &clip_rect );
    _wpi_selectcliprgn( dc, _ClipRgn );
    _wpi_deletecliprgn( dc, temprgn );

    y1 = _wpi_cvth_y( y1, _GetPresHeight() );
    y2 = _wpi_cvth_y( y2, _GetPresHeight() );

//  Update the window
    _MyInvalidate( x1, y1, x2, y2 );
    _RefreshWindow();
#else

    _StartDevice();
    EllInfo.x_reflect = x1 + x2;        // need to reflect to use symmetry
    EllInfo.y_reflect = y1 + y2;
    if( fill == _GFILLINTERIOR ) {
        FilledArc( x1, y1, x2, y2, a, b );
    } else {
        if( _LineStyle == SOLID_LINE ) {
            EllInfo.line_mask[ 0 ] = _LineStyle;
            EllInfo.line_mask[ 1 ] = _LineStyle;
            EllInfo.line_mask[ 2 ] = _LineStyle;
            EllInfo.line_mask[ 3 ] = _LineStyle;
        } else {
            InitLineMasks( x1, y1, x2, y2 );
        }
        _ArcInfo.plot = PutDot;
        HollowArc( x1, y1, x2, y2, a, b );
    }
    _ResetDevice();
#endif
}


#if defined( _DEFAULT_WINDOWS )

#if defined( __OS2__ )

static void OS2_Arc( HPS pres, short fill, short a, short b )
/*===========================================================

  This function is used to draw an Arc in OS/2. */

{
    ARCPARAMS           arcp;
    POINTL              pts, pte, ptc;
    float               start, end, sweep;

// Use the centre and the two end points to get an intermediate point
    pts.x = _ArcInfo.start.xcoord;
    pts.y = _wpi_cvth_y( _ArcInfo.start.ycoord, _GetPresHeight() );
    pte.x = _ArcInfo.end.xcoord;
    pte.y = _wpi_cvth_y( _ArcInfo.end.ycoord, _GetPresHeight() );
    ptc.x = _ArcInfo.centre.xcoord;
    ptc.y = _wpi_cvth_y( _ArcInfo.centre.ycoord, _GetPresHeight() );

    start = GetAngle( pts.x, pts.y, ptc.x, ptc.y, a, b );
    end = GetAngle( pte.x, pte.y, ptc.x, ptc.y, a, b );

// Find an intermediate point
    sweep = end - start;
    if( sweep < 0 ) {
        sweep += 360;
    }

// Set the size and orientation of the arc.
    arcp.lP = a;
    arcp.lQ = b;
    arcp.lR = 0;
    arcp.lS = 0;
    GpiSetArcParams( pres, &arcp );
    GpiSetDefArcParams( pres, &arcp );

// Draw the arc
    if( ( fill == _GFILLINTERIOR ) || ( fill == _GPIEBORDER ) ) {
        GpiSetCurrentPosition( pres, &ptc );
        if( fill == _GFILLINTERIOR ) {
            GpiBeginArea( pres, BA_BOUNDARY | BA_ALTERNATE );
        }
        GpiPartialArc( pres, &ptc, MAKEFIXED( 1, 0 ),
                       MAKEFIXED( round( start ), 0 ),
                       MAKEFIXED( round( sweep ), 0 ) );
        GpiLine( pres, &ptc );
        if( fill == _GFILLINTERIOR ) {
            GpiEndArea( pres );
        }
    } else {
        GpiSetCurrentPosition( pres, &pts );
        GpiPartialArc( pres, &ptc, MAKEFIXED( 1, 0 ),
                       MAKEFIXED( round( start ), 0 ),
                       MAKEFIXED( round( sweep ), 0 ) );
    }
}

#endif

static struct xycoord Cal_Coord( float x, float y, float a, float b )
/*===================================================================
   This function calculates the intesection coordinates of a vector, and
   an ellipse. */
{
    struct xycoord      point;
    float               delta, gamma;

    x = x - _ArcInfo.centre.xcoord;
    y = y - _ArcInfo.centre.ycoord;

    if( ( x == 0 ) && ( y == 0 ) ) {
        return _ArcInfo.centre;
    }
    if( x != 0 ) {
        delta = atan2( b * x,  a * y );
        gamma = delta;
        delta = sin( delta );
        gamma = cos( gamma );
        point.xcoord = round( a * delta ) + _ArcInfo.centre.xcoord;
        point.ycoord = round( b * gamma ) + _ArcInfo.centre.ycoord;
    } else {
        point.xcoord = _ArcInfo.centre.xcoord;
        if( y > 0 ) {
            point.ycoord = b + _ArcInfo.centre.ycoord;
        } else {
            point.ycoord = -b + _ArcInfo.centre.ycoord;
        }
    }
    return( point );
}

#else
static void HollowArc( short x1, short y1, short x2, short y2,
                                           short a, short b )
//============================================================

// set up to draw a hollow arc

{
    short               q;
    short               q1;
    short               q2;
    short               x_axis_width;
    short               y_axis_width;
    long                xprod;

    for( q = 0; q <= 3; ++q ) {     // assume no drawing
        _ArcInfo.qinf[ q ] = ARC_EMPTY;
    }
    x_axis_width = y2 - y1 - 2*b - 1;   // either 0 or 1
    y_axis_width = x2 - x1 - 2*a - 1;
    q1 = Quadrant( _ArcInfo.vecta.xcoord, _ArcInfo.vecta.ycoord,
                    x_axis_width, y_axis_width );
    q2 = Quadrant( _ArcInfo.vectb.xcoord, _ArcInfo.vectb.ycoord,
                    x_axis_width, y_axis_width );
    if( q1 == q2 ) {    // check cross product
        xprod = (long) _ArcInfo.vecta.xcoord * _ArcInfo.vectb.ycoord -
                (long) _ArcInfo.vectb.xcoord * _ArcInfo.vecta.ycoord;
        if( xprod == 0 ) {
            _ArcInfo.qinf[ q1 ] = ARC_LINE;     // single point
        } else if( xprod > 0 ) {
            _ArcInfo.qinf[ q1 ] = ARC_BOTH_IN;
        } else {
            _ArcInfo.qinf[ q1 ] = ARC_BOTH_OUT;
            for( q = 0; q <= 3; ++q ) {     // fully draw rest of quadrants
                if( q != q1 ) {
                    _ArcInfo.qinf[ q ] = ARC_FULL;
                }
            }
        }
    } else {
        _ArcInfo.qinf[ q1 ] = ARC_VECT_A;
        _ArcInfo.qinf[ q2 ] = ARC_VECT_B;
        for( q = ( q1 + 1 ) % 4; q != q2; q = ( q + 1 ) % 4 ) {
            _ArcInfo.qinf[ q ] = ARC_FULL;
        }
    }

    _ArcInfo.start.xcoord = -1;           // indicate not started drawing yet
    _ArcInfo.end.xcoord = -1;           // indicate not started drawing yet
    _L0Ellipse( x1, y1, x2, y2, ArcPlot );
    for( q = 0; q <= 3; ++q ) {
        if( _ArcInfo.qinf[ q ] == ARC_LINE ) {  // plot was delayed
            _ArcInfo.end.xcoord = _ArcInfo.start.xcoord;
            _ArcInfo.end.ycoord = _ArcInfo.start.ycoord;
            (*_ArcInfo.plot)( _ArcInfo.start.xcoord, _ArcInfo.start.ycoord, q );
        }
    }
}


static void ArcPlot( short x, short y, short q )
//==============================================

// plot the ellipse point (x,y) if it is on the arc.
// if vector a crosses the current quadrant check for the start point
// if vector b crosses the current quadrant remember the last point drawn

{
    short               inc;
    short               x1;
    short               y1;
    long                v_cross_a;
    long                v_cross_b;

    if( q == 4 ) {  // do all the quadrants
        q = 0;      // start with 0
        inc = 1;
    } else {
        inc = 4;
    }
    for( ; q <= 3; q += inc ) {
        if( q == 1 || q == 2 ) {
            x1 = EllInfo.x_reflect - x;
        } else {
            x1 = x;
        }
        if( q == 2 || q == 3 ) {
            y1 = EllInfo.y_reflect - y;
        } else {
            y1 = y;
        }
        if( q == 0 || q == 2 ) {
            EllInfo.line_mask[ q ] = _wrol( EllInfo.line_mask[ q ], 1 );
        } else {
            EllInfo.line_mask[ q ] = _wror( EllInfo.line_mask[ q ], 1 );
        }

        switch( _ArcInfo.qinf[ q ] ) {
        case ARC_EMPTY:
            break;
        case ARC_FULL:
            (*_ArcInfo.plot)( x1, y1, q );
            break;
        case ARC_VECT_A:    // plot if V cross A <= 0
            v_cross_a = (long) ( x1 - _ArcInfo.centre.xcoord ) * _ArcInfo.vecta.ycoord
                      - (long) ( _ArcInfo.centre.ycoord - y1 ) * _ArcInfo.vecta.xcoord;
            if( v_cross_a <= 0 ) {
                (*_ArcInfo.plot)( x1, y1, q );
                if( _ArcInfo.start.xcoord == -1 || _is_odd( q ) ) {
                    _ArcInfo.start.xcoord = x1;     // remember 1st point
                    _ArcInfo.start.ycoord = y1;
                }
            }
            break;
        case ARC_VECT_B:    // plot if V cross B >= 0
            v_cross_b = (long) ( x1 - _ArcInfo.centre.xcoord ) * _ArcInfo.vectb.ycoord
                      - (long) ( _ArcInfo.centre.ycoord - y1 ) * _ArcInfo.vectb.xcoord;
            if( v_cross_b >= 0 ) {
                (*_ArcInfo.plot)( x1, y1, q );
                if( _ArcInfo.end.xcoord == -1 || _is_even( q ) ) {
                    _ArcInfo.end.xcoord = x1;       // remember last point
                    _ArcInfo.end.ycoord = y1;
                }
            }
            break;
        case ARC_BOTH_IN:   // plot if V cross A <= 0 and V cross B >= 0
            v_cross_a = (long) ( x1 - _ArcInfo.centre.xcoord ) * _ArcInfo.vecta.ycoord
                      - (long) ( _ArcInfo.centre.ycoord - y1 ) * _ArcInfo.vecta.xcoord;
            v_cross_b = (long) ( x1 - _ArcInfo.centre.xcoord ) * _ArcInfo.vectb.ycoord
                      - (long) ( _ArcInfo.centre.ycoord - y1 ) * _ArcInfo.vectb.xcoord;
            if( v_cross_a <= 0 && v_cross_b >= 0 ) {
                (*_ArcInfo.plot)( x1, y1, q );
                if( _ArcInfo.start.xcoord == -1 || _is_odd( q ) ) {
                    _ArcInfo.start.xcoord = x1;     // remember 1st point
                    _ArcInfo.start.ycoord = y1;
                }
                if( _ArcInfo.end.xcoord == -1 || _is_even( q ) ) {
                    _ArcInfo.end.xcoord = x1;       // remember last point
                    _ArcInfo.end.ycoord = y1;
                }
            }
            break;
        case ARC_BOTH_OUT:  // plot if V cross A <= 0 or V cross B >= 0
            v_cross_a = (long) ( x1 - _ArcInfo.centre.xcoord ) * _ArcInfo.vecta.ycoord
                      - (long) ( _ArcInfo.centre.ycoord - y1 ) * _ArcInfo.vecta.xcoord;
            v_cross_b = (long) ( x1 - _ArcInfo.centre.xcoord ) * _ArcInfo.vectb.ycoord
                      - (long) ( _ArcInfo.centre.ycoord - y1 ) * _ArcInfo.vectb.xcoord;
            if( v_cross_a <= 0 ) {
                (*_ArcInfo.plot)( x1, y1, q );
                if( _ArcInfo.start.xcoord == -1 || _is_odd( q ) ) {
                    _ArcInfo.start.xcoord = x1;     // remember 1st point
                    _ArcInfo.start.ycoord = y1;
                }
            }
            if( v_cross_b >= 0 ) {
                (*_ArcInfo.plot)( x1, y1, q );
                if( _ArcInfo.end.xcoord == -1 || _is_even( q ) ) {
                    _ArcInfo.end.xcoord = x1;       // remember last point
                    _ArcInfo.end.ycoord = y1;
                }
            }
            break;
        case ARC_LINE:  // plot single point if V cross A <= 0
            v_cross_a = (long) ( x1 - _ArcInfo.centre.xcoord ) * _ArcInfo.vecta.ycoord
                      - (long) ( _ArcInfo.centre.ycoord - y1 ) * _ArcInfo.vecta.xcoord;
            if( v_cross_a <= 0 ) {
// delay plot   (*_ArcInfo.plot)( x1, y1, q );
                if( _ArcInfo.start.xcoord == -1 || _is_odd( q ) ) {
                    _ArcInfo.start.xcoord = x1;     // remember 1st point
                    _ArcInfo.start.ycoord = y1;
                }
                if( _ArcInfo.end.xcoord == -1 || _is_even( q ) ) {
                    _ArcInfo.end.xcoord = x1;       // remember last point
                    _ArcInfo.end.ycoord = y1;
                }
            }
        }
    }
}


static void FilledArc( short x1, short y1, short x2, short y2,
                                           short a, short b )
//============================================================

// Set up to draw a filled arc. First call the HollowArc routine
// to determine the points where the lines intersect the arc.
// Then use these values for the vectors A and B. (Otherwise
// there is too much roundoff error when calculating the line
// coordinates.)

{
    short               q1;
    short               q2;
    short               xl;
    short               xr;

    // pretend to draw the arc, so that we know the start and end point
    _ArcInfo.plot = DummyPlot;
    HollowArc( x1, y1, x2, y2, a, b );
    // reset vectors a and b to their exact values
    _ArcInfo.vecta.xcoord = _ArcInfo.start.xcoord - _ArcInfo.centre.xcoord;
    _ArcInfo.vecta.ycoord = _ArcInfo.centre.ycoord - _ArcInfo.start.ycoord;
    _ArcInfo.vectb.xcoord = _ArcInfo.end.xcoord - _ArcInfo.centre.xcoord;
    _ArcInfo.vectb.ycoord = _ArcInfo.centre.ycoord - _ArcInfo.end.ycoord;

    _ArcInfo.qinf[ 0 ] = ARC_EMPTY;     // assume top and bottom halves empty
    _ArcInfo.qinf[ 2 ] = ARC_EMPTY;
    if( _ArcInfo.vecta.ycoord > 0 ) {
        q1 = 0;     // top half
    } else if( _ArcInfo.vecta.ycoord == 0 ) {
        q1 = 1;     // on centre line
    } else {
        q1 = 2;     // bottom half
    }
    if( _ArcInfo.vectb.ycoord > 0 ) {
        q2 = 0;
    } else if( _ArcInfo.vectb.ycoord == 0 ) {
        q2 = 1;
    } else {
        q2 = 2;
    }
    if( q1 == 1 && q2 == 1 ) {      // both on axis
        if( _ArcInfo.vecta.xcoord > 0 && _ArcInfo.vectb.xcoord < 0 ) {
            _ArcInfo.qinf[ 0 ] = ARC_FULL;
        } else if( _ArcInfo.vecta.xcoord < 0 && _ArcInfo.vectb.xcoord > 0 ) {
            _ArcInfo.qinf[ 2 ] = ARC_FULL;
        }
    } else {
        if( q1 == q2 ) {    // check cross product
            if( (long) _ArcInfo.vecta.xcoord * _ArcInfo.vectb.ycoord -
                    (long) _ArcInfo.vectb.xcoord * _ArcInfo.vecta.ycoord >= 0 ) {
                _ArcInfo.qinf[ q1 ] = ARC_BOTH_IN;
            } else {
                _ArcInfo.qinf[ q1 ] = ARC_BOTH_OUT;
                _ArcInfo.qinf[ 2 - q1 ] = ARC_FULL;
            }
        } else {
            _ArcInfo.qinf[ q1 ] = ARC_VECT_A;
            _ArcInfo.qinf[ q2 ] = ARC_VECT_B;
            if( q1 == 1 ) {     // vector a is on x-axis
                if( q2 == 2 && _ArcInfo.vecta.xcoord > 0 ) {
                    _ArcInfo.qinf[ 0 ] = ARC_FULL;
                } else if( q2 == 0 && _ArcInfo.vecta.xcoord < 0 ) {
                    _ArcInfo.qinf[ 2 ] = ARC_FULL;
                }
            } else if( q2 == 1 ) {
                if( q1 == 0 && _ArcInfo.vectb.xcoord > 0 ) {
                    _ArcInfo.qinf[ 2 ] = ARC_FULL;
                } else if( q1 == 2 && _ArcInfo.vectb.xcoord < 0 ) {
                    _ArcInfo.qinf[ 0 ] = ARC_FULL;
                }
            }
        }
    }
    // initialize line segments, and draw centre line
    if( q1 != 1 ) {
        _LineInit( _ArcInfo.centre.xcoord, _ArcInfo.centre.ycoord,
                   _ArcInfo.vecta.xcoord + _ArcInfo.centre.xcoord,
                   _ArcInfo.vecta.ycoord + _ArcInfo.centre.ycoord, &VectA );
    }
    if( q2 != 1 ) {
        _LineInit( _ArcInfo.centre.xcoord, _ArcInfo.centre.ycoord,
                   _ArcInfo.vectb.xcoord + _ArcInfo.centre.xcoord,
                   _ArcInfo.vectb.ycoord + _ArcInfo.centre.ycoord, &VectB );
    }
    // calculate extents of x on the axis line
    if( _ArcInfo.qinf[ 0 ] == ARC_FULL || _ArcInfo.qinf[ 2 ] == ARC_FULL ) {
        xl = x1;
        xr = x2;
    } else {
        // find left most point
        if( q1 == 0 && q2 == 2 ) {  // start on top and end on bottom
            xl = x1;
        } else {
            xl = _ArcInfo.centre.xcoord;
            if( _ArcInfo.vecta.xcoord < 0 ) {
                if( q1 == 1 ) {
                    xl = x1;
                } else {
                    xl = VectA.left_x;
                }
            }
            if( _ArcInfo.vectb.xcoord < 0 ) {
                if( q2 == 1 ) {
                    xl = x1;
                } else {
                    xl = min( xl, VectB.left_x );
                }
            }
        }
        // find right most point
        if( q1 == 2 && q2 == 0 ) {  // start on bottom and end on top
            xr = x2;
        } else {
            xr = _ArcInfo.centre.xcoord;
            if( _ArcInfo.vecta.xcoord > 0 ) {
                if( q1 == 1 ) {
                    xr = x2;
                } else {
                    xr = VectA.right_x;
                }
            }
            if( _ArcInfo.vectb.xcoord > 0 ) {
                if( q2 == 1 ) {
                    xr = x2;
                } else {
                    xr = max( xr, VectB.right_x );
                }
            }
        }
    }
    _L1ClipFill( xl, xr, _ArcInfo.centre.ycoord );
    // fill the ellipse
    EllInfo.x_reflect = x1 + x2;        // need to reflect to use symmetry
    EllInfo.y_reflect = y1 + y2;
    EllInfo.prev_y = _ArcInfo.centre.ycoord;    // already done
    _L0Ellipse( x1, y1, x2, y2, ArcFill );
}


static void ArcFill( short x, short y, short q )
//==============================================

// given the arc point (x,y), fill the arc

{
    short               xl;
    short               xr;
    short               x1;
    short               x2;

    if( q == 4 ) {    // only care about points in all 4 quadrants
        if( y != EllInfo.prev_y ) {
            EllInfo.prev_y = y;
            for( q = 0; q <= 2; q += 2 ) {
                xl = EllInfo.x_reflect - x;  // reflected point is x1 + ( x2 - x )
                xr = x;
                if( q == 2 ) {  // bottom half
                    y = EllInfo.y_reflect - y;
                }
                switch( _ArcInfo.qinf[ q ] ) {
                case ARC_EMPTY:
                    break;
                case ARC_FULL:
                    _L1ClipFill( xl, xr, y );
                    break;
                case ARC_VECT_A:
                    _LineMove( &VectA );
                    if( q == 0 ) {
                        xr = min( xr, VectA.right_x );
                    } else {
                        xl = max( xl, VectA.left_x );
                    }
                    _L1ClipFill( xl, xr, y );
                    break;
                case ARC_VECT_B:
                    _LineMove( &VectB );
                    if( q == 0 ) {
                        xl = max( xl, VectB.left_x );
                    } else {
                        xr = min( xr, VectB.right_x );
                    }
                    _L1ClipFill( xl, xr, y );
                    break;
                case ARC_BOTH_IN:
                    _LineMove( &VectA );
                    _LineMove( &VectB );
                    if( q == 0 ) {
                        xr = min( xr, VectA.right_x );
                        xl = max( xl, VectB.left_x );
                    } else {
                        xr = min( xr, VectB.right_x );
                        xl = max( xl, VectA.left_x );
                    }
                    _L1ClipFill( xl, xr, y );
                    break;
                case ARC_BOTH_OUT:
                    _LineMove( &VectA );
                    _LineMove( &VectB );
                    if( q == 0 ) {
                        x1 = min( xr, VectA.right_x );
                        x2 = max( xl, VectB.left_x );
                    } else {
                        x1 = min( xr, VectB.right_x );
                        x2 = max( xl, VectA.left_x );
                    }
                    if( x1 > x2 ) { // segments overlap
                        _L1ClipFill( xl, xr, y );
                    } else {
                        _L1ClipFill( xl, x1, y );
                        _L1ClipFill( x2, xr, y );
                    }
                }
            }
        }
    }
}

#endif
