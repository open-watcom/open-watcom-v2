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
* Description:  Non-exhaustive test of the safe wide string functions.
*
****************************************************************************/


#define __STDC_WANT_LIB_EXT1__ 1       // Enable Safer C interfaces

#include <wchar.h>
#include <locale.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#ifdef __SW_BW
    #include <wdefwin.h>
#endif
#include "rterrno.h"
#include "thread.h"


#define VERIFY( exp )   if( !(exp) ) {                                      \
                            printf( "%s: ***FAILURE*** at line %d of %s.\n",\
                                    ProgramName, __LINE__,                  \
                                    strlwr(__FILE__) );                     \
                            NumErrors++;                                    \
                            exit( -1 );                                     \
                        }
#define ARRAYCOUNT( array )  ( sizeof( array )  / sizeof( array[ 0 ] ) )

char    ProgramName[128];   /* executable filename */
int     NumErrors = 0;      /* number of errors */
int     NumViolations = 0;  /* runtime-constraint violation counter */


/* Runtime-constraint handler for tests; doesn't abort program. */
void my_constraint_handler( const char *msg, void *ptr, errno_t error )
{
#ifdef DEBUG_MSG
    fprintf( stderr, "Runtime-constraint in %s", msg );
#endif
    ++NumViolations;
}

/* Test wmemcpy_s(), wmemmove_s()                                       */
/* Test wcscpy_s(), wcscat_s(), wcsnlen_s(), wcsncpy_s(), wcsncat_s() */
/* Test wcstok_s()                                                    */

