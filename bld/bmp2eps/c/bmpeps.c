/*
 * bmpeps - BMP to EPS conversion module
 * Copyright (C) 2003 - Michal Necasek
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 * In this package the copy of the GNU Library General Public License
 * is placed in file COPYING.
 */

#include "bmepsco.h"
#include "bmpeps.h"
#include "bmeps.h"


static char default_name[] = { "noname.bmp" };

static int
bmp_run( FILE *out, FILE *in, char *name, unsigned long *w, unsigned long *h, int cmd )
{
    int             success = 0;
    unsigned long   width;
    unsigned long   height;
    unsigned long   x, y;
    int             bpp;    /* bits per pixel */
//    int             alpha, trans, altrig;
    int             rowbytes;
//    int             mix;    /* mix foreground and background */
//    int             specbg; /* specified background from command line */
//    int             bg_red, bg_green, bg_blue;
    unsigned char   *row, **rows, **rowp;
    bmp_file_header bmfh;
    bmp_info_header bmih;
    bmp_rgb_quad    *bmpal;
    size_t          palsize = 0;
    size_t          hdrsize;

    if( !in )
        return 0;

//    alpha    = bmeps_get_alpha();
//    trans    = bmeps_get_trans();
//    altrig   = bmeps_get_altrig();
//    mix      = bmeps_get_mix();
//    specbg   = bmeps_get_specbg();
//    bg_red   = bmeps_get_bg_red();
//    bg_green = bmeps_get_bg_green();
//    bg_blue  = bmeps_get_bg_blue();
    rewind( in );

    // Check for the 'BM' header
    if( !fread( &bmfh, sizeof( bmfh ), 1, in ) || ( bmfh.type != 0x4d42 ))
        return 0;

    if( !fread( &bmih, sizeof( bmih ), 1, in ) )
        return 0;

    bpp = bmih.bit_count;

    if( bpp != 24 ) {
        palsize = bmih.clr_used * sizeof( bmp_rgb_quad );
        if( palsize == 0 )
            switch( bpp ) {
            case 4:
                palsize = 16 * sizeof( bmp_rgb_quad );
                break;
            case 8:
                palsize = 256 * sizeof( bmp_rgb_quad );
                break;
            }

        bmpal = (bmp_rgb_quad *)malloc( palsize );
        if( !fread( bmpal, palsize, 1, in ) )
            return 0;
    }

    hdrsize = sizeof( bmfh ) + sizeof( bmih ) + palsize;
    width   = bmih.width;
    height  = bmih.height;

    switch(cmd) {
    case 0:
        if(out) {
            bmeps_header( out, (name ? name : default_name), width, height );
            if( bmeps_get_draft() ) {
                success = 1;
                bmeps_draft( out, width, height );
            } else {
                rowbytes = width * sizeof( bmp_rgb_triplet );
                rowbytes = ( rowbytes + 3 ) & ~3;   // Dword aligned
                rows = (unsigned char **)malloc( height * sizeof( unsigned char * ));
                if( rows ) {
                    success = 1;
                    rowp = rows;
                    for( y = 0; y < height; y++ ) {
                        *rowp = NULL;
                        row = (unsigned char *)malloc( rowbytes * sizeof( unsigned char));
                        if( row )
                            *rowp = row;
                        else
                            success = 0;

                        rowp++;
                    }

                    if( bmfh.off_bits > hdrsize )
                        success = 0;

                    if( success ) {
                        // Read bitmap data from file - stored bottom up!
                        rowp--;
                        switch( bpp ) {
                        case 24:
                            // Just dump file data into memory
                            for( y = 0; y < height; y++ ) {
                                fread( *rowp, rowbytes, 1, in );
                                rowp--;
                            }
                            break;
                        case 8: {
                            unsigned char   *frow;
                            size_t          frowbytes;

                            frowbytes = width * sizeof( unsigned char );
                            frowbytes = ( frowbytes + 3 ) & ~3;   // Dword aligned
                            frow = (unsigned char *)malloc( frowbytes );
                            // Expand 8bpp data using palette
                            for( y = 0; y < height; y++ ) {
                                fread( frow, frowbytes, 1, in );
                                row = *rowp;
                                for( x = 0; x < width; x++ ) {
                                    row[x*3+0] = bmpal[frow[x]].blue;
                                    row[x*3+1] = bmpal[frow[x]].green;
                                    row[x*3+2] = bmpal[frow[x]].red;
                                }
                                rowp--;
                            }
                            }
                            break;
                        case 4: {
                            unsigned char   *frow, nibble;
                            size_t          frowbytes;

                            frowbytes = ( width * sizeof( unsigned char ) + 1 ) / 2;
                            frowbytes = ( frowbytes + 3 ) & ~3;   // Dword aligned
                            frow = (unsigned char *)malloc( frowbytes );
                            // Expand 4bpp data using palette
                            for( y = 0; y < height; y++ ) {
                                fread( frow, frowbytes, 1, in );
                                row = *rowp;
                                for( x = 0; x < width; x++ ) {
                                    nibble = frow[x/2];
                                    if( x & 1 )
                                        nibble &= 0x0F;
                                    else
                                        nibble >>= 4;

                                    row[x*3+0] = bmpal[nibble].blue;
                                    row[x*3+1] = bmpal[nibble].green;
                                    row[x*3+2] = bmpal[nibble].red;
                                }
                                rowp--;
                            }
                            }
                            break;
                        default:
                            success = 0;
                        }
                    }

                    if( success ) {
                        bmeps_begin_image( out, width, height );
                        rowp = rows;
                        for( y = 0; y < height; y++ ) {
                            row = *(rowp++);
                            for(x = 0; x < width; x++)
                                bmeps_add_rgb( row[x*3+2], row[x*3+1], row[x*3+0] );
                        }
                        bmeps_end_image( out );
                  }
                  /* done with rows */
                  rowp = rows;
                  for( y = 0; y < height; y++ ) {
                      row = *rowp;
                      if( row )
                        free( row );

                      *(rowp++) = NULL;
                  }
                  free( rows );
                }
            }
            bmeps_footer(out);
        }
        break;
    case 1:
        if( out ) {
            success = 1;
            bmeps_bb( out, width, height );
        }
        break;
    case 2:
        if( w && h ) {
            success = 1;
            *w = width;
            *h = height;
        }
        break;
    }
    /* done with it */
    return success;
}

int bmeps_bmp( FILE *out, FILE *in, char *name )
{
    int success = 0;
    if( out && in ) {
        bmeps_configure();
        success = bmp_run( out, in, name, NULL, NULL, 0 );
    }
    return success;
}

int bmeps_bmp_bb( FILE *out, FILE *in, char *name )
{
    int success = 0;
    if( out && in ) {
        bmeps_configure();
        success = bmp_run( out, in, name, NULL, NULL, 1 );
    }
    return success;
}

int bmeps_bmp_wh( FILE *in, unsigned long *w, unsigned long *h )
{
    int success = 0;
    if( w && h && in ) {
        bmeps_configure();
        success = bmp_run( NULL, in, NULL, w, h, 2 );
    }
    return success;
}
