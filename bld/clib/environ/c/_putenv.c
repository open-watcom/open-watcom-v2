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
* Description:  Implementation for __(w)putenv().
*
****************************************************************************/


#include "widechar.h"
#include <string.h>
#include <ctype.h>
#include <mbstring.h>
#include <errno.h>
#ifdef __WIDECHAR__
    #include <wctype.h>
#endif
#ifdef __NT__
    #include <windows.h>
    #include "libwin32.h"
#elif defined( __RDOS__ ) || defined( __RDOSDEV__ )
    #include <rdos.h>
#endif
#include "liballoc.h"
#include "rtdata.h"
#include "seterrno.h"
#include "_environ.h"


int __F_NAME(__putenv,__wputenv)( const CHAR_TYPE *env_string )
{
#ifdef __NETWARE__
    env_string = env_string;
    return( -1 );
#else
    int                 index = -1;
    const CHAR_TYPE     *p;
    int                 delete_var;

    if( env_string == NULL || _TCSTERM( env_string ) )
        return( index );
    // handle NAME=STRING
    for( p = _TCSINC( env_string ); !_TCSTERM( p ); p = _TCSINC( p ) ) {    // (used under NT)
        if( _TCSNEXTC( p ) == STRING( '=' ) ) {
            break;
        }
    }
    if( _TCSTERM( p ) )
        return( index ); /* <name> with no '=' is illegal */
    delete_var = ( _TCSTERM( _TCSINC( p ) ) );
    index = __F_NAME(__findenv,__wfindenv)( env_string, delete_var );
    if( delete_var )
        return( 0 );
    if( index == -1 ) {
        return( index );
    }
    if( index > 0 ) {
        ((const CHAR_TYPE **)__F_NAME(_RWD_environ,_RWD_wenviron))[index - 1] = env_string;
  #ifndef __WIDECHAR__
        _RWD_env_mask[index - 1] = 0;     /* indicate not alloc'd */
  #endif
    }
    return( 0 );
#endif
}
