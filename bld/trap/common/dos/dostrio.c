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
* Description:  DOS specific trap I/O.
*
****************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dos.h>
#include "tinyio.h"
#include "trptypes.h"
#include "digcli.h"
#include "digld.h"
#include "servio.h"


#define HANDLE2FP(ph)    ((FILE *)((unsigned long)(ph) + 1))
#define FP2HANDLE(fp)    ((int)((unsigned long)(fp) - 1))

extern int KeyPress_pragma( void );
#pragma aux KeyPress_pragma =   \
        "mov  ah,1"     \
        "int 16h"       \
        "jnz short L1"  \
        "xor  ax,ax"    \
        "jmp short L2"  \
    "L1: mov  ax,1"     \
    "L2:"               \
    __parm      [] \
    __value     [__ax] \
    __modify    [__ax]

extern int KeyGet_pragma( void );
#pragma aux KeyGet_pragma = \
        "xor    ah,ah"  \
        "int    16h"    \
    __parm      [] \
    __value     [__ax] \
    __modify    [__ax]


void Output( const char *str )
{
    TinyWrite( TINY_ERR, str, strlen( str ) );
}

void SayGNiteGracey( int return_code )
{
    TinyTerminateProcess( return_code );
    // never return
}

void StartupErr( const char *err )
{
    Output( err );
    Output( "\r\n" );
    SayGNiteGracey( 1 );
    // never return
}

int KeyPress( void )
{
    return( KeyPress_pragma() );
}

int KeyGet( void )
{
    return( KeyGet_pragma() );
}

int WantUsage( const char *ptr )
{
    if( (*ptr == '-') || (*ptr == '/') )
        ++ptr;
    return( *ptr == '?' );
}

FILE *DIGLoader( Open )( const char *name, unsigned name_len, const char *exts, char *result, unsigned max_result )
{
    bool        has_ext;
    bool        has_path;
    const char  *src;
    char        *dst;
    char        trpfile[256];
    tiny_ret_t  rc;
    char        c;

    result = result; max_result = max_result;
    has_ext = false;
    has_path = false;
    src = name;
    dst = trpfile;
    while( name_len-- > 0 ) {
        c = *src++;
        *dst++ = c;
        switch( c ) {
        case '.':
            has_ext = true;
            break;
        case '/':
        case '\\':
            has_ext = false;
                /* fall through */
        case ':':
            has_path = true;
            break;
        }
    }
    if( !has_ext ) {
        *dst++ = '.';
        name_len = strlen( exts );
        memcpy( dst, exts, name_len );
        dst += name_len;
    }
    *dst = '\0';
    src = trpfile;
    if( !has_path ) {
        _searchenv( trpfile, "PATH", RWBuff );
        src = RWBuff;
    }
    rc = TinyOpen( src, TIO_READ );
    if( TINY_ERROR( rc ) )
        return( NULL );
    return( HANDLE2FP( TINY_INFO( rc ) ) );
}

int DIGLoader( Read )( FILE *fp, void *buff, size_t len )
{
    tiny_ret_t  rc;

    rc = TinyFarRead( FP2HANDLE( fp ), buff, len );
    return( TINY_ERROR( rc ) || TINY_INFO( rc ) != len );
}

int DIGLoader( Seek )( FILE *fp, unsigned long offs, dig_seek where )
{
    return( TINY_ERROR( TinySeek( FP2HANDLE( fp ), offs, where ) ) );
}

int DIGLoader( Close )( FILE *fp )
{
    return( TINY_ERROR( TinyClose( FP2HANDLE( fp ) ) ) );
}
