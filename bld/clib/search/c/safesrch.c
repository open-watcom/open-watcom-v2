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
* Description:  Non-exhaustive test of Safer C library search functions.
*
****************************************************************************/


#define __STDC_WANT_LIB_EXT1__ 1       // Enable Safer C interfaces

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <search.h>
#ifdef __SW_BW
    #include <wdefwin.h>
#endif


/* Test macros */

#define VERIFY( expr ) ++tests; if( !(expr) ) {                         \
                      printf( "***FAILURE*** Condition failed.\n" );    \
                      printf( "   %s, line %u.\n", #expr, __LINE__ );   \
                          ++failures;                                   \
                      exit( -1 );                                       \
                }

#define EXPECT( expr ) ++tests; if( !(expr) ) {                         \
                      printf( "***WARNING*** Condition failed.\n" );    \
                      printf( "   %s, line %u.\n", #expr, __LINE__ );   \
                      ++warnings;                                       \
                      exit( -1 );                                       \
                }

#define VERIFYS( exp )   if( !(exp) ) {                                     \
                            printf( "%s: ***FAILURE*** at line %d of %s.\n",\
                                    ProgramName, __LINE__,                  \
                                    strlwr(__FILE__) );                     \
                            NumErrors++;                                    \
                            exit( -1 );                                     \
                        }


#define QSORT_SIZE  300
#define LIST_SIZE   200

unsigned int    warnings, failures, tests;
char            ProgramName[128];       /* executable filename */
int             NumErrors = 0;          /* number of errors */
int             NumViolations = 0;      /* runtime-constraint violation counter */


void Status_Print( void )
/***********************/
{
    /* Print the number of warnings/failures. */

    printf( "   Tests: %d\n", tests );
    printf( "Warnings: %d\n", warnings );
    printf( "Failures: %d\n", failures );
}


/*** Safer C Tests ***/

/*** Comparison routines ***/

int floatcmp_s( void const *_a, void const *_b, void *context )
{
    float   *a = (float *)_a;
    float   *b = (float *)_b;

    if( *a == *b ) {
        return 0;
    } else if( *a < *b ) {
        return -1;
    } else {
        return 1;
    }
}

/* compare routine with context parameter      */
int longintcmp_s( void const *_a, void const *_b, void *context )
{
    unsigned long   *a = (unsigned long *)_a;
    unsigned long   *b = (unsigned long *)_b;

    if( (*a) == (*b) ) {
        return 0;
    } else if( (*a) < (*b) ) {
        return -1;
    } else {
        return 1;
    }
}

/* Runtime-constraint handler for tests; doesn't abort program. */
void my_constraint_handler( const char *msg, void *ptr, errno_t error )
{
#ifdef DEBUG_MSG
    fprintf( stderr, "Runtime-constraint in %s", msg );
#endif
    ++NumViolations;
}

/***************************************************************************/
/*  Test bsearch_s()                                                       */
/***************************************************************************/
void test_bsearch_s( void )
{
    int             violations = NumViolations;
    int             i;
    unsigned long   list[ LIST_SIZE ], cur;
    char            *context = "foo";


    /* prevent overflows in test data */
#if !( LIST_SIZE < 4000 )
    #error LIST_SIZE too large
#endif

    /* Create the search list */
    for( i = 0; i < LIST_SIZE; i++ ) {
        list[i] = i*i;
    }

    /* Test bsearch_s */
    for( i = 0; i < LIST_SIZE; i++ ) {
        cur = list[i];
        VERIFYS( list[i] == i*i );
        VERIFYS( bsearch_s( &cur, list, LIST_SIZE, sizeof( unsigned long ), longintcmp_s, context ) == &list[i] );
    }

    /* Test runtime-constraints */
    VERIFYS( bsearch_s( NULL, list, LIST_SIZE, sizeof( int ), longintcmp_s, context ) == NULL );
    VERIFYS( NumViolations == ++violations );

    VERIFYS( bsearch_s( &cur, NULL, LIST_SIZE, sizeof( int ), longintcmp_s, context ) == NULL );
    VERIFYS( NumViolations == ++violations );

    VERIFYS( bsearch_s( &cur, list, LIST_SIZE, sizeof( int ), NULL, context ) == NULL );
    VERIFYS( NumViolations == ++violations );

    VERIFYS( bsearch_s( &cur, NULL, LIST_SIZE, sizeof( int ), longintcmp_s, context ) == NULL );
    VERIFYS( NumViolations == ++violations );

    VERIFYS( bsearch_s( &cur, list, 0, sizeof( int ), longintcmp_s, context ) == NULL );
    VERIFYS( NumViolations == violations );

#if RSIZE_MAX != SIZE_MAX
    VERIFYS( bsearch_s( &cur, list, ~0, sizeof( int ), longintcmp_s, context ) == NULL );
    VERIFYS( NumViolations == ++violations );

    VERIFYS( bsearch_s( &cur, list, LIST_SIZE, ~0, longintcmp_s, context ) == NULL );
    VERIFYS( NumViolations == ++violations );
#endif
}


