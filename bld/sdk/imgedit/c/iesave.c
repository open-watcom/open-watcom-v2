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


#if defined( __NT__ ) && 0
    #undef  WINVER
    #define WINVER 0x0500
    #undef  _WIN32_WINNT
    #define _WIN32_WINNT 0x0500
#endif

#include "imgedit.h"
#include <commdlg.h>
#include <dos.h>
#include <math.h>
#include <limits.h>
#include "wrbitmap.h"
#include "wricon.h"
#include "wrselft.h"
#include "wresdefn.h"
#include "wclbhelp.h"
#include "pathgrp2.h"

#include "clibext.h"


#define DEF_MEMFLAGS    (MEMFLAG_MOVEABLE | MEMFLAG_PURE)
#define SCANLINE_SIZE   32
#define MAX_CHUNK       32768

WINEXPORT UINT_PTR CALLBACK SaveOFNHookProc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam );

static char     initialDir[_MAX_PATH];

/*
 * writeDataInPieces - writes the XOR data for the bitmap in chunks
 */
static bool writeDataInPieces( BITMAPINFO *bmi, FILE *fp, img_node *node )
{
    HDC         hdc;
    HDC         memdc;
    int         scanline_count;
    int         one_scanline_size;
    long        chunk_size;
    int         start;
    int         num_lines;
    long        byte_count;
    BYTE        *buffer;

    hdc = GetDC( NULL );
    memdc = CreateCompatibleDC( hdc );
    ReleaseDC( NULL, hdc );

    byte_count = bmi->bmiHeader.biSizeImage;
    start = 0;
    num_lines = SCANLINE_SIZE;
#if 0
    if( node->width > 32 ) {
        one_scanline_size = BITS_INTO_BYTES( node->width * node->bitcount, 1 );
    } else {
        one_scanline_size = BITS_TO_BYTES( node->width * node->bitcount, 1 );
    }
#else
    one_scanline_size = BITS_TO_BYTES( node->width * node->bitcount, 1 );
#endif
    scanline_count = node->height;
    chunk_size = one_scanline_size * num_lines;
    while( chunk_size > MAX_CHUNK ) {
        chunk_size >>= 1;
        num_lines = chunk_size / one_scanline_size;
    }

    buffer = MemAlloc( chunk_size );
    while( scanline_count > num_lines ) {
        GetDIBits( memdc, node->xor_hbitmap, start, num_lines, buffer, bmi, DIB_RGB_COLORS );
        fwrite( buffer, sizeof( BYTE ), chunk_size, fp );
        scanline_count -= num_lines;
        start += num_lines;
        byte_count -= chunk_size;
    }
    GetDIBits( memdc, node->xor_hbitmap, start, scanline_count, buffer, bmi, DIB_RGB_COLORS );
    fwrite( buffer, sizeof( BYTE ), one_scanline_size * scanline_count, fp );
    MemFree( buffer );
    DeleteDC( memdc );
    return( true );

} /* writeDataInPieces */

/*
 * writeDataInPiecesData
 */
static bool writeDataInPiecesData( BITMAPINFO *bmi, BYTE **data, size_t *size, img_node *node )
{
    HDC         hdc;
    HDC         memdc;
    int         scanline_count;
    int         one_scanline_size;
    long        chunk_size;
    int         start;
    int         num_lines;
    long        byte_count;

    if( data == NULL || *data == NULL || size == NULL ) {
        return( false );
    }

    hdc = GetDC( NULL );
    memdc = CreateCompatibleDC( hdc );
    ReleaseDC( NULL, hdc );

    byte_count = bmi->bmiHeader.biSizeImage;
    start = 0;
    num_lines = SCANLINE_SIZE;
#if 0
    if( node->width > 32 ) {
        one_scanline_size = (int)BITS_INTO_BYTES( node->width * node->bitcount, 1 );
    } else {
        one_scanline_size = (int)BITS_TO_BYTES( node->width * node->bitcount, 1 );
    }
#else
    one_scanline_size = (int)BITS_TO_BYTES( node->width * node->bitcount, 1 );
#endif
    scanline_count = node->height;
    chunk_size = one_scanline_size * num_lines;
    while( chunk_size > MAX_CHUNK ) {
        chunk_size >>= 1;
        num_lines = chunk_size / one_scanline_size;
    }

    while( scanline_count > num_lines ) {
        GetDIBits( memdc, node->xor_hbitmap, start, num_lines, *data + *size, bmi, DIB_RGB_COLORS );
        *size += chunk_size;
        scanline_count -= num_lines;
        start += num_lines;
        byte_count -= chunk_size;
    }
    GetDIBits( memdc, node->xor_hbitmap, start, scanline_count, *data + *size, bmi, DIB_RGB_COLORS );
    *size += scanline_count * one_scanline_size;
    DeleteDC( memdc );
    return( true );

} /* writeDataInPiecesData */

