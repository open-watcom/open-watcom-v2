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
* Description:  OMF dumper output routines.
*
****************************************************************************/


#include <ctype.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <errno.h>

#include "dmpobj.h"

#define OUT_BUFF_SIZE   1024
#define OUT_BUFF_WRITE  512     /* write outBuff when this full */

bool    no_disp = false;

static  char            outBuff[ OUT_BUFF_SIZE ];
static  char            *outBuffPtr;
static  FILE            *outputFH;
static  size_t          col;

static  const char hexStr[] = "0123456789abcdef";

static void flush( void )
{
    size_t  len;

    len = outBuffPtr - outBuff;
    outBuffPtr = outBuff;
    if( len > 0 ) {
        if( fwrite( outBuff, 1, len, outputFH ) != len ) {
            fprintf( stderr, "Write error - %s\n", strerror( errno ) );
            leave( 20 );
        }
    }
}

static char *toHex2( char *p, byte val )
{
    p[1] = hexStr[ val & 0xf ];
    p[0] = hexStr[ val >> 4 ];
    return( p + 2 );
}

static char *toHex4( char *p, unsigned_16 val )
{
    byte    lo;
    byte    hi;

    lo = val & 0xff;
    hi = val >> 8;
    p[1] = hexStr[ hi & 0xf ];
    p[0] = hexStr[ hi >> 4 ];
    p[3] = hexStr[ lo & 0xf ];
    p[2] = hexStr[ lo >> 4 ];
    return( p + 4 );
}

static char *toHex8( char *p, unsigned_32 val )
{
    unsigned_16 low;
    byte        low_lob;
    byte        low_hib;
    unsigned_16 hiw;
    byte        hiw_lob;
    byte        hiw_hib;

    low = val & 0xffff;
    hiw = val >> 16;
    hiw_lob = hiw & 0xff;
    hiw_hib = hiw >> 8;
    p[1] = hexStr[ hiw_hib & 0xf ];
    p[0] = hexStr[ hiw_hib >> 4 ];
    p[3] = hexStr[ hiw_lob & 0xf ];
    p[2] = hexStr[ hiw_lob >> 4 ];
    low_lob = low & 0xff;
    low_hib = low >> 8;
    p[5] = hexStr[ low_hib & 0xf ];
    p[4] = hexStr[ low_hib >> 4 ];
    p[7] = hexStr[ low_lob & 0xf ];
    p[6] = hexStr[ low_lob >> 4 ];
    return( p +  8 );
}

static char *toDec16( char *dest, unsigned_16 num )
{
    char    *orig;
    char    *str;
    div_t    res;

    orig = dest;
    str = dest + ( 3 * sizeof( unsigned ) / sizeof( char ) );
    if( num > 32767 ) {       /* 'cause div takes ints not unsigneds...*/
        ++dest;
        *--str = '0' + num % 10U;
        num /= 10U;     /* now it should be less than INT_MAX... */
    }
    res.quot = num;
    do {
        ++dest;
        res = div( res.quot, 10 );
        *--str = '0' + res.rem;
    } while( res.quot != 0 );

    while( dest >= orig ) {
        *orig++ = *str++;
    }

    return( dest );
}

static char *to5Dec16( char *dest, unsigned_16 num )
{
    div_t   res;
    char    *orig;

    if( num & 0x8000 ) { // magic index into header file line number table
        *dest++ = '*';
        num &= 0x7FFF;
    } else {
        *dest++ = ' ';
    }
    res.quot = num;
    orig = dest;
    dest += 5;
    do {
        res = div( res.quot, 10 );
        *--dest = '0' + res.rem;
    } while( res.quot != 0 );

    while( dest > orig ) {
        *--dest = ' ';
    }
    return( orig + 5 );
}

