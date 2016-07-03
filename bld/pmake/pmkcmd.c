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
* Description:  Standalone pmake utility.
*
****************************************************************************/


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#if defined( __WATCOMC__ ) || !defined( __UNIX__ )
    #include <process.h>
#endif
#ifndef __UNIX__
    #include <direct.h>
    #include <dos.h>
#endif
#include "wio.h"
#include "bool.h"
#include "watcom.h"
#include "pmake.h"

#include "clibext.h"
#include "clibint.h"


#if defined( __OS2__ )
#define TMPBAT "tmp.cmd"
#elif defined( __UNIX__ )
#define TMPBAT "./tmp.sh"
#else
#define TMPBAT "tmp.bat"
#endif

static char     buffer[512];

void PMakeOutput( char *out )
{
    puts( out );
}

static void WriteCmdFile( pmake_data *data )
{
    FILE        *fp;
    pmake_list  *curr;

    fp = fopen( TMPBAT, "w+t" );
    if( fp == NULL ) {
        printf( "PMAKE: unable to open %s for writing: %s\n",
            TMPBAT, strerror( errno ) );
        exit( EXIT_FAILURE );
    }
#ifdef __UNIX__
    fprintf( fp, "#!/bin/sh\n" );
    fprintf( fp, "rm "TMPBAT"\n" );
    fchmod( fileno( fp ), 0777 );
#else
    fprintf( fp, "@echo off\n" );
#endif
    for( curr = data->dir_list; curr != NULL; curr = curr->next ) {
        if( curr->dir_name[0] != '\0' ) {
            fprintf( fp, "cd %s\n", curr->dir_name );
            if( data->display ) {
                fprintf( fp, "cdsay .\n" );
            }
        }
        PMakeCommand( data, buffer );
        fprintf( fp, "%s\n", buffer );
    }
    getcwd( buffer, sizeof( buffer ) );
    fprintf( fp, "cd %s\n", buffer );
    if( fclose( fp ) ) {
        printf( "PMAKE: unable to close %s: %s\n",
            TMPBAT, strerror( errno ) );
        exit( EXIT_FAILURE );
    }
}

static int RunCommand( const char *cmd )
{
    const char  *p;
    char        *sp;
    char        c;
    const char  **argv;
    int         i;
    bool        skip_sp;

    skip_sp = true;
    i = 1;
    for( p = cmd; *p != '\0'; ++p ) {
        if( *p == ' ' ) {
            skip_sp = true;
        } else if( skip_sp ) {
            ++i;
            skip_sp = false;
        }
    }
    argv = (const char **)malloc( i * sizeof( char * ) + strlen( cmd ) + 1 );
    if( argv == NULL )
        return( 1 );    // error no memory
    sp = (char *)argv + i * sizeof( char * );
    skip_sp = true;
    i = 0;
    for( p = cmd; (c = *p) != '\0'; ++p ) {
        if( c == ' ' ) {
            if( skip_sp )
                continue;
            skip_sp = true;
            c = '\0';
        } else if( skip_sp ) {
            skip_sp = false;
            argv[i++] = sp;
        }
        *sp++ = c;
    }
    *sp = '\0';
    argv[i] = NULL;
    i = (int)spawnvp( P_WAIT, (char *)argv + i * sizeof( char * ), argv );
    free( argv );
    return( i );
}

static int DoPMake( pmake_data *data )
{
    pmake_list  *curr;
    int         res;
    int         rc;

    res = 0;
    for( curr = data->dir_list; curr != NULL; curr = curr->next ) {
        rc = chdir( curr->dir_name );
        if( rc != 0 ) {
            res = rc;
            break;
        }
        if( data->display ) {
            fputs( "==== ", stdout );
            puts( curr->dir_name );
        }
        PMakeCommand( data, buffer );
        rc = RunCommand( buffer );
        if( rc != 0 ) {
            res = rc;
            if( data->ignore_errors == 0 ) {
                break;
            }
        }
    }
    return( res );
}

static int ProcPMake( pmake_data  *data )
{
    int         res;
    char        save[_MAX_PATH];

    getcwd( save, sizeof( save ) );
    res = DoPMake( data );
    chdir( save );
    return( res );
}

static char    *Help[] = {
"Usage: pmake [options] [targ_list] [make options]",
"     Execute 'wmake' in each subdirectory of the current working",
"     directory that has a makefile which contains the specified ",
"     'targ_list' in the format: '#pmake[/<priority>]: <target>*'",
"",
"Options:",
"     -b create batch file -- without executing, file is " TMPBAT,
"     -d display -- display progress in batch file",
"     -ffilename -- use specified filename for makefile",
"     -i ignore -- ignore commands return code",
"     -ln level n -- only descend n directory levels, n defaults to 1",
"     -mfile -- use specified file for make command",
"     -o optimize -- minimize cd commands",
"     -r reverse -- top down directory traversal",
"     -v verbose mode -- print directory headers",
"",
"Walk order:",
"     Subdirectories are visited in depthfirst traversal order",
"     unless changed by the -r option or the <priority> value.",
"     <priority> is an optional integer value following the 'pmake'",
"     keyword. Makefiles with lower <priority> are always visited first.",
"     The default <priority> is 100.",
"",
"Target list:",
"     The list of targets on the command line may contain the following",
"     operators (evaluated using RPN notation):",
"       .and  - logically and the top two elements of the expression stack",
"       .or   - logically or the top two elements of the expression stack",
"       .not  - logically not the top element of the expression stack",
"       all   - is always true (this is the default target)",
"     If there is more than one element left on the expression stack at",
"     at the end, the elements are all 'anded' together.",
"",
"Make options:",
"     Make options start with the first - or / prefixed option after",
"     the target list. (-- or // indicates no target list)",
"",
    NULL
};

static void PrintHelp( void )
{
    int         i;

    for( i = 0; Help[i] != NULL; ++i )
        puts( Help[i] );
    exit( EXIT_FAILURE );
}

static char     CmdBuff[512];

#if !defined( __WATCOMC__ )
int main( int argc, char **argv )
{
#else
int main( void )
{
#endif
    pmake_data  *data;
    int         rc;

#if !defined( __WATCOMC__ )
    _argv = argv;
    _argc = argc;
#endif
    getcmd( CmdBuff );
    data = PMakeBuild( CmdBuff );
    if( data == NULL ) {
        exit( EXIT_FAILURE );
    }
    if( data->want_help ) {
        PrintHelp();
    }
    if( data->want_help || data->signaled ) {
        PMakeCleanup( data );
        exit( EXIT_FAILURE );
    }
    /* If -b was given, only write out a batch file. By default,
     * execute the commands directly.
     */
    if( data->batch ) {
        WriteCmdFile( data );
        rc = 0;
    } else {
        rc = ProcPMake( data );
    }

    PMakeCleanup( data );
    return( rc );
}
