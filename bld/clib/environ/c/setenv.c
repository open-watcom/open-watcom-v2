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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#include "variety.h"
#include "widechar.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <mbstring.h>
#include <errno.h>
#ifdef __NT__
    #include <windows.h>
    #include "libwin32.h"
#endif
#include "liballoc.h"
#include "rtdata.h"
#include "seterrno.h"
#ifdef __WIDECHAR__
    #include "wenviron.h"
#endif


extern _WCRTLINK int _setenv( const char *name, const char *newvalue, int overwrite );
extern _WCRTLINK int __wsetenv( const wchar_t *name, const wchar_t *newvalue, int overwrite );


// _wsetenv and setenv are implemented this way so that each can call the
// other without having the other call it, which would call the other, and
// so on, making bad things happen.  This inter-calling is necessary to keep
// the wide and MBCS environments consistent.
_WCRTLINK int __F_NAME(setenv,_wsetenv)( const CHAR_TYPE *name, const CHAR_TYPE *newvalue, int overwrite )
{
#ifdef __WIDECHAR__
    char *              otherName;
    char *              otherNewval;
    const size_t        charsize = sizeof(wchar_t);
    const size_t        fact = MB_CUR_MAX;
#else
    wchar_t *           otherName;
    wchar_t *           otherNewval;
    const size_t        charsize = MB_CUR_MAX;
    const size_t        fact = 1;
#endif
    size_t              otherNameLen;
    size_t              otherNewvalLen;
    int                 rc;
#ifdef __NT__
    BOOL                osRc;
#endif

    /*** Ensure variable is deleted if newvalue=="" ***/
    #ifndef __QNX__
        if( newvalue != NULL  &&  *newvalue == NULLCHAR ) {
            if( overwrite  ||  __F_NAME(getenv,_wgetenv)(name) == NULL ) {
                newvalue = NULL;
            }
        }
    #endif

    /*** Update the process environment if using Win32 ***/
    #ifdef __NT__
        if( overwrite  ||  __F_NAME(getenv,_wgetenv)(name) == NULL ) {
            #ifdef __WIDECHAR__
                osRc = __lib_SetEnvironmentVariableW( name, newvalue );
            #else
                osRc = SetEnvironmentVariableA( name, newvalue );
            #endif
            if( osRc == FALSE )  return( -1 );
        }
    #endif

    /*** Update the (__WIDECHAR__ ? wide : MBCS) environment ***/
    #ifdef __WIDECHAR__
        if( _RWD_wenviron == NULL )  __create_wide_environment();
    #endif
    if( __F_NAME(_setenv,__wsetenv)( name, newvalue, overwrite )  !=  0 ) {
        return( -1 );
    }

    /*** Update the other environment ***/
    #ifndef __WIDECHAR__
        if( _RWD_wenviron == NULL )  return( 0 );  // _wenviron uninitialized
    #endif
    otherNameLen = __F_NAME(_mbslen,wcslen)(name) + 1;
    otherName = lib_malloc( otherNameLen * charsize );
    if( otherName == NULL ) {
        __set_errno( ENOMEM );
        return( -1 );
    }
    if( newvalue != NULL ) {
        otherNewvalLen = __F_NAME(_mbslen,wcslen)(newvalue) + 1;
        otherNewval = lib_malloc( otherNewvalLen * charsize );
        if( otherNewval == NULL ) {
            lib_free( otherName );
            __set_errno( ENOMEM );
            return( -1 );
        }
    } else {
        otherNewval = NULL;
    }
    if( __F_NAME(mbstowcs,wcstombs)( otherName, name, otherNameLen*fact )  ==  -1 ) {
        lib_free( otherName );
        lib_free( otherNewval );
        return( -1 );
    }
    if( otherNewval != NULL ) {
        if( __F_NAME(mbstowcs,wcstombs)( otherNewval, newvalue, otherNewvalLen*fact )  ==  -1 ) {
            lib_free( otherName );
            lib_free( otherNewval );
            return( -1 );
        }
    }
    rc = __F_NAME(__wsetenv,_setenv)( otherName, otherNewval, overwrite );
    lib_free( otherName );
    if( otherNewval != NULL )  lib_free( otherNewval );
    return( rc );
}


