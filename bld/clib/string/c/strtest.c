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


/*
 *  STRTEST.C
 *  Non-exhaustive test of the C library string functions.
 *
 *  12 January 1995
 *  By Matthew Hildebrand
 */

#include <errno.h>
#include <locale.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __SW_BW
    #include <wdefwin.h>
#endif

#define VERIFY( exp )   if( !(exp) ) {                                      \
                            printf( "%s: ***FAILURE*** at line %d of %s.\n",\
                                    ProgramName, __LINE__,                  \
                                    strlwr(__FILE__) );                     \
                            NumErrors++;                                    \
                            exit( -1 );                                     \
                        }

void TestCompare( void );
void TestCase( void );
void TestMove( void );
void TestSearch( void );
void TestSubstring( void );
void TestToken( void );
void TestLocale( void );
void TestError( void );
void TestFormatted( void );
void Test__vbprintf( char *buf, char *format, ... );
int  Test_vsscanf( char *buf, char *format, ... );
void Test_vsprintf( char *buf, char *format, ... );

#if !defined(__AXP__)
void TestCompareF( void );
void TestCaseF( void );
void TestMoveF( void );
void TestSearchF( void );
void TestSubstringF( void );
void TestTokenF( void );
#endif


char ProgramName[128];                          /* executable filename */
int NumErrors = 0;                              /* number of errors */



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
            exit( -1 );
        }
    #endif
    /*** Initialize ***/
    strcpy( ProgramName, strlwr(argv[0]) );     /* store filename */

    /*** Test various functions ***/
    TestCompare();                              /* compare stuff */
    TestMove();                                 /* moving data about */
    TestCase();                                 /* upper/lowercase stuff */
    TestSearch();                               /* searching stuff */
    TestSubstring();                            /* substring stuff */
    TestToken();                                /* tokenizing stuff */
    TestLocale();                               /* locale stuff */
    TestError();                                /* error string stuff */
    TestFormatted();                            /* formatted I/O stuff */
    #if !defined(__AXP__)
        TestCompareF();
        TestMoveF();
        TestCaseF();
        TestSearchF();
        TestSubstringF();
        TestTokenF();
    #endif

    /*** Print a pass/fail message and quit ***/
    if( NumErrors!=0 ) {
        printf( "%s: FAILURE (%d errors).\n", ProgramName, NumErrors );
        return( EXIT_FAILURE );
    }
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



/****
***** Test strcmp(), stricmp(), strcmpi(), strncmp(), and strnicmp().
****/

void TestCompare( void )
{
    char            bufA[80] = "FoO baR gOoBeR bLaH";
    char            bufLower[80] = "foo bar goober blah";
    char            bufUpper[80] = "FOO BAR GOOBER BLAH";
    int             status;

    status = strcmp( bufA, bufA );              /* ensure same */
    VERIFY( status == 0 );

    status = strcmp( bufA, bufLower );          /* ensure not same */
    VERIFY( status != 0 );

    status = stricmp( bufA, bufUpper );         /* ensure same */
    VERIFY( status == 0 );

    status = stricmp( bufA, "foo" );            /* ensure not same */
    VERIFY( status != 0 );

    status = strcmpi( bufA, bufUpper );         /* ensure same */
    VERIFY( status == 0 );

    status = strcmpi( bufA, "foo" );            /* ensure not same */
    VERIFY( status != 0 );

    status = strncmp( bufA, bufA, 100 );        /* ensure same */
    VERIFY( status == 0 );

    status = strncmp( bufA, bufLower, 1 );      /* ensure not same */
    VERIFY( status != 0 );

    status = strnicmp( bufA, bufUpper, 100 );   /* ensure same */
    VERIFY( status == 0 );

    status = strnicmp( bufA, "fOo B!!!", 5 );   /* ensure same */
    VERIFY( status == 0 );

    status = strnicmp( bufA, "fOo B!!!", 6 );   /* ensure not same */
    VERIFY( status != 0 );
}


