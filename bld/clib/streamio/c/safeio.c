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
* Description:  Non-exhaustive test of Safer C library stream I/O functions.
*
****************************************************************************/


#define __STDC_WANT_LIB_EXT1__  1
#include <stdio.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <malloc.h>
#include <unistd.h>
#ifdef __SW_BW
    #include <wdefwin.h>
#endif
#include "rterrno.h"
#include "thread.h"

#ifdef __UNIX__
    #define CONSOLE_OUT "/dev/tty"
    #define CONSOLE_IN  "/dev/tty"
#elif defined(__NT__) && !defined(__SW_BW)
    #define CONSOLE_OUT "CONOUT$"
    #define CONSOLE_IN  "CONIN$"
#else
    #define CONSOLE_OUT "CON"
    #define CONSOLE_IN  "CON"
#endif

#define MAX_MODE 4           /* Max length of mode parameter */
#define NUM_OMODES 6         /* Number of (r, w, a) style modes */
                             /*  ur, uw, ua  Safer C Library */
#define NUM_FTYPES 3         /* Number of file types */
#define MAX_FILE_SIZE 50     /* Maximum number of bytes in a test file */
#define NUM_FILES 10         /* Maximum number of files open at one time */

/* Test macros */

#define VERIFY( expr ) ++tests; if( !(expr) ) {             \
              fprintf( con, "***FAILURE*** Condition failed in (%s)\n", cur_mode );  \
              fprintf( con, "   %s, line %u.\n", #expr, __LINE__ ); \
              fprintf( con, "   strerror(errno): %s\n\n", strerror( errno ) ); \
              ++failures;                                   \
              exit( -1 );                                     \
        }

#define EXPECT( expr ) ++tests;    if( !(expr) ) {                \
              fprintf( con, "***WARNING*** Condition failed in (%s)\n", cur_mode );  \
              fprintf( con, "   %s, line %u.\n", #expr, __LINE__ ); \
              fprintf( con, "   strerror(errno): %s\n\n", strerror( errno ) ); \
              ++warnings;                                \
              exit( -1 );                                     \
            }

#define INTERNAL( expr ) if( !(expr) ) {                    \
              fprintf( con, "*** INTERNAL FAILURE ***" );         \
              fprintf( con, " %s, line %u.\n", #expr, __LINE__ ); \
              exit( -1 );                                     \
            }

#define VERIFYS( exp )   if( !(exp) ) {                                     \
                            printf( "%s: ***FAILURE*** at line %d of %s.\n",\
                                    ProgramName, __LINE__,                  \
                                    strlwr(__FILE__) );                     \
                            NumErrors++;                                    \
                            exit( -1 );                                     \
                        }

typedef struct testfile {
    FILE    *fp;
    char    filename[ L_tmpnam ];
} TestFile;

unsigned long int   tests = 0;         /* total number of tests */
unsigned long int   warnings = 0;      /* warnings to not skip further tests */
unsigned long int   failures = 0;      /* failures skip the test section */
FILE                *con;              /* console device */


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


/***********************************/
/* Generic TestFile based routines */
/* *********************************/

TestFile *TestFile_Get( const char *cur_mode )
/********************************************/
{
    TestFile    *cur_test;
    errno_t     rc;
    int         pos = 0;

    cur_test = malloc( sizeof( TestFile ) );
    INTERNAL( cur_test!=NULL );

//    EXPECT( tmpnam( cur_test->filename ) != NULL );
    rc = tmpnam_s( cur_test->filename, sizeof(cur_test->filename ) );
    VERIFYS( rc == 0 );
    VERIFYS( cur_test->filename[0] != '\0' );
    rc = fopen_s( &(cur_test->fp), cur_test->filename, cur_mode );

    if( cur_mode[0] == 'u' ) pos++; /* ignore u */
    if( cur_mode[pos] == 'r' ) {
        VERIFY( rc != 0 );
        EXPECT( cur_test->fp == NULL );
    } else {
        VERIFY( rc == 0 );
        VERIFY( cur_test->fp != NULL );
        EXPECT( !ferror( cur_test->fp ) );
    }

    return( cur_test );
}