void TestMove_s( void )
{

    wchar_t    buf[128];
    wchar_t    s2[] = L"VALUE";
    wchar_t    str[] = L"VALUE";

    wchar_t    src1[100] = L"hello";
    wchar_t    src2[7] = L"goodbye";
    wchar_t    dst1[6], dst2[5], dst3[5];

    wchar_t    sc1[100] = L"good";
    wchar_t    sc2[6] = L"hello";
    wchar_t    sc3[6] = L"hello";
    wchar_t    sc4[7] = L"abc";
    wchar_t    sc5[1000] = L"bye";

    int     violations = NumViolations;

    /*   wcstok_s */
    static wchar_t     str1[] = L"?a???b,,,#c";
    static wchar_t     str2[] = L"\t \t";
    static wchar_t     str3[] = L"?a???b,,,#c";
    wchar_t            *t, *ptr1, *ptr2, *ptr3;
    rsize_t         max1 = ARRAYCOUNT( str1 );
    rsize_t         max2 = ARRAYCOUNT( str2 );
    rsize_t         max3 = ARRAYCOUNT( str3 );


    /***********************************************************************/
    /*  set constraint-handler                                             */
    /***********************************************************************/

    set_constraint_handler_s( my_constraint_handler );

    /***********************************************************************/
    /*  memcpy_s                                                           */
    /***********************************************************************/
//  printf( "Test memcpys      (%s).\n", ProgramName );

    /* Test the "good" case */
    VERIFY( wmemcpy_s( buf, ARRAYCOUNT( buf ), s2, 0 ) == 0 );
    VERIFY( wmemcpy_s( buf, ARRAYCOUNT( buf ), s2, 1 + wcslen( s2 ) ) == 0 );
    VERIFY( wmemcpy_s( buf, wcslen( s2 ) + 2, s2, 1 + wcslen( s2 ) ) == 0 );

    VERIFY( wcslen( buf ) == wcslen( L"VALUE" ) );
    VERIFY( wcscmp( buf, L"VALUE" ) == 0 );
    VERIFY( NumViolations == violations );

    /* Test various failing cases */
    /* Test runtime-constraint violations */
    VERIFY( wmemcpy_s( buf, 3, s2, wcslen( s2 ) ) != 0 );
    VERIFY( buf[0] == L'\0' );
    VERIFY( NumViolations == ++violations );

    VERIFY( wmemcpy_s( NULL, ARRAYCOUNT( buf ), s2, wcslen( s2 ) ) != 0 );
    VERIFY( NumViolations == ++violations );

    VERIFY( wmemcpy_s( buf, ARRAYCOUNT( buf ), NULL, wcslen( s2 ) ) != 0 );
    VERIFY( NumViolations == ++violations );

    VERIFY( wmemcpy_s( buf, ARRAYCOUNT( buf ), s2, sizeof( buf ) / sizeof( buf[0] )+ 1 ) != 0 );
    VERIFY( NumViolations == ++violations );

    VERIFY( wmemcpy_s( buf, ARRAYCOUNT( buf ), buf + 1, wcslen( s2 ) ) != 0 );
    VERIFY( NumViolations == ++violations );

#if RSIZE_MAX != SIZE_MAX
    VERIFY( wmemcpy_s( buf, ARRAYCOUNT( buf ), s2, ~0 ) != 0 );
    VERIFY( NumViolations == ++violations );
#endif

    /***********************************************************************/
    /*  memmove_s                                                          */
    /***********************************************************************/
//  printf( "Test memmove      (%s).\n", ProgramName );

    /* Test the "good" cases */
    VERIFY( wmemmove_s( buf, ARRAYCOUNT( buf ), s2, 0 ) == 0 );
    VERIFY( wmemmove_s( buf, ARRAYCOUNT( buf ), s2, 1 + wcslen( s2 ) ) == 0 );

    VERIFY( wmemmove_s( buf, ARRAYCOUNT( buf ), buf + 1, 1 + wcslen( s2 ) ) == 0 );

    VERIFY( wmemmove_s( buf, 1 + wcslen( s2 ), s2, 1 + wcslen( s2 ) ) == 0 );

    VERIFY( wcslen( buf ) == wcslen( L"VALUE" ) );
    VERIFY( wcscmp( buf, L"VALUE" ) == 0 );
    VERIFY( NumViolations == violations );

    /* Test various failing cases */
    /* Test runtime-constraint violations */
    VERIFY( wmemmove_s( buf, 3, s2, wcslen( s2 ) ) != 0 );
    VERIFY( buf[0] == L'\0' );
    VERIFY( NumViolations == ++violations );

    VERIFY( wmemmove_s( NULL, ARRAYCOUNT( buf ), s2, wcslen( s2 ) ) != 0 );
    VERIFY( NumViolations == ++violations );

    VERIFY( wmemmove_s( buf, ARRAYCOUNT( buf ), NULL, wcslen( s2 ) ) != 0 );
    VERIFY( NumViolations == ++violations );

    VERIFY( wmemmove_s( buf, ARRAYCOUNT( buf ), s2, ARRAYCOUNT( buf ) + 1 ) != 0 );
    VERIFY( NumViolations == ++violations );

#if RSIZE_MAX != SIZE_MAX
    VERIFY( wmemmove_s( buf, ~0, s2, wcslen( s2 ) ) != 0 );
    VERIFY( NumViolations == ++violations );

    VERIFY( wmemmove_s( buf, ARRAYCOUNT( buf ), s2, ~0 ) != 0 );
    VERIFY( NumViolations == ++violations );
#endif

    /***********************************************************************/
    /*  wcscpy_s                                                           */
    /***********************************************************************/
//  printf( "Test memcpy       (%s).\n", ProgramName );

    /* Test the "good" cases */
    VERIFY( wcscpy_s( buf, ARRAYCOUNT( buf ), s2 ) == 0 );
    VERIFY( wcscpy_s( buf, ARRAYCOUNT( buf ), s2 ) == 0 );
    VERIFY( wcscpy_s( buf, wcslen( s2 ) + 1, s2 ) == 0 );


    VERIFY( wcslen( buf ) == wcslen( L"VALUE" ) );
    VERIFY( wcscmp( buf, L"VALUE" ) == 0 );
    VERIFY( NumViolations == violations );

    /* Test various failing cases */
    /* Test runtime-constraint violations */
    VERIFY( wcscpy_s( buf, 3, s2 ) != 0 );
    VERIFY( NumViolations == ++violations );
    VERIFY( buf[0] == L'\0' );

    VERIFY( wcscpy_s( NULL, ARRAYCOUNT( buf ), s2 ) != 0 );
    VERIFY( NumViolations == ++violations );

    VERIFY( wcscpy_s( buf, ARRAYCOUNT( buf ), NULL ) != 0 );
    VERIFY( NumViolations == ++violations );

    VERIFY( wcscpy_s( buf, 5, s2 ) != 0 );
    VERIFY( NumViolations == ++violations );

    VERIFY( wcscpy_s( buf, ARRAYCOUNT( buf ), buf + 1 ) != 0 );
    VERIFY( NumViolations == ++violations );

#if RSIZE_MAX != SIZE_MAX
    VERIFY( wcscpy_s( buf, ~0, s2 ) != 0 );
    VERIFY( NumViolations == ++violations );
#endif

    /***********************************************************************/
    /*  wcscat_s                                                           */
    /***********************************************************************/
//  printf( "Test wcscat       (%s).\n", ProgramName );
    wcscpy( sc1,src1 );
    VERIFY( wcscmp( sc1,src1 ) == 0 );
    VERIFY( wcscat_s( sc1, 100, sc5 ) == 0 );
    VERIFY( wcscmp( sc1, L"hellobye") == 0 );

    VERIFY( wcscat_s( sc2, 6, L"" ) == 0 );

    VERIFY( wcscat_s( sc3, 6, L"X" ) != 0 );
    VERIFY( NumViolations == ++violations );
    VERIFY( sc3[0] == L'\0');

    VERIFY( wcscat_s(sc4, 7, L"defghijklmn") != 0);
    VERIFY( NumViolations == ++violations );

    VERIFY( wcscmp(sc4, L"" ) == 0);


    /***********************************************************************/
    /*  wcsnlen_s                                                          */
    /***********************************************************************/

//  printf( "Test wcsnlen      (%s).\n", ProgramName );
    /* Test the "good" case */
    VERIFY( wcsnlen_s( str, ARRAYCOUNT( str ) ) == wcslen( str ) );
    VERIFY( wcsnlen_s( str, 4 ) == 4 );
    VERIFY( wcsnlen_s( str, 0 ) == 0 );
    VERIFY( wcsnlen_s( NULL, 1000 ) == 0 );

    /* Test various failing cases */

    /* No runtime-constraint violations to test */

    VERIFY( NumViolations == violations );

    /***********************************************************************/
    /*  wcsncpy_s                                                          */
    /***********************************************************************/

//  printf( "Test wcsncpy      (%s).\n", ProgramName );
    /* Test the "good" case */
    VERIFY( wcsncpy_s( buf, ARRAYCOUNT( buf ), s2, 0 ) == 0 );
    VERIFY( wcsncpy_s( buf, ARRAYCOUNT( buf ), s2, wcslen( s2 ) ) == 0 );
    VERIFY( wcsncpy_s( buf, wcslen( s2 ) + 1, s2, wcslen( s2 ) ) == 0 );

    VERIFY( wcslen( buf ) == wcslen( L"VALUE" ) );
    VERIFY( wcscmp( buf, L"VALUE" ) == 0 );
    VERIFY( NumViolations == violations );

    VERIFY( wcsncpy_s( dst1, 6, src1, 100 ) == 0 );
    VERIFY( wcscmp( dst1, src1 ) == 0 );

    VERIFY( wcsncpy_s( dst3, 5, src2, 4 ) == 0 );

    /* Test various failing cases */
    /* Test runtime-constraint violations */
    VERIFY( wcsncpy_s( buf, 3, s2, wcslen( s2 ) ) != 0 );
    VERIFY( NumViolations == ++violations );
    VERIFY( buf[0] == '\0' );

    VERIFY( wcsncpy_s( NULL, ARRAYCOUNT( buf ), s2, wcslen( s2 ) ) != 0 );
    VERIFY( NumViolations == ++violations );

    VERIFY( wcsncpy_s( buf, ARRAYCOUNT( buf ), NULL, wcslen( s2 ) ) != 0 );
    VERIFY( NumViolations == ++violations );

    VERIFY( wcsncpy_s( buf, ARRAYCOUNT( buf ), buf + 1, wcslen( s2 ) ) != 0 );
    VERIFY( NumViolations == ++violations );

    VERIFY( wcsncpy_s( dst2, 5, src2, 7 ) != 0 );
    VERIFY( NumViolations == ++violations );


#if RSIZE_MAX != SIZE_MAX
    VERIFY( wcsncpy_s( buf, ~0, s2, wcslen( s2 ) ) != 0 );
    VERIFY( NumViolations == ++violations );

    VERIFY( wcsncpy_s( buf, ARRAYCOUNT( buf ), s2, ~0 ) != 0 );
    VERIFY( NumViolations == ++violations );
#endif


    /***********************************************************************/
    /*  wcsncat_s                                                          */
    /***********************************************************************/

//  printf( "Test wcsncat      (%s).\n", ProgramName );
    wcscpy( sc1, L"good" );
    wcscpy( sc2, L"hello" );
    wcscpy( sc3, L"hello" );
    wcscpy( sc4, L"abc" );
    VERIFY( wcsncat_s( sc1, 100, sc5, 1000 ) == 0);
    VERIFY( wcscmp( sc1, L"goodbye" ) == 0 );

    VERIFY( wcsncat_s( sc2, 6, L"", 1 ) == 0 );

    VERIFY( wcsncat_s( sc4, 7, L"defghijklmn", 3 ) == 0 );
    VERIFY( wcscmp( sc4, L"abcdef" ) == 0 );

    /* Test runtime-constraint violations */
    VERIFY( wcsncat_s( sc3, 6, L"XXX", 3 ) != 0 );
    VERIFY( NumViolations == ++violations );
    VERIFY( sc3[0] == L'\0');

    /***********************************************************************/
    /*  wcstok_s                                                           */
    /***********************************************************************/

//  printf( "Test wcstok       (%s).\n", ProgramName );
    VERIFY( (t = wcstok_s( str1, &max1, L"?", &ptr1 )) != NULL );    /* points to the token "a" */
    VERIFY( wcscmp( t, L"a" ) == 0 );

    VERIFY( (t = wcstok_s( NULL, &max1, L",", &ptr1 )) != NULL );    /* points to the token "??b" */
    VERIFY( wcscmp( t, L"??b" ) == 0 );

    VERIFY( NULL == wcstok_s( str2, &max2, L" \t", &ptr2 ) );        /* null pointer */
    VERIFY( NumViolations == violations );

    VERIFY( (t = wcstok_s( NULL, &max1, L"#,", &ptr1 )) != NULL  );  /* points to the token "c" */
    VERIFY( wcscmp( t, L"c" ) == 0 );
    VERIFY( ptr1 != NULL );
    VERIFY( NumViolations == violations );

    VERIFY( NULL == wcstok_s( NULL, &max1, L"#,", &ptr1 ) );  /* at the end */

    wcscpy( str1, str3 );
    max1 = ARRAYCOUNT( str1 );
    VERIFY( NULL == wcstok_s( str1, &max1, str3, &ptr3 ) );         /* only delimiter chars */

//  printf( "Test wcstok rtc   (%s).\n", ProgramName );
    /* Test runtime-constraint violations */
    ptr1 = NULL;
    VERIFY( NULL == wcstok_s( NULL, &max1, L"?", &ptr1 ) );          /* null pointer */
    VERIFY( NumViolations == ++violations );

    VERIFY( NULL == wcstok_s( str3, NULL, L"?", &ptr1 ) );
    VERIFY( NumViolations == ++violations );

    VERIFY( NULL == wcstok_s( str3, &max3, NULL, &ptr1 ) );
    VERIFY( NumViolations == ++violations );

    VERIFY( NULL == wcstok_s( str3, &max3, L"?", NULL ) );
    VERIFY( NumViolations == ++violations );

    ptr3 = NULL;
    VERIFY( NULL == wcstok_s( NULL, &max3, L"?", &ptr3 ) );
    VERIFY( NumViolations == ++violations );

#if RSIZE_MAX != SIZE_MAX
    max1 = ~0;
    VERIFY( NULL == wcstok_s( str3, &max1, L"?", &ptr1 ) );
    VERIFY( NumViolations == ++violations );
#endif
}

