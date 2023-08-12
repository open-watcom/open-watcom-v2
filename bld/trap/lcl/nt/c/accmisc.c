/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2015-2023 The Open Watcom Contributors. All Rights Reserved.
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
#include "digcpu.h"
#include "stdnt.h"
#include "globals.h"
#include "trperr.h"
#include "madregs.h"
#if MADARCH & MADARCH_X86
#include "x86cpu.h"
#endif


bool GetSelectorLDTEntry( WORD sel, LDT_ENTRY *ldt )
{
    thread_info *ti;

    ti = FindThread( DebugeeTid );
    if( ti != NULL ) {
        if( GetThreadSelectorEntry( ti->thread_handle, sel, ldt ) ) {
            return( true );
        }
    }
    return( false );
}

bool IsBigSel( WORD sel )
{
#if MADARCH & (MADARCH_AXP | MADARCH_PPC)
    return( true );
#elif MADARCH & (MADARCH_X86 | MADARCH_X64)
    LDT_ENTRY   ldt;

    if( sel == FlatCS || sel == FlatDS ) {
        return( true );
    }
    if( GetSelectorLDTEntry( sel, &ldt ) != NULL )
        return( ldt.HighWord.Bits.Default_Big );
    return( true );
#else
   #error IsBigSel not configured
#endif
}

#if MADARCH & MADARCH_AXP
typedef struct {
    unsigned_32 beg_addr;
    unsigned_32 end_addr;
    unsigned_32 except_handler;
    unsigned_32 handler_data;
    unsigned_32 pro_end_addr;
} nt_pdata_struct;

bool FindPData( addr_off off, axp_pdata_struct *axp_pdata )
{
    nt_pdata_struct nt_pdata;
    LPVOID          tbl;
    addr_off        size;
    DWORD           bytes;

    if( !FindExceptInfo( off, &tbl, &size ) ) {
        return( false );
    }
    for( ;; ) {
        if( size == 0 ) {
            return( false );
        }
        ReadProcessMemory( ProcessInfo.process_handle, tbl, (LPVOID)&nt_pdata, sizeof( nt_pdata ), &bytes );
        if( bytes != sizeof( nt_pdata ) ) {
            return( false );
        }
        if( off >= nt_pdata.beg_addr && off < nt_pdata.end_addr ) {
            /*
             *  This is an optimization - if the prologue end addr is not
             *  in the exception start/end range, this is not the entry
             *  for the start of the procedure and the MAD isn't interested.
             *  Keep looking for real one.
             */
            if( nt_pdata.pro_end_addr >= nt_pdata.beg_addr &&
                    nt_pdata.pro_end_addr < nt_pdata.end_addr ) {
                break;
            }
        }
        tbl = (LPVOID)( (DWORD)tbl + sizeof( nt_pdata ) );
        size -= sizeof( nt_pdata );
    }
    axp_pdata->beg_addr.u._32[0] = nt_pdata.beg_addr;
    axp_pdata->end_addr.u._32[0] = nt_pdata.end_addr;
    axp_pdata->except_handler.u._32[0] = nt_pdata.except_handler;
    axp_pdata->handler_data.u._32[0] = nt_pdata.handler_data;
    axp_pdata->pro_end_addr.u._32[0] = nt_pdata.pro_end_addr;
    return( true );
}
#endif

