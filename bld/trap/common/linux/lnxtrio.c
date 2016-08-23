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
* Description:  Trap I/O functions for Linux
*
****************************************************************************/


#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <termios.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include <sys/time.h>
#if defined(__WATCOMC__)
    #include <process.h>
#endif
#include "trptypes.h"
#include "digld.h"
#include "servio.h"


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
    struct timeval  tv;
    fd_set          rdfs;

    tcgetattr( 0, &old );
    new = old;
    new.c_lflag &= ~(ICANON | ECHO);
    new.c_cc[VMIN] = 1;
    new.c_cc[VTIME] = 0;
    tcsetattr( 0, TCSANOW, &new );

    FD_ZERO( &rdfs );
    FD_SET( 0, &rdfs );
    tv.tv_sec = 0;
    tv.tv_usec = 0;
    ret = select(1, &rdfs, NULL, NULL, &tv );

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

int WantUsage( const char *ptr )
{
    if( *ptr == '-' )
        ++ptr;
    return( *ptr == '?' );
}

static char *StrCopy( const char *src, char *dst )
{
    while( (*dst = *src) ) {
        ++src;
        ++dst;
    }
    return( dst );
}

static unsigned TryOnePath( const char *path, struct stat *tmp, const char *name, char *result )
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
            /* look in the executable's directory */
            len = TryOnePath( cmd, &tmp, name, result );
            if( len != 0 )
                return( len );
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
    return( TryOnePath( "/opt/watcom/wd", &tmp, name, result ) );
}

dig_lhandle DIGLoadOpen( const char *name, unsigned name_len, const char *ext, char *result, unsigned max_result )
{
    bool                has_ext;
    bool                has_path;
    const char          *src;
    char                *dst;
    char                trpfile[256];
    int                 fh;
    char                c;

    max_result = max_result;
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
            has_ext = false;
            has_path = true;
            /* fall through */
            break;
        }
    }
    if( !has_ext ) {
        *dst++ = '.';
        name_len = strlen( ext );
        memcpy( dst, ext, name_len );
        dst += name_len;
    }
    *dst = '\0';
    fh = -1;
    if( has_path ) {
        fh = open( trpfile, O_RDONLY );
    } else if( FindFilePath( trpfile, result ) ) {
        fh = open( result, O_RDONLY );
    }
    if( fh == -1 )
        return( DIG_NIL_LHANDLE );
    return( fh );
}

#if 0
int DIGLoadRead( dig_lhandle lfh, void *buff, unsigned len )
{
    return( read( lfh, buff, len ) != len );
}

int DIGLoadSeek( dig_lhandle lfh, unsigned long offs, dig_seek where )
{
    return( lseek( lfh, offs, where ) == -1L );
}
#endif

int DIGLoadClose( dig_lhandle lfh )
{
    return( close( lfh ) );
}
