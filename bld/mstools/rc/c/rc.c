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
* Description:  Microsoft RC clone tool.
*
****************************************************************************/


#include <process.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cmdline.h"
#include "context.h"
#include "error.h"
#include "file.h"
#include "message.h"
#include "optparse.h"
#include "parse.h"
#include "rc.h"
#include "translat.h"
#include "system.h"


#if defined(__TARGET_386__)
    #define RESCOMPILER         "wrc"
#elif defined(__TARGET_AXP__)
    #define RESCOMPILER         "wrc"
#elif defined(__TARGET_PPC__)
    #define RESCOMPILER         "wrc"
#else
    #error Unrecognized CPU type
#endif

#define RC_SUCCESS              0
#define RC_NOACTION             (-1)
#define RC_ERROR                (-2)


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

    /*** Process the RC_OPTIONS environment variable ***/
    if( OpenEnvironContext( "RC_OPTIONS" )  ==  0 ) {
        CmdStringParse( cmdOpts, &itemsParsed );
    }

    /*** Process the RC environment variable ***/
    if( OpenEnvironContext( "RC" )  ==  0 ) {
        CmdStringParse( cmdOpts, &itemsParsed );
    }

    /*** Process the command line ***/
    OpenCmdLineContext();
    CmdStringParse( cmdOpts, &itemsParsed );

    return( itemsParsed );
}


/*
 * Spawn the resource compiler.
 */
static int res_compile( const OPT_STORAGE *cmdOpts, CmdLine *cmdLine )
/********************************************************************/
{
    char **             args;
    char *              filename;
    char *              nextFilename;
    int                 rc;
    int                 count;

    /*** Get the name of the .rc file to compiler ***/
    filename = GetNextFile( NULL, TYPE_RC_FILE, TYPE_INVALID_FILE );
    if( filename == NULL )  return( RC_NOACTION );
    nextFilename = GetNextFile( NULL, TYPE_RC_FILE, TYPE_INVALID_FILE );
    if( nextFilename != NULL ) {
        FatalError( "Can only compile one file at a time" );
    }

    /*** Prepare to spawn the resource compiler ***/
    AppendCmdLine( cmdLine, RC_PROGNAME_SECTION, RESCOMPILER );
    AppendCmdLine( cmdLine, RC_FILENAMES_SECTION, filename );
    args = MergeCmdLine( cmdLine, RC_PROGNAME_SECTION, RC_OPTS_SECTION,
                         RC_FILENAMES_SECTION, INVALID_MERGE_CMDLINE );

    /*** Spawn the compiler ***/
    if( cmdOpts->showwopts ) {
        for( count=0; args[count]!=NULL; count++ ) {
            fprintf( stderr, "%s ", args[count] );
        }
        fprintf( stderr, "\n" );
    }
    if( !cmdOpts->noinvoke ) {
        rc = spawnvp( P_WAIT, RESCOMPILER, (const char **)args );
        if( rc != 0 ) {
            if( rc == -1  ||  rc == 255 ) {
                FatalError( "Error executing '%s'", RESCOMPILER );
            } else {
                return( RC_ERROR );
            }
        }
    }
    return( RC_SUCCESS );
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
    int                 rc = RC_NOACTION;

    /*** Initialize ***/
    SetBannerFuncError( BannerMessage );
    cmdLine = InitCmdLine( RC_NUM_SECTIONS );
    SetDefaultFile( TYPE_RC_FILE, "rc" );
    AllowTypeFile( TYPE_RC_FILE, TYPE_INVALID_FILE );

    /*** Parse the command line and translate to Watcom options ***/
    InitParse( &cmdOpts );
    itemsParsed = do_parsing( &cmdOpts );
    if( itemsParsed==0 || cmdOpts.help ) {
        PrintHelpMessage();
        exit( EXIT_SUCCESS );
    }
    OptionsTranslate( &cmdOpts, cmdLine );

    /*** Spawn the compiler ***/
    rc = res_compile( &cmdOpts, cmdLine );
    switch( rc ) {
      case RC_ERROR:
        exit( EXIT_FAILURE );
        break;
      case RC_NOACTION:
        FatalError( "Nothing to do!" );
        break;
      case RC_SUCCESS:
        FiniParse( &cmdOpts );
        exit( EXIT_SUCCESS );
        break;
      default:
        Zoinks();
    }
}
