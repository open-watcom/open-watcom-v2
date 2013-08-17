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


_WCRTLINK grcolor _WCI86FAR _CGRAPH _getcolor( void )
/*==================================

   This function returns the current colour. */

{
    return( _CurrColor );
}

Entry( _GETCOLOR, _getcolor ) // alternate entry-point


_WCRTLINK grcolor _WCI86FAR _CGRAPH _setcolor( grcolor pixval )
/*==========================================

   This routine sets the colour for line drawing and object filling. */

{
    if( _GraphMode() ) {
        return( _L2setcolor( pixval ) );
    } else {
        return( -1 );
    }
}

Entry( _SETCOLOR, _setcolor ) // alternate entry-point

#if defined( VERSION2 )

#define r_shift (8-_CurrState->mi.RedMaskSize)
#define g_shift (8-_CurrState->mi.GreenMaskSize)
#define b_shift (8-_CurrState->mi.BlueMaskSize)

#define r_pos (_CurrState->mi.RedFieldPosition)
#define g_pos (_CurrState->mi.GreenFieldPosition)
#define b_pos (_CurrState->mi.BlueFieldPosition)

_WCRTLINK grcolor _WCI86FAR _CGRAPH _rgb2pixel(unsigned char r, unsigned char g, unsigned char b)
/*=========================================================================

   This routine generates pixels for hi- and truecolor modes. rgb values are in range 0-255. */
{
    return (((grcolor)(r>>r_shift))<<r_pos)|
    (((grcolor)(g>>g_shift))<<g_pos)|
    (((grcolor)(b>>b_shift))<<b_pos);
}

_WCRTLINK void _WCI86FAR _CGRAPH _pixel2rgb(unsigned char *r, unsigned char *g, unsigned char *b, grcolor pix)
/*=========================================================================================

   This routine extracts rgb values from hi-and truecolor pixels. rgb values will be in 0-255 range. 
   There may be loss of precision. */
{
    *r=((pix>>r_pos)<<r_shift)&0xff;
    *g=((pix>>g_pos)<<g_shift)&0xff;
    *b=((pix>>b_pos)<<b_shift)&0xff;
}

#undef r_pos
#undef g_pos
#undef b_pos

#undef r_shift
#undef g_shift
#undef b_shift

#endif

grcolor _WCI86FAR _L2setcolor( grcolor pixval )
/*====================================

   This routine sets the colour for line drawing and object filling. */

{
    grcolor         prev;

    prev = _CurrColor;
#if defined( VERSION2 )
    _CurrColor = pixval & _CurrState->pixel_mask;
#else
    _CurrColor = pixval & ( _CurrState->vc.numcolors - 1 );
#endif
    return( prev );
}
