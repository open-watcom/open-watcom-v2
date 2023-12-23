/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2023 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Checks a gendev/wgml library for unusual type designators.
*
****************************************************************************/

#include <process.h>
#include <stdlib.h>

#include "cfcheck.h"
#include "common.h"
#include "research.h"
#include "clibext.h"

/*  Function main().
 *  Given a valid directory path, check all files in that directory
 *  for various conditions:
 *      whether the length is a multiple of 16.
 *      whether the file type byte is 0x02, 0x0, 0x04 or something unexpected.
 *  The actual checks are performed in the function check_directory(); main()
 *  is concerned with overall program architecture, not details.
 *
 *  Returns:
 *      EXIT_FAILURE or EXIT_SUCCESS, as appropriate.
 */

int main( void )
{
    /* Declare automatic variables. */

    int     cmd_len;
    char *  cmd_line;
    int     retval;

    /* Display the banner. */

    print_banner();

    /* Get the command line. */

    cmd_len = _bgetcmd( NULL, 0 ) + 1;
    cmd_line = malloc( cmd_len );
    if( cmd_line == NULL ) {
        return( EXIT_FAILURE );
    }
    _bgetcmd( cmd_line, cmd_len );

    /* Display the usage information if the command line is empty. */

    if( *cmd_line == '\0' ) {
        free( cmd_line );
        print_usage();
        return( EXIT_FAILURE );
    }

    /* Initialize the globals. */

    initialize_globals();
    res_initialize_globals();

    /* Parse the command line: allocates and sets tgt_path. */

    retval = parse_cmdline( cmd_line );

    /* Free the memory held by cmdline and reset it. */

    free( cmd_line );
    cmd_line = NULL;
    if( retval == FAILURE ) {
        return( EXIT_FAILURE );
    }

    /* Check all files in current directory. */

    retval = check_directory();

    /* Free the memory held by tgt_path and reset it. */

    free( tgt_path );
    tgt_path = NULL;

    /* Display the usage if appropriate. */

    if( retval == FAILURE ) {
        print_usage();
        return( EXIT_FAILURE );
    }

    return( EXIT_SUCCESS );
}
