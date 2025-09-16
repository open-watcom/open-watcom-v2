/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2025 The Open Watcom Contributors. All Rights Reserved.
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
#include "widechar.h"
#include <process.h>
#include <windows.h>
#include "seterrno.h"
#include "_process.h"
#include "_int23.h"
#include "thread.h"

_WCRTLINK int __F_NAME(execve,_wexecve)( const CHAR_TYPE *path, const CHAR_TYPE * const argv[], const CHAR_TYPE * const envp[] )
{
    int         rc;

    rc = __F_NAME(spawnve,_wspawnve)( P_NOWAIT, path, argv, envp );
    if( _RWD_errno == 0 ) {
        __int23_exit();
        ExitProcess( rc );
        // never return
    }
    return( -1 );
}
