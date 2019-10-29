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


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#ifdef __UNIX__
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#else
#include <direct.h>
#endif
#include <fnmatch.h>

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
    #define _tDIR               DIR
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

#ifdef __UNIX__
#define DIRSEP      '/'
#define DIRSEP_S    "/"
#else
#define DIRSEP      '\\'
#define DIRSEP_S    "\\"
#endif

#define NUM_OPEN        5
#ifdef __SW_BW
    #define VERIFY( expr ) \
        if( !(expr) ) {                                             \
            printf( "FAIL: %s, line %u\n", #expr, __LINE__ );       \
            printf( "Note: make sure "TMPDIRNAME" is removed\n" );  \
            printf( "Abnormal termination.\n" );                    \
            exit( 100 );                                            \
        }
#else
    #define VERIFY( expr ) \
        if( !(expr) ) {                                             \
            printf( "FAIL: %s, line %u\n", #expr, __LINE__ );       \
            printf( "Note: make sure "TMPDIRNAME" is removed\n" );  \
            abort();                                                \
        }
#endif

int main( int argc, char *argv[] )
{
    CHAR_TYPE           *cwd;
    CHAR_TYPE           buffer[256], buffer2[256];
    CHAR_TYPE           pattern[256];
    int                 ctr;
    FILE                *fp;
    _tDIR               *dirp;
    struct dirent       *direntp;
    unsigned            checkbits = 0;
    unsigned            save_checkbits;

#ifdef __SW_BW
    FILE *my_stdout;

    my_stdout = __F_NAME(freopen,_wfreopen)( STRING( "tmp.log" ), STRING( "a" ), stdout );
    VERIFY( my_stdout != NULL );
#endif

    /* unused parameters */ (void)argc;

    /*** Convert multi-byte name to wide char name ***/
#ifdef MBYTE_NAMES
    mbstowcs( TmpDir, "_TMP\x90\x92\x90\x90", 9 );
    mbstowcs( TmpFilePrefix, "TMP_\x90\x90\x90\x92", 9 );
#endif

    VERIFY( ( cwd = __F_NAME(getcwd,_wgetcwd)( NULL, 0 ) ) != NULL );
    VERIFY( __F_NAME(getcwd,_wgetcwd)( buffer, 256 ) != NULL );
    VERIFY( __F_NAME(strcmp,wcscmp)( buffer, cwd ) == 0 );
    free( cwd );
#ifdef __UNIX__
    VERIFY( __F_NAME(mkdir,_wmkdir)( TMPDIR, S_IRWXU ) == 0 );
#else
    VERIFY( __F_NAME(mkdir,_wmkdir)( TMPDIR ) == 0 );
#endif
    VERIFY( __F_NAME(chdir,_wchdir)( TMPDIR ) == 0 );
    VERIFY( ( cwd = __F_NAME(getcwd,_wgetcwd)( NULL, 0 ) ) != NULL );
    if( buffer[__F_NAME(strlen,wcslen)(buffer)-1] != DIRSEP ) {
        __F_NAME(strcat,wcscat)( buffer, STRING( DIRSEP_S ) );
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

    save_checkbits = checkbits;

    VERIFY( __F_NAME(chdir,_wchdir)( STRING( ".." ) ) == 0 );
    __F_NAME(strcpy,wcscpy)( buffer, TMPDIR );
    __F_NAME(strcpy,wcscpy)( pattern, TMPFILEPREFIX );
    __F_NAME(strcat,wcscat)( pattern, STRING( ".*" ) );
#ifndef __UNIX__
    __F_NAME(strcat,wcscat)( buffer, STRING( DIRSEP_S ) );
    __F_NAME(strcat,wcscat)( buffer, pattern );
#endif

    /* Open the directory using a wildcard pattern. */
    VERIFY( ( dirp = __F_NAME(opendir,_wopendir)( buffer ) ) != NULL );

    for( ctr = 0; ctr < NUM_OPEN; ) {
        direntp = __F_NAME(readdir,_wreaddir)( dirp );
        if( direntp == NULL )
            break;
#ifdef __UNIX__
        if( fnmatch( pattern, direntp->d_name, 0 ) )
            continue;
#endif
        __F_NAME(strcpy,wcscpy)( buffer, TMPFILEPREFIX );
        __F_NAME(strcat,wcscat)( buffer, STRING( "." ) );
        __F_NAME(itoa,_witoa)( ctr, buffer2, 10 );
        __F_NAME(strcat,wcscat)( buffer, buffer2 );
        if( __F_NAME(strcmp,wcscmp)(buffer,direntp->d_name) != 0 )
            continue;
        checkbits &= ~( 1 << ctr );
        ++ctr;
        __F_NAME(rewinddir,_wrewinddir)( dirp );
    }

    VERIFY( checkbits == 0 );   // If not, readdir() didn't report all files

    __F_NAME(rewinddir,_wrewinddir)( dirp );
    checkbits = save_checkbits;

    for( ctr = 0; ctr < NUM_OPEN; ) {
        direntp = __F_NAME(readdir,_wreaddir)( dirp );
        if( direntp == NULL )
            break;
#ifdef __UNIX__
        if( fnmatch( pattern, direntp->d_name, 0 ) )
            continue;
#endif
        __F_NAME(strcpy,wcscpy)( buffer, TMPFILEPREFIX );
        __F_NAME(strcat,wcscat)( buffer, STRING( "." ) );
        __F_NAME(itoa,_witoa)( ctr, buffer2, 10 );
        __F_NAME(strcat,wcscat)( buffer, buffer2 );
        if( __F_NAME(strcmp,wcscmp)(buffer,direntp->d_name) != 0 )
            continue;
        checkbits &= ~( 1 << ctr );
        ++ctr;
        __F_NAME(rewinddir,_wrewinddir)( dirp );
    }

    VERIFY( checkbits == 0 );   // If not, readdir() didn't report all files
    VERIFY( __F_NAME(closedir,_wclosedir)( dirp ) == 0 );

    /* Open the directory itself, no pattern. */
    VERIFY( ( dirp = __F_NAME(opendir,_wopendir)( TMPDIR ) ) != NULL );
    checkbits = save_checkbits;

    for( ctr = 0; ctr < NUM_OPEN; ) {
        direntp = __F_NAME(readdir,_wreaddir)( dirp );
        if( direntp == NULL )
            break;
        /* Skip '.' and '..' entries. */
        if( direntp->d_name[0] == '.' )
            continue;
        __F_NAME(strcpy,wcscpy)( buffer, TMPFILEPREFIX );
        __F_NAME(strcat,wcscat)( buffer, STRING( "." ) );
        __F_NAME(itoa,_witoa)( ctr, buffer2, 10 );
        __F_NAME(strcat,wcscat)( buffer, buffer2 );
        if( __F_NAME(strcmp,wcscmp)(buffer,direntp->d_name) != 0 )
            continue;
        checkbits &= ~( 1 << ctr );
        ++ctr;
        __F_NAME(rewinddir,_wrewinddir)( dirp );
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
    return( EXIT_SUCCESS );
}

