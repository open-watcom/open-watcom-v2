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


    // The Windows side
#ifndef __OS2_PM__

#define ROTATE_COUNTERCLOCKWISE     IMGED_ROTATECC
#define SHIFT_UP                    IMGED_UP
#define SHIFT_DOWN                  IMGED_DOWN

#define _imged_getthebits( bits, pres, bmp, oldbmp ) \
                                oldbmp = _wpi_selectbitmap( pres, bmp )
#define _imged_getpixel( bits, pres, x, y ) _wpi_getpixel( pres, x, y )
#define _imged_setpixel( bits, pres, x, y, clr ) _wpi_setpixel(pres, x, y, clr)
#define _imged_freethebits( bits, pres, bmp, fflag, oldbmp ) \
                                _wpi_getoldbitmap( pres, bmp )

    // The PM side
#else

#define ROTATE_COUNTERCLOCKWISE     IMGED_ROTATECL
#define SHIFT_UP                    IMGED_DOWN
#define SHIFT_DOWN                  IMGED_UP

#define _imged_getthebits( bits, pres, bmp, oldbmp ) bits = GetTheBits( bmp )
#define _imged_getpixel( bits, pres, x, y ) MyGetPixel( bits, x, y )
#define _imged_setpixel( bits, pres, x, y, clr ) MySetPixel( bits, x, y, clr )
#define _imged_freethebits( bits, pres, bmp, fflag, oldbmp ) \
                                            FreeTheBits( bits, bmp, fflag )

#endif
