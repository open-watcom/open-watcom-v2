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
* Description:  Wide character version of __(w)findenv().
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
#if defined( __RDOS__ ) || defined( __RDOSDEV__ )
    #include <rdos.h>
#endif
#include "rtdata.h"
#include "liballoc.h"
#include "_environ.h"
#include "_tcsstr.h"


#define _ISNAMEEND(p)   (_TCSNEXTC(p)==STRING('=')||_TCSTERM(p))

static int __F_NAME(findenv,wfindenv)( const CHAR_TYPE *name, int delete_var )
/*
 * return 1 based index if found
 * otherwise return existing size as negative nuber or 0
 */
{
    CHAR_TYPE           **envp, **tmp_envp;
    const CHAR_TYPE     *p1, *p2;
#ifndef __WIDECHAR__
    size_t              len;
    char                *envm;
#endif

    for( envp = __F_NAME(_RWD_environ,_RWD_wenviron); (p1 = *envp) != NULL; ++envp ) {
        for( p2 = name; ; p1 = _TCSINC(p1), p2 = _TCSINC(p2) ) {
#ifdef __NT__
            /*
             * on NT if first character is '=' then it is part of name
             * this is used by command processor cmd.exe to handle
             * current directories on disk drives '=C:=C:\....' etc.
             */
            if( _ISNAMEEND( p1 ) && _ISNAMEEND( p2 ) && ( p2 != name ) ) {
#else
            if( _ISNAMEEND( p1 ) && _ISNAMEEND( p2 ) ) {
#endif
                if( delete_var == 0 ) {
                    /* return index origin 1 */
                    return( envp - __F_NAME(_RWD_environ,_RWD_wenviron) + 1 );
                }
                /* delete entry */
#ifdef __WIDECHAR__
                lib_free( *envp );
#else
                len = envp - _RWD_environ;
                if( _RWD_env_mask[len] ) {
                    lib_free( *envp );
                }
#endif
                /* shift string array entries */
                for( tmp_envp = envp; *tmp_envp != NULL; tmp_envp++ ) {
                    *tmp_envp = *(tmp_envp + 1);
                }
#ifndef __WIDECHAR__
                /* shift flag array entries */
                envm = (char *)tmp_envp;
                while( len-- > 0 ) {
                    *envm++ = *_RWD_env_mask++;
                }
                _RWD_env_mask++;
                len = tmp_envp - envp;
                while( len-- > 0 ) {
                    *envm++ = *_RWD_env_mask++;
                }
                _RWD_env_mask = (char *)tmp_envp;
#endif
                envp--;
                break;      /* delete more entries */
            }
#if defined( __UNIX__ )
            if( _TCSCMP( p1, p2 ) ) {
#else
            if( _TCSICMP( p1, p2 ) ) {      /* case independent search */
#endif
                break;
            }
        }
    }
    /* not found */
    return( __F_NAME(_RWD_environ,_RWD_wenviron) - envp );
}

int __F_NAME(__findenvadd,__wfindenvadd)( const CHAR_TYPE *env_string )
/*
 * find entry, if not exist then allocate new one
 * return zero based index for entry
 * or -1 if error
 */
{
    CHAR_TYPE       **envp;
    int             index;

    envp = __F_NAME(_RWD_environ,_RWD_wenviron);
    if( envp == NULL ) {
        envp = lib_malloc( ENVARR_SIZE( 1 ) );
        if( envp == NULL )
            return( -1 );
        index = 0;
        envp[0] = NULL;
    } else {
        index = __F_NAME(findenv,wfindenv)( env_string, 0 );
        if( index > 0 ) {
            /* name found, return 0 based entry index */
            return( index - 1 );
        }
        /* name not found */
        index = -index;
        envp = lib_realloc( envp, ENVARR_SIZE( index + 1 ) );
        if( envp == NULL )
            return( -1 );
#ifndef __WIDECHAR__
        memmove( &envp[index + 2], &envp[index + 1], index );
#endif
    }
#ifndef __WIDECHAR__
    _RWD_env_mask = (char *)&envp[index + 2];
    _RWD_env_mask[index] = 0;
#endif
    __F_NAME(_RWD_environ,_RWD_wenviron) = envp;
    __F_NAME(_RWD_environ,_RWD_wenviron)[index + 1] = NULL;
    return( index );
}

int __F_NAME(__findenvdel,__wfindenvdel)( const CHAR_TYPE *env_string )
/*
 * find all entries and delete them
 *
 * return 0 if succeded
 * otherwise return -1
 */
{
    if( __F_NAME(_RWD_environ,_RWD_wenviron) != NULL )
        __F_NAME(findenv,wfindenv)( env_string, 1 );
    return( 0 );
}