/*
 * SaveOFNHookProc - hook used called by common dialog for 3D controls
 */
UINT_PTR CALLBACK SaveOFNHookProc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam )
{
    wparam = wparam;
    lparam = lparam;
    hwnd = hwnd;
    switch( msg ) {
    case WM_INITDIALOG:
        // We must call this to subclass the directory listbox even
        // if the app calls Ctl3dAutoSubclass (commdlg bug).
#ifndef _WIN64
  #if defined( __NT__ )
        // Only do it if NOT new shell.
        if( LOBYTE( LOWORD( GetVersion() ) ) < 4 ) {
  #endif
           IECtl3dSubclassDlgAll( hwnd );
  #if defined( __NT__ )
        }
  #endif
#endif
        return( TRUE );
    }
    return( FALSE );

} /* SaveOFNHookProc */

/*
 * updateSaveFileInfo
 */
static bool updateSaveFileInfo( const char *fname )
{
    pgroup2     pg;
    size_t      len;

    _splitpath2( fname, pg.buffer, &pg.drive, &pg.dir, NULL, NULL );
    if( pg.dir[0] != '\0' ) {
        len = strlen( pg.dir ) - 1;
        if( len > 0 && pg.dir[len] == '\\' ) {
            pg.dir[len] = '\0';
        }
    }
    _makepath( initialDir, pg.drive, pg.dir, NULL, NULL );
    return( true );

} /* updateSaveFileInfo */

/*
 * getSaveFName - get the name of the file to be saved
 */
static bool getSaveFName( char *fname, image_type img_type )
{
    static OPENFILENAME of;
    char                szFileTitle[_MAX_PATH];
    bool                ok;
    long                of_size;

    fname[0] = '\0';

    of_size = sizeof( OPENFILENAME );
#ifndef _WIN64
  #if defined( __NT__ ) && (_WIN32_WINNT >= 0x0500)
    if( LOBYTE( LOWORD( GetVersion() ) ) < 5 ) {
        /* Set the appropriate structure size to make this work on Windows 95. */
        of_size = OPENFILENAME_SIZE_VERSION_400;
    }
  #endif
#endif
    memset( &of, 0, of_size );
    of.lStructSize = of_size;
    of.hwndOwner = HMainWindow;
    of.lpstrFilter = (LPSTR)IEImageFilter;
    of.nFilterIndex = (long)img_type;
    of.lpstrFile = fname;
    of.nMaxFile = _MAX_PATH;
    of.lpstrFileTitle = szFileTitle;
    of.nMaxFileTitle = sizeof( szFileTitle );
    of.lpstrTitle = IESaveImageTitle;
    of.lpstrInitialDir = initialDir;
#if !defined( __NT__ )
    /* Important! Do not use hook in WIN32, you will not get the nice dialog! */
    of.lpfnHook = MakeProcInstance_OFNHOOK( SaveOFNHookProc, Instance );
    of.Flags = OFN_ENABLEHOOK;
#endif
    of.Flags |= OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY;
    ok = ( GetSaveFileName( &of ) != 0 );
#ifndef __NT__
    FreeProcInstance_OFNHOOK( of.lpfnHook );
#endif

    if( ok ) {
        ok = updateSaveFileInfo( fname );
    }
    return( ok );

} /* getSaveFName */

/*
 * saveBitmapFile - get the bitmap data and save it in fname
 */
static bool saveBitmapFile( img_node *node )
{
    BITMAPFILEHEADER    bmfh;
    BITMAPINFO          *bmi;
    long                bitmap_size;
    long                number_of_bytes;
    FILE                *fp;
    char                filename[_MAX_FNAME + _MAX_EXT];
    HDC                 hdc;
    bool                ok;

    bmi = GetDIBitmapInfo( node );
#if 0
    if( bmi->bmiHeader.biWidth > 32 ) {
        number_of_bytes = BITS_INTO_BYTES( bmi->bmiHeader.biBitCount *
            bmi->bmiHeader.biWidth, bmi->bmiHeader.biHeight );
    } else {
        number_of_bytes = BITS_TO_BYTES( bmi->bmiHeader.biBitCount *
            bmi->bmiHeader.biWidth, bmi->bmiHeader.biHeight );
    }
#else
    number_of_bytes = BITS_TO_BYTES( bmi->bmiHeader.biBitCount *
        bmi->bmiHeader.biWidth, bmi->bmiHeader.biHeight );
#endif

    bitmap_size = DIB_INFO_SIZE( bmi->bmiHeader.biBitCount );

    hdc = GetDC( NULL );
    GetDIBits( hdc, node->xor_hbitmap, 0, node->height, NULL, bmi, DIB_RGB_COLORS );
    ReleaseDC( NULL, hdc );
    if( bmi->bmiHeader.biSizeImage == 0 ) {
        bmi->bmiHeader.biSizeImage = number_of_bytes;
    } else {
        number_of_bytes = bmi->bmiHeader.biSizeImage;
    }

    bmfh.bfType = BITMAP_TYPE;
    bmfh.bfSize = sizeof( BITMAPFILEHEADER ) + bitmap_size + number_of_bytes;
    bmfh.bfReserved1 = 0;
    bmfh.bfReserved2 = 0;
    bmfh.bfOffBits = sizeof( BITMAPFILEHEADER ) + bitmap_size;

    GetFnameFromPath( node->fname, filename );
    ok = false;
    fp = fopen( node->fname, "wb" );
    if( fp != NULL ) {
        if( fseek( fp, 0L, SEEK_SET ) == 0 ) {
            if( fwrite( &bmfh, sizeof( BITMAPFILEHEADER ), 1, fp ) == 1 ) {
                if( fwrite( bmi, bitmap_size, 1, fp ) == 1 ) {
                    if( writeDataInPieces( bmi, fp, node ) ) {
                        ok = true;
                    }
                }
            }
        }
        fclose( fp );
    }
    FreeDIBitmapInfo( bmi );
    if( ok ) {
        AllowRestoreOption( node );
        SetIsSaved( node->hwnd, true );
        PrintHintTextByID( WIE_BITMAPSAVEDTO, filename );
    } else {
        WImgEditError( WIE_ERR_SAVE_FAIL, filename );
    }
    return( ok );

} /* saveBitmapFile */

