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
* Description:  Fatal runtime error handler for 16-bit OS/2.
*
****************************************************************************/


#include "variety.h"
#include <wos2.h>
#include "rtdata.h"
#include "exitwmsg.h"


_WCRTLINK _NORETURN void __exit_with_msg( char _WCI86FAR *msg, unsigned retcode )
/*******************************************************************************/
{
    unsigned    len;
    char        _WCI86FAR *end;
    char        newline[2];

    end = msg;
    for( len = 0; *end++ != '\0'; len++ )
        ;
    VioWrtTTY( msg, len, 0 );
    newline[0] = '\r';
    newline[1] = '\n';
    VioWrtTTY( &newline, 2, 0 );
    __exit( retcode );
    // never return
}

_WCRTLINK void __fatal_runtime_error( char _WCI86FAR *msg, unsigned retcode )
/***************************************************************************/
{
    if( __EnterWVIDEO( msg ) )
        __exit( retcode );
    __exit_with_msg( msg, retcode );
    // never return
}