int TestFile_ReOpen( TestFile *cur_test, const char *cur_mode )
/*************************************************************/
{
    errno_t     rc;

    if( cur_test->fp != NULL ) {
        fclose( cur_test->fp );
        VERIFY( !ferror( cur_test->fp ) );
        rc = fopen_s( &(cur_test->fp), cur_test->filename, cur_mode );
        VERIFY( rc == 0 );
        VERIFY( cur_test->fp != NULL );
        VERIFY( !ferror( cur_test->fp ) );
    } else {
        return( 0 );
    }

    return( 1 );
}

int TestFile_Destroy( TestFile *cur_test )
/****************************************/
{
    char    cur_mode[6] = "";

    /* Remove the test file */
    if( cur_test->fp != NULL ) {
        fclose( cur_test->fp );
        VERIFY( !ferror( cur_test->fp ) );
        EXPECT( remove( cur_test->filename ) == 0 );
    }

    /* Free the memory */

    free( cur_test );

    return( 1 );
}

void Status_Print( void )
/***********************/
{
    /* Print the number of warnings/failures. */
    fprintf( con, "   Tests: %d\n", tests );
    fprintf( con, "Warnings: %d\n", warnings );
    fprintf( con, "Failures: %d\n", failures );
}

void Mode_Get_s( char *cur_mode, int cur_omode, int cur_update, int cur_ftype )
/*****************************************************************************/
{
    int     pos = 0;    /* Current string position */

    /* new modes with u as first char                           */
    if( cur_omode > 2) {
        cur_mode[pos] = 'u';
        pos++;
        cur_omode -= 3 ;
    }
    /* Convert the above integers to the coresponding file mode */
    switch( cur_omode ) {
        case 0: cur_mode[pos] = 'r'; break;
        case 1: cur_mode[pos] = 'w'; break;
        case 2: cur_mode[pos] = 'a'; break;
        default: INTERNAL( cur_omode < 3 ); break;
    };

    pos++;
    switch( cur_update ) {
        case 0:  break;
        default: cur_mode[pos] = '+'; pos++; break;
    };

    switch( cur_ftype ) {
        case 0: break;
        case 1: cur_mode[pos] = 'b'; pos++; break;
        case 2: cur_mode[pos] = 't'; pos++; break;
        default: INTERNAL( cur_ftype < 3 ); break;
    }

    cur_mode[pos] = '\0';
}

TestFile *Test_File_Create( char *cur_mode, char *cur_string, char test1 )
/************************************************************************/
{
    int         i;
    TestFile    *cur_test;
    int         pos = 0;

    /* Get a test string to insert into the file */

    for( i = 0; i < MAX_FILE_SIZE; i++ ) {
        cur_string[i] = 'k';
    }
    cur_string[i] = '\0';

    /* Create a test file */
    if( cur_mode[0] == 'u' ) pos++;

    if( cur_mode[pos] == 'r' ) {
        cur_mode[pos] = 'w';
        cur_test = TestFile_Get( cur_mode );
        cur_mode[pos] = 'r';
    } else {
        cur_test = TestFile_Get( cur_mode );
    }
    VERIFY( fputc( test1, cur_test->fp ) == test1 );
    VERIFY( fputs( cur_string, cur_test->fp ) > 0 );

    return( cur_test );
}

/**********************/
/* Main test routines */
/**********************/


int Test_vfprintf( FILE *fp, char *format, ... )
/**********************************************/
{
    va_list     arglist;
    char        cur_mode[10] = "vfprintf";

    va_start( arglist, format );
    VERIFY( vfprintf( fp, format, arglist ) > 0 );
    va_end( arglist );

    return( 1 );
}

int Test_vfscanf( FILE *fp, char *format, ... )
/*********************************************/
{
    va_list     arglist;
    char        cur_mode[10] = "vfscanf";

    va_start( arglist, format );
    VERIFY( vfscanf( fp, format, arglist ) > 0 );
    va_end( arglist );

    return( 1 );
}

int Test_vprintf( char *format, ... )
/***********************************/
{
    va_list     arglist;
    char        cur_mode[10] = "vfprintf";

    va_start( arglist, format );
    VERIFY( vprintf( format, arglist ) > 0 );
    va_end( arglist );

    return( 1 );
}

int Test_vscanf( char *format, ... )
/**********************************/
{
    va_list     arglist;
    char        cur_mode[10] = "vfscanf";

    va_start( arglist, format );
    VERIFY( vscanf( format, arglist ) > 0 );
    va_end( arglist );

    return( 1 );
}

