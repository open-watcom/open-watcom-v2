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


_WCRTLINK void _WCI86FAR _CGRAPH _getimage( short x1, short y1, short x2, short y2,
/*========================*/ char _WCI86HUGE * image )

/* This routine places the rectangle defined by ( x1, y1 ) and ( x2, y2 ),
   in viewport coordinates, into a buffer pointed to by image. */

{
    if( _GrProlog() ) {
        _L2getimage( _VtoPhysX( x1 ), _VtoPhysY( y1 ),
                     _VtoPhysX( x2 ), _VtoPhysY( y2 ), image );
        _GrEpilog();
    }
}

Entry( _GETIMAGE, _getimage ) // alternate entry-point


void _WCI86FAR _L2getimage( short x1, short y1, short x2, short y2,
/*==================*/ char _WCI86HUGE * image )

/* This routine places the rectangle defined by ( x1, y1 ) and ( x2, y2 ),
   in physical coordinates, into a buffer pointed to by image.  */

{
    _L1GetPic( x1, y1, x2, y2, (struct picture _WCI86HUGE *)image );
}


#if defined( _DEFAULT_WINDOWS )

_WCRTLINK void _WCI86FAR _CGRAPH _freeimage( char _WCI86HUGE *image )
//===============================================

/* This function frees the dc used by _getimage and _putimage */

{
    struct picture _WCI86FAR *picture;

    picture = (struct picture _WCI86FAR *) image;
    _wpi_deletepres( picture->buffer, picture->pdc );
    _wpi_deletebitmap( picture->bmp );
}

#endif
