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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#include <process.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cmdline.h"
#include "context.h"
#include "error.h"
#include "asaxp.h"
#include "message.h"
#include "parse.h"
#include "translat.h"
#include "system.h"


#if defined(__TARGET_386__)
    #define ASM         "wasaxp"
#elif defined(__TARGET_AXP__)
    #define ASM         "wasaxp"
#elif defined(__TARGET_PPC__)
    #define ASM         "wasaxp"
#else
    #error Unrecognized CPU type
#endif

#define ASAXP_SUCCESS           0
#define ASAXP_ERROR             (-2)


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

    /*** Process the ASAXP_OPTIONS environment variable ***/
    if( OpenEnvironContext( "ASAXP_OPTIONS" )  ==  0 ) {
        CmdStringParse( cmdOpts, &itemsParsed );
    }

    /*** Process the ASAXP environment variable ***/
    if( OpenEnvironContext( "ASAXP" )  ==  0 ) {
        CmdStringParse( cmdOpts, &itemsParsed );
    }

    /*** Process the command line ***/
    OpenCmdLineContext();
    CmdStringParse( cmdOpts, &itemsParsed );

    return( itemsParsed );
}


/*
 * Spawn the Watcom wasaxp.  Returns ASAXP_ERROR if the wmake
 * returned a bad status code or if it could
 * not be spawned, or else ASAXP_SUCCESS if everything went smoothly.
 */
static int asaxp( const OPT_STORAGE *cmdOpts, CmdLine *cmdLine )
/**************************************************************/
{
    char **             args;
    int                 rc;
    int                 count;

    /*** merge commands ***/
    AppendCmdLine( cmdLine, ASAXP_PROGNAME_SECTION, ASM );
    args = MergeCmdLine( cmdLine, INVALID_MERGE_CMDLINE );

    /*** Spawn the wasaxp ***/
    if( cmdOpts->showwopts ) {
        for( count=0; args[count]!=NULL; count++ ) {
            fprintf( stderr, "%s ", args[count] );
        }
        fprintf( stderr, "\n" );
    }
    if( !cmdOpts->noinvoke ) {
        rc = spawnvp( P_WAIT, ASM, (const char **)args );
        if( rc != 0 ) {
            if( rc == -1  ||  rc == 255 ) {
                FatalError( "Unable to execute '%s'", ASM );
            } else {
                return( ASAXP_ERROR );
            }
        }
    }
    DestroyCmdLine( cmdLine );

    return( ASAXP_SUCCESS );
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

    /*** Initialize ***/
    SetBannerFuncError( BannerMessage );
    cmdLine = InitCmdLine( ASAXP_NUM_SECTIONS );

    /*** Parse the command line and translate to Watcom options ***/
    InitParse( &cmdOpts );
    itemsParsed = do_parsing( &cmdOpts );
    if( itemsParsed == 0 ) {
        PrintHelpMessage();
        exit( EXIT_SUCCESS );
    }
    OptionsTranslate( &cmdOpts, cmdLine );

    /*** Spawn the assembler ***/
    asaxp( &cmdOpts, cmdLine );
    FiniParse( &cmdOpts );
    exit( EXIT_SUCCESS );
}
