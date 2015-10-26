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
* Description:  RDOS trap file startup and shutdown.
*
****************************************************************************/


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <direct.h>
#include "rdos.h"
#include "stdrdos.h"

trap_version TRAPENTRY TrapInit( const char *parms, char *err, bool remote )
{
    trap_version    ver;

    remote = remote;
    parms = parms;

    err[0] = 0;
    ver.major = TRAP_MAJOR_VERSION;
    ver.minor = TRAP_MINOR_VERSION;
    ver.remote = FALSE;
    return( ver );
}

void TRAPENTRY TrapFini( void )
{
}
