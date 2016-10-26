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
* Description:  Fatal runtime error handler for NetWare.
*
****************************************************************************/


#include "variety.h"
#include <string.h>
#include <io.h>
#include "exitwmsg.h"

#if defined (_NETWARE_CLIB)
extern void             ExitThread( int,int );
#endif
#if defined (_NETWARE_LIBC)
extern void             NXThreadExit( void *);
#endif

_WCRTLINK _NORETURN void __exit_with_msg( char *msg, unsigned retcode )
{
    char    newline[2];

    write( STDOUT_FILENO, msg, strlen( msg ) );
    newline[0] = '\r';
    newline[1] = '\n';
    write( STDOUT_FILENO, &newline, 2 );
#if defined (_NETWARE_CLIB)
    ExitThread( 0, retcode );
#else
    NXThreadExit(&retcode);
#endif
    // never return
}

_WCRTLINK void __fatal_runtime_error( char *msg, unsigned retcode )
{
    if( __EnterWVIDEO( msg ) )
#if defined (_NETWARE_CLIB)
        ExitThread( 0, retcode );
#else
        NXThreadExit( &retcode );
#endif
    __exit_with_msg( msg, retcode );
    // never return
}
