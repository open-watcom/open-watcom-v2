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
* Description:  Heap management functions test.
*
****************************************************************************/


#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#ifdef __SW_BW
 #include <wdefwin.h>
#endif

#define MAX_ITER        5000

#define NUM_ALLOCS      211
#define NUM_FREES       51
#define COPRIME         7
#define BIGGER_PRIME    521

#define ROWS    16
#define COLS    64
#define UNIT    ( 0x10000 / ( ROWS * COLS ))

unsigned last_row, last_col;
unsigned failed_mallocs;

void *m[NUM_ALLOCS];

#ifdef VERBOSE

#ifndef _M_I86
#error Verbose mode only works with 16-bit libraries!
#endif

#include <i86.h>

char usage[ROWS][COLS];

static void dump_heap( unsigned long free_size,
                       unsigned long used_size,
                       unsigned largest_free_size )
{
    unsigned    i;
    unsigned    row;

    for( row = 0; row < ROWS; ++row ) {
        printf( "%4x:  ", row * ( COLS * UNIT ) );
        for( i = 0; i < COLS; ++i ) {
            if(( i % 8 ) == 0 ) {
                printf( "%1x", (((((COLS * UNIT)*(i/8))/8)>>8)&0x0f) );
            }
            putchar( usage[row][i] );
        }
        putchar( '\n' );
    }
    putchar( '\n' );
    printf( "Total size of used blocks:                   %lu bytes\n",
            used_size );
    printf( "Total size of fragmented free blocks:        %lu bytes\n",
            free_size );
    printf( "Largest free block:                          %u bytes\n",
            largest_free_size );
    printf( "Number of failed allocations:                %u\n",
            failed_mallocs );
    if( free_size > used_size ) {
        /* really bad performance! */
        free_size = used_size;
    }
    printf( "Percentage of perfect algorithm performance: %lu%%\n",
            100 - (( free_size * 100 ) / used_size ) );
    memset( usage, 'Û', ROWS * COLS );
}

static void dump_heaps( void )
{
    unsigned            row, col, siz;
    unsigned            prev_seg;
    int                 heap_status;
    unsigned long       free_size;
    unsigned long       used_size;
    unsigned            largest_free_size;
    size_t              old_size;
    struct _heapinfo    hinfo;

    used_size = 0;
    free_size = 0;
    old_size = 0;
    largest_free_size = 0;
    hinfo._pentry = NULL;
    heap_status = _heapwalk( &hinfo );
    if( heap_status == _HEAPOK ) {
        prev_seg = FP_SEG( hinfo._pentry );
    }
    while( heap_status == _HEAPOK ) {
        if( prev_seg != FP_SEG( hinfo._pentry )) {
            printf( "segment: %x\n", prev_seg );
            memset( &usage[last_row][last_col], ' ',
                    &usage[ROWS][0] - &usage[last_row][last_col] );
            last_row = 0;
            last_col = 0;
            dump_heap( free_size, used_size, largest_free_size );
            used_size = 0;
            free_size = 0;
            old_size = 0;
            largest_free_size = 0;
            prev_seg = FP_SEG( hinfo._pentry );
        }
        /* we don't want to count the last free block */
        free_size += old_size;
        if( largest_free_size < old_size ) {
            largest_free_size = old_size;
        }
        if( hinfo._useflag == _USEDENTRY ) {
            used_size += hinfo._size;
            old_size = 0;
        } else {
            row = (unsigned) hinfo._pentry;
            row /= ( COLS * UNIT );
            col = (unsigned) hinfo._pentry;
            col %= ( COLS * UNIT );
            col /= UNIT / 2;
            siz = hinfo._size / (UNIT / 2);
            if( col & 1 ) {
                if( siz == 0 ) {
                    col >>= 1;
                } else {
                    usage[row][col>>1] = 'Ý';
                    last_row = row;
                    last_col = col>>1;
                    --siz;
                    col >>= 1;
                    ++col;
                    if( col == COLS ) {
                        ++row;
                        col = 0;
                    }
                }
            } else {
                col >>= 1;
            }
            if( siz == 0 ) {
                usage[row][col] = '°';
                last_row = row;
                last_col = col;
            } else {
                for(; siz > 1; siz -= 2 ) {
                    usage[row][col] = ' ';
                    last_row = row;
                    last_col = col;
                    ++col;
                    if( col == COLS ) {
                        ++row;
                        col = 0;
                    }
                }
                if( siz == 1 ) {
                    usage[row][col] = 'Þ';
                    last_row = row;
                    last_col = col;
                }
            }
            old_size = hinfo._size;
        }
        heap_status = _heapwalk( &hinfo );
    }
    if( heap_status != _HEAPEND ) {
        printf( "FAIL: line %d heap_status=%d\n", __LINE__, heap_status );
        exit( EXIT_FAILURE );
    }
    memset( &usage[last_row][last_col], ' ',
            &usage[ROWS][0] - &usage[last_row][last_col] );
    last_row = 0;
    last_col = 0;
#if defined(__SMALL__) || defined(__MEDIUM__) || defined(__386__) || defined(__AXP__)
    printf( "data segment:\n" );
#else
    printf( "segment: %x\n", prev_seg );
#endif
    dump_heap( free_size, used_size, largest_free_size );
}
#endif

static void *try_malloc( size_t amount )
{
    void *p;

    p = malloc( amount );
    if( p == NULL && amount != 0 ) {
        ++failed_mallocs;
    }
    return( p );
}


