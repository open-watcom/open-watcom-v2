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


#include <stdlib.h>
#include "mem.h"

#ifdef __OS2_PM__
#define INCL_GPI
#include <os2.h>
#else
#include <windows.h>
#endif

#ifndef __OS2_PM__

HPALETTE CreateDIBPalette( BITMAPINFO *info )
{
    unsigned            num_colours, i;
    LOGPALETTE          *palette;
    HPALETTE            palette_handle;
    RGBQUAD             *quads;

    num_colours = info->bmiHeader.biClrUsed;
    if( num_colours == 0 && info->bmiHeader.biBitCount != 24 ) {
        num_colours = 1 << info->bmiHeader.biBitCount;
    }

    palette_handle = (HPALETTE)0;

    if( num_colours ) {
        palette = MemAlloc( sizeof( LOGPALETTE ) +
                num_colours * sizeof( PALETTEENTRY ) );
        if( palette == NULL ) return( (HPALETTE)0 );
        palette->palNumEntries = num_colours;
        palette->palVersion = 0x300;

        quads = &info->bmiColors[0];
        for( i = 0; i < num_colours; i++ ) {
            palette->palPalEntry[i].peRed = quads[i].rgbRed;
            palette->palPalEntry[i].peGreen = quads[i].rgbGreen;
            palette->palPalEntry[i].peBlue = quads[i].rgbBlue;
            palette->palPalEntry[i].peFlags = 0;
        }
        palette_handle = CreatePalette( palette );
        MemFree( palette );
    }
    return( palette_handle );
}

#else


/*
 * CreateDIBPalette - creates a colour palette for PM given a BITMAPINFO2
 *                    structure (which contains rgb values)
 */
HPAL CreateDIBPalette( BITMAPINFO2 *info )
{
    USHORT      i;
    ULONG       *clrtable;
    USHORT      num_colours;
    HPAL        palette_handle;

    num_colours = info->cclrUsed;
    if( num_colours == 0 && info->cBitCount != 24 ) {
        num_colours = 1 << info->cBitCount;
    }
    palette_handle = (HPAL)0;

    if( num_colours ) {
        clrtable = MemAlloc( sizeof(ULONG) * num_colours );
        if( clrtable == NULL ) return( (HPAL)0 );

        for( i = 0; i < num_colours; i++ ) {
            clrtable[i] = PC_RESERVED * 0xFFFFFF +
                                info->argbColor[i].bRed * 0x00FFFF +
                                info->argbColor[i].bGreen * 0x0000FF +
                                info->argbColor[i].bBlue;
        }
        palette_handle = GpiCreatePalette( 0, LCOL_PURECOLOR,
                            LCOLF_CONSECRGB, (LONG)num_colours, clrtable );
        MemFree( clrtable );
    }
    return( palette_handle );
} /* CreateDIBPalette */
#endif

