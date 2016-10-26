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
* Description:  Fatal runtime error handler for Windows.
*
****************************************************************************/


#include "variety.h"
#include <stdlib.h>
#include <windows.h>
#include "rtdata.h"
#include "exitwmsg.h"


_WCRTLINK _NORETURN void __exit_with_msg( char _WCI86FAR *msg, unsigned retcode )
{
    char        tmp[128];
    LPSTR       a,b;

    a = msg;
    b = tmp;
    while( *a && *a != '\r' && b < tmp+sizeof(tmp) )
        *b++ = *a++;
    *b = 0;

    MessageBox( (HWND)NULL, tmp, "Open Watcom", MB_OK );
    _exit( retcode );
    // never return
}

_WCRTLINK void __fatal_runtime_error( char _WCI86FAR *msg, unsigned retcode )
{
    if( __EnterWVIDEO( msg ) )
        _exit( retcode );
    __exit_with_msg( msg, retcode );
    // never return
}
