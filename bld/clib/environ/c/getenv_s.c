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
* Description:  Implementation of getenv_s() - bounds checking getenv().
*
****************************************************************************/


#include "variety.h"
#include "saferlib.h"
#include <string.h>


_WCRTLINK extern errno_t getenv_s( size_t * __restrict len, char * __restrict value,
                                   rsize_t maxsize, const char * __restrict name )
/**********************************************************************************/
{
    char        *env_str;
    size_t      env_str_len = 0;
    errno_t     rc = -1;

    /* Pre-set length *before* calling runtime-constraint handler! */
    if( len != NULL ) {
        *len = 0;
    }

    /* Verify runtime-constraints */
    if( __check_constraint_nullptr( name ) &&
        __check_constraint_maxsize( maxsize ) &&
        ((maxsize == 0) || __check_constraint_nullptr( value )) ) {

        /* Make sure destination string is always terminated */
        if( maxsize > 0 ) {
            *value = '\0';
        }

        env_str = getenv( name );
        if( env_str != NULL ) {
            /* Env var found, find out its size */
            env_str_len = strlen( env_str );
            if( len != NULL ) {
                *len = env_str_len;
            }
            if( env_str_len < maxsize ) {
                /* Target large enough; safe to copy */
                strcpy( value, env_str );
                rc = 0;
            }
        }
    }
    return( rc );
}
