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
#include "asaxp.h"
#include "message.h"
#include "memory.h"
#include "translat.h"
#include "system.h"

#define UNSUPPORTED_STR_SIZE    512


/*
 * Translate scanned MS options to Watcom options.
 */
void OptionsTranslate( OPT_STORAGE *cmdOpts, CmdLine *cmdLine )
/*************************************************************/
{
    /*** Parse the /nologo switch now so we can print the banner ***/
    if( cmdOpts->nologo ) {
        QuietModeMessage();
    } else {
        BannerMessage();
    }

    if( cmdOpts->help || cmdOpts->_ ) {
        PrintHelpMessage();
        exit( EXIT_SUCCESS );

    }

    /*** Parse everything ***/
    default_opts( cmdOpts, cmdLine );
    unsupported_opts( cmdOpts );
    asaxp_opts( cmdOpts, cmdLine );
    watcom_opts( cmdOpts, cmdLine );
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
    if( cmdOpts->nopp                )  append_unsupported( opts, "nopp"                );
    if( cmdOpts->O0                  )  append_unsupported( opts, "O0"                  );
    if( cmdOpts->O1                  )  append_unsupported( opts, "O1"                  );
    if( cmdOpts->resumptionsafe      )  append_unsupported( opts, "resumptionsafe"      );
    if( cmdOpts->symbolsaligned0mod4 )  append_unsupported( opts, "symbolsaligned0mod4" );
    if( cmdOpts->symbolsnotaligned   )  append_unsupported( opts, "symbolsnotaligned"   );
    if( cmdOpts->stackaligned0mod8   )  append_unsupported( opts, "stackaligned0mod8"   );
    if( cmdOpts->stacknotaligned     )  append_unsupported( opts, "stacknotaligned"     );
    if( cmdOpts->eflag               )  append_unsupported( opts, "eflag"               );
    if( cmdOpts->QApdst              )  append_unsupported( opts, "QApdst"              );
    if( cmdOpts->QApdsg              )  append_unsupported( opts, "QApdsg"              );
    if( cmdOpts->QApdsa              )  append_unsupported( opts, "QApdsa"              );
    if( cmdOpts->QApdie              )  append_unsupported( opts, "QApdie"              );
    if( cmdOpts->QApdca              )  append_unsupported( opts, "QApdca"              );
    if( cmdOpts->u                   )  append_unsupported( opts, "U"                   );
    if( cmdOpts->Zd                  )  append_unsupported( opts, "Zd"                  );
    if( cmdOpts->Zi                  )  append_unsupported( opts, "Zi"                  );
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
 * Parse ASAXP options, targets and macros.
 */
static void asaxp_opts( OPT_STORAGE *cmdOpts, CmdLine *cmdLine )
/************************************************************/
{
    OPT_STRING *        optStr;

    optStr = cmdOpts->d_value;
    while( optStr != NULL ) {
        AppendFmtCmdLine( cmdLine, ASAXP_OPTS_SECTION, "/d%s ", optStr->data );
        optStr = optStr->next;
    }

    if( cmdOpts->fo || cmdOpts->o) {
        AppendFmtCmdLine( cmdLine, ASAXP_OPTS_SECTION, "/fo=%s ", cmdOpts->fo_value->data );
    }

    optStr = cmdOpts->i_value;
    while( optStr != NULL ) {
        AppendFmtCmdLine( cmdLine, ASAXP_OPTS_SECTION, "/i=%s ", optStr->data );
        optStr = optStr->next;
    }

    /* transfer stored filenames*/
    optStr = cmdOpts->t010101010101_value;
    while( optStr != NULL ) {
        AppendFmtCmdLine( cmdLine, ASAXP_OPTS_SECTION, "%s", optStr->data );
        optStr = optStr->next;
    }


}


/*
 * Activate options which are always to be turned on.
 */
static void default_opts( OPT_STORAGE *cmdOpts, CmdLine *cmdLine )
/**************************************************************/
{
    if (!cmdOpts->nowopts) {
        AppendCmdLine( cmdLine, ASAXP_OPTS_SECTION, "/oc /zq" );
    }
}


/*
 * Add any options meant for the Watcom tools.
 */
static void watcom_opts( OPT_STORAGE *cmdOpts, CmdLine *cmdLine )
/**************************************************************/
{
    OPT_STRING *curr;

    if (cmdOpts->passwopts)
    {
        for (curr = cmdOpts->passwopts_value; curr; curr = curr->next)
        {
            AppendCmdLine(cmdLine, ASAXP_OPTS_SECTION, curr->data);
        }
    }

}
