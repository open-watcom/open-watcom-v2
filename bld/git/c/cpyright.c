/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2019-2020 The Open Watcom Contributors. All Rights Reserved.
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


#define PROJECT_TITLE   "Open Watcom Project"
#define SYBASE_CPYRIGHT "-2002 Sybase, Inc. All Rights Reserved."
#define CPYRIGHT1   	"* Copyright (c) 2002-"
#define CPYRIGHT2   	"xxxx"
#define CPYRIGHT3   	" The Open Watcom Contributors. All Rights Reserved."

#define CPYRIGHT1_LEN   (sizeof( CPYRIGHT1 ) - 1)
#define CPYRIGHT2_LEN   (sizeof( CPYRIGHT2 ) - 1)
#define CPYRIGHT3_LEN   (sizeof( CPYRIGHT3 ) - 1)

#define GML2        ":cmt"

#define IS_ASM(p)   (p[0] == ';' && p[1] == '*')
#define IS_GML1(p)  (p[0] == '.' && p[1] == '*')
#define IS_GML2(p)  (p[0] == GML2[0] && p[1] == GML2[1] && p[2] == GML2[2] && p[3] == GML2[3])

static char     cpyright[] = CPYRIGHT1 CPYRIGHT2 CPYRIGHT3 "\n";
static size_t   line = 1;
static size_t   start_line = 0;
static size_t   size = 0;
static int      status = 0;
static char     *buffer = NULL;
static FILE     *fi = NULL;
static FILE     *fo = NULL;

static void output_buffer( void )
{
    char    *start;

    if( status < 2 && line < 30 ) {
        if( strstr( buffer, "****************************************************************************" ) != NULL ) {
            start_line = line;
            status = 1;
        } else if( status == 1 && line == start_line + 2 ) {
            if( strstr( buffer, PROJECT_TITLE ) != NULL ) {
                status = 2;
            }
        }
    } else if( line == start_line + 4 ) {
        if( status == 2 ) {
            if( strstr( buffer, SYBASE_CPYRIGHT ) != NULL ) {
                if( IS_ASM( buffer ) ) {
                    fputc( ';', fo );
                } else if( IS_GML1( buffer ) ) {
                    fputc( '.', fo );
                } else if( IS_GML2( buffer ) ) {
                    fputs( GML2, fo );
                    fputc( ' ', fo );
                } else {
                }
                fputs( cpyright, fo );
                status = 3;
            } else if( strstr( buffer, CPYRIGHT3 ) != NULL ) {
                if( IS_ASM( buffer ) || IS_GML1( buffer ) ) {
                    start = buffer + 1;
                } else if( IS_GML1( buffer ) ) {
                    start = buffer + 1;
                } else if( IS_GML2( buffer ) ) {
                    start = buffer + sizeof( GML2 ) - 1 + 1;
                } else {
                    start = buffer;
                }
                memcpy( start + CPYRIGHT1_LEN, cpyright + CPYRIGHT1_LEN, 4 );
                status = 3;
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
    memcpy( cpyright + CPYRIGHT1_LEN, cyear, 4 );
    if( argc > 1 ) {
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
        buffer[len++] = (char)c;
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
