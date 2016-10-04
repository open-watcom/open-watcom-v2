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
* Description:  Implementation of spawnlpe() and _wspawnlpe().
*
****************************************************************************/


#include "widechar.h"
#include "variety.h"
#include <stdio.h>
#include <stdarg.h>
#include <process.h>
#include <malloc.h>
#include <memory.h>
#include "rterrno.h"
#include "thread.h"


_WCRTLINK int __F_NAME(spawnlpe,_wspawnlpe)( int mode, const CHAR_TYPE *path, const CHAR_TYPE *arg0, ... )
{
    va_list             ap;
    const CHAR_TYPE     **env;
#if defined(__AXP__) || defined(__PPC__) || defined(__MIPS__)
    va_list             bp;
    const CHAR_TYPE     **a;
    const CHAR_TYPE     **tmp;
    int                 num = 1;
#endif

    arg0 = arg0;
    va_start( ap, path );
#if defined(__AXP__) || defined(__PPC__) || defined(__MIPS__)
    memcpy( &bp, &ap, sizeof( ap ) );
#endif

    /*
     * Scan until NULL in parm list
     */
    while( va_arg( ap, CHAR_TYPE * ) ) {
#if defined(__AXP__) || defined(__PPC__) || defined(__MIPS__)
        ++num;
#else
        ;
#endif
    }

    /*
     * Point to environment parameter.
     */
    env = (const CHAR_TYPE **)va_arg( ap, const CHAR_TYPE ** );

#if defined(__AXP__) || defined(__PPC__) || defined(__MIPS__)
    a = (const CHAR_TYPE **)alloca( num * sizeof( CHAR_TYPE * ) );
    if( !a ) {
        _RWD_errno = ENOMEM;
        return( -1 );
    }

    for( tmp = a; num > 0; --num )
        *tmp++ = (CHAR_TYPE *)va_arg( bp, CHAR_TYPE * );

    return( __F_NAME(spawnvpe,_wspawnvpe)( mode, path, a, env ) );
#else
    va_end( ap );
    va_start( ap, path );
    return( __F_NAME(spawnvpe,_wspawnvpe)( mode, path,
            (const CHAR_TYPE**)ap[0], env ) );
#endif
}
