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
* Description:  Implementation of remote access routines.
*
****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <direct.h>
#include "stdrdos.h"
#include "rdos.h"
#include "debug.h"
#include "trperr.h"
#include "madregs.h"
#include "x86cpu.h"

trap_elen ReqAddr_info( void )
{
    WORD            seg;
    addr_info_req   *acc;
    addr_info_ret   *ret;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    seg = acc->in_addr.segment;
    ret->is_big = TRUE;
    return( sizeof( *ret ) );
}

trap_elen ReqMachine_data()
{
    int                 sel;
    int                 size;
    int                 bitness;
    machine_data_req    *acc;
    machine_data_ret    *ret;
    union {
        unsigned_8      u8;
    }                   *data;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    data = GetOutPtr( sizeof( *ret ) );

    sel = acc->addr.segment;

    if( RdosGetSelectorInfo( sel, &size, &bitness ) ) {
        if( size > 0xFFFF )
            bitness = 32;
        ret->cache_start = 0;
        ret->cache_end = size;
        if( bitness == 16 )            
            data->u8 = 0;
        else
            data->u8 = 1;
    } else {
        ret->cache_start = 0;
        ret->cache_end = 0xFFFFFFFF;
        data->u8 = 1;
    }
    return( sizeof( *ret ) + sizeof( data->u8 ) );
}

trap_elen ReqGet_sys_config( void )
{
    get_sys_config_ret  *ret;
    int                 major, minor, release;

        RdosGetVersion(&major, &minor, &release);

    ret = GetOutPtr( 0 );

    ret->sys.cpu = 0x3F;
    ret->sys.fpu = 0xF;
    ret->sys.osmajor = (char)major;
    ret->sys.osminor = (char)minor;
    ret->sys.os = 0;
    ret->sys.huge_shift = 3;
    ret->sys.mad = MAD_X86;

    return( sizeof( *ret ) );
}

trap_elen ReqGet_message_text( void )
{
    get_message_text_ret    *ret;
    char                    *err_txt;
    struct TDebug           *obj;
    char                    *msg;

    obj = GetCurrentDebug();

    ret = GetOutPtr( 0 );
    ret->flags = MSG_ERROR;
    err_txt = GetOutPtr( sizeof( *ret ) );

    if( obj->CurrentThread )
        msg = obj->CurrentThread->FaultText;    
    else
        msg = "Exception fault";

    strcpy( err_txt, msg );

    return( sizeof( *ret ) + strlen( err_txt ) + 1 );
}

trap_elen ReqGet_next_alias( void )
{
    get_next_alias_ret  *ret;

    ret = GetOutPtr( 0 );
    ret->seg = 0;
    ret->alias = 0;
    return( sizeof( *ret ) );
}

trap_elen ReqGet_err_text( void )
{
    get_err_text_req    *acc;
    char                *err_txt;
    char                *msg;

    acc = GetInPtr( 0 );
    err_txt = GetOutPtr( 0 );

    switch (acc->err)
    {
        case MSG_LOAD_FAIL:
            msg = "Cannot load program";
            break;

        case MSG_FILE_NOT_FOUND:
            msg = "File not found";
            break;

        case MSG_NO_THREAD:
            msg = "Thread not found";
            break;

        case MSG_FILE_MODE_ERROR:
            msg = "Invalid seek mode";
            break;

        case MSG_NOT_FOUND:
            msg = "Not found";
            break;

        default:
            msg = "Unknown error";
            break;
    }
    strcpy( err_txt, msg );

    return( strlen( err_txt ) + 1 );
}

trap_elen ReqSplit_cmd( void )
{
    char            *cmd;
    char            *start;
    split_cmd_ret   *ret;
    unsigned        len;

    cmd = GetInPtr( sizeof( split_cmd_req ) );
    len = GetTotalSize() - sizeof( split_cmd_req );
    start = cmd;
    ret = GetOutPtr( 0 );
    ret->parm_start = 0;
    for( ;; ) {
        if( len == 0 )
            goto done;
        switch( *cmd ) {
        case '/':
        case '=':
        case '(':
        case ';':
        case ',':
            goto done;
        case '\0':
        case ' ':
        case '\t':
            ret->parm_start = 1;
            goto done;
        case '\"':
            while( --len && (*++cmd != '\"') )
                ;
            if( len == 0 ) {
                ret->parm_start = 1;
                goto done;
            }
        }
        ++cmd;
        --len;
    }
done:
    ret->parm_start += cmd - start;
    ret->cmd_end = cmd - start;
    return( sizeof( *ret ) );
}

trap_elen ReqRead_io( void )
{
    return( 0 );
}

trap_elen ReqWrite_io( void )
{
    write_io_ret    *ret;

    ret = GetOutPtr( 0 );
    ret->len = 0;
    return( sizeof( *ret ) );
}

trap_elen ReqSet_user_screen( void )
{
    SetUserScreen();
    return( 0 );
}

trap_elen ReqSet_debug_screen( void )
{
    SetDebugScreen();
    return( 0 );
}
