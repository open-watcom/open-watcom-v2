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
* Description:  Non-exhaustive test of the Safer C library
*               multibyte character functions.
*
****************************************************************************/


#define __STDC_WANT_LIB_EXT1__ 1       // Enable Safer C interfaces

#include <locale.h>
#include <stdarg.h>
#include <mbstring.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <wchar.h>
#ifdef __SW_BW
    #include <wdefwin.h>
#endif
#include "rterrno.h"
#include "thread.h"


/* DUMMY_DBCS won't be needed now that Windows and DOS are fully supported */
//#if !defined(__NT__) && !defined(__OS2__) && !defined(__WINDOWS__)
//    #define DUMMY_DBCS
//#endif


#define VERIFY( exp )   if( !(exp) ) {                                      \
                            printf( "%s: ***FAILURE*** at line %d of %s.\n",\
                                    ProgramName, __LINE__,                  \
                                    _mbslwr(__FILE__) );                    \
                            NumErrors++;                                    \
                        }

char ProgramName[128];                          /* executable filename */
int NumErrors = 0;                              /* number of errors */

int     NumViolations = 0;  /* runtime-constraint violation counter */


/* Runtime-constraint handler for tests; doesn't abort program. */
void my_constraint_handler( const char *msg, void *ptr, errno_t error )
{
#ifdef DEBUG_FMT
    fprintf( stderr, "Runtime-constraint in %s", msg );
#endif
    ++NumViolations;
}



/****
***** Test mbstowcs_s() and mbsrtowcs_s().
****/

