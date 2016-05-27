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


#include <wwindow.hpp>

#include "screendv.h"
#include "assure.h"

void ScreenDev::open( WWindow *w )
//--------------------------------
{
    const int fudge = 1;

    _window = w;
    w->getClientRect( _clipRect );
    _clipRect.x( _clipRect.x() - fudge );
    _clipRect.y( _clipRect.y() - fudge );
    _clipRect.w( _clipRect.w() + 2 * fudge );
    _clipRect.h( _clipRect.h() + 2 * fudge );
}

void ScreenDev::rectangle( const WRect & rect )
//---------------------------------------------
{
    REQUIRE( _window != NULL, "accessing unopened output device" );
    REQUIRE( _pinf != NULL, "paint info not set" );
    _window->drawRect( rect, _pinf->getColour() );
}

void ScreenDev::drawText( const WPoint & start, const char * name )
//-----------------------------------------------------------------
{
    REQUIRE( _window != NULL, "accessing unopened output device" );
    _window->drawText( start, name, ColorBlack, _window->backgroundColour() );
}

void ScreenDev::drawText( const WPoint & start, const char * name,
                          Color fg, Color bg )
//--------------------------------------------------------------------
{
    REQUIRE( _window != NULL, "accessing unopened output device" );
    _window->drawText( start, name, fg, bg );
}

inline long max( long a, long b ) { return (a>b) ? a : b; }
inline long min( long a, long b ) { return (a<b) ? a : b; }

enum ClipCode {
    CL_Inside  = 0x00,
    CL_Above   = 0x08,
    CL_Below   = 0x04,
    CL_Right   = 0x02,
    CL_Left    = 0x01,
};

struct PointCode {
            PointCode( long & xv, long & yv, ClipCode cv )
                : x( xv ), y( yv ), code( cv ){}
    long &      x;
    long &      y;
    ClipCode    code;
};

inline static void CalcOut( PointCode & point,
                            long top, long left,
                            long bottom, long right )
//---------------------------------------------------
{
    point.code = CL_Inside;

    if( point.x < left ) {
        point.code = (ClipCode) (point.code | CL_Left);
    }
    if( point.x > right ) {
        point.code = (ClipCode) (point.code | CL_Right);
    }
    if( point.y < top ) {
        point.code = (ClipCode) (point.code | CL_Above);
    }
    if( point.y > bottom ) {
        point.code = (ClipCode) (point.code | CL_Below);
    }
}

static bool ClipToRect( long & x1, long & y1, long & x2, long & y2,
                        const WRect & clip )
//-----------------------------------------------------------------
// Cohen-Sutherland Clipping Algorithm from _Fundamentals of Interactive
// Computer Graphics_, page 148.
{
    PointCode   point1( x1, y1, CL_Inside );
    PointCode   point2( x2, y2, CL_Inside );
    PointCode * p1;
    PointCode * p2;
    PointCode * tmp;
    long        top = clip.y();
    long        left = clip.x();
    long        bottom = clip.y() + clip.h();
    long        right = clip.x() + clip.w();

    p1 = & point1;
    p2 = & point2;

    while( 1 ) {
        CalcOut( *p1, top, left, bottom, right );
        CalcOut( *p2, top, left, bottom, right );

        if( p1->code == CL_Inside && p2->code == CL_Inside ) {
            return true;                                // trivial acceptance
        }
        if( (p1->code & p2->code) != 0 ) {
            return false;                               // trivial rejection
        }

        if( p1->code == 0 ) {   // p1 inside -- swap so p1 outside
            tmp = p1;
            p1 = p2;
            p2 = tmp;
        }

        // perform a subdivision; move p1 to the intersection point.
        // use the formula y = y1 + slope * (x - x1),
        //                 x = x1 + (y - y1) / slope.

        if( p1->code & CL_Above ) {                 // divide at top
            p1->x += ((p2->x - p1->x) * (top - p1->y)) / (p2->y - p1->y);
            p1->y = top;
        } else if( p1->code & CL_Below ) {          // divide at bottom of
            p1->x += ((p2->x - p1->x) * (bottom - p1->y)) / (p2->y - p1->y);
            p1->y = bottom;
        } else if( p1->code & CL_Right ) {          // divide at right
            p1->y += ((p2->y - p1->y) * (right - p1->x)) / (p2->x - p1->x);
            p1->x = right;
        } else if( p1->code & CL_Left ) {           // divide at left
            p1->y += ((p2->y - p1->y) * (left - p1->x)) / (p2->x - p1->x);
            p1->x = left;
        }
    }
}

void ScreenDev::lineTo( long x2, long y2 )
//----------------------------------------
{
    REQUIRE( _window != NULL, "accessing unopened output device" );
    REQUIRE( _pinf != NULL, "paint info not set" );

    long x1 = _currx;
    long y1 = _curry;

    if( ClipToRect( x1, y1, x2, y2, _clipRect ) ) {
        WPoint p1( (int) x1, (int) y1 );
        WPoint p2( (int) x2, (int) y2 );
        _window->drawLine( p1, p2, _pinf->getStyle(),
                           _pinf->getThickness(), _pinf->getColour() );
    }
}

int ScreenDev::getTextExtentX( const char * name )
//------------------------------------------------
{
    REQUIRE( _window != NULL, "accessing unopened output device" );
    return _window->getTextExtentX( name );
}

int ScreenDev::getTextExtentY( const char * name )
//------------------------------------------------
{
    WPoint      avg;
    WPoint      max;

    REQUIRE( _window != NULL, "accessing unopened output device" );
    name=name;//get rid of unref'd error
    // changed so that this reflects how much space GUI uses to draw the text
    _window->textMetrics( avg, max );
    return( avg.y() );
}