trap_retval TRAP_CORE( Machine_data )( void )
{
    machine_data_req    *acc;
    machine_data_ret    *ret;
    machine_data_spec   *data;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
#if MADARCH & MADARCH_X86
    ret->cache_start = 0;
    ret->cache_end = ~(addr_off)0;
    if( acc->info_type == X86MD_ADDR_CHARACTERISTICS ) {
        data = GetOutPtr( sizeof( *ret ) );
  #ifdef WOW
        data->x86_addr_flags = ( IsBigSel( acc->addr.segment ) ) ? X86AC_BIG : (( IsDOS ) ? X86AC_REAL : 0);
  #else
        data->x86_addr_flags = ( IsBigSel( acc->addr.segment ) ) ? X86AC_BIG : 0;
  #endif
        return( sizeof( *ret ) + sizeof( data->x86_addr_flags ) );
    }
#elif MADARCH & MADARCH_X64
    ret->cache_start = 0;
    ret->cache_end = ~(addr_off)0;
    if( acc->info_type == X64MD_ADDR_CHARACTERISTICS ) {
        data = GetOutPtr( sizeof( *ret ) );
        data->x64_addr_flags = ( IsBigSel( acc->addr.segment ) ) ? X64AC_BIG : 0;
        return( sizeof( *ret ) + sizeof( data->x64_addr_flags ) );
    }
#elif MADARCH & MADARCH_AXP
    if( acc->info_type == AXPMD_PDATA ) {
        data = GetOutPtr( sizeof( *ret ) );
        memset( &data->axp_pdata, 0, sizeof( data->axp_pdata ) );
        if( FindPData( acc->addr.offset, &data->axp_pdata ) ) {
            ret->cache_start = data->axp_pdata.beg_addr.u._32[0];
            ret->cache_end = data->axp_pdata.end_addr.u._32[0];
        } else {
            ret->cache_start = 0;
            ret->cache_end = 0;
        }
        return( sizeof( *ret ) + sizeof( data->axp_pdata ) );
    }
    ret->cache_start = 0;
    ret->cache_end = ~(addr_off)0;
#elif MADARCH & MADARCH_PPC
    ret->cache_start = 0;
    ret->cache_end = ~(addr_off)0;
#else
    #error TRAP_CORE( Machine_data ) not configured
#endif
    return( sizeof( *ret ) );
}

trap_retval TRAP_CORE( Get_sys_config )( void )
{
    get_sys_config_ret  *ret;
    SYSTEM_INFO         info;

    ret = GetOutPtr( 0 );
    ret->os = DIG_OS_NT;
#if defined( __WATCOMC__ )
    ret->osmajor = _osmajor;
    ret->osminor = _osminor;
#else
#endif
    ret->huge_shift = 3;

    GetSystemInfo( &info );
#if MADARCH & MADARCH_X86
    ret->cpu = X86CPUType();
    ret->fpu = ret->cpu & X86_CPU_MASK;
  #ifdef WOW
    if( IsWOW ) {
        ret->os = DIG_OS_WINDOWS;
    }
  #endif
    ret->arch = DIG_ARCH_X86;
#elif MADARCH & MADARCH_X64
    ret->cpu = X86_P4 | X86_MMX | X86_XMM;
    ret->fpu = ret->cpu & X86_CPU_MASK;
//    ret->cpu = X64_CPU1;
//    ret->fpu = X64_FPU1;
    if( !IsWOW64 ) {
//        ret->os = DIG_OS_NT64;
    }
//    ret->arch = DIG_ARCH_X64;
    ret->arch = DIG_ARCH_X86;
#elif MADARCH & MADARCH_AXP
    switch( info.dwProcessorType ) {
    case PROCESSOR_ALPHA_21064:
        ret->cpu = AXP_21064;
        break;
    case 21164: /* guessing that this is the constant */
        ret->cpu = AXP_21164;
        break;
    default:
        ret->cpu = AXP_DUNNO;
        break;
    }
    ret->fpu = 0;
    ret->arch = DIG_ARCH_AXP;
#elif MADARCH & MADARCH_PPC
    switch( info.dwProcessorType ) {
    case PROCESSOR_PPC_601:
        ret->cpu = PPC_601;
        break;
    case PROCESSOR_PPC_603:
        ret->cpu = PPC_603;
        break;
    case PROCESSOR_PPC_604:
        ret->cpu = PPC_604;
        break;
    case PROCESSOR_PPC_620:
        ret->cpu = PPC_620;
        break;
    default:
        ret->cpu = AXP_DUNNO;
        break;
    }
    ret->fpu = 0;
    ret->arch = DIG_ARCH_PPC;
#else
    #error TRAP_CORE( Get_sys_config ) not configured
#endif
    return( sizeof( *ret ) );
}

