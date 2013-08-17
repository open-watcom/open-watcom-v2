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


_WCRTLINK struct _wxycoord _WCI86FAR _CGRAPH _moveto_w( double x, double y )
/*===========================================================

   This routine moves the current output position to the logical
   co-ordinates specifed by the parameters.  It returns the previous
   location. */

{
    struct _wxycoord    prev;

    prev = _CurrPos_w;
    _CurrPos_w.wx = x;
    _CurrPos_w.wy = y;
    _CurrPos.xcoord = _GetLogX( _WtoPhysX( x ) );
    _CurrPos.ycoord = _GetLogY( _WtoPhysY( y ) );
    return( prev );
}

Entry1( _MOVETO_W, _moveto_w ) // alternate entry-point


_WCRTLINK struct _wxycoord _WCI86FAR _CGRAPH _moveto_w_87( double x, double y )
/*============================================================*/

{
    return( _moveto_w( x, y ) );
}

Entry1( _MOVETO_W_87, _moveto_w_87 ) // alternate entry-point
