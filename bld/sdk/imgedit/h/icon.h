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


/* move these here from icon.c */
#define BITS_INTO_BYTES( x, y )   (((x) * (y) + 7) / 8)

#define BITMAP_SIZE( bc )  \
    ((bc)->biBitCount < 9 ? \
     sizeof( BITMAPINFO ) + (sizeof( RGBQUAD ) * ((1 << (bc)->biBitCount) - 1)) : \
     sizeof( BITMAPINFOHEADER ))

#pragma pack( 1 );

typedef struct an_img_resource {
    BYTE        width;
    BYTE        height;
    BYTE        color_count;
    BYTE        reserved;
    WORD        xhotspot;
    WORD        yhotspot;
    DWORD       DIB_size;
    DWORD       DIB_offset;
} an_img_resource;

typedef struct an_img_file {
    WORD            reserved;
    WORD            type;
    WORD            count;
    an_img_resource resources[1];
} an_img_file;

#pragma pack();

typedef struct an_img {
    BITMAPINFO  *bm;
    BYTE        *xor_mask;
    WORD        xor_size;
    BYTE        *and_mask;
    WORD        and_size;
} an_img;
