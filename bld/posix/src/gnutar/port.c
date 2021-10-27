/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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


/*
 * @(#)port.c 1.6       86/08/11        Public Domain, by John Gilmore, 1986
 * MS-DOS port 2/87 by Eric Roskos.
 * Minix port  3/88 by Eric Roskos.
 *
 * These are routines not available in all environments.
 *
 * The following comment is by John Gilmore, I didn't write it!  Though
 * I did bowdlerize it a bit. :-)  --E.R.:
 *
 * I know this introduces an extra level of subroutine calls and is
 * slightly slower.  Frankly, my dear, I don't give a flying whooey.  Let the
 * Missed-Em Vee losers suffer a little.  This software is proud to
 * have been written on a BSD system.
 *
 * (Obviously a totally unbiased viewpoint, coming from John... :-)
 * This opinion is not shared by JER, who finds merit in both systems.)
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>
#include <errno.h>
#include <stdarg.h>
#include "wio.h"
#include "port.h"

/*
 * These first routines (whose names start with 'u') were written to
 * handle I/O that occurs between the program and the user portably.
 * The reason for this is that Minix's stdio package doesn't support
 * 2-directional I/O (e.g., "r+", "w+").  So these routines operate on
 * plain Unix-type file descriptors instead of FILE pointers. -- JER
 */

int ugetc( int f )
{
    char c;

    if( read( f, &c, 1 ) != 1 )
        return( EOF );

    return( c );
}

#define UPRBUFSIZ 256

void uprintf( int z, char * fmt, ... )
{
    char        *buf;
    va_list     args;
    int         buflen;

    buf = malloc( UPRBUFSIZ );
    if( buf == NULL ) {
        fprintf( stderr, "uprintf: out of memory\n" );
        exit( -1 );
    }

    va_start( args, fmt );
    vsprintf( buf, fmt, args );
    va_end( args );
    if( strlen( buf ) + 1 > UPRBUFSIZ ) {
        fprintf( stderr, "uprintf: overflowed buffer.\n" );
        exit( -1 );
    }

    buflen = (int)strlen( buf );
    if( write( z, buf, buflen ) != buflen ) {
        sprintf( buf, "uprintf: fd %d", z );
        perror( buf );
    }

    free( buf );
}

#if 0
char *  ugets(char *buf, int siz, int f)
{
        int size;
        size = read(f, buf, siz);
        if( buf[size] == '\n') {
                buf[size] == '\0';
        }
}
#endif

#ifndef BSD42

#ifndef MSDOS
/*
**                              NMKDIR.C
**
** Written by Robert Rother, Mariah Corporation, August 1985.
**
** I wrote this out of shear disgust with myself because I couldn't
** figure out how to do this in /bin/sh.
**
** If you want it, it's yours.  All I ask in return is that if you
** figure out how to do this in a Bourne Shell script you send me
** a copy.
**                                      sdcsvax!rmr or rmr@uscd
*
* Severely hacked over by John Gilmore to make a 4.2BSD compatible
* subroutine.   11Mar86; hoptoad!gnu
*/

/*
 * Make a directory.  Compatible with the mkdir() system call on 4.2BSD.
 */
int mkdir( char *dpath, mode_t dmode )
{
        int             cpid, status;
        mode_t          umaskval;

        switch (cpid = fork())
        {

        case -1:                                        /* Error in fork() */
                return (-1);                    /* Errno is set already */

        case 0:                                 /* Child process */

                /*
                 * Cheap hack to set mode of new directory.  Since this child process
                 * is going away anyway, we zap its umask. FIXME, this won't suffice
                 * to set SUID, SGID, etc. on this directory.  Does anybody care?
                 */
                umaskval = umask(0);              /* Get current umask */
                umaskval = umask(umaskval | (0777 & ~dmode));               /* Set for mkdir */
                execl("/bin/mkdir", "mkdir", dpath, (char *) 0);
#ifdef V7
                exit(-1);
#else
                _exit(-1);                              /* Can't exec /bin/mkdir */
#endif

        default:                                        /* Parent process */
                while (cpid != wait(&status));  /* Wait for kid to finish */
        }

        if (TERM_SIGNAL(status) != 0 || TERM_VALUE(status) != 0)
        {
                errno = EIO;                    /* We don't know why, but */
                return -1;                              /* /bin/mkdir failed */
        }

        return 0;
}

#endif                                                  /* MSDOS */
#endif
