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
* Description:  Platform independent abort() implementation.
*
****************************************************************************/


#include "variety.h"
#include <stdio.h>
#include <stdlib.h>
#include "rtdata.h"
#include "exitwmsg.h"
#include <signal.h>
#include <unistd.h>


void    (*_RWD_abort)( void ) = __terminate;


// TODO: Use the QNX code once we get signal handling working properly

#if defined(__QNX__)
/*
 * abort() the u**x way
 */
#include "initfini.h"

_WCRTLINK void abort( void )
{
    struct sigaction    oact;
    sigset_t            mask;

    sigaction( SIGABRT, NULL, &oact );
    if( oact.sa_handler == SIG_DFL ) {
                                /* '0' is not the right value here */
        __FiniRtns( 0, 255 );   /* get the I/O system shut down */
    }
    sigfillset( &mask );
    sigdelset( &mask, SIGABRT );
    sigprocmask( SIG_SETMASK, &mask, (sigset_t *)NULL );
    raise( SIGABRT );
    signal( SIGABRT,SIG_DFL );
    raise( SIGABRT );
    __terminate();
}
#else

_WCRTLINK void abort( void )
{
    if( _RWD_abort != __terminate ) {
        (*_RWD_abort)();
    }
    __terminate();                          /* 23-may-90 */
}

#endif

void __terminate( void )
{
    __fatal_runtime_error( "ABNORMAL TERMINATION", EXIT_FAILURE );
}
