/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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


#include "imgedit.h"
#include <math.h>

/*
 * InitXorAndBitmaps - initialize the XOR and AND bitmaps
 */
void InitXorAndBitmaps( img_node *node )
{
    HDC                 hdc;
    HDC                 memdc;
    HBITMAP             old_hbitmap;
    BITMAPINFOHEADER    bmih;

    hdc = GetDC( NULL );
    memdc = CreateCompatibleDC( hdc );

    if( node->bitcount == 1 ) {
        node->xor_hbitmap = CreateCompatibleBitmap( memdc, node->width, node->height );
    } else {
        bmih.biSize = sizeof( BITMAPINFOHEADER );
        bmih.biWidth = node->width;
        bmih.biHeight = node->height;
        bmih.biPlanes = 1;
        bmih.biBitCount = node->bitcount;
        bmih.biCompression = 0;
        bmih.biSizeImage =  0;
        bmih.biXPelsPerMeter = 0;
        bmih.biYPelsPerMeter = 0;
        bmih.biClrUsed = 0;
        bmih.biClrImportant = 0;

        node->xor_hbitmap = CreateDIBitmap( hdc, &bmih, 0L, NULL, NULL, 0 );
    }
    ReleaseDC( NULL, hdc );

    old_hbitmap = SelectObject( memdc, node->xor_hbitmap);
    if( node->imgtype == BITMAP_IMG ) {
        PatBlt( memdc, 0, 0, node->width, node->height, WHITENESS );
    } else {
        PatBlt( memdc, 0, 0, node->width, node->height, BLACKNESS );
    }
    SelectObject( memdc, old_hbitmap );

    node->and_hbitmap = CreateCompatibleBitmap( memdc, node->width, node->height );

    old_hbitmap = SelectObject( memdc, node->and_hbitmap );
    if( node->imgtype == BITMAP_IMG ) {
        PatBlt( memdc, 0, 0, node->width, node->height, BLACKNESS );
    } else {
        PatBlt( memdc, 0, 0, node->width, node->height, WHITENESS );
    }
    SelectObject( memdc, old_hbitmap );
    DeleteDC( memdc );

} /* InitXorAndBitmaps */
