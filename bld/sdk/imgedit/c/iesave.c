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


#include "imgedit.h"
#include <commdlg.h>
#include <dos.h>
#include <malloc.h>
#include <math.h>
#include <limits.h>
#include "wrbitmap.h"
#include "wricon.h"
#include "wrselft.h"
#include "iemem.h"


#define DEF_MEMFLAGS    (MEMFLAG_MOVEABLE | MEMFLAG_PURE)
#define SCANLINE_SIZE   32
#define MAX_CHUNK       32768

static char     initialDir[_MAX_PATH];

/*
 * writeDataInPieces - writes the XOR data for the bitmap in chunks
 */
static BOOL writeDataInPieces( BITMAPINFO *bmi, FILE *fp, img_node *node )
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
        GetDIBits( memdc, node->hxorbitmap, start, num_lines, buffer, bmi, DIB_RGB_COLORS );
        fwrite( buffer, sizeof( BYTE ), chunk_size, fp );
        scanline_count -= num_lines;
        start += num_lines;
        byte_count -= chunk_size;
    }
    GetDIBits( memdc, node->hxorbitmap, start, scanline_count, buffer, bmi, DIB_RGB_COLORS );
    fwrite( buffer, sizeof( BYTE ), one_scanline_size * scanline_count, fp );
    MemFree( buffer );
    DeleteDC( memdc );
    return( TRUE );

} /* writeDataInPieces */

/*
 * writeDataInPiecesData
 */
static BOOL writeDataInPiecesData( BITMAPINFO *bmi, BYTE **data,
                                   uint_32 *size, img_node *node )
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
        return( FALSE );
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
        GetDIBits( memdc, node->hxorbitmap, start, num_lines, *data + *size,
                   bmi, DIB_RGB_COLORS );
        *size += chunk_size;
        scanline_count -= num_lines;
        start += num_lines;
        byte_count -= chunk_size;
    }
    GetDIBits( memdc, node->hxorbitmap, start, scanline_count, *data + *size,
               bmi, DIB_RGB_COLORS );
    *size += scanline_count * one_scanline_size;
    DeleteDC( memdc );
    return( TRUE );

} /* writeDataInPiecesData */

/*
 * checkForExt - if no extension is given, use the default for the given type
 */
static void checkForExt( img_node *node )
{
    char        drive[_MAX_PATH];
    char        dir[_MAX_DIR];
    char        fname[_MAX_FNAME];
    char        ext[_MAX_EXT];
    char        *fullpath;
    img_node    *next_icon;
    char        default_ext[3][4] = { "bmp", "ico", "cur" };

    next_icon = node;
    while( next_icon != NULL ) {
        fullpath = next_icon->fname;
        _splitpath( fullpath, drive, dir, fname, ext );

        if( strlen( ext ) > 1 ) {
            return;
        }

        if( fullpath[strlen( fullpath ) - 1] != '.' ) {
            strcat( fullpath, "." );
        }
        strcat( fullpath, default_ext[next_icon->imgtype - 1] );
        next_icon = next_icon->nexticon;
    }

} /* checkForExt */

/*
 * checkForPalExt - if no extension is given, use the default palette
 *                  extension of .pal.
 */
static void checkForPalExt( char *filename )
{
    char        ext[_MAX_EXT];

    _splitpath( filename, NULL, NULL, NULL, ext );

    if( strlen( ext ) > 1 ) {
        return;
    }

    if( filename[strlen( filename ) - 1] != '.' ) {
        strcat( filename, "." );
    }
    strcat( filename, "pal" );

} /* checkForPalExt */

/*
 * SaveHook - hook used called by common dialog for 3D controls
 */
