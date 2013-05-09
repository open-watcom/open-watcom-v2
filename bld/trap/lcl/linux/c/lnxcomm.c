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
* Description:  Linux trap file communication functions.
*
****************************************************************************/

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>
#include <sys/stat.h>
#include <sys/time.h>
#include "exeelf.h"
#include "lnxcomm.h"
#include "trpimp.h"

void print_msg( const char *format, ... )
{
    va_list     args;
    static char buf[2048];

    va_start( args, format );
    vsprintf( buf, format, args );
    write( 1, buf, strlen( buf ) );
    va_end( args );
}

char *StrCopy( char *src, char *dst )
{
    while( (*dst = *src) ) {
        ++src;
        ++dst;
    }
    return( dst );
}

unsigned TryOnePath( char *path, struct stat *tmp, char *name, char *result )
{
    char        *end;
    char        *ptr;

    if( path == NULL ) return( 0 );
    ptr = result;
    for( ;; ) {
        switch( *path ) {
        case ':':
        case '\0':
            if( ptr != result && ptr[-1] != '/' )
                *ptr++ = '/';
            end = StrCopy( name, ptr );
            if( stat( result, tmp ) == 0 )
                return( end - result );
            if( *path == '\0' )
                return( 0 );
            ptr = result;
            break;
        case ' ':
        case '\t':
            break;
        default:
            *ptr++ = *path;
            break;
        }
        ++path;
    }
}

unsigned FindFilePath( int exe, char *name, char *result )
{
    struct stat tmp;
    unsigned    len;
    char        *end;

    if( stat( name, &tmp ) == 0 ) {
        end = StrCopy( name, result );
        return( end - result );
    }
    // TODO: Need to find out how to get at the environment for the
    //       debug server process (I think!).
    if( exe ) {
        return( TryOnePath( getenv( "PATH" ), &tmp, name, result ) );
    } else {
        len = TryOnePath( getenv( "WD_PATH" ), &tmp, name, result );
        if( len != 0 ) return( len );
        len = TryOnePath( getenv( "HOME" ), &tmp, name, result );
        if( len != 0 ) return( len );
        return( TryOnePath( "/usr/watcom/wd", &tmp, name, result ) );
    }
    return 0;
}

trap_elen ReqRead_user_keyboard( void )
{
    read_user_keyboard_req  *acc;
    read_user_keyboard_ret  *ret;
    fd_set                  rdfs;
    struct timeval          tv;
    struct timeval          *ptv;
    struct termios          old, new;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    CONV_LE_16( acc->wait );

    tcgetattr( STDIN_FILENO, &old );
    new = old;
    new.c_iflag &= ~(IXOFF | IXON);
    new.c_lflag &= ~(ECHO | ICANON | NOFLSH);
    new.c_lflag |= ISIG;
    new.c_cc[VMIN] = 1;
    new.c_cc[VTIME] = 0;
    tcsetattr( STDIN_FILENO, TCSADRAIN, &new );

    FD_ZERO( &rdfs );
    FD_SET( STDIN_FILENO, &rdfs );
    tv.tv_sec = acc->wait;
    tv.tv_usec = 0;
    ptv = &tv;
    if( acc->wait == 0 ) ptv = NULL;

    ret->key = '\0';
    if ( select( 1, &rdfs, NULL, NULL, ptv ) )
        read( STDIN_FILENO, &ret->key, 1 );

    tcsetattr( STDIN_FILENO, TCSADRAIN, &old );
    return( sizeof( *ret ) );
}

trap_elen ReqGet_err_text( void )
{
    get_err_text_req    *acc;
    char                *err_txt;

    acc = GetInPtr( 0 );
    CONV_LE_32( acc->err );
    err_txt = GetOutPtr( 0 );
    strcpy( err_txt, strerror( acc->err ) );
    return( strlen( err_txt ) + 1 );
}

trap_elen ReqSplit_cmd( void )
{
    char                *cmd;
    char                *start;
    split_cmd_ret       *ret;
    trap_elen           len;

    cmd = GetInPtr( sizeof( split_cmd_req ) );
    ret = GetOutPtr( 0 );
    start = cmd;
    len = GetTotalSize() - sizeof( split_cmd_req );
    for( ;; ) {
        if( len == 0 ) break;
        switch( *cmd ) {
        case '\0':
        case ' ':
        case '\t':
            ret->parm_start = cmd - start + 1;
            ret->cmd_end = cmd - start;
            CONV_LE_16( ret->cmd_end );
            CONV_LE_16( ret->parm_start );
            return( sizeof( *ret ) );
        }
        ++cmd;
        --len;
    }
    ret->parm_start = cmd - start;
    ret->cmd_end = cmd - start;
    CONV_LE_16( ret->cmd_end );
    CONV_LE_16( ret->parm_start );
    return( sizeof( *ret ) );
}

trap_elen ReqGet_next_alias( void )
{
    get_next_alias_ret  *ret;

    ret = GetOutPtr( 0 );
    ret->seg = 0;
    ret->alias = 0;
    return( sizeof( *ret ) );
}

trap_elen ReqSet_user_screen( void )
{
    return( 0 );
}

trap_elen ReqSet_debug_screen( void )
{
    return( 0 );
}
