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
* Description:  Implementation of (_w)getenv().
*
****************************************************************************/


#include "widechar.h"
#include "variety.h"
#include <stdlib.h>
#include <mbstring.h>
#include <string.h>
#ifdef __WIDECHAR__
    #include <wctype.h>
#endif
#include "rtdata.h"
#include "_environ.h"

_WCRTLINK CHAR_TYPE *__F_NAME(getenv,_wgetenv)( const CHAR_TYPE *name )
{
#ifdef __NETWARE__
    name = name;
#else
    CHAR_TYPE       **envp;
    CHAR_TYPE       *p;

  #ifdef __WIDECHAR__
    if( _RWD_wenviron == NULL ) {
        __create_wide_environment();
    }
  #endif

    /*** Find the environment string ***/
    __ptr_check( name, 0 );
    envp = __F_NAME(_RWD_environ,_RWD_wenviron);
    if( (envp != NULL) && (name != NULL) ) {
        for( ; p = *envp; ++envp ) {
            const CHAR_TYPE     *s = name;

            while( !_TCSTERM( p ) ) {
                if( _TCSTERM( s ) ) {
                    if( _TCSNEXTC( p ) == STRING( '=' ) )
                        return( _TCSINC( p ) );
                    break;
                }
  #if defined(__UNIX__)
                if( _TCSCMP( p, s ) )
  #else
                if( _TCSICMP( p, s ) )
  #endif
                    break;
                p = _TCSINC( p );   /* skip over character */
                s = _TCSINC( s );   /* skip over character */
            }
        }
    }
#endif
    return( NULL );                 /* not found */
}