/*
 * SaveBitmapToData - get the bitmap data and save into a block of memory
 */
bool SaveBitmapToData( img_node *node, BYTE **data, size_t *size )
{
    BITMAPFILEHEADER    bmfh;
    BITMAPINFO          *bmi;
    long                bitmap_size;
    long                number_of_bytes;
    HDC                 hdc;
    bool                ok;

    ok = false;
    if( data == NULL || size == NULL ) {
        return( ok );
    }

    bmi = GetDIBitmapInfo( node );
#if 0
    if( bmi->bmiHeader.biWidth > 32 ) {
        number_of_bytes = BITS_INTO_BYTES( bmi->bmiHeader.biBitCount *
            bmi->bmiHeader.biWidth, bmi->bmiHeader.biHeight );
    } else {
        number_of_bytes = BITS_TO_BYTES( bmi->bmiHeader.biBitCount *
            bmi->bmiHeader.biWidth, bmi->bmiHeader.biHeight );
    }
#else
    number_of_bytes = BITS_TO_BYTES( bmi->bmiHeader.biBitCount *
        bmi->bmiHeader.biWidth, bmi->bmiHeader.biHeight );
#endif

    bitmap_size = DIB_INFO_SIZE( bmi->bmiHeader.biBitCount );

    hdc = GetDC( NULL );
    GetDIBits( hdc, node->xor_hbitmap, 0, node->height, NULL, bmi, DIB_RGB_COLORS );
    ReleaseDC( NULL, hdc );
    if( bmi->bmiHeader.biSizeImage == 0 ) {
        bmi->bmiHeader.biSizeImage = number_of_bytes;
    } else {
        number_of_bytes = bmi->bmiHeader.biSizeImage;
    }

    bmfh.bfType = BITMAP_TYPE;
    bmfh.bfSize = sizeof( BITMAPFILEHEADER ) + bitmap_size + number_of_bytes;
    bmfh.bfReserved1 = 0;
    bmfh.bfReserved2 = 0;
    bmfh.bfOffBits = sizeof( BITMAPFILEHEADER ) + bitmap_size;

    // Make sure the bitmap can actually be malloc'd!!
    if( bmfh.bfSize <= INT_MAX ) {
        *data = MemAlloc( bmfh.bfSize );
        if( *data != NULL ) {
            ok = true;
            *size = 0;
            memcpy( *data + *size, &bmfh, sizeof( BITMAPFILEHEADER ) );
            *size += sizeof( BITMAPFILEHEADER );
            memcpy( *data + *size, bmi, bitmap_size );
            *size += bitmap_size;
            if( !writeDataInPiecesData( bmi, data, size, node ) ) {
                ok = false;
            }
        }
    }
    FreeDIBitmapInfo( bmi );

    return( ok );

} /* SaveBitmapToData */

/*
 * saveImgFile - save the image (icon or cursor) file
 *             - note that node points to the "root" of the icon nodes
 */
