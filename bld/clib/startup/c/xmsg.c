/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2020 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Fatal runtime error handler.
*
****************************************************************************/


#include "variety.h"
#if defined( __DOS__ )
    #include "doexitwm.h"
#elif defined( __LINUX__ )
    #include <string.h>
    #include <unistd.h>
    #include "linuxsys.h"
#elif defined( __NETWARE__ )
    #include <string.h>
    #include <io.h>
    #include "nw_lib.h"
#elif defined( __QNX__ )
    #include <unistd.h>
    #include "owqnx.h"
#elif defined( __WINDOWS__ )
    #include <stdlib.h>
    #include <windows.h>
#elif defined( __OS2__ )
    #include <io.h>
    #include <wos2.h>
#elif defined( __RDOS__ )
    #include <stdio.h>
    #include <string.h>
    #include <io.h>
    #include <rdos.h>
#elif defined( __RDOSDEV__ )
#elif defined( __NT__ )
    #include <windows.h>
    #include "iomode.h"
#endif
#include "exitwmsg.h"


#if defined( __LINUX__ )
    #define EXIT(rc)    _sys_exit( rc )
#elif defined( __NETWARE__ )
  #if defined( _NETWARE_CLIB )
    #define EXIT(rc)    ExitThread( 0, rc )
  #else
    #define EXIT(rc)    NXThreadExit( &rc );
  #endif
#elif defined( __QNX__ )
    #define EXIT(rc)    __qnx_exit( rc );
#elif defined( __WINDOWS__ )
    #define EXIT(rc)    _exit( rc )
#else
    #define EXIT(rc)    __exit( rc )
#endif

#if defined( __RDOS__ )
static ThreadState state;
static char FatalErrorStr[256];
#endif

_WCRTLINK _WCNORETURN void __exit_with_msg( char _WCI86FAR *msg, int retcode )
{
#if defined( __DOS__ )
    __do_exit_with_msg( msg, retcode );
    // never return
#else
  #if defined( __LINUX__ )
    char    eol[1];

    write( STDERR_FILENO, msg, strlen( msg ) );
    eol[0] = '\n';
    write( STDERR_FILENO, eol, 1 );
  #elif defined( __NETWARE__ )
    char  eol[2];

    write( STDOUT_FILENO, msg, strlen( msg ) );
    eol[0] = '\r';
    eol[1] = '\n';
    write( STDOUT_FILENO, eol, 2 );
  #elif defined( __OS2__ )
    unsigned    len;
    char        _WCI86FAR *end;
    char        eol[2];

    end = msg;
    for( len = 0; *end++ != '\0'; len++ )
        ;

    #if defined( _M_I86 )

    VioWrtTTY( msg, len, 0 );
    eol[0] = '\r';
    eol[1] = '\n';
    VioWrtTTY( eol, 2, 0 );

    #else

    {
        ULONG       written;

        DosWrite( STDERR_FILENO, msg, len, &written );
        eol[0] = '\r';
        eol[1] = '\n';
        DosWrite( STDERR_FILENO, eol, 2, &written );
    }

    #endif
  #elif defined( __QNX__ )
    char    chr[1];

    while( *msg != '\0' ) {
        chr[0] = *msg++;
        write( STDERR_FILENO, chr, 1 );
    }
    chr[0] = '\n';
    write( STDERR_FILENO, chr, 1 );
  #elif defined( __RDOS__ )
    int     handle;
    char    eol[2];

    handle = RdosGetThreadHandle();
    RdosGetThreadState( handle, &state );
    sprintf( FatalErrorStr, "Fatal error in thread: %04hX %s", handle, state.Name );

    write( STDERR_FILENO, FatalErrorStr, strlen( FatalErrorStr ) );
    eol[0] = '\r';
    eol[1] = '\n';
    write( STDERR_FILENO, eol, 2 );
    write( STDERR_FILENO, msg, strlen( msg ) );
    write( STDERR_FILENO, eol, 2 );

    RdosFatalErrorExit();
  #elif defined( __RDOSDEV__ )

    /* unused parameters */ (void)msg;

  #elif defined( __WINDOWS__ )
    char        tmp[128];
    LPSTR       b;

    b = tmp;
    while( *msg != '\0' && *msg != '\r' && b < tmp + sizeof( tmp ) - 1 )
        *b++ = *msg++;
    *b = '\0';

    MessageBox( (HWND)NULL, tmp, "Open Watcom", MB_OK );
  #elif defined( __NT__ )
    unsigned    len;
    char        *end;
    ULONG       written;
    char        eol[2];

    end = msg;
    for( len = 0; *end++ != '\0'; len++ )
        ;
    WriteFile( NT_STDERR_FILENO, msg, len, &written, NULL );
    eol[0] = '\r';
    eol[1] = '\n';
    WriteFile( NT_STDERR_FILENO, eol, 2, &written, NULL );
  #endif
    EXIT( retcode );
    // never return
#endif
#if defined( __RDOSDEV__ )
    __asm int 3
#endif
}

_WCRTLINK _WCNORETURN void __fatal_runtime_error( char _WCI86FAR *msg, int retcode )
{
#if defined( __RDOS__ )
#elif defined( __RDOSDEV__ )
#else
    if( __EnterWVIDEO( msg ) )
        EXIT( retcode );
        // never return
#endif
    __exit_with_msg( msg, retcode );
    // never return
}
