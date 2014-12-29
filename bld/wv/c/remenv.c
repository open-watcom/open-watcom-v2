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


#include <string.h>
#include "dbgdefn.h"
#include "dbgio.h"
#include "trpcore.h"
#include "trpenv.h"

extern trap_shandle     GetSuppId( char * );

#define SUPP_ENV_SERVICE( in, request )         \
        in.supp.core_req        = REQ_PERFORM_SUPPLEMENTARY_SERVICE;    \
        in.supp.id              = SuppEnvId;    \
        in.req                  = request;

static trap_shandle     SuppEnvId = 0;

bool InitEnvSupp( void )
{
    SuppEnvId = GetSuppId( ENV_SUPP_NAME );
    if( SuppEnvId != 0 )
        return( TRUE );
    return( FALSE );
}

bool RemoteSetEnvironmentVariable( char *name, char *value )
{
    in_mx_entry         in[3];
    mx_entry            out[1];
    env_set_var_req     acc;
    env_set_var_ret     ret;

    if( SuppEnvId == 0 ) return( FALSE );
    SUPP_ENV_SERVICE( acc, REQ_ENV_SET_VAR );
    in[0].ptr = &acc;
    in[0].len = sizeof( acc );
    in[1].ptr = name;
    in[1].len = strlen( name ) + 1;
    in[2].ptr = value;
    in[2].len = strlen( value ) + 1;
    out[0].ptr = &ret;
    out[0].len = sizeof( ret );
    TrapAccess( 3, in, 1, out );
    if( ret.err != 0 ) {
        StashErrCode( ret.err, OP_REMOTE );
        return( FALSE );
    } else {
        return( TRUE );
    }
}

bool RemoteGetEnvironmentVariable( char *name, char *res, int res_len )
{
    in_mx_entry         in[2];
    mx_entry            out[2];
    env_get_var_req     acc;
    env_get_var_ret     ret;

    if( SuppEnvId == 0 ) return( FALSE );
    SUPP_ENV_SERVICE( acc, REQ_ENV_GET_VAR );
    acc.res_len = res_len;
    in[0].ptr = &acc;
    in[0].len = sizeof( acc );
    in[1].ptr = name;
    in[1].len = strlen( name ) + 1;
    out[0].ptr = &ret;
    out[0].len = sizeof( ret );
    out[1].ptr = res;
    out[1].len = res_len;
    TrapAccess( 2, in, 2, out );
    if( ret.err != 0 ) {
        StashErrCode( ret.err, OP_REMOTE );
        return( FALSE );
    } else {
        return( TRUE );
    }
}
