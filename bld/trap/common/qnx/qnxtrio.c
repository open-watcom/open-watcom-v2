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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <termios.h>
#include <sys/kernel.h>
#include <sys/dev.h>
#include <sys/proc_msg.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include <process.h>
#include "trpimp.h"
#include "servio.h"
#include "digio.h"


void Output( const char *str )
{
    write( STDERR_FILENO, str, strlen( str ) );
}

void SayGNiteGracey( int return_code )
{
    _exit( return_code );
}

void StartupErr( const char *err )
{
    Output( err );
    Output( "\n" );
    SayGNiteGracey( 1 );
}

int KeyPress( void )
{
    int             ret;
    struct termios  old;
    struct termios  new;

    tcgetattr( 0, &old );
    new = old;
    new.c_lflag &= ~(ICANON | ECHO);
    new.c_cc[VMIN] = 1;
    new.c_cc[VTIME] = 0;
    tcsetattr( 0, TCSANOW, &new );

    ret = dev_ischars( 0 );
    tcsetattr( 0, TCSANOW, &old );
    return( ret != 0 );
}

int KeyGet( void )
{
    struct termios  old;
    struct termios  new;
    char            key;

    tcgetattr( 0, &old );
    new = old;
    new.c_lflag &= ~(ICANON | ECHO);
    new.c_cc[VMIN] = 1;
    new.c_cc[VTIME] = 0;
    tcsetattr( 0, TCSANOW, &new );
    read( 0, &key, 1 );
    tcsetattr( 0, TCSANOW, &old );
    return( key );
}

static char *StrCopy( const char *src, char *dst )
{
    while( *dst = *src ) {
        ++src;
        ++dst;
    }
    return( dst );
}

int WantUsage( const char *ptr )
{
    /*
        This is a really stupid place to put this, but it's the first
        system dependant code that the servers run. This code sets the
        effective GID and UID back to the real ones so that the server
        can be made set UID root without being a security hole. That allows
        use to run at ring 1 for the parallel server/trap file.
    */
    setegid( getgid() );
    seteuid( getuid() );
    if( *ptr == '?' )
        return( TRUE );
    return( FALSE );
}

static unsigned TryOnePath( const char *path, struct stat *tmp, const char *name,
                         char *result )
{
    char        *end;
    char        *ptr;

    if( path == NULL )
        return( 0 );
    ptr = result;
    for( ;; ) {
        if( *path == '\0' || *path == ':' ) {
            if( ptr != result )
                *ptr++ = '/';
            end = StrCopy( name, ptr );
            if( stat( result, tmp ) == 0 )
                return( end - result );
            if( *path == '\0' )
                return( 0 );
            ++path;
            ptr = result;
        }
        if( *path != ' ' && *path != '\t' ) {
            *ptr++ = *path;
        }
        ++path;
    }
}

static unsigned FindFilePath( const char *name, char *result )
{
    struct stat tmp;
    unsigned    len;
    char        *end;
    char        cmd[256];

    if( stat( name, &tmp ) == 0 ) {
        end = StrCopy( name, result );
        return( end - result );
    }
    len = TryOnePath( getenv( "WD_PATH" ), &tmp, name, result );
    if( len != 0 )
        return( len );
    len = TryOnePath( getenv( "HOME" ), &tmp, name, result );
    if( len != 0 )
        return( len );
    if( _cmdname( cmd ) != NULL ) {
        end = strrchr( cmd, '/' );
        if( end != NULL ) {
            *end = '\0';
            end = strrchr( cmd, '/' );
            if( end != NULL ) {
                /* look in the wd sibling directory of where the command
                   came from */
                StrCopy( "wd", end + 1 );
                len = TryOnePath( cmd, &tmp, name, result );
                if( len != 0 ) {
                    return( len );
                }
            }
        }
    }
    return( TryOnePath( "/usr/watcom/wd", &tmp, name, result ) );
}

dig_lhandle DIGLoadOpen( const char *name, size_t name_len, const char *ext, char *result, size_t max_result )
{
    bool            has_ext;
    bool            has_path;
    char            *ptr;
    const char      *endptr;
    char            trpfile[256];
    int             fh;

    result = result; max_result = max_result;
    has_ext = FALSE;
    has_path = FALSE;
    endptr = name + name_len;
    for( ptr = name; ptr != endptr; ++ptr ) {
        switch( *ptr ) {
        case '.':
            has_ext = TRUE;
            break;
        case '/':
            has_ext = FALSE;
            has_path = TRUE;
            /* fall through */
            break;
        }
    }
    memcpy( trpfile, name, name_len );
    trpfile[name_len] = '\0';
    if( !has_ext ) {
        trpfile[name_len++] = '.';
        memcpy( trpfile + name_len, exts, strlen( exts ) + 1 );
    }
    fh = -1;
    if( has_path ) {
        fh = open( trpfile, O_RDONLY );
    } else if( FindFilePath( trpfile, RWBuff ) ) {
        fh = open( RWBuff, O_RDONLY );
    }
    if( fh == -1 )
        return( DIG_NIL_LHANDLE );
    return( fh );
}

int DIGLoadRead( dig_lhandle lfh, void *buff, unsigned len )
{
    return( read( lfh, buff, len ) != len );
}

int DIGLoadSeek( dig_lhandle lfh, unsigned long offs, dig_seek where )
{
    return( lseek( lfh, offs, where ) == -1L );
}

int DIGLoadClose( dig_lhandle lfh )
{
    return( close( lfh ) );
}

void *DIGCLIENTRY( Alloc )( size_t amount )
{
    return( malloc( amount ) );
}

void DIGCLIENTRY( Free )( void *p )
{
    free( p );
}
