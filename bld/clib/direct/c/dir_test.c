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
* Description:  Simple test of directory related clib functions.
*
****************************************************************************/


#include <sys/types.h>
#include <direct.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef __SW_BW
    #include <wdefwin.h>
#endif

#ifdef __WIDECHAR__
    #define CHAR_TYPE           wchar_t
    #define __F_NAME(n1,n2)     n2
    #define STRING(a)           L##a
#else
    #define CHAR_TYPE           char
    #define __F_NAME(n1,n2)     n1
    #define STRING(a)           a
#endif


#ifdef __WIDECHAR__
    #define _tDIR               struct _wdirent
    #define TMPDIR              TmpDir
    #define TMPDIRNAME          "_TMP""\x90\x92\x90\x90"
    #define TMPFILEPREFIX       TmpFilePrefix
    wchar_t                     TmpDir[256];
    wchar_t                     TmpFilePrefix[256];
    #define MBYTE_NAMES
#else
    #define _tDIR               struct dirent
#ifdef __RDOS__
    #define TMPDIR              "tmpdir"
    #define TMPDIRNAME          "tmpdir"
    #define TMPFILEPREFIX       "tmpfile"
#else    
    #define TMPDIR              "_T_M_P_"
    #define TMPDIRNAME          "_T_M_P_"
    #define TMPFILEPREFIX       "_TMPFILE"
#endif    
#endif

#define NUM_OPEN        5
#ifdef __SW_BW
 #define VERIFY( expr ) if( !(expr) ) {                                     \
                          printf( "FAIL: %s, line %u\n", #expr, __LINE__ ); \
                          printf( "Note: make sure "TMPDIRNAME" is removed\n" );\
                          printf( "Abnormal termination.\n" );              \
                          exit( 100 );                                      \
                        }
#else
 #define VERIFY( expr ) if( !(expr) ) {                                     \
                          printf( "FAIL: %s, line %u\n", #expr, __LINE__ ); \
                          printf( "Note: make sure "TMPDIRNAME" is removed\n" );\
                          abort();                                          \
                        }
#endif

