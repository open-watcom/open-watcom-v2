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


#include <stdio.h>
#include <limits.h>
#include "yacc.h"
#include "alloc.h"

unsigned wperset;
short *setmembers;

void InitSets( unsigned n )
{
    wperset = (n + WSIZE - 1)/WSIZE;
    setmembers = CALLOC( n, short );
}

a_word *AllocSet( unsigned n )
{
    return( CALLOC( n * wperset, a_word ) );
}

void Union( a_word *s, a_word *t )
{
    int n;

    for( n = wperset; n; --n ) {
        *s++ |= *t++;
    }
}

void Assign( a_word *s, a_word *t )
{
    int n;

    for( n = wperset; n; --n ) {
        *s++ = *t++;
    }
}

void Clear( a_word *s )
{
    int n;

    for( n = wperset; n; --n ) {
        *s++ = 0;
    }
}

bool Empty( a_word *s )
{
    int n;

    for( n = wperset; n; --n ) {
        if( *s++ ) {
            return( FALSE );
        }
    }
    return( TRUE );
}

bool EmptyIntersection( a_word *s, a_word *t )
{
    int n;

    for( n = wperset; n; --n ) {
        if( *s++ & *t++ ) {
            return( FALSE );
        }
    }
    return( TRUE );
}

bool Equal( a_word *s, a_word *t )
{
    int n;

    for( n = wperset; n; --n ) {
        if( *s++ != *t++ ) {
            return( FALSE );
        }
    }
    return( TRUE );
}

void Intersection( a_word *s, a_word *t )
{
    int n;

    for( n = wperset; n; --n ) {
        *s++ &= *t++;
    }
}

void AndNot( a_word *s, a_word *t )
{
    int n;

    for( n = wperset; n; --n ) {
        *s++ &= ~*t++;
    }
}

void UnionAnd( a_word *s, a_word *t, a_word *u )
{
    int n;

    for( n = wperset; n; --n ) {
        *s++ |= *t++ & *u++;
    }
}

short *Members( a_word *s )
{
    a_word *t;
    a_word word;
    int i, j;
    short *p;

    p = setmembers;
    i = 0;
    for( t = s + wperset; s < t; ++s ) {
        j = i;
        for( word = *s; word; word >>= 1 ) {
            if( word & 1 ) {
                *p++ = j;
            }
            ++j;
        }
        i += WSIZE;
    }
    return( p );
}

void DumpSet( a_word *s )
{
    char *p;
    char *t;
    int size;

    size = wperset * sizeof( a_word );
    p = (char *)s;
    t = &p[size];
    for( ; p < t; ++p ) {
        printf( "%02x", *p );
    }
    printf( "\n" );
}

static unsigned bit_count( unsigned x )
{
    unsigned y;

#if SHRT_MAX == INT_MAX
    y = x & 0x5555;
    x = y + (( x ^ y ) >> 1 );
    y = x & 0x3333;
    x = y + (( x ^ y ) >> 2 );
    y = x & 0x0f0f;
    x = y + (( x ^ y ) >> 4 );
    y = x & 0x00ff;
    x = y + (( x ^ y ) >> 8 );
#else
    y = x & 0x55555555;
    x = y + (( x ^ y ) >> 1 );
    y = x & 0x33333333;
    x = y + (( x ^ y ) >> 2 );
    y = x & 0x0f0f0f0f;
    x = y + (( x ^ y ) >> 4 );
    y = x & 0x00ff00ff;
    x = y + (( x ^ y ) >> 8 );
    y = x & 0x0000ffff;
    x = y + (( x ^ y ) >> 16 );
#endif
    return( x );
}

unsigned Cardinality( a_word *s )
/*******************************/
{
    int n;
    unsigned sum;

    sum = 0;
    for( n = wperset; n; --n ) {
        sum += bit_count( *s );
        ++s;
    }
    return( sum );
}
