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
#include "stdnt.h"
#include "trperr.h"
#include "madregs.h"
#if defined( MD_x86 )
#include "x86cpu.h"
#endif

BOOL IsBigSel( WORD sel )
{
#if defined( MD_axp ) | defined( MD_ppc )
    return( TRUE );
#elif defined( MD_x86 ) || defined( MD_x64 )
    thread_info *ti;
    LDT_ENTRY   ldt;

    if( sel == FlatCS || sel == FlatDS ) {
        return( TRUE );
    }
    ti = FindThread( DebugeeTid );
    if( ti == NULL ) {
        return( TRUE );
    }
    GetThreadSelectorEntry( ti->thread_handle, sel, &ldt );
    return( ldt.HighWord.Bits.Default_Big );
#else
   #error IsBigSel not configured
#endif
}

#if defined( MD_axp )
typedef struct {
    unsigned_32 beg_addr;
    unsigned_32 end_addr;
    unsigned_32 except_handler;
    unsigned_32 handler_data;
    unsigned_32 pro_end_addr;
} nt_pdata;

bool FindPData( addr_off off, axp_pdata *pdata )
{
    nt_pdata    pd;
    LPVOID      tbl;
    addr_off    size;
    DWORD       bytes;

    if( !FindExceptInfo( off, &tbl, &size ) ) {
        return( FALSE );
    }
    for( ;; ) {
        if( size == 0 ) {
            return( FALSE );
        }
        ReadProcessMemory( ProcessInfo.process_handle, tbl, ( LPVOID )&pd,
                    sizeof( pd ), &bytes );
        if( bytes != sizeof( pd ) ) {
            return( FALSE );
        }
        if( off >= pd.beg_addr && off < pd.end_addr ) {
            /*
             *  This is an optimization - if the prologue end addr is not
             *  in the exception start/end range, this is not the entry
             *  for the start of the procedure and the MAD isn't interested.
             *  Keep looking for real one.
             */
            if( pd.pro_end_addr >= pd.beg_addr &&
                    pd.pro_end_addr < pd.end_addr ) {
                break;
            }
        }
        tbl = ( LPVOID ) ( ( DWORD ) tbl + sizeof( pd ) );
        size -= sizeof( pd );
    }
    pdata->beg_addr.u._32[0] = pd.beg_addr;
    pdata->end_addr.u._32[0] = pd.end_addr;
    pdata->except_handler.u._32[0] = pd.except_handler;
    pdata->handler_data.u._32[0] = pd.handler_data;
    pdata->pro_end_addr.u._32[0] = pd.pro_end_addr;
    return( TRUE );
}
#endif

trap_retval ReqMachine_data( void )
{
    machine_data_req    *acc;
    machine_data_ret    *ret;
    union {
        unsigned_8      u8;
#if defined( MD_axp )
        axp_pdata       pd;
#endif
    }                   *data;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    data = GetOutPtr( sizeof( *ret ) );
#if defined( MD_x86 )
    ret->cache_start = 0;
    ret->cache_end = ~( addr_off ) 0;
    data->u8 = 0;
    if( IsBigSel( acc->addr.segment ) ) {
        data->u8 |= X86AC_BIG;
    }
    return( sizeof( *ret ) + sizeof( data->u8 ) );
#elif defined( MD_x64 )
    ret->cache_start = 0;
    ret->cache_end = ~( addr_off ) 0;
    data->u8 = 0;
    if( IsBigSel( acc->addr.segment ) ) {
        data->u8 |= X86AC_BIG;
//        data->u8 |= X64AC_BIG;
    }
    return( sizeof( *ret ) + sizeof( data->u8 ) );
#elif defined( MD_axp )
    memset( &data->pd, 0, sizeof( data->pd ) );
    if( FindPData( acc->addr.offset, &data->pd ) ) {
        ret->cache_start = data->pd.beg_addr.u._32[0];
        ret->cache_end = data->pd.end_addr.u._32[0];
    } else {
        ret->cache_start = 0;
        ret->cache_end = 0;
    }
    return( sizeof( *ret ) + sizeof( data->pd ) );
#elif defined( MD_ppc )
    return( sizeof( *ret ) );
#else
    #error ReqMachine_data not configured
#endif
}

trap_retval ReqGet_sys_config( void )
{
    get_sys_config_ret  *ret;
    SYSTEM_INFO         info;

    ret = GetOutPtr( 0 );
    ret->sys.os = MAD_OS_NT;
#if defined( __WATCOMC__ )
    ret->sys.osmajor = _osmajor;
    ret->sys.osminor = _osminor;
#else
#endif
    ret->sys.huge_shift = 3;

    GetSystemInfo( &info );
#if defined( MD_x86 )
    ret->sys.cpu = X86CPUType();
    ret->sys.fpu = ret->sys.cpu & X86_CPU_MASK;
    if( IsWOW ) {
        ret->sys.os = MAD_OS_WINDOWS;
    }
    ret->sys.mad = MAD_X86;
#elif defined( MD_x64 )
    ret->sys.cpu = X86_P4 | X86_MMX | X86_XMM;
    ret->sys.fpu = ret->sys.cpu & X86_CPU_MASK;
//    ret->sys.cpu = X64_CPU1;
//    ret->sys.fpu = X64_FPU1;
    if( !IsWOW ) {
//        ret->sys.os = MAD_OS_NT64;
    }
//    ret->sys.mad = MAD_X64;
    ret->sys.mad = MAD_X86;
#elif defined( MD_axp )
    switch( info.dwProcessorType ) {
    case PROCESSOR_ALPHA_21064:
        ret->sys.cpu = AXP_21064;
        break;
    case 21164: /* guessing that this is the constant */
        ret->sys.cpu = AXP_21164;
        break;
    default:
        ret->sys.cpu = AXP_DUNNO;
        break;
    }
    ret->sys.fpu = 0;
    ret->sys.mad = MAD_AXP;
#elif defined( MD_ppc )
    switch( info.dwProcessorType ) {
    case PROCESSOR_PPC_601:
        ret->sys.cpu = PPC_601;
        break;
    case PROCESSOR_PPC_603:
        ret->sys.cpu = PPC_603;
        break;
    case PROCESSOR_PPC_604:
        ret->sys.cpu = PPC_604;
        break;
    case PROCESSOR_PPC_620:
        ret->sys.cpu = PPC_620;
        break;
    default:
        ret->sys.cpu = AXP_DUNNO;
        break;
    }
    ret->sys.fpu = 0;
    ret->sys.mad = MAD_PPC;
#else
    #error ReqGet_sys_config not configured
#endif
    return( sizeof( *ret ) );
}

