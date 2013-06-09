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

    size_t  cmdlen  = 0;
    char *  cmdline = NULL;
    int     retval;

    /* Display the banner. */

    print_banner();

    /* Display the usage information if the command line is empty. */

    cmdlen = _bgetcmd( NULL, 0 );
    if( cmdlen == 0 ) {
        print_usage();
        return( EXIT_FAILURE );
    }

    /* Get the command line. */

    cmdlen++; /* Include space for the terminating null character. */
    cmdline = malloc( cmdlen );
    if( cmdline == NULL ) {
        return( EXIT_FAILURE );
    }

    cmdlen = _bgetcmd( cmdline, cmdlen );

    /* Initialize the globals. */

    initialize_globals();
    res_initialize_globals();
    
    /* Parse the command line: allocates and sets tgt_path. */

    retval = parse_cmdline( cmdline );
    if( retval == FAILURE ) {
        free( cmdline );
        return( EXIT_FAILURE );
    }

    /* Free the memory held by cmdline and reset it. */

    free( cmdline );
    cmdline = NULL;

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