#if !defined(__AXP__)
void TestCompareF( void )
{
    char            bufA[80] = "FoO baR gOoBeR bLaH";
    char            bufLower[80] = "foo bar goober blah";
    char            bufUpper[80] = "FOO BAR GOOBER BLAH";
    int             status;

    status = _fstrcmp( bufA, bufA );            /* ensure same */
    VERIFY( status == 0 );

    status = _fstrcmp( bufA, bufLower );        /* ensure not same */
    VERIFY( status != 0 );

    status = _fstricmp( bufA, bufUpper );       /* ensure same */
    VERIFY( status == 0 );

    status = _fstricmp( bufA, "foo" );          /* ensure not same */
    VERIFY( status != 0 );

    status = _fstrncmp( bufA, bufA, 100 );      /* ensure same */
    VERIFY( status == 0 );

    status = _fstrncmp( bufA, bufLower, 1 );    /* ensure not same */
    VERIFY( status != 0 );

    status = _fstrnicmp( bufA, bufUpper, 100 ); /* ensure same */
    VERIFY( status == 0 );

    status = _fstrnicmp( bufA, "fOo B!!!", 5 ); /* ensure same */
    VERIFY( status == 0 );

    status = _fstrnicmp( bufA, "fOo B!!!", 6 ); /* ensure not same */
    VERIFY( status != 0 );
}
#endif



/****
***** Test strcpy(), strcat(), strset(), strncpy(), strncat(), strnset(),
***** and strrev().
****/

void TestMove( void )
{
    char            bufA[80] = "FoO baR gOoBeR bLaH";
    char            bufB[80];
    char            *bufPtr;
    char            *newBuf;
    int             status;

    bufPtr = strcpy( bufB, "FoO baR" );         /* copy string */
    VERIFY( bufPtr == bufB );

    bufPtr = strcat( bufB, " gOoBeR bLaH" );    /* append the rest */
    VERIFY( bufPtr == bufB );

    status = strcmp( bufA, bufB );              /* check result */
    VERIFY( status == 0 );

    bufPtr = strset( bufB, 0x00 );              /* zero out buffer */
    VERIFY( bufPtr == bufB );

    bufPtr = strncpy( bufB, "ABCDEFGHIJ", 2 );  /* copy two bytes */
    VERIFY( bufPtr == bufB );

    bufPtr = strncat( bufB, "CDEFGHIJ", 3 );    /* copy three more */
    VERIFY( bufPtr == bufB );

    status = strcmp( bufB, "ABCDE" );           /* ensure only five bytes */
    VERIFY( status == 0 );

    bufPtr = strnset( bufB, 0x00, 10 );         /* blank string */
    VERIFY( bufPtr == bufB );

    status = strcmp( bufB, "" );                /* verify empty */
    VERIFY( status == 0 );

    bufPtr = strcpy( bufB, "abcdefghij" );      /* copy string */
    VERIFY( bufPtr == bufB );

    bufPtr = strrev( bufB );                    /* reverse it */
    VERIFY( bufPtr == bufB );

    status = strcmp( bufB, "jihgfedcba" );      /* ensure reversed ok */
    VERIFY( status == 0 );

    newBuf = strdup( bufA );                    /* duplicate string */
    status = strcmp( bufA, newBuf );
    VERIFY( status == 0 );
}


#if !defined(__AXP__)
void TestMoveF( void )
{
    char            bufA[80] = "FoO baR gOoBeR bLaH";
    char            bufB[80];
    char __far      *bufPtr;
    char __far      *newBuf;
    int             status;

    bufPtr = _fstrcpy( bufB, "FoO baR" );       /* copy string */
    VERIFY( bufPtr == bufB );

    bufPtr = _fstrcat( bufB, " gOoBeR bLaH" );  /* append the rest */
    VERIFY( bufPtr == bufB );

    status = _fstrcmp( bufA, bufB );            /* check result */
    VERIFY( status == 0 );

    bufPtr = _fstrset( bufB, 0x00 );            /* zero out buffer */
    VERIFY( bufPtr == bufB );

    bufPtr = _fstrncpy( bufB, "ABCDEFGHIJ", 2 );/* copy two bytes */
    VERIFY( bufPtr == bufB );

    bufPtr = _fstrncat( bufB, "CDEFGHIJ", 3 );  /* copy three more */
    VERIFY( bufPtr == bufB );

    status = _fstrcmp( bufB, "ABCDE" );         /* ensure only five bytes */
    VERIFY( status == 0 );

    bufPtr = _fstrnset( bufB, 0x00, 10 );       /* blank string */
    VERIFY( bufPtr == bufB );

    status = _fstrcmp( bufB, "" );              /* verify empty */
    VERIFY( status == 0 );

    bufPtr = _fstrcpy( bufB, "abcdefghij" );    /* copy string */
    VERIFY( bufPtr == bufB );

    bufPtr = _fstrrev( bufB );                  /* reverse it */
    VERIFY( bufPtr == bufB );

    status = _fstrcmp( bufB, "jihgfedcba" );    /* ensure reversed ok */
    VERIFY( status == 0 );

    newBuf = _fstrdup( bufA );                  /* duplicate string */
    status = _fstrcmp( bufA, newBuf );
    VERIFY( status == 0 );
}
#endif



