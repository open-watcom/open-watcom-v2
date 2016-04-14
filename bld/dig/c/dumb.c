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
#include "trpimp.h"
#include "trpld.h"

mx_entry_p  Out_Mx_Ptr;

#define OUTPTR( type, name ) type *name = Out_Mx_Ptr->ptr;

trap_retval ReqConnect( void )
{
    OUTPTR( connect_ret, ret );

    ret->max_msg_size = 0xFFFF;
    return( sizeof( *ret ) );
}

static trap_retval ReqSimpleStub( void )
{
    return( 0 );
}

trap_retval ReqGet_supplementary_service(void)
{
    OUTPTR( get_supplementary_service_ret, ret );

    ret->err = 0;
    ret->id = 0;
    return( sizeof( *ret ) );
}

trap_retval ReqMap_addr( void )
{
    OUTPTR( map_addr_ret, ret );

    ret->out_addr.offset = 0;
    ret->out_addr.segment = 0;
    ret->lo_bound = 0;
    ret->hi_bound = ~(addr48_off)0;
    return( sizeof( *ret ) );
}

trap_retval ReqChecksum_mem( void )
{
    OUTPTR( checksum_mem_ret, ret );

    ret->result = 0;
    return( sizeof( *ret ) );
}

trap_retval ReqProg_load( void )
{
    OUTPTR( prog_load_ret, ret );

    ret->err = 2; // file not found
    return( sizeof( *ret ) );
}

trap_retval ReqProg_kill( void )
{
    OUTPTR( prog_kill_ret, ret );

    ret->err = 0;
    return( sizeof( *ret ) );
}

trap_retval ReqSet_watch( void )
{
    OUTPTR( set_watch_ret, ret );

    ret->multiplier = 100000;
    ret->err = 1;
    return( sizeof( *ret ) );
}

trap_retval ReqSet_break( void )
{
    OUTPTR( set_break_ret, ret );

    ret->old = 0;
    return( sizeof( *ret ) );
}

trap_retval ReqGet_next_alias( void )
{
    OUTPTR( get_next_alias_ret, ret );

    ret->seg = 0;
    ret->alias = 0;
    return( sizeof( *ret ) );
}

trap_retval ReqRead_user_keyboard( void )
{
    OUTPTR( read_user_keyboard_ret, ret );

    ret->key = ' ';
    return( sizeof( *ret ) );
}

trap_retval ReqGet_lib_name( void )
{
    OUTPTR( get_lib_name_ret, ret );

    ret->handle = 0;
    return( sizeof( *ret ) );
}

trap_retval ReqRedirect_stdin( void  )
{
    OUTPTR( redirect_stdin_ret, ret );

    ret->err = 1;
    return( sizeof( *ret ) );
}

trap_retval ReqRedirect_stdout( void  )
{
    OUTPTR( redirect_stdout_ret, ret );

    ret->err = 1;
    return( sizeof( *ret ) );
}


trap_retval ReqSplit_cmd( void )
{
    OUTPTR( split_cmd_ret, ret );

    ret->parm_start = 0;
    ret->cmd_end = 0;
    return( sizeof( *ret ) );
}

static trap_retval (* const DumbRequests[])(void) = {
    ReqConnect,
    ReqSimpleStub, // ReqDisconnect,
    ReqSimpleStub, // ReqSuspend,
    ReqSimpleStub, // ReqResume,
    ReqGet_supplementary_service,
    ReqSimpleStub, // ReqPerform_supplementary_service,
    ReqSimpleStub, // ReqGet_sys_config,
    ReqMap_addr,
    ReqChecksum_mem,
    ReqSimpleStub, // ReqRead_mem,
    ReqSimpleStub, // ReqWrite_mem,
    ReqSimpleStub, // ReqRead_io,
    ReqSimpleStub, // ReqWrite_io,
    ReqSimpleStub, // ReqProg_go,
    ReqSimpleStub, // ReqProg_step,
    ReqProg_load,
    ReqProg_kill,
    ReqSet_watch,
    ReqSimpleStub, // ReqClear_watch,
    ReqSet_break,
    ReqSimpleStub, // ReqClear_break,
    ReqGet_next_alias,
    ReqSimpleStub, // ReqSet_user_screen,
    ReqSimpleStub, // ReqSet_debug_screen,
    ReqRead_user_keyboard,
    ReqGet_lib_name,
    ReqSimpleStub, // ReqGet_err_text,
    ReqSimpleStub, // ReqGet_message_text,
    ReqRedirect_stdin,
    ReqRedirect_stdout,
    ReqSplit_cmd,
    ReqSimpleStub, // ReqRead_regs,
    ReqSimpleStub, // ReqWrite_regs,
    ReqSimpleStub, // ReqMachine_data,
};


static trap_retval TRAPENTRY DumbRequest( trap_elen num_in_mx, in_mx_entry_p mx_in, trap_elen num_out_mx, mx_entry_p mx_out )
{
    trap_retval     result;

    num_in_mx = num_in_mx; num_out_mx = num_out_mx;
    Out_Mx_Ptr = mx_out;
    result = DumbRequests[TRP_REQUEST( mx_in )]();
    return( result );
}

char *LoadDumbTrap( trap_version *ver )
{
    ReqFunc = DumbRequest;
    ver->major = TRAP_MAJOR_VERSION;
    ver->minor = TRAP_MINOR_VERSION;
    ver->remote = false;
    return( NULL );
}
