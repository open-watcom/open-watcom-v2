/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2020-2021 The Open Watcom Contributors. All Rights Reserved.
*
*  ========================================================================
*
* Description:  conversion tool to convert Japanese text files
*                  Shift-JIS Windows CP932 <-> UTF-8
*
****************************************************************************/


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <locale.h>
#include "bool.h"


#define MAX_MB  1024
#define MARGIN  6

/*
 * Latin-1 CP1252
 * only range 0x80-0x9F need mapping
 * rest is equivalent to UNICODE
 * 0x00-0x7F
 * 0xA0-0xFF
 */
#define LATIN1_DEFS \
pick( 0x0080, 0x0080 ) \
pick( 0x0081, 0x0081 ) \
pick( 0x0082, 0x201A ) \
pick( 0x0083, 0x0192 ) \
pick( 0x0084, 0x201E ) \
pick( 0x0085, 0x2026 ) \
pick( 0x0086, 0x2020 ) \
pick( 0x0087, 0x2021 ) \
pick( 0x0088, 0x02C6 ) \
pick( 0x0089, 0x2030 ) \
pick( 0x008A, 0x0160 ) \
pick( 0x008B, 0x2039 ) \
pick( 0x008C, 0x0152 ) \
pick( 0x008D, 0x008D ) \
pick( 0x008E, 0x008E ) \
pick( 0x008F, 0x008F ) \
pick( 0x0090, 0x0090 ) \
pick( 0x0091, 0x2018 ) \
pick( 0x0092, 0x2019 ) \
pick( 0x0093, 0x201C ) \
pick( 0x0094, 0x201D ) \
pick( 0x0095, 0x2022 ) \
pick( 0x0096, 0x2013 ) \
pick( 0x0097, 0x2014 ) \
pick( 0x0098, 0x02DC ) \
pick( 0x0099, 0x2122 ) \
pick( 0x009A, 0x0161 ) \
pick( 0x009B, 0x203A ) \
pick( 0x009C, 0x0153 ) \
pick( 0x009D, 0x009D ) \
pick( 0x009E, 0x009E ) \
pick( 0x009F, 0x0178 )

#define IS_ASCII(c)     (c < 0x80)

typedef enum {
    CP932 = 1,
    CP1252
} encoding;

typedef enum {
    TO_UTF8 = 1,
    FROM_UTF8
} direction;

typedef struct cvt_chr {
    unsigned short  s;
    unsigned short  u;
} cvt_chr;

typedef int (*comp_fn)(const void *,const void *);

/*
 * Shift-JIS (CP932) lead byte ranges
 * 0x81-0x9F
 * 0xE0-0xFC
 */
static cvt_chr cvt_table_from_932[] = {
    #define pickb(s,u) {s, u},
    #define picki(s,u) {s, u},
    #include "cp932uni.h"
    #undef picki
    #undef pickb
};

static cvt_chr cvt_table_to_932[] = {
    #define pickb(s,u) {s, u},
    #define picki(s,u)
    #include "cp932uni.h"
    #undef picki
    #undef pickb
};

static int compare_enc( const cvt_chr *p1, const cvt_chr *p2 )
{
    return( p1->s - p2->s );
}

static int compare_utf8( const cvt_chr *p1, const cvt_chr *p2 )
{
    return( p1->u - p2->u );
}

static unsigned short latin1_to_unicode( unsigned short cp )
{
    switch( cp ) {
    #define pick(l,u) case l: return( u );
    LATIN1_DEFS
    #undef pick
    default:   return( 0 );
    }
}

static unsigned short unicode_to_latin1( unsigned short u )
{
    switch( u ) {
    #define pick(l,u) case u: return( l );
    LATIN1_DEFS
    #undef pick
    default:   return( 0 );
    }
}

static size_t enc_to_utf8( encoding enc, const char *src, char *dst )
{
    size_t      i;
    size_t      o;
    size_t      src_len;
    cvt_chr     x;
    cvt_chr     *p;

    src_len = strlen( src );
    o = 0;
    for( i = 0; i < src_len && o < MAX_MB - MARGIN; i++ ) {
        x.s = (unsigned char)src[i];
        if( IS_ASCII( x.s ) ) {
            /*
             * ASCII (0x00-0x7F), no conversion
             */
            dst[o++] = (char)x.s;
        } else if( enc == 0 ) {
            dst[o++] = (char)x.s;
        } else {
            /*
             *  selected encoding to UNICODE conversion
             */
            if( enc == CP932 ) {
                if( x.s > 0x80 && x.s < 0xA0 || x.s > 0xDF && x.s < 0xFD ) {
                    x.s = x.s << 8 | (unsigned char)src[++i];
                }
                p = bsearch( &x, cvt_table_from_932, sizeof( cvt_table_from_932 ) / sizeof( cvt_table_from_932[0] ), sizeof( cvt_table_from_932[0] ), (comp_fn)compare_enc );
                if( p == NULL ) {
                    printf( "unknown double-byte character: 0x%4X\n", x.s );
                    dst[o++] = '?';
                    continue;
                }
                x.u = p->u;
            } else if( enc == CP1252 ) {
                if( x.s < 0xA0 ) {  /* 0x80-0x9F */
                    x.u = latin1_to_unicode( x.s );
                } else {            /* 0xA0-0xFF */
                    x.u = x.s;
                }
            } else {
                x.u = (unsigned char)x.s;
            }
            /*
             * UNICODE to UTF-8 conversion
             */
            if( x.u > 0x7FF ) {
                dst[o++] = (char)((x.u >> 12) | 0xE0);
                dst[o++] = (char)(((x.u >> 6) & 0x3F) | 0x80);
            } else {
                dst[o++] = (char)((x.u >> 6) | 0xC0);
            }
            dst[o++] = (char)((x.u & 0x3F) | 0x80);
        }
    }
    dst[o] = '\0';
    return( o );
}

