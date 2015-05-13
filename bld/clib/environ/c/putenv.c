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
* Description:  Implementation for (_w)putenv().
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
#elif defined( __OS2__ )
    #include <wos2.h>
#endif
#include "rtdata.h"
#ifdef __NT__
    #include "libwin32.h"
#endif
#include "liballoc.h"
#include "errorno.h"
#include "_environ.h"
#include "thread.h"


// _wpetenv and putenv are implemented this way so that each can call the
// other without having the other call it, which would call the other, and
// so on, making bad things happen.  This inter-calling is necessary to keep
// the wide and MBCS environments consistent.  Also, with this method
// __create_wide_environment can call __wputenv, without having the function
// it calls call __create_wide_environment, causing similar bad things.
_WCRTLINK int __F_NAME(putenv,_wputenv)( const CHAR_TYPE *env_string )
{
#if !defined( __UNIX__ ) && !defined( __RDOS__ ) && !defined( __RDOSDEV__ )
  #ifdef __WIDECHAR__
    char                *otherStr;
    const size_t        charsize = sizeof(wchar_t);
    const size_t        fact = MB_CUR_MAX;
  #else
    wchar_t             *otherStr;
    const size_t        charsize = MB_CUR_MAX;
    const size_t        fact = 1;
  #endif
    size_t              otherStrLen;
#endif
#if defined( __NT__ ) || defined( __RDOS__ ) || defined( __RDOSDEV__ )
    CHAR_TYPE           *name;
    CHAR_TYPE           *value;
    CHAR_TYPE           *p;
    size_t              len;
  #if defined( __NT__ )
    BOOL                osRc;
  #else
    int                 handle;
  #endif

    /*** Update the process environment if using Win32 ***/
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
    p++;                                    /* point past the '=' */
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
        value = NULL;               /* don't need a buffer to delete */
    }
  #ifdef __NT__
    /*** Tell the OS about the change ***/
    osRc = __lib_SetEnvironmentVariable( name, value );
  #elif defined( __RDOS__ )
    handle = RdosOpenProcessEnv();
    RdosDeleteEnvVar( handle, name );
    RdosAddEnvVar( handle, name, value );
    RdosCloseEnv( handle );        
  #elif defined( __RDOSDEV__ )
    handle = RdosOpenSysEnv();
    RdosDeleteEnvVar( handle, name );
    RdosAddEnvVar( handle, name, value );
    RdosCloseEnv( handle );        
  #endif
    lib_free( name );
    lib_free( value );
  #ifdef __NT__
    if( osRc == FALSE ) {
        if( value == NULL ) {
            // we couldn't find the envvar but since we are deleting it,
            // the putenv() is successful
            return( 0 );
        }
        return( -1 );
    }
  #endif
#endif

    /*** Update the (__WIDECHAR__ ? wide : MBCS) environment ***/
#ifdef __WIDECHAR__
    if( _RWD_wenviron == NULL ) {
        __create_wide_environment();
    }
#endif

#if defined( __UNIX__ ) || defined( __RDOS__ ) || defined( __RDOSDEV__ )
    return( __F_NAME(__putenv,__wputenv)( env_string ) );
#else
    if( __F_NAME(__putenv,__wputenv)( env_string ) != 0 )
        return( -1 );

    /*** Update the other environment ***/
  #ifndef __WIDECHAR__
    if( _RWD_wenviron == NULL )
        return( 0 );    // _wenviron uninitialized
  #endif
    otherStrLen = _TCSLEN( env_string ) + 1;
    otherStr = lib_malloc( otherStrLen * charsize );
    if( otherStr == NULL ) {
        _RWD_errno = ENOMEM;
        return( -1 );
    }
    if( __F_NAME(mbstowcs,wcstombs)( otherStr, env_string, otherStrLen * fact ) == -1 ) {
        lib_free( otherStr );
        _RWD_errno = ERANGE;
        return( -1 );
    }
    return( __F_NAME(__wputenv,__putenv)( otherStr ) );
#endif
}
