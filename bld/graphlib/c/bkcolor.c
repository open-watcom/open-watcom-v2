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
* Description:  Background color get/set routines.
*
****************************************************************************/


#include "gdefn.h"
#if !defined( _DEFAULT_WINDOWS )
#include "gbios.h"
#endif


_WCRTLINK long _WCI86FAR _CGRAPH _getbkcolor( void )
/*===================================

   This function returns the current background colour. */

{
    return( _CurrBkColor );
}

Entry( _GETBKCOLOR, _getbkcolor ) // alternate entry-point


_WCRTLINK long _WCI86FAR _CGRAPH _setbkcolor( long pixval )
/*==========================================

   This routine sets the background colour.  In text modes, it simply
   modifies the character attribute setting for later output.  In
   graphics modes, it also sets background pixels to the specified
   colour.  The routine returns the previous background colour. */

{
    short               color;
    long                prev;

    prev = _CurrBkColor;
    _CurrBkColor = pixval;

#if defined( _DEFAULT_WINDOWS )
    _remappalette( 0, pixval );
    color = color;
#else
    if( _GrMode ) {     // in graphics modes, remap colour 0
        if( _CurrState->vc.adapter == _CGA && _CurrState->vc.mode != _HRESBW ) {
            VideoInt( _BIOS_SET_OVERSCAN, _CnvColour( pixval ), 0, 0 );
        } else {
            _remappalette( 0, pixval );
        }
    } else {            // in text modes, set _CharAttr
        color = _CnvColour( pixval ) & 7;   // keep only 3 background bits
        _CharAttr &= 0x8F;                  // eliminate old colour
        _CharAttr |= color << 4;            // insert new colour
    }
#endif
    return( prev );
}

Entry( _SETBKCOLOR, _setbkcolor ) // alternate entry-point
