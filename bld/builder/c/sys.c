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
* Description:  System specific functions for builder
*
****************************************************************************/

#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#ifdef __UNIX__
#include <sys/wait.h>
#else
#include <process.h>
#endif
#include "watcom.h"
#include "builder.h"

int SysRunCommand( const char *cmd )
{
    int         my_std_output;
    int         my_std_error;
    int         bytes_read;
    int         rc;
    int         readpipe = -1;
    char        buff[256 + 1];

    my_std_output = dup( STDOUT_FILENO );
    my_std_error = dup( STDERR_FILENO );
    rc = SysRunCommandPipe( cmd, &readpipe );
    dup2( my_std_output, STDOUT_FILENO );
    dup2( my_std_error, STDERR_FILENO );
    close( my_std_output );
    close( my_std_error );
    if( rc == -1 ) {
        if( readpipe != -1 )
        close( readpipe );
        return( rc );
    }
    if( readpipe != -1 ) {
        for( ;; ) {
            bytes_read = read( readpipe, buff, sizeof( buff ) - 1 );
            if( bytes_read == 0 )
            break;
            buff[bytes_read] = '\0';
            Log( Quiet, "%s", buff );
        }
        close( readpipe );
    }
    /* free up the zombie (if there is one) */
    while( wait( &rc ) == -1 && errno == EINTR )
        ;
    return( rc );
}