BOOL CALLBACK SaveHook( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam )
{
    wparam = wparam;
    lparam = lparam;
    hwnd = hwnd;
    switch( msg ) {
    case WM_INITDIALOG:
        // We must call this to subclass the directory listbox even
        // if the app calls Ctl3dAutoSubclass (commdlg bug).
#if defined( __NT__ )
        // Only do it if NOT new shell.
        if( LOBYTE( LOWORD( GetVersion() ) ) < 4 ) {
#endif
           IECtl3dSubclassDlgAll( hwnd );
#if defined( __NT__ )
        }
#endif
        return( TRUE );
    }
    return( FALSE );

} /* SaveHook */

/*
 * getSaveFName - get the name of the file to be saved
 */
static BOOL getSaveFName( char *fname, int imgtype )
{
    static OPENFILENAME of;
    char                szFileTitle[_MAX_PATH];
    char                drive[_MAX_DRIVE];
    char                path[_MAX_PATH];
    BOOL                ret_val;
    long                of_size;
#if defined( __NT__ ) && (WINVER >= 0x0500) && (_WIN32_WINNT >= 0x0500)
    OSVERSIONINFO       os_info;
#endif

    of_size = sizeof( OPENFILENAME );
#if defined( __NT__ ) && (WINVER >= 0x0500) && (_WIN32_WINNT >= 0x0500)
    os_info.dwOSVersionInfoSize = sizeof( OSVERSIONINFO );
    GetVersionEx( &os_info );
    if( os_info.dwMajorVersion < 5 ) {
        /* Set the appropriate structure size to make this work on Windows 95. */
        of_size = OPENFILENAME_SIZE_VERSION_400;
    }
#endif

    fname[0] = '\0';
    memset( &of, 0, of_size );
    of.lStructSize = of_size;
    of.hwndOwner = HMainWindow;
    of.lpstrFilter = (LPSTR)IEImageFilter;
    of.nFilterIndex = (long)imgtype;
    of.lpstrFile = fname;
    of.nMaxFile = _MAX_PATH;
    of.lpstrFileTitle = szFileTitle;
    of.nMaxFileTitle = sizeof( szFileTitle );
    of.lpstrTitle = IESaveImageTitle;
    of.lpstrInitialDir = initialDir;
#if !defined( __NT__ )
    /* Important! Do not use hook in WIN32, you will not get the nice dialog! */
    of.lpfnHook = (LPOFNHOOKPROC)MakeProcInstance( (FARPROC)SaveHook, Instance );
    of.Flags = OFN_ENABLEHOOK;
#endif
    of.Flags |= OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY;
    ret_val = GetSaveFileName( &of );
#ifndef __NT__
    FreeProcInstance( (FARPROC)of.lpfnHook );
#endif

    if( ret_val ) {
        _splitpath( fname, drive, path, NULL, NULL );
        strcpy( initialDir, drive );
        strcat( initialDir, path );
        initialDir[strlen( initialDir ) - 1] = '\0';
    }
    return( ret_val );

} /* getSaveFName */

/*
 * saveBitmapFile - get the bitmap data and save it in fname
 */
static BOOL saveBitmapFile( img_node *node )
{
    BITMAPFILEHEADER    bmfh;
    BITMAPINFO          *bmi;
    long                bitmap_size;
    long                number_of_bytes;
    FILE                *fp;
    char                filename[_MAX_FNAME + _MAX_EXT];
    HDC                 hdc;

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
    GetDIBits( hdc, node->hxorbitmap, 0, node->height, NULL, bmi, DIB_RGB_COLORS );
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
    fp = fopen( node->fname, "wb" );
    if( fp == NULL ) {
        WImgEditError( WIE_ERR_SAVE_FAIL, filename );
        return( FALSE );
    }

    if( fseek( fp, 0L, SEEK_SET ) ) {
        fclose( fp );
        FreeDIBitmapInfo( bmi );
        WImgEditError( WIE_ERR_SAVE_FAIL, filename );
        return( FALSE );
    }

    if( fwrite( &bmfh, sizeof( BITMAPFILEHEADER ), 1, fp ) != 1 ) {
        fclose( fp );
        FreeDIBitmapInfo( bmi );
        WImgEditError( WIE_ERR_SAVE_FAIL, filename );
        return( FALSE );
    }

    if( fwrite( bmi, bitmap_size, 1, fp ) != 1 ) {
        fclose( fp );
        FreeDIBitmapInfo( bmi );
        WImgEditError( WIE_ERR_SAVE_FAIL, filename );
        return( FALSE );
    }

    if( !writeDataInPieces( bmi, fp, node ) ) {
        fclose( fp );
        FreeDIBitmapInfo( bmi );
        WImgEditError( WIE_ERR_SAVE_FAIL, filename );
        return( FALSE );
    }

    fclose( fp );
    FreeDIBitmapInfo( bmi );

    AllowRestoreOption( node );
    SetIsSaved( node->hwnd, TRUE );
    PrintHintTextByID( WIE_BITMAPSAVEDTO, filename );
    return( TRUE );

} /* saveBitmapFile */

