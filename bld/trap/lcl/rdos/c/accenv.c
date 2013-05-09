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

#include <io.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include "stdrdos.h"
#include "rdos.h"

trap_elen ReqEnv_setvar( void )
{
    env_set_var_req *req;
    env_set_var_ret *ret;
    char            *var;
    char            *value;
    int             handle;

    req = GetInPtr( 0 );
    var = GetInPtr( sizeof( *req ) );
    value = GetInPtr( sizeof( *req ) + strlen( var ) + 1 );
    ret = GetOutPtr( 0 );

    ret->err = 0;

    if( value[0] == '\0' )
        value = NULL;

    handle = RdosOpenProcessEnv();
    RdosDeleteEnvVar( handle, var );

    if( value )
        RdosAddEnvVar( handle, var, value );   

    RdosCloseEnv( handle );

    return( sizeof( *ret ) );
}

trap_elen ReqEnv_getvar( void )
{
    env_get_var_req *req;
    env_get_var_ret *ret;
    char            *var;
    char            *value;
    int             handle;

    req = GetInPtr( 0 );
    var = GetInPtr( sizeof( *req ) );
    ret = GetOutPtr( 0 );
    value = GetOutPtr( sizeof( *ret ) );
    ret->err = 0;

    handle = RdosOpenProcessEnv();
    RdosDeleteEnvVar( handle, var );

    if( !RdosFindEnvVar( handle, var, value ) ) {
        ret->err = MSG_NOT_FOUND;
        return( sizeof( *ret ) );
    }
    
    return( sizeof( *ret ) + strlen( value ) + 1 );
}
