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


void _WCI86FAR _CGRAPH _putimage_w( double x, double y, char _WCI86HUGE *image,
/*==========================*/ short dispmode )

/* This routine retrieves an picture stored at image and places it on the
   screen starting at ( x, y ), in window coordinates. The mode in which
   the picture is placed on the screen is specified by dispmode.    */

{
    if( _GrProlog() ) {
        _L2putimage( _WtoPhysX( x ), _WtoPhysY( y ), image, dispmode );
        _GrEpilog();
    }
}

Entry( _PUTIMAGE_W, _putimage_w ) // alternate entry-point


void _WCI86FAR _CGRAPH _putimage_w_87( double x, double y, char _WCI86HUGE *image,
/*=============================*/ short dispmode )

{
    _putimage_w( x, y, image, dispmode );
}

Entry( _PUTIMAGE_W_87, _putimage_w_87 ) // alternate entry-point
