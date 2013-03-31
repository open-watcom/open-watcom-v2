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
* Description:  Sweep utility - run a command recursively in subtree.
*
****************************************************************************/

#include <ctype.h>
#ifdef __WATCOMC__
    #include <process.h>
#endif
#include <signal.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#ifdef __UNIX__
    #include <dirent.h>
#else
    #include <direct.h>
    #include <dos.h>
#endif
#include "wio.h"
#include "watcom.h"

char    *Help[] = {
"Usage: SWEEP [options] cmd",
"     Execute 'cmd' in the each subdirectory of the current working directory.",
"",
"Options:",
"     -a execute command once for each file in each directory",
"     -v verbose mode -- print directory headers",
"     -p print mode -- print all commands as executed",
"     -n don't really execute commands, (turns on -p)",
"     -d perform depth-first traversal, (default is breadth-first)",
"     -ln only descend n directory levels (n defaults to 1)",
"",
"In 'cmd', the following substitutions are made:",
"     %f name relative to starting directory.   ie: 'SUBDIR\\C\\NEW\\FOO.BAR'",
"     %p path relative to starting directory.   ie: 'SUBDIR\\C\\NEW'",
"     %l full name of current file if -a used.  ie: 'FOO.BAR'",
"     %n name of current file if -a used.       ie: 'FOO'",
"     %e extension of current file if -a used.  ie: '.BAR'",
"     %% a percent charater",
"",
"Note:",
"     SWEEP sets the current working directory to each directory as it proceeds.",
"     The command 'SWEEP -a type %f' will not work, since %f is relative to",
"     the starting directory. The command 'SWEEP -a type %l' will work.",
"",
"     The special command 'COMPARE' (must be upper case) is handled internally:",
"         SWEEP -a -v COMPARE %l x:\\dir\\%f",
"     will compare two trees and make sure all the files are the same.",
    NULL
};

char    Buff[1024];
char    CmdBuff[1024];
char    *CmdLine;
char    SaveDir[_MAX_PATH];

struct {
    unsigned    allfiles : 1;
    unsigned    verbose : 1;
    unsigned    print : 1;
    unsigned    noexec : 1;
    unsigned    depthfirst : 1;
    int         levels;
}       Options;

typedef struct dirstack {
    struct dirstack     *prev;
    char                name_len;
    char                name[_MAX_FNAME + _MAX_EXT];
} dirstack;

dirstack        *Stack = NULL;
int             DoneFlag = 0;


void SetDoneFlag( int dummy )
{
    dummy = dummy;
    DoneFlag = 1;
}


void *SafeMalloc( size_t n )
{
    void *p = malloc( n );
    if( p == NULL ) {
        puts( "Out of memory!" );
        SetDoneFlag( 17 /* dummy - could be any value */  );
    }
    return( p );
}


static char     CurrPathBuff[_MAX_PATH + 2];

char *CurrPath( void )
{
    char        *p;
    dirstack    *stack;

    if( Stack->prev == NULL )
        return( "." );
    p = CurrPathBuff + _MAX_PATH;
    *--p = '\0';
    stack = Stack;
    for( ;; ) {
        p -= stack->name_len;
        memcpy( p, stack->name, stack->name_len );
        stack = stack->prev;
        if( stack->prev == NULL )
            break;
        *--p = '\\';
    }
    return( p );
}


char *StringCopy( char *dst, char *src )
{
    while( ( *dst = *src ) != '\0' ) {
        ++dst;
        ++src;
    }
    return( dst );
}


char *strip( char *buff )
{
    char        *p;
    char        *old;

    old = p = buff;
    while( *p ) {
        if( *p == '"' ) {
            memmove( p, p + 1, strlen( p ) );
        } else {
            ++p;
        }
    }
    return old;
}
#define BUFF_SIZE       (16*1024)

