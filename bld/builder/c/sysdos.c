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
* Description:  DOS specific functions for builder.
*
****************************************************************************/


#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <dos.h>
#include <process.h>
#include "builder.h"


void SysInit( int argc, char *argv[] )
{
    argc = argc;
    argv = argv;
    setenv( "BLD_HOST", "DOS", 1 );
}

int SysRunCommandPipe( const char *cmd, int *readpipe )
{
    /* no pipes for DOS so we call spawn with P_WAIT and hence cannot log */
    char        *cmdnam = strdup( cmd );
    char        *sp = strchr( cmdnam, ' ' );
    int         rc;

    if( sp != NULL ) {
        *sp = '\0';
        sp++;
    }
    rc = spawnlp( P_WAIT, cmdnam, cmdnam, sp, NULL );
    *readpipe = -1;
    free( cmdnam );
    return rc;
}

int SysChdir( char *dir )
{
    return SysDosChdir( dir );
}

int wait( int *status )
{
    return 0;
}
