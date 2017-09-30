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
* Description:  Fatal runtime error handler for RDOS.
*
****************************************************************************/


#include "variety.h"
#include <rdos.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "iomode.h"
#include "exitwmsg.h"
#include "rtdata.h"

static ThreadState state;
static char FatalErrorStr[256];

_WCRTLINK _WCNORETURN void __exit_with_msg( char *msg, unsigned retcode )
{
    int handle = RdosGetThreadHandle();
    RdosGetThreadState( handle, &state );
    sprintf( FatalErrorStr, "Fatal error in thread: %04hX %s", handle, state.Name );

    write( STDERR_FILENO, FatalErrorStr, strlen( FatalErrorStr ) );
    write( STDERR_FILENO, "\r\n", 2 );
    write( STDERR_FILENO, msg, strlen( msg ) );
    write( STDERR_FILENO, "\r\n", 2 );

    RdosFatalErrorExit();
    __exit( retcode );
    // never return
}

_WCRTLINK _WCNORETURN void __fatal_runtime_error( char *msg, unsigned retcode )
{
    __exit_with_msg( msg, retcode );
    // never return
}
