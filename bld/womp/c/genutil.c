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
* Description:  Generic utility function primarily for console output.
*
****************************************************************************/


#include <io.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <watcom.h>
#include "womp.h"
#include "genutil.h"
#include "myassert.h"
#include "cmdline.h"


STATIC char *strHex2( char *dest, uint_8 val ) {

    static char hex_digits[] = "0123456789abcdef";

    *dest++ = hex_digits[ val >> 4   ];
    *dest++ = hex_digits[ val & 0x0f ];
    return( dest );
}

STATIC char *strHex4( char *dest, uint_16 val ) {

    return( strHex2( strHex2( dest, val >> 8 ), val & 0xff ) );
}

STATIC char *strHex8( char *dest, uint_32 val ) {

    return( strHex4( strHex4( dest, val >> 16 ), val & 0xffff ) );
}

char *StrDec( char *dest, unsigned short num ) {

    char    *orig;
    char    *str;
    div_t    res;

/**/myassert( dest != NULL );

    orig = dest;
    str = dest + ( 3 * sizeof( unsigned short ) / sizeof( char ) );
    res.quot = num;
    do {
        ++dest;
        res = div( res.quot, 10 );
        *--str = '0' + res.rem;
    } while( res.quot != 0 );

    while( dest > orig ) {
        *orig++ = *str++;
    }
    *dest = 0;

    return( dest );
}

#define MXBUF   128
#define MXFMT   20          /* maximum amount a non %s fmt option will need */
#define MNPRINT (MXBUF-MXFMT)

STATIC size_t doFmt( const char *fmt, va_list args ) {

    char    buf[MXBUF];    /* internal buffer */
    char    *dest;         /* pointer inside internal buffer */
    size_t  printed;
    char    *str;
    int     saved_errno;    /* cause we want to use write */
    char    msgbuff[MAX_RESOURCE_SIZE];

/**/myassert( fmt != NULL );
    saved_errno = errno;
    printed = 0;
    dest = buf;
    while( *fmt ) {
        if( *fmt != '%' ) {
            *dest++ = *fmt;
            if( dest - buf >= MNPRINT ) {
                printed += write( STDOUT, buf, dest - buf );
                dest = buf;
            }
        } else {
            ++fmt;
/**/        myassert( *fmt != 0 );
            switch( *fmt ) {
            case '1':
            case '2':
                if( *(fmt+1) == 'X' ) {
                    *dest++ = '0';
                    *dest++ = 'x';
                    dest = strHex8( dest, va_arg( args, uint_32 ) );
                    ++fmt;
                } else {
                    *dest++ = *fmt;
                }
                break;
            case 'c':
                *dest++ = va_arg( args, char );
                break;
            case 's':
                printed += write( STDOUT, buf, dest - buf );
                dest = buf;
                str = va_arg( args, char * );
                printed += write( STDOUT, str, strlen( str ) );
                break;
            case 'M':
                printed += write( STDOUT, buf, dest - buf );
                dest = buf;
                MsgGet( va_arg( args, int ), msgbuff );
                str = msgbuff;
                printed += write( STDOUT, str, strlen( str ) );
                break;
            case 'x':
                *dest++ = '0';
                *dest++ = 'x';
                dest = strHex4( dest, va_arg( args, uint_16 ) );
                break;
            case 'X':
                *dest++ = '0';
                *dest++ = 'x';
                dest = strHex8( dest, va_arg( args, uint_32 ) );
                break;
            case 't':
                dest = strHex2( dest, va_arg( args, uint_8 ) );
                break;
            case 'u':
                dest = StrDec( dest, va_arg( args, unsigned ) );
                break;
            case 'Z':
                printed += write( STDOUT, buf, dest - buf );
                dest = buf;
                MsgGet( MSG_SYS_ERR_0+saved_errno, msgbuff );
                str = msgbuff;
                printed += write( STDOUT, str, strlen( str ) );
                break;
            default:
                *dest++ = *fmt;
            }
        }
        ++fmt;
    }
    printed += write( STDOUT, buf, dest - buf );
    va_end( args );
    return( printed );
}

size_t PrtFmt( const char *fmt, ... ) {
/***********************************/
    va_list args;

    va_start( args, fmt );
    return( doFmt( fmt, args ) );
}

STATIC void doPrtMsg( unsigned msg, va_list args ) {

    char        msgbuff[MAX_RESOURCE_SIZE];

    if( msg & WRN ) {
        if( BeQuiet() ) {
            return;
        }
        MsgGet( MSG_WARNING, msgbuff );
        PrtFmt( msgbuff );
    }
    MsgGet( msg & ~WRN, msgbuff );
    doFmt( msgbuff, args );
    write( STDOUT, "\n", 1 );
}

void PrtMsg( unsigned msg, ... ) {
/*********************************************/

    va_list args;

    va_start( args, msg );
    doPrtMsg( msg, args );
}

void Fatal( unsigned msg, ... ) {
/********************************************/

    va_list args;

    va_start( args, msg );
    doPrtMsg( msg, args );
    exit( EXIT_FAILURE );
}

void Fatal2X( unsigned msg, uint_32 firstx, uint_32 secondx ) {
/*************************************************************/

    char        msgbuff[MAX_RESOURCE_SIZE];

    MsgGet( msg, msgbuff );
    if( strstr( strstr( msgbuff, "%1X" ), "%2X" ) ) {
        PrtFmt( msgbuff, firstx, secondx );
    } else {
        PrtFmt( msgbuff, secondx, firstx );
    }
    write( STDOUT, "\n", 1 );
    exit( EXIT_FAILURE );
}

#ifndef NDEBUG
int InternalError( const char *file, unsigned line ) {
/***************************************************/

    PrtFmt( "Internal error in %s(%u)\n", file, line );
    exit( EXIT_FAILURE );
	return( 0 );
}
#endif