trap_retval TRAP_CORE( Get_message_text )( void )
{
    get_message_text_ret    *ret;
    char                    *err_txt;
    msg_list                *next;

    ret = GetOutPtr( 0 );
    ret->flags = MSG_NEWLINE | MSG_ERROR;
    err_txt = GetOutPtr( sizeof( *ret ) );
    if( DebugString != NULL ) {
        strcpy( err_txt, DebugString->msg );
        next = DebugString->next;
        LocalFree( DebugString );
        DebugString = next;
        if( next != NULL ) {
            ret->flags |= MSG_MORE;
        }
    } else {
        switch( LastExceptionCode ) {
        case -1:
            *err_txt = '\0';
            break;
        case STATUS_INVALID_HANDLE: /* new for NT 4.0 */
            strcpy( err_txt, TRP_NT_invalid_handle );
            break;
        case STATUS_WAIT_0:
            strcpy( err_txt, TRP_NT_wait_0 );
            break;
        case STATUS_ABANDONED_WAIT_0:
            strcpy( err_txt, TRP_NT_abandoned_Wait_0 );
            break;
        case STATUS_TIMEOUT:
            strcpy( err_txt, TRP_NT_timeout );
            break;
        case STATUS_PENDING:
            strcpy( err_txt, TRP_NT_pending );
            break;
        case STATUS_DATATYPE_MISALIGNMENT:
            strcpy( err_txt, TRP_EXC_data_type_misalignment );
            break;
        case STATUS_ACCESS_VIOLATION:
            strcpy( err_txt, TRP_EXC_access_violation );
            break;
        case STATUS_NONCONTINUABLE_EXCEPTION:
            strcpy( err_txt, TRP_NT_noncontinuable_exception );
            break;
        case STATUS_CONTROL_C_EXIT:
            strcpy( err_txt, TRP_NT_control_C_exit );
            break;
        case STATUS_USER_APC:
            strcpy( err_txt, TRP_NT_status_user_apc );
            break;
        case STATUS_GUARD_PAGE_VIOLATION:
            strcpy( err_txt, TRP_NT_status_guard_page_violation );
            break;
        case STATUS_IN_PAGE_ERROR:
            strcpy( err_txt, TRP_NT_status_in_page_error );
            break;
        case STATUS_NO_MEMORY:
            strcpy( err_txt, TRP_NT_status_no_memory );
            break;
        case STATUS_ILLEGAL_INSTRUCTION:
            strcpy( err_txt, TRP_EXC_illegal_instruction );
            break;
        case STATUS_INVALID_DISPOSITION:
            strcpy( err_txt, TRP_NT_status_invalid_disposition );
            break;
        case STATUS_ARRAY_BOUNDS_EXCEEDED:
            strcpy( err_txt, TRP_EXC_bounds_check );
            break;
        case STATUS_FLOAT_DENORMAL_OPERAND:
            strcpy( err_txt, TRP_EXC_floating_point_denormal_operand );
            break;
        case STATUS_FLOAT_DIVIDE_BY_ZERO:
            strcpy( err_txt, TRP_EXC_floating_point_divide_by_zero );
            break;
        case STATUS_FLOAT_INEXACT_RESULT:
            strcpy( err_txt, TRP_EXC_floating_point_inexact_result );
            break;
        case STATUS_FLOAT_INVALID_OPERATION:
            strcpy( err_txt, TRP_EXC_floating_point_invalid_operation );
            break;
        case STATUS_FLOAT_OVERFLOW:
            strcpy( err_txt, TRP_EXC_floating_point_overflow );
            break;
        case STATUS_FLOAT_STACK_CHECK:
            strcpy( err_txt, TRP_EXC_floating_point_stack_check );
            break;
        case STATUS_FLOAT_UNDERFLOW:
            strcpy( err_txt, TRP_EXC_floating_point_underflow );
            break;
        case STATUS_INTEGER_DIVIDE_BY_ZERO:
            strcpy( err_txt, TRP_EXC_integer_divide_by_zero );
            break;
        case STATUS_INTEGER_OVERFLOW:
            strcpy( err_txt, TRP_EXC_integer_overflow );
            break;
        case STATUS_PRIVILEGED_INSTRUCTION:
            strcpy( err_txt, TRP_EXC_privileged_instruction );
            break;
        case STATUS_STACK_OVERFLOW:
            strcpy( err_txt, TRP_NT_status_stack_overflow );
            break;
        default:
            strcpy( err_txt, TRP_EXC_unknown );
            wsprintf( err_txt, "%s (0x%8.8lx)", TRP_EXC_unknown,
                LastExceptionCode );
        }
        LastExceptionCode = -1;
    }
    return( sizeof( *ret ) + strlen( err_txt ) + 1 );
}

