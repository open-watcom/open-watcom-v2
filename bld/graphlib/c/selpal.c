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
#include "gbios.h"


short _WCI86FAR _CGRAPH _selectpalette( short palnum )
/*===============================================

   This routine selects the palette to be manipulated.  It returns the
   previous palette number. */

{
    short               prev;           /* previous palette number */

    if( !_GraphMode() ) {
        return( -1 );
    }
    prev = _Palette;
    if( _CurrState->vc.mode == _MRESNOCOLOR ) {
        if( _CurrState->vc.adapter == _CGA ) {
            palnum &= 1;
        } else {
            palnum &= 3;
        }
    } else if ( _CurrState->vc.mode == _MRES4COLOR ) {
        palnum &= 3;
    } else {
        _ErrorStatus = _GRERROR;
        return( -1 );
    }
    _Palette = palnum;
    /* select either green-red-yellow or cyan-magenta-white */
    VideoInt( _BIOS_SET_OVERSCAN, ( 1 << 8 ) + ( palnum & 1 ), 0, 0 );
    /* select either low or high intensity */
    VideoInt( _BIOS_SET_OVERSCAN, ( palnum & 2 ) << 3, 0, 0 );
    return( prev );
}

Entry( _SELECTPALETTE, _selectpalette ) // alternate entry-point
