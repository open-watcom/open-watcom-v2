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


short _WCI86FAR _CGRAPH _ellipse( short fill, short x1, short y1, short x2, short y2 )
/*===============================================================================

   This function draws or fills an ellipse defined by the rectangle whose
   opposite corners are ( x1, y1 ) and ( x2, y2 ) in viewport coordinates.  */

{
    short               success;

    if( _GrProlog() ) {
        success = _L2ellipse( fill, _VtoPhysX( x1 ), _VtoPhysY( y1 ),
                                    _VtoPhysX( x2 ), _VtoPhysY( y2 ) );
        _GrEpilog();
    } else {
        success = 0;
    }
    return( success );
}

Entry( _ELLIPSE, _ellipse ) // alternate entry-point


short _WCI86FAR _L2ellipse( short fill, short x1, short y1, short x2, short y2 )
/*=========================================================================

   This function draws or fills an ellipse defined by the rectangle
   whose opposite corners are ( x1, y1 ) and ( x2, y2 ). All values are
   in physical coordinates. */

{
    _L1Ellipse( fill, x1, y1, x2, y2 );
    return( _ErrorStatus == _GROK );
}