trap_retval TRAP_CORE( Get_next_alias )( void )
{
    get_next_alias_ret  *ret;

    ret = GetOutPtr( 0 );
    ret->seg = 0;
    ret->alias = 0;
    return( sizeof( *ret ) );
}

static DWORD DoFmtMsg( char **p, DWORD err, ... )
{
    va_list args;
    DWORD   len;
    LPSTR   q;
    DWORD   options;

    va_start( args, err );
    options = FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM;
    len = FormatMessage( options, NULL, err,
        MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ), (char *)p, 0, &args );
    while( (q = strchr( *p, '\r' )) != NULL ) {
        *q = ' ';
    }
    while( (q = strchr( *p, '\n' )) != NULL ) {
        *q = ' ';
    }
    va_end( args );
    return( len );
}

void AddMessagePrefix( const char *buff, size_t len )
/***************************************************/
{
    if( len == 0 ) {
        len = strlen( buff ) + 1;
    }
    if( MsgPrefix != NULL ) {
        LocalFree( MsgPrefix );
    }
    MsgPrefix = LocalAlloc( LMEM_FIXED, len );
    strcpy( MsgPrefix, buff );
}

trap_retval TRAP_CORE( Get_err_text )( void )
{
    get_err_text_req    *acc;
    char                *err_txt;
    char                *lpMessageBuffer;
    DWORD               len;
    char                buff[20];

    acc = GetInPtr( 0 );
    err_txt = GetOutPtr( 0 );

    if( acc->err >= 0x30000000L || acc->err < 0x20000000L ) {
        len = DoFmtMsg( &lpMessageBuffer, acc->err, "%1", "%2", "%3", "%4" );
        if( len > 0 ) {
            if( MsgPrefix != NULL ) {
                strcpy( err_txt, MsgPrefix );
                strcat( err_txt, ": " );
                strcat( err_txt, lpMessageBuffer );
                LocalFree( MsgPrefix );
                MsgPrefix = NULL;
            } else {
                strcpy( err_txt, lpMessageBuffer );
            }
            LocalFree( lpMessageBuffer );
        }
    } else {
        strcpy( err_txt, "Unknown error code: " );
        itoa( acc->err, buff, 16 );
        strcat( err_txt, buff );
    }
    return( strlen( err_txt ) + 1 );
}

trap_retval TRAP_CORE( Split_cmd )( void )
{
    const char      *cmd;
    const char      *start;
    split_cmd_ret   *ret;
    size_t          len;

    cmd = GetInPtr( sizeof( split_cmd_req ) );
    len = GetTotalSizeIn() - sizeof( split_cmd_req );
    start = cmd;
    ret = GetOutPtr( 0 );
    ret->parm_start = 0;
    while( len > 0 ) {
        switch( *cmd ) {
        case '\"':
            cmd++;
            while( --len > 0 && ( *cmd++ != '\"' ) )
                {}
            if( len == 0 )
                continue;
            switch( *cmd ) {
            CASE_SEPS
                ret->parm_start = 1;
            }
            len = 0;
            continue;
        CASE_SEPS
            ret->parm_start = 1;
            /* fall through */
        case '/':
        case '=':
        case '(':
        case ';':
        case ',':
            len = 0;
            continue;
        }
        ++cmd;
        --len;
    }
    ret->parm_start += cmd - start;
    ret->cmd_end = cmd - start;
    return( sizeof( *ret ) );
}

trap_retval TRAP_CORE( Read_io )( void )
{
    return( 0 );
}

trap_retval TRAP_CORE( Write_io )( void )
{
    write_io_ret    *ret;

    ret = GetOutPtr( 0 );
    ret->len = 0;
    return( sizeof( *ret ) );
}

trap_retval TRAP_CORE( Set_user_screen )( void )
{
    return( 0 );
}

trap_retval TRAP_CORE( Set_debug_screen )( void )
{
    ProcessQueuedRepaints();
    return( 0 );
}

void say( const char *fmt, ... )
{
    va_list args;
    char    buff[512];

    va_start( args, fmt );
    vsprintf( buff, fmt, args );
    va_end( args );
    MessageBox( NULL, buff, TRP_The_WATCOM_Debugger, MB_SYSTEMMODAL + MB_OK );
}
