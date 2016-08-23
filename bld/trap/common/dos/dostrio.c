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


#include <dos.h>
#include <string.h>
#include <stdlib.h>
#include "tinyio.h"
#include "trptypes.h"
#include "digld.h"
#include "servio.h"


extern int KeyPress_pragma( void );
extern int KeyGet_pragma( void );

#pragma aux KeyPress_pragma =   \
    "mov    ah,1"               \
    "int    16h"                \
    "jnz    l1"                 \
    "xor    ax,ax"              \
    "jmp    l2"                 \
    "l1:mov ax,1"               \
    "l2:"                       \
    modify [ax];

#pragma aux KeyGet_pragma =     \
    "xor    ah,ah"              \
    "int    16h"                \
    modify [ax];


void Output( const char *str )
{
    TinyWrite( TINY_ERR, str, strlen( str ) );
}

void SayGNiteGracey( int return_code )
{
    TinyTerminateProcess( return_code );
}

void StartupErr( const char *err )
{
    Output( err );
    Output( "\r\n" );
    SayGNiteGracey( 1 );
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

dig_lhandle DIGLoadOpen( const char *name, unsigned name_len, const char *exts, char *result, unsigned max_result )
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
        return( DIG_NIL_LHANDLE );
    return( TINY_INFO( rc ) );
}

int DIGLoadRead( dig_lhandle lfh, void *buff, unsigned len )
{
    tiny_ret_t  rc;

    rc = TinyFarRead( lfh, buff, len );
    return( TINY_ERROR( rc ) || TINY_INFO( rc ) != len );
}

int DIGLoadSeek( dig_lhandle lfh, unsigned long offs, dig_seek where )
{
    return( TINY_ERROR( TinySeek( lfh, offs, where ) ) );
}

int DIGLoadClose( dig_lhandle lfh )
{
    tiny_ret_t  rc;

    rc = TinyClose( lfh );
    if( TINY_ERROR( rc ) )
        return( -1 );
    return( 0 );
}
