/*
**********************************************************************
*       Copyright by WATCOM International Corporation, 1987, 1995.   *
*       All rights reserved. No part of this software may be         *
*       reproduced in any form or by any means - graphic, electronic,*
*       mechanical or otherwise, including, without limitation,      *
*       photocopying, recording, taping or information storage and   *
*       retrieval systems - except with the written permission of    *
*       WATCOM International Corporation.                            *
**********************************************************************

CHKTEST -- verify test stream log file

Modified        By              Reason
--------        --              ------
95/12/31	A.F.Scian	defined
*/
#include <stdio.h>
#include <stdarg.h>
#include <ctype.h>
#include <stdlib.h>

char buff[256];
char test0[256];
char test1[256];
unsigned line;
char err[1];

FILE *ofp;

void fatal( char *msg )
{
    fprintf( stderr, "error on line %u\n", line );
    fprintf( stderr, "%s\n", msg );
    exit( 1 );
}

void tprintf( char const *f, ... ) {
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

char *SkipSequenceOf( char *p, char c ) {
    char *s = p;
    while( *p ) {
	if( *p != c ) break;
	++p;
    }
    if( s == p ) return( err );
    return( p );
}

char *SkipTime( char *p ) {
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

char *SkipTestDir( char *p, char *t ) {
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

char *SkipStr( char *p, char *s ) {
    while( *p ) {
	if( *s == '\0' ) break;
	if( *p != *s ) return( err );
	++p;
	++s;
    }
    return( p );
}

int IsCdsayLine( char *t ) {
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
    return( p[0] == '\n' && p[1] == '\0' );
}

int IsPassLine( char *t ) {
    char *p;

    p = SkipStr( buff, "PASS" );
    if( *p == '\0' ) return( 0 );
    p = SkipSequenceOf( p, ' ' );
    if( *p == '\0' ) return( 0 );
    p = SkipTestDir( p, t );
    if( *p == '\0' ) return( 0 );
    return( p[0] == '\n' && p[1] == '\0' );
}

static void cleanUp( char *p ) {
    while( *p ) {
	if( *p == '\n' ) {
	    *p = ' ';
	}
	++p;
    }
}

int main( int argc, char **argv ) {
    FILE *fp;
    char *check;
    int state;
    int ret;
    char *curr_buff;
    char *last_buff;

    if( argc != 2 ) {
	fatal( "usage: chktest <file>" );
    }
    fp = fopen( argv[1], "r" );
    if( fp == NULL ) {
	fatal( "cannot open input file" );
    }
    line = 0;
    state = 0;	// 0 - expecting cdsay, 1 - expecting PASS
    ret = 0;
    curr_buff = test0;
    last_buff = test1;
    for(;;) {
	check = fgets( buff, sizeof(buff), fp );
	if( check == NULL ) break;
	++line;
	if( state == 0 ) {
	    if( IsCdsayLine( curr_buff ) ) {
		state = 1;
	    } else {
		if( IsPassLine( curr_buff ) ) {
		    tprintf( "line %u: '%s' test passed without cdsay\n", line, curr_buff );
		    ret = 1;
		} else {
		    cleanUp( buff );
		    tprintf( "line %u: '%s' invalid contents (expecting cdsay)\n", line, buff );
		    ret = 1;
		}
	    }
	} else {
	    if( IsPassLine( curr_buff ) ) {
		state = 0;
	    } else {
		if( IsCdsayLine( curr_buff ) ) {
		    tprintf( "line %u: '%s' test failed\n", line, last_buff );
		    ret = 1;
		} else {
		    cleanUp( buff );
		    tprintf( "line %u: '%s' invalid contents (expecting PASS)\n", line, buff );
		    ret = 1;
		}
	    }
	}
	if( curr_buff == test0 ) {
	    curr_buff = test1;
	    last_buff = test0;
	} else {
	    curr_buff = test0;
	    last_buff = test1;
	}
    }
    if( state != 1 ) {
	tprintf( "line %u: missing cdsay for this test\n", line );
	ret = 1;
    }
    fclose( fp );
    if( ofp ) {
	fclose( ofp );
    }
    return( ret );
}
