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


extern int __putenv( const char *env_string );
extern int __wputenv( const wchar_t *env_string );


// _wpetenv and putenv are implemented this way so that each can call the
// other without having the other call it, which would call the other, and
// so on, making bad things happen.  This inter-calling is necessary to keep
// the wide and MBCS environments consistent.  Also, with this method
// __create_wide_environment can call __wputenv, without having the function
// it calls call __create_wide_environment, causing similar bad things.
_WCRTLINK int __F_NAME(putenv,_wputenv)( const CHAR_TYPE *env_string )
{
#ifdef __WIDECHAR__
    char *              otherStr;
    const size_t        charsize = sizeof(wchar_t);
    const size_t        fact = MB_CUR_MAX;
#else
    wchar_t *           otherStr;
    const size_t        charsize = MB_CUR_MAX;
    const size_t        fact = 1;
#endif
    size_t              otherStrLen;
    int                 rc;
#ifdef __NT__
    CHAR_TYPE *         name;
    CHAR_TYPE *         value;
    CHAR_TYPE *         p;
    size_t              len;
    BOOL                osRc;
#endif

    /*** Update the process environment if using Win32 ***/
    #ifdef __NT__
        /*** Validate the input string ***/
        p = __F_NAME(_mbschr,wcschr)( env_string, __F_NAME('=',L'=') );
        if( p == NULL )  return( -1 );        /* must have form name=value */
        if( p == env_string )  return( -1 );  /* must have form name=value */

        /*** Extract the variable name ***/
        len = p - env_string;
        name = lib_malloc( (len+1) * CHARSIZE );
        if( name == NULL )  return( -1 );
        memcpy( name, env_string, len*CHARSIZE );
        name[len] = NULLCHAR;

        /*** Extract the new value, if any ***/
        p++;                            /* point past the '=' */
        len = __F_NAME(_mbslen,wcslen)( p );
        if( len != 0 ) {
            value = lib_malloc( (len+1) * CHARSIZE );
            if( value == NULL ) {
                lib_free( name );
                return( -1 );
            }
            memcpy( value, p, len*CHARSIZE );
            value[len] = NULLCHAR;
        } else {
            value = NULL;               /* don't need a buffer to delete */
        }

        /*** Tell the OS about the change ***/
        #ifdef __WIDECHAR__
            osRc = __lib_SetEnvironmentVariableW( name, value );
        #else
            osRc = SetEnvironmentVariableA( name, value );
        #endif
        lib_free( name );
        lib_free( value );
        if( osRc == FALSE ) {
            if( value == NULL ) {
                // we couldn't find the envvar but since we are deleting it,
                // the putenv() is successful
                return 0;
            }
            return( -1 );
        }
    #endif

    /*** Update the (__WIDECHAR__ ? wide : MBCS) environment ***/
    #ifdef __WIDECHAR__
        if( _RWD_wenviron == NULL )  __create_wide_environment();
    #endif
    if( __F_NAME(__putenv,__wputenv)( env_string )  !=  0 )  return( -1 );

    /*** Update the other environment ***/
    #ifndef __WIDECHAR__
        if( _RWD_wenviron == NULL )  return( 0 );  // _wenviron uninitialized
    #endif
    otherStrLen = __F_NAME(_mbslen,wcslen)(env_string) + 1;
    otherStr = lib_malloc( otherStrLen * charsize );
    if( otherStr == NULL ) {
        __set_errno( ENOMEM );
        return( -1 );
    }
    if( __F_NAME(mbstowcs,wcstombs)( otherStr, env_string, otherStrLen*fact )  ==  -1 ) {
        lib_free( otherStr );
        __set_errno( ERANGE );
        return( -1 );
    }
    rc = __F_NAME(__wputenv,__putenv)( otherStr );
    return( rc );
}


int __F_NAME(__putenv,__wputenv)( const CHAR_TYPE *env_string )
    {
#ifdef __NETWARE__

        env_string = env_string;
        return( -1 );
#else

        const CHAR_TYPE **envp;
        int index;
        const CHAR_TYPE *p;
        int delete_var;

        if( env_string == NULL ) return( -1 );
        if( *env_string != __F_NAME('\0',L'\0') ) { // handle =NAME=STRING
            for( p=env_string+1; *p; ++p ) {        // (used under NT)
                if( *p == __F_NAME('=',L'=') ) break;
            }
        }
        if( *p == __F_NAME('\0',L'\0') )  return( -1 ); /* <name> with no '=' is illegal */
        delete_var = p[ 1 ] == __F_NAME('\0',L'\0');
        envp = (const CHAR_TYPE **)__F_NAME(_RWD_environ,_RWD_wenviron);
        if( envp == NULL ) {
            if( delete_var ) return( 0 );
            #ifdef __WIDECHAR__
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
            index = findenv( env_string, delete_var );
            if( delete_var ) return( 0 );
            if( index <= 0 ) {                  /* name not found */
                index = - index;
                #ifdef __WIDECHAR__
                    /* wide environment doesn't use alloc'd mask */
                    envp = lib_realloc( envp, (index+2) * sizeof(CHAR_TYPE*) );
                    if( envp == NULL ) return( -1 );
                    memcpy( envp, _RWD_wenviron, index * sizeof(CHAR_TYPE*) );
                #else
                    if( _RWD_env_mask == NULL ) {
                        envp = lib_malloc( (index+2) * sizeof(CHAR_TYPE*) +
                                       (index+1) * sizeof(char) );
                        if( envp == NULL ) return( -1 );
                        memcpy( envp, _RWD_environ, index * sizeof(CHAR_TYPE*) );
                        _RWD_env_mask = (char *) &envp[ index + 2 ];
                        memset( _RWD_env_mask, 0, (index+1) * sizeof(char) );
                    } else {
                        envp = lib_realloc( envp, (index+2) * sizeof(CHAR_TYPE*)
                                                + (index+1) * sizeof(char) );
                        if( envp == NULL ) return( -1 );
                        memmove( &envp[ index + 2 ], _RWD_env_mask,
                                    index * sizeof(char) );
                        _RWD_env_mask = (char *) &envp[ index + 2 ];
                    }
                #endif
                envp[ index + 1 ] = NULL;
                __F_NAME(_RWD_environ,_RWD_wenviron) = (CHAR_TYPE**) envp;
            } else {                            /* name found */
                index--;
            }
        }
        envp[ index ] = env_string;
        #ifndef __WIDECHAR__
            _RWD_env_mask[ index ] = 0;     /* indicate not alloc'd */
        #endif
        return( 0 );
#endif
    }


#ifndef __NETWARE__
static int findenv( const CHAR_TYPE *env_string, int delete_var )
    {
        CHAR_TYPE **envp, **tmp_envp;
        const CHAR_TYPE *p1, *p2, *env_str;
        int index1;
    #ifndef __WIDECHAR__
        int index2;
        char *envm;
    #endif

        for( envp=__F_NAME(_RWD_environ,_RWD_wenviron); p1=*envp; ++envp ) {
            for( p2 = env_string; *p2; ++p1, ++p2 ) {
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
                if( *p1 == __F_NAME('=',L'=') ) {
                    index1 = envp - __F_NAME(_RWD_environ,_RWD_wenviron);
                    if( delete_var ) {
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
                        return( 0 );            /* pretend it wasn't found */
                    } else {
                        return( index1 + 1 );   /* return index origin 1 */
                    }
                }
            }
        }
        return( __F_NAME(_RWD_environ,_RWD_wenviron) - envp );/* not found */
    }
#endif
