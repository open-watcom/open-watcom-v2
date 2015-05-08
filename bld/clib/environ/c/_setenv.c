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
* Description:  Implementation of __(w)setenv().
*
****************************************************************************/


#include "widechar.h"
#include "variety.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <mbstring.h>
#ifdef __WIDECHAR__
    #include <wctype.h>
#endif
#ifdef __NT__
    #include <windows.h>
#elif defined( __RDOS__ ) || defined( __RDOSDEV__ )
    #include <rdos.h>
#endif
#include "rtdata.h"
#ifdef __NT__
    #include "libwin32.h"
#endif
#include "liballoc.h"
#include "seterrno.h"
#include "_environ.h"


#ifndef __NETWARE__
static int __F_NAME(addenv,waddenv)( int index, const CHAR_TYPE *name, const CHAR_TYPE *newvalue )
{
    int                 len;
    CHAR_TYPE           *env_str;
    const CHAR_TYPE     *old_val;
    const CHAR_TYPE     **envp;

    envp = (const CHAR_TYPE **)__F_NAME(_RWD_environ,_RWD_wenviron);
    len = __F_NAME(strlen,wcslen)( name );
    old_val = _RWD_env_mask[index] ? envp[index] : NULL;
    env_str = lib_realloc( (void *)old_val, ( len + __F_NAME(strlen,wcslen)( newvalue ) + 2 ) * sizeof( CHAR_TYPE ) );
    if( env_str == NULL )
        return( -1 );
    memcpy( env_str, name, len*sizeof( CHAR_TYPE ) );
    env_str[len] = STRING( '=' );
    __F_NAME(strcpy,wcscpy)( &env_str[len + 1], newvalue );
    envp[index] = env_str;
#ifndef __WIDECHAR__
    _RWD_env_mask[index] = 1;     /* indicate string alloc'd */
#endif
    return( 0 );
}
#endif

/*
 * if newvalue == NULL then find all matching entries and delete them
 * if newvalue != NULL then find first matching entry in evironment list and setup new value
 */

int __F_NAME(__setenv,__wsetenv)( const CHAR_TYPE *name, const CHAR_TYPE *newvalue, int overwrite )
{
#ifdef __NETWARE__
    name = name; newvalue = newvalue; overwrite = overwrite;
    return( -1 );
#else
    int     rc;

    if( name == NULL || *name == NULLCHAR ) {
        return( -1 );
    }
  #ifdef __WIDECHAR__
    if( _RWD_wenviron == NULL ) {
        __create_wide_environment();
    }
  #endif
    rc = __F_NAME(__findenv,__wfindenv)( name, ( newvalue == NULL ) );
    if( rc > 0 ) {
        rc = __F_NAME(addenv,waddenv)( rc - 1, name, newvalue );
    }
    return( rc );
#endif
}
