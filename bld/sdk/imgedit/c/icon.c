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


#include "precomp.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "imgedit.h"
#include "iemem.h"

#define ICON_FILE_TYPE      1
#define CURSOR_FILE_TYPE    2

/*
 * ImageOpen - icon and cursor files are nearly identical so I've grouped them
 *             into img/image data structures
 *           - this function takes a handle to an image resource file and returns
 *             an img_file structure that can be used to load an image resource or
 *             extract the AND and XOR bitmaps
 *           - any file opened with this routine should be closed with ImageClose (below)
 */

an_img_file *ImageOpen( FILE *fp )
{
    an_img_file     *img_file;
    unsigned        size;

    size = sizeof( an_img_file );

    img_file = MemAlloc( sizeof( an_img_file ) );
    /* read the header once to find out how many images are in the file */
    fseek( fp, 0L, SEEK_SET );
    fread( img_file, sizeof( an_img_file ), 1, fp );
    if( img_file->type != ICON_FILE_TYPE && img_file->type != CURSOR_FILE_TYPE ) {
        MemFree( img_file );
        return( NULL );
    }

    if( img_file->count > 1 ) {
        size = sizeof( an_img_file ) + sizeof( an_img_resource ) * (img_file->count - 1);
        img_file = MemReAlloc( img_file, size );
        fseek( fp, 0L, SEEK_SET );
        fread( img_file, (long)size, 1, fp );
    }
    return( img_file );

} /* ImageOpen */

/*
 * ImageOpenData
 */
an_img_file *ImageOpenData( BYTE *data, unsigned *pos )
{
    an_img_file     *img_file;
    unsigned        size;

    if( data == NULL || pos == NULL ) {
        return( NULL );
    }

    /* read the header once to find out how many images are in the file */
    size = sizeof( an_img_file );
    img_file = MemAlloc( size );
    memcpy( img_file, data, size );
    if( img_file->type != ICON_FILE_TYPE && img_file->type != CURSOR_FILE_TYPE ) {
        MemFree( img_file );
        return( NULL );
    }

    if( img_file->count > 1 ) {
        size = sizeof( an_img_file ) + sizeof( an_img_resource ) * (img_file->count - 1);
        img_file = MemReAlloc( img_file, size );
        memcpy( img_file, data, size );
    }

    *pos = size;

    return( img_file );

} /* ImageOpenData */

/*
 * readImgBitmap - read in the bitmap information for an individual image
 *               - assume fp is positioned at the start of the bitmap information
 *               - first read in the BITMAPINFOHEADER to get information about the
 *                 number of quads needed, then reposition and read in the entire
 *                 BITMAPINFOHEADER structure
 */
static BITMAPINFO *readImgBitmap( FILE *fp )
{
    BITMAPINFO          *bm;
    BITMAPINFOHEADER    *header;
    long                DIB_offset, bitmap_size;

    header = MemAlloc( sizeof( BITMAPINFOHEADER ) );
    DIB_offset = ftell( fp );
    fread( header, sizeof( BITMAPINFOHEADER ), 1, fp );
    fseek( fp, DIB_offset, SEEK_SET );
    bitmap_size = BITMAP_SIZE( header );
    bm = MemReAlloc( header, bitmap_size );
    if( bm == NULL ) {
        return( NULL );
    }
    fread( bm, bitmap_size, 1, fp );
    return( bm );

} /* readImgBitmap */

/*
 * readImgBitmapData
 */
static BITMAPINFO *readImgBitmapData( BYTE *data, unsigned *pos )
{
    BITMAPINFO          *bm;
    BITMAPINFOHEADER    *header;
    long                bitmap_size;

    if( data == NULL || pos == NULL ) {
        return( NULL );
    }

    header = (BITMAPINFOHEADER *)(data + *pos);
    bitmap_size = BITMAP_SIZE( header );
    bm = MemAlloc( bitmap_size );
    if( bm == NULL ) {
        return( NULL );
    }
    memcpy( bm, data + *pos, bitmap_size );
    *pos += bitmap_size;

    return( bm );

} /* readImgBitmapData */

/*
 * reverseAndBits - reverse the AND mask bits
 *                - without this routine, the bits are stored such that the image is
 *                  upside down
 *                - row 0 becomes row height - 1, 1 becomes height - 2, ...
 */