int main( int argc, char **argv )
{
    int heap_status;
    unsigned iter;
    unsigned i;
#ifdef __SW_BW
    FILE *my_stdout;
    my_stdout = freopen( "tmp.log", "a", stdout );
    if( my_stdout == NULL ) {
        fprintf( stderr, "Unable to redirect stdout\n" );
        return( EXIT_FAILURE );
    }
#endif
#if !( defined(__SMALL__) || defined(__MEDIUM__) || defined(__386__) || defined(__AXP__) )
    {
        void *p1, *p2, *p3;

        /* test heap shrink for large data */
        p1 = malloc( 0xf000 );      /* tail */
        p2 = malloc( 0xf000 );      /* .... */
        p3 = malloc( 0xf000 );      /* head */

        /* case 1: free head of heap list */
        free( p3 );
        _heapshrink();
        heap_status = _heapchk();
        if( heap_status != _HEAPOK ) {
            printf( "FAIL: line %d\n", __LINE__ );
            return( EXIT_FAILURE );
        }
        p3 = malloc( 0xf000 );
        /* case 2: free tail of heap list */
        free( p1 );
        _heapshrink();
        heap_status = _heapchk();
        if( heap_status != _HEAPOK ) {
            printf( "FAIL: line %d\n", __LINE__ );
            return( EXIT_FAILURE );
        }
        p1 = p2;
        p2 = p3;
        p3 = malloc( 0xf000 );
        /* case 3: free middle of heap list */
        free( p2 );
        _heapshrink();
        heap_status = _heapchk();
        if( heap_status != _HEAPOK ) {
            printf( "FAIL: line %d\n", __LINE__ );
            return( EXIT_FAILURE );
        }
        /* case 4: free only item in list */
        free( p1 );
        _heapshrink();
        heap_status = _heapchk();
        if( heap_status != _HEAPOK ) {
            printf( "FAIL: line %d\n", __LINE__ );
            return( EXIT_FAILURE );
        }
        free( p3 );
        _heapshrink();
        heap_status = _heapchk();
        if( heap_status != _HEAPEMPTY && heap_status != _HEAPOK ) {
            printf( "FAIL: line %d\n", __LINE__ );
            return( EXIT_FAILURE );
        }
    }
#endif

#if !defined(__386__) && !defined(__AXP__)
    _nheapgrow();
#endif
#if !defined(__386__) && !defined(__AXP__) && !defined(__WINDOWS__)
    if( sbrk( 0 ) < (void near *) ~0x0f ) {
        printf( "FAIL: line %d\n", __LINE__ );
        return( EXIT_FAILURE );
    }
#endif
    if( _nheapchk() != _HEAPOK ) {
        printf( "FAIL: line %d\n", __LINE__ );
        return( EXIT_FAILURE );
    }
    if( argc == 2 ) {
        _amblksiz = atoi( argv[1] );
    }
#ifdef VERBOSE
    memset( usage, 'Û', ROWS * COLS );
#endif
    srand( 0x8207 );
    for( i = 0; i < NUM_ALLOCS; ++i ) {
        m[i] = try_malloc( 1 + rand() % NUM_ALLOCS );
    }
    heap_status = _heapchk();
    if( heap_status != _HEAPOK ) {
        printf( "FAIL: line %d\n", __LINE__ );
        return( EXIT_FAILURE );
    }
    for( iter = 0; iter < NUM_FREES; ++iter ) {
        i = rand() % NUM_ALLOCS;
        free( m[i] );
        m[i] = NULL;
    }
    heap_status = _heapchk();
    if( heap_status != _HEAPOK ) {
        printf( "FAIL: line %d\n", __LINE__ );
        return( EXIT_FAILURE );
    }
    for( iter = 0; iter < MAX_ITER; ++iter ) {
        _heapshrink();
        failed_mallocs = 0;
        heap_status = _heapchk();
        if( heap_status != _HEAPOK ) {
            printf( "FAIL: line %d iter=%d\n", __LINE__, iter );
            return( EXIT_FAILURE );
        }
        i = rand() % NUM_ALLOCS;
        do {
            free( m[i] );
            m[i] = try_malloc( rand() % BIGGER_PRIME );
            i += COPRIME;
            i %= NUM_ALLOCS;
        } while( i != 0 );
        heap_status = _heapchk();
        if( heap_status != _HEAPOK ) {
            printf( "FAIL: line %d iter=%d\n", __LINE__, iter );
            return( EXIT_FAILURE );
        }
    }
    heap_status = _heapchk();
    if( heap_status != _HEAPOK ) {
        printf( "FAIL: line %d heap_status=%d\n", __LINE__, heap_status );
        return( EXIT_FAILURE );
    }
    heap_status = _heapset( 1 );
    if( heap_status != _HEAPOK ) {
        printf( "FAIL: line %d heap_status=%d\n", __LINE__, heap_status );
        return( EXIT_FAILURE );
    }
#ifdef VERBOSE
    dump_heaps();
#endif
    printf( "Tests completed (%s).\n", strlwr( argv[0] ) );
#ifdef __SW_BW
    fprintf( stderr, "Tests completed (%s).\n", strlwr( argv[0] ) );
    fclose( my_stdout );
    _dwShutDown();
#endif
    return( EXIT_SUCCESS );
}
