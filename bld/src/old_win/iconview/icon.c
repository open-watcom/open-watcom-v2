#include <stdlib.h>
#include <stdio.h>
#include <windows.h>
#include "icon.h"
#include "util.h"
#include "view.h"

#define BITS_TO_BYTES( x, y )   ( ( ( x ) * ( y ) + 7 ) / 8 )

#define BITMAP_SIZE( bm )       ( sizeof(BITMAPINFO) + \
                                    sizeof(RGBQUAD) * ((1<<(bm)->biBitCount)-1))
/*
 * IconOpen - takes a handle to an icon resource file and returns an icon_file
 * structure which can be passed to other routines in this module to load a
 * given icon resource or extract a bitmap (XOR or AND) from an icon. Any file
 * opened in this manner must be closed with IconClose (see below).
 */

an_icon_file *IconOpen( FILE *fp )
{
    an_icon_file        *icon_file;
    unsigned            size;

    icon_file = UtilMalloc( sizeof( an_icon_file ) );
    /* read the header once to find out how many icons are in the file */
    fseek( fp, 0L, SEEK_SET );
    fread( icon_file, sizeof( an_icon_file ), 1, fp );
    if( icon_file->count > 1 ) {
        size = sizeof( an_icon_file ) +
                sizeof( an_icon_resource ) * ( icon_file->count - 1 );
        icon_file = realloc( icon_file, size );
        fseek( fp, 0L, SEEK_SET );
        fread( icon_file, (long) size, 1, fp );
    }
    return( icon_file );
} /* IconOpen */

/*
 * IconCount - returns the number of differ icons in an icon file.
 */

WORD IconCount( an_icon_file *icon_file )
{
    return( icon_file->count );
} /* IconCount */

/*
 * IconResInfo - puts the info for a particular icon into the resource
 * structure supplied.
 */

void IconResInfo( an_icon_file *icon_file, an_icon_resource *res, unsigned i )
{
    *res = icon_file->resources[ i ];
} /* IconResInfo */

/*
 * ReadIconBitmap - read in the bitmap information for an individual icon.
 * NOTE: assume fp is positioned at the start of the bitmap information.
 *      we first read in the BITMAPINFOHEADER to get information about the
 *      number of quads needed, then we reposition ourselves and read in
 *      the entire BITMAPINFOHEADER structure.
 */

static BITMAPINFO *ReadIconBitmap( FILE *fp )
{
    BITMAPINFO          *bm;
    BITMAPINFOHEADER    *header;
    long                DIB_offset, bitmap_size;

    header = UtilMalloc( sizeof( BITMAPINFOHEADER ) );
    DIB_offset = ftell( fp );
    fread( header, sizeof( BITMAPINFOHEADER ), 1, fp );
    fseek( fp, DIB_offset, SEEK_SET );
    bitmap_size = BITMAP_SIZE( header );
    bm = realloc( header, bitmap_size );
    if( !bm ) return( NULL );
    fread( bm, bitmap_size, 1, fp );
    return( bm );
} /* ReadIconBitmap */

/*
 * IconResourceToIcon - takes an icon file and creates an icon structure
 * from the i'th icon in the file. The structure must later be freed with
 * a call to the IconFini function.
 */

an_icon *IconResourceToIcon(  FILE *fp, an_icon_file *icon_file, unsigned i )
{
    an_icon_resource    *res;
    BITMAPINFO          *bm;
    BITMAPINFOHEADER    *h;
    an_icon             *icon;

    if( i >= icon_file->count ) return( NULL );
    res = &icon_file->resources[ i ];
    fseek( fp, res->DIB_offset, SEEK_SET );
    bm = ReadIconBitmap( fp );
    if( bm ) {
        icon = UtilMalloc( sizeof( an_icon ) );
        icon->bm = bm;
        h = &bm->bmiHeader;
        // h->biHeight /= 2;            /* code gen bug */
        h->biHeight = res->height;      /* they have height * 2 in this field */
        h->biSizeImage =
            BITS_TO_BYTES( h->biWidth * h->biBitCount, h->biHeight );
        icon->xor_size = h->biSizeImage;
        icon->and_size = BITS_TO_BYTES( h->biWidth, h->biHeight );
        icon->xor_mask = UtilMalloc( icon->xor_size + icon->and_size );
        icon->and_mask = (char *)icon->xor_mask + icon->xor_size;
        fseek( fp, res->DIB_offset + BITMAP_SIZE( h ), SEEK_SET );
        fread( icon->xor_mask, icon->xor_size + icon->and_size, 1, fp );
        return( icon );
    }
    return( NULL );
} /* IconResourceToIcon */

/*
 * IconFini - frees up memory allocated for an icon structure.
 */

void IconFini( an_icon *icon )
{
    UtilFree( icon->bm );
    UtilFree( icon->xor_mask );
    UtilFree( icon );
} /* IconFini */

/*
 * CreateIconPalette - creates a palette for the colours present in an
 * icon's bitmap.
 */

static HPALETTE CreateIconPalette( BITMAPINFO *info )
{
    unsigned            num_colours, i;
    LOGPALETTE          *palette;
    HPALETTE            palette_handle;
    RGBQUAD             *quads;

    num_colours = info->bmiHeader.biClrUsed;
    if( num_colours == 0 ) {
        num_colours = 1 << info->bmiHeader.biBitCount;
    }

    palette = UtilMalloc( sizeof( LOGPALETTE ) +
            num_colours * sizeof( PALETTEENTRY ) );
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
    UtilFree( palette );
    return( palette_handle );
} /* CreateIconPalette */

/*
 * IconToXorBitmap - creates a bitmap which contains the part of an icon
 * which is to be XOR'd against the background.
 */

HBITMAP IconToXorBitmap( HDC hdc, an_icon *icon )
{
    HBITMAP             bitmap_handle = NULL;
    HPALETTE            new_palette, old_palette;

    new_palette = CreateIconPalette( icon->bm );
    old_palette = SelectPalette( hdc, new_palette, FALSE );
    RealizePalette( hdc );
    bitmap_handle = CreateDIBitmap( hdc, &icon->bm->bmiHeader, CBM_INIT,
        icon->xor_mask, icon->bm, DIB_RGB_COLORS );
    SelectPalette( hdc, old_palette, FALSE );
    DeleteObject( new_palette );
    return( bitmap_handle );
} /* IconToXorBitmap */

/*
 * IconToAndBitmap - creates the bitmap which allows an icon to have a
 * tranparent border around the central image.
 */

HBITMAP IconToAndBitmap( HDC hdc, an_icon *icon )
{
    BITMAP              bitmap;
    HBITMAP             bitmap_handle;
    BITMAPINFOHEADER    *h;

    hdc = hdc;
    h = &icon->bm->bmiHeader;
    bitmap.bmType = 0;
    bitmap.bmWidth = h->biWidth;
    bitmap.bmHeight = h->biWidth;
    bitmap.bmWidthBytes = ( bitmap.bmWidth + 15 ) / 16;
    bitmap.bmPlanes = 1;
    bitmap.bmBitsPixel = 1;
    bitmap.bmBits = icon->and_mask;
    bitmap_handle = CreateBitmapIndirect( &bitmap );
    return( bitmap_handle );
} /* IconToAndBitmap */

/*
 * IconClose - closes an icon file which was opened with IconOpen.
 */

void IconClose( an_icon_file *icon_file )
{
    UtilFree( icon_file );
}