/*
    Output is a customized printf with the following format options:

    %c  same as printf %c
    %s  same as printf %s   (string must be less than OUT_BUFF_WRITE)
    %R  print remainder of RecBuff starting at RecPtr
    %N  print NameLen chars from NamePtr     (implicit arguments)
    %b  same as printf %2.2xh
    %2  same as printf %2.2x
    %x  same as printf %4.4xh
    %X  same as printf %8.8xh
    %8  same as printf %8.8x
    %u  same as printf %u
    %5  same as printf %5u
    %>  arg is number of spaces to print, truncates to 80 spaces
    %<  arg is minimum width of print up to this point (pads with spaces)
    %%  print a %

    Output makes the assumption that % is not followed by the null-
    terminator.
*/
size_t  Output( const char *fmt, ... )
{
    va_list     args;
    char        *p;
    const char  *probe;
    const char  *str;
    size_t      len;
    char        *pcrlf;

    if( no_disp )
        return( 0 );
    va_start( args, fmt );
    p = outBuffPtr;
    for(;;) {
        probe = strchr( fmt, '%' );
        if( probe == NULL ) {
            len = strlen( fmt );
            memcpy( p, fmt, len );
            p += len;
            break;
        }
        len = probe - fmt;
        if( len > 0 ) {
            memcpy( p, fmt, len );
            p += len;
        }
        fmt = probe + 1;
        switch( *fmt ) {
        case 'c':
            *p++ = va_arg( args, int );
            break;
        case 's':
            str = va_arg( args, const char * );
            len = strlen( str );
            memcpy( p, str, len );
            p += len;
            break;
        case 'R':
            len = RecLen - RecOffset();
            if( len > OUT_BUFF_WRITE ) {
                len = OUT_BUFF_WRITE;
            }
            memcpy( p, RecPtr, len );
            p += len;
            break;
        case 'N':
            *p++ = '\'';
            memcpy( p, NamePtr, NameLen );
            p += NameLen;
            *p++ = '\'';
            break;
        case 'b':
            p = toHex2( p, va_arg( args, unsigned ) );
            *p++ = 'h';
            break;
        case '2':
            p = toHex2( p, va_arg( args, unsigned ) );
            break;
        case 'x':
            p = toHex4( p, va_arg( args, unsigned ) );
            *p++ = 'h';
            break;
        case 'X':
            p = toHex8( p, va_arg( args, unsigned_32 ) );
            *p++ = 'h';
            break;
        case '8':
            p = toHex8( p, va_arg( args, unsigned_32 ) );
            break;
        case 'u':
            p = toDec16( p, va_arg( args, unsigned ) );
            break;
        case '5':
            p = to5Dec16( p, va_arg( args, unsigned ) );
            break;
        case '<':
            len = va_arg( args, unsigned );
            while( p - outBuffPtr < len ) {
                *p++ = ' ';
            }
            break;
        case '>':
            len = va_arg( args, unsigned );
            if( len > 80 ) {
                len = 80;
            }
            memset( p, ' ', len );
            p += len;
            break;
        default:
            *p++ = *fmt;
            break;
        }
        ++fmt;
    }
    va_end( args );
    outBuffPtr = p;
    len = p - outBuff;
    *p = '\0';                        /* for following str.. function */
    pcrlf = strrchr( outBuff, '\n' ); /* need CRLF as char not string */
    if( pcrlf != NULL ) {
        col = p - pcrlf;
    } else {
        col += len;
    }
    if( len > OUT_BUFF_WRITE ) flush();
    return( col );
}

void OutputInit( void )
/*********************/
{
    outBuffPtr = outBuff;
    outputFH = stdout;
    col = 0;
}

void OutputSetFH( FILE *fh )
/**************************/
{
    flush();
    if( outputFH != stdout ) {
        fclose( outputFH );
    }
    outputFH = fh;
}

void OutputFini( void )
/*********************/
{
    flush();
}

void OutputData( unsigned_32 offset, unsigned_32 len )
/****************************************************/
{
    int         i;
    char        *j;
    char        ch;
    char        ascbuf[80];

    if( no_disp )
        return;
    if( len == 0 ) {
        len = RecLen - RecOffset();  /* FIXME - kind of a kludge */
    }
    Output( INDENT "%8", offset );
    offset += 16;
    i = 0;
    j = ascbuf;
    for(;;) {
        if( len == 0 )
            break;
        if( i > 0xf ) {
            *j = 0;
            Output( " <%s>" CRLF INDENT "%8", ascbuf, offset );
            offset += 16;
            i = 0;
            j = ascbuf;
        }
        ch = GetByte();
        --len;
        *j++ = isprint( ch ) ? ch : '.';
        Output( "%c%2", ( i & 1 ) ? '|' : ' ', ch );
        i++;
    }
    *j = 0;
    Output( "%> <%s>" CRLF, 3 * ( 0x10 - i ), ascbuf );
}