void Compare( char *buff )
{
    char        *one;
    char        *two;
    FILE        *fp1;
    FILE        *fp2;
    size_t      len1;
    size_t      len2;
    static char *buff1;
    static char *buff2;
    int         inQuote;

    if( buff1 == NULL ) {
        buff1 = SafeMalloc( 2*BUFF_SIZE );
        if( buff1 == NULL )
            return;
        buff2 = &buff1[BUFF_SIZE];
    }
    while( isspace( *buff ) )
        ++buff;
    one = buff;
    inQuote = 0;
    for( ;; ) {
        if( isspace( *buff ) && !inQuote )
            break;
        if( *buff == '"' )
            inQuote = !inQuote;
        ++buff;
    }
    *buff++ = '\0';
    while( isspace( *buff ) )
        ++buff;
    two = buff;
    fp1 = fopen( strip( one ), "rb" );
    if( fp1 == NULL ) {
        printf( "Can't open '%s'\n", one );
        return;
    }
    fp2 = fopen( strip( two ), "rb" );
    if( fp2 == NULL ) {
        fclose( fp1 );
        printf( "Can't open '%s'\n", two );
        return;
    }
    for( ;; ) {
        len1 = fread( buff1, 1, BUFF_SIZE, fp1 );
        len2 = fread( buff2, 1, BUFF_SIZE, fp2 );
        if( len1 < len2 ) {
            printf( " '%s' is shorter than '%s'\n", one, two );
            break;
        }
        if( len1 > len2 ) {
            printf( " '%s' is longer than '%s'\n", one, two );
            break;
        }
        if( len1 == 0 )
            break;
        if( memcmp( buff1, buff2, len1 ) != 0 ) {
            printf( " '%s' differs from '%s'\n", one, two );
            break;
        }
    }
    fclose( fp1 );
    fclose( fp2 );
}


void SubstituteAndRun( char *fname )
{
    char        *src, *dst, *start;
    char        name[_MAX_FNAME];
    char        ext[_MAX_EXT];

    _splitpath( fname, NULL, NULL, name, ext );
    dst = Buff;
    for( src = CmdLine; *src != '\0';++src ) {
        if( *src != '%' ) {
            *dst++ = *src;
            continue;
        }
        ++src;
        start = dst;
        switch( *src ) {
        case 'f':
        case 'F':
            dst = StringCopy( dst, CurrPath() );
            if( name[0] || ext[0] ) {
                dst = StringCopy( dst, "\\" );
            }
            dst = StringCopy( dst, name );
            dst = StringCopy( dst, ext );
            break;
        case 'l':
        case 'L':
            dst = StringCopy( dst, name );
            dst = StringCopy( dst, ext );
            break;
        case 'p':
        case 'P':
            dst = StringCopy( dst, CurrPath() );
            break;
        case 'n':
        case 'N':
            dst = StringCopy( dst, name );
            break;
        case 'e':
        case 'E':
            dst = StringCopy( dst, ext );
            break;
        case '\0':
            *dst++ = '%';
            --src; /* special case for % at end of string */
            break;
        default:
            *dst++ = *src;
            break;
        }
        *dst = '\0';
        if( strchr( start, ' ' ) != NULL ) {
            memmove( start + 1, start, dst - start );
            *start = '"';
            ++dst;
            *dst = '"';
            ++dst;
        }
    }
    *dst = '\0';
    if( Options.print || Options.noexec ) {
        printf( "%s\n", Buff );
    }
    if( !Options.noexec ) {
        fflush( stdout );
        #define CMP_NAME        "COMPARE"
        if( memcmp( Buff, CMP_NAME, sizeof( CMP_NAME ) - 1 ) == 0 ) {
            Compare( &Buff[sizeof( CMP_NAME )] );
            fflush( stdout );
        } else {
            system( Buff );
        }
    }
}


