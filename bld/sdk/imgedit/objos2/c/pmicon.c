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


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <dos.h>

#define INCL_GPI
#define INCL_WIN
#define INCL_GPILOGCOLORTABLE
#include <os2.h>

#include "wpi.h"
#include "..\h\pmicon.h"
#include "..\h\wbitmap.h"
#include "..\..\..\misc\mem.h"

/*
 * convertToBmp2 - converts an old form BITMAPINFO structure to a
 *                   BITMAPINFO2 structure (all extra fields are 0).
 */
static BITMAPINFO2 *convertToBmp2( BITMAPINFO *bmi )
{
    BITMAPINFO2         *bmi2;
    LONG                info_size;
    USHORT              colour_count;
    USHORT              i;

    info_size = BMPINFO2_SIZE( bmi->cBitCount );
    bmi2 = MemAlloc( info_size );
    if (!bmi2) return(NULL);

    memset(bmi2, 0, sizeof(info_size) );
    bmi2->cbFix = sizeof(BITMAPINFO2) - sizeof(RGB2);
    bmi2->cx = bmi->cx;
    bmi2->cy = bmi->cy;
    bmi2->cPlanes = bmi->cPlanes;
    bmi2->cBitCount = bmi->cBitCount;
    colour_count = (1<<(bmi->cBitCount));
    for (i=0; i < colour_count; ++i) {
        bmi2->argbColor[i].bBlue = bmi->argbColor[i].bBlue;
        bmi2->argbColor[i].bGreen = bmi->argbColor[i].bGreen;
        bmi2->argbColor[i].bRed = bmi->argbColor[i].bRed;
    }
    return( bmi2 );
} /* convertToBmp2 */

/*
 * readBitmapInfo - returns the bitmap info
 */
static BITMAPINFO2 *readBitmapInfo( FILE *fp, BOOL is_bmp2 )
{
    ULONG               size;
    BITMAPINFO2         *bmi2;
    BITMAPINFO          *bmi;
    BITMAPINFOHEADER    *bmih;
    BITMAPINFOHEADER2   *bmih2;
    long                file_pos;
    USHORT              bitcount;

    // store current position in the file
    file_pos = ftell( fp );

    if ( is_bmp2 ) {
        bmih2 = MemAlloc( sizeof(BITMAPINFOHEADER2) );
        fread( bmih2, sizeof(BITMAPINFOHEADER2), 1, fp );
        bitcount = bmih2->cBitCount;
        MemFree( bmih2 );
        fseek( fp, file_pos, SEEK_SET );

        size = BMPINFO2_SIZE( bitcount );
        bmi2 = MemAlloc( size );
        if( !bmi2 ) return( NULL );

        fread( bmi2, size, 1, fp );
    } else {
        bmih = MemAlloc( sizeof(BITMAPINFOHEADER) );
        fread( bmih, sizeof(BITMAPINFOHEADER), 1, fp );
        bitcount = bmih->cBitCount;
        MemFree( bmih );
        fseek( fp, file_pos, SEEK_SET );

        size = BMPINFO_SIZE( bitcount );
        bmi = MemAlloc( size );
        if ( !bmi ) return (NULL);

        fread( bmi, size, 1, fp );
        bmi2 = convertToBmp2( bmi );
        MemFree( bmi );
    }
    return( bmi2 );
} /* readBitmapInfo */

/*
 * readFileHeader - reads in the first 5 elements of the bitmapfileheader2
 *                  ie. everything except the bitmapinfoheader2
 */
static BITMAPFILEHEADER2 *readFileHeader( FILE *fp )
{
    BITMAPFILEHEADER2   *bfh;
    USHORT              size;

    size = sizeof(USHORT) + 2*(sizeof(SHORT) + sizeof(ULONG));
    bfh = MemAlloc( size );

    fread( bfh, size, 1, fp );
    return( bfh );
} /* readFileHeader */

/*
 * assignImageInfo - assigns the values in the image info
 */