/****
***** Test strlwr() and strupr().
****/

void TestCase( void )
{
    char            bufA[80] = "FoO baR gOoBeR bLaH";
    char            bufB[80];
    char            bufLower[80] = "foo bar goober blah";
    char            bufUpper[80] = "FOO BAR GOOBER BLAH";
    char            *bufPtr;
    int             status;

    bufPtr = strcpy( bufB, bufA );              /* copy string */
    VERIFY( bufPtr == bufB );

    bufPtr = strlwr( bufB );                    /* lowercase */
    VERIFY( bufPtr == bufB );

    status = strcmp( bufB, bufLower );          /* ensure ok */
    VERIFY( status == 0 );

    bufPtr = strcpy( bufB, bufA );              /* copy string */
    VERIFY( bufPtr == bufB );

    bufPtr = strupr( bufB );                    /* uppercase */
    VERIFY( bufPtr == bufB );

    status = strcmp( bufB, bufUpper );          /* ensure ok */
    VERIFY( status == 0 );
}


#if !defined(__AXP__)
void TestCaseF( void )
{
    char            bufA[80] = "FoO baR gOoBeR bLaH";
    char            bufB[80];
    char            bufLower[80] = "foo bar goober blah";
    char            bufUpper[80] = "FOO BAR GOOBER BLAH";
    char __far      *bufPtr;
    int             status;

    bufPtr = _fstrcpy( bufB, bufA );            /* copy string */
    VERIFY( bufPtr == bufB );

    bufPtr = _fstrlwr( bufB );                  /* lowercase */
    VERIFY( bufPtr == bufB );

    status = _fstrcmp( bufB, bufLower );        /* ensure ok */
    VERIFY( status == 0 );

    bufPtr = _fstrcpy( bufB, bufA );            /* copy string */
    VERIFY( bufPtr == bufB );

    bufPtr = _fstrupr( bufB );                  /* uppercase */
    VERIFY( bufPtr == bufB );

    status = _fstrcmp( bufB, bufUpper );        /* ensure ok */
    VERIFY( status == 0 );
}
#endif



/****
***** Test strchr(), strrchr(), strlen()
****/

void TestSearch( void )
{
    char            buf[] = "The quick brown fox jumped over the lazy dogs.";
    char            *ptr;

    ptr = strchr( buf, '!' );                   /* try to find a '!' */
    VERIFY( ptr == NULL );

    ptr = strchr( buf, 0x00 );                  /* try to find the NULL */
    VERIFY( ptr == buf+strlen(buf) );

    ptr = strchr( buf, 'k' );                   /* try to find a 'k' */
    VERIFY( ptr == buf+8 );

    ptr = strrchr( buf, '!' );                  /* try to find a '!' */
    VERIFY( ptr == NULL );

    ptr = strrchr( buf, 0x00 );                 /* try to find the NULL */
    VERIFY( ptr == buf+strlen(buf) );

    ptr = strrchr( buf, 'k' );                  /* try to find a 'k' */
    VERIFY( ptr == buf+8 );
}


