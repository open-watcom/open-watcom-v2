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
* Description:  Implementation of (_w)setenv().
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
#include "rterrno.h"
#ifdef __NT__
    #include "libwin32.h"
#endif
#include "liballoc.h"
#include "_environ.h"
#include "thread.h"


// _wsetenv and setenv are implemented this way so that each can call the
// other without having the other call it, which would call the other, and
// so on, making bad things happen. This inter-calling is necessary to keep
// the wide and MBCS environments consistent.
_WCRTLINK int __F_NAME(setenv,_wsetenv)( const CHAR_TYPE *name, const CHAR_TYPE *newvalue, int overwrite )
{
#if !defined( __UNIX__ ) && !defined( __RDOS__ ) && !defined( __RDOSDEV__ )
  #ifdef __WIDECHAR__
    char                *otherName;
    char                *otherNewval;
    const size_t        charsize = sizeof( wchar_t );
    const size_t        fact = MB_CUR_MAX;
  #else
    wchar_t             *otherName;
    wchar_t             *otherNewval;
    const size_t        charsize = MB_CUR_MAX;
    const size_t        fact = 1;
  #endif
    size_t              otherNameLen;
    size_t              otherNewvalLen;
#endif
    int                 rc;

    /*** Ensure variable is deleted if newvalue == "" ***/
#ifndef __UNIX__
    if( (newvalue != NULL) && (*newvalue == NULLCHAR) ) {
        if( overwrite || (__F_NAME(getenv,_wgetenv)( name ) == NULL) ) {
            newvalue = NULL;
        }
    }
#endif

#ifdef __NT__
    /*** Update the process environment if using Win32 ***/
    if( overwrite || __F_NAME(getenv,_wgetenv)( name ) == NULL ) {
        if( __lib_SetEnvironmentVariable( name, newvalue ) == FALSE ) {
            return( -1 );
        }
    }
#elif defined( __RDOS__ )
    /*** Update the process environment if using RDOS ***/
    if( overwrite  ||  __F_NAME(getenv,_wgetenv)( name ) == NULL ) {
        int handle;

        handle = RdosOpenProcessEnv();
        RdosDeleteEnvVar( handle, name );
        if( *newvalue != NULLCHAR )
            RdosAddEnvVar( handle, name, newvalue );
        RdosCloseEnv( handle );
    }
#elif defined( __RDOSDEV__ )
    /*** Update the process environment if using RDOSDEV ***/
    if( overwrite  ||  __F_NAME(getenv,_wgetenv)( name ) == NULL ) {
        int handle;

        handle = RdosOpenSysEnv();
        RdosDeleteEnvVar( handle, name );
        if( *newvalue != NULLCHAR )
            RdosAddEnvVar( handle, name, newvalue );
        RdosCloseEnv( handle );
    }
#endif

    /*** Update the (__WIDECHAR__ ? wide : MBCS) environment ***/
#ifdef __WIDECHAR__
    if( _RWD_wenviron == NULL ) {
        __create_wide_environment();
    }
#endif
    rc = __F_NAME(__setenv,__wsetenv)( name, newvalue, overwrite );
    if( rc == -1 ) {
        return( -1 );
    }
#if !defined( __UNIX__ ) && !defined( __RDOS__ ) && !defined( __RDOSDEV__ )
    /*** Update the other environment ***/
  #ifndef __WIDECHAR__
    if( _RWD_wenviron == NULL ) {
        return( 0 );    // _wenviron uninitialized
    }
  #endif
  #ifdef __WIDECHAR__
    otherNameLen = wcslen( name ) + 1;
  #else
    otherNameLen = _mbslen( (unsigned char *)name ) + 1;
  #endif
    otherName = lib_malloc( otherNameLen * charsize );
    if( otherName == NULL ) {
        _RWD_errno = ENOMEM;
        return( -1 );
    }
    if( newvalue != NULL ) {
  #ifdef __WIDECHAR__
        otherNewvalLen = wcslen( newvalue ) + 1;
  #else
        otherNewvalLen = _mbslen( (unsigned char *)newvalue ) + 1;
  #endif
        otherNewval = lib_malloc( otherNewvalLen * charsize );
        if( otherNewval == NULL ) {
            lib_free( otherName );
            _RWD_errno = ENOMEM;
            return( -1 );
        }
    } else {
        otherNewval = NULL;
    }
    rc = __F_NAME(mbstowcs,wcstombs)( otherName, name, otherNameLen * fact );
    if( rc != -1 ) {
        if( otherNewval != NULL ) {
            rc = __F_NAME(mbstowcs,wcstombs)( otherNewval, newvalue, otherNewvalLen * fact );
        }
        if( rc != -1 ) {
            rc = __F_NAME(__wsetenv,__setenv)( otherName, otherNewval, overwrite );
        }
    }
    lib_free( otherName );
    lib_free( otherNewval );
    if( rc == -1 ) {
        return( -1 );
    }
#endif
    return( 0 );
}
