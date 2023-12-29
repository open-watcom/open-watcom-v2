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
* Description:  Implementation of (_w)setenv().
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
#include "_cvtstr.h"
#include "thread.h"


#ifdef CLIB_USE_OTHER_ENV

static int __other_env_update( const CHAR_TYPE *in_name, const CHAR_TYPE *in_value, int overwrite )
/*
 * it updates counterpart environment data (wide or narrow)
 *
 * following definitions have reversed near/wide character pointers
 * for narrow/wide environment synchronization
 */
{
    __F_NAME(wchar_t,char)  *name;
    __F_NAME(wchar_t,char)  *value;
    int                     rc;

  #ifndef __WIDECHAR__
    if( _RWD_wenviron == NULL ) {
        return( 0 );    // _wenviron uninitialized
    }
  #endif
    name = __F_NAME(__lib_cvt_mbstowcs_errno,__lib_cvt_wcstombs_errno)( in_name );
    if( name == NULL ) {
        return( -1 );
    }
    if( in_value != NULL ) {
        value = __F_NAME(__lib_cvt_mbstowcs_errno,__lib_cvt_wcstombs_errno)( in_value );
        if( value == NULL ) {
            lib_free( name );
            return( -1 );
        }
    } else {
        value = NULL;
    }
    rc = __F_NAME(__wsetenv,__setenv)( name, value, overwrite );
    lib_free( name );
    lib_free( value );
    return( rc );
}

#endif

// _wsetenv and setenv are implemented this way so that each can call the
// other without having the other call it, which would call the other, and
// so on, making bad things happen. This inter-calling is necessary to keep
// the wide and MBCS environments consistent.
_WCRTLINK int __F_NAME(setenv,_wsetenv)( const CHAR_TYPE *name, const CHAR_TYPE *value, int overwrite )
{
    int                 rc;

    if( name == NULL || *name == NULLCHAR || __F_NAME(strchr,wcschr)( name, STRING( '=' ) ) != NULL ) {
        _RWD_errno = EINVAL;
        return( -1 );
    }

    /*** Ensure variable is deleted if value == "" ***/

#ifndef __UNIX__
    if( (value != NULL) && (*value == NULLCHAR) ) {
        if( overwrite || (__F_NAME(getenv,_wgetenv)( name ) == NULL) ) {
            value = NULL;
        }
    }
#endif

    /*** Update the OS process environment ***/

#ifdef CLIB_UPDATE_OS_ENV
    if( overwrite || __F_NAME(getenv,_wgetenv)( name ) == NULL ) {
        if( __F_NAME(__os_env_update_narrow,__os_env_update_wide)( name, value ) ) {
            return( -1 );
        }
    }
#endif

    /*** Update current environment (wide or MBCS) ***/

    CHECK_WIDE_ENV();
    rc = __F_NAME(__setenv,__wsetenv)( name, value, overwrite );

    /*** Update the other environment ***/

#ifdef CLIB_USE_OTHER_ENV
    if( rc == 0 ) {
        rc = __other_env_update( name, value, overwrite );
    }
#endif
    return( rc );
}