static void assignImageInfo( pm_image_info *info, int filetype, FILE *fp,
                                ULONG offset, ULONG offbits, BOOL is_bmp2 )
{
    BITMAPFILEHEADER2   *bfh;

    if (filetype != PMBITMAP_FILETYPE) {
        info->andinfo = readBitmapInfo( fp, is_bmp2 );
        // Divide height by 2 because it's stored as twice the real height
        info->andinfo->cy = info->andinfo->cy / 2;
        info->and_offset = offset + offbits;
        bfh = readFileHeader( fp );
        info->clr_offset = offset + bfh->offBits;
        info->xorinfo = readBitmapInfo( fp, is_bmp2 );
    } else {
        info->andinfo = NULL;
        info->and_offset = 0;
        info->xorinfo = readBitmapInfo( fp, is_bmp2 );
        info->clr_offset = offset + offbits;
    }
} /* assignImageInfo */

a_pm_image_file *OpenPMImage( FILE *fp, int type, int *retcode )
{
    a_pm_image_file             *img_file;
    ULONG                       size;
    BITMAPARRAYFILEHEADER2      *bafh;
    USHORT                      i;
    USHORT                      filetype;
    BOOL                        is_bmp2 = TRUE;
    ULONG                       bafh_offset;

    type = type;                // temporary

    bafh = MemAlloc( sizeof(BITMAPARRAYFILEHEADER2) );
    fseek( fp, 0L, SEEK_SET );
    fread( bafh, PMFILE_HEADER_SIZE, 1, fp );
    if ( bafh->usType != BFT_BITMAPARRAY ) {
        /*
         * Eventually we'll want to do other stuff here (old version,
         * windows bitmap file, etc...)
         */
        *retcode =  NON_OS2_FILE;
        MemFree( bafh );
        return( NULL );
    }

    switch( bafh->bfh2.usType ) {
    case BFT_COLORICON:
    case BFT_ICON:
        filetype = PMICON_FILETYPE;
        break;

    case BFT_POINTER:
    case BFT_COLORPOINTER:
        filetype = PMPOINTER_FILETYPE;
        break;

    case BFT_BMAP:
        filetype = PMBITMAP_FILETYPE;
        break;
    default:
        MemFree( bafh );
        *retcode = NON_OS2_FILE;
        return( NULL );
        break;
    }

    if (bafh->cbSize != sizeof(BITMAPARRAYFILEHEADER2)) {
        is_bmp2 = FALSE;
    }

    i = 0;
    bafh_offset = 0L;
    size = sizeof(a_pm_image_file) - sizeof(pm_image_info);
    img_file = MemAlloc( size );
    while( TRUE ) {
        size += sizeof( pm_image_info );
        img_file = MemReAlloc( img_file, size );
        img_file->resources[i].xhotspot = bafh->bfh2.xHotspot;
        img_file->resources[i].yhotspot = bafh->bfh2.yHotspot;

        assignImageInfo( &(img_file->resources[i]), filetype, fp, bafh_offset,
                                 bafh->bfh2.offBits, is_bmp2);
        if ( bafh->offNext == 0 ) {
            break;
        }
        fseek( fp, bafh->offNext, SEEK_SET );
        bafh_offset = bafh->offNext;
        fread( bafh, sizeof(BITMAPARRAYFILEHEADER2), 1, fp );
        ++i;
    }
    img_file->count = i + 1;
    img_file->type = filetype;
    *retcode = filetype;

    return( img_file );
} /* OpenPMImage */

/*
 * ClosePMImage - closes an image file which was opened with OpenPMImage.
 */
void ClosePMImage( a_pm_image_file *img_file )
{
    USHORT      i;

    for (i = 0; i < img_file->count; ++i) {
        MemFree( img_file->resources[i].xorinfo );
        if ( img_file->resources[i].andinfo ) {
            MemFree( img_file->resources[i].andinfo );
        }
    }
    MemFree( img_file );
} /* ClosePMImage */