#if !defined(__AXP__)
void TestSearchF( void )
{
    char            buf[] = "The quick brown fox jumped over the lazy dogs.";
    char __far      *ptr;

    ptr = _fstrchr( buf, '!' );                 /* try to find a '!' */
    VERIFY( ptr == NULL );

    ptr = _fstrchr( buf, 0x00 );                /* try to find the NULL */
    VERIFY( ptr == buf+strlen(buf) );

    ptr = _fstrchr( buf, 'k' );                 /* try to find a 'k' */
    VERIFY( ptr == buf+8 );

    ptr = _fstrrchr( buf, '!' );                /* try to find a '!' */
    VERIFY( ptr == NULL );

    ptr = _fstrrchr( buf, 0x00 );               /* try to find the NULL */
    VERIFY( ptr == buf+strlen(buf) );

    ptr = _fstrrchr( buf, 'k' );                /* try to find a 'k' */
    VERIFY( ptr == buf+8 );
}
#endif



/****
***** Test strcspn(), strpbrk(), strspn(), and strstr().
****/

void TestSubstring( void )
{
    char            buf[] = "The quick brown fox jumped over the lazy dogs.";
    int             status;
    char            *ptr;

    status = strcspn( "abcdefghij", "bca" );    /* should be zero chars */
    VERIFY( status == 0 );

    status = strcspn( "123456", "cab" );        /* should be whole string */
    VERIFY( status == 6 );

    status = strcspn( "!x,y:zfoo", "t.7of" );   /* should be 6 chars */
    VERIFY( status == 6 );

    ptr = strpbrk( "aBcDeFgHiJ", "1234567" );   /* should be NULL */
    VERIFY( ptr == NULL );

    ptr = strpbrk( "Foo", "" );                 /* should be NULL */
    VERIFY( ptr == NULL );

    ptr = strpbrk( buf, "t!q-");                /* find the 'q' */
    VERIFY( ptr == buf+4 );

    status = strspn( buf, "!1@2#3$4%5^6&7" );   /* should be zero */
    VERIFY( status == 0 );

    status = strspn( buf, "kciuq ehT" );        /* should be 10 */
    VERIFY( status == 10);

    ptr = strstr( buf, "foo" );                 /* should be NULL */
    VERIFY( ptr == NULL );

    ptr = strstr( buf, "ck br" );               /* find "ck br" */
    VERIFY( ptr == buf+7 );
}


#if !defined(__AXP__)
void TestSubstringF( void )
{
    char            buf[] = "The quick brown fox jumped over the lazy dogs.";
    int             status;
    char __far      *ptr;

    status = _fstrcspn( "abcdefghij", "bca" );  /* should be zero chars */
    VERIFY( status == 0 );

    status = _fstrcspn( "123456", "cab" );      /* should be whole string */
    VERIFY( status == 6 );

    status = _fstrcspn( "!x,y:zfoo", "t.7of" ); /* should be 6 chars */
    VERIFY( status == 6 );

    ptr = _fstrpbrk( "aBcDeFgHiJ", "1234567" ); /* should be NULL */
    VERIFY( ptr == NULL );

    ptr = _fstrpbrk( "Foo", "" );               /* should be NULL */
    VERIFY( ptr == NULL );

    ptr = _fstrpbrk( buf, "t!q-");              /* find the 'q' */
    VERIFY( ptr == buf+4 );

    status = _fstrspn( buf, "!1@2#3$4%5^6&7" ); /* should be zero */
    VERIFY( status == 0 );

    status = _fstrspn( buf, "kciuq ehT" );      /* should be 10 */
    VERIFY( status == 10);

    ptr = _fstrstr( buf, "foo" );               /* should be NULL */
    VERIFY( ptr == NULL );

    ptr = _fstrstr( buf, "ck br" );             /* find "ck br" */
    VERIFY( ptr == buf+7 );
}
#endif



/****
***** Test strtok().
****/

