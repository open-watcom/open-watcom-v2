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
* Description:  Non-exhaustive test of C library stream I/O functions.
*
****************************************************************************/


#include <stdio.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <unistd.h>
#include <errno.h>

#ifdef __SW_BW
    #include <wdefwin.h>
#endif

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

#define MAX_MODE 3           /* Max length of mode parameter */
#define NUM_OMODES 3         /* Number of (r, w, a) style modes */
#define NUM_FTYPES 3         /* Number of file types */
#define MAX_FILE_SIZE 100    /* Maximum number of bytes in a test file */
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

typedef struct testfile {
    FILE    *fp;
    char    filename[ L_tmpnam ];
} TestFile;

unsigned long int   tests = 0;         /* total number of tests */
unsigned long int   warnings = 0;      /* warnings to not skip further tests */
unsigned long int   failures = 0;      /* failures skip the test section */
FILE                *con;              /* console device */

/***********************************/
/* Generic TestFile based routines */
/* *********************************/

TestFile *TestFile_Get( const char *cur_mode )
/********************************************/
{
    TestFile    *cur_test;

    cur_test = malloc( sizeof( TestFile ) );
    INTERNAL( cur_test!=NULL );

    EXPECT( tmpnam( cur_test->filename ) != NULL );
    cur_test->fp = fopen( cur_test->filename, cur_mode );

    if( cur_mode[0] == 'r' ) {
        EXPECT( cur_test->fp == NULL );
    } else {
        VERIFY( cur_test->fp != NULL );
        EXPECT( !ferror( cur_test->fp ) );
    }

    return( cur_test );
}

