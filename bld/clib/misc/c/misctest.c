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
* Description:  Non-exhaustive test of the C library miscellaneous functions.
*
****************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <libgen.h>
#ifdef __SW_BW
  #include <wdefwin.h>
#endif


#define VERIFY( exp )                                   \
    if( !(exp) ) {                                      \
        printf( "%s: ***FAILURE*** at line %d of %s.\n",\
                ProgramName, __LINE__,                  \
                strlwr(__FILE__) );                     \
        NumErrors++;                                    \
        exit( EXIT_FAILURE );                           \
    }

void TestCompare( void );
void TestCompareF( void );
void TestCopy( void );
void TestCopyF( void );
void TestOverlap( void );
void TestOverlapF( void );
void TestMisc( void );


char    ProgramName[128];                       /* executable filename */
int     NumErrors = 0;                          /* number of errors */

#ifdef __SW_BW
FILE    *my_stdout;

/*
 * Local run-time error message output implementation. Needed
 * to avoid popping up a dialog box when an assertion is hit. Also
 * sends the message to stdout instead of the usual stderr.
 */
void __rterr_msg( const char *hdr, const char *msg )
{
    fputs( hdr, stdout );
    fputs( ": ", stdout );
    fputs( msg, stdout );
}
#endif

/****
***** Test _rotl(), _rotr(), _lrotl() and _lrotr()
****/

void TestRot( void )
{
    unsigned long       lmask = 0x12345678;
#if defined( _M_I86 )
    unsigned int        imask = 0x7654;
#else
    unsigned int        imask = 0x76543210;
#endif

    /*** Test and verify various combinations ***/
    lmask = _lrotl( lmask, 4 );
    VERIFY( lmask == 0x23456781 );
    lmask = _lrotr( lmask, 8 );
    VERIFY( lmask == 0x81234567 );
    lmask = _lrotl( lmask, 4 );
    VERIFY( lmask == 0x12345678 );

#if defined( _M_I86 )
    imask = _rotl( imask, 4 );
    VERIFY( imask == 0x6547 );
    imask = _rotr( imask, 8 );
    VERIFY( imask == 0x4765 );
    imask = _rotl( imask, 4 );
    VERIFY( imask == 0x7654 );
#else
    imask = _rotl( imask, 4 );
    VERIFY( imask == 0x65432107 );
    imask = _rotr( imask, 8 );
    VERIFY( imask == 0x07654321 );
    imask = _rotl( imask, 4 );
    VERIFY( imask == 0x76543210 );
#endif
}

/****
***** Test libgen.h functions
****/

void TestLibgen( void )
{
    /* NB: We deliberately pass string literals that may be read only.
     * Our libgen.h functions are supposed to handle that.
     */
    VERIFY( !strcmp( basename( "/usr/lib" ), "lib" ) );
    VERIFY( !strcmp( basename( "/usr/" ), "usr" ) );
    VERIFY( !strcmp( basename( "/" ), "/" ) );
    VERIFY( !strcmp( basename( "///" ), "/" ) );
    VERIFY( !strcmp( basename( "//usr//lib//" ), "lib" ) );
    VERIFY( !strcmp( basename( "" ), "." ) );
    VERIFY( !strcmp( basename( NULL ), "." ) );

    VERIFY( !strcmp( dirname( "/usr/lib" ), "/usr" ) );
    VERIFY( !strcmp( dirname( "/usr/" ), "/" ) );
    VERIFY( !strcmp( dirname( "usr" ), "." ) );
    VERIFY( !strcmp( dirname( "/" ), "/" ) );
    VERIFY( !strcmp( dirname( "." ), "." ) );
    VERIFY( !strcmp( dirname( ".." ), "." ) );
    VERIFY( !strcmp( dirname( "" ), "." ) );
    VERIFY( !strcmp( dirname( NULL ), "." ) );
}

/****
***** Test assert macro.
****/

/* NB: Since assert.h includes declarations, it's not possible to
 * include it in the middle of a function (not pre-C99)
 */
#undef NDEBUG
#include <assert.h>
void TestAssert1( int i )
{
    assert( i == 1 );                           /* must pass */
}

#undef  NDEBUG
#define NDEBUG
#include <assert.h>
void TestAssert2( int i )
{
    assert( i == 0 );                           /* must not do nothing */
}

#undef NDEBUG
#include <assert.h>
void TestAssert3( int i )
{
    assert( i == 1 );                           /* must pass again */
}

void TestAssert( int i )
{
    assert( i == 0 );                           /* must fail */
}

/* This signal handler should be called at the end of this program */
void abort_handler( int sig )
{
    printf( "Note: \"Assertion failed\" message should precede this text.\n" );

    /*** Print a pass/fail message and quit ***/
    VERIFY( NumErrors == 0 );
    printf( "Tests completed (%s).\n", ProgramName );
#ifdef __SW_BW
    fprintf( stderr, "Tests completed (%s).\n", ProgramName );
    fclose( my_stdout );
    _dwShutDown();
#endif
    exit( EXIT_SUCCESS );
}

int main( int argc, char *argv[] )
{
#ifdef __SW_BW
    my_stdout = freopen( "tmp.log", "a", stdout );
    if( my_stdout == NULL ) {
        fprintf( stderr, "Unable to redirect stdout\n" );
        exit( EXIT_FAILURE );
    }
#endif

    /*** Initialize ***/
    strcpy( ProgramName, strlwr( argv[0] ) );   /* store filename */

    /*** Test various functions ***/
    TestRot();
    TestLibgen();

    /*** Test assert macro ***/
    TestAssert1( 1 );
    TestAssert2( 1 );
    TestAssert3( 1 );

#ifdef __RDOS__ /* RDOS does not support signals! */
    printf( "Tests completed (%s).\n", ProgramName );
    return( 0 );
#else    
    signal( SIGABRT, abort_handler );           /* will be called via abort() */
    TestAssert( 1 );

    VERIFY( 0 );                                /* should never get here! */
    return( EXIT_FAILURE );
#endif    
}