/*
 * SaveBitmapToData - get the bitmap data and save into a block of memory
 */
BOOL SaveBitmapToData( img_node *node, BYTE **data, uint_32 *size )
{
    BITMAPFILEHEADER    bmfh;
    BITMAPINFO          *bmi;
    long                bitmap_size;
    long                number_of_bytes;
    HDC                 hdc;

    if( data == NULL || size == NULL ) {
        return( FALSE );
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
    GetDIBits( hdc, node->hxorbitmap, 0, node->height, NULL, bmi, DIB_RGB_COLORS );
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
    if( bmfh.bfSize > INT_MAX ) {
        FreeDIBitmapInfo( bmi );
        return( FALSE );
    }

    *data = MemAlloc( bmfh.bfSize );
    if( *data == NULL ) {
        FreeDIBitmapInfo( bmi );
        return( FALSE );
    }
    *size = 0;

    memcpy( *data + *size, &bmfh, sizeof( BITMAPFILEHEADER ) );
    *size += sizeof( BITMAPFILEHEADER );

    memcpy( *data + *size, bmi, bitmap_size );
    *size += bitmap_size;

    if( !writeDataInPiecesData( bmi, data, size, node ) ) {
        FreeDIBitmapInfo( bmi );
        return( FALSE );
    }

    FreeDIBitmapInfo( bmi );

    return( TRUE );

} /* SaveBitmapToData */

/*
 * saveImgFile - save the image (icon or cursor) file
 *             - note that node points to the "root" of the icon nodes
 */
static BOOL saveImgFile( img_node *node )
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

    count = node->num_of_images;                // Will be 1 for cursors
    imgfile_size = sizeof( an_img_file ) + sizeof( an_img_resource ) * (count - 1);
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
        if( currentimage == NULL ) {
            break;
        }
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
            img_res.DIB_offset = sizeof( an_img_file ) +
                                 sizeof( an_img_resource ) * (count - 1);
        } else {
            img_res.DIB_offset = prevDIBoffset + prevDIBsize;
        }
        prevDIBsize = img_res.DIB_size;
        prevDIBoffset = img_res.DIB_offset;

        memcpy( &img_file->resources[i], &img_res, sizeof( an_img_resource ) );
        currentimage = currentimage->nexticon;
    }

    GetFnameFromPath( node->fname, filename );
    fp = fopen( node->fname, "wb" );
    if( fp == NULL ) {
        MemFree( img_file );
        MemFree( imginfo );
        WImgEditError( WIE_ERR_SAVE_FAIL, filename );
        return( FALSE );
    }
    if( fseek( fp, 0L, SEEK_SET ) ) {
        MemFree( img_file );
        MemFree( imginfo );
        fclose( fp );
        WImgEditError( WIE_ERR_SAVE_FAIL, filename );
        return( FALSE );
    }
    if( fwrite( img_file, imgfile_size, 1, fp ) != 1 ) {
        MemFree( img_file );
        MemFree( imginfo );
        fclose( fp );
        WImgEditError( WIE_ERR_SAVE_FAIL, filename );
        return( FALSE );
    }

    currentimage = node;
    for( i = 0; i < img_file->count; i++ ) {
        if( currentimage == NULL ) {
            break;
        }
        bitmap_size = DIB_INFO_SIZE( imginfo[i].biBitCount );
#if 0
        if( imginfo[i].biWidth > 32 ) {
            img.xor_size = (WORD)BITS_INTO_BYTES( imginfo[i].biWidth * imginfo[i].biBitCount,
                                            imginfo[i].biHeight );
            img.and_size = (WORD)BITS_INTO_BYTES( imginfo[i].biWidth, imginfo[i].biHeight );
        } else {
            img.xor_size = (WORD)BITS_TO_BYTES( imginfo[i].biWidth * imginfo[i].biBitCount,
                                          imginfo[i].biHeight );
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
            MemFree( img_file );
            MemFree( imginfo );
            fclose( fp );
            WImgEditError( WIE_ERR_SAVE_FAIL, filename );
            return( FALSE );
        }
        if( fwrite( img.xor_mask, sizeof( BYTE ), img.xor_size, fp ) != img.xor_size ) {
            MemFree( img_file );
            MemFree( imginfo );
            fclose( fp );
            WImgEditError( WIE_ERR_SAVE_FAIL, filename );
            return( FALSE );
        }
        if( fwrite( img.and_mask, sizeof( BYTE ), img.and_size, fp ) != img.and_size ) {
            MemFree( img_file );
            MemFree( imginfo );
            fclose( fp );
            WImgEditError( WIE_ERR_SAVE_FAIL, filename );
            return( FALSE );
        }
        MemFree( img.bm );
        MemFree( img.xor_mask );
        MemFree( img.and_mask );
        currentimage = currentimage->nexticon;
    }

    fclose( fp );
    MemFree( img_file );
    MemFree( imginfo );

    AllowRestoreOption( node );
    SetIsSaved( node->hwnd, TRUE );

    if( node->imgtype == ICON_IMG ) {
        PrintHintTextByID( WIE_ICONSAVEDTO, filename );
    } else {
        PrintHintTextByID( WIE_CURSORSAVEDTO, filename );
    }
    return( TRUE );

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
        if( currentimage == NULL ) {
            break;
        }
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
    }

    return( data_length );

} /* getSaveImgDataLength */