/*
 * GetPMImageBits - reads the bits in and stores them in the appropriate
 *                  mask.
 */
a_pm_image *GetPMImageBits( a_pm_image_file *img_file, FILE *fp, int img_num )
{
    BITMAPINFO2 *bm;
    ULONG       offset;
    a_pm_image  *image;

    bm = img_file->resources[img_num].xorinfo;

    if (img_file->type != PMBITMAP_FILETYPE) {
        image = MemAlloc( sizeof(a_pm_image) );
        image->and_size = BITS_TO_BYTES( bm->cx, bm->cy );
        image->xor_size = image->and_size;
        image->clr_size = BITS_TO_BYTES( bm->cx * bm->cBitCount, bm->cy );

        /*
         * Read in the XOR info first, then the AND info and finally the
         * CLR info.  Note that this is different from Windows.  The and
         * offset is really the offset to the xor info.  The and info follows
         * the xor info.
         */
        image->xor_mask = MemAlloc( image->xor_size );
        offset = img_file->resources[img_num].and_offset;
        fread( image->xor_mask, 1, image->xor_size, fp );

        image->and_mask = MemAlloc( image->and_size );
        offset = offset + image->xor_size;
        fseek( fp, offset, SEEK_SET );
        fread( image->and_mask, 1, image->and_size, fp );
    } else {
        image = MemAlloc( sizeof(a_pm_image) );
        image->and_size = 0;
        image->xor_size = BITS_TO_BYTES( bm->cx * bm->cBitCount, bm->cy );
        image = MemReAlloc( image, sizeof(a_pm_image) + image->xor_size );
        image->and_mask = NULL;
    }

    image->clr_mask = MemAlloc( image->clr_size );
    offset = img_file->resources[img_num].clr_offset;
    fseek( fp, offset, SEEK_SET );
    fread( image->clr_mask, 1, image->clr_size, fp );
    return( image );
} /* GetPMImageBits */

/*
 * FiniPMImage - cleans up after the GettPMImageBits
 */
void FiniPMImage( a_pm_image *image )
{
    free( image->clr_mask );
    if (image->and_mask) {
        free( image->xor_mask );
        free( image->and_mask );
    }
    free( image );
} /* FiniPMImage */

/*
 * PMImageToAndBitmap - converts the given image data to the AND bitmap.  Note
 *                      that the routine actually returns a wpi_handle.
 */
HBITMAP PMImageToAndBitmap( a_pm_image *image, a_pm_image_file *img_file,
                                                                int index )
{
    WPI_PRES            pres;
    HBITMAP             hbitmap;
    BITMAPINFOHEADER2   bmh;

    memcpy(&bmh, img_file->resources[index].andinfo, sizeof(BITMAPINFOHEADER2));

    pres = _wpi_getpres( HWND_DESKTOP );
    hbitmap = _wpi_createdibitmap( pres, &bmh, CBM_INIT, image->and_mask,
                                img_file->resources[index].andinfo, 0 );
    _wpi_releasepres( HWND_DESKTOP, pres );
    return( hbitmap );
} /* PMImageToAndBitmap */

/*
 * PMImageToXorBitmap - converts the given image data to the XOR bitmap.
 *                      Note that it returns a bitmap handle created by wpi
 */
HBITMAP PMImageToXorBitmap( a_pm_image *image, a_pm_image_file *img_file,
                                                                int index )
{
    WPI_PRES            pres;
    HBITMAP             hbitmap;
    BITMAPINFOHEADER2   bmh;

    memcpy(&bmh, img_file->resources[index].andinfo, sizeof(BITMAPINFOHEADER2));

    pres = _wpi_getpres( HWND_DESKTOP );
    hbitmap = _wpi_createdibitmap( pres, &bmh, CBM_INIT, image->xor_mask,
                                    img_file->resources[index].andinfo, 0 );
    _wpi_releasepres( HWND_DESKTOP, pres );
    return( hbitmap );
} /* PMImageToXorBitmap */

