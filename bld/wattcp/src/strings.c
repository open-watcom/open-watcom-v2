#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <dos.h>

#include "wattcp.h"
#include "strings.h"

/*
 * This file contains some gross hacks. Take great care !
 */

#pragma off(check_stack)

extern void _outchar( char c );
#pragma aux _outchar =  \
        "mov ah,2"      \
        "int 21h"       \
        __parm [__dl]   \
        __modify [__ax]

/*
 * Print a single character to stdout.
 */
static void outch( char c )
{
    if( c != '\x1A' ) { /* skip EOF (^Z) character (it causes trouble to stdout) */
        _outchar( c );
    }
}

void (*_outch)(char c) = outch;

/*---------------------------------------------------*/

#if defined(USE_DEBUG)

int (*_printf)( const char*, ... ) = printf;

#else

static int EmptyPrint (const char *fmt, ...)
{
    outsnl("`(*_printf)()' called outside `USE_DEBUG'");
    ARGSUSED( fmt );
    return( 0 );
}

int (*_printf)( const char*, ... ) = EmptyPrint;

#endif

/*---------------------------------------------------*/

void outs( const char *s )
{
    while (*s != '\0') {
        if( *s == '\n' )
            (*_outch)( '\r' );
        (*_outch)( *s++ );
    }
}

void outsnl( const char *s )
{
    outs( s );
    (*_outch)( '\r' );
    (*_outch)( '\n' );
}

void outsn( const char *s, int n )
{
    while( *s != '\0' && n-- >= 0 ) {
        if( *s == '\n' )
            (*_outch)( '\r' );
        (*_outch)( *s++ );
    }
}

void outhex( char c )
{
    char hi = ( c >> 4 ) & 0x0F;
    char lo = c & 0x0F;

    if( hi > 9 ) {
        (*_outch)( (char)( hi - 10 + 'A' ) );
    } else {
        (*_outch)( (char)( hi + '0' ) );
    }
    if( lo > 9 ) {
        (*_outch)( (char)( lo - 10 + 'A' ) );
    } else {
        (*_outch)( (char)( lo + '0' ) );
    }
}

void outhexes( const char *s, int n )
{
    while( n-- > 0 ) {
        outhex( *s++ );
        (*_outch)( ' ' );
    }
}

/*---------------------------------------------------*/

char *rip( char *s )
{
    char *p;

    if( (p = strchr( s, '\n' )) != NULL )
        *p = '\0';
    if( (p = strchr( s, '\r' )) != NULL )
        *p = '\0';
    return( s );
}

/*---------------------------------------------------*/

#ifdef NOT_USED
int isstring( const char *string, unsigned stringlen )
{
    if( (unsigned)strlen( string ) > stringlen - 1 )
        return( 0 );

    while( *string ) {
        if( !isprint( *string++ ) ) {
            string--;
            if( !isspace( *string++ ) ) {
                return( 0 );
            }
        }
    }
    return( 1 );
}
#endif
