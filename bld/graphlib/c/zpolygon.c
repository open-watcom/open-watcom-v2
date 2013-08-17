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


#include <malloc.h>
#include "gdefn.h"
#include "stkavail.h"


_WCRTLINK short _WCI86FAR _CGRAPH _polygon_wxy( short fill, short numpts,
/*============================*/ struct _wxycoord _WCI86FAR *points )

/* This routine draws or fills a polygon specified by the array
   points[], in window coordinates. */

{
    short                   i;
    short                   needed_bytes;
    short                   x1, y1, x2, y2;
    short                   success;
    struct xycoord _WCI86FAR *   stack;


    if( numpts <= 2 ) {
        _ErrorStatus = _GRINVALIDPARAMETER;
        return( 0 );
    }
    success = 0;                            /* assume not successful    */
    if( _GrProlog() ) {
        if( fill == _GFILLINTERIOR ) {
            needed_bytes = _RoundUp( numpts * sizeof( struct xycoord ) );
#if defined( _DEFAULT_WINDOWS )
            stack = _MyAlloc( needed_bytes );
#else
            if( _stackavail() - needed_bytes > 0x100 ) {
                stack = __alloca( needed_bytes );
#endif
                for( i = 0; i < numpts; i++ ) {
                    stack[i].xcoord = _WtoPhysX( points[i].wx );
                    stack[i].ycoord = _WtoPhysY( points[i].wy );
                }
                success = _L1FillArea( numpts, stack );
#if defined( _DEFAULT_WINDOWS )
            _MyFree( stack );
            _GrEpilog();
#else
            } else {
                _ErrorStatus = _GRINSUFFICIENTMEMORY;
            }
#endif
        } else {
            x1 = _WtoPhysX( points[numpts-1].wx );
            y1 = _WtoPhysY( points[numpts-1].wy );
            for( i = 0; i < numpts; i++ ) {
                x2 = _WtoPhysX( points[i].wx );
                y2 = _WtoPhysY( points[i].wy );
                if( y1 < y2 ) {
                    _L1Line( x2, y2, x1, y1 );
                } else {
                    _L1Line( x1, y1, x2, y2 );
                }
                x1 = x2;
                y1 = y2;
            }
            success = 1;
        }
        _GrEpilog();
    }
    return( success );
}

Entry1( _POLYGON_WXY, _polygon_wxy ) // alternate entry-point


_WCRTLINK short _WCI86FAR _CGRAPH _polygon_w( short fill, short numpts,
/*==========================*/ double _WCI86FAR *points )

{
    return( _polygon_wxy( fill, numpts, (struct _wxycoord _WCI86FAR *) points ) );
}

Entry1( _POLYGON_W, _polygon_w ) // alternate entry-point
