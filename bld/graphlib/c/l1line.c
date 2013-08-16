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
* Description:  High level line drawing routines.
*
****************************************************************************/


#include "gdefn.h"
#include "rotate.h"


static void         _L0Line( short, short, short, short, short, unsigned short );


void _L1SLine( short x1, short y1, short x2, short y2 )
/*=====================================================

    Draw a clipped solid line using the given color.    */

/*  REMINDER: For default windows, this function DOES NOT update the window.
              If you call this function you have to update the window with
              the UpdateWindow yourself.*/
{
    if( _L0LineClip( &x1, &y1, &x2, &y2 ) == 0 ) {
        _L0Line( x1, y1, x2, y2, _CurrColor, SOLID_LINE );
    }
}

short _L1Line( short x1, short y1, short x2, short y2 )
/*===================================================*/

/*  Draw a clipped line using the given color and style.    */

/*  REMINDER: For default windows, this function DOES NOT update the window.
              If you call this function you have to update the window with
              the UpdateWindow yourself.*/
{
    short               init_x;
    short               init_y;
    short               dx;
    short               dy;
    unsigned short      style;

    init_x = x1;
    init_y = y1;
    style = _LineStyle;
    if( _L0LineClip( &x1, &y1, &x2, &y2 ) == 0 ) {
        // if the initial point of a non-solid line was clipped,
        // rotate the mask accordingly
        if( style != SOLID_LINE && ( x1 != init_x || y1 != init_y ) ) {
            dx = abs( x1 - init_x );
            dy = abs( y1 - init_y );
            if( dx < dy ) {
                dx = dy;        // set dx to max of the two
            }
            style = _wrol( style, dx & 15 );    // rotate mask left
        }
        _L0Line( x1, y1, x2, y2, _CurrColor, style );
        return( TRUE );
    } else {
        return( FALSE );
    }
}

static void _L0Line( short x1, short y1, short x2, short y2,
/*================*/ short color, unsigned short style )

/*  Implement the line drawing alogorithm.  */

/*  REMINDER: For default windows, this function DOES NOT update the window.
              If you call this function you have to update the window with
              the UpdateWindow yourself.*/
{
#if defined( _DEFAULT_WINDOWS )
    WPI_PRES            dc;
    HPEN                pen;
    HPEN                old_pen;
    int                 line_style;
    WPI_COLOUR          RGB_color;
    WPI_POINT           point1, point2;

    dc = _Mem_dc;

// Setup before drawing
    line_style = _MapLineStyle( style );
    RGB_color = _Col2RGB( color );
    pen = _wpi_createpen( line_style, 0, RGB_color );
    old_pen = _wpi_selectpen( dc, pen );

    y1 = _wpi_cvth_y( y1, _GetPresHeight() );
    y2 = _wpi_cvth_y( y2, _GetPresHeight() );
    _wpi_setpoint( &point1, x1, y1 );
    _wpi_setpoint( &point2, x2, y2 );

    _wpi_moveto( dc, &point1 );
    _wpi_lineto( dc, &point2 );

// Clean up
    _wpi_getoldpen( dc, old_pen );
    _wpi_deletepen( pen );

// Invalidate the window area to be updated
    _MyInvalidate( x1, y1, x2, y2 );
#else
    short               delta_x;
    short               delta_y;
    gr_device _FARD     *dev_ptr;
    move_fn             *x_func;
    move_fn             *y_func;
    putdot_fn           *putdot;

    _StartDevice();
    dev_ptr = _CurrState->deviceptr;
    putdot = dev_ptr->plot[ _PlotAct ];
    ( *dev_ptr->setup )( x1, y1, color );

    delta_x = x2 - x1;              /* select the move right or left        */
    if( delta_x < 0 ) {             /* function such that x is increasing   */
        delta_x = -delta_x;
        x_func = dev_ptr->left;
    } else {
        x_func = dev_ptr->right;
    }
    delta_y = y2 - y1;              /* select the move up or down function  */
    if( delta_y < 0 ) {             /* such that y is increasing            */
        delta_y = -delta_y;
        y_func = dev_ptr->up;
    } else {
        y_func = dev_ptr->down;
    }
    /*  Invert the order of the parameters such that the change in the first
        coordinate is larger than the change in the second coordinate. */
    if( delta_x < delta_y ) {
        _L0DrawLine( _Screen.mem, _Screen.colour, style, _Screen.mask,
                     delta_y << 1, delta_x << 1, y_func, x_func, putdot );
    } else {
        _L0DrawLine( _Screen.mem, _Screen.colour, style, _Screen.mask,
                     delta_x << 1, delta_y << 1, x_func, y_func, putdot );
    }
    _ResetDevice();
#endif
}
