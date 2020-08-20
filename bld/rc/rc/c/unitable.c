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


static cvt_chr  **cvt_table = NULL;
static size_t   cvt_table_len = 0;

static int getcharUTF8( const char **p, uint_32 *c )
/**************************************************/
{
    int     len;
    int     i;
    uint_32 value;

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
    size_t          outlen;
    uint_32         unicode;
    size_t          i;
    cvt_chr         x;
    cvt_chr         *p;

    ret = 0;
    if( len > 0 ) {
        if( buf == NULL ) {
            outlen = 0;
        } else {
            outlen = len;
        }
        for( i = 0; i < len; i++ ) {
            unicode = (unsigned char)*str++;
            if( unicode < 0x80 ) {
                if( ret < outlen ) {
                    *buf++ = (char)unicode;
                    ret++;
                }
            } else {
                i += getcharUTF8( &str, &unicode );
                if( ret < outlen ) {
                    x.u = unicode;
                    p = bsearch( &x, cvt_table, cvt_table_len, sizeof( cvt_chr ), (int(*)(const void*,const void*))compare_utf8 );
                    if( p == NULL ) {
                        printf( "unknown unicode character: 0x%4X\n", x.u );
                        *buf++ = '?';
                    } else {
                        if( p->s > 0xFF ) {
                            *buf++ = (char)( p->s >> 8 );
                            ret++;
                        }
                        *buf++ = (char)p->s;
                    }
                    ret++;
                }
            }
        }
    }
    return( ret );
}

static size_t UTF8StringToUnicode( size_t len, const char *str, char *buf )
/**************************************************************************
 * this function convert UTF-8 buffer to 16-bit UNICODE buffer
 * first read UTF-8 character then convert it to UNICODE and put to
 * output buffer, repeat until consume all input bytes
 */
{
    size_t          ret;
    size_t          outlen;
    uint_32         unicode;
    size_t          i;

    ret = 0;
    if( len > 0 ) {
        if( buf == NULL ) {
            outlen = 0;
        } else {
            outlen = len;
        }
        for( i = 0; i < len; i++ ) {
            unicode = (unsigned char)*str++;
            if( unicode > 0x7F ) {
                i += getcharUTF8( &str, &unicode );
            }
            if( ret < outlen ) {
                *buf++ = (char)unicode;
                *buf++ = (char)( unicode >> 8 );
                ret++;
            }
        }
    }
    return( ret * 2 );
}

RcStatus SetUTF8toUnicode( void )
{
    ConvToUnicode = UTF8StringToUnicode;
    return( RS_OK );
}

RcStatus SetUTF8toMultiByte( void )
{
    FILE            *fh;
    RcStatus        ret;
    unsigned short  u16;
    char            path[_MAX_PATH];
    char            *fname;

    ret = RS_OK;
    if( CmdLineParms.MBCharSupport == MB_UTF8_KANJI ) {
        fname = "to932.uni";
    } else {
        fname = "";
    }
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