_WCRTLINK int __F_NAME(_setenv,__wsetenv)( const CHAR_TYPE *name,
                                           const CHAR_TYPE *newvalue,
                                           int overwrite )
    {
#ifdef __NETWARE__
        name = name; newvalue = newvalue; overwrite = overwrite;
        return( -1 );
#else
        const CHAR_TYPE **  envp;
        int                 index, len;
        CHAR_TYPE *         env_str;
        const CHAR_TYPE *   old_val;

        if( name == NULL ) return( -1 );
        if( *name == __F_NAME('\0',L'\0') ) return( -1 );

        #ifdef __WIDECHAR__
            if( _RWD_wenviron == NULL )  __create_wide_environment();
        #endif

        envp = (const CHAR_TYPE **)__F_NAME(_RWD_environ,_RWD_wenviron);
        if( envp == NULL ) {
            if( newvalue == NULL ) return( 0 ); /* nothing to do */
            #ifdef __WIDECHAR__
                /* wide environment doesn't use alloc'd mask */
                envp = lib_malloc( 2 * sizeof(CHAR_TYPE*) );
            #else
                envp = lib_malloc( 2 * sizeof(CHAR_TYPE*) + sizeof(char) );
            #endif
            if( envp == NULL ) return( -1 );
            envp[ 0 ] = NULL;                   /* fill in below */
            envp[ 1 ] = NULL;
            __F_NAME(_RWD_environ,_RWD_wenviron) = (CHAR_TYPE**) envp;
            #ifndef __WIDECHAR__
                _RWD_env_mask = (char *) &envp[ 2 ];
            #endif
            index = 0;
        } else {
            index = findenv( name, newvalue );
            if( newvalue == NULL ) return( 0 );
            if( index <= 0 ) {                  /* name not found */
                index = - index;
                #ifdef __WIDECHAR__
                    envp = lib_realloc( envp, (index+2) * sizeof(CHAR_TYPE*) );
                    if( envp == NULL ) return( -1 );
                    memcpy( envp, _RWD_wenviron, index * sizeof(CHAR_TYPE*) );
                #else
                    if( _RWD_env_mask == NULL ) {
                        envp = lib_malloc( (index+2) * sizeof(CHAR_TYPE*) +
                                           (index+1) * sizeof(char) );
                        if( envp == NULL ) return( -1 );
                        memcpy( envp, __F_NAME(_RWD_environ,_RWD_wenviron),
                                index * sizeof(CHAR_TYPE*) );
                        _RWD_env_mask = (char *) &envp[ index + 2 ];
                        memset( _RWD_env_mask, 0, (index+1) * sizeof(char) );
                    } else {
                        envp = lib_realloc( envp, (index+2) * sizeof(CHAR_TYPE*) +
                                                  (index+1) * sizeof(char) );
                        if( envp == NULL ) return( -1 );
                        memmove( &envp[ index + 2 ], _RWD_env_mask,
                                    index * sizeof(char) );
                        _RWD_env_mask = (char *) &envp[ index + 2 ];
                    }
                    _RWD_env_mask[ index ] = 0;     /* indicate string not alloc'd */
                #endif
                envp[ index + 1 ] = NULL;
                __F_NAME(_RWD_environ,_RWD_wenviron) = (CHAR_TYPE**) envp;
            } else {                            /* name found */
                if( overwrite == 0 ) return( 0 );
                index--;
            }
        }
        len = __F_NAME(strlen,wcslen)( name );
        old_val = _RWD_env_mask[ index ] ? envp[ index ] : NULL;
        env_str = lib_realloc( (void*)old_val,
                               (len + __F_NAME(strlen,wcslen)(newvalue) + 2)
                               * sizeof(CHAR_TYPE) );
        if( env_str == NULL ) return( -1 );
        memcpy( env_str, name, len*sizeof(CHAR_TYPE) );
        env_str[ len ] = __F_NAME('=',L'=');
        __F_NAME(strcpy,wcscpy)( &env_str[ len+1 ], newvalue );

        envp[ index ] = env_str;
        #ifndef __WIDECHAR__
            _RWD_env_mask[ index ] = 1;     /* indicate string alloc'd */
        #endif
        return( 0 );
#endif
    }

/*
 * if newvalue == NULL then find all matching entries and delete them
 * if newvalue != NULL then find first matching entry in evironment list
 */

#ifndef __NETWARE__
static int findenv( const CHAR_TYPE *name, const CHAR_TYPE *newvalue )
    {
        CHAR_TYPE **envp, **tmp_envp;
        const CHAR_TYPE *p1, *p2, *env_str;
        int index1;
    #ifndef __WIDECHAR__
        int index2;
        char *envm;
    #endif

        for( envp=__F_NAME(_RWD_environ,_RWD_wenviron); p1=*envp; ++envp ) {
            for( p2 = name; ; ++p1, ++p2 ) {
                if( *p1==__F_NAME('=',L'=') && *p2==__F_NAME('\0',L'\0') ) {
                    index1 = envp - __F_NAME(_RWD_environ,_RWD_wenviron);
                    if( newvalue == NULL ) {
                        env_str = *envp;
                        tmp_envp = envp;        /* delete entry */
                        for( ; *tmp_envp; ++tmp_envp ) {
                            *tmp_envp = *(tmp_envp+1);
                        }
                        #ifdef __WIDECHAR__
                            lib_free( (void*)env_str );
                        #else
                            if( _RWD_env_mask != NULL ) {
                                if( _RWD_env_mask[ index1 ] != 0 ) {
                                    lib_free( (void*)env_str );
                                }
                                envm = (char *)(tmp_envp);
                                index2 = tmp_envp - _RWD_environ;
                                memmove( envm, _RWD_env_mask, index2 * sizeof(char) );
                                _RWD_env_mask = envm;
                                for( ; index1 < index2; index1++ ) {
                                    envm[ index1 ] = envm[ index1 + 1 ];
                                }
                            }
                        #endif
                                                /* delete more entries */
                    } else {
                        return( index1 + 1 );   /* return index origin 1 */
                    }
                }
#ifdef __QNX__
                if( *p1 != *p2 ) break;
#else
                /* case independent search */
                #ifdef __WIDECHAR__
                    if( towupper( *p1 ) != towupper( *p2 ) )  break;
                #else
                    if( toupper( *p1 ) != toupper( *p2 ) )  break;
                #endif
#endif
                /* QNX can have just NAME in env list instead of NAME=value */
                if( *p2 == __F_NAME('\0',L'\0') ) break;
            }
        }
        return( __F_NAME(_RWD_environ,_RWD_wenviron) - envp );/* not found */
    }
#endif
