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

mx_entry TRAPFAR    *Out_Mx_Ptr;

#define OUTPTR( type, name ) type *name = Out_Mx_Ptr->ptr;

extern unsigned (TRAPENTRY *ReqFunc)( unsigned, mx_entry *,
                                        unsigned, mx_entry * );

unsigned ReqConnect( void )
{
    OUTPTR( connect_ret, ret );

    ret->max_msg_size = 0xFFFF;
    return( sizeof( *ret ) );
}

unsigned ReqSimpleStub( void )
{
    return( 0 );
}

unsigned ReqGet_supplementary_service(void)
{
    OUTPTR( get_supplementary_service_ret, ret );

    ret->err = 0;
    ret->id = 0;
    return( sizeof( *ret ) );
}

unsigned ReqMap_addr( void )
{
    OUTPTR( map_addr_ret, ret );

    ret->out_addr.offset = 0;
    ret->out_addr.segment = 0;
    ret->lo_bound = 0;
    ret->hi_bound = ~(addr48_off)0;
    return( sizeof( *ret ) );
}

unsigned ReqChecksum_mem( void )
{
    OUTPTR( checksum_mem_ret, ret );

    ret->result = 0;
    return( sizeof( *ret ) );
}

unsigned ReqProg_load( void )
{
    OUTPTR( prog_load_ret, ret );

    ret->err = 2; // file not found
    return( sizeof( *ret ) );
}

unsigned ReqProg_kill( void )
{
    OUTPTR( prog_kill_ret, ret );

    ret->err = 0;
    return( sizeof( *ret ) );
}

unsigned ReqSet_watch( void )
{
    OUTPTR( set_watch_ret, ret );

    ret->multiplier = 100000;
    ret->err = 1;
    return( sizeof( *ret ) );
}

unsigned ReqSet_break( void )
{
    OUTPTR( set_break_ret, ret );

    ret->old = 0;
    return( sizeof( *ret ) );
}

unsigned ReqGet_next_alias( void )
{
    OUTPTR( get_next_alias_ret, ret );

    ret->seg = 0;
    ret->alias = 0;
    return( sizeof( *ret ) );
}

unsigned ReqRead_user_keyboard( void )
{
    OUTPTR( read_user_keyboard_ret, ret );

    ret->key = ' ';
    return( sizeof( *ret ) );
}

unsigned ReqGet_lib_name( void )
{
    OUTPTR( get_lib_name_ret, ret );

    ret->handle = 0;
    return( sizeof( *ret ) );
}

unsigned ReqRedirect_stdin( void  )
{
    OUTPTR( redirect_stdin_ret, ret );

    ret->err = 1;
    return( sizeof( *ret ) );
}

unsigned ReqRedirect_stdout( void  )
{
    OUTPTR( redirect_stdout_ret, ret );

    ret->err = 1;
    return( sizeof( *ret ) );
}


unsigned ReqSplit_cmd( void )
{
    OUTPTR( split_cmd_ret, ret );

    ret->parm_start = 0;
    ret->cmd_end = 0;
    return( sizeof( *ret ) );
}

static unsigned (* const DumbRequests[])(void) = {
        ReqConnect,
        ReqSimpleStub, // ReqDisconnect,
        ReqSimpleStub, // ReqSuspend,
        ReqSimpleStub, // ReqResume,
        ReqGet_supplementary_service,
        ReqSimpleStub, // ReqPerform_supplementary_service,
        ReqSimpleStub, // ReqGet_sys_config,
        ReqMap_addr,
        ReqSimpleStub, // ReqAddr_info, //obsolete
        ReqChecksum_mem,
        ReqSimpleStub, // ReqRead_mem,
        ReqSimpleStub, // ReqWrite_mem,
        ReqSimpleStub, // ReqRead_io,
        ReqSimpleStub, // ReqWrite_io,
        ReqSimpleStub, // ReqRead_cpu,  //obsolete
        ReqSimpleStub, // ReqRead_fpu,  //obsolete
        ReqSimpleStub, // ReqWrite_cpu, //obsolete
        ReqSimpleStub, // ReqWrite_fpu, //obsolete
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


unsigned TRAPENTRY DumbRequest( unsigned num_in_mx, mx_entry *mx_in,
                            unsigned num_out_mx, mx_entry *mx_out )
{
    unsigned    len;

    Out_Mx_Ptr = mx_out;
    len = DumbRequests[ *(access_req *)mx_in[0].ptr ]();
    return( len );
}

char *LoadDumbTrap( trap_version *ver )
{
    ReqFunc = DumbRequest;
    ver->major = TRAP_MAJOR_VERSION;
    ver->minor = TRAP_MINOR_VERSION;
    ver->remote = FALSE;
    return( NULL );
}
