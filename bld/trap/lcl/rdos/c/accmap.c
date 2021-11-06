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
* Description:  Routines to keep track of loaded modules and address maps.
*
****************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stdrdos.h"
#include "debug.h"

trap_retval TRAP_CORE( Map_addr )( void )
{
    map_addr_req        *acc;
    map_addr_ret        *ret;
    struct TDebug       *obj;
        struct TDebugModule *mod = 0;
        int                  sel;
        int                  offset;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );

    sel = acc->in_addr.segment;
    offset = acc->in_addr.offset;

    obj = GetCurrentDebug();

    if (obj) {
        mod = LockModule( obj, acc->mod_handle );
        if( mod ) {
            switch ( sel ) {
            case MAP_FLAT_CODE_SELECTOR:
                ret->out_addr.segment = 0x1B3;
                ret->out_addr.offset = mod->ImageBase + mod->ObjectRva + offset;
                ret->lo_bound = 0;
                ret->hi_bound = mod->ImageSize - 1;
                break;
            case MAP_FLAT_DATA_SELECTOR:
                ret->out_addr.segment = 0x1BB;
                ret->out_addr.offset = mod->ImageBase + mod->ObjectRva + offset;
                ret->lo_bound = 0;
                ret->hi_bound = mod->ImageSize - 1;
                break;
            case 0:
            case 1:
                ret->out_addr.segment = mod->CodeSel;
                ret->out_addr.offset = offset;
                ret->lo_bound = 0;
                ret->hi_bound = mod->ImageSize - 1;
                break;
            case 2:
            case 3:
                ret->out_addr.segment = mod->DataSel;
                ret->out_addr.offset = offset;
                ret->lo_bound = 0;
                if( mod->DataSel && mod->DataSize )
                    ret->hi_bound = mod->DataSize - 1;
                else
                    ret->hi_bound = 0;
                break;
            default:
                if( sel == mod->CodeSel ) {
                    ret->out_addr.segment = mod->CodeSel;
                    ret->out_addr.offset = offset;
                    ret->lo_bound = 0;
                    ret->hi_bound = mod->ImageSize - 1;
                    break;
                }
                if( sel == mod->DataSel ) {
                    ret->out_addr.segment = mod->DataSel;
                    ret->out_addr.offset = offset;
                    ret->lo_bound = 0;
                    if( mod->DataSel && mod->DataSize )
                        ret->hi_bound = mod->DataSize - 1;
                    else
                        ret->hi_bound = 0;
                    break;
                }
                ret->out_addr.segment = 0;
                ret->out_addr.offset = offset;
                ret->lo_bound = 0;
                ret->hi_bound = 0;
                break;
            }
        }
        UnlockModule( obj );
    }

    if( mod )
        return( sizeof( *ret ) );
    else
        return( 0 );
}

/*
 * AccGetLibName - get lib name of current module
 */
trap_retval TRAP_CORE( Get_lib_name )( void )
{
    get_lib_name_req    *acc;
    get_lib_name_ret    *ret;
    char                *name;
    struct TDebug       *obj;
    struct TDebugModule *mod;
    size_t              max_len;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );

    obj = GetCurrentDebug();
    if( obj != NULL ) {
        ret->mod_handle = GetNextModule( obj, acc->mod_handle );
        if( ret->mod_handle != 0 ) {
            name = GetOutPtr( sizeof( *ret ) );
            *name = '\0';
            mod = LockModule( obj, ret->mod_handle );
            if( mod != NULL ) {
                max_len = GetTotalSizeOut() - 1 - sizeof( *ret );
                strncpy( name, mod->ModuleName, max_len );
                name[max_len] = '\0';
            }
            UnlockModule( obj );
            return( sizeof( *ret ) + strlen( name ) + 1 );
        }
    }
    ret->mod_handle = 0;
    return( sizeof( *ret ) );
}
