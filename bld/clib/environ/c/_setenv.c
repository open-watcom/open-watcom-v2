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
* Description:  Implementation of __(w)setenv().
*
****************************************************************************/


#include "variety.h"
#include "widechar.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#if defined( CLIB_USE_MBCS_TRANSLATION )
    #include <mbstring.h>
#endif
#ifdef __WIDECHAR__
    #include <wctype.h>
#endif
#if defined( __NT__ )
    #include <windows.h>
#elif defined( __RDOS__ ) || defined( __RDOSDEV__ )
    #include <rdos.h>
#elif defined( __OS2__ )
    #include <wos2.h>
#endif
#include "rtdata.h"
#ifdef __NT__
    #include "libwin32.h"
#endif
#include "liballoc.h"
#include "_environ.h"


#ifndef __NETWARE__
static int __F_NAME(addenv,waddenv)( int index, const CHAR_TYPE *name, const CHAR_TYPE *value )
/*
 * return 0 if succeded
 * otherwise return -1
 */
{
    int             len;
    CHAR_TYPE       *env_str;
    CHAR_TYPE       *old_val;
    CHAR_TYPE       **envp;

    envp = __F_NAME(_RWD_environ,_RWD_wenviron);
    len = __F_NAME(strlen,wcslen)( name );
    old_val = _RWD_env_mask[index] ? envp[index] : NULL;
    env_str = lib_realloc( old_val, ( len + 1 + __F_NAME(strlen,wcslen)( value ) + 1 ) * sizeof( CHAR_TYPE ) );
    if( env_str == NULL )
        return( -1 );
    memcpy( env_str, name, len * sizeof( CHAR_TYPE ) );
    env_str[len] = STRING( '=' );
    __F_NAME(strcpy,wcscpy)( &env_str[len + 1], value );
    envp[index] = env_str;
#ifndef __WIDECHAR__
    _RWD_env_mask[index] = 1;     /* indicate string alloc'd */
#endif
    return( 0 );
}
#endif

int __F_NAME(__setenv,__wsetenv)( const CHAR_TYPE *name, const CHAR_TYPE *value, int overwrite )
/*
 * if value == NULL then find all matching entries and delete them
 * if value != NULL then find first matching entry in evironment list and setup new value
 *
 * return 0 if succeded
 * otherwise return -1
 */
{
#ifdef __NETWARE__

    /* unused parameters */ (void)name; (void)value; (void)overwrite;

    return( -1 );
#else
    int     index;

    /* unused parameters */ (void)overwrite;

    CHECK_WIDE_ENV();
    if( value == NULL ) {
        return( __F_NAME(__findenvdel,__wfindenvdel)( name ) );
    }
    index = __F_NAME(__findenvadd,__wfindenvadd)( name );
    if( index < 0 ) {
        return( -1 );
    }
    return( __F_NAME(addenv,waddenv)( index, name, value ) );
#endif
}
