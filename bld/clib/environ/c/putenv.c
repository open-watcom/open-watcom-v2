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
* Description:  Implementation for (_w)putenv().
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
#ifdef __NT__
    #include <windows.h>
#elif defined( __RDOS__ ) || defined( __RDOSDEV__ )
    #include <rdos.h>
#elif defined( __OS2__ )
    #include <wos2.h>
#endif
#include "rtdata.h"
#include "rterrno.h"
#ifdef __NT__
    #include "libwin32.h"
#endif
#include "liballoc.h"
#include "_environ.h"
#include "_tcsstr.h"
#include "_cvtstr.h"
#include "thread.h"


#ifdef CLIB_USE_OTHER_ENV

static int __other_env_update( const CHAR_TYPE *in_env_string )
/*
 * it updates counterpart environment data (wide or narrow)
 *
 * following definitions have reversed near/wide character pointers
 * for narrow/wide environment synchronization
 */
{
    __F_NAME(wchar_t,char)  *env_string;
    int                     rc;

  #ifndef __WIDECHAR__
    if( _RWD_wenviron == NULL )
        return( 0 );    // _wenviron uninitialized
  #endif
    env_string = __F_NAME(__lib_cvt_mbstowcs_errno,__lib_cvt_wcstombs_errno)( in_env_string );
    if( env_string == NULL ) {
        return( -1 );
    }
    rc = __F_NAME(__wputenv,__putenv)( env_string );
    if( rc ) {
        lib_free( env_string );
    }
    return( rc );
}

#endif

#ifdef CLIB_UPDATE_OS_ENV

static int __os_env_update( const CHAR_TYPE *env_string )
{
    CHAR_TYPE           *name;
    CHAR_TYPE           *value;
    CHAR_TYPE           *p;
    size_t              len;
    int                 rc;

    /*** Validate the input string ***/
    p = _TCSCHR( env_string, STRING( '=' ) );
    if( p == NULL || p == env_string )
        return( -1 );       /* must have form name=value */

    /*** Extract the variable name ***/
    len = p - env_string;
    name = lib_malloc( ( len + 1 ) * CHARSIZE );
    if( name == NULL )
        return( -1 );
    memcpy( name, env_string, len * CHARSIZE );
    name[len] = NULLCHAR;

    /*** Extract the new value, if any ***/
    p++;                        /* point past the '=' */
    len = _TCSLEN( p );
    if( len != 0 ) {
        value = lib_malloc( ( len + 1 ) * CHARSIZE );
        if( value == NULL ) {
            lib_free( name );
            return( -1 );
        }
        memcpy( value, p, len * CHARSIZE );
        value[len] = NULLCHAR;
    } else {
        value = NULL;           /* don't need a buffer to delete */
    }

    /*** Update the OS process environment ***/
    rc = __F_NAME(__os_env_update_narrow,__os_env_update_wide)( name, value );

    lib_free( name );
    lib_free( value );
    return( rc );
}

#endif

// _wputenv and putenv are implemented this way so that each can call the
// other without having the other call it, which would call the other, and
// so on, making bad things happen.  This inter-calling is necessary to keep
// the wide and MBCS environments consistent.  Also, with this method
// __create_wide_environment can call __wputenv, without having the function
// it calls call __create_wide_environment, causing similar bad things.
_WCRTLINK int __F_NAME(putenv,_wputenv)( const CHAR_TYPE *env_string )
{
    int     rc;

    /*** Update the OS process environment ***/

#ifdef CLIB_UPDATE_OS_ENV
    if( __os_env_update( env_string ) ) {
        return( -1 );
    }
#endif

    /*** Update the current environment (wide or MBCS) ***/

    CHECK_WIDE_ENV();

    rc = __F_NAME(__putenv,__wputenv)( env_string );

    /*** Update the other environment ***/

#ifdef CLIB_USE_OTHER_ENV
    if( rc == 0 ) {
        rc = __other_env_update( env_string );
    }
#endif
    return( rc );
}
