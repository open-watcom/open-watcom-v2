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


#include <stdlib.h>
#include <string.h>
#include "pgvars.h"


static short _JustifyTitle( titletype _WCI86FAR *title,
                            short left, short right, short dim )
//==============================================================

/*  Determine title position. */

{
    short               pos;
    short               len;

    if( title->justify == _PG_LEFT ) {
        pos = left;
    } else {
        len = _sLabelLength( title->title, dim );
        if( title->justify == _PG_CENTER ) {
            pos = ( left + right - len ) / 2;
        } else {    // _PG_RIGHT
            pos = right - len;
        }
    }
    return( pos );
}


static void DisplayHTitle( titletype _WCI86FAR *title, chartenv _WCI86FAR *env,
                           short y, windowtype _WCI86FAR *window, short extra )
//========================================================================

//  Display a horizontal title.

{
    short               x;

    x = _JustifyTitle( title, window->x1 + extra, window->x2 - extra, _XVECTOR );
    _pg_hlabelchart( env, x - env->chartwindow.x1, y - env->chartwindow.y1,
                     title->titlecolor, title->title );
}


static void DisplayVTitle( titletype _WCI86FAR *title, chartenv _WCI86FAR *env, short x )
//============================================================================

//  Display a vertical title.

{
    short               y;

    y = _JustifyTitle( title, env->datawindow.y1, env->datawindow.y2, _YVECTOR );
    _pg_vlabelchart( env, x - env->chartwindow.x1, y - env->chartwindow.y1,
                     title->titlecolor, title->title );
}


void _Title( chartenv _WCI86FAR *env )
//===============================

/*  Draws the titles stored in the chartenv variable on the screen in
    the appropriate position.   */

{
    short               x, y;

    y = env->chartwindow.y1 + _CharHeight;
    if( env->maintitle.title[ 0 ] != '\0' ) {
        DisplayHTitle( &env->maintitle, env, y, &env->chartwindow, _CharWidth );
    }
    if( env->subtitle.title[ 0 ] != '\0' ) {
        if( env->maintitle.title[ 0 ] != '\0' ) {
            y += 3 * _CharHeight / 2;
        }
        DisplayHTitle( &env->subtitle, env, y, &env->chartwindow, _CharWidth );
    }

    if( env->legend.legend && ( env->legend.place == _PG_BOTTOM ) ) {
        y = env->legend.legendwindow.y1;
    } else {
        y = env->chartwindow.y2;
    }
    y -= 2 * _CharWidth;
    if( env->xaxis.axistitle.title[ 0 ] != '\0' &&
        env->charttype != _PG_PIECHART ) {
        DisplayHTitle( &env->xaxis.axistitle, env, y, &env->datawindow, 0 );
        y -= 2 * _CharWidth;
    }
    if( env->xaxis.labeled && env->xaxis.scaletitle.title[ 0 ] != '\0' &&
        ( env->charttype == _PG_BARCHART || env->charttype == _PG_SCATTERCHART ) ) {
        DisplayHTitle( &env->xaxis.scaletitle, env, y, &env->datawindow, 0 );
    }

    if( env->charttype != _PG_PIECHART ) {
        x = env->chartwindow.x1 + _CharWidth;
        if( env->yaxis.axistitle.title[ 0 ] != '\0') {
            DisplayVTitle( &env->yaxis.axistitle, env, x );
            x += 2 * _CharWidth;
        }
        if( env->yaxis.labeled && env->yaxis.scaletitle.title[ 0 ] != '\0' &&
            env->charttype != _PG_BARCHART ) {
            DisplayVTitle( &env->yaxis.scaletitle, env, x );
        }
    }
}