static bool saveImgFile( img_node *node )
{
    an_img_file         *img_file;
    an_img_resource     img_res;
    an_img              img;
    BITMAPINFOHEADER    *imginfo;
    FILE                *fp;
    long                bitmap_size;
    long                imgfile_size;
    DWORD               prevDIBsize = 0;
    DWORD               prevDIBoffset = 0;
    WORD                count;
    short               i;
    img_node            *currentimage;
    char                filename[_MAX_FNAME + _MAX_EXT];
    bool                ok;

    count = node->num_of_images;                // Will be 1 for cursors
    imgfile_size = sizeof( an_img_file ) + sizeof( an_img_resource ) * ( count - 1 );
    img_file = MemAlloc( imgfile_size );

    img_file->count = count;
    img_file->reserved = 0;
    if( node->imgtype == ICON_IMG ) {
        img_file->type = 1;
    } else {
        img_file->type = 2;
    }

    imginfo = MemAlloc( sizeof( BITMAPINFOHEADER ) * img_file->count );

    currentimage = node;
    for( i = 0; i < img_file->count; i++ ) {
        FillImageResource( &img_res, currentimage );

        if( currentimage->imgtype == CURSOR_IMG ) {
            img_res.color_count = 0;
        }

        GetBitmapInfoHeader( &imginfo[i], currentimage );
        bitmap_size = DIB_INFO_SIZE( imginfo[i].biBitCount );
#if 0
        if( imginfo[i].biWidth > 32 ) {
            img_res.DIB_size = bitmap_size +
                BITS_INTO_BYTES( imginfo[i].biWidth * imginfo[i].biBitCount,
                                 imginfo[i].biHeight ) +
                BITS_INTO_BYTES( imginfo[i].biWidth, imginfo[i].biHeight );
        } else {
            img_res.DIB_size = bitmap_size +
                BITS_TO_BYTES( imginfo[i].biWidth * imginfo[i].biBitCount,
                               imginfo[i].biHeight ) +
                BITS_TO_BYTES( imginfo[i].biWidth, imginfo[i].biHeight );
        }
#else
        img_res.DIB_size = bitmap_size +
            BITS_TO_BYTES( imginfo[i].biWidth * imginfo[i].biBitCount,
                           imginfo[i].biHeight ) +
            BITS_TO_BYTES( imginfo[i].biWidth, imginfo[i].biHeight );
#endif
        if( i == 0 ) {
            img_res.DIB_offset = sizeof( an_img_file ) + sizeof( an_img_resource ) * ( count - 1 );
        } else {
            img_res.DIB_offset = prevDIBoffset + prevDIBsize;
        }
        prevDIBsize = img_res.DIB_size;
        prevDIBoffset = img_res.DIB_offset;

        memcpy( &img_file->resources[i], &img_res, sizeof( an_img_resource ) );
        currentimage = currentimage->nexticon;
        if( currentimage == NULL ) {
            break;
        }
    }

    GetFnameFromPath( node->fname, filename );
    ok = false;
    fp = fopen( node->fname, "wb" );
    if( fp != NULL ) {
        if( fseek( fp, 0L, SEEK_SET ) == 0 ) {
            if( fwrite( img_file, imgfile_size, 1, fp ) == 1 ) {
                ok = true;
                currentimage = node;
                for( i = 0; i < img_file->count; i++ ) {
                    bitmap_size = DIB_INFO_SIZE( imginfo[i].biBitCount );
#if 0
                    if( imginfo[i].biWidth > 32 ) {
                        img.xor_size = (WORD)BITS_INTO_BYTES( imginfo[i].biWidth * imginfo[i].biBitCount, imginfo[i].biHeight );
                        img.and_size = (WORD)BITS_INTO_BYTES( imginfo[i].biWidth, imginfo[i].biHeight );
                    } else {
                        img.xor_size = (WORD)BITS_TO_BYTES( imginfo[i].biWidth * imginfo[i].biBitCount, imginfo[i].biHeight );
                        img.and_size = (WORD)BITS_TO_BYTES( imginfo[i].biWidth, imginfo[i].biHeight );
                    }
#else
                    img.xor_size = (WORD)BITS_TO_BYTES( imginfo[i].biWidth * imginfo[i].biBitCount, imginfo[i].biHeight );
                    img.and_size = (WORD)BITS_TO_BYTES( imginfo[i].biWidth, imginfo[i].biHeight );
#endif
                    img.bm = MemAlloc( bitmap_size );
                    // JAMIE
                    MemFree( img.bm );
                    img.bm = MemAlloc( bitmap_size );

                    img.xor_mask = MemAlloc( img.xor_size );
                    img.and_mask = MemAlloc( img.and_size );
                    memcpy( &img.bm->bmiHeader, &imginfo[i], sizeof( BITMAPINFOHEADER ) );

                    GetImageData( &img, currentimage );

                    /*
                     * Here's a couple of quirks:  1. we have to reverse the and_mask bits
                     * We multilply the height by 2 because this height represents the
                     * combined height of the AND mask and the XOR mask.
                     * Addendum:  Actually, we only reverse the and bits when we open
                     * AND if we store the bits in device DEPENDENT bitmap form.
                     */

                    img.bm->bmiHeader.biHeight = img.bm->bmiHeader.biHeight * 2;
                    img.bm->bmiHeader.biSizeImage = img.xor_size + img.and_size;

                    if( fwrite( img.bm, bitmap_size, 1, fp ) != 1 ) {
                        ok = false;
                    } else if( fwrite( img.xor_mask, sizeof( BYTE ), img.xor_size, fp ) != img.xor_size ) {
                        ok = false;
                    } else if( fwrite( img.and_mask, sizeof( BYTE ), img.and_size, fp ) != img.and_size ) {
                        ok = false;
                    }
                    MemFree( img.bm );
                    MemFree( img.xor_mask );
                    MemFree( img.and_mask );
                    currentimage = currentimage->nexticon;
                    if( !ok || currentimage == NULL ) {
                        break;
                    }
                }
            }
        }
        fclose( fp );
    }
    MemFree( img_file );
    MemFree( imginfo );
    if( ok ) {
        AllowRestoreOption( node );
        SetIsSaved( node->hwnd, true );
        if( node->imgtype == ICON_IMG ) {
            PrintHintTextByID( WIE_ICONSAVEDTO, filename );
        } else {
            PrintHintTextByID( WIE_CURSORSAVEDTO, filename );
        }
    } else {
        WImgEditError( WIE_ERR_SAVE_FAIL, filename );
    }
    return( ok );

} /* saveImgFile */