int Test_File_FWriteRead( TestFile *cur_test, char *cur_mode )
/************************************************************/
{
    char        buf1[30], buf2[20];
    int         temp_int;
    int         pos = 0;

    /* fread, fwrite, fprintf, fscanf test */
    strcpy( buf1, "Hello, World!" );
    VERIFY( fwrite( buf1, 1, 13, cur_test->fp ) == 13 );
    VERIFY( fprintf( cur_test->fp, "%d\n", 31415 ) > 0 );

    Test_vfprintf( cur_test->fp, "%d\n", 34 );
    TestFile_ReOpen( cur_test, cur_mode );

    if( cur_mode[0] == 'u' ) pos++; /* ignore u */
    if( cur_mode[pos] == 'r' ) {
        VERIFY( fseek( cur_test->fp, 0, SEEK_SET ) == 0);
        VERIFY( ftell(cur_test->fp) == 0 );
        VERIFY( fread( buf2, 1, 13, cur_test->fp ) == 13 );
        buf2[13] = '\0';
        VERIFY( strcmp( buf1, buf2 ) == 0 );
        VERIFY( fscanf( cur_test->fp, "%d", &temp_int ) == 1 );
        EXPECT( temp_int == 31415 );
        Test_vfscanf( cur_test->fp, "%d", &temp_int );
        EXPECT( temp_int == 34 );
    }

    return( 1 );
}

int Test_File_IO_More( char *cur_mode )
/*************************************/
{
    TestFile    *cur_test;
    int         pos = 0;

    /* Create a file (testing some write functinos) */
    if( cur_mode[0] == 'u' ) pos++; /* ignore u */
    if( cur_mode[pos] == 'r' ) {
        cur_mode[pos] = 'w';
        cur_test = TestFile_Get( cur_mode );
        cur_mode[pos] = 'r';
    } else {
        cur_test = TestFile_Get( cur_mode );
    }

    if( cur_test != 0 ) {
        Test_File_FWriteRead( cur_test, cur_mode );
    }

    /* Free memory; close and remove test file */
    TestFile_Destroy( cur_test );

    return( 1 );
}

int Test_Temp_IO( void )
/**********************/
{
    int     i, num_closed;
    FILE    *cur_file[ NUM_FILES ];
    char    cur_mode[4] = "tmp";     /* for the VERIFY/EXPECT macros */

    for( i = 0; i < NUM_FILES; i++ ) {
        cur_file[i] = tmpfile();
        VERIFY( cur_file[i] != NULL );
        VERIFY( fputc( 'x', cur_file[i] ) != EOF );
    }

#ifndef __UNIX__    // _TMPFIL flag isn't used on UNIX
    for( i = 0; i < NUM_FILES; i++ ) {
        EXPECT( ( cur_file[i]->_flag & _TMPFIL ) != 0 );
    }
#endif

    fcloseall();
    num_closed = fcloseall();

    /* re-open the console and the windows tmp.log file */
#ifdef __SW_BW
    con = fopen( "tmp.log", "a" );
#else
    con = fopen( CONSOLE_OUT, "w" );
#endif
    VERIFY( con != NULL );

    EXPECT( num_closed == 0 );

    return( 1 );
}