int TestFile_ReOpen( TestFile *cur_test, const char *cur_mode )
/*************************************************************/
{
    if( cur_test->fp != NULL ) {
        fclose( cur_test->fp );
        VERIFY( !ferror( cur_test->fp ) );
        cur_test->fp = fopen( cur_test->filename, cur_mode );
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
    char    cur_mode[5] = "";

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

void Mode_Get( char *cur_mode, int cur_omode, int cur_update, int cur_ftype )
/***************************************************************************/
{
    int     pos;    /* Current string position */

    /* Convert the above integers to the coresponding file mode */
    switch( cur_omode ) {
        case 0: cur_mode[0] = 'r'; break;
        case 1: cur_mode[0] = 'w'; break;
        case 2: cur_mode[0] = 'a'; break;
        default: INTERNAL( cur_omode < 3 ); break;
    };

    pos = 1;
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

    /* Get a test string to insert into the file */

    for( i = 0; i < MAX_FILE_SIZE; i++ ) {
        cur_string[i] = 'k';
    }
    cur_string[i] = '\0';

    /* Create a test file */
    if( cur_mode[0] == 'r' ) {
        cur_mode[0] = 'w';
        cur_test = TestFile_Get( cur_mode );
        cur_mode[0] = 'r';
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

int Test_File_Seek( TestFile *cur_test, char *cur_mode )
/******************************************************/
{
    VERIFY( fseek( cur_test->fp, (1+MAX_FILE_SIZE), SEEK_SET ) == 0 );
    VERIFY( ftell( cur_test->fp ) == (1+MAX_FILE_SIZE) );
    VERIFY( fseek( cur_test->fp, -(1+MAX_FILE_SIZE), SEEK_CUR ) == 0 );
    VERIFY( ftell( cur_test->fp ) == 0 );
    VERIFY( fseek( cur_test->fp, 0L, SEEK_END ) == 0 );
    VERIFY( ftell( cur_test->fp ) == (1+MAX_FILE_SIZE) );
    rewind( cur_test->fp );
    VERIFY( ftell( cur_test->fp ) == 0 );

    return( 1 );
}

int Test_File_Writes( TestFile *cur_test, char *cur_mode, char test1 )
/********************************************************************/
{
    fpos_t      position;

    /* Test append and write modes */

    if( strchr( cur_mode, '+' )  !=  NULL ) {
        VERIFY( fseek( cur_test->fp, (1+MAX_FILE_SIZE), SEEK_SET ) == 0 );
        VERIFY( fgetpos( cur_test->fp, &position ) == 0 );
        VERIFY( fseek( cur_test->fp, 0, SEEK_SET ) == 0 );
        VERIFY( fputc( test1, cur_test->fp ) == test1 );
        EXPECT( fsetpos( cur_test->fp, &position ) == 0 );
        if( cur_mode[0] == 'a' ) {
            VERIFY( fgetc( cur_test->fp ) != EOF );
            VERIFY( feof( cur_test->fp ) == 0 );
        } else {
            VERIFY( fgetc( cur_test->fp ) == EOF );
            VERIFY( feof( cur_test->fp ) != 0 );
        }
    }

    return( 1 );
}

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

    /* fread, fwrite, fprintf, fscanf test */
    strcpy( buf1, "Hello, World!" );
    VERIFY( fwrite( buf1, 1, 13, cur_test->fp ) == 13 );
    VERIFY( fprintf( cur_test->fp, "%d\n", 31415 ) > 0 );

    Test_vfprintf( cur_test->fp, "%d\n", 34 );
    TestFile_ReOpen( cur_test, cur_mode );

    if( cur_mode[0] == 'r' ) {
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

int Test_File_Reads( TestFile *cur_test, char *cur_mode, char test1, char *cur_string )
/*************************************************************************************/
{
    char    buff[MAX_FILE_SIZE + 1];

    /* Test the reads from the test file  */
    if( cur_mode[0] == 'r' ) {
        INTERNAL( TestFile_ReOpen( cur_test, cur_mode ) );
        VERIFY( fseek( cur_test->fp, 0, SEEK_SET ) == 0 );
        VERIFY( fgetc( cur_test->fp ) == test1 );
        VERIFY( fgets( buff, MAX_FILE_SIZE + 1, cur_test->fp ) != NULL );
        EXPECT( strcmp( cur_string, buff ) == 0 );
    }

    return( 1 );
}

int Test_File_IO( char *cur_mode )
/********************************/
{
    TestFile    *cur_test;
    char        cur_string[MAX_FILE_SIZE + 1];
    char        test1 = 'a';

    cur_test = Test_File_Create( cur_mode, cur_string, test1 );

    if( cur_test != 0 ) {
        /* seek type functions */
        Test_File_Seek( cur_test, cur_mode );

        if( Test_File_Writes( cur_test, cur_mode, test1 ) != 0 ) {
            /* read functions */
            TestFile_ReOpen( cur_test, cur_mode );
            Test_File_Reads( cur_test, cur_mode, test1, cur_string );
        }
    }

    /* Free memory; close and remove test file */
    TestFile_Destroy( cur_test );

    return( 1 );
}

int Test_File_IO_More( char *cur_mode )
/*************************************/
{
    TestFile    *cur_test;

    /* Create a file (testing some write functinos) */
    if( cur_mode[0] == 'r' ) {
        cur_mode[0] = 'w';
        cur_test = TestFile_Get( cur_mode );
        cur_mode[0] = 'r';
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

int Test_StdWrite( FILE *fp, FILE **my_fp, const char *filename )
/***************************************************************/
{
    int     test_int = 21718, test_buff;
    char    test_str[10] = "Hello", buff[80] = "Different";
    char    cur_mode[10];

    if( fp == stdout ) {
        strcpy( cur_mode, "stdout" );
    } else if( fp == stderr ) {
        strcpy( cur_mode, "stderr" );
    }

    /* Test all the standard write and read routines */

    VERIFY( putc( 'a', fp ) != EOF );

    if( fp == stdout ) {
        VERIFY( putchar( 'b' ) != EOF );
        VERIFY( fputchar( 'c' ) != EOF );
        VERIFY( printf( "%d\n", test_int ) > 0 );
        VERIFY( puts( test_str ) > 0 );
        VERIFY( putchar( '\n' ) != EOF );
        Test_vprintf( "%d\n", 53 );
    }

    fflush( *my_fp );

    /* Check the data just written */
    fseek( *my_fp, 0, SEEK_SET );
    VERIFY( fgetc( *my_fp ) == 'a' );

    if( fp == stdout ) {
        VERIFY( fgetc( *my_fp ) == 'b' );
        VERIFY( fgetc( *my_fp ) == 'c' );

        /* Check all the reads as well */
        EXPECT( freopen( CONSOLE_IN, "r+t", *my_fp ) != NULL );
        VERIFY( (*my_fp = freopen( filename, "rt", stdin )) != NULL );

        fseek( *my_fp, 0, SEEK_SET );
        VERIFY( getc( stdin ) == 'a' );
        VERIFY( getchar() == 'b' );
        VERIFY( fgetchar() == 'c' );
        VERIFY( scanf( "%d", &test_buff ) > 0 );
        VERIFY( test_buff == test_int );
        VERIFY( ( gets( buff ) ) != NULL );
        VERIFY( ( gets( buff ) ) != NULL );
        VERIFY( strcmp( buff, test_str ) == 0 );
        Test_vscanf( "%d\n", &test_int );
        VERIFY( test_int == 53 );
    }

    return( 1 );
}

int Test_StdWrites( FILE *fp )
/****************************/
{
    char    cur_mode[10] = "stdio";
    char    filename[ L_tmpnam ];
    FILE    *my_fp;

    EXPECT( tmpnam( filename ) != NULL );
    EXPECT( (my_fp = freopen( filename, "a+t", fp )) != NULL );

    /* A separate function is used to assure the deletion of the test file. */

    Test_StdWrite( fp, &my_fp, filename );

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

#ifdef __SW_BW
    con = fopen( "tmp.log", "a" );
#else
    con = fopen( CONSOLE_OUT, "w" );
#endif
    VERIFY( con != NULL );

    /******************/
    /* Start of tests */
    /******************/

    /* The following tests are run for each file mode type */

    for( cur_omode = 0; cur_omode < NUM_OMODES; cur_omode++ ) {
        for( cur_update = 0; cur_update < 2; cur_update++ ) {
            for( cur_ftype = 0; cur_ftype < NUM_FTYPES; cur_ftype++ ) {
                /* Get the mode string */
                Mode_Get( cur_mode, cur_omode, cur_update, cur_ftype);

                /* Test fopen, fclose, and ferror */
                cur_test = TestFile_Get( cur_mode );
                  TestFile_Destroy( cur_test );

                /* Test standard file input and output functions */
                Test_File_IO( cur_mode );

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

    Test_StdWrites( stderr );
    Test_StdWrites( stdout );
    Test_File_Errors( );   /* eg.  perror, ferror, etc.. */
    Test_Flushes( );
    Test_fdopen( );
    Test_setbuf( );
    Test_setvbuf( );
    Test_ungetc( );

    fprintf( old_stdout, "Tests completed (%s).\n", strlwr( argv[0] ) );
    fclose( old_stdout );
    fclose( con );
    //Status_Print( );
#ifdef __SW_BW
    con = fopen( "tmp.log", "a" );
    fprintf( con, "Tests completed (%s).\n", strlwr( argv[0] ) );
    _dwShutDown();
#endif

    /****************/
    /* End of tests */
    /****************/

    return( 0 );
}
