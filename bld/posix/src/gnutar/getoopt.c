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
* Description:  Plug-compatible replacement for getopt()
*                for parsing tar-like
*
****************************************************************************/


/*
 * Plug-compatible replacement for getopt() for parsing tar-like
 * arguments.  If the first argument begins with "-", it uses getopt;
 * otherwise, it uses the old rules used by tar, dump, and ps.
 *
 * Written 25 August 1985 by John Gilmore (ihnp4!hoptoad!gnu) and placed
 * in the Pubic Domain for your edification and enjoyment.
 * MS-DOS port 2/87 by Eric Roskos.
 * Minix  port 3/88 by Eric Roskos.
 *
 * @(#)getoldopt.c 1.4 2/4/86 Public Domain - gnu
 */

#include <stdio.h>
#if defined( __WATCOMC__ ) || defined( __UNIX__ )
#include <unistd.h>
#endif
#include "getoopt.h"    /* local copy of getopt */
#include "port.h"
#include "clibext.h"

int getoldopt( int argc, char **argv, char *optstring )
{
        static char    *key;            /* Points to next keyletter */
        static char    use_getopt;     /* !=0 if argv[1][0] was '-' */
        char            c;
        char            *place;

        optarg = NULL;

        if (key == NULL)
        {                                                       /* First time */
                if (argc < 2)
                        return EOF;
                key = argv[1];
                if (*key == '-')
                        use_getopt++;
                else
                        optind = 2;
        }

        if (use_getopt)
                return getopt(argc, argv, optstring);

        c = *key++;
        if (c == '\0')
        {
                key--;
                return EOF;
        }
        place = index(optstring, c);

        if (place == NULL || c == ':')
        {
                fprintf(stderr, "%s: unknown option %c\n", argv[0], c);
                return ('?');
        }

        place++;
        if (*place == ':')
        {
                if (optind < argc)
                {
                        optarg = argv[optind];
                        optind++;
                }
                else
                {
                        fprintf(stderr, "%s: %c argument missing\n",
                                argv[0], c);
                        return ('?');
                }
        }

        return (c);
}