static void reverseAndBits( int width, int height, BYTE *bits )
{
    WORD        i;
    int         cpysize;
    BYTE        *temp;

#ifdef REVERSE_TESTING
    FILE        *fp;
    int         j;
    char        ch, shift;
#endif

    if( width == 24 ) {         /* this fixes 24x24 icon problem */
        width = 32;
    }

    cpysize = width / 8;        /* each row is width / 8 bytes */


#ifdef REVERSE_TESTING
    fp = fopen( "output", "wt" );
    for( j = 0; j < height; j++ ) {
        for( i = 0; i < cpysize; i++ ) {
            ch = bits[j * cpysize + i];
            shift = 0x80;
            while( shift > 0x00 ) {
                fprintf( fp, "%c", (ch & shift) ? '1' : '0' );
                shift = shift >> 1;
            }
        }
        fprintf( fp, "\n" );
    }
    fprintf( fp, "\n" );
#endif

    temp = MemAlloc( cpysize );
    for( i = 0; i < height / 2; i++ ) {
        memcpy( temp, &bits[i * cpysize], cpysize );
        memcpy( &bits[i * cpysize], &bits[(height - i - 1) * cpysize], cpysize );
        memcpy( &bits[(height - i - 1) * cpysize], temp, cpysize );
    }

#ifdef REVERSE_TESTING
    for( j = 0; j < height; j++ ) {
        for( i = 0; i < cpysize; i++ ) {
            ch = bits[j * cpysize + i];
            shift = 0x80;
            while( shift > 0x00 ) {
                fprintf( fp, "%c", (ch & shift) ? '1' : '0' );
                shift = shift >> 1;
            }
        }
        fprintf( fp, "\n" );
    }
    fprintf( fp, "\n" );
    fclose( fp );
#endif

    MemFree( temp );

} /* reverseAndBits */

void dropBitmapPadding(an_img *img, BITMAPINFOHEADER *h, WORD org_and_sz, WORD org_xor_sz)
{
    if( h->biWidth == 16 && h->biHeight == 16 ) {
        int ii, jj;

        /*
         * fix the AND size that was double above and drop every other
         * pair of bytes from the AND mask
        */
        // img->and_size /= 2;
        img->and_size = org_and_sz;
        for( ii = 0, jj = 0; ii < img->and_size; ) {
            img->and_mask[ii] = img->and_mask[jj];
            img->and_mask[ii + 1] = img->and_mask[jj + 1];
            ii += 2;
            jj += 4;
        }
        if( h->biBitCount == 1 ) {
            /* do the same for the XOR size and mask */
            img->xor_size = org_xor_sz;
            for( ii = 0, jj = 0; ii < img->xor_size; ) {
                img->xor_mask[ii] = img->xor_mask[jj];
                img->xor_mask[ii + 1] = img->xor_mask[jj + 1];
                ii += 2;
                jj += 4;
            }
        }

    } else if( h->biWidth == 24 || h->biWidth == 48 ) {
        int ii, jj;

        /*
         * fix the AND size that was increased above and drop every other
         * pair of bytes from the AND mask
        */
        img->and_size = org_and_sz;
        if( h->biWidth == 48 ) {  /* don't shift bytes if its 24x24 */
            for( ii = 0, jj = 0; ii < img->and_size; ) {
                img->and_mask[ii] = img->and_mask[jj];
                img->and_mask[ii + 1] = img->and_mask[jj + 1];
                img->and_mask[ii + 2] = img->and_mask[jj + 2];
                if( h->biWidth == 48 ) {
                    img->and_mask[ii + 3] = img->and_mask[jj + 3];
                    img->and_mask[ii + 4] = img->and_mask[jj + 4];
                    img->and_mask[ii + 5] = img->and_mask[jj + 5];
                    ii += 6;
                    jj += 8;
                } else {
                    ii += 3;
                    jj += 4;
                }
            }
            if( h->biBitCount == 1 ) {
                /* do the same for the XOR size and mask */
                // img->xor_size = img->xor_size * 3 / 4;
                img->xor_size = org_xor_sz;
                for( ii = 0, jj = 0; ii < img->xor_size; ) {
                    img->xor_mask[ii] = img->xor_mask[jj];
                    img->xor_mask[ii + 1] = img->xor_mask[jj + 1];
                    img->xor_mask[ii + 2] = img->xor_mask[jj + 2];
                    if( h->biWidth == 48 ) {
                        img->xor_mask[ii + 3] = img->xor_mask[jj + 3];
                        img->xor_mask[ii + 4] = img->xor_mask[jj + 4];
                        img->xor_mask[ii + 5] = img->xor_mask[jj + 5];
                        ii += 6;
                        jj += 8;
                    } else {
                        ii += 3;
                        jj += 4;
                    }
                }
            }
        }
    }
}
/*
 * ImgResourceToImg - take an image file and creates an image structure
 *                    from the i'th image in the file
 *                  - the structure must later be freed with a call to ImgFini
 */
