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
* Description:  Test startup and nonlocal goto related processing.
*
****************************************************************************/


#include <stdarg.h>             /* For va_list, etc. */
#include <stdio.h>
#include <stdlib.h>             /* for EXIT_SUCCESS, etc */
#include <string.h>             /* For strcmp */
#include <setjmp.h>

#ifdef __SW_BW
    #include <wdefwin.h>
#endif


/*
 * The following is abstracted from "widechar.h".
 * The makefile does not support -I directory
 * Relative file references to that file do not work.
 * Data duplication is reluctantly necessary
 */

/*** Define some handy macros ***/
#ifdef __WIDECHAR__
    #include <wchar.h>          /* For wcscmp */
    #define CHAR_TYPE           wchar_t
    #define __F_NAME(n1,n2)     n2
#else
    #define CHAR_TYPE           char
    #define __F_NAME(n1,n2)     n1
#endif

/*
 * The following is abstracted from initargv.c
 */
extern  int         __historical_splitparms;
_WCRTDATA extern CHAR_TYPE  *__F_NAME( _LpPgmName, _LpwPgmName );
_WCRTDATA extern CHAR_TYPE  *__F_NAME( _LpCmdLine, _LpwCmdLine );
extern  int         __F_NAME( _argc, _wargc );              /* argument count  */
_WCRTDATA extern int        __F_NAME( ___Argc, ___wArgc );  /* argument count */
extern  CHAR_TYPE **__F_NAME( _argv, _wargv );              /* argument vector */
_WCRTDATA extern CHAR_TYPE**__F_NAME( ___Argv, ___wArgv );  /* argument vector */

void __F_NAME( __Init_Argv, __wInit_Argv )( void );
void __F_NAME( __Fini_Argv, __wFini_Argv )( void );

#ifndef __WIDECHAR__
#define TAGSTR(X) X
#define STR(X) #X
#else
#define TAGSTR(X) L##X
#define STR(X) TAGSTR(#X)
#endif
#define TEXTNAME(X) STR(X)

#define VERIFY( exp )   if( !( exp ) ) {                                                 \
                            printf( __F_NAME(                                            \
                            "%s: ***FAILURE*** at line %d of %s from line %d.\n",        \
                            "%ls: ***FAILURE*** at line %d of %s from line %d.\n" ),     \
                                    ProgramName, __LINE__, SrcFILE, line );              \
                            NumErrors++;                                                 \
                            exit( EXIT_FAILURE );                                        \
                        }

CHAR_TYPE ProgramName[128];                     // executable filename
char SrcFILE[128];                              // Lowered __FILE__
int NumErrors = 0;                              // number of errors

static void jump_fn( jmp_buf env )
{
    longjmp( env, 7 );
}

static void test_long_jump( int line )
{
    volatile int    first_pass;     // volatile is required!
    jmp_buf         env;
    int             rc;

    first_pass = 1;
    rc = setjmp( env );
    if( first_pass ) {
        VERIFY( rc == 0 );
        first_pass = 0;
        jump_fn( env );
    } else {
        VERIFY( rc == 7 );
    }
    VERIFY( rc == 7 );
}

static void tryanalysis( int line, int hist, const CHAR_TYPE *pgm, const CHAR_TYPE *args,
        int argc, ... )
{
    int         i;
    int         status;

    // Set __Init_Argv input interface
    __historical_splitparms = hist;
    __F_NAME( _LpPgmName, _LpwPgmName ) = (CHAR_TYPE *) pgm;
    __F_NAME( _LpCmdLine, _LpwCmdLine ) = (CHAR_TYPE *) args;

    __F_NAME( __Init_Argv, __wInit_Argv )(); // Call the function under test

    // These statements facilitate development.
#if 0
    printf( __F_NAME( "%s\t= \"%s\"\n" , "%ls\t= \"%ls\"\n" ),
        TEXTNAME( __F_NAME( _LpPgmName, _LpwPgmName ) ),
        __F_NAME( _LpPgmName, _LpwPgmName ) );
    printf( __F_NAME( "%s\t= \"%s\"\n" , "%ls\t= \"%ls\"\n" ),
        TEXTNAME( __F_NAME( _LpCmdLine, _LpwCmdLine ) ),
        __F_NAME( _LpCmdLine, _LpwCmdLine ) );
    printf( __F_NAME( "%s\t= %d\n" , "%ls\t= %d\n" ),
        TEXTNAME( __F_NAME( _argc, _wargc ) ),
        __F_NAME( _argc, _wargc ) );
    printf( __F_NAME( "%s\t= %d\n" , "%ls\t= %d\n" ),
        TEXTNAME( __F_NAME( ___Argc, ___wArgc ) ),
        __F_NAME( ___Argc, ___wArgc ) );
    printf( __F_NAME( "%s\t= %p\n" , "%ls\t= %p\n" ),
        TEXTNAME( __F_NAME( _argv, _wargv ) ),
        __F_NAME( _argv, _wargv ) );
    printf( __F_NAME( "%s\t= %p\n" , "%ls\t= %p\n" ),
        TEXTNAME( __F_NAME( ___Argv, ___wArgv ) ),
        __F_NAME( ___Argv, ___wArgv ) );

    for( i = 1; i < __F_NAME( _argc, _wargc ); i++ ) {
        printf( __F_NAME( "%s[%d]\t= \"%s\"\n" , "%ls[%d]\t= \"%ls\"\n" ),
            TEXTNAME( __F_NAME( _argv, _wargv ) ), i,
            __F_NAME( _argv, _wargv ) [i] );
    }
#endif

    // Check output interface

    status = ( __F_NAME( strcmp, wcscmp )( pgm, __F_NAME( _LpPgmName, _LpwPgmName ) ) );
    VERIFY( status == 0 ); // Input and output program name should be equal

    status = ( __F_NAME( strcmp, wcscmp )( args, __F_NAME( _LpCmdLine, _LpwCmdLine ) ) );
    VERIFY( status == 0 ); // Input and output arguments should be equal

    status = (argc == __F_NAME( _argc, _wargc ) );
    VERIFY( status == 1 ); // Expected argc and _argc should be equal

    status = (argc == __F_NAME( ___Argc, ___wArgc ) );
    VERIFY( status == 1 ); // Expected argc and ___Argc should be equal

    status = ( __F_NAME( _argv, _wargv ) == __F_NAME( ___Argv, ___wArgv ) );
    VERIFY( status == 1 ); // Expected _argv and ___Argv should be equal

    status = ( __F_NAME( strcmp, wcscmp )( pgm, __F_NAME( _argv, _wargv )[0] ) );
    VERIFY( status == 0 ); // program name and first argument should be equal

    {
        va_list ap;

        va_start( ap, argc );
        for( i = 1; i < argc; i++ ) {
            const CHAR_TYPE * const expected = va_arg( ap, CHAR_TYPE * );
            const CHAR_TYPE * const happened = __F_NAME( _argv, _wargv )[i];

            status = ( __F_NAME( strcmp, wcscmp )( expected, happened ) );
            VERIFY( status == 0 );
        }
        va_end( ap );
    }

    __F_NAME( __Fini_Argv, __wFini_Argv )(); // Throw away data
}