trap_retval ReqGet_message_text( void )
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
        case 0xC0000008L: // STATUS_INVALID_HANDLE - new for NT 4.0
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

trap_retval ReqGet_next_alias( void )
{
    get_next_alias_ret  *ret;

    ret = GetOutPtr( 0 );
    ret->seg = 0;
    ret->alias = 0;
    return( sizeof( *ret ) );
}

static DWORD DoFmtMsg( LPTSTR *p, DWORD err, ... )
{
    va_list args;
    DWORD   len;
    LPSTR   q;
    DWORD   options;

    va_start( args, err );
    options = FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM;
    len = FormatMessage( options, NULL, err,
        MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ), ( LPSTR )p, 0, &args );
    while( ( q = strchr( *p, '\r' ) ) != NULL ) {
        *q = ' ';
    }
    while( ( q = strchr( *p, '\n' ) ) != NULL ) {
        *q = ' ';
    }
    va_end( args );
    return( len );
}

void AddMessagePrefix( char *buff, int len )
/******************************************/
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

trap_retval ReqGet_err_text( void )
{
    get_err_text_req    *acc;
    char                *err_txt;
    LPTSTR              lpMessageBuffer;
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

static int tryPath( char *name, char *end, const char *ext_list )
{
    char    *p;
    BOOL    done;
    HANDLE  h;

    done = FALSE;
    do {
        if( *ext_list == 0 ) {
            done = 1;
        }
        for( p = end; (*p = *ext_list) != 0; ++p,++ext_list ) {
        }

        h = CreateFile( name, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
        if( h != INVALID_HANDLE_VALUE ) {
            CloseHandle( h );
            return( 0 );
        }
        ++ext_list;
    } while( !done );
    return( -1 );
}

int FindFilePath( char *pgm, char *buffer, const char *ext_list )
{
    char    *p;
    char    *p2;
    char    *p3;
    BOOL    have_ext;
    BOOL    have_path;
    char    *envbuf;
    DWORD   envlen;
    int     rc;

    have_ext = FALSE;
    have_path = FALSE;
    for( p = pgm, p2 = buffer; (*p2 = *p) != 0; ++p, ++p2 ) {
        switch( *p ) {
        case '\\':
        case '/':
        case ':':
            have_path = TRUE;
            have_ext = FALSE;
            break;
        case '.':
            have_ext = TRUE;
            break;
        }
    }
    if( have_ext ) {
        ext_list = "";
    }
    if( !tryPath( buffer, p2, ext_list ) ) {
        return( 0 );
    }
    if( have_path ) {
        return( TRUE );
    }
    envlen = GetEnvironmentVariable( "PATH", NULL, 0 );
    if( envlen == 0 )
        return( -1 );
    envbuf = LocalAlloc( LMEM_FIXED, envlen );
    GetEnvironmentVariable( "PATH", envbuf, envlen );
    rc = -1;
    for( p = envbuf; *p != '\0'; ++p ) {
        p2 = buffer;
        while( *p != '\0' ) {
            if( *p == ';' ) {
                break;
            }
            *p2++ = *p++;
        }
        if( p2[-1] != '\\' && p2[-1] != '/' ) {
            *p2++ = '\\';
        }
        for( p3 = pgm; (*p2 = *p3) != '\0'; ++p2, ++p3 ) {
        }
        if( !tryPath( buffer, p2, ext_list ) ) {
            rc = 0;
            break;
        }
        if( *p == '\0' ) {
            break;
        }
    }
    LocalFree( envbuf );
    return( rc );
}

trap_retval ReqSplit_cmd( void )
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
    while( len != 0 ) {
        switch( *cmd ) {
        case '\"':
            while( --len && (*++cmd != '\"') )
                ;
            if( len != 0 )
                break;
            /* fall down */
        case '\0':
        case ' ':
        case '\t':
            ret->parm_start = 1;
            /* fall down */
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

trap_retval ReqRead_io( void )
{
    return( 0 );
}

trap_retval ReqWrite_io( void )
{
    write_io_ret    *ret;

    ret = GetOutPtr( 0 );
    ret->len = 0;
    return( sizeof( *ret ) );
}

trap_retval ReqSet_user_screen( void )
{
    return( 0 );
}

trap_retval ReqSet_debug_screen( void )
{
    ProcessQueuedRepaints();
    return( 0 );
}

void say( char *fmt, ... )
{
    va_list args;
    char    buff[512];

    va_start( args, fmt );
    vsprintf( buff, fmt, args );
    va_end( args );
    MessageBox( NULL, buff, TRP_The_WATCOM_Debugger, MB_SYSTEMMODAL + MB_OK );
}
