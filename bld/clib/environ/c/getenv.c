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
#include <mbstring.h>
#include <stdlib.h>
#include <string.h>
#include "rtdata.h"
#ifdef __WIDECHAR__
    #include "wenviron.h"
#endif

#if defined(__QNX__)
    #ifdef __WIDECHAR__
        #define CMP_FUNC        wcsncmp
    #else
        #define CMP_FUNC        strncmp
    #endif
#else
    #ifdef __WIDECHAR__
        #define CMP_FUNC        _wcsnicmp
    #else
        #define CMP_FUNC        _mbsnicmp
    #endif
#endif


_WCRTLINK CHAR_TYPE *__F_NAME(getenv,_wgetenv)( const CHAR_TYPE *name )
    {
#ifdef __NETWARE__
        name = name;
#else
        CHAR_TYPE **    envp;
        CHAR_TYPE *     p;
        int             len;

        #ifdef __WIDECHAR__
            if( _RWD_wenviron == NULL )  __create_wide_environment();
        #endif

        /*** Find the environment string ***/
        __ptr_check( name, 0 );
        envp = __F_NAME(_RWD_environ,_RWD_wenviron);
        if( envp != NULL  &&  name != NULL ) {
            len = __F_NAME(strlen,wcslen)( name );
            for( ; p = *envp; ++envp ) {
                if( CMP_FUNC( p, name, len ) == 0 ) {
                    if( p[len] == __F_NAME('=',L'=') )  return( &p[len+1] );
                }
            }
        }
#endif
        return( NULL );                 /* not found */
    }
