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
* Description:  verify test stream log file.
*
****************************************************************************/


#include <stdio.h>
#include <stdarg.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

char buff[256];
char test0[256];
char test1[256];
unsigned line;
char err[1] = { '\0' };

FILE *ofp;

static void fatal( char *msg )
{
    fprintf( stderr, "error on line %u\n", line );
    fprintf( stderr, "%s\n", msg );
    exit( 1 );
}

static void tprintf( char const *f, ... ) {
    va_list args;

    if( !ofp ) {
        ofp = fopen( "chktest.sav", "w" );
        if( ofp == NULL ) {
            fatal( "cannot open output file" );
        }
    }
    va_start( args, f );
    vfprintf( ofp, f, args );
    va_end( args );
    fflush( ofp );
    va_start( args, f );
    vfprintf( stdout, f, args );
    va_end( args );
}

static char *SkipSequenceOf( char *p, char c ) {
    char *s = p;
    while( *p ) {
        if( *p != c ) break;
        ++p;
    }
    if( s == p ) return( err );
    return( p );
}

static char *SkipTime( char *p ) {
    char *s;
    s = p;
    while( *p ) {
        if( ! isdigit( *p ) ) break;
        ++p;
    }
    if( s == p ) return( err );
    if( *p == ':' ) {
        ++p;
    } else {
        return( err );
    }
    s = p;
    while( *p ) {
        if( ! isdigit( *p ) ) break;
        ++p;
    }
    if( s == p ) return( err );
    if( *p == ':' ) {
        ++p;
    } else {
        return( err );
    }
    s = p;
    while( *p ) {
        if( ! isdigit( *p ) ) break;
        ++p;
    }
    if( s == p ) return( err );
    return( p );
}

static char *SkipTestDir( char *p, char *t ) {
    char *s = p;

    while( *p ) {
        if( isspace( *p ) ) break;
        *t++ = *p;
        ++p;
    }
    *t = '\0';
    if( s == p ) return( err );
    return( p );
}

static char *SkipStr( char *p, char *s ) {
    while( *p ) {
        if( *s == '\0' ) break;
        if( *p != *s ) return( err );
        ++p;
        ++s;
    }
    return( p );
}

static int IsCdsayLine( char *t ) {
    char *p;

    p = SkipSequenceOf( buff, '=' );
    if( *p == '\0' ) return( 0 );
    p = SkipSequenceOf( p, ' ' );
    if( *p == '\0' ) return( 0 );
    p = SkipTime( p );
    if( *p == '\0' ) return( 0 );
    p = SkipSequenceOf( p, ' ' );
    if( *p == '\0' ) return( 0 );
    p = SkipTestDir( p, t );
    if( *p == '\0' ) return( 0 );
    p = SkipSequenceOf( p, ' ' );
    if( *p == '\0' ) return( 0 );
    p = SkipSequenceOf( p, '=' );
    return( *p == '\0' && p != err );
}

static int IsPassLine( char *t ) {
    char *p;

    p = SkipStr( buff, "PASS" );
    if( *p == '\0' ) return( 0 );
    p = SkipSequenceOf( p, ' ' );
    if( *p == '\0' ) return( 0 );
    p = SkipTestDir( p, t );
    return( *p == '\0' && p != err );
}

static int IsTestLine( char *t ) {
    char *p;

    p = SkipStr( buff, "TEST" );
    if( *p == '\0' ) return( 0 );
    p = SkipSequenceOf( p, ' ' );
    if( *p == '\0' ) return( 0 );
    p = SkipTestDir( p, t );
    return( *p == '\0' && p != err );
}

static void *my_fgets( FILE *fp ) {
    char *p;

    if( feof( fp ) || fgets( buff, sizeof(buff), fp ) == NULL )
        return( NULL );
    for( p = buff + strlen( buff ) - 1;
        p >= buff && ( isspace(*p) || ( *p == 0x1A ) ); --p ) {
        *p = '\0';
    }
    return( buff );
}

int main( int argc, char **argv ) {
    FILE    *fp;
    char    *p;
    int     state;
    int     retval;
    char    *curr_buff;
    char    *last_buff;

    if( argc != 2 ) {
        fatal( "usage: chktest <file>" );
    }
    fp = fopen( argv[1], "r" );
    if( fp == NULL ) {
        fatal( "cannot open input file" );
    }
    line = 0;
    state = 0;        // 0 - expecting cdsay, 1 - expecting TEST, 2 - expecting PASS
    retval = 0;
    curr_buff = test0;
    last_buff = test1;
    for(;;) {
        if( my_fgets( fp ) == NULL )
            break;
        ++line;
        if( IsCdsayLine( curr_buff ) ) {
            if( state == 1 ) {
                retval = 1;
                tprintf( "line %u: '%s' test failed\n", line, last_buff );
            } else if( state == 2 ) {
                retval = 1;
                tprintf( "line %u: '%s' test failed\n", line, last_buff );
            }
            p = last_buff;
            last_buff = curr_buff;
            curr_buff = p;
            state = 1;
        } else if( IsTestLine( curr_buff ) ) {
            if( state == 0 ) {
                tprintf( "line %u: mismatch - expecting cdsay\n", line );
                retval = 1;
            } else if( state == 2 ) {
                tprintf( "line %u: mismatch - expecting PASS\n", line );
                retval = 1;
            }
            state = 2;
        } else if( IsPassLine( curr_buff ) ) {
            if( state == 0 ) {
                retval = 1;
                tprintf( "line %u: '%s' test passed without cdsay\n", line, curr_buff );
            }
            state = 0;
        } else {
            retval = 1;
            if( state == 0 ) {
                tprintf( "line %u: '%s' invalid contents (expecting cdsay)\n", line, buff );
            } else if( state == 1 ) {
                tprintf( "line %u: '%s' invalid contents (expecting TEST)\n", line, buff );
            } else {
                tprintf( "line %u: '%s' invalid contents (expecting PASS)\n", line, buff );
            }
        }
    }
    if( state == 0 ) {
        retval = 1;
        tprintf( "line %u: mismatch - expecting cdsay\n", line );
    } else if( state == 2 ) {
        retval = 1;
        tprintf( "line %u: '%s' test failed\n", line, last_buff );
    }
    fclose( fp );
    if( ofp ) {
        fclose( ofp );
    }
    return( retval );
}
