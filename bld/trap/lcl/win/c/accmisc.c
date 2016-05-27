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


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <direct.h>
#include <ctype.h>
#include <dos.h>
#include "stdwin.h"
#include "wdebug.h"
#include "trperr.h"
#include "madx86.h"
#include "x86cpu.h"
#include "di386cli.h"


BOOL IsSegSize32( WORD seg )
{
    WORD        desc[4];

    if( WDebug386 ) {
        GetDescriptor( seg, desc );
        if( desc[3] & 0x40 ) {
            return( TRUE );
        }
    }
    return( FALSE );
}

trap_retval ReqMachine_data( void )
{
    machine_data_req    *acc;
    machine_data_ret    *ret;
    unsigned_8          *data;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    data = GetOutPtr( sizeof( *ret ) );
    ret->cache_start = 0;
    ret->cache_end = ~(addr_off)0;
    *data = 0;
    if( IsSegSize32( acc->addr.segment ) ) {
        *data |= X86AC_BIG;
    }
    return( sizeof( *ret ) + sizeof( *data ) );
}

trap_retval ReqGet_sys_config( void )
{
    unsigned_8          fpu;
    get_sys_config_ret  *ret;

    ret = GetOutPtr( 0 );

    ret->sys.os = MAD_OS_WINDOWS;
    ret->sys.osmajor = _osmajor;
    ret->sys.osminor = _osminor;

    if( WindowsFlags & WF_CPU086 ) {
        ret->sys.cpu = X86_86;
        fpu = X86_87;
    } else if( WindowsFlags & WF_CPU186 ) {
        ret->sys.cpu = X86_186;
        fpu = X86_87;
    } else if( WindowsFlags & WF_CPU286 ) {
        ret->sys.cpu = X86_286;
        fpu = X86_287;
    } else {
        ret->sys.cpu = X86CPUType();
        fpu = ret->sys.cpu & X86_CPU_MASK;
    }

    if( WindowsFlags & WF_80x87 ) {
        FPUType = fpu;
    } else {
        FPUType = X86_NO;
    }
    ret->sys.fpu = FPUType;
    ret->sys.mad = MAD_X86;
    ret->sys.huge_shift = 3;
    return( sizeof( *ret ) );
}

trap_retval ReqGet_message_text( void )
{
    static const char * const ExceptionMsgs[] = {
        #define pick(a,b) b,
        #include "x86exc.h"
        #undef pick
    };
    char                    *err_txt;
    get_message_text_ret    *ret;
    unsigned                len;

    ret = GetOutPtr( 0 );
    ret->flags = MSG_NEWLINE | MSG_ERROR;
    err_txt = GetOutPtr( sizeof( *ret ) );
    if( OutPos != 0 ) {
        len = strlen( OutBuff ) + 1;
        memcpy( err_txt, OutBuff, len );
        OutPos -= len;
        memmove( &OutBuff[0], &OutBuff[len], OutPos );
        if( OutPos != 0 ) {
            ret->flags |= MSG_MORE;
        }
    } else if( IntResult.InterruptNumber < sizeof( ExceptionMsgs ) / sizeof( ExceptionMsgs[0] ) ) {
        strcpy( err_txt, ExceptionMsgs[IntResult.InterruptNumber] );
    } else {
        strcpy( err_txt, TRP_EXC_unknown );
    }
    return( sizeof( ret ) + strlen( err_txt ) + 1 );
}

trap_retval ReqRead_io( void )
{
    return( 0 );
}

trap_retval ReqWrite_io( void )
{
    write_io_ret        *ret;

    ret = GetOutPtr(0);
    ret->len = 0;
    return( sizeof( *ret ) );
}

trap_retval ReqSet_user_screen( void )
{
    return( 0 );
}

trap_retval ReqSet_debug_screen( void )
{
    return( 0 );
}