void TestToWide( void )
{
    wchar_t             wcs[20];
    errno_t             rc;
    size_t              retval;
    size_t              retval2;
    mbstate_t           ps;
    const char         *pchar;
    const char         *pchar2;
    int                 violations = NumViolations;



    rc = mbstowcs_s( &retval, wcs, 20, "", 0 );
    VERIFY( rc == 0 );
    VERIFY( wcs[0] == L'\0' );
    VERIFY( violations == NumViolations );

    rc = mbstowcs_s( &retval, wcs, 20, "foo", 3 );
    VERIFY( rc == 0 );
    VERIFY( retval == 3 );
    VERIFY( violations == NumViolations );

    rc = mbstowcs_s( &retval, wcs, 20, "\x81\xFC""foo", 0 );
    VERIFY( rc == 0 );
    VERIFY( retval == 0 );
    VERIFY( violations == NumViolations );

    rc = mbstowcs_s( &retval, wcs, 20, "foo", 10 );
    VERIFY( rc == 0 );
    VERIFY( retval == 3 );
    VERIFY( violations == NumViolations );

    rc = mbstowcs_s( &retval, wcs, 6, "\x81\xFC""foo", 10 );
    VERIFY( rc == 0 );
    VERIFY( retval == 4 );
    VERIFY( wcs[1] == 'f' );
    VERIFY( wcs[2] == 'o' );
    VERIFY( wcs[3] == 'o' );
    VERIFY( wcs[4] == '\0' );
    VERIFY( violations == NumViolations );
    rc = mbstowcs_s( &retval2, NULL, 0, "\x81\xFC""foo", 10 );
    VERIFY( retval == retval2 );
    VERIFY( violations == NumViolations );

    pchar = "";
    rc = mbsrtowcs_s( &retval, wcs, 20, &pchar, 0, &ps );
    VERIFY( rc == 0 );
    VERIFY( wcs[0] == L'\0' );
    VERIFY( violations == NumViolations );

    pchar = "foo";
    rc = mbsrtowcs_s( &retval, wcs, 20, &pchar, 3, &ps );
    VERIFY( rc == 0 );
    VERIFY( retval == 3 );
    VERIFY( violations == NumViolations );

    pchar = "\x81\xFC""foo";
    rc = mbsrtowcs_s( &retval, wcs, 20, &pchar, 0, &ps );
    VERIFY( rc == 0 );
    VERIFY( retval == 0 );
    VERIFY( violations == NumViolations );

    pchar = "foo";
    rc = mbsrtowcs_s( &retval, wcs, 20, &pchar, 10, &ps );
    VERIFY( rc == 0 );
    VERIFY( retval == 3 );
    VERIFY( violations == NumViolations );

    pchar = "\x81\xFC""foo";
    rc = mbsrtowcs_s( &retval, wcs, 6, &pchar, 10, &ps );
    VERIFY( rc == 0 );
    VERIFY( retval == 4 );
    VERIFY( wcs[1] == 'f' );
    VERIFY( wcs[2] == 'o' );
    VERIFY( wcs[3] == 'o' );
    VERIFY( wcs[4] == '\0' );
    VERIFY( violations == NumViolations );
    pchar2 = "\x81\xFC""foo";
    rc = mbsrtowcs_s( &retval2, NULL, 0, &pchar2, 10, &ps );
    VERIFY( retval == retval2 );
    VERIFY( violations == NumViolations );

    /***********************************************************************/
    /*  now test runtime-constraints                                       */
    /***********************************************************************/
    rc = mbstowcs_s( NULL, wcs, 20, "\x81\xFC""foo", 10 );
    VERIFY( rc != 0 );
    VERIFY( wcs[0] == L'\0' );
    VERIFY( ++violations == NumViolations );

    rc = mbstowcs_s( &retval, wcs, 6, NULL, 10 );
    VERIFY( rc != 0 );
    VERIFY( retval == -1 );
    VERIFY( wcs[0] == L'\0' );
    VERIFY( ++violations == NumViolations );

#if RSIZE_MAX != SIZE_MAX
    wcs[0] = L'X';
    rc = mbstowcs_s( &retval, wcs, ~0, "\x81\xFC""foo", 10 );
    VERIFY( rc != 0 );
    VERIFY( retval == -1 );
    VERIFY( wcs[0] == L'X' );
    VERIFY( ++violations == NumViolations );

    rc = mbstowcs_s( &retval, wcs, 20, "\x81\xFC""foo", ~0 );
    VERIFY( rc != 0 );
    VERIFY( retval == -1 );
    VERIFY( wcs[0] == L'\0' );
    VERIFY( ++violations == NumViolations );
#endif

    wcs[0] = L'X';
    rc = mbstowcs_s( &retval, wcs, 0, "\x81\xFC""foo", 10 );
    VERIFY( rc != 0 );
    VERIFY( retval == -1 );
    VERIFY( wcs[0] == L'X' );
    VERIFY( ++violations == NumViolations );

    rc = mbstowcs_s( &retval, wcs, 2, "\x81\xFC""foo", 10 );
    VERIFY( rc != 0 );
    VERIFY( retval == -1 );
    VERIFY( wcs[0] == L'\0' );
    VERIFY( ++violations == NumViolations );

#if 0
    /* don't know how to force an encoding error */
    rc = mbstowcs_s( &retval, wcs, 20, "\x01\x11""foo", 10 );
    VERIFY( rc != 0 );
    VERIFY( retval == -1 );
    VERIFY( wcs[0] == L'\0' );
    VERIFY( ++violations == NumViolations );
#endif

    pchar = "\x81\xFC""foo";
    rc = mbsrtowcs_s( NULL, wcs, 20, &pchar, 10, &ps );
    VERIFY( rc != 0 );
    VERIFY( wcs[0] == L'\0' );
    VERIFY( ++violations == NumViolations );

    rc = mbsrtowcs_s( &retval, wcs, 6, NULL, 10, &ps );
    VERIFY( rc != 0 );
    VERIFY( retval == -1 );
    VERIFY( wcs[0] == L'\0' );
    VERIFY( ++violations == NumViolations );

    pchar = NULL;
    rc = mbsrtowcs_s( &retval, wcs, 6, &pchar, 10, &ps );
    VERIFY( rc != 0 );
    VERIFY( retval == -1 );
    VERIFY( wcs[0] == L'\0' );
    VERIFY( ++violations == NumViolations );

#if RSIZE_MAX != SIZE_MAX
    wcs[0] = L'X';
    pchar = "\x81\xFC""foo";
    rc = mbsrtowcs_s( &retval, wcs, ~0, &pchar, 10, &ps );
    VERIFY( rc != 0 );
    VERIFY( retval == -1 );
    VERIFY( wcs[0] == L'X' );
    VERIFY( ++violations == NumViolations );

    pchar = "\x81\xFC""foo";
    rc = mbsrtowcs_s( &retval, wcs, 20, &pchar, ~0, &ps );
    VERIFY( rc != 0 );
    VERIFY( retval == -1 );
    VERIFY( wcs[0] == L'\0' );
    VERIFY( ++violations == NumViolations );
#endif

    pchar = "\x81\xFC""foo";
    wcs[0] = L'X';
    rc = mbsrtowcs_s( &retval, wcs, 0, &pchar, 10, &ps );
    VERIFY( rc != 0 );
    VERIFY( retval == -1 );
    VERIFY( wcs[0] == L'X' );
    VERIFY( ++violations == NumViolations );

    pchar = "\x81\xFC""foo";
    rc = mbsrtowcs_s( &retval, wcs, 2, &pchar, 10, &ps );
    VERIFY( rc != 0 );
    VERIFY( retval == -1 );
    VERIFY( wcs[0] == L'\0' );
    VERIFY( ++violations == NumViolations );
#if 0
    /* don't know how to force an encoding error */
    pchar = "\x01\x11""foo";
    rc = mbsrtowcs_s( &retval, wcs, 20, &pchar, 10, &ps );
    VERIFY( rc != 0 );
    VERIFY( retval == -1 );
    VERIFY( wcs[0] == L'\0' );
    VERIFY( ++violations == NumViolations );
#endif
}



