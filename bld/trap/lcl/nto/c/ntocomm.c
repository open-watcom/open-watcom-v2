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
* Description:  QNX Neutrino common trap file routines.
*
****************************************************************************/


#include <string.h>
#include <termios.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/time.h>
#include "trpimp.h"


char *StrCopy( char *src, char *dst )
{
    while( (*dst = *src) ) {
        ++src;
        ++dst;
    }
    return( dst );
}

#if 0
// TODO: NIDs don't exist on Neutrino. There is a different mechanism
// available to support distributed computing.
char *CollectNid( char *ptr, unsigned len, nid_t *nidp )
{
    char        *start;
    nid_t       nid;
    char        ch;

    *nidp = 0;
    start = ptr;
    if( ptr[0] != '/' || ptr[1] != '/' ) {
        return( ptr );
    }
    len -= 2;
    ptr += 2;
    nid = 0;
    // NYI: will need beefing up when NID's can be symbolic
    for( ;; ) {
        if( len == 0 ) break;
        ch = *ptr;
        if( ch < '0' || ch > '9' ) break;
        nid = (nid * 10) + ( ch - '0' );
        ++ptr;
        --len;
    }
    *nidp = nid;
    // NYI: how do I check to see if NID is valid?
    if( len == 0 ) {
        return( ptr );
    }
    switch( ptr[0] ) {
    case ' ':
    case '\t':
    case '\0':
        break;
    default:
        *nidp = 0;
        return( start );
    }
    return( ptr );
}
#endif

unsigned TryOnePath( char *path, struct stat *tmp, char *name,
                         char *result )
{
    char        *end;
    char        *ptr;

    if( path == NULL ) return( 0 );
    ptr = result;
    for( ;; ) {
        switch( *path ) {
        case ':':
        case '\0':
            if( ptr != result && ptr[-1] != '/' ) *ptr++ = '/';
            end = StrCopy( name, ptr );
            //NYI: really should check permission bits
            if( stat( result, tmp ) == 0 ) return( end - result );
            if( *path == '\0' ) return( 0 );
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

unsigned FindFilePath( bool exe, char *name, char *result )
{
    struct stat tmp;
    unsigned    len;
    char        *end;

    if( stat( (char *)name, &tmp ) == 0 ) {
        end = StrCopy( name, result );
        return( end - result );
    }
    if( exe ) {
        return( TryOnePath( getenv( "PATH" ), &tmp, name, result ) );
    } else {
        len = TryOnePath( getenv( "WD_PATH" ), &tmp, name, result );
        if( len != 0 ) return( len );
        len = TryOnePath( getenv( "HOME" ), &tmp, name, result );
        if( len != 0 ) return( len );
        return( TryOnePath( "/usr/watcom/wd", &tmp, name, result ) );
    }
}


trap_retval ReqRead_user_keyboard( void )
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


trap_retval ReqGet_err_text( void )
{
    get_err_text_req    *acc;
    char                *err_txt;

    acc = GetInPtr( 0 );
    CONV_LE_32( acc->err );
    err_txt = GetOutPtr( 0 );
    strcpy( err_txt, strerror( acc->err ) );
    return( strlen( err_txt ) + 1 );
}


trap_retval ReqSplit_cmd( void )
{
    char                ch;
    char                *cmd;
    char                *start;
    split_cmd_ret       *ret;
    unsigned            len;
//    nid_t               nid;

    cmd = GetInPtr( sizeof( split_cmd_req ) );
    len = GetTotalSize() - sizeof( split_cmd_req );
    start = cmd;
    ret = GetOutPtr( 0 );
    ret->parm_start = 0;
//    cmd = CollectNid( cmd, len, &nid );
    len -= cmd - start;
    while( len != 0 ) {
        ch = *cmd;
        if( !( ch == '\0' || ch == ' ' || ch == '\t' ) ) break;
        ++cmd;
        --len;
    }
    while( len != 0 ) {
        switch( *cmd ) {
        case '\0':
        case ' ':
        case '\t':
            ret->parm_start = 1;
            len = 0;
            continue;
        }
        ++cmd;
    }
    ret->parm_start += cmd - start;
    ret->cmd_end = cmd - start;
    CONV_LE_16( ret->cmd_end );
    CONV_LE_16( ret->parm_start );
    return( sizeof( *ret ) );
}

trap_retval ReqGet_next_alias( void )
{
    get_next_alias_ret  *ret;

    ret = GetOutPtr( 0 );
    ret->seg = 0;
    ret->alias = 0;
    return( sizeof( *ret ) );
}

trap_retval ReqSet_user_screen( void )
{
    return( 0 );
}

trap_retval ReqSet_debug_screen( void )
{
    return( 0 );
}