/*
 * SaveImgToData
 */
BOOL SaveImgToData( img_node *node, BYTE **data, uint_32 *size )
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

    if( data == NULL || size == NULL ) {
        return( FALSE );
    }

    count = node->num_of_images;                // Will be 1 for cursors
    imgfile_size = sizeof( an_img_file ) + sizeof( an_img_resource ) * (count - 1);
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
        if( currentimage == NULL ) {
            break;
        }
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
            img_res.DIB_offset = sizeof( an_img_file ) +
                                 sizeof( an_img_resource ) * (count - 1);
        } else {
            img_res.DIB_offset = prevDIBoffset + prevDIBsize;
        }
        prevDIBsize = img_res.DIB_size;
        prevDIBoffset = img_res.DIB_offset;

        memcpy( &img_file->resources[i], &img_res, sizeof( an_img_resource ) );
        currentimage = currentimage->nexticon;
    }

    data_length = getSaveImgDataLength( node, img_file, imginfo, imgfile_size );
    // Make sure the bitmap can actually be malloc'd!!
    if( data_length > INT_MAX ) {
        MemFree( img_file );
        MemFree( imginfo );
        return( FALSE );
    }

    // allocate the data for the image
    *data = MemAlloc( data_length );
    if( *data == NULL ) {
        MemFree( img_file );
        MemFree( imginfo );
        return( FALSE );
    }
    *size = 0;

    memcpy( *data + *size, img_file, imgfile_size );
    *size += imgfile_size;

    currentimage = node;
    for( i = 0; i < img_file->count; i++ ) {
        if( currentimage == NULL ) {
            break;
        }
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
    }

    MemFree( img_file );
    MemFree( imginfo );

    AllowRestoreOption( node );
    SetIsSaved( node->hwnd, TRUE );

    return( TRUE );

} /* SaveImgToData */