/*
 * getSaveImgDataLength
 */
static int getSaveImgDataLength( img_node *node, an_img_file *img_file,
                          BITMAPINFOHEADER *imginfo, long imgfile_size )
{
    img_node            *currentimage;
    int                 data_length;
    int                 i;

    data_length = imgfile_size;

    currentimage = node;
    for( i = 0; i < img_file->count; i++ ) {
        data_length += DIB_INFO_SIZE( imginfo[i].biBitCount );
#if 0
        if( imginfo[i].biWidth > 32 ) {
            data_length += (int)BITS_INTO_BYTES( imginfo[i].biWidth * imginfo[i].biBitCount, imginfo[i].biHeight );
            data_length += (int)BITS_INTO_BYTES( imginfo[i].biWidth, imginfo[i].biHeight );
        } else {
            data_length += (int)BITS_TO_BYTES( imginfo[i].biWidth * imginfo[i].biBitCount, imginfo[i].biHeight );
            data_length += (int)BITS_TO_BYTES( imginfo[i].biWidth, imginfo[i].biHeight );
        }
#else
        data_length += (int)BITS_TO_BYTES( imginfo[i].biWidth * imginfo[i].biBitCount, imginfo[i].biHeight );
        data_length += (int)BITS_TO_BYTES( imginfo[i].biWidth, imginfo[i].biHeight );
#endif
        currentimage = currentimage->nexticon;
        if( currentimage == NULL ) {
            break;
        }
    }

    return( data_length );

} /* getSaveImgDataLength */

/*
 * SaveImgToData
 */
