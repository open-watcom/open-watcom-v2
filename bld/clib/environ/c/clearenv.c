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
#include <stdlib.h>
#include "liballoc.h"
#include "rtdata.h"

_WCRTLINK int (clearenv)( void ) {
    #ifndef __NETWARE__
        char **envp;
        char *env_str;
        int index;

        if( _RWD_environ != NULL ) {
            for( envp = _RWD_environ; env_str = *envp; ++envp ) {
                if( _RWD_env_mask != NULL ) {
                    index = envp - _RWD_environ;
                    if( _RWD_env_mask[ index ] != 0 ) {
                        lib_free( (void *)env_str );
                    }
                    *envp = NULL;
                }
            }
            if( _RWD_env_mask == NULL ) {
                envp = lib_malloc( sizeof(char *) + sizeof(char) );
            } else {
                envp = lib_realloc( _RWD_environ, sizeof(char *) + sizeof(char) );
            }
            if( envp == NULL ) return( -1 );
            _RWD_environ = envp;
            *_RWD_environ = NULL;
            _RWD_env_mask = ((char *)envp)+sizeof(char *);
            *_RWD_env_mask = 0;
        }
    #endif
    return( 0 );                /* success */
}