an_img *ImgResourceToImg( FILE *fp, an_img_file *img_file, unsigned i )
{
    an_img_resource     *res;
    BITMAPINFO          *bm;
    BITMAPINFOHEADER    *h;
    an_img              *img;
    LONG                height;
    WORD                original_and_size, original_xor_size;

    if( i >= img_file->count ) {
        return( NULL );
    }
    res = &img_file->resources[i];
    fseek( fp, res->DIB_offset, SEEK_SET );
    bm = readImgBitmap( fp );
    if( bm != NULL ) {
        img = MemAlloc( sizeof( an_img ) );
        img->bm = bm;
        h = &bm->bmiHeader;
        // h->biHeight /= 2;            /* code gen bug */
        if( res->height != 0 ) {
            h->biHeight = res->height;  /* they have height * 2 in this field */
        } else {
            height = h->biHeight;
            height = height / 2;
            h->biHeight = height;
        }
        h->biSizeImage = BITS_INTO_BYTES( h->biWidth * h->biBitCount, h->biHeight );
        img->xor_size = h->biSizeImage;
        img->and_size = BITS_INTO_BYTES( h->biWidth, h->biHeight );
        original_and_size = img->and_size;   /* save the sizes for later */
        original_xor_size = img->xor_size;

        /*
         * JPK - for 16x16, 24x24 & 48x48 icons, need to adjust sizes so
         *       they are multiples of 32
        */
        if( img_file->type == ICON_FILE_TYPE ) {
            /* 16x16 - double AND size, and XOR size if monochrome */
            if( h->biWidth == 16 && h->biHeight == 16 ) {
                img->and_size *= 2;
                if( h->biBitCount == 1 ) {
                    img->xor_size *= 2;
                }

            /* 24x24, 48x48 - increase AND size, and XOR size if monochrome */
            } else if( h->biWidth == 24 || h->biWidth == 48 ) {
                img->and_size = img->and_size * 4 / 3;
                if( h->biBitCount == 1 ) {
                    img->xor_size = img->xor_size * 4 / 3;
                }
            }
        }

        img->xor_mask = MemAlloc( img->xor_size + img->and_size );
        img->and_mask = (unsigned char *)img->xor_mask + img->xor_size;
        fseek( fp, res->DIB_offset + BITMAP_SIZE( h ), SEEK_SET );
        fread( img->xor_mask, img->xor_size + img->and_size, 1, fp );
        if( img_file->type == ICON_FILE_TYPE )
            dropBitmapPadding( img, h, original_and_size, original_xor_size );
        reverseAndBits( h->biWidth, h->biHeight, img->and_mask );
        return( img );
    }
    return( NULL );

} /* ImgResourceToImg */