static size_t utf8_to_enc( encoding enc, const char *src, char *dst )
{
    size_t      i;
    size_t      o;
    size_t      src_len;
    cvt_chr     x;
    cvt_chr     *p;

    src_len = strlen( src );
    o = 0;
    for( i = 0; i < src_len && o < MAX_MB - MARGIN; i++ ) {
        x.u = (unsigned char)src[i];
        if( IS_ASCII( x.u ) ) {
            /*
             * ASCII (0x00-0x7F), no conversion
             */
            dst[o++] = (char)x.u;
        } else if( enc == 0 ) {
            dst[o++] = (char)x.u;
        } else {
            /*
             * UTF-8 to UNICODE conversion
             */
            if( (x.u & 0xF0) == 0xE0 ) {
                x.u &= 0x0F;
                x.u = (x.u << 6) | ((unsigned char)src[++i] & 0x3F);
            } else {
                x.u &= 0x1F;
            }
            x.u = (x.u << 6) | ((unsigned char)src[++i] & 0x3F);
            /*
             * UNICODE to selected encoding conversion
             */
            if( enc == CP932 ) {
                p = bsearch( &x, cvt_table_to_932, sizeof( cvt_table_to_932 ) / sizeof( cvt_table_to_932[0] ), sizeof( cvt_table_to_932[0] ), (comp_fn)compare_utf8 );
                if( p == NULL ) {
                    printf( "unknown unicode character: 0x%4X\n", x.u );
                    x.s = '?';
                } else {
                    x.s = p->s;
                }
            } else if( enc == CP1252 ) {
                if( x.u > 0x9F && x.u < 0x100 ) {        /* 0xA0-0xFF */
                    x.s = x.u;
                } else {
                    x.s = unicode_to_latin1( x.u );
                    if( x.s == 0 ) {
                        printf( "unknown unicode character: 0x%4X\n", x.u );
                        x.s = '?';
                    }
                }
            } else {
                x.s = (unsigned char)x.u;
            }
            if( x.s > 0xFF ) {
                /* write lead byte first */
                dst[o++] = (char)(x.s >> 8);
            }
            dst[o++] = (char)x.s;
        }
    }
    dst[o] = '\0';
    return( o );
}

static void usage( void )
{
    puts( "Usage: cvtutf8 [options] <input file> <output file>" );
    puts( "  options:" );
    puts( "    -to      convert utf-8 to selected encoding" );
    puts( "    -from    convert selected encoding to utf-8" );
    puts( "    -cp1252  use Latin-1 CP1252" );
    puts( "    -cp932   use Shift-JIS CP932" );
}

int main( int argc, char *argv[] )
{
    FILE        *fi;
    FILE        *fo;
    char        in_buff[MAX_MB];
    char        out_buff[MAX_MB];
    size_t      in_len;
    direction   cvt_dir;
    encoding    enc;

    /* unused parameters */ (void)argc;

    setlocale(LC_ALL,"C");
    cvt_dir = 0;
    enc = 0;
    ++argv;
    while( *argv != NULL && **argv == '-' ) {
        if( strcmp( argv[0] + 1, "to" ) == 0 ) {
            cvt_dir = FROM_UTF8;
        } else if( strcmp( argv[0] + 1, "from" ) == 0 ) {
            cvt_dir = TO_UTF8;
        } else if( strcmp( argv[0] + 1, "cp932" ) == 0 ) {
            enc = CP932;
        } else if( strcmp( argv[0] + 1, "cp1252" ) == 0 ) {
            enc = CP1252;
        } else {
            usage();
            printf( "\nOption '%s' not recognized.\n", *argv );
            return( 1 );
        }
        ++argv;
    }
    if( cvt_dir == 0 || enc == 0 ) {
        usage();
        printf( "\nNo option was selected!\nEncoding and direction must be specified.\n" );
        return( 1 );
    }
    if( *argv == NULL ) {
        usage();
        printf( "\nMissing input file name.\n" );
        return( 2 );
    }
    fi = fopen( *argv, "rb" );
    if( fi == NULL ) {
        printf( "Cannot open input file '%s'.\n", *argv );
        return( 3 );
    }
    ++argv;
    if( *argv == NULL ) {
        fclose( fi );
        usage();
        printf( "\nMissing output file name.\n" );
        return( 4 );
    }
    fo = fopen( *argv, "wb" );
    if( fo == NULL ) {
        printf( "Cannot open output file '%s'.\n", *argv );
        fclose( fi );
        return( 5 );
    }
    if( enc == CP932 ) {
        if( cvt_dir == FROM_UTF8 ) {
            qsort( cvt_table_to_932, sizeof( cvt_table_to_932 ) / sizeof( cvt_table_to_932[0] ), sizeof( cvt_table_to_932[0] ), (comp_fn)compare_utf8 );
        } else {
            qsort( cvt_table_from_932, sizeof( cvt_table_from_932 ) / sizeof( cvt_table_from_932[0] ), sizeof( cvt_table_from_932[0] ), (comp_fn)compare_enc );
        }
    }
    while( fgets( in_buff, sizeof( in_buff ), fi ) != NULL ) {
        in_len = strlen( in_buff );
        if( in_len ) {
            if( in_buff[in_len - 1] == '\r' || in_buff[in_len - 1] == 0x1A ) {
                in_buff[--in_len] = '\0';
            }
        }
        if( cvt_dir == FROM_UTF8 ) {
            utf8_to_enc( enc, in_buff, out_buff );
        } else {
            enc_to_utf8( enc, in_buff, out_buff );
        }
        fputs( out_buff, fo );
    }
    fclose( fi );
    fclose( fo );
    return( 0 );
}