void TestToken( void )
{
    char            buf[] = "Find!all;the.tokens,";
    char            *ptr;

    ptr = strtok( buf, " ;.,!" );               /* find a token */
    VERIFY( !strcmp(ptr,"Find") );

    ptr = strtok( NULL, " ;.,!" );              /* find a token */
    VERIFY( !strcmp(ptr,"all") );

    ptr = strtok( NULL, " ;.,!" );              /* find a token */
    VERIFY( !strcmp(ptr,"the") );

    ptr = strtok( NULL, " ;.,!" );              /* find a token */
    VERIFY( !strcmp(ptr,"tokens") );

    ptr = strtok( NULL, " '.,!" );              /* try to find another */
    VERIFY( ptr == NULL );
}


#if !defined(__AXP__)
void TestTokenF( void )
{
    char            buf[] = "Find!all;the.tokens,";
    char __far      *ptr;

    ptr = _fstrtok( buf, " ;.,!" );             /* find a token */
    VERIFY( !_fstrcmp(ptr,"Find") );

    ptr = _fstrtok( NULL, " ;.,!" );            /* find a token */
    VERIFY( !_fstrcmp(ptr,"all") );

    ptr = _fstrtok( NULL, " ;.,!" );            /* find a token */
    VERIFY( !_fstrcmp(ptr,"the") );

    ptr = _fstrtok( NULL, " ;.,!" );            /* find a token */
    VERIFY( !_fstrcmp(ptr,"tokens") );

    ptr = _fstrtok( NULL, " '.,!" );            /* try to find another */
    VERIFY( ptr == NULL );
}
#endif



/****
***** Test strcoll() and strxfrm().
****/

void TestLocale( void )
{
    char            bufA[80] = "FoO baR gOoBeR bLaH";
    char            bufB[80];
    int             status;

    setlocale( LC_ALL, "C" );                   /* set locale to "C" */

    status = strxfrm( bufB, bufA, 20 );         /* transform the string */
    VERIFY( status == 19 );

    status = strcoll( bufB, bufA );             /* compare with "C" locale */
    VERIFY( status == 0 );

    status = strcoll( bufB, "Bar" );            /* compare with "C" locale */
    VERIFY( status != 0 );
}



/****
***** Test strerror().
****/

void TestError( void )
{
    char            *error = NULL;

    error = strerror( EBADF );                  /* get an error string */
    VERIFY( strlen(error) != 0 );
}



/****
***** Test _bprintf(), sprintf(), sscanf(), _vbprintf(), vsprintf(), and
***** vsscanf().
****/

void TestFormatted( void )
{
    char            buf[80];
    int             status;
    int             numA, numB;

    status = _bprintf( buf, 3, "%d", 12345 );   /* try to print too much */
    VERIFY( status <= 3 );

    status = _bprintf( buf, 80, "%d", 12345 );  /* print a string */
    VERIFY( status == 5 );
    VERIFY( !strcmp(buf,"12345") );

    status = sprintf( buf, " %d %d", 191, 67 ); /* print a string */
    VERIFY( status == 7 );

    status = sscanf( buf, "%d %d", &numA, &numB );  /* parse string */
    VERIFY( status == 2 );
    VERIFY( numA == 191 );
    VERIFY( numB == 67 );

    Test__vbprintf( buf, "%d %d", 101, 37 );
    status = Test_vsscanf( buf, "%d %d", &numA, &numB );
    VERIFY( status == 2 );
    VERIFY( numA == 101 );
    VERIFY( numB == 37 );

    Test_vsprintf( buf, "%d %d", 101, 37 );
    status = Test_vsscanf( buf, "%d %d", &numA, &numB );
    VERIFY( status == 2 );
    VERIFY( numA == 101 );
    VERIFY( numB == 37 );
}


void Test__vbprintf( char *buf, char *format, ... )
{
    va_list         args;
    int             status;

    va_start( args, format );
    status = _vbprintf( buf, 80, format, args );    /* print some stuff */
    VERIFY( status <= 80 );
    va_end( args );
}


int Test_vsscanf( char *buf, char *format, ... )
{
    va_list         args;
    int             status;

    va_start( args, format );
    status = vsscanf( buf, format, args );
    va_end( args );
    return( status );
}


void Test_vsprintf( char *buf, char *format, ... )
{
    va_list         args;
    int             status;

    va_start( args, format );
    status = vsprintf( buf, format, args );     /* print some stuff */
    VERIFY( status > 0 );
    va_end( args );
}
