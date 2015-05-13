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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#undef __INLINE_FUNCTIONS__
#include "variety.h"
#include <process.h>
#define INCL_DOSPROCESS
#include <wos2.h>
#include "rtdata.h"
#include "_process.h"
#include "_int23.h"
#include "errorno.h"
#include "thread.h"

int __p_overlay = 2;

_WCRTLINK int execve( const char *path, const char *const argv[],
    const char *const envp[] )
{
    int         rc;

    rc = spawnve( P_NOWAIT, path, argv, envp );
    if( _RWD_errno != 0 ) return( -1 );
    __int23_exit();
    DosExit( EXIT_PROCESS, rc );
    return( 0 );
}
