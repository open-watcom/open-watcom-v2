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
* Description:  Microsoft LIB clone tool.
*
****************************************************************************/


#include <process.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bool.h"
#include "watcom.h"
#include "cmdline.h"
#include "context.h"
#include "error.h"
#include "file.h"
#include "lib.h"
#include "message.h"
#include "parse.h"
#include "translat.h"
#include "system.h"


#if defined(__TARGET_386__)
    #define LIBRARIAN           "wlib"
#elif defined(__TARGET_AXP__)
    #define LIBRARIAN           "wlib"
#elif defined(__TARGET_PPC__)
    #define LIBRARIAN           "wlib"
#else
    #error Unrecognized CPU type (lib.c)
#endif

#define LIB_SUCCESS             0
#define LIB_ERROR               (-2)


/*
 * Top-level parsing routine.  Returns the number of items parsed.
 */
static int do_parsing( OPT_STORAGE *cmdOpts )
/*******************************************/
{
    int                 itemsParsed = 0;

    /*** Process the WATCOM_CLONE_OPTIONS environment variable ***/
    if( OpenEnvironContext( "WATCOM_CLONE_OPTIONS" )  ==  0 ) {
        CmdStringParse( cmdOpts, &itemsParsed );
    }

    /*** Process the LIB_OPTIONS environment variable ***/
    if( OpenEnvironContext( "LIB_OPTIONS" )  ==  0 ) {
        CmdStringParse( cmdOpts, &itemsParsed );
    }

    /*** Process the command line ***/
    OpenCmdLineContext();
    CmdStringParse( cmdOpts, &itemsParsed );

    return( itemsParsed );
}


/*
 * Spawn the Watcom librarian.  Returns LIB_ERROR if the librarian
 * returned a bad status code or if it could
 * not be spawned, or else LIB_SUCCESS if everything went smoothly.
 */
static int lib( const OPT_STORAGE *cmdOpts, CmdLine *cmdLine )
/************************************************************/
{
    char **             args;
    int                 rc = 0;
    char *              cmdFileName;
    FILE *              fp = NULL;
    int                 count;
    CmdLine *           spawnCmdLine;


    /*** Make the command file ***/
    args = MergeCmdLine( cmdLine, INVALID_MERGE_CMDLINE );
    cmdFileName = tmpnam( NULL );
    if( !cmdOpts->noinvoke ) {
        fp = fopen( cmdFileName, "wt" );
        if( fp == NULL ) {
            FatalError( "Cannot open temporary file '%s' -- aborting", cmdFileName );
        }
    }
    for( count=0; args[count]!=NULL; count++ ) {
        if( !cmdOpts->noinvoke ) {
            fprintf( fp, "%s\n", args[count] );
        }
        if( cmdOpts->showwopts ) {
            fprintf( stderr, "echo.%s%s%s\n",
                args[count], (count == 0 ? ">" : ">>"), cmdFileName );
        }
    }
    if( !cmdOpts->noinvoke ) {
        fclose( fp );
    }

    /*** Spawn the librarian ***/
    spawnCmdLine = InitCmdLine( LIB_NUM_SECTIONS );
    AppendCmdLine( spawnCmdLine, LIB_PROGNAME_SECTION, LIBRARIAN );
    AppendFmtCmdLine( spawnCmdLine, LIB_OPTS_SECTION, "@%s", cmdFileName );
    args = MergeCmdLine( spawnCmdLine, INVALID_MERGE_CMDLINE );
    if( cmdOpts->showwopts ) {
        for( count=0; args[count]!=NULL; count++ ) {
            fprintf( stderr, "%s ", args[count] );
        }
        fprintf( stderr, "\n" );
    }
    if( !cmdOpts->noinvoke ) {
        rc = spawnvp( P_WAIT, LIBRARIAN, (const char **)args );
    }
    if( cmdOpts->showwopts ) {
        fprintf( stderr, "del %s\n", cmdFileName );
    }
    if( !cmdOpts->noinvoke ) {
        remove( cmdFileName );
        if( rc != 0 ) {
            if( rc == -1  ||  rc == 255 ) {
                FatalError( "Unable to execute '%s'", LIBRARIAN );
            } else {
                return( LIB_ERROR );
            }
        }
    }
    DestroyCmdLine( spawnCmdLine );

    return( LIB_SUCCESS );
}


/*
 * Program entry point.
 */
void main( int argc, char *argv[] )
/*********************************/
{
    OPT_STORAGE         cmdOpts;
    CmdLine *           cmdLine;
    int                 itemsParsed;

#ifndef __WATCOMC__
    _argc = argc;
    _argv = argv;
#endif
    /*** Initialize ***/
    SetBannerFuncError( BannerMessage );
    cmdLine = InitCmdLine( LIB_NUM_SECTIONS );
    SetDefaultFile( TYPE_OBJ_FILE, "object or library" );
    AllowTypeFile( TYPE_OBJ_FILE, TYPE_LIB_FILE, TYPE_RES_FILE,
                   TYPE_RBJ_FILE, TYPE_RS_FILE, TYPE_INVALID_FILE );

    /*** Parse the command line and translate to Watcom options ***/
    InitParse( &cmdOpts );
    itemsParsed = do_parsing( &cmdOpts );
    if( itemsParsed == 0 ) {
        PrintHelpMessage();
        exit( EXIT_SUCCESS );
    }
    OptionsTranslate( &cmdOpts, cmdLine );

    /*** Spawn the librarian ***/
    lib( &cmdOpts, cmdLine );
    FiniParse( &cmdOpts );
    exit( EXIT_SUCCESS );
}