int __F_NAME( main, wmain )( int argc, const CHAR_TYPE * const * const argv )
{
#ifdef __SW_BW
    FILE    *my_stdout;

    my_stdout = freopen( "tmp.log", "a", stdout );
    if( my_stdout == NULL ) {
        fprintf( stderr, "Unable to redirect stdout\n" );
        return( EXIT_FAILURE );
    }
#endif

    // Initialize
    (void) __F_NAME( strcpy, wcscpy )( ProgramName, argv[0] );     // store filename
    (void) __F_NAME( strlwr, _wcslwr )( ProgramName );             // and lower case it
    (void) strcpy( SrcFILE, __FILE__ ), (void) strlwr( SrcFILE );  // lowered __FILE__

    (void) argc; // Unused

//  This was causing a crash in the multi-threaded 16-bit OS/2 runtime.
//  This test program relies far too much on clib internals but probably
//  isn't worth "fixing" properly.
//    __F_NAME( __Fini_Argv, __wFini_Argv )(); // Release allocated data

    // tryanalysis( __LINE__, 0, 0, 0 ); // Fails as _Lp* can't be NULL

        // Program name and no arguments
    tryanalysis( __LINE__, 0, TAGSTR( "hello" ), TAGSTR( "" ), 1);

        // one argument
    tryanalysis( __LINE__, 0, TAGSTR( "hello" ), TAGSTR( "world" ),
        2, TAGSTR( "world" ) );

        // argument-enclosing whitespace is ignored
    tryanalysis( __LINE__, 0, TAGSTR( "2 params" ), TAGSTR( " \thello \tworld \t" ),
        3, TAGSTR( "hello" ), TAGSTR( "world" ) );

        // quotes at both ends
    tryanalysis( __LINE__, 0, TAGSTR( "1 arg" ), TAGSTR( "\"hello world\"" ),
        2, TAGSTR( "hello world" ) );

        // Lost closing quote ignored
    tryanalysis( __LINE__, 0, TAGSTR( "1 arg" ), TAGSTR( "\"hello world" ),
        2, TAGSTR( "hello world" ) );

        // opening quote not at start
    tryanalysis( __LINE__, 0, TAGSTR( "1 arg" ), TAGSTR( "hell\"o world" ),
        2, TAGSTR( "hello world" ) );

        // quotes away from both ends
    tryanalysis( __LINE__, 0, TAGSTR( "1 arg" ), TAGSTR( "hell\"o w\"orld" ),
        2, TAGSTR( "hello world" ) );

    // Check historical behaviour
        // quotes at both ends
    tryanalysis( __LINE__, 1, TAGSTR( "1 arg" ), TAGSTR( "\"hello world\"" ),
        2, TAGSTR( "hello world" ) );

        // Lost closing quote ignored
    tryanalysis( __LINE__, 1, TAGSTR( "1 arg" ), TAGSTR( "\"hello world" ),
        2, TAGSTR( "hello world" ) );

        // opening quote not at start
    tryanalysis( __LINE__, 1, TAGSTR( "1 arg" ), TAGSTR( "hell\"o world" ),
        3, TAGSTR( "hell\"o" ), TAGSTR( "world" ) );

        // quotes away from both ends
    tryanalysis( __LINE__, 1, TAGSTR( "2 args" ), TAGSTR( "hell\"o w\"orld" ),
        3, TAGSTR( "hell\"o" ), TAGSTR( "w\"orld" ) );

        // backslash usage
    tryanalysis( __LINE__, 0, TAGSTR( "2 args" ), TAGSTR( "hell\\\"o w\\o\"rld wars" ),
        3, TAGSTR( "hell\"o" ), TAGSTR( "w\\orld wars" ) );

    tryanalysis( __LINE__, 1, TAGSTR( "3 args" ), TAGSTR( "hell\\\"o w\\o\"rld wars" ),
        4, TAGSTR( "hell\"o" ), TAGSTR( "w\\o\"rld" ), TAGSTR( "wars" ) );

    test_long_jump( __LINE__ );

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