bool SaveImgToData( img_node *node, BYTE **data, size_t *size )
{
    an_img_file         *img_file;
    an_img_resource     img_res;
    an_img              img;
    BITMAPINFOHEADER    *imginfo;
    long                bitmap_size;
    long                imgfile_size;
    DWORD               prevDIBsize = 0;
    DWORD               prevDIBoffset = 0;
    WORD                count;
    short               i;
    img_node            *currentimage;
    uint_32             data_length;
    bool                ok;

    if( data == NULL || size == NULL ) {
        return( false );
    }

    count = node->num_of_images;                // Will be 1 for cursors
    imgfile_size = sizeof( an_img_file ) + sizeof( an_img_resource ) * ( count - 1 );
    img_file = MemAlloc( imgfile_size );

    img_file->count = count;
    img_file->reserved = 0;
    if( node->imgtype == ICON_IMG ) {
        img_file->type = 1;
    } else {
        img_file->type = 2;
    }

    imginfo = MemAlloc( sizeof( BITMAPINFOHEADER ) * img_file->count );

    currentimage = node;
    for( i = 0; i < img_file->count; i++ ) {
        FillImageResource( &img_res, currentimage );

        if( currentimage->imgtype == CURSOR_IMG ) {
            img_res.color_count = 0;
        }

        GetBitmapInfoHeader( &imginfo[i], currentimage );
        bitmap_size = DIB_INFO_SIZE( imginfo[i].biBitCount );
#if 0
        if( imginfo[i].biWidth > 32 ) {
            img_res.DIB_size = bitmap_size +
                BITS_INTO_BYTES( imginfo[i].biWidth * imginfo[i].biBitCount, imginfo[i].biHeight ) +
                BITS_INTO_BYTES( imginfo[i].biWidth, imginfo[i].biHeight );
        } else {
            img_res.DIB_size = bitmap_size +
                BITS_TO_BYTES( imginfo[i].biWidth * imginfo[i].biBitCount, imginfo[i].biHeight ) +
                BITS_TO_BYTES( imginfo[i].biWidth, imginfo[i].biHeight );
        }
#else
        img_res.DIB_size = bitmap_size +
            BITS_TO_BYTES( imginfo[i].biWidth * imginfo[i].biBitCount, imginfo[i].biHeight ) +
            BITS_TO_BYTES( imginfo[i].biWidth, imginfo[i].biHeight );
#endif
        if( i == 0 ) {
            img_res.DIB_offset = sizeof( an_img_file ) + sizeof( an_img_resource ) * ( count - 1 );
        } else {
            img_res.DIB_offset = prevDIBoffset + prevDIBsize;
        }
        prevDIBsize = img_res.DIB_size;
        prevDIBoffset = img_res.DIB_offset;

        memcpy( &img_file->resources[i], &img_res, sizeof( an_img_resource ) );
        currentimage = currentimage->nexticon;
        if( currentimage == NULL ) {
            break;
        }
    }

    ok = false;
    data_length = getSaveImgDataLength( node, img_file, imginfo, imgfile_size );
    // Make sure the bitmap can actually be malloc'd!!
    if( data_length <= INT_MAX ) {
        // allocate the data for the image
        *data = MemAlloc( data_length );
        if( *data != NULL ) {
            ok = true;
            *size = 0;
            memcpy( *data + *size, img_file, imgfile_size );
            *size += imgfile_size;
            currentimage = node;
            for( i = 0; i < img_file->count; i++ ) {
                bitmap_size = DIB_INFO_SIZE( imginfo[i].biBitCount );
                img.bm = MemAlloc( bitmap_size );
#if 0
                if( imginfo[i].biWidth > 32 ) {
                    img.xor_size = (WORD)BITS_INTO_BYTES( imginfo[i].biWidth * imginfo[i].biBitCount, imginfo[i].biHeight );
                    img.and_size = (WORD)BITS_INTO_BYTES( imginfo[i].biWidth, imginfo[i].biHeight );
                } else {
                    img.xor_size = (WORD)BITS_TO_BYTES( imginfo[i].biWidth * imginfo[i].biBitCount, imginfo[i].biHeight );
                    img.and_size = (WORD)BITS_TO_BYTES( imginfo[i].biWidth, imginfo[i].biHeight );
                }
#else
                img.xor_size = (WORD)BITS_TO_BYTES( imginfo[i].biWidth * imginfo[i].biBitCount, imginfo[i].biHeight );
                img.and_size = (WORD)BITS_TO_BYTES( imginfo[i].biWidth, imginfo[i].biHeight );
#endif
                img.xor_mask = MemAlloc( img.xor_size );
                img.and_mask = MemAlloc( img.and_size );
                memcpy( &img.bm->bmiHeader, &imginfo[i], sizeof( BITMAPINFOHEADER ) );

                GetImageData( &img, currentimage );

                /*
                 * Here's a couple of quirks:  1. we have to reverse the and_mask bits
                 * We multilply the height by 2 because this height represents the
                 * combined height of the AND mask and the XOR mask.
                 * Addendum:  Actually, we only reverse the and bits when we open
                 * AND if we store the bits in device DEPENDENT bitmap form.
                 */

                img.bm->bmiHeader.biHeight = img.bm->bmiHeader.biHeight * 2;
                img.bm->bmiHeader.biSizeImage = img.xor_size + img.and_size;

                memcpy( *data + *size, img.bm, bitmap_size );
                *size += bitmap_size;

                memcpy( *data + *size, img.xor_mask, img.xor_size );
                *size += img.xor_size;

                memcpy( *data + *size, img.and_mask, img.and_size );
                *size += img.and_size;

                MemFree( img.bm );
                MemFree( img.xor_mask );
                MemFree( img.and_mask );
                currentimage = currentimage->nexticon;
                if( currentimage == NULL ) {
                    break;
                }
            }
        }
    }
    MemFree( img_file );
    MemFree( imginfo );
    if( ok ) {
        AllowRestoreOption( node );
        SetIsSaved( node->hwnd, true );
    }
    return( ok );

} /* SaveImgToData */

/*
 * createNewImageLNODE
 */
static bool createNewImageLNODE( img_node *node, uint_16 type )
{
    pgroup2             pg;
    WResID              *tname;
    WResID              *rname;
    WResLangType        lang;
    bool                dup;
    bool                ok;

    tname = NULL;
    rname = NULL;
    lang.lang = DEF_LANG;
    lang.sublang = DEF_SUBLANG;
    ok = (node != NULL && node->wrinfo != NULL);

    if( ok ) {
        if( node->wrinfo->dir == NULL ) {
            node->wrinfo->dir = WResInitDir();
            ok = (node->wrinfo->dir != NULL);
        }
    }

    if( ok ) {
        tname = WResIDFromNum( type );
        ok = (tname != NULL);
    }

    if( ok ) {
        _splitpath2( node->fname, pg.buffer, NULL, NULL, &pg.fname, NULL );
        rname = WResIDFromStr( pg.fname );
        ok = (rname != NULL);
    }

    if( ok ) {
        ok = !WResAddResource( tname, rname, DEF_MEMFLAGS, 0, 0,
                               node->wrinfo->dir, &lang, &dup ) && !dup;
    }

    if( ok ) {
        node->lnode = WRFindLangNode( node->wrinfo->dir, tname, rname, &lang );
        ok = (node->lnode != NULL);
    }

    if( tname != NULL ) {
        MemFree( tname );
    }

    if( rname != NULL ) {
        MemFree( rname );
    }

    return( ok );

} /* createNewImageLNODE */

