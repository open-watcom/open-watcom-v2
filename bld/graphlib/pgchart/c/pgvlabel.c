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


#include "pgvars.h"


_WCRTLINK short _WCI86FAR _CGRAPH _pg_vlabelchart( chartenv _WCI86FAR *env,
                            short x, short y, short color, char _WCI86FAR *label )
//===========================================================================

/*  Draws vertical label on the chart.  Coordinates are relative to the
    upper left corner of the chartwindow.   */

{
    char                buffer[ 2 ];

    _setcolor( _PGPalette[ color ].color );
    buffer[ 1 ] = '\0';
    x += env->chartwindow.x1;
    y += env->chartwindow.y1;
    while( *label != '\0' ) {
        buffer[ 0 ] = *label;
        _moveto( x + ( _CharWidth - _getgtextextent( buffer ) ) / 2, y );
        _outgtext( buffer );
        ++label;
        y += _CharHeight;
    }
    return( 0 );
}

Entry2( _PG_VLABELCHART, _pg_vlabelchart ) // alternate entry-point
