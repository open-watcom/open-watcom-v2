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


short _WCI86FAR _CGRAPH _lineto_w( double x, double y )
/*================================================

   This function draws a line from the current position to the specified
   position using the current colour and linestyle.  It returns a
   success flag. */

{
    short               success;        /* line successfully drawn */

    if( _GrProlog() ) {
        success = _L2line( _WtoPhysX( _CurrPos_w.wx ),
                           _WtoPhysY( _CurrPos_w.wy ),
                           _WtoPhysX( x ), _WtoPhysY( y ) );
        _RefreshWindow();
        _moveto_w( x, y );
        _GrEpilog();
    } else {
        success = 0;
    }
    return( success );
}

Entry( _LINETO_W, _lineto_w ) // alternate entry-point


short _WCI86FAR _CGRAPH _lineto_w_87( double x, double y )
/*=================================================*/

{
    return( _lineto_w( x, y ) );
}

Entry( _LINETO_W_87, _lineto_w_87 ) // alternate entry-point