int Test_Safeio( FILE *fp, FILE **my_fp, const char *filename )
/***************************************************************/
{

/***************************************************************************/
/*  slightly modified Test_StdWrite() to use gets_s() instead of gets()    */
/*  therefore some tests are just duplicates                               */
/***************************************************************************/

    char    cur_mode[10];

    char    *p;
    char    rbuff[10];
    size_t  maxsize = sizeof( rbuff );
    int     violations = NumViolations;

    if( fp == stdout ) {
        strcpy( cur_mode, "stdout" );
    } else if( fp == stderr ) {
        strcpy( cur_mode, "stderr" );
    }


    /***********************************************************************/
    /*  write testdata for gets_s                                          */
    /***********************************************************************/

    VERIFY( fprintf( *my_fp, "%s\n", "LENGTH08" ) > 0 );
    VERIFY( fprintf( *my_fp, "%s\n\n", "LENGTH16--------" ) > 0 );
    VERIFY( fprintf( *my_fp, "%s\n", "LENGTH009" ) > 0 );

    fflush( *my_fp );

    EXPECT( freopen( CONSOLE_IN, "r+t", *my_fp ) != NULL );
    VERIFY( (*my_fp = freopen( filename, "rt", stdin )) != NULL );

    fseek( *my_fp, 0, SEEK_SET );

    /***********************************************************************/
    /*  now test get_s                                                     */
    /***********************************************************************/

    p = gets_s( rbuff, maxsize );       /* normal input */
    VERIFYS( p != NULL );
    VERIFYS( strcmp( p ,"LENGTH08" ) == 0 );
    VERIFYS( NumViolations == violations );

    p = gets_s( rbuff, maxsize );       /* no nl within maxsize chars*/
    VERIFYS( p == NULL );
    VERIFYS( rbuff[ 0 ] == '\0' );
    VERIFYS( NumViolations == ++violations );

    p = gets_s( rbuff, maxsize );       /* empty line */
    VERIFYS( p != NULL );
    VERIFYS( *p == '\0' );
    VERIFYS( NumViolations == violations );

    p = gets_s( rbuff, maxsize );       /* normal input */
    VERIFYS( p != NULL );
    VERIFYS( strcmp( p ,"LENGTH009" ) == 0 );
    VERIFYS( NumViolations == violations );

    /* Test runtime-constraints for gets_s */
    p = gets_s( NULL, maxsize );
    VERIFYS( p == NULL );
    VERIFYS( NumViolations == ++violations );

    p = gets_s( rbuff, 0 );
    VERIFYS( p == NULL );
    VERIFYS( NumViolations == ++violations );

#if RSIZE_MAX != SIZE_MAX
    p = gets_s( rbuff, ~0 );
    VERIFYS( p == NULL );
    VERIFYS( NumViolations == ++violations );
#endif

    return( 1 );
}

int Test_Safeios( FILE *fp )
/****************************/
{
    char    cur_mode[10] = "safeio";
    char    filename[ L_tmpnam ];
    FILE    *my_fp;

    EXPECT( tmpnam( filename ) != NULL );
    EXPECT( (my_fp = freopen( filename, "a+t", fp )) != NULL );

    /* A separate function is used to assure the deletion of the test file. */

    Test_Safeio( fp, &my_fp, filename ); /* for gets_s() */

    EXPECT( (fp = freopen( CONSOLE_IN, "r+t", my_fp )) != NULL );
    EXPECT( remove( filename ) == 0 );

    return( 1 );
}

int Test_File_Errors( void )
/**************************/
{
    char    cur_mode[10] = "error";
    char    filename[ L_tmpnam ], filename2[ L_tmpnam ];
    FILE    *fp, *fp2;

    /* get a temporary file */
    EXPECT( tmpnam( filename ) != NULL );
    EXPECT( ( fp = fopen( filename, "wt" ) ) != NULL );

    /* test the error routines */
    EXPECT( fgetc( fp ) == EOF );
    EXPECT( fgetc( fp ) == EOF );
    EXPECT( ferror( fp ) != 0 );

    /* test perror */
    EXPECT( tmpnam( filename2 ) != NULL );
    EXPECT( ( fp2 = freopen( filename2, "a+t", stderr ) ) != NULL );
    perror( "" );
    fflush( stderr );
    EXPECT( fgetc( fp2 ) == EOF );
    EXPECT( fclose( fp2 ) == 0 );
    EXPECT( remove( filename2 ) == 0 );

    /* test clearerr() */
    clearerr( fp );
    EXPECT( ferror( fp ) == 0 );

    /* cleanup */
    EXPECT( fclose( fp ) == 0 );
    EXPECT( remove( filename ) == 0 );

    return( 1 );
}

int Test_Flushes( void )
/**********************/
{
    char    cur_mode[10] = "flushes";
    FILE    *fpr, *fpw;
    char    filename[ L_tmpnam ];

    VERIFY( tmpnam( filename ) != NULL );
    EXPECT( (fpw = fopen( filename, "w" )) != NULL );
    EXPECT( (fpr = fopen( filename, "r" )) != NULL );

    /* Test fflush() */

    EXPECT( fputc( 'z', fpw ) );
    EXPECT( fgetc( fpr ) == EOF );
    EXPECT( fflush( fpw ) == 0 );
    EXPECT( fgetc( fpr ) != EOF );

    /* Test flushall() */

    EXPECT( fputc( 'z', fpw ) );
    EXPECT( fgetc( fpr ) == EOF );
    EXPECT( flushall() );
    EXPECT( fgetc( fpr ) != EOF );

    EXPECT( fclose( fpr ) == 0 );
    EXPECT( fclose( fpw ) == 0 );
    EXPECT( remove( filename ) == 0 );

    return( 1 );
}

