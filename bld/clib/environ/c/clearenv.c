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
* Description:  Implementation of clearenv() - clear the environment.
*
****************************************************************************/


#include "variety.h"
#include <stdlib.h>
#include <env.h>
#include "liballoc.h"
#include "rtdata.h"
#include "_environ.h"


/* Note - clearenv() is always called at library exit */

_WCRTLINK int (clearenv)( void )
{
#ifdef __NETWARE__
    return( 0 );                /* success */
#else
    int     index;
    int     rc;

    rc = 0;
    if( _RWD_environ != NULL ) {
        index = 0;
        while( _RWD_environ[index] != NULL ) {
            if( _RWD_env_mask[index] != 0 ) {
                lib_free( _RWD_environ[index] );
            }
            index++;
        }
        lib_free( _RWD_environ );
        _RWD_environ = lib_malloc( ENVARR_SIZE( 0 ) );
        if( _RWD_environ == NULL ) {
            _RWD_env_mask = NULL;
            rc = -1;
        } else {
            _RWD_environ[0] = NULL;
            _RWD_env_mask = (char *)&_RWD_environ[1];
        }
    }
  #ifdef CLIB_USE_OTHER_ENV
    if( _RWD_wenviron != NULL ) {
        index = 0;
        while( _RWD_wenviron[index] != NULL ) {
            lib_free( _RWD_wenviron[index] );
            index++;
        }
        lib_free( _RWD_wenviron );
        _RWD_wenviron = lib_malloc( ENVARR_SIZE( 0 ) );
        if( _RWD_wenviron == NULL ) {
            rc = -1;
        } else {
            _RWD_wenviron[0] = NULL;
        }
    }
  #endif
    return( rc );
#endif
}