/****
***** Test wctomb_s() and wcstombs_s().
****/

void TestFromWide( void )
{
    char                mbs[20];
    wchar_t             wc = L'\0';
    wchar_t             wcs[] = { 'H', 'e', 'l', 'l', 'o', '\0' };

    errno_t             rc;
    size_t              retval;
    size_t              retval2;
    int                 status;
    int                 violations = NumViolations;

    rc = wctomb_s( &status, mbs, 20, wc );
    VERIFY( rc == 0 );
    VERIFY( mbs[0] == '\0' );
    VERIFY( status == 1 );
    VERIFY( violations == NumViolations );

    rc = wctomb_s( &status, mbs, 20, L'X' );
    VERIFY( rc == 0 );
    VERIFY( mbs[0] == 'X' );
    VERIFY( status == 1 );
    VERIFY( violations == NumViolations );

    rc = wctomb_s( &status, NULL, 0, L'X' );
    VERIFY( rc == 0 );
    VERIFY( status == 0 );  /* no state-dependant encodings */
    VERIFY( violations == NumViolations );


    rc = wcstombs_s( &retval, mbs, 20, wcs, 10 );
    VERIFY( rc == 0 );
    VERIFY( retval == 5 );
    VERIFY( !_mbscmp(mbs,"Hello") );
    VERIFY( violations == NumViolations );

    rc = wcstombs_s( &retval2, NULL, 0, wcs, 10 );
    VERIFY( rc == 0 );
    VERIFY( retval2 == 5 );
    VERIFY( violations == NumViolations );


    /***********************************************************************/
    /*  test runtime-constraints                                           */
    /***********************************************************************/
    rc = wctomb_s( &status, NULL, 20, wc );
    VERIFY( rc != 0 );
    VERIFY( ++violations == NumViolations );

    rc = wctomb_s( &status, mbs, 0, wc );
    VERIFY( rc != 0 );
    VERIFY( ++violations == NumViolations );

#if RSIZE_MAX != SIZE_MAX
    rc = wctomb_s( &status, mbs, ~0, wc );
    VERIFY( rc != 0 );
    VERIFY( ++violations == NumViolations );
#endif

    rc = wctomb_s( &status, mbs, 1, '\x81\xFC' );
    VERIFY( rc != 0 );
    VERIFY( ++violations == NumViolations );


    mbs[0]= 'X';
    rc = wcstombs_s( NULL, mbs, 20, wcs, 10 );
    VERIFY( rc != 0 );
    VERIFY( mbs[0] == '\0' );
    VERIFY( ++violations == NumViolations );

    mbs[0]= 'X';
    rc = wcstombs_s( &retval, mbs, 20, NULL, 10 );
    VERIFY( rc != 0 );
    VERIFY( retval == -1 );
    VERIFY( mbs[0] == '\0' );
    VERIFY( ++violations == NumViolations );

#if RSIZE_MAX != SIZE_MAX
    mbs[0]= 'X';
    rc = wcstombs_s( &retval, mbs, ~0, wcs, 10 );
    VERIFY( rc != 0 );
    VERIFY( retval == -1 );
    VERIFY( mbs[0] == 'X' );
    VERIFY( ++violations == NumViolations );

    mbs[0]= 'X';
    rc = wcstombs_s( &retval, mbs, 20, wcs, ~0 );
    VERIFY( rc != 0 );
    VERIFY( retval == -1 );
    VERIFY( mbs[0] == '\0' );
    VERIFY( ++violations == NumViolations );
#endif



}




