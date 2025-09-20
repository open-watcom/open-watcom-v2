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
* Description:  Implementation of spawnlp() and _wspawnlp().
*
****************************************************************************/


#include "variety.h"
#include "widechar.h"
#include "seterrno.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <process.h>
#if defined( __NT__ )
    #include <windows.h>
#elif defined( __OS2__ )
    #include <wos2.h>
#endif
#include "thread.h"
#include "_process.h"
#include "_environ.h"


_WCRTLINK int __F_NAME(spawnlp,_wspawnlp)( int mode, const CHAR_TYPE *path, const CHAR_TYPE *arg0, ... )
{
    va_list             args;
    ARGS_TYPE_ARR       argv;
#if defined(__AXP__) || defined(__PPC__) || defined(__MIPS__)
    ARGS_TYPE           *tmp;
    int                 num;
#endif

    /* unused parameters */ (void)arg0;

    va_start( args, path );
#if defined(__AXP__) || defined(__PPC__) || defined(__MIPS__)
    num = 1;
    while( ARGS_NEXT_VA( args ) != NULL )
        num++;
    va_end( args );

    argv = tmp = alloca( num * sizeof( ARGS_TYPE ) );
    if( argv == NULL ) {
        lib_set_errno( ENOMEM );
        return( -1 );
    }

    va_start( args, path );
    while( num-- > 0 )
        *tmp++ = ARGS_NEXT_VA( args );
#else
    argv = ARGS_ARRAY_VA( args );
#endif
    va_end( args );
    return( __F_NAME(spawnvp,_wspawnvp)( mode, path, argv ) );
} /* spawnlp() */