/*
 * saveResourceFile
 */
static bool saveResourceFile( img_node *node )
{
    BYTE            *data;
    size_t          size = 0;
    uint_16         type;
    WRFileType      save_type = 0;
    bool            info_created;
    bool            was32bit;
    bool            is32bit;
    bool            ok;
    HELPFUNC        hcb;

    info_created = false;
    data = NULL;
    type = 0;
    ok = (node != NULL);

    if( ok ) {
        switch( node->imgtype ) {
        case BITMAP_IMG:
            type = RESOURCE2INT( RT_BITMAP );
            break;
        case ICON_IMG:
            type = RESOURCE2INT( RT_GROUP_ICON );
            break;
        case CURSOR_IMG:
            type = RESOURCE2INT( RT_GROUP_CURSOR );
            break;
        default:
            ok = false;
            break;
        }
    }

    if( ok ) {
        if( node->wrinfo == NULL ) {
            node->wrinfo = WRAllocWRInfo();
            info_created = true;
            ok = (node->wrinfo != NULL);
        }
    }

    if( ok ) {
        if( info_created ) {
            ok = createNewImageLNODE( node, type );
        }
        if( ok ) {
            ok = ( node->lnode != NULL );
        }
    }

    if( ok ) {
        was32bit = WRIs32Bit( node->wrinfo->file_type );
        for( ;; ) {
            hcb = MakeProcInstance_HELP( IEHelpCallBack, Instance );
            save_type = WRSelectFileType( HMainWindow, node->fname, was32bit, true, hcb );
            FreeProcInstance_HELP( hcb );
            is32bit = WRIs32Bit( save_type );
            if( was32bit ) {
                if( is32bit ) {
                    break;
                }
                IEDisplayErrorMsg( WIE_SAVEIMAGETITLE2, WIE_NOSAVE32TO16, MB_ICONSTOP | MB_OK );
            } else {
                if( !is32bit ) {
                    break;
                }
                IEDisplayErrorMsg( WIE_SAVEIMAGETITLE2, WIE_NOSAVE16TO32, MB_ICONSTOP | MB_OK );
            }
        }
        ok = (save_type != WR_DONT_KNOW);
    }

    // get rid of the old image resources for icons or cursors
    if( ok ) {
        if( type != RESOURCE2INT( RT_BITMAP ) ) {
            ok = WRDeleteGroupImages( node->wrinfo, node->lnode, type );
        }
    }

    if( ok ) {
        // delete the data image of the resource
        if( node->lnode->data != NULL ) {
            MemFree( node->lnode->data );
            node->lnode->data = NULL;
        }
        // create the data for this resource
        switch( node->imgtype ) {
        case BITMAP_IMG:
            ok = SaveBitmapToData( node, &data, &size );
            break;
        case ICON_IMG:
        case CURSOR_IMG:
            ok = SaveImgToData( node, &data, &size );
            break;
        default:
            ok = false;
            break;
        }
    }

    // massage and add the data back into the resource
    if( ok ) {
        switch( node->imgtype ) {
        case BITMAP_IMG:
            ok = WRStripBitmapFileHeader( &data, &size );
            if( ok ) {
                node->lnode->data = data;
                node->lnode->Info.Length = size;
            }
            break;
        case ICON_IMG:
            ok = WRCreateIconEntries( node->wrinfo, node->lnode, data, size );
            break;
        case CURSOR_IMG:
            ok = WRCreateCursorEntries( node->wrinfo, node->lnode, data, size );
            break;
        default:
            ok = false;
            break;
        }
    }

    if( ok ) {
        node->wrinfo->save_type = save_type;
        node->wrinfo->save_name = node->fname;
        ok = WRSaveResource( node->wrinfo, true );
        node->wrinfo->save_name = NULL;
    }

    if( ok ) {
        AllowRestoreOption( node );
        SetIsSaved( node->hwnd, true );
        PrintHintTextByID( WIE_IMAGESAVEDTO, node->fname );
    }

    if( type != RESOURCE2INT( RT_BITMAP ) ) {
        if( data != NULL ) {
            MemFree( data );
        }
    }

    return( ok );

} /* saveResourceFile */

/*
 * SaveFileFromNode - save the image file (bitmap, cursor, or icon)
 */
