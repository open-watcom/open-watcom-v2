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
* Description:  Real-mode stub program to start up protect-mode compilers.
*
****************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <process.h>
#include <errno.h>
#include <string.h>

int     __near  _fmode;

main( int argc, char *argv[] )
{
    const char  *av[3];
    auto char   cmdline[128];

    av[0] = PGM_NAME;                   /* name of executable to run */
    av[1] = getcmd( cmdline );          /* command line */
    av[2] = NULL;                       /* end of list */
    execvp( av[0], av );
    cputs( "Unable to start '" );
    cputs( av[0] );
    cputs( "': " );
    cputs( strerror( errno ) );
    cputs( "\r\n" );
    exit( 1 );                  /* indicate error */
}


int puts(const char *msg )          /* override function in library */
{
    if (cputs( msg ) != 0)
      return EOF;
    return strlen(msg);             /* Newline not appended by cputs */
}