/****
***** Test mbtob(), sisinit(), mbrlen(), mbrtowc(), wcrtomb(), mbsrtowcs(),
***** wcsrtombs(), ie. all Normative Addendum functions.
****/

void TestAddendum( void )
{
    wchar_t             wc;
    wchar_t             wcs[10];
    wchar_t *           wcsPtr;
    int                 status;
    mbstate_t           state;
    unsigned char       mbc[MB_LEN_MAX];
    unsigned char       mbcs[10*MB_LEN_MAX+1];
    unsigned char *     mbcsPtr;

    errno_t             rc;
    size_t              retval;
    int                 violations = NumViolations;

    status = wctob( L'!' );
    VERIFY( status == '!' );

    status = mbtowc( &wc, "\x90\x90", 2 );
    #ifndef DUMMY_DBCS
        VERIFY( status == 2 );
        status = wctob( wc );
        VERIFY( status == EOF );
    #else
        VERIFY( status == 1 );
        status = wctob( wc );
        VERIFY( status == 0x90 );
    #endif

    status = sisinit( NULL );
    VERIFY( status != 0 );

    state = 0;
    status = sisinit( &state );
    VERIFY( status == 0 );

    state = 123;
    status = sisinit( &state );
    VERIFY( status == 0 );

    status = mbrlen( "\0""!", 2, &state );
    VERIFY( status == 0 );

    status = mbrlen( "!", 2, &state );
    VERIFY( status == 1 );

    wc = L'\0';
    rc = wcrtomb_s( &retval, mbc, MB_LEN_MAX, wc, &state );
    VERIFY( rc == 0 );
    VERIFY( *mbc == '\0' );
    VERIFY( retval == 1 );
    VERIFY( violations == NumViolations );

    rc = wcrtomb_s( &retval, mbc, MB_LEN_MAX, L'X', &state );
    VERIFY( rc == 0 );
    VERIFY( *mbc == 'X' );
    VERIFY( retval == 1 );
    VERIFY( violations == NumViolations );

    rc = wcrtomb_s( &retval, NULL, 0, L'X', &state );
    VERIFY( rc == 0 );
    VERIFY( retval == 1 );
    VERIFY( violations == NumViolations );


    _mbscpy( mbcs, "Foo!\x90\x90" );
    mbcsPtr = mbcs;
    wcs[5] = wcs[6] = L'-';
    rc = mbsrtowcs_s( &retval, wcs, 10, (const char**)(&mbcsPtr), 6, &state );
    VERIFY( rc == 0 );
    VERIFY( retval == 5 );
    VERIFY( wcs[0] == L'F' );
    VERIFY( wcs[1] == L'o' );
    VERIFY( wcs[2] == L'o' );
    VERIFY( wcs[3] == L'!' );

    mbcsPtr = mbcs;
    rc = mbsrtowcs_s( &retval, NULL, 0, (const char**)(&mbcsPtr), 1, &state );
    VERIFY( rc == 0 );
    VERIFY( retval == 5 );

    wcsPtr = wcs;
    _mbsset( mbcs, _mbsnextc("#") );
    rc = wcsrtombs_s( &retval, mbcs, 10, (const wchar_t**)(&wcsPtr), 6, &state );
    VERIFY( rc == 0 );
    VERIFY( retval == 6 );
    mbcs[retval] = '\0';
    VERIFY( _mbscmp( mbcs, "Foo!\x90\x90" ) == 0 );

    wcsPtr = wcs;
    rc = wcsrtombs_s( &retval, NULL, 0, (const wchar_t**)(&wcsPtr), 20, &state );
    VERIFY( rc == 0 );
    VERIFY( retval == 6 );

    /***********************************************************************/
    /*  test runtime-constraints                                           */
    /***********************************************************************/

    *mbc = 'X';
    rc = wcrtomb_s( NULL, mbc, MB_LEN_MAX, wc, &state );
    VERIFY( rc != 0 );
    VERIFY( *mbc == '\0' );
    VERIFY( ++violations == NumViolations );

    *mbc = 'X';
    rc = wcrtomb_s( &retval, mbc, MB_LEN_MAX, L'X', NULL );
    VERIFY( rc != 0 );
    VERIFY( *mbc == '\0' );
    VERIFY( retval == -1 );
    VERIFY( ++violations == NumViolations );

    rc = wcrtomb_s( &retval, NULL, MB_LEN_MAX, L'X', &state );
    VERIFY( rc != 0 );
    VERIFY( retval == -1 );
    VERIFY( ++violations == NumViolations );

    *mbc = 'X';
    rc = wcrtomb_s( &retval, mbc, 0, L'X', &state );
    VERIFY( rc != 0 );
    VERIFY( *mbc == 'X' );
    VERIFY( retval == -1 );
    VERIFY( ++violations == NumViolations );

#if RSIZE_MAX != SIZE_MAX
    rc = wcrtomb_s( &retval, mbc, ~0, L'X', &state );
    VERIFY( rc != 0 );
    VERIFY( retval == -1 );
    VERIFY( *mbc == 'X' );
    VERIFY( ++violations == NumViolations );
#endif

    mbcsPtr = mbcs;
    rc = mbsrtowcs_s( NULL, NULL, 0, (const char**)(&mbcsPtr), 1, &state );
    VERIFY( rc != 0 );
    VERIFY( ++violations == NumViolations );

    rc = mbsrtowcs_s( &retval, NULL, 0, NULL, 1, &state );
    VERIFY( rc != 0 );
    VERIFY( retval == -1 );
    VERIFY( ++violations == NumViolations );

    mbcsPtr = NULL;
    rc = mbsrtowcs_s( &retval, NULL, 0, (const char**)(&mbcsPtr), 1, &state );
    VERIFY( rc != 0 );
    VERIFY( retval == -1 );
    VERIFY( ++violations == NumViolations );

    rc = mbsrtowcs_s( &retval, NULL, 0, (const char**)(&wcsPtr), 1, NULL );
    VERIFY( rc != 0 );
    VERIFY( retval == -1 );
    VERIFY( ++violations == NumViolations );

    rc = mbsrtowcs_s( &retval, NULL, 111, (const char**)(&wcsPtr), 1, &state );
    VERIFY( rc != 0 );
    VERIFY( retval == -1 );
    VERIFY( ++violations == NumViolations );

    wcsPtr = wcs;
    rc = wcsrtombs_s( &retval, mbcs, 5, (const wchar_t**)(&wcsPtr), 10, &state );
    VERIFY( rc != 0 );
    VERIFY( retval == -1 );
    VERIFY( ++violations == NumViolations );
}


