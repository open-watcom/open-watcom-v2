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


_WCRTLINK void _WCI86FAR _CGRAPH _setcliprgn( short x1, short y1, short x2, short y2 )
/*=====================================================================

   This routine sets the clip region for graphics. */

{
    if( _GraphMode() ) {
        _L2setcliprgn( x1, y1, x2, y2 );
    }
}

Entry( _SETCLIPRGN, _setcliprgn ) // alternate entry-point


void _WCI86FAR _L2setcliprgn( short x1, short y1, short x2, short y2 )
/*===============================================================

   This routine sets the clip region for graphics. */

{
    short               xmax;
    short               ymax;
    short               t;

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
    xmax = _CurrState->vc.numxpixels - 1;
    ymax = _CurrState->vc.numypixels - 1;
    if( x1 < 0 ) {
        x1 = 0;
    }
    if( y1 < 0 ) {
        y1 = 0;
    }
    if( x2 > xmax ) {
        x2 = xmax;
    }
    if( y2 > ymax ) {
        y2 = ymax;
    }

#if defined( __OS2__ )
    _CurrState->clip_def.xmin = x1 - 1;
    _CurrState->clip_def.ymin = y1 - 1;
#else
    _CurrState->clip_def.xmin = x1;
    _CurrState->clip_def.ymin = y1;
#endif
#if defined( __WINDOWS__ )
    _CurrState->clip_def.xmax = x2 + 1;
    _CurrState->clip_def.ymax = y2 + 1;
#else
    _CurrState->clip_def.xmax = x2;
    _CurrState->clip_def.ymax = y2;
#endif
    _setclip( _GCLIPON );
}


_WCRTLINK void _WCI86FAR _CGRAPH _getcliprgn( short _WCI86FAR *xmin, short _WCI86FAR *ymin,
                               short _WCI86FAR *xmax, short _WCI86FAR *ymax )
//=================================================================

{
#if defined( __OS2__ )
    *xmin = _CurrState->clip.xmin + 1;
    *ymin = _CurrState->clip.ymin + 1;
#else
    *xmin = _CurrState->clip.xmin;
    *ymin = _CurrState->clip.ymin;
#endif
#if defined( __WINDOWS__ )
    *xmax = _CurrState->clip.xmax - 1;
    *ymax = _CurrState->clip.ymax - 1;
#else
    *xmax = _CurrState->clip.xmax;
    *ymax = _CurrState->clip.ymax;
#endif
}

Entry( _GETCLIPRGN, _getcliprgn ) // alternate entry-point
