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
* Description:  Bitmap file loader interface.
*
****************************************************************************/


#ifndef _BITMAP_H_INCLUDED
#define _BITMAP_H_INCLUDED

#define BITMAP_TYPE     ((((WORD)'M') << 8) + 'B')

/* This macro determines the number of bytes of storage needed by a bitmap. */
#define BITS_TO_BYTES( x, y )   ((((x) + 31) / 32) * 4 * (y))

#define DIB_INFO_SIZE( bc ) \
    ((bc) < 9 ? sizeof( BITMAPINFO ) + sizeof( RGBQUAD ) * ((1 << (bc)) - 1) : \
    sizeof( BITMAPINFOHEADER ))

#define CORE_INFO_SIZE( bc ) \
    (sizeof( BITMAPCOREINFO ) + sizeof( RGBTRIPLE ) * ((1 << (bc)) - 1))

typedef struct {
    union {
        BITMAPCOREINFO  *bm_core;
        BITMAPINFO      *bm_info;
    } u;
    bool                is_core;
} bitmap_info;

extern HBITMAP  ReadBitmapFile( HWND, char *, bitmap_info * );

#endif /* _BITMAP_H_INCLUDED */
