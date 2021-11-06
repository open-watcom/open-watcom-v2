/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2020-2020 The Open Watcom Contributors. All Rights Reserved.
*
*  ========================================================================
*
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include "global.h"
#include "rccore.h"
#include "param.h"
#include "rcvars.h"
#include "rcmem.h"
#include "cvttable.h"
#include "unitable.h"

#include "clibext.h"


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

static cvt_chr  **cvt_table = NULL;
static size_t   cvt_table_len = 0;

static int getcharUTF8( const char **p, unsigned short *c )
/*********************************************************/
{
    int             len;
    int             i;
    unsigned short  value;

    value = *c;
    len = CharSetLen[value];
    if( len == 1 ) {
        value &= 0x1F;
    } else if( len == 2 ) {
        value &= 0x0F;
    } else if( len == 3 ) {
        value &= 0x07;
    } else if( len == 4 ) {
        value &= 0x03;
    } else if( len == 5 ) {
        value &= 0x01;
    } else {
        return( 0 );
    }
    for( i = 0; i < len; ++i ) {
        value = ( value << 6 ) + ( **p & 0x3F );
        (*p)++;
    }
    *c = value;
    return( len );
}

static unsigned short unicode_to_latin1( unsigned short u )
/*********************************************************/
{
    switch( u ) {
    #define pick(l,u) case u: return( l );
    LATIN1_DEFS
    #undef pick
    default:   return( 0 );
    }
}

static int compare_utf8( const cvt_chr *key, const cvt_chr *data )
/****************************************************************/
{
    return( key->u - data->u );
}

static size_t UTF8StringToMultiByte( size_t len, const char *str, char *buf )
/****************************************************************************
 * this function convert UTF-8 buffer to Shift-JIS (CP932) buffer
 * first read UTF-8 character then convert it to CP932 and put to
 * output buffer, repeat until consume all input bytes
 */
{
    size_t          ret;
    unsigned short  u;
    size_t          i;
    cvt_chr         x;
    cvt_chr         *p;

    ret = 0;
    for( i = 0; i < len; i++ ) {
        u = (unsigned char)*str++;
        if( !IS_ASCII( u ) ) {
            i += getcharUTF8( &str, &u );
            x.u = u;
            p = bsearch( &x, cvt_table, cvt_table_len, sizeof( cvt_chr ), (int(*)(const void*,const void*))compare_utf8 );
            if( p == NULL ) {
                printf( "unknown unicode character: 0x%4X\n", x.u );
                u = '?';
            } else {
                if( p->s > 0xFF ) {
                    if( ret < len ) {
                        if( buf != NULL ) {
                            *buf++ = (char)( p->s >> 8 );
                        }
                        ret++;
                    }
                }
                u = (char)p->s;
            }
        }
        if( ret < len ) {
            if( buf != NULL ) {
                *buf++ = (char)u;
            }
            ret++;
        }
    }
    return( ret );
}

static size_t UTF8StringToCP1252( size_t len, const char *str, char *buf )
/*************************************************************************
 * this function convert UTF-8 buffer to Latin-1 (CP1252) buffer
 * first read UTF-8 character then convert it to CP1252 and put to
 * output buffer, repeat until consume all input bytes
 */
{
    size_t          ret;
    unsigned short  u;
    size_t          i;
    char            c;

    ret = 0;
    for( i = 0; i < len; i++ ) {
        u = (unsigned char)*str++;
        if( !IS_ASCII( u ) ) {
            i += getcharUTF8( &str, &u );
            /*
             * UNICODE to CP1252
             */
            c = (char)unicode_to_latin1( u );
            if( c != 0 ) {
                u = c;
            }
        }
        if( ret < len ) {
            if( buf != NULL ) {
                *buf++ = (char)u;
            }
            ret++;
        }
    }
    return( ret );
}

static size_t UTF8StringToUTF8( size_t len, const char *str, char *buf )
/***********************************************************************
 * this function copy UTF-8 buffer to UTF-8 buffer
 */
{
    if( len > 0 ) {
        if( buf != NULL ) {
            memcpy( buf, str, len );
        }
    }
    return( len );
}

static size_t UTF8StringToUnicode( size_t len, const char *str, char *buf )
/**************************************************************************
 * this function convert UTF-8 buffer to 16-bit UNICODE buffer
 * first read UTF-8 character then convert it to UNICODE and put to
 * output buffer, repeat until consume all input bytes
 */
{
    size_t          ret;
    unsigned short  u;
    size_t          i;

    ret = 0;
    for( i = 0; i < len; i++ ) {
        u = (unsigned char)*str++;
        if( !IS_ASCII( u ) ) {
            i += getcharUTF8( &str, &u );
        }
        if( ret < len ) {
            if( buf != NULL ) {
                *buf++ = (char)u;
                *buf++ = (char)( u >> 8 );
            }
            ret++;
        }
    }
    return( ret * 2 );
}

RcStatus SetUTF8toUnicode( void )
{
    ConvToUnicode = UTF8StringToUnicode;
    return( RS_OK );
}

RcStatus SetUTF8toCP932( void )
{
    FILE            *fh;
    RcStatus        ret;
    unsigned short  u16;
    char            path[_MAX_PATH];
    char            *fname;

    ret = RS_OK;
    fname = "to932.uni";
    _searchenv( fname, "PATH", path );
    if( path[0] == '\0' ) {
        printf( "%s file not found\n", fname );
        return( RS_FILE_NOT_FOUND );
    }
    fh = fopen( path, "rb" );
    if( fh == NULL ) {
        ret = RS_OPEN_ERROR;
    } else {
        if( ret == RS_OK ) {
            cvt_table_len = fread( &u16, 1, sizeof( u16 ), fh );
            if( cvt_table_len != sizeof( u16 ) ) {
                cvt_table_len = 0;
                ret = ( feof( fh ) ) ? RS_READ_INCMPLT : RS_READ_ERROR;
            } else {
                cvt_table_len = u16;
                cvt_table = RcMemMalloc( sizeof( cvt_chr ) * cvt_table_len );
                if( cvt_table_len != fread( cvt_table, sizeof( cvt_chr ), cvt_table_len , fh ) ) {
                    ret = ( feof( fh ) ) ? RS_READ_INCMPLT : RS_READ_ERROR;
                }
            }
        }
        fclose( fh );
    }
    ConvToMultiByte = UTF8StringToMultiByte;

    return( ret );
}


RcStatus SetUTF8toCP1252( void )
{
    ConvToMultiByte = UTF8StringToCP1252;
    return( RS_OK );
}


RcStatus SetUTF8toUTF8( void )
{
    ConvToMultiByte = UTF8StringToUTF8;
    return( RS_OK );
}
