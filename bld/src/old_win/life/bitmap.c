#include "life.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <dos.h>
#include "palette.h"
#include "bitmap.h"
#include "filedlg.h"

#define HUGE_SHIFT      8

/*
 * NOTE: assume fp is positioned at the start of the bitmap information.
 *      we first read in the BITMAPINFOHEADER to get information about the
 *      number of quads needed, then we reposition ourselves and read in
 *      the entire BITMAPINFOHEADER structure.
 */

static BITMAPINFO *ReadBitmapInfo( FILE *fp )
{
    BITMAPINFO          *bm;
    BITMAPINFOHEADER    *header;
    long                DIB_offset, bitmap_size;

    header = malloc( sizeof( BITMAPINFOHEADER ) );
    if( !header ) return( NULL );
    DIB_offset = ftell( fp );
    fread( header, sizeof( BITMAPINFOHEADER ), 1, fp );
    fseek( fp, DIB_offset, SEEK_SET );
    bitmap_size = BITMAP_INFO_SIZE( header );
    bm = realloc( header, bitmap_size );
    if( !bm ) return( NULL );
    fread( bm, bitmap_size, 1, fp );
    return( bm );
}

#ifndef __WINDOWS_386__
#define _HUGE huge
static void HugeMemCopy( char far *dst, char far *src, unsigned bytes )
{
    long                offset, selector;
    long                bytes_before_segment_end;

    offset = FP_OFF( dst );
    selector = FP_SEG( dst );
    bytes_before_segment_end = 0x10000L - offset;
    if( bytes_before_segment_end < bytes ) {
        _fmemcpy( dst, src, bytes_before_segment_end );
        bytes -= bytes_before_segment_end;
        selector += HUGE_SHIFT;
        dst = MK_FP( selector, 0 );
        src += bytes_before_segment_end;
    }
    _fmemcpy( dst, src, bytes );
}
#else
#define HugeMemCopy( a, b, c ) memcpy( a, b, c )
#define _HUGE
#endif

/* this is the amount of memory we read in at once */

#define CHUNK_SIZE      (48 * 1024)

static void ReadInPieces( BYTE _HUGE *dst, FILE *fp, DWORD size )
{
    BYTE                *buffer;
    WORD                chunk_size;

    chunk_size = CHUNK_SIZE;
    while( ( buffer = malloc( chunk_size ) ) == NULL ) {
        chunk_size >>= 1;
    }
    while( size > chunk_size ) {
        fread( buffer, chunk_size, 1, fp );
        HugeMemCopy( dst, buffer, chunk_size );
        dst += chunk_size;
        size -= chunk_size;
    }
    fread( buffer, size, 1, fp );
    _memcpy( dst, buffer, size );
    free( buffer );
} /* ReadInPieces */

/*
 * Loads a device independant bitmap from the file <file_name> and
 * returns a handle to a newly created BITMAP.
 */

HBITMAP ReadBitmapFile( HWND hwnd, char *file_name )
{
    FILE                *fp;
    BITMAPFILEHEADER    file_header;
    BITMAPINFOHEADER    *h;             /* to avoid typeing bitmap_info->... */
    BITMAPINFO          *bitmap_info;
    DWORD               size;           /* generic size - used repeatedly */
    BYTE _HUGE          *mask_ptr;      /* pointer to bit array in memory */
    HBITMAP             bitmap_handle;
    HDC                 hdc;
    HPALETTE            new_palette, old_palette;

    bitmap_handle = (HBITMAP)0;
    fp = fopen( file_name, "rb" );
    if( fp == NULL ) return( bitmap_handle );
    fread( &file_header, sizeof( BITMAPFILEHEADER ), 1, fp );
    if( file_header.bfType != BITMAP_TYPE ) {
        fclose( fp );
        return( bitmap_handle );
    }

    bitmap_info = ReadBitmapInfo( fp );
    if( bitmap_info != NULL ) {
        h = &bitmap_info->bmiHeader;
        fseek( fp, file_header.bfOffBits, SEEK_SET );
        size = BITS_TO_BYTES( h->biWidth * h->biBitCount, h->biHeight );
        mask_ptr = _halloc( size, 1 );
        if( mask_ptr != NULL ) {
            ReadInPieces( mask_ptr, fp, size );
            new_palette = CreateDIBPalette( bitmap_info );
            if( new_palette ) {
                hdc = GetDC( hwnd );
                old_palette = SelectPalette( hdc, new_palette, FALSE );
                RealizePalette( hdc );
                bitmap_handle = CreateDIBitmap( hdc, h, CBM_INIT,
                                    mask_ptr, bitmap_info, DIB_RGB_COLORS );
                SelectPalette( hdc, old_palette, FALSE );
                DeleteObject( new_palette );
                ReleaseDC( hwnd, hdc );
            }
            _hfree( mask_ptr );
        }
        free( bitmap_info );
    }
    fclose( fp );
    return( bitmap_handle );
} /* ReadBitmapFile */

HBITMAP RequestBitmapFile( void )
{
    char                file_name[ _MAX_PATH ];
    HBITMAP             bitmap = 0;

    if( GetFileName( "Open Bitmap",FILE_OPEN, "*.BMP",
                                file_name, _MAX_PATH ) ) {
        bitmap = ReadBitmapFile( WinHandle, file_name );
    }
    return( bitmap );
} /*RequestBitmapFile */
