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


short _WCI86FAR _CGRAPH _getcolor( void )
/*==================================

   This function returns the current colour. */

{
    return( _CurrColor );
}

Entry( _GETCOLOR, _getcolor ) // alternate entry-point


short _WCI86FAR _CGRAPH _setcolor( short pixval )
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


short _WCI86FAR _L2setcolor( short pixval )
/*====================================

   This routine sets the colour for line drawing and object filling. */

{
    short               prev;

    prev = _CurrColor;
    _CurrColor = pixval & ( _CurrState->vc.numcolors - 1 );
    return( prev );
}
