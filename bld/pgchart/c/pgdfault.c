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


#include <string.h>
#include "pgvars.h"


short _XHeight( chartenv _WCI86FAR *env, float value )
//===============================================

/*  Given env and a value the x position on the graph is calculated.  */

{
    return( env->datawindow.x1 +
        ( env->datawindow.x2 - env->datawindow.x1 ) *
        ( value - env->xaxis.scalemin ) /
        ( env->xaxis.scalemax - env->xaxis.scalemin ) );
}


short _YHeight( chartenv _WCI86FAR *env, float value )
//===============================================

/*  Given env and a value the x position on the graph is calculated.  */

{
    return( env->datawindow.y2 -
        ( env->datawindow.y2 - env->datawindow.y1 ) *
        ( value - env->yaxis.scalemin ) /
        ( env->yaxis.scalemax - env->yaxis.scalemin ) );
}


void _CommonSetup( chartenv _WCI86FAR *env, char _WCI86FAR *_WCI86FAR *categories, short n )
//===========================================================================

/*  Groups the common setup function calls into one
    function to save space. */

{
    _setcliprgn( env->chartwindow.x1, env->chartwindow.y1,
                 env->chartwindow.x2, env->chartwindow.y2 );
    _pg_window( &env->chartwindow );
    _pg_window( &env->datawindow );
    _Title( env );
    if( env->charttype != _PG_PIECHART ) {
        _Axes( env, categories, n );
    }
    _setcliprgn( env->datawindow.x1 + 1, env->datawindow.y1 + 1,
                 env->datawindow.x2 - 1, env->datawindow.y2 - 1 );
}


static void _WindowInit( windowtype _WCI86FAR *window )
//================================================

{
    window->border = TRUE;
    window->borderstyle = 1;
    window->bordercolor = 1;
}


static void _TitleInit( titletype _WCI86FAR *title )
//=============================================

{
    title->titlecolor = 1;
    title->justify = _PG_CENTER;
}


static void _AxisInit( axistype _WCI86FAR *axis )
//==========================================

{
    axis->gridstyle = 1;
    axis->axiscolor = 1;
    axis->labeled = TRUE;
    axis->rangetype = _PG_LINEARAXIS;
    axis->logbase = 10;
    axis->autoscale = TRUE;
    axis->scalefactor = 1;
    axis->ticformat = _PG_DECFORMAT;
    _TitleInit( &axis->axistitle );
    _TitleInit( &axis->scaletitle );
}


static void _LegendInit( legendtype _WCI86FAR *legend )
//================================================

{
    legend->legend = TRUE;
    legend->place = _PG_RIGHT;
    legend->textcolor = 1;
    legend->autosize = TRUE;
    _WindowInit( &legend->legendwindow );
}


short _WCI86FAR _CGRAPH _pg_defaultchart( chartenv _WCI86FAR *env,
                                     short type, short style )
//============================================================

/*  Initializes the chartenv variable "env" to default values.  */

{
    MemorySet( env, 0, sizeof( chartenv ) );
    env->charttype = type;
    env->chartstyle = style;
    _WindowInit( &env->chartwindow );
    _WindowInit( &env->datawindow );
    _TitleInit( &env->maintitle );
    _TitleInit( &env->subtitle );
    _AxisInit( &env->xaxis );
    _AxisInit( &env->yaxis );
    _LegendInit( &env->legend );
    return( 0 );
}

Entry( _PG_DEFAULTCHART, _pg_defaultchart ) // alternate entry-point