void ExecuteCommands( void )
{
    DIR                 *dirh;
    struct dirent       *dp;

    if( Options.verbose )
        printf( "\n>>> SWEEP >>> %s\n", CurrPath() );
    if( !Options.allfiles ) {
        SubstituteAndRun( "" );
        return;
    }
    dirh = opendir( "." );
    if( dirh != NULL ) {
        for( ;; ) {
            if( DoneFlag )
                return;
            dp = readdir( dirh );
            if( dp == NULL )
                break;
#ifdef __UNIX__
            {
                struct stat buf;
                stat( dp->d_name, &buf );
                if( S_ISDIR( buf.st_mode ) )
                    continue;
            }
#else
            if( dp->d_attr & _A_SUBDIR )
                continue;
#endif
            SubstituteAndRun( dp->d_name );
        }
        closedir( dirh );
    }
}


void ProcessCurrentDirectory( void )
{
    DIR                 *dirh;
    struct dirent       *dp;
    dirstack            *stack;

    if( !Options.depthfirst ) {
        ExecuteCommands();
    }
    if( Options.levels != 0 ) {
        dirh = opendir( "." );
        if( dirh != NULL ) {
            --Options.levels;
            for( ;; ) {
                if( DoneFlag )
                    return;
                dp = readdir( dirh );
                if( dp == NULL )
                    break;
#ifdef __UNIX__
                {
                    struct stat buf;
                    stat( dp->d_name, &buf );
                    if( !S_ISDIR( buf.st_mode ) )
                        continue;
                }
#else
                if( !( dp->d_attr & _A_SUBDIR ) )
                    continue;
#endif
                if( dp->d_name[0] == '.' ) {
                    if( dp->d_name[1] == '.' || dp->d_name[1] == '\0' )
                        continue;
                }
                stack = SafeMalloc( sizeof( *stack ) );
                if( DoneFlag )
                    return;
                stack->name_len = strlen( dp->d_name );
                memcpy( stack->name, dp->d_name, stack->name_len + 1 );
                stack->prev = Stack;
                Stack = stack;
                chdir( stack->name );
                ProcessCurrentDirectory();
                chdir( ".." );
                Stack = stack->prev;
                free( stack );
            }
            ++Options.levels;
            closedir( dirh );
        }
    }
    if( Options.depthfirst ) {
        ExecuteCommands();
    }
}


void PrintHelp( void )
{
    int i;

    for( i = 0; Help[i] != NULL; ++i )
        puts( Help[i] );
    exit( EXIT_FAILURE );
}


int GetNumber( int default_num )
{
    int number;

    if( !isdigit( CmdLine[1] ) )
        return( default_num );
    number = 0;
    for( ;; ) {
        if( !isdigit( CmdLine[1] ) )
            return( number );
        number *= 10;
        number += CmdLine[1] - '0';
        ++CmdLine;
    }
}


#ifndef __WATCOMC__
int main( int argc, char **argv ) {
    _argv = argv;
    _argc = argc;
#else
int main( void ) {
#endif

    getcmd( CmdBuff );
    CmdLine = CmdBuff;
    Options.levels = INT_MAX;
    while( *CmdLine == ' ' )
        ++CmdLine;
    if( *CmdLine == '\0' || *CmdLine == '?' )
        PrintHelp();
    for( ;; ) {
        if( *CmdLine != '-' && *CmdLine != '/' )
            break;
        ++CmdLine;
        switch( *CmdLine ) {
        case 'a':
            Options.allfiles = 1;
            break;
        case 'v':
            Options.verbose = 1;
            break;
        case 'p':
            Options.print = 1;
            break;
        case 'n':
            Options.noexec = 1;
            break;
        case 'd':
            Options.depthfirst = 1;
            break;
        case 'l':
            Options.levels = GetNumber( 1 );
            break;
        case '?':
        default:
            PrintHelp();
        }
        ++CmdLine;
        while( *CmdLine == ' ' )
            ++CmdLine;
    }
    Stack = SafeMalloc( sizeof( *Stack ) );
    Stack->name_len = 1;
    Stack->prev = NULL;
    StringCopy( Stack->name, "." );
    getcwd( SaveDir, _MAX_PATH );
    signal( SIGINT, SetDoneFlag );
    ProcessCurrentDirectory();
    free( Stack );
    Stack = NULL;
    chdir( SaveDir );
    return( EXIT_SUCCESS );
}