/***************************************************************************/
/*  Test qsort_s()                                                         */
/***************************************************************************/
void test_qsort_s( void )
{
    int     violations = NumViolations;
    float   test[ QSORT_SIZE ];
    int     i;
    char    *context = "Bar";

    srand( 0 );
    for( i = 0; i < QSORT_SIZE; i++ ) {   /* Create test data */
        test[i] = rand( );
    }

    qsort_s( test, QSORT_SIZE, sizeof( float ), floatcmp_s, context ); /* Sort the data */

#if !( QSORT_SIZE - 1 > 0 )
    #error  QSORT_SIZE too small
#endif
    for( i = 0; i < QSORT_SIZE - 1; i++ ) {   /* Check the order */
        VERIFYS( test[i] <= test[i+1] );
    }

    /* Test runtime-constraints */
    VERIFYS( qsort_s( NULL, LIST_SIZE, sizeof( int ), floatcmp_s, context ) != 0 );
    VERIFYS( NumViolations == ++violations );

    VERIFYS( qsort_s( test, LIST_SIZE, sizeof( int ), NULL, context ) != 0 );
    VERIFYS( NumViolations == ++violations );

    VERIFYS( qsort_s( NULL, LIST_SIZE, sizeof( int ), floatcmp_s, context ) != 0 );
    VERIFYS( NumViolations == ++violations );

    VERIFYS( qsort_s( test, 0, sizeof( int ), floatcmp_s, context ) == 0 );
    VERIFYS( NumViolations == violations );

#if RSIZE_MAX != SIZE_MAX
    VERIFYS( qsort_s( test, ~0, sizeof( int ), floatcmp_s, context ) != 0 );
    VERIFYS( NumViolations == ++violations );

    VERIFYS( qsort_s( test, LIST_SIZE, ~0, floatcmp_s, context ) != 0 );
    VERIFYS( NumViolations == ++violations );
#endif
}


int main( int argc, char *argv[] )
/********************************/
{
#ifdef __SW_BW
    FILE    *my_stdout;

    my_stdout = freopen( "tmp.log", "a", stdout );
    if( my_stdout == NULL ) {
        fprintf( stderr, "Unable to redirect stdout\n" );
        exit( -1 );
    }
#endif

    /*** Initialize ***/
    strcpy( ProgramName, strlwr( argv[0] ) );   /* store filename */

    /****  Start of Tests  ****/

    /***********************************************************************/
    /*  set constraint-handler                                             */
    /***********************************************************************/

    set_constraint_handler_s( my_constraint_handler );

    test_bsearch_s();
    test_qsort_s();

    /****  End of Tests  ****/

    //Status_Print( );

    printf( "Tests completed (%s).\n", ProgramName );
#ifdef __SW_BW
    fprintf( stderr, "Tests completed (%s).\n", ProgramName );
    fclose( my_stdout );
    _dwShutDown();
#endif
    return( 0 );
}