int Test_fdopen( void )
/*********************/
{
    int     handle;
    FILE    *fp;
    char    filename[ L_tmpnam ];
    char    cur_mode[10] = "fdopen";

    VERIFY( tmpnam( filename ) != NULL );

    VERIFY( (fp = fopen( filename, "w+t" )) != NULL );
    EXPECT( fputc( 'y', fp ) != EOF );
    EXPECT( fclose( fp ) == 0 );

    handle = open( filename, O_RDONLY | O_TEXT );
    VERIFY( handle != -1 );

    EXPECT( (fp = fdopen( handle, "r" )) != NULL );
    if( fp != NULL ) {
        EXPECT( fgetc( fp ) == 'y' );
        fclose( fp );
    } else {
        close( handle );
    }
    EXPECT( remove( filename ) == 0 );

    return( 1 );
}

int Test_setbuf( void )
/*********************/
{
    FILE    *fp;
    char    buffer[ BUFSIZ ];
    char    filename[ L_tmpnam ];
    char    cur_mode[10] = "setbuf";

    VERIFY( tmpnam( filename ) != NULL );
    VERIFY( (fp = fopen( filename, "w" )) != NULL );
    setbuf( fp, buffer );

    fputc( 'a', fp );
    fputc( 'z', fp );
    EXPECT( buffer[0] == 'a' );
    EXPECT( buffer[1] == 'z' );

    fclose( fp );
    EXPECT( remove( filename ) == 0 );

    return( 1 );
}

int Test_setvbuf( void )
/**********************/
{
    FILE    *fp;
    char    buff1[ BUFSIZ ];
    char    filename[ L_tmpnam ];
    char    cur_mode[10] = "setvbuf";

    /* open the tmpfile */
    VERIFY( tmpnam( filename ) != NULL );
    VERIFY( (fp = fopen( filename, "w" )) != NULL );

    /* setvbuf is tested in only one mode */
    setvbuf( fp, buff1, _IOFBF, BUFSIZ );

    fclose( fp );
    EXPECT( remove( filename ) == 0 );

    return( 1 );
}

int Test_ungetc( void )
/*********************/
{
    FILE    *fp;
    char    filename[ L_tmpnam ];
    char    cur_mode[10] = "ungetc";

    /* open the tmpfile */
    VERIFY( tmpnam( filename ) != NULL );
    VERIFY( (fp = fopen( filename, "w+t" )) != NULL );

    /* Test of ungetc() */
    VERIFY( 'z' == ungetc( 'z', fp ) );
    VERIFY( 'z' == getc( fp ) );

    fclose( fp );
    EXPECT( remove( filename ) == 0 );

    return( 1 );
}

