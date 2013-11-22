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

set_size *setmembers;

static set_size wperset;

void InitSets( unsigned n )
{
    wperset = (n + WSIZE - 1)/WSIZE;
    setmembers = CALLOC( n, set_size );
}

a_word *AllocSet( unsigned n )
{
    return( CALLOC( n * wperset, a_word ) );
}

unsigned GetSetSize( unsigned n )
{
    return( n * wperset );
}

void Union( a_word *s, a_word *t )
{
    set_size    n;

    for( n = wperset; n > 0; --n ) {
        *s++ |= *t++;
    }
}

void Assign( a_word *s, a_word *t )
{
    set_size    n;

    for( n = wperset; n > 0; --n ) {
        *s++ = *t++;
    }
}

void Clear( a_word *s )
{
    set_size    n;

    for( n = wperset; n > 0; --n ) {
        *s++ = 0;
    }
}

bool Empty( a_word *s )
{
    set_size    n;

    for( n = wperset; n > 0; --n ) {
        if( *s++ ) {
            return( FALSE );
        }
    }
    return( TRUE );
}

bool EmptyIntersection( a_word *s, a_word *t )
{
    set_size    n;

    for( n = wperset; n > 0; --n ) {
        if( *s++ & *t++ ) {
            return( FALSE );
        }
    }
    return( TRUE );
}

bool Equal( a_word *s, a_word *t )
{
    set_size    n;

    for( n = wperset; n > 0; --n ) {
        if( *s++ != *t++ ) {
            return( FALSE );
        }
    }
    return( TRUE );
}

void Intersection( a_word *s, a_word *t )
{
    set_size    n;

    for( n = wperset; n > 0; --n ) {
        *s++ &= *t++;
    }
}

void AndNot( a_word *s, a_word *t )
{
    set_size    n;

    for( n = wperset; n > 0; --n ) {
        *s++ &= ~*t++;
    }
}

void UnionAnd( a_word *s, a_word *t, a_word *u )
{
    set_size    n;

    for( n = wperset; n > 0; --n ) {
        *s++ |= *t++ & *u++;
    }
}

set_size *Members( a_word *s )
{
    a_word word;
    set_size i, j;
    set_size *p;

    p = setmembers;
    for( i = 0; i < wperset; ++i ) {
        j = i * WSIZE;
        for( word = *s++; word != 0; word >>= 1 ) {
            if( word & 1 ) {
                *p++ = j;
            }
            ++j;
        }
    }
    return( p );
}

void DumpSet( a_word *s )
{
    char        *p;
    set_size    size;

    p = (char *)s;
    for( size = wperset * WSIZE; size > 0; --size ) {
        printf( "%02x", *p++ );
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
    set_size    n;
    unsigned    sum;

    sum = 0;
    for( n = wperset; n > 0; --n ) {
        sum += bit_count( *s );
        ++s;
    }
    return( sum );
}
