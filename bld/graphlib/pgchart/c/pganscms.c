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


#include <stddef.h>
#include "pgvars.h"


_WCRTLINK short _WCI86FAR _CGRAPH _pg_analyzescatterms( chartenv _WCI86FAR *env,
                            float _WCI86FAR *xvalues,
                            float _WCI86FAR *yvalues, short nseries, short n,
                            short arraydim, char _WCI86FAR *_WCI86FAR *serieslabels )
//=========================================================================

/*  Sets up the chartenv based on the parameter "env" where necessary
    i.e. placement of the datawindow, determination of scale markings etc.
    -checks to see if previous settings are "legal", if not they are
    set to default values.  */

{
    short               error;

    error = _analyze_common( env, xvalues, yvalues,
                             nseries, n, arraydim, serieslabels );
    if( error < 100 ) {
        error = _analyze_datawindow( env, NULL, n );
    }
    return( error );
}

Entry( _PG_ANALYZESCATTERMS, _pg_analyzescatterms ) // alternate entry-point


_WCRTLINK short _WCI86FAR _CGRAPH _pg_analyzescatter( chartenv _WCI86FAR *env,
                       float _WCI86FAR *xvalues, float _WCI86FAR *yvalues, short n )
//========================================================================

/*  Sets up the chartenv based on the parameter "env" where necessary
    i.e. placement of the datawindow, determination of scale markings etc.
    -checks to see if previous settings are "legal", if not they are
    set to default values.  */

{
    short               oldlegend;
    short               error;

    oldlegend = env->legend.legend;
    env->legend.legend = FALSE;

    error = _pg_analyzescatterms( env, xvalues, yvalues, 1, n, n, NULL );

    env->legend.legend = oldlegend;
    return( error );
}

Entry( _PG_ANALYZESCATTER, _pg_analyzescatter ) // alternate entry-point
