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
* Description:  Non-exhaustive test of C library search functions.
*
****************************************************************************/


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <search.h>

#ifdef __SW_BW
    #include <wdefwin.h>
#endif

/* Test macros */

#define VERIFY( expr ) ++tests; if( !(expr) ) {             \
                      printf( "***FAILURE*** Condition failed.\n" );  \
                      printf( "   %s, line %u.\n", #expr, __LINE__ ); \
                          ++failures;                                   \
                      exit( -1 );                                     \
                }

#define EXPECT( expr ) ++tests; if( !(expr) ) {             \
                      printf( "***WARNING*** Condition failed.\n" );  \
                      printf( "   %s, line %u.\n", #expr, __LINE__ ); \
                      ++warnings;                                   \
                      exit( -1 );                                     \
                }

#define QSORT_SIZE 300
#define LIST_SIZE 200

unsigned int warnings, failures, tests;

void Status_Print( )
/*******************/
{
    /* Print the number of warnings/failures. */

    printf( "   Tests: %d\n", tests );
    printf( "Warnings: %d\n", warnings );
    printf( "Failures: %d\n", failures );
}

/*** Comparison routines ***/

int floatcmp( void const *_a, void const *_b )
{
    float *a = (float *)_a;
    float *b = (float *)_b;

    if( *a == *b ) {
        return 0;
    } else if( *a < *b ) {
        return -1;
    } else {
        return 1;
    }
}

int longintcmp( void const *_a, void const *_b )
{
    unsigned long int *a = (unsigned long int *)_a;
    unsigned long int *b = (unsigned long int *)_b;

    if( (*a) == (*b) ) {
        return 0;
    } else if( (*a) < (*b) ) {
        return -1;
    } else {
        return 1;
    }
}

/*** Test Routines ***/


int test_qsort_fl_arr( )
/**********************/
{
    float  test[ QSORT_SIZE ];
    int    i;

    srand( 0 );
    for( i = 0; i < QSORT_SIZE; i++ ) {   /* Create test data */
        test[i] = rand( );
    }

    qsort( test, QSORT_SIZE, sizeof( float ), floatcmp ); /* Sort the data */

    #if !( QSORT_SIZE - 1 > 0 )
        #error  QSORT_SIZE too small
    #endif
    for( i = 0; i < QSORT_SIZE - 1; i++ ) {   /* Check the order */
        VERIFY( test[i] <= test[i+1] );
    }

    /* Verify the sort */
    return 1;
}

int test_qsort( )
/***************/
{
    test_qsort_fl_arr( );

    return 1;
}

int test_bsearch( )
/*****************/
{
    int i;
    unsigned long int list[ LIST_SIZE ], cur;

    /* prevent overflows in test data */
    #if !( LIST_SIZE < 4000 )
        #error LIST_SIZE too large
    #endif

    /* Create the search list */
    for( i = 0; i < LIST_SIZE; i++ ) {
        list[i] = i*i;
    }

    /* Test bsearch */
    for( i = 0; i < LIST_SIZE; i++ ) {
        cur = list[i];
        VERIFY( list[i] == i*i );
        VERIFY( bsearch( &cur, list, LIST_SIZE,
            sizeof( unsigned long int), longintcmp ) == &list[i] );
    }

    return 1;
}

int test_lfind_lsearch( )
/***********************/
{
    int i;
    unsigned num;
    unsigned long int list[ LIST_SIZE + 1 ], cur;

    srand( 0 );

    /* Create the search list */
    for( i = 0; i < LIST_SIZE; i++ ) {
        list[i] = abs( (rand() * rand()) % 1000000000 );
    }

    num = LIST_SIZE;

    for( i = 0; i < LIST_SIZE; i++ ) {
        cur = list[i];
        /* test bsearch */

        VERIFY( (unsigned long int*)(lfind( &cur, list, &num,
            sizeof( unsigned long int ), longintcmp )) == (&list[i]) );

        /* test lsearch */

        VERIFY( (unsigned long int *)lsearch( &cur, list, &num,
            sizeof( unsigned long int ), longintcmp ) == (void *)(&list[i]) );
    }

    cur = 2000000001;

    /* test of bsearch */
    EXPECT( lfind( &cur, list, &num,
        sizeof( unsigned long int ), longintcmp ) == NULL );

        /* test of lsearch */
    EXPECT( lsearch( &cur, list, &num,
        sizeof( unsigned long int ), longintcmp ) == &list[ LIST_SIZE ] );

    EXPECT( num == (LIST_SIZE + 1) );
    return 1;
}

int main( int argc, char *argv[] )
/********************************/
{
    #ifdef __SW_BW
        FILE *my_stdout;
        my_stdout = freopen( "tmp.log", "a", stdout );
        if( my_stdout == NULL ) {
            fprintf( stderr, "Unable to redirect stdout\n" );
            exit( -1 );
        }
    #endif

    /****  Start of Tests  ****/

    test_bsearch( );
    test_lfind_lsearch( );
    test_qsort( );

    /****  End of Tests  ****/

    //Status_Print( );

    printf( "Tests completed (%s).\n", strlwr( argv[0] ) );
    #ifdef __SW_BW
    {
        fprintf( stderr, "Tests completed (%s).\n", strlwr( argv[0] ) );
        fclose( my_stdout );
        _dwShutDown();
    }
    #endif
    return( 0 );
}