void main( int argc, char *argv[] )
{
    CHAR_TYPE           *cwd;
    CHAR_TYPE           buffer[256], buffer2[256];
    int                 ctr;
    FILE                *fp;
    _tDIR               *dirp;
    _tDIR               *direntp;
    unsigned            checkbits = 0;
    unsigned            save_checkbits;

    #ifdef __SW_BW
        FILE *my_stdout;
        my_stdout = __F_NAME(freopen,_wfreopen)( STRING( "tmp.log" ), STRING( "a" ), stdout );
        VERIFY( my_stdout != NULL );
    #endif

    /*** Convert multi-byte name to wide char name ***/
    #ifdef MBYTE_NAMES
        mbstowcs( TmpDir, "_TMP\x90\x92\x90\x90", 9 );
        mbstowcs( TmpFilePrefix, "TMP_\x90\x90\x90\x92", 9 );
    #endif

    VERIFY( ( cwd = __F_NAME(getcwd,_wgetcwd)( NULL, 0 ) ) != NULL );
    VERIFY( __F_NAME(getcwd,_wgetcwd)( buffer, 256 ) != NULL );
    VERIFY( __F_NAME(strcmp,wcscmp)( buffer, cwd ) == 0 );
    free( cwd );
    VERIFY( __F_NAME(mkdir,_wmkdir)( TMPDIR ) == 0 );
    VERIFY( __F_NAME(chdir,_wchdir)( TMPDIR ) == 0 );
    VERIFY( ( cwd = __F_NAME(getcwd,_wgetcwd)( NULL, 0 ) ) != NULL );
    if( buffer[__F_NAME(strlen,wcslen)(buffer)-1] != '\\' ) {
        __F_NAME(strcat,wcscat)( buffer, STRING( "\\" ) );
    }
    __F_NAME(strcat,wcscat)( buffer, TMPDIR );
    VERIFY( __F_NAME(strcmp,wcscmp)( buffer, cwd ) == 0 );
    free( cwd );

    for( ctr = 0; ctr < NUM_OPEN; ++ctr ) {
        __F_NAME(strcpy,wcscpy)( buffer, TMPFILEPREFIX );
        __F_NAME(strcat,wcscat)( buffer, STRING( "." ) );
        __F_NAME(itoa,_witoa)( ctr, buffer2, 10 );
        __F_NAME(strcat,wcscat)( buffer, buffer2 );
        fp = __F_NAME(fopen,_wfopen)( buffer, STRING( "w" ) );
        if( fp == NULL ) {
            __F_NAME(printf,wprintf)( STRING( "INTERNAL: fopen failed\n" ) );
            abort();
        }
        checkbits |= ( 1 << ctr );
        fclose( fp );
    }

    VERIFY( __F_NAME(chdir,_wchdir)( STRING( ".." ) ) == 0 );
    __F_NAME(strcpy,wcscpy)( buffer, TMPDIR );
    __F_NAME(strcat,wcscat)( buffer, "\\" );
    __F_NAME(strcat,wcscat)( buffer, TMPFILEPREFIX );
    __F_NAME(strcat,wcscat)( buffer, ".*" );
    save_checkbits = checkbits;

    /* Open the directory using a wildcard pattern. */
    VERIFY( ( dirp = __F_NAME(opendir,_wopendir)( buffer ) ) != NULL );

    for( ctr = 0;; ++ctr ) {
        direntp = __F_NAME(readdir,_wreaddir)( dirp );
        if( direntp == NULL )
            break;
        __F_NAME(strcpy,wcscpy)( buffer, TMPFILEPREFIX );
        __F_NAME(strcat,wcscat)( buffer, STRING( "." ) );
        __F_NAME(itoa,_witoa)( ctr, buffer2, 10 );
        __F_NAME(strcat,wcscat)( buffer, buffer2 );
        VERIFY( __F_NAME(strcmp,wcscmp)(buffer,direntp->d_name) == 0 );
        checkbits &= ~( 1 << ctr );
    }

    VERIFY( checkbits == 0 );   // If not, readdir() didn't report all files

    __F_NAME(rewinddir,_wrewinddir)( dirp );
    checkbits = save_checkbits;

    for( ctr = 0;; ++ctr ) {
        direntp = __F_NAME(readdir,_wreaddir)( dirp );
        if( direntp == NULL )
            break;
        __F_NAME(strcpy,wcscpy)( buffer, TMPFILEPREFIX );
        __F_NAME(strcat,wcscat)( buffer, STRING( "." ) );
        __F_NAME(itoa,_witoa)( ctr, buffer2, 10 );
        __F_NAME(strcat,wcscat)( buffer, buffer2 );
        VERIFY( __F_NAME(strcmp,wcscmp)(buffer,direntp->d_name) == 0 );
        checkbits &= ~( 1 << ctr );
    }

    VERIFY( checkbits == 0 );   // If not, readdir() didn't report all files
    VERIFY( __F_NAME(closedir,_wclosedir)( dirp ) == 0 );

    /* Open the directory itself, no pattern. */
    VERIFY( ( dirp = __F_NAME(opendir,_wopendir)( TMPDIR ) ) != NULL );
    checkbits = save_checkbits;

    ctr = 0;
    direntp = __F_NAME(readdir,_wreaddir)( dirp );
    while( direntp ) {
        /* Skip '.' and '..' entries. */
        if( direntp->d_name[0] != '.' ) {
            __F_NAME(strcpy,wcscpy)( buffer, TMPFILEPREFIX );
            __F_NAME(strcat,wcscat)( buffer, STRING( "." ) );
            __F_NAME(itoa,_witoa)( ctr, buffer2, 10 );
            __F_NAME(strcat,wcscat)( buffer, buffer2 );
            VERIFY( __F_NAME(strcmp,wcscmp)(buffer,direntp->d_name) == 0 );
            checkbits &= ~( 1 << ctr );
            ++ctr;
        }
        direntp = __F_NAME(readdir,_wreaddir)( dirp );
    }

    VERIFY( checkbits == 0 );   // If not, readdir() didn't report all files
    VERIFY( __F_NAME(closedir,_wclosedir)( dirp ) == 0 );

    VERIFY( __F_NAME(rmdir,_wrmdir)( TMPDIR ) == -1 ); // Should == -1; TMPDIR non-empty
    VERIFY( __F_NAME(chdir,_wchdir)( TMPDIR ) == 0 );

    for( ctr = 0; ctr < NUM_OPEN; ++ctr ) {
        __F_NAME(strcpy,wcscpy)( buffer, TMPFILEPREFIX );
        __F_NAME(strcat,wcscat)( buffer, STRING( "." ) );
        __F_NAME(itoa,_witoa)( ctr, buffer2, 10 );
        __F_NAME(strcat,wcscat)( buffer, buffer2 );
        if( __F_NAME(remove,_wremove)( buffer ) != 0 ) {
            __F_NAME(printf,wprintf)( STRING( "INTERNAL: remove() failed\n" ) );
            abort();
        }
    }

    VERIFY( __F_NAME(chdir,_wchdir)( STRING( ".." ) ) == 0 );
    VERIFY( __F_NAME(rmdir,_wrmdir)( TMPDIR ) == 0 );
    VERIFY( __F_NAME(chdir,_wchdir)( TMPDIR ) != 0 );
    printf( "Tests completed (%s).\n", strlwr( argv[0] ) );

    #ifdef __SW_BW
        fprintf( stderr, "Tests completed (%s).\n", strlwr( argv[0] ) );
        fclose( my_stdout );
        _dwShutDown();
    #endif
    exit( 0 );
}
