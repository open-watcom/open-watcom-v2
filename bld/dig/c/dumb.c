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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#include <string.h>
#include "trpcore.h"
#include "trpld.h"


#define TRAP_DUMB(a)        TRAP_SYM( Dumb, a )

mx_entry_p  Out_Mx_Ptr;

#define OUTPTR( type, name ) type *name = Out_Mx_Ptr->ptr;

static trap_retval TRAP_DUMB( Connect )( void )
{
    OUTPTR( connect_ret, ret );

    ret->max_msg_size = 0xFFFF;
    return( sizeof( *ret ) );
}

static trap_retval TRAP_DUMB( EmptyStub )( void )
{
    return( 0 );
}

static trap_retval TRAP_DUMB( Get_supplementary_service )( void )
{
    OUTPTR( get_supplementary_service_ret, ret );

    ret->err = 0;
    ret->id = 0;
    return( sizeof( *ret ) );
}

static trap_retval TRAP_DUMB( Map_addr )( void )
{
    OUTPTR( map_addr_ret, ret );

    ret->out_addr.offset = 0;
    ret->out_addr.segment = 0;
    ret->lo_bound = 0;
    ret->hi_bound = ~(addr48_off)0;
    return( sizeof( *ret ) );
}

static trap_retval TRAP_DUMB( Checksum_mem )( void )
{
    OUTPTR( checksum_mem_ret, ret );

    ret->result = 0;
    return( sizeof( *ret ) );
}

static trap_retval TRAP_DUMB( Prog_load )( void )
{
    OUTPTR( prog_load_ret, ret );

    ret->err = 2; // file not found
    return( sizeof( *ret ) );
}

static trap_retval TRAP_DUMB( Prog_kill )( void )
{
    OUTPTR( prog_kill_ret, ret );

    ret->err = 0;
    return( sizeof( *ret ) );
}

static trap_retval TRAP_DUMB( Set_watch )( void )
{
    OUTPTR( set_watch_ret, ret );

    ret->multiplier = 100000;
    ret->err = 1;
    return( sizeof( *ret ) );
}

static trap_retval TRAP_DUMB( Set_break )( void )
{
    OUTPTR( set_break_ret, ret );

    ret->old = 0;
    return( sizeof( *ret ) );
}

static trap_retval TRAP_DUMB( Get_next_alias )( void )
{
    OUTPTR( get_next_alias_ret, ret );

    ret->seg = 0;
    ret->alias = 0;
    return( sizeof( *ret ) );
}

static trap_retval TRAP_DUMB( Read_user_keyboard )( void )
{
    OUTPTR( read_user_keyboard_ret, ret );

    ret->key = ' ';
    return( sizeof( *ret ) );
}

static trap_retval TRAP_DUMB( Get_lib_name )( void )
{
    OUTPTR( get_lib_name_ret, ret );

    ret->mod_handle = 0;
    return( sizeof( *ret ) );
}

static trap_retval TRAP_DUMB( Redirect_stdin )( void  )
{
    OUTPTR( redirect_stdin_ret, ret );

    ret->err = 1;
    return( sizeof( *ret ) );
}

static trap_retval TRAP_DUMB( Redirect_stdout )( void  )
{
    OUTPTR( redirect_stdout_ret, ret );

    ret->err = 1;
    return( sizeof( *ret ) );
}


static trap_retval TRAP_DUMB( Split_cmd )( void )
{
    OUTPTR( split_cmd_ret, ret );

    ret->parm_start = 0;
    ret->cmd_end = 0;
    return( sizeof( *ret ) );
}

static trap_retval (* const _dumbRequests[])( void ) = {
    #define pick(sym,dumbfunc,stdfunc)  TRAP_DUMB( dumbfunc ),
    #include "_trpreq.h"
    #undef pick
};


static trap_retval TRAPENTRY DumbRequest( trap_elen num_in_mx, in_mx_entry_p mx_in, trap_elen num_out_mx, mx_entry_p mx_out )
{
    trap_retval     result;

    /* unused parameters */ (void)num_in_mx; (void)num_out_mx;

    Out_Mx_Ptr = mx_out;
    result = _dumbRequests[TRP_REQUEST( mx_in )]();
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
