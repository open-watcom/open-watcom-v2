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


void _WCI86FAR _CGRAPH _gettextextent( short x, short y, char _WCI86FAR * str,
/*=============================*/ struct xycoord _WCI86FAR * concat,
                                  struct xycoord _WCI86FAR * extent )

/* This routine finds the four corners of the parallelogram surrounding the
   given text string.  It also returns the concatenation point, i.e. the
   point where the next character would logically be output. */

{
    short               i;

    if( !_GraphMode() ) {
        return;
    }
    x = _GetPhysX( x );
    y = _GetPhysY( y );
    _L1TXX( x, y, str, concat, extent );
    for( i = 0; i < 4; i++ ) {
        extent[ i ].xcoord = _GetLogX( extent[ i ].xcoord );
        extent[ i ].ycoord = _GetLogY( extent[ i ].ycoord );
    }
    concat->xcoord = _GetLogX( concat->xcoord );
    concat->ycoord = _GetLogY( concat->ycoord );
}

Entry( _GETTEXTEXTENT, _gettextextent ) // alternate entry-point
