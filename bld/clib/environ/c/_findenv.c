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
* Description:  Wide character version of __(w)findenv().
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
#if defined( __RDOS__ ) || defined( __RDOSDEV__ )
    #include <rdos.h>
#endif
#include "rtdata.h"
#include "liballoc.h"
#include "_environ.h"

#define _ISNAMEEND(p)   (_TCSNEXTC(p)==STRING('=')||_TCSTERM(p))

static int __F_NAME(findenv,wfindenv)( const CHAR_TYPE *name, int delete_var )
{
    CHAR_TYPE           **envp, **tmp_envp;
    const CHAR_TYPE     *p1, *p2, *env_str;
    int                 index1;
#ifndef __WIDECHAR__
    int                 index2;
    char                *envm;
#endif

    for( envp = __F_NAME(_RWD_environ,_RWD_wenviron); (p1 = *envp) != NULL; ++envp ) {
        for( p2 = name; ; p1 = _TCSINC(p1), p2 = _TCSINC(p2) ) {
            if( _ISNAMEEND( p1 ) && _ISNAMEEND( p2 ) ) {
                index1 = envp - __F_NAME(_RWD_environ,_RWD_wenviron);
                if( delete_var ) {
                    env_str = *envp;
                    tmp_envp = envp;        /* delete entry */
                    for( ; *tmp_envp != NULL; ++tmp_envp ) {
                        *tmp_envp = *(tmp_envp + 1);
                    }
#ifdef __WIDECHAR__
                    lib_free( (void *)env_str );
#else
                    if( _RWD_env_mask != NULL ) {
                        if( _RWD_env_mask[index1] != 0 ) {
                            lib_free( (void *)env_str );
                        }
                        envm = (char *)(tmp_envp);
                        index2 = tmp_envp - _RWD_environ;
                        memmove( envm, _RWD_env_mask, index2 * sizeof( char ) );
                        _RWD_env_mask = envm;
                        for( ; index1 < index2; index1++ ) {
                            envm[index1] = envm[index1 + 1];
                        }
                    }
#endif
                                            /* delete more entries */
                } else {
                    return( index1 + 1 );   /* return index origin 1 */
                }
            }
#if defined( __UNIX__ )
            if( _TCSCMP( p1, p2 ) ) {
#else
            if( _TCSICMP( p1, p2 ) ) {       /* case independent search */
#endif
                break;
            }
        }
    }
    return( __F_NAME(_RWD_environ,_RWD_wenviron) - envp );  /* not found */
}

int __F_NAME(__findenv,__wfindenv)( const CHAR_TYPE *env_string, int delete_var )
{
    const CHAR_TYPE **envp;
    int             index;

    envp = (const CHAR_TYPE **)__F_NAME(_RWD_environ,_RWD_wenviron);
    if( envp == NULL ) {
        if( delete_var )
            return( 0 );
  #ifdef __WIDECHAR__
        envp = lib_malloc( 2 * sizeof( CHAR_TYPE * ) );
  #else
        envp = lib_malloc( 2 * sizeof( CHAR_TYPE * ) + sizeof( char ) );
  #endif
        if( envp == NULL )
            return( -1 );
        envp[0] = NULL;                   /* fill in below */
        envp[1] = NULL;
        __F_NAME(_RWD_environ,_RWD_wenviron) = (CHAR_TYPE **)envp;
  #ifndef __WIDECHAR__
        _RWD_env_mask = (char *)&envp[2];
  #endif
        index = 0;
    } else {
        index = __F_NAME(findenv,wfindenv)( env_string, delete_var );
        if( delete_var )
            return( 0 );
        if( index <= 0 ) {                  /* name not found */
            index = 1 - index;
  #ifdef __WIDECHAR__
            /* wide environment doesn't use alloc'd mask */
            envp = lib_realloc( envp, ( index + 1 ) * sizeof( CHAR_TYPE * ) );
            if( envp == NULL )
                return( -1 );
            memcpy( envp, _RWD_wenviron, ( index - 1 ) * sizeof( CHAR_TYPE * ) );
  #else
            if( _RWD_env_mask == NULL ) {
                envp = lib_malloc( ( index + 1 ) * sizeof( CHAR_TYPE * ) + index * sizeof( char ) );
                if( envp == NULL )
                    return( -1 );
                memcpy( envp, _RWD_environ, ( index - 1 ) * sizeof( CHAR_TYPE * ) );
                _RWD_env_mask = (char *)&envp[index + 1];
                memset( _RWD_env_mask, 0, index * sizeof( char ) );
            } else {
                envp = lib_realloc( envp, ( index + 1 ) * sizeof( CHAR_TYPE * ) + index * sizeof( char ) );
                if( envp == NULL )
                    return( -1 );
                memmove( &envp[index + 1], _RWD_env_mask, ( index - 1 ) * sizeof( char ) );
                _RWD_env_mask = (char *)&envp[index + 1];
            }
  #endif
            envp[index] = NULL;
            __F_NAME(_RWD_environ,_RWD_wenviron) = (CHAR_TYPE **)envp;
        }
    }
    return( index );
}
