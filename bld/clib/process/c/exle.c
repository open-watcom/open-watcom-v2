/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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


#include "variety.h"
#include "widechar.h"
#include <stddef.h>
#include <stdio.h>
#include <stdarg.h>
#include <process.h>
#include "_process.h"
#include "_environ.h"


_WCRTLINK int __F_NAME(execle,_wexecle)( const CHAR_TYPE *path, const CHAR_TYPE *arg0, ... )
{
    va_list         ap;
    ARGS_TYPE_ARR   args;
    ARGS_TYPE_ARR   env;

    /* unused parameters */ (void)arg0;

    va_start( ap, path );
#if defined(__AXP__) || defined(__MIPS__)
    args = (ARGS_TYPE_ARR)ap.__base;
#else
    args = (ARGS_TYPE_ARR)ap[0];
#endif
    /* scan until NULL in parm list */
    while( va_arg( ap, ARGS_TYPE ) != NULL )
        ;
    /* point to environ parm */
    env = va_arg( ap, ARGS_TYPE_ARR );
    va_end( ap );
    return( __F_NAME(execve,_wexecve)( path, args, env ) );
}
