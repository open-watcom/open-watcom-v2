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
#include "pgvars.h"


short _analyze_pie( chartenv _WCI86FAR *env )
//======================================

{
    short               error;
    short               x1, y1;
    short               x2, y2;

    error = 0;
    if( env->chartstyle == _PG_PERCENT ) {
        x1 = env->datawindow.x1 + 7 * _CharWidth;
        x2 = env->datawindow.x2 - 7 * _CharWidth;
        y1 = env->datawindow.y1 + 3 * _CharHeight;
        y2 = env->datawindow.y2 - 3 * _CharHeight;
        if( x1 > x2 || y1 > y2 ) {
            error = _PG_BADDATAWINDOW;
        }
    }
    return( error );
}


short _WCI86FAR _CGRAPH _pg_analyzepie( chartenv _WCI86FAR *env,
                           char _WCI86FAR *_WCI86FAR *categories,
                           float _WCI86FAR *values, short _WCI86FAR *explode, short n )
//===========================================================================

/*  Calculates all the data necessary to draw a pie chart.  */

{
    short               error;

    values = values;
    explode = explode;
    error = _analyze_common( env, NULL, NULL, n, n, n, categories );
    if( error < 100 ) {
        error = _analyze_pie( env );
    }
    return( error );
}

Entry( _PG_ANALYZEPIE, _pg_analyzepie ) // alternate entry-point