bool SaveFileFromNode( img_node *node, int how )
{
    img_node    *rootnode;
    char        new_name[_MAX_PATH];
    pgroup2     pg;
    bool        ok;
    image_type  img_type;

    ok = ( node != NULL );
    if( ok ) {
        rootnode = GetImageNode( node->hwnd );
        ok = ( rootnode != NULL );
    }
    if( ok ) {
        if( strnicmp( rootnode->fname, IEImageUntitled, strlen( IEImageUntitled ) ) == 0 ) {
            how = SB_SAVE_AS;
        }

        if( how == SB_SAVE_AS ) {
            ok = getSaveFName( new_name, rootnode->imgtype );
            if( ok ) {
                for( node = rootnode; node != NULL; node = node->nexticon ) {
                    strcpy( node->fname, new_name );
                }
            }
        }
    }
    if( ok ) {
        CheckForExt( rootnode );

        _splitpath2( rootnode->fname, pg.buffer, NULL, NULL, NULL, &pg.ext );
        img_type = GetImageFileType( pg.ext, true );
        if( img_type == RESOURCE_IMG ) {
            return( saveResourceFile( rootnode ) );
        }
        switch( rootnode->imgtype ) {
        case BITMAP_IMG:
            ok = saveBitmapFile( rootnode );
            break;
        case ICON_IMG:
        case CURSOR_IMG:
            ok = saveImgFile( rootnode );
            break;
        }
    }
    return( ok );

} /* SaveFileFromNode */

/*
 * SaveFile - save the image file (bitmap, cursor, or icon)
 */
bool SaveFile( int how )
{
    img_node    *node;

    node = GetCurrentNode();

    return( SaveFileFromNode( node, how ) );

} /* SaveFile */

/*
 * getSavePalName - get the name of the palette file to be saved
 */
static bool getSavePalName( char *fname )
{
    static OPENFILENAME of;
    char                szFileTitle[_MAX_PATH];
    bool                ok;
    long                of_size;

    fname[0] = '\0';

    of_size = sizeof( OPENFILENAME );
#ifndef _WIN64
  #if defined( __NT__ ) && (_WIN32_WINNT >= 0x0500)
    if( LOBYTE( LOWORD( GetVersion() ) ) < 5 ) {
        /* Set the appropriate structure size to make this work on Windows 95. */
        of_size = OPENFILENAME_SIZE_VERSION_400;
    }
  #endif
#endif
    memset( &of, 0, of_size );
    of.lStructSize = of_size;
    of.hwndOwner = HMainWindow;
    of.lpstrFilter = (LPSTR)IEPaletteFilter;
    of.nFilterIndex = 0L;
    of.lpstrFile = fname;
    of.nMaxFile = _MAX_PATH;
    of.lpstrFileTitle = szFileTitle;
    of.nMaxFileTitle = sizeof( szFileTitle );
    of.lpstrTitle = IESavePaletteTitle;
    of.lpstrInitialDir = initialDir;
    of.Flags = OFN_SHOWHELP | OFN_OVERWRITEPROMPT;
#ifndef __NT__
    of.Flags |= OFN_ENABLEHOOK;
    of.lpfnHook = MakeProcInstance_OFNHOOK( SaveOFNHookProc, Instance );
#endif
    ok = ( GetSaveFileName( &of ) != 0 );
#ifndef __NT__
    FreeProcInstance_OFNHOOK( of.lpfnHook );
#endif
    return( ok );

} /* getSavePalName */

/*
 * SaveColorPalette - save the current color palette
 */
bool SaveColorPalette( void )
{
    a_pal_file          pal_file;
    FILE                *fp;
    char                fname[_MAX_PATH];
    char                filename[_MAX_FNAME + _MAX_EXT];
    bool                ok;

    if( !GetPaletteFile( &pal_file ) ) {
        WImgEditError( WIE_ERR_PALETTE_NOT16, NULL );
        return( true );                     // Just return ... no error
    }

    ok = getSavePalName( fname );
    if( ok ) {
        CheckForPalExt( fname );

        GetFnameFromPath( fname, filename );
        ok = false;
        fp = fopen( fname, "wb" );
        if( fp != NULL ) {
            if( fseek( fp, 0L, SEEK_SET ) == 0 ) {
                ok = ( fwrite( &pal_file, sizeof( a_pal_file ), 1, fp ) == 1 );
            }
            fclose( fp );
        }
    }
    if( ok ) {
        PrintHintTextByID( WIE_PALETTESAVEDTO, filename );
    } else {
        WImgEditError( WIE_ERR_SAVE_FAIL, filename );
    }
    return( ok );

} /* SaveColorPalette */

/*
 * SetInitialSaveDir - set the initial directory for the open filename
 */
void SetInitialSaveDir( char *new_dir )
{
    if( new_dir != NULL ) {
        strcpy( initialDir, new_dir );
    } else {
        strcpy( initialDir, "" );
    }

} /* SetInitialSaveDir */

/*
 * GetInitSaveDir - get the directory which we want to use as our initial one
 *                  next time we run
 */
char *GetInitSaveDir( void )
{
    return( initialDir );

} /* GetInitSaveDir */
