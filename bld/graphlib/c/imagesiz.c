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
#include "picdef.h"


_WCRTLINK long _WCI86FAR _CGRAPH _imagesize( short x1, short y1, short x2, short y2 )
/*====================================================================

   This routine returns the size of buffer needed to store the picture
   in the rectangle defined by ( x1, y1 ) and ( x2, y2 ), in viewport
   coordinates. */

{
    if( _GraphMode() ) {
        return( _L2imagesize( _VtoPhysX( x1 ), _VtoPhysY( y1 ),
                              _VtoPhysX( x2 ), _VtoPhysY( y2 ) ) );
    } else {
        return( 0 );
    }
}

Entry( _IMAGESIZE, _imagesize ) // alternate entry-point


long _WCI86FAR _L2imagesize( short x1, short y1, short x2, short y2 )
/*==============================================================

   This routine returns the size of buffer needed to store the picture
   in the rectangle defined by ( x1, y1 ) and ( x2, y2 ), in physical
   coordinates. */
{
#if defined( _DEFAULT_WINDOWS )
// For Windows and OS/2, the size is only the size of the record
    x1 = x1;
    x2 = x2;
    y1 = y1;
    y2 = y2;
    return( sizeof( struct picture ) );
#else
    short               xwid;
    short               ywid;
    long                size;           /* size of image */

    xwid = abs( x2 - x1 ) + 1;
    ywid = abs( y2 - y1 ) + 1;
    size = (long)ywid * _RowLen( xwid );
    return( size + 6L );
#endif
}


short _RowLen( short pixels )
/*===========================

    This routine calculates the number of bytes needed to store one row
    of pixels. */

{
    short               size;

    if( _CurrState->misc_info & PLANAR ) {
        size = ( ( pixels + 7 ) / 8 ) * _CurrState->vc.bitsperpixel;
    } else {
#if defined( VERSION2 )
        if( _CurrState->vc.bitsperpixel == 15 ) {
            size = ( ( pixels * 16 + 7 ) / 8 );
        } else {
            size = ( ( pixels * _CurrState->vc.bitsperpixel + 7 ) / 8 );
        }
#else
        size = ( ( pixels * _CurrState->vc.bitsperpixel + 7 ) / 8 );
#endif
    }
    return( size );
}