/****
***** Program entry point.
****/

void main( int argc, char *argv[] )
{
    int             exitcode;

    /*** Initialize ***/
#ifdef __SW_BW
    FILE            *my_stdout;
    my_stdout = freopen( "tmp.log", "a", stdout );
    if( my_stdout == NULL ) {
        fprintf( stderr, "Unable to redirect stdout\n" );
        exit( EXIT_FAILURE );
    }
#endif
    strcpy( ProgramName, strlwr( argv[0] ) );   /* store executable filename */
    if( _setmbcp( 932 ) != 0 ) {
        printf( "Cannot initialize code page.\n\n" );
        exit( EXIT_FAILURE );
    }

    /***********************************************************************/
    /*  set constraint-handler                                             */
    /***********************************************************************/

    set_constraint_handler_s( my_constraint_handler );


    /*** Test various functions ***/
    TestToWide();                               /* conversion to wide chars */
    TestFromWide();                             /* conversion from wide chars */
    TestAddendum();                             /* Normative Addendum stuff */


    /*** Print a pass/fail message and quit ***/
    if( NumErrors == 0 ) {
        printf( "%s: SUCCESS.\n", ProgramName );
#ifdef __SW_BW
        fprintf( stderr, "%s: SUCCESS.\n", ProgramName );
#endif
        exitcode = EXIT_SUCCESS;
    } else {
        printf( "%s: FAILURE (%d errors).\n", ProgramName, NumErrors );
#ifdef __SW_BW
        fprintf( stderr, "%s: FAILURE (%d errors).\n",
                 ProgramName, NumErrors );
#endif
        exitcode = EXIT_FAILURE;
    }

#ifdef __SW_BW
    fclose( my_stdout );
    _dwShutDown();
#endif
    exit( exitcode );
}