/*
 * createNewImageLNODE
 */
static bool createNewImageLNODE( img_node *node, uint_16 type )
{
    char                fn[_MAX_FNAME];
    WResID              *tname = NULL;
    WResID              *rname = NULL;
    WResLangType        lang;
    bool                dup;
    bool                ok;

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
        _splitpath( node->fname, NULL, NULL, fn, NULL );
        rname = WResIDFromStr( fn );
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
    BYTE        *data;
    uint_32     size = 0;
    uint_16     type;
    WRFileType  save_type = 0;
    BOOL        info_created;
    bool        was32bit;
    bool        is32bit;
    bool        ok;
    WPI_PROC    cb;

    info_created = FALSE;
    data = NULL;
    type = 0;
    ok = (node != NULL);

    if( ok ) {
        switch( node->imgtype ) {
        case BITMAP_IMG:
            type = (uint_16)(pointer_int)RT_BITMAP;
            break;
        case ICON_IMG:
            type = (uint_16)(pointer_int)RT_GROUP_ICON;
            break;
        case CURSOR_IMG:
            type = (uint_16)(pointer_int)RT_GROUP_CURSOR;
            break;
        default:
            ok = false;
            break;
        }
    }

    if( ok ) {
        if( node->wrinfo == NULL ) {
            node->wrinfo = WRAllocWRInfo();
            info_created = TRUE;
            ok = (node->wrinfo != NULL);
        }
    }

    if( ok ) {
        if( info_created ) {
            ok = createNewImageLNODE( node, type );
        }
        ok = (ok && node->lnode != NULL);
    }

    if( ok ) {
        was32bit = WRIs32Bit( node->wrinfo->file_type );
        for( ;; ) {
            cb = _wpi_makeprocinstance( (WPI_PROC)IEHelpCallBack, Instance );
            save_type = WRSelectFileType( HMainWindow, node->fname, was32bit, TRUE, cb );
            _wpi_freeprocinstance( cb );
            is32bit = WRIs32Bit( save_type );
            if( was32bit ) {
                if( is32bit ) {
                    break;
                } else {
                    IEDisplayErrorMsg( WIE_SAVEIMAGETITLE2, WIE_NOSAVE32TO16,
                                       MB_ICONSTOP | MB_OK );
                }
            } else {
                if( is32bit ) {
                    IEDisplayErrorMsg( WIE_SAVEIMAGETITLE2, WIE_NOSAVE16TO32,
                                       MB_ICONSTOP | MB_OK );
                } else {
                    break;
                }
            }
        }
        ok = (save_type != WR_DONT_KNOW);
    }

    // get rid of the old image resources for icons or cursors
    if( ok ) {
        if( type != (uint_16)(pointer_int)RT_BITMAP ) {
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
        SetIsSaved( node->hwnd, TRUE );
        PrintHintTextByID( WIE_IMAGESAVEDTO, node->fname );
    }

    if( type != (uint_16)(pointer_int)RT_BITMAP ) {
        if( data != NULL ) {
            MemFree( data );
        }
    }

    return( ok );

} /* saveResourceFile */

/*
 * SaveFileFromNode - save the image file (bitmap, cursor, or icon)
 */
BOOL SaveFileFromNode( img_node *node, int how )
{
    img_node    *rootnode;
    char        new_name[_MAX_PATH];
    char        ext[_MAX_EXT];

    if( node == NULL ) {
        return( FALSE );
    }
    rootnode = GetImageNode( node->hwnd );

    if( rootnode == NULL ) {
        return( FALSE );
    }

    if( strnicmp( rootnode->fname, IEImageUntitled, strlen( IEImageUntitled ) ) == 0 ) {
        how = SB_SAVE_AS;
    }

    if( how == SB_SAVE_AS ) {
        if( !getSaveFName( new_name, rootnode->imgtype ) ) {
            return( FALSE );
        }
        node = rootnode;
        while( node != NULL ) {
            strcpy( node->fname, new_name );
            node = node->nexticon;
        }
    }

    checkForExt( rootnode );

    _splitpath( rootnode->fname, NULL, NULL, NULL, ext );
    if( !stricmp( ext, ".res" ) || !stricmp( ext, ".exe" ) ||
        !stricmp( ext, ".dll" ) ) {
        return( saveResourceFile( rootnode ) );
    }

    switch( rootnode->imgtype ) {
    case BITMAP_IMG:
        if( !saveBitmapFile( rootnode ) ) {
            return( FALSE );
        }
        break;

    case ICON_IMG:
    case CURSOR_IMG:
        if( !saveImgFile( rootnode ) ) {
            return( FALSE );
        }
        break;

    default:
        return( FALSE );
    }
    return( TRUE );

} /* SaveFileFromNode */

/*
 * SaveFile - save the image file (bitmap, cursor, or icon)
 */
BOOL SaveFile( int how )
{
    img_node    *node;

    node = GetCurrentNode();

    return( SaveFileFromNode( node, how ) );

} /* SaveFile */

/*
 * getSavePalName - get the name of the palette file to be saved
 */
static BOOL getSavePalName( char *fname )
{
    static OPENFILENAME of;
    char                szFileTitle[_MAX_PATH];
    int                 rc;
    long                of_size;
#if defined( __NT__ ) && (WINVER >= 0x0500) && (_WIN32_WINNT >= 0x0500)
    OSVERSIONINFO       os_info;
#endif

    of_size = sizeof( OPENFILENAME );
#if defined( __NT__ ) && (WINVER >= 0x0500) && (_WIN32_WINNT >= 0x0500)
    os_info.dwOSVersionInfoSize = sizeof( OSVERSIONINFO );
    GetVersionEx( &os_info );
    if( os_info.dwMajorVersion < 5 ) {
        /* Set the appropriate structure size to make this work on Windows 95. */
        of_size = OPENFILENAME_SIZE_VERSION_400;
    }
#endif

    fname[0] = '\0';
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
#if !defined( __NT__ ) 
    of.Flags |= OFN_ENABLEHOOK;
    of.lpfnHook = (LPOFNHOOKPROC)MakeProcInstance( (FARPROC)SaveHook, Instance );
#endif
    rc = GetSaveFileName( &of );
#ifndef __NT__
    FreeProcInstance( (FARPROC)of.lpfnHook );
#endif
    return( rc );

} /* getSavePalName */

/*
 * SaveColorPalette - save the current color palette
 */
BOOL SaveColorPalette( void )
{
    a_pal_file          pal_file;
    FILE                *fp;
    char                fname[_MAX_PATH];
    char                filename[_MAX_FNAME + _MAX_EXT];

    if( !GetPaletteFile( &pal_file ) ) {
        WImgEditError( WIE_ERR_PALETTE_NOT16, NULL );
        return( TRUE );                   // Just return ... no error
    }

    if( !getSavePalName( fname ) ) {
        return( FALSE );
    }
    checkForPalExt( fname );

    GetFnameFromPath( fname, filename );
    fp = fopen( fname, "wb" );
    if( fp == NULL ) {
        WImgEditError( WIE_ERR_SAVE_FAIL, filename );
        return( FALSE );
    }

    if( fseek( fp, 0L, SEEK_SET ) ) {
        WImgEditError( WIE_ERR_SAVE_FAIL, filename );
        return( FALSE );
    }

    if( fwrite( &pal_file, sizeof( a_pal_file ), 1, fp ) != 1 ) {
        WImgEditError( WIE_ERR_SAVE_FAIL, filename );
        return( FALSE );
    }

    fclose( fp );
    PrintHintTextByID( WIE_PALETTESAVEDTO, filename );
    return( TRUE );

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
