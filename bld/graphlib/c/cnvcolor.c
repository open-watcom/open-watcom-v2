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

#if defined( VERSION2 )

#define __BLACK          0x000000L
#define __BLUE           0x2a0000L
#define __GREEN          0x002a00L
#define __CYAN           0x2a2a00L
#define __RED            0x00002aL
#define __MAGENTA        0x2a002aL
#define __BROWN          0x00152aL
#define __WHITE          0x2a2a2aL
#define __GRAY           0x151515L
#define __LIGHTBLUE      0x3F1515L
#define __LIGHTGREEN     0x153f15L
#define __LIGHTCYAN      0x3f3f15L
#define __LIGHTRED       0x15153fL
#define __LIGHTMAGENTA   0x3f153fL
#define __YELLOW         0x153f3fL
#define __BRIGHTWHITE    0x3f3f3fL
#define __LIGHTYELLOW    __YELLOW

#else

#define __BLACK          _BLACK
#define __BLUE           _BLUE
#define __GREEN          _GREEN
#define __CYAN           _CYAN
#define __RED            _RED
#define __MAGENTA        _MAGENTA
#define __BROWN          _BROWN
#define __WHITE          _WHITE
#define __GRAY           _GRAY
#define __LIGHTBLUE      _LIGHTBLUE
#define __LIGHTGREEN     _LIGHTGREEN
#define __LIGHTCYAN      _LIGHTCYAN
#define __LIGHTRED       _LIGHTRED
#define __LIGHTMAGENTA   _LIGHTMAGENTA
#define __YELLOW         _YELLOW
#define __BRIGHTWHITE    _BRIGHTWHITE
#define __LIGHTYELLOW    _YELLOW

#endif

long _VGA_Colours[ 16 ] = {     /* Maps VGA colours to EGA equivalents */
    __BLACK, __BLUE, __GREEN, __CYAN,
    __RED, __MAGENTA, __BROWN, __WHITE,
    __GRAY, __LIGHTBLUE, __LIGHTGREEN, __LIGHTCYAN,
    __LIGHTRED, __LIGHTMAGENTA, __LIGHTYELLOW, __BRIGHTWHITE
};


short _CnvColour( long colr )
/*===========================

   This routine converts a VGA colour to an equivalent EGA colour. */

{
    short                 i;                      /* loop counter */

    for( i = 0; i < 16; i++ ) {
        if( colr == _VGA_Colours[ i ] ) {       /* if its a standard one */
            return( i );                        /* use the index */
        }
    }
    return( colr & 0x000f );                    /* otherwise, just use */
}                                               /* colour mod 16 */
