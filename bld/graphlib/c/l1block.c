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


void _L1Block( short x1, short y1, short x2, short y2 )
/*=====================================================

    Fill a block which has already been clipped.    */
{
    short           t;
#if defined( _DEFAULT_WINDOWS )
    WPI_COLOUR          color;
    WPI_PRES            dc;
    HBITMAP             bm;
    HBRUSH              brush;
    HBRUSH              old_brush;
    HPEN                pen;
    HPEN                old_pen;
#else
    short           deltax;
#endif

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

#if defined( _DEFAULT_WINDOWS )
    /* set up the brushes and pens */
    color = _Col2RGB( _CurrColor );
    dc = _Mem_dc;
    pen = _wpi_createpen( PS_NULL, 0, color );
    old_pen = _wpi_selectpen( dc, pen );

    if( _HaveMask == 0 ) {
        brush = _wpi_createsolidbrush( color );
    } else {
        /* if a mask is defined, convert it to bitmap */
        bm = _Mask2Bitmap( dc, &_FillMask );
        brush = _wpi_createpatternbrush( bm );
    }
    old_brush = _wpi_selectbrush( dc, brush );

    y1 = _wpi_cvth_y( y1, _GetPresHeight() );
    y2 = _wpi_cvth_y( y2 + 1, _GetPresHeight() );

    /* draw the rectangle */
    _wpi_rectangle( dc, x1, y1, x2+1, y2 );

    /* clean up */
    _wpi_getoldpen( dc, old_pen );
    _wpi_deletepen( pen );

    _wpi_getoldbrush( dc, old_brush );
    _wpi_deletebrush( brush );

    if( _HaveMask != 0 ) {
        _wpi_deletebitmap( bm );
    }

    /* Update the rectangle */
    _MyInvalidate( x1, y1, x2, y2 );
    _RefreshWindow();
#else
    _StartDevice();
    deltax = x2 - x1 + 1;                           /* include end points   */
    for( ; y1 <= y2; y1++ ) {                           /* draw the rows    */
        _L1Fill( x1, y1, deltax );
    }
    _ResetDevice();
#endif
}