/* Test wcserrorlen_s(), wcserror_s()                                      */
/*                                                                         */

void TestError_s( void )
{
    wchar_t     error[ 256 ];
    size_t      errlen;

    int     violations = NumViolations;

    errlen = wcserrorlen_s( EBADF );
    VERIFY( errlen != 0 );

    VERIFY( wcserror_s( error, ARRAYCOUNT( error ), EBADF ) == 0 ); /* get an error string */
    VERIFY( wcslen( error ) != 0 );

    VERIFY( wcserror_s( error, errlen - 1, EBADF ) != 0 ); /* truncated error string */
    VERIFY( wcscmp( error + errlen - 5, L"..." ) == 0 ); /* really truncated? */
    VERIFY( NumViolations == violations );

    //rt constraints
    VERIFY( wcserror_s( NULL, errlen - 1, EBADF ) != 0 );
    VERIFY( NumViolations == ++violations );

    VERIFY( wcserror_s( error, 0, EBADF ) != 0 );
    VERIFY( NumViolations == ++violations );

#if RSIZE_MAX != SIZE_MAX
    VERIFY( wcserror_s( error, ~0, EBADF ) != 0 );
    VERIFY( NumViolations == ++violations );
#endif

}


/****
***** Program entry point.
****/

int main( int argc, char *argv[] )
{
#ifdef __SW_BW
    FILE *my_stdout;
    my_stdout = freopen( "tmp.log", "a", stdout );
    if( my_stdout == NULL ) {
        fprintf( stderr, "Unable to redirect stdout\n" );
        exit( EXIT_FAILURE );
    }
#endif
    /*** Initialize ***/
    strcpy( ProgramName, strlwr( argv[0] ) );   /* store filename */

    /*** Test various safe functions ***/
    TestMove_s();
    TestError_s();

    /*** Print a pass/fail message and quit ***/
    if( NumErrors != 0 ) {
        printf( "%s: FAILURE (%d errors).\n", ProgramName, NumErrors );
        return( EXIT_FAILURE );
    }
    printf( "Tests completed (%s).\n", ProgramName );

#ifdef __SW_BW
    fprintf( stderr, "Tests completed (%s).\n", ProgramName );
    fclose( my_stdout );
    _dwShutDown();
#endif
    return( EXIT_SUCCESS );
}
