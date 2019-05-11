/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2017 The Open Watcom Contributors. All Rights Reserved.
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


//
// FGETENV      : get value of environment variable
//

#include "ftnstd.h"
#include <string.h>
#include <process.h>
#include "pgmacc.h"
#include "ftnapi.h"
#include "walloca.h"


intstar4        __fortran FGETENV( string PGM *env_var, string PGM *value ) {
//=========================================================================

    char        *buff;
    char        *ptr;
    uint        len;

    for( len = 0; len < env_var->len; ++len ) {
        if( env_var->strptr[ len ] == ' ' ) {
            break;
        }
    }
    if( len == 0 )
        return( 0 );
    buff = alloca( len + sizeof( char ) );
    if( buff != NULL ) {
        pgm_memget( buff, env_var->strptr, len );
        buff[ len ] = NULLCHAR;
        ptr = getenv( buff );
        if( ptr != NULL ) {
            len = strlen( ptr );
            if( len > value->len ) {
                len = value->len;
            }
            pgm_memput( value->strptr, ptr, len );
            pgm_memset( value->strptr + len, ' ', value->len - len );
            return( len );
        }
    }
    return( 0 );
}
