/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2019-2019 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  GIT filter driver for copyright notes.
*
****************************************************************************/


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "bool.h"


static char     cpyright[] = "* Copyright (c) 2002-xxxx The Open Watcom Contributors. All Rights Reserved.\n";
static size_t   line = 1;
static size_t   size = 0;
static int      status = 0;
static char     *buffer = NULL;
static FILE     *fi = NULL;
static FILE     *fo = NULL;

static void output_buffer( void )
{
    if( line == 3 ) {
        if( status == 0 ) {
            if( strstr( buffer, "Open Watcom Project" ) != NULL ) {
                status = 1;
            }
        }
    } else if( line == 5 ) {
        if( status == 1 ) {
            if( strstr( buffer, "-2002 Sybase, Inc. All Rights Reserved." ) != NULL ) {
                fputs( cpyright, fo );
                status = 2;
            } else if( strstr( buffer, cpyright + 25 ) != NULL ) {
                memcpy( buffer + 21, cpyright + 21, 4 );
                status = 2;
            }
        }
    }
    fputs( buffer, fo );
}

static bool filecopy( void )
{
    size_t      bufflen;
    bool        ok;

    ok = true;
    while( (bufflen = fread( buffer, 1, size, fi )) == size ) {
        if( fwrite( buffer, 1, size, fo ) != size ) {
            ok = false;
        }
    }
    if( ferror( fi ) ) {
        ok = false;
    }
    if( bufflen > 0 ) {
        if( fwrite( buffer, 1, bufflen, fo ) != bufflen ) {
            ok = false;
        }
    }
    return( ok );
}

int main( int argc, char *argv[] )
{
    size_t          len;
    char            cyear[6];
    time_t          ltime;
    const struct tm *t;
    int             c;

    time( &ltime );
    t = localtime( &ltime );
    sprintf( cyear, "%4.4d", 1900 + t->tm_year );
    memcpy( cpyright + 21, cyear, 4 );
    if( argc > 2 ) {
        fi = fopen( argv[1], "rt" );
    } else if( argc == 2 ) {
        fi = fopen( argv[1], "rt" );
    } else {
        fi = stdin;
    }
    if( fi == NULL )
        return( 1 );
    if( argc > 2 ) {
        fo = fopen( argv[2], "wt" );
    } else if( argc == 2 ) {
        fo = tmpfile();
    } else {
        fo = stdout;
    }
    if( fo == NULL )
        return( 1 );
    len = 0;
    while( (c = fgetc( fi )) != EOF ) {
        if( len + 2 > size ) {
            size += 512;
            buffer = realloc( buffer, size );
            if( buffer == NULL ) {
                return( 1 );
            }
        }
        buffer[len++] = c;
        if( c == '\n' ) {
            buffer[len] = '\0';
            output_buffer();
            line++;
            len = 0;
        }
    }
    if( len > 0 ) {
        buffer[len] = '\0';
        output_buffer();
    }
    fflush( fo );
    if( fo != stdout ) {
        if( argc > 2 ) {
            fclose( fo );
        } else if( argc == 2 ) {
            rewind( fo );
            fi = fo;
            fo = fopen( argv[1], "wt" );
            if( fo == NULL )
                return( 1 );
            filecopy();
            fflush( fo );
            fclose( fo );
        }
    }
    return( 0 );
}