an_img *ImgResourceToImgData( BYTE *data, unsigned *pos, an_img_file *img_file, unsigned i )
{
    an_img_resource     *res;
    BITMAPINFO          *bm;
    BITMAPINFOHEADER    *h;
    an_img              *img;
    WORD                original_and_size, original_xor_size;

    if( data == NULL || pos == NULL ) {
        return( NULL );
    }

    if( i >= img_file->count ) {
        return( NULL );
    }
    res = &img_file->resources[i];
    *pos = res->DIB_offset;

    bm = readImgBitmapData( data, pos );
    if( bm ) {
        img = MemAlloc( sizeof( an_img ) );
        img->bm = bm;
        h = &bm->bmiHeader;
        // h->biHeight /= 2;            /* code gen bug */
        h->biHeight = res->height;      /* they have height * 2 in this field */
        h->biSizeImage = BITS_INTO_BYTES( h->biWidth * h->biBitCount, h->biHeight );
        img->xor_size = h->biSizeImage;
        img->and_size = BITS_INTO_BYTES( h->biWidth, h->biHeight );
        original_and_size = img->and_size;   /* save the sizes for later */
        original_xor_size = img->xor_size;

        /*
         * See the same code in preceding function.
         */
        if( img_file->type == ICON_FILE_TYPE ) {
            /* 16x16 - double AND size, and XOR size if monochrome */
            if( h->biWidth == 16 && h->biHeight == 16 ) {
                img->and_size *= 2;
                if( h->biBitCount == 1 ) {
                    img->xor_size *= 2;
                }

            /* 24x24, 48x48 - increase AND size, and XOR size if monochrome */
            } else if( h->biWidth == 24 || h->biWidth == 48 ) {
                img->and_size = img->and_size * 4 / 3;
                if( h->biBitCount == 1 ) {
                    img->xor_size = img->xor_size * 4 / 3;
                }
            }
        }

        img->xor_mask = MemAlloc( img->xor_size + img->and_size );
        img->and_mask = (unsigned char *)img->xor_mask + img->xor_size;
        *pos = res->DIB_offset + BITMAP_SIZE( h );
        memcpy( img->xor_mask, data + *pos, img->xor_size + img->and_size );
        if( img_file->type == ICON_FILE_TYPE )
            dropBitmapPadding( img, h, original_and_size, original_xor_size );
        reverseAndBits( h->biWidth, h->biHeight, img->and_mask );
        return( img );
    }
    return( NULL );

} /* ImgResourceToImgData */

/*
 * ImageFini - free up memory allocated for an image structure
 */
void ImageFini( an_img *img )
{
    MemFree( img->bm );
    MemFree( img->xor_mask );
    MemFree( img );

} /* ImageFini */

/*
 * ImgToXorBitmap - create a bitmap which contains the part of an image
 *                  which is to be XOR'd against the background
 */
HBITMAP ImgToXorBitmap( HDC hdc, an_img *img )
{
    HBITMAP             bitmap_handle = NULL;
    HPALETTE            new_palette, old_palette;
    BITMAPINFOHEADER    *h;

    h = &img->bm->bmiHeader;
#if 1
    if( h->biBitCount == 1 ) {
        // this is really just a patch until this is figured out
        reverseAndBits( h->biWidth, h->biHeight, img->xor_mask );
        bitmap_handle = CreateBitmap( h->biWidth, h->biHeight, 1, 1, img->xor_mask );
    } else {
        new_palette = CreateDIBPalette( img->bm );
        if( new_palette != NULL ) {
            old_palette = SelectPalette( hdc, new_palette, FALSE );
            RealizePalette( hdc );
            bitmap_handle = CreateDIBitmap( hdc, &img->bm->bmiHeader, CBM_INIT,
                                            img->xor_mask, img->bm, DIB_RGB_COLORS );
            SelectPalette( hdc, old_palette, FALSE );
            DeleteObject( new_palette );
        }
    }
#else
    new_palette = CreateDIBPalette( img->bm );
    if( new_palette != NULL ) {
        old_palette = SelectPalette( hdc, new_palette, FALSE );
        RealizePalette( hdc );
        bitmap_handle = CreateDIBitmap( hdc, &img->bm->bmiHeader, CBM_INIT,
                                        img->xor_mask, img->bm, DIB_RGB_COLORS );
        SelectPalette( hdc, old_palette, FALSE );
        DeleteObject( new_palette );
    }
#endif
    return( bitmap_handle );

} /* ImgToXorBitmap */

/*
 * ImgToAndBitmap - create the bitmap that allows an image to have a
 *                  tranparent border around the central image
 */
HBITMAP ImgToAndBitmap( HDC hdc, an_img *img )
{
    HBITMAP             bitmap_handle;
    BITMAPINFOHEADER    *h;

    hdc = hdc;
    h = &img->bm->bmiHeader;
    bitmap_handle = CreateBitmap( h->biWidth, h->biHeight, 1, 1, img->and_mask );
    return( bitmap_handle );

} /* ImgToAndBitmap */

/*
 * ImageClose - close an image file that was opened with ImageOpen
 */
void ImageClose( an_img_file *img_file )
{
    MemFree( img_file );

} /* ImageClose */