/*
 * PMImageToClrBitmap - creates the colour part of the bitmap.
 */
HBITMAP PMImageToClrBitmap( a_pm_image *image, a_pm_image_file *img_file,
                                                                int index )
{
    WPI_PRES            pres;
    HBITMAP             hbitmap;
    BITMAPINFOHEADER2   bmh;

    memcpy(&bmh, img_file->resources[index].xorinfo, sizeof(BITMAPINFOHEADER2));
    pres = _wpi_getpres( HWND_DESKTOP );
    hbitmap = _wpi_createdibitmap( pres, &bmh, CBM_INIT, image->clr_mask,
                                    img_file->resources[index].xorinfo, 0 );
    _wpi_releasepres( HWND_DESKTOP, pres );
    return( hbitmap );
} /* PMImageToClrBitmap */

/*
 * createColourMask - This routine creates the colour bitmap mask that
 *                      is saved for icons and cursors.  It's different
 *                      than the xor mask that i store since it has 0's
 *                      anywhere where the AND bitmap has 1's.
 */
static HBITMAP createColourMask( HBITMAP xorbitmap, HBITMAP clrbitmap, WPI_INST hab )
{
    WPI_PRES    pres;
    WPI_PRES    srcpres;
    HDC         srcdc;
    WPI_PRES    destpres;
    HDC         destdc;
    HBITMAP     new_bitmap;
    IMAGEBUNDLE p;
    int         width;
    int         height;
    int         planes;
    int         bitspixel;
    HBITMAP     oldsrc;
    HBITMAP     olddest;

    pres = _wpi_getpres( HWND_DESKTOP );
    srcpres = _wpi_createcompatiblepres( pres, hab, &srcdc );
    destpres = _wpi_createcompatiblepres( pres, hab, &destdc );
    _wpi_releasepres( HWND_DESKTOP, pres );
    _wpi_torgbmode( srcpres );
    _wpi_torgbmode( destpres );

    p.lBackColor = RGB_BLACK;
    p.lColor = RGB_WHITE;
    GpiSetAttrs(destpres, PRIM_IMAGE, IBB_COLOR | IBB_BACK_COLOR, 0, &p);

    _wpi_getbitmapparms( clrbitmap, &width, &height, &planes, NULL, &bitspixel);
    new_bitmap = _wpi_createbitmap( width, height, planes, bitspixel, NULL );
    oldsrc = _wpi_selectbitmap( srcpres, xorbitmap );
    olddest = _wpi_selectbitmap( destpres, new_bitmap );

    _wpi_bitblt( destpres, 0, 0, width, height, srcpres, 0, 0, SRCCOPY );
    _wpi_getoldbitmap( srcpres, oldsrc );
    oldsrc = _wpi_selectbitmap( srcpres, clrbitmap );

    _wpi_bitblt( destpres, 0, 0, width, height, srcpres, 0, 0, SRCPAINT );
    _wpi_getoldbitmap( srcpres, oldsrc );
    _wpi_getoldbitmap( destpres, olddest );

    _wpi_deletecompatiblepres( srcpres, srcdc );
    _wpi_deletecompatiblepres( destpres, destdc );

    return( new_bitmap );
} /* createColourMask */

/*
 * PMImageToWinXorBitmap - combines previous 2 routines.
 */
HBITMAP PMImageToWinXorBitmap( a_pm_image *image, a_pm_image_file *img_file,
                                                int index, WPI_INST hab )
{
    HBITMAP     xorbitmap;
    HBITMAP     clrbitmap;
    HBITMAP     hbitmap;

    clrbitmap = PMImageToClrBitmap( image, img_file, index );
    xorbitmap = PMImageToXorBitmap( image, img_file, index );
    hbitmap = createColourMask( xorbitmap, clrbitmap, hab );
    return( hbitmap );
} /* PMImageToWinXorBitmap */

