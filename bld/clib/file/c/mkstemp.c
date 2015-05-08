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
* Description:  Implementation of mkstemp().
*
****************************************************************************/


#include "widechar.h"
#include "variety.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#ifndef __UNIX__
    #include <process.h>
#endif
#include "rtdata.h"

/* Note: This code is similar to _mktemp() but intended for POSIX usage */

static int is_valid_template( char *template, char **xs )
{
    int                 len;
    char                *p;

    /*** Ensure the last 6 characters form the string "XXXXXX" ***/
    len = strlen( template );
    if( len < 6 ) {
        return( 0 );        /* need room for six exes */
    }
    p = template + len - 6;
    if( strcmp( p, "XXXXXX" ) ) {
        return( 0 );
    }
    *xs = p;

    return( 1 );
}


_WCRTLINK int mkstemp( char *template )
{
    char                letter;
    unsigned            pid;
    char                *xs;
    int                 fh;

    /*** Ensure the template is valid ***/
    if( !is_valid_template( template, &xs ) ) {
        return( -1 );
    }

    /*** Get the process/thread ID ***/
    pid = getpid();
    pid %= 100000;      /* first few digits could be repeated */

    /*** Try to build a unique filename ***/
    for( letter = 'a'; letter <= 'z'; letter++ ) {
        snprintf( xs, strlen( xs ) + 1, "%c%05u", letter, pid );
        if( access( template, F_OK ) != 0 ) {
            fh = open( template,
                       O_RDWR | O_CREAT | O_TRUNC | O_EXCL | O_BINARY,
                       S_IRUSR | S_IWUSR );
            if( fh != -1 ) {
                return( fh );       /* file successfully created */
            }
            /* EEXIST may occur in case of a race condition or if we simply
             * created that temp file earlier, and we'll try again. If however
             * the creation failed for some other reason, it will almost
             * certainly fail again no matter how many times we try. So don't.
             */
            if( _RWD_errno != EEXIST ) {
                return( -1 );
            }
        }
    }
    return( -1 );
}
