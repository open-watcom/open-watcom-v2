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


#include <stdlib.h>
#include <string.h>
#include "cmdline.h"
#include "error.h"
#include "message.h"
#include "memory.h"
#include "rc.h"
#include "translat.h"

#define UNSUPPORTED_STR_SIZE    512


/*
 * Translate scanned MS options to Watcom options.
 */
void OptionsTranslate( OPT_STORAGE *cmdOpts, CmdLine *cmdLine )
/*************************************************************/
{
    if( cmdOpts->nologo ) {
        QuietModeMessage();
    } else {
        BannerMessage();
    }
    unsupported_opts( cmdOpts );
    default_opts( cmdOpts, cmdLine );
    convert_opts( cmdOpts, cmdLine );
}


/*
 * Parse unsupported options.
 */
static void unsupported_opts( OPT_STORAGE *cmdOpts )
/**************************************************/
{
    char                opts[UNSUPPORTED_STR_SIZE];

    /*** Build a string listing all unsupported options that were used ***/
    opts[0] = '\0';
    if( cmdOpts->c )  append_unsupported( opts, "c" );
    if( cmdOpts->l )  append_unsupported( opts, "l" );
    if( cmdOpts->v )  append_unsupported( opts, "v" );

    /*** If an unsupported option was used, give a warning ***/
    if( opts[0] != '\0' ) {
        UnsupportedOptsMessage( opts );
    }
}


/*
 * Add one more unsupported option to optStr.
 */
static void append_unsupported( char *optStr, char *opt )
/*******************************************************/
{
    if( optStr[0] != '\0' ) {
        strcat( optStr, " /" );
    } else {
        strcat( optStr, "/" );
    }
    strcat( optStr, opt );
}


/*
 * Parse the options.
 */
static void default_opts( OPT_STORAGE *cmdOpts, CmdLine *cmdLine )
{
    OPT_STRING *curr;

    /*** Emit default options if so desired ***/
    if (!cmdOpts->nowopts)
    {
        AppendCmdLine( cmdLine, RC_OPTS_SECTION, "-r" );
        AppendCmdLine( cmdLine, RC_OPTS_SECTION, "-bt=nt");
        AppendCmdLine( cmdLine, RC_OPTS_SECTION, "-d_WIN32");
        AppendCmdLine( cmdLine, RC_OPTS_SECTION, "-q" );
    }

    /*** Add any options meant for the Watcom tools ***/
    if (cmdOpts->passwopts)
    {
        for (curr = cmdOpts->passwopts_value; curr; curr = curr->next)
        {
            AppendCmdLine(cmdLine, RC_OPTS_SECTION, curr->data);
        }
    }
} /* default_opts() */


/*
 * Parse the options.
 */
static void convert_opts( OPT_STORAGE *cmdOpts, CmdLine *cmdLine )
/****************************************************************/
{
    OPT_STRING *        optStr;

    optStr = cmdOpts->d_value;
    while( optStr != NULL ) {
        AppendFmtCmdLine( cmdLine, RC_OPTS_SECTION, "-d%s", optStr->data );
        optStr = optStr->next;
    }

    optStr = cmdOpts->fo_value;
    while( optStr != NULL ) {
        AppendFmtCmdLine( cmdLine, RC_OPTS_SECTION, "-fo%s", optStr->data );
        optStr = optStr->next;
    }

    optStr = cmdOpts->i_value;
    while( optStr != NULL ) {
        AppendFmtCmdLine( cmdLine, RC_OPTS_SECTION, "-i%s", optStr->data );
        optStr = optStr->next;
    }

    if( cmdOpts->x ) {
        AppendCmdLine( cmdLine, RC_OPTS_SECTION, "-x" );
    }
}
