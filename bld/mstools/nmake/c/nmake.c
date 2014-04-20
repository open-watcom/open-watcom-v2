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
* Description:  Microsoft NMAKE clone tool.
*
****************************************************************************/


#include <process.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <direct.h>
#include "bool.h"
#include "watcom.h"
#include "cmdline.h"
#include "context.h"
#include "error.h"
#include "nmake.h"
#include "message.h"
#include "parse.h"
#include "translat.h"
#include "system.h"


#define MAKE                    "wmake"

#define NMAKE_SUCCESS           0
#define NMAKE_ERROR             (-2)


/*
 * Top-level parsing routine.  Returns the number of items parsed.
 */
static int do_parsing( OPT_STORAGE *cmdOpts )
/*******************************************/
{
    int                 itemsParsed = 0;

    /*** Process the NMAKE_OPTIONS environment variable ***/
    if( OpenEnvironContext( "NMAKE_OPTIONS" )  ==  0 ) {
        CmdStringParse( cmdOpts, &itemsParsed );
    }

    /*** Process the NMAKE environment variable ***/
    if( OpenEnvironContext( "NMAKE" )  ==  0 ) {
        CmdStringParse( cmdOpts, &itemsParsed );
    }

    /*** Process the command line ***/
    OpenCmdLineContext();
    CmdStringParse( cmdOpts, &itemsParsed );

    return( itemsParsed );
}


/*
 * Spawn the Watcom wmake.  Returns NMAKE_ERROR if wmake returned a bad
 * status code or if it could not be spawned, or else NMAKE_SUCCESS if
 * everything went smoothly.
 */
static int nmake( const OPT_STORAGE *cmdOpts, CmdLine *cmdLine )
/**************************************************************/
{
    char **             args;
    int                 rc;
    int                 count;
    char *              cwd;
    char                flagstmp[32] = {0};

    /*** get value for MAKEDIR field ***/
    cwd = getcwd( NULL, 0 );

    /*** construct MAKEFLAGS field ***/
    if( cmdOpts->a )      strcat(flagstmp, "A");
    if( cmdOpts->c )      strcat(flagstmp, "C");
    if( cmdOpts->d )      strcat(flagstmp, "D");
    if( cmdOpts->e )      strcat(flagstmp, "E");
    if( cmdOpts->nologo ) strcat(flagstmp, "L");
    if( cmdOpts->n )      strcat(flagstmp, "N");
    if( cmdOpts->p )      strcat(flagstmp, "P");
    if( cmdOpts->r )      strcat(flagstmp, "R");
    if( cmdOpts->s )      strcat(flagstmp, "S");
    if( cmdOpts->u )      strcat(flagstmp, "U");
    if( cmdOpts->y )      strcat(flagstmp, "Y");

    /*** pass builtin macros to wmake, so nmake wrapper gets called in recursive actions ***/
    AppendFmtCmdLine( cmdLine, NMAKE_OPTS_SECTION, "MAKE=\"%s\"", "nmake" );
    AppendFmtCmdLine( cmdLine, NMAKE_OPTS_SECTION, "MAKEDIR=\"%s\"", cwd );
    AppendFmtCmdLine( cmdLine, NMAKE_OPTS_SECTION, "MAKEFLAGS=\"%s\"", flagstmp );

    /*** merge commands ***/
    AppendCmdLine( cmdLine, NMAKE_PROGNAME_SECTION, MAKE );
    args = MergeCmdLine( cmdLine, INVALID_MERGE_CMDLINE );

    /*** Spawn the wmake ***/
    if( cmdOpts->showwopts ) {
        for( count=0; args[count]!=NULL; count++ ) {
            fprintf( stderr, "%s ", args[count] );
        }
        fprintf( stderr, "\n" );
    }
    if( !cmdOpts->noinvoke ) {
        rc = spawnvp( P_WAIT, MAKE, (const char **)args );
        if( rc != 0 ) {
            if( rc == -1  ||  rc == 255 ) {
                FatalError( "Unable to execute '%s'", MAKE );
            } else {
                return( NMAKE_ERROR );
            }
        }
    }
    DestroyCmdLine( cmdLine );

    return( NMAKE_SUCCESS );
}


/*
 * Program entry point.
 */
void main( int argc, char *argv[] )
/*********************************/
{
    OPT_STORAGE         cmdOpts;
    CmdLine *           cmdLine;


#ifndef __WATCOMC__
    _argc = argc;
    _argv = argv;
#endif
    /*** Initialize ***/
    SetBannerFuncError( BannerMessage );
    cmdLine = InitCmdLine( NMAKE_NUM_SECTIONS );

    /*** Parse the command line and translate to Watcom options ***/
    InitParse( &cmdOpts );
    do_parsing( &cmdOpts );
    OptionsTranslate( &cmdOpts, cmdLine );

    /*** Spawn the librarian ***/
    nmake( &cmdOpts, cmdLine );
    FiniParse( &cmdOpts );
    exit( EXIT_SUCCESS );
}
