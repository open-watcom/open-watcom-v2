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
#include <string.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/stat.h>

FILE    near __iob[2];

static int File2Hdl( FILE *f )
{
    if( f == &__iob[0] ) return( 0 );
    if( f == &__iob[1] ) return( 1 );
    if( f == &__iob[2] ) return( 2 );
    return( (int) f );
}

unsigned stackavail()
{
    return( ~0 );
}

int fputc( int c, FILE *fp )
{
    char        ch;

    ch = c;
    if( write( File2Hdl( fp ), &ch, sizeof( ch ) ) != sizeof( ch ) ) {
        return( EOF );
    }
    return( c );
}

int fputs( const char *b, FILE *fp )
{
    int         len;

    len = strlen( b );
    if( write( File2Hdl( fp ), b, len ) != len ) return( EOF );
    return( len );
}

char *fgets( char *b, int n, FILE *fp )
{
    int         h;
    char        *start;

    start = b;
    h = File2Hdl( fp );
    --n;
    for( ;; ) {
        if( n == 0 ) break;
        if( read( h, b, 1 ) != 1 ) {
            *b = '\0';
            return( NULL );
        }
        if( *b++ == '\n' ) break;
    }
    *b = '\0';
    return( start );
}

int fprintf( FILE *fp, const char *fmt, ... )
{
    va_list     arg;
    char        buff[128];
    int         len;

    va_start( arg, fmt );
    len = vsprintf( buff, fmt, arg );
    va_end( arg );
    write( File2Hdl( fp ), buff, len );
    return( len );
}

int printf( const char *fmt, ... )
{
    va_list     arg;
    char        buff[128];
    int         len;

    va_start( arg, fmt );
    len = vsprintf( buff, fmt, arg );
    va_end( arg );
    write( 1, buff, len );
    return( len );
}


void rewind( FILE *fp )
{
    lseek( File2Hdl( fp ), 0, SEEK_SET );
}

int fclose( FILE *fp )
{
    return( close( File2Hdl( fp ) ) );
}

FILE *fopen( const char *name, const char *mode )
{
    unsigned    op_mode;
    int         h;

    switch( *mode ) {
    case 'r':
       op_mode = O_RDONLY;
       break;
    case 'w':
        op_mode = O_WRONLY | O_CREAT;
        break;
    case 'a':
        op_mode = O_WRONLY | O_CREAT | O_APPEND;
        break;

    }
    h = open( name, op_mode, 0666 );
    if( h == -1 ) return( NULL );
    return( (FILE *)h );
}