void Test_rt_constraints_fopen( void )
/************************************/
{
    errno_t rc;
    FILE    * streamptr;
    FILE    * newstreamptr;
    int     violations = NumViolations;
    char    filename[] = "TESTFILE";

    rc = fopen_s( NULL, filename, "r" );
    VERIFYS( NumViolations == ++violations );
    VERIFYS( rc != 0 );

    rc = fopen_s( &streamptr, NULL, "r" );
    VERIFYS( NumViolations == ++violations );
    VERIFYS( rc != 0 );
    VERIFYS( streamptr == NULL );

    rc = fopen_s( &streamptr, filename, NULL );
    VERIFYS( NumViolations == ++violations );
    VERIFYS( rc != 0 );
    VERIFYS( streamptr == NULL );

    rc = fopen_s( &streamptr, filename, "r" );
    VERIFYS( NumViolations == violations );   /* no rt constraint */
    VERIFYS( rc != 0 );
    VERIFYS( streamptr == NULL );

/***********************************************************/
    /* open testfile */
    rc = fopen_s( &streamptr, filename, "w" );
    VERIFYS( NumViolations == violations );
    VERIFYS( rc == 0 );

    /* reopen testfile */
    rc = freopen_s( &newstreamptr, filename, "a", streamptr );
    VERIFYS( NumViolations == violations );
    VERIFYS( rc == 0 );
    VERIFYS( newstreamptr != NULL );

    /* reopen without filename allowed by os/2 vac3.xx */
    /* disliked by NT VC++                             */
    rc = freopen_s( &streamptr, "", "w", newstreamptr );
    VERIFYS( NumViolations == violations );
    VERIFYS( rc != 0 );
    VERIFYS( streamptr == NULL );

    /* open testfile */
    rc = fopen_s( &streamptr, filename, "w" );
    VERIFYS( NumViolations == violations );
    VERIFYS( rc == 0 );

    /* newstreamptr NULL */
    rc = freopen_s( NULL, filename, "a", streamptr );
    VERIFYS( NumViolations == ++violations );
    VERIFYS( rc != 0 );
    VERIFYS( streamptr != NULL );

    /* mode NULL */
    rc = freopen_s( &newstreamptr, filename, NULL, streamptr );
    VERIFYS( NumViolations == ++violations );
    VERIFYS( rc != 0 );
    VERIFYS( newstreamptr == NULL );
    VERIFYS( streamptr != NULL );

    /* old stream NULL */
    rc = freopen_s( &newstreamptr, filename, "a", NULL );
    VERIFYS( NumViolations == ++violations );
    VERIFYS( rc != 0 );
    VERIFYS( newstreamptr == NULL );
    fclose( streamptr );
    unlink( filename );


    return;
}

int main( int argc, char *argv[] )
/********************************/
{
    int         cur_omode;              /* current open mode */
    int         cur_update;             /* current update mode */
    int         cur_ftype;              /* current file type */
    char        cur_mode[MAX_MODE + 1]; /* actual file mode paramater */
    TestFile    *cur_test;
    int         old_stdout_fd;
    FILE        *old_stdout;

    /*** Initialize ***/
    strcpy( ProgramName, strlwr( argv[0] ) );   /* store filename */


#ifdef __SW_BW
    con = fopen( "tmp.log", "a" );
#else
    con = fopen( CONSOLE_OUT, "w" );
#endif
    VERIFY( con != NULL );

    /***********************************************************************/
    /*  set constraint-handler                                             */
    /***********************************************************************/

    set_constraint_handler_s( my_constraint_handler );

    /***********************************************************************/
    /*  Test runtime-constraints  fopen_s freopen_s                        */
    /***********************************************************************/

    Test_rt_constraints_fopen();


    /******************/
    /* Start of tests */
    /******************/

    /* The following tests are run for each file mode type */

    for( cur_omode = 0; cur_omode < NUM_OMODES; cur_omode++ ) {
        for( cur_update = 0; cur_update < 2; cur_update++ ) {
            for( cur_ftype = 0; cur_ftype < NUM_FTYPES; cur_ftype++ ) {
                /* Get the mode string */
                Mode_Get_s( cur_mode, cur_omode, cur_update, cur_ftype);

                /* Test fopen, fclose, and ferror */
                cur_test = TestFile_Get( cur_mode );
                  TestFile_Destroy( cur_test );

                /* Test standard file input and output functions */
//              Test_File_IO( cur_mode );

                /* Test more of the standard functions */
                Test_File_IO_More( cur_mode );

            } /* cur_ftype */
        } /* cur_update */
    } /* cur_omode */

    Test_Temp_IO( );   /* tmpfile based tests */

    /* Create a clone of stdout for later use; must be done after
     * Test_Temp_IO() which calls fcloseall(). This needs to be done
     * for output redirection to work.
     */
    old_stdout_fd = fileno( stdout );
    EXPECT( (old_stdout_fd = dup( old_stdout_fd )) != -1 );
    EXPECT( (old_stdout = fdopen( old_stdout_fd, "wt" )) != NULL );

    Test_Safeios( stdout );


    fprintf( old_stdout, "Tests completed (%s).\n", strlwr( argv[0] ) );
    fclose( old_stdout );
    fclose( con );
    //Status_Print( );
#ifdef __SW_BW
    fprintf( stderr, "Tests completed (%s).\n", strlwr( argv[0] ) );
    _dwShutDown();
#endif

    /****************/
    /* End of tests */
    /****************/

    return( 0 );
}
