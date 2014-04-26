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
* Description:  Implementation of spawnlp() and _wspawnlp().
*
****************************************************************************/


#include "widechar.h"
#include "variety.h"
#include <stdio.h>
#include <stdarg.h>
#include <process.h>
#include <malloc.h>
#include <memory.h>
#include <errno.h>
#include "seterrno.h"


_WCRTLINK int __F_NAME(spawnlp,_wspawnlp)( int mode, const CHAR_TYPE *path, const CHAR_TYPE *arg0, ... )
{
    va_list             ap;
    int                 rc;
#if defined(__AXP__) || defined(__PPC__) || defined(__MIPS__)
    va_list             bp;
    const CHAR_TYPE     **a;
    const CHAR_TYPE     **tmp;
    int                 num;
#endif

    arg0 = arg0;
    va_start(ap, path);

#if defined(__AXP__) || defined(__PPC__) || defined(__MIPS__)
    memcpy( &bp, &ap, sizeof( ap ) );

    for( num = 1; va_arg( ap, CHAR_TYPE * ); ++num )
        ;

    a = (const CHAR_TYPE **)alloca( num * sizeof( CHAR_TYPE * ) );
    if( !a ) {
        __set_errno( ENOMEM );
        return( -1 );
    }

    for( tmp = a; num > 0; --num )
        *tmp++ = (CHAR_TYPE *)va_arg( bp, CHAR_TYPE * );

    rc = __F_NAME(spawnvp,_wspawnvp)( mode, path, a );
#else
    rc = __F_NAME(spawnvp,_wspawnvp)( mode, path, (const CHAR_TYPE **)ap[0] );
    va_end( ap );
#endif
    return( rc );
} /* spawnlp() */
