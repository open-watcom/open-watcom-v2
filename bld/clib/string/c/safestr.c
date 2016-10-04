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
* Description:  Non-exhaustive test of the C library safe string functions.
*
****************************************************************************/


#define __STDC_WANT_LIB_EXT1__ 1       // Enable Safer C interfaces

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

/* Test memcpy_s(), memmove_s()                                       */
/* Test strcpy_s(), strcat_s(), strnlen_s(), strncpy_s(), strncat_s() */
/* Test strtok_s()                                                    */

void TestMove_s( void )
{

    char    buf[128];
    char    s2[] = "VALUE";
    char    str[] = "VALUE";

    char    src1[100] = "hello";
    char    src2[7] = {'g', 'o', 'o', 'd', 'b', 'y', 'e'};
    char    dst1[6], dst2[5], dst3[5];

    char    sc1[100] = "good";
    char    sc2[6] = "hello";
    char    sc3[6] = "hello";
    char    sc4[7] = "abc";
    char    sc5[1000] = "bye";

    int     violations = NumViolations;

    /*   strtok_s */
    static char     str1[] = "?a???b,,,#c";
    static char     str2[] = "\t \t";
    static char     str3[] = "?a???b,,,#c";
    char            *t, *ptr1, *ptr2, *ptr3;
    rsize_t         max1 = sizeof( str1 );
    rsize_t         max2 = sizeof( str2 );
    rsize_t         max3 = sizeof( str3 );


    /***********************************************************************/
    /*  set constraint-handler                                             */
    /***********************************************************************/

    set_constraint_handler_s( my_constraint_handler );

    /***********************************************************************/
    /*  memcpy_s                                                           */
    /***********************************************************************/

    /* Test the "good" case */
    VERIFY( memcpy_s( buf, sizeof( buf ), s2, 0 ) == 0 );
    VERIFY( memcpy_s( buf, sizeof( buf ), s2, 1 + strlen( s2 ) ) == 0 );
    VERIFY( memcpy_s( buf, strlen( s2 ) + 2, s2, 1 + strlen( s2 ) ) == 0 );

    VERIFY( strlen( buf ) == strlen( "VALUE" ) );
    VERIFY( strcmp( buf, "VALUE" ) == 0 );
    VERIFY( NumViolations == violations );

    /* Test various failing cases */
    /* Test runtime-constraint violations */
    VERIFY( memcpy_s( buf, 3, s2, strlen( s2 ) ) != 0 );
    VERIFY( buf[0] == '\0' );
    VERIFY( NumViolations == ++violations );

    VERIFY( memcpy_s( NULL, sizeof( buf ), s2, strlen( s2 ) ) != 0 );
    VERIFY( NumViolations == ++violations );

    VERIFY( memcpy_s( buf, sizeof( buf ), NULL, strlen( s2 ) ) != 0 );
    VERIFY( NumViolations == ++violations );

    VERIFY( memcpy_s( buf, sizeof( buf ), s2, sizeof( buf ) + 1 ) != 0 );
    VERIFY( NumViolations == ++violations );

    VERIFY( memcpy_s( buf, sizeof( buf ), buf + 1, strlen( s2 ) ) != 0 );
    VERIFY( NumViolations == ++violations );

#if RSIZE_MAX != SIZE_MAX
    VERIFY( memcpy_s( buf, sizeof( buf ), s2, ~0 ) != 0 );
    VERIFY( NumViolations == ++violations );
#endif

    /***********************************************************************/
    /*  memmove_s                                                          */
    /***********************************************************************/

    /* Test the "good" cases */
    VERIFY( memmove_s( buf, sizeof( buf ) , s2, 0 ) == 0 );
    VERIFY( memmove_s( buf, sizeof( buf ) , s2, 1 + strlen( s2 ) ) == 0 );

    VERIFY( memmove_s( buf, sizeof( buf ), buf + 1, 1 + strlen( s2 ) ) == 0 );

    VERIFY( memmove_s( buf, 1 + strlen( s2 ), s2, 1 + strlen( s2 ) ) == 0 );

    VERIFY( strlen( buf ) == strlen( "VALUE" ) );
    VERIFY( strcmp( buf, "VALUE" ) == 0 );
    VERIFY( NumViolations == violations );

    /* Test various failing cases */
    /* Test runtime-constraint violations */
    VERIFY( memmove_s( buf, 3, s2, strlen( s2 ) ) != 0 );
    VERIFY( buf[0] == '\0' );
    VERIFY( NumViolations == ++violations );

    VERIFY( memmove_s( NULL, sizeof( buf ), s2, strlen( s2 ) ) != 0 );
    VERIFY( NumViolations == ++violations );

    VERIFY( memmove_s( buf, sizeof( buf ), NULL, strlen( s2 ) ) != 0 );
    VERIFY( NumViolations == ++violations );

    VERIFY( memmove_s( buf, sizeof( buf ), s2, sizeof( buf ) + 1 ) != 0 );
    VERIFY( NumViolations == ++violations );

#if RSIZE_MAX != SIZE_MAX
    VERIFY( memmove_s( buf, ~0, s2, strlen( s2 ) ) != 0 );
    VERIFY( NumViolations == ++violations );

    VERIFY( memmove_s( buf, sizeof( buf ), s2, ~0 ) != 0 );
    VERIFY( NumViolations == ++violations );
#endif

    /***********************************************************************/
    /*  strcpy_s                                                           */
    /***********************************************************************/

    /* Test the "good" cases */
    VERIFY( strcpy_s( buf, sizeof( buf ), s2 ) == 0 );
    VERIFY( strcpy_s( buf, sizeof( buf ), s2 ) == 0 );
    VERIFY( strcpy_s( buf, strlen( s2 ) + 1, s2 ) == 0 );


    VERIFY( strlen( buf ) == strlen( "VALUE" ) );
    VERIFY( strcmp( buf, "VALUE" ) == 0 );
    VERIFY( NumViolations == violations );

    /* Test various failing cases */
    /* Test runtime-constraint violations */
    VERIFY( strcpy_s( buf, 3, s2 ) != 0 );
    VERIFY( NumViolations == ++violations );
    VERIFY( buf[0] == '\0' );

    VERIFY( strcpy_s( NULL, sizeof( buf ), s2 ) != 0 );
    VERIFY( NumViolations == ++violations );

    VERIFY( strcpy_s( buf, sizeof( buf ), NULL ) != 0 );
    VERIFY( NumViolations == ++violations );

    VERIFY( strcpy_s( buf, 5, s2 ) != 0 );
    VERIFY( NumViolations == ++violations );

    VERIFY( strcpy_s( buf, sizeof( buf ), buf + 1 ) != 0 );
    VERIFY( NumViolations == ++violations );

#if RSIZE_MAX != SIZE_MAX
    VERIFY( strcpy_s( buf, ~0, s2 ) != 0 );
    VERIFY( NumViolations == ++violations );
#endif

    /***********************************************************************/
    /*  strcat_s                                                           */
    /***********************************************************************/
    strcpy( sc1, src1 );
    VERIFY( strcat_s( sc1, 100, sc5 ) == 0 );
    VERIFY( strcmp( sc1, "hellobye") == 0 );

    VERIFY( strcat_s( sc2, 6, "" ) == 0 );

    VERIFY( strcat_s( sc3, 6, "X" ) != 0 );
    VERIFY( NumViolations == ++violations );
    VERIFY( sc3[0] == '\0');

    VERIFY( strcat_s(sc4, 7, "defghijklmn") != 0);
    VERIFY( NumViolations == ++violations );

    VERIFY( strcmp(sc4, "" ) == 0);


    /***********************************************************************/
    /*  strnlen_s                                                          */
    /***********************************************************************/

    /* Test the "good" case */
    VERIFY( strnlen_s( str, sizeof( str ) ) == strlen( str ) );
    VERIFY( strnlen_s( str, 4 ) == 4 );
    VERIFY( strnlen_s( str, 0 ) == 0 );
    VERIFY( strnlen_s( NULL, 1000 ) == 0 );

    /* Test various failing cases */

    /* No runtime-constraint violations to test */

    VERIFY( NumViolations == violations );

    /***********************************************************************/
    /*  strncpy_s                                                          */
    /***********************************************************************/

    /* Test the "good" case */
    VERIFY( strncpy_s( buf, sizeof( buf ), s2, 0 ) == 0 );
    VERIFY( strncpy_s( buf, sizeof( buf ), s2, strlen( s2 ) ) == 0 );
    VERIFY( strncpy_s( buf, strlen( s2 ) + 1, s2, strlen( s2 ) ) == 0 );

    VERIFY( strlen( buf ) == strlen( "VALUE" ) );
    VERIFY( strcmp( buf, "VALUE" ) == 0 );
    VERIFY( NumViolations == violations );

    VERIFY( strncpy_s( dst1, 6, src1, 100 ) == 0 );
    VERIFY( strcmp( dst1, src1 ) == 0 );

    VERIFY( strncpy_s( dst3, 5, src2, 4 ) == 0 );

    /* Test various failing cases */
    /* Test runtime-constraint violations */
    VERIFY( strncpy_s( buf, 3, s2, strlen( s2 ) ) != 0 );
    VERIFY( NumViolations == ++violations );
    VERIFY( buf[0] == '\0' );

    VERIFY( strncpy_s( NULL, sizeof( buf ), s2, strlen( s2 ) ) != 0 );
    VERIFY( NumViolations == ++violations );

    VERIFY( strncpy_s( buf, sizeof( buf ), NULL, strlen( s2 ) ) != 0 );
    VERIFY( NumViolations == ++violations );

    VERIFY( strncpy_s( buf, sizeof( buf ), buf + 1, strlen( s2 ) ) != 0 );
    VERIFY( NumViolations == ++violations );

    VERIFY( strncpy_s( dst2, 5, src2, 7 ) != 0 );
    VERIFY( NumViolations == ++violations );


#if RSIZE_MAX != SIZE_MAX
    VERIFY( strncpy_s( buf, ~0, s2, strlen( s2 ) ) != 0 );
    VERIFY( NumViolations == ++violations );

    VERIFY( strncpy_s( buf, sizeof( buf ), s2, ~0 ) != 0 );
    VERIFY( NumViolations == ++violations );
#endif


    /***********************************************************************/
    /*  strncat_s                                                          */
    /***********************************************************************/

    strcpy( sc1, "good" );
    strcpy( sc2, "hello" );
    strcpy( sc3, "hello" );
    strcpy( sc4, "abc" );
    VERIFY( strncat_s( sc1, 100, sc5, 1000 ) == 0);
    VERIFY( strcmp( sc1, "goodbye" ) == 0 );

    VERIFY( strncat_s( sc2, 6, "", 1 ) == 0 );

    VERIFY( strncat_s( sc4, 7, "defghijklmn", 3 ) == 0 );
    VERIFY( strcmp( sc4, "abcdef" ) == 0 );

    /* Test runtime-constraint violations */
    VERIFY( strncat_s( sc3, 6, "XXX", 3 ) != 0 );
    VERIFY( NumViolations == ++violations );
    VERIFY( sc3[0] == '\0');

    /***********************************************************************/
    /*  strtok_s                                                           */
    /***********************************************************************/

    VERIFY( (t = strtok_s( str1, &max1, "?", &ptr1 )) != NULL );    /* points to the token "a" */
    VERIFY( strcmp( t, "a" ) == 0 );

    VERIFY( (t = strtok_s( NULL, &max1, ",", &ptr1 )) != NULL );    /* points to the token "??b" */
    VERIFY( strcmp( t, "??b" ) == 0 );

    VERIFY( NULL == strtok_s( str2, &max2, " \t", &ptr2 ) );        /* null pointer */
    VERIFY( NumViolations == violations );

    VERIFY( (t = strtok_s( NULL, &max1, "#,", &ptr1 )) != NULL  );  /* points to the token "c" */
    VERIFY( strcmp( t, "c" ) == 0 );
    VERIFY( ptr1 != NULL );
    VERIFY( NumViolations == violations );


    VERIFY( NULL == strtok_s( NULL, &max1, "#,", &ptr1 ) );  /* at the end */

    strcpy( str1, str3 );
    max1 = sizeof( str1 );
    VERIFY( NULL == strtok_s( str1, &max1, str3, &ptr3 ) );         /* only delimiter chars */

    /* Test runtime-constraint violations */
    ptr1 = NULL;
    VERIFY( NULL == strtok_s( NULL, &max1, "?", &ptr1 ) );          /* null pointer */
    VERIFY( NumViolations == ++violations );

    VERIFY( NULL == strtok_s( str3, NULL, "?", &ptr1 ) );
    VERIFY( NumViolations == ++violations );

    VERIFY( NULL == strtok_s( str3, &max3, NULL, &ptr1 ) );
    VERIFY( NumViolations == ++violations );

    VERIFY( NULL == strtok_s( str3, &max3, "?", NULL ) );
    VERIFY( NumViolations == ++violations );

    ptr3 = NULL;
    VERIFY( NULL == strtok_s( NULL, &max3, "?", &ptr3 ) );
    VERIFY( NumViolations == ++violations );

#if RSIZE_MAX != SIZE_MAX
    max1 = ~0;
    VERIFY( NULL == strtok_s( str3, &max1, "?", &ptr1 ) );
    VERIFY( NumViolations == ++violations );
#endif
}

/* Test strerrorlen_s(), strerror_s()                                      */
/*                                                                         */

void TestError_s( void )
{
    char        error[ 256 ];
    size_t      errlen;

    int     violations = NumViolations;

    errlen = strerrorlen_s( EBADF );
    VERIFY( errlen != 0 );

    VERIFY( strerror_s( error, sizeof( error ), EBADF ) == 0 ); /* get an error string */
    VERIFY( strlen(error) != 0 );

    VERIFY( strerror_s( error, errlen - 1, EBADF ) != 0 ); /* truncated error string */
    VERIFY( strcmp( error + errlen - 5, "..." ) == 0 ); /* really truncated? */
    VERIFY( NumViolations == violations );

    //rt constraints
    VERIFY( strerror_s( NULL, errlen - 1, EBADF ) != 0 );
    VERIFY( NumViolations == ++violations );

    VERIFY( strerror_s( error, 0, EBADF ) != 0 );
    VERIFY( NumViolations == ++violations );

#if RSIZE_MAX != SIZE_MAX
    VERIFY( strerror_s( error, ~0, EBADF ) != 0 );
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
