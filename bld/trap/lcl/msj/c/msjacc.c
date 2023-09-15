/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2023 The Open Watcom Contributors. All Rights Reserved.
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


#include <windows.h>
#include <malloc.h>
#include <string.h>
#include "trpimp.h"
#include "madregs.h"
#include "msjerr.h"
#include "control.h"
#include "dipshim.h"
#include "msjutil.h"
#include "readwr.h"

static bool     TaskLoaded;
HANDLE          FakeHandle;

HWND            DebuggerWindow;

trap_version TRAPENTRY TrapInit( const char *parms, char *err, bool remote )
/**************************************************************************/
{
    trap_version        ver;

    /* unused parameters */ (void)remote; (void)parms;

    MSJMemInit();
    ver.major = TRAP_VERSION_MAJOR;
    ver.minor = TRAP_VERSION_MINOR;
    ver.remote = FALSE;
    TaskLoaded = FALSE;
    if( InitProc() ) {
        err[0] = '\0'; /* all ok */
    } else {
        strcpy( err, "unable to connect to debug manager" );
    }
    FakeHandle = (HANDLE)&TrapInit;
    return( ver );
}

void TRAPENTRY TrapFini( void )
/*****************************/
{
    FiniProc();
    MSJMemFini();
}

trap_retval TRAP_CORE( Get_sys_config )( void )
/*********************************************/
{
    get_sys_config_ret *ret;

    ret = GetOutPtr( 0 );
    ret->cpu = 0;
    ret->fpu = 0;
    ret->arch = DIG_ARCH_MSJ;
    ret->huge_shift = 3;
    if( !TaskLoaded ) {
        ret->os = DIG_OS_IDUNNO;
        ret->osmajor = 0;
        ret->osminor = 0;
    } else {
        ret->os = DIG_OS_NT;
        ret->osmajor = 1;
        ret->osminor = 0;
    }
    return( sizeof( *ret ) );
}

trap_retval TRAP_CORE( Map_addr )( void )
/***************************************/
{
    map_addr_req *      acc;
    map_addr_ret *      ret;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    ret->out_addr = acc->in_addr;
    ret->lo_bound = 0;
    ret->hi_bound = ~(addr48_off)0;
    return( sizeof( *ret ) );
}

trap_retval TRAP_CORE( Checksum_mem )( void )
/*******************************************/
{
    unsigned char       *buffer;
    checksum_mem_req    *acc;
    checksum_mem_ret    *ret;
    size_t              got;
    unsigned long       sum;

    sum = 0;
    if( TaskLoaded ) {
        acc = GetInPtr( 0 );
        buffer = (unsigned char *)alloca( acc->len );
        if( buffer != NULL ) {
            got = ReadMemory( &acc->in_addr, buffer, acc->len );
            while( got-- > 0 ) {
                sum += *buffer++;
            }
            ret->result = sum;
        }
    }
    ret = GetOutPtr( 0 );
    ret->result = sum;
    return sizeof( *ret );
}

unsigned DoRead( int addr, char *buff, unsigned length )
/******************************************************/
{
    DWORD               bytes;

    ReadProcessMemory( GetCurrentProcess(), (LPVOID)addr, buff, length, (LPDWORD)&bytes );
    return( bytes );
}

trap_retval TRAP_CORE( Read_mem )( void )
/***************************************/
{
    read_mem_req *acc;

    if( !TaskLoaded )
        return( 0 );
    acc = GetInPtr( 0 );
    switch( acc->mem_addr.segment ) {
    case JVM_DIP_GETCUE_SELECTOR:
        return( DipCue( acc->mem_addr.offset, GetOutPtr( 0 ) ) );
    case JVM_DIP_GETFILE_SELECTOR:
        return( DipFileName( acc->mem_addr.offset, GetOutPtr( 0 ) ) );
    case JVM_DIP_GETMODNAME_SELECTOR:
        return( DipModName( acc->mem_addr.offset, GetOutPtr( 0 ) ) );
    case JVM_DIP_GETMODBASE_SELECTOR:
        return( DipModBase( acc->mem_addr.offset, GetOutPtr( 0 ) ) );
    case JVM_DIP_GETMODEND_SELECTOR:
        return( DipModEnd( acc->mem_addr.offset, GetOutPtr( 0 ) ) );
    case JVM_MAD_UPSTACK_SELECTOR:
        return( MadUpStack( &acc->mem_addr, GetOutPtr( 0 ) ) );
    case JVM_DIP_READMEM_SELECTOR:
        return( DoRead( acc->mem_addr.offset, GetOutPtr( 0 ), acc->len ) );
    default:
        return( ReadMemory( &acc->mem_addr, GetOutPtr( 0 ), acc->len ) );
    }
}

unsigned DoWrite( int addr, char *buff, unsigned length )
/*******************************************************/
{
    DWORD               bytes;

    WriteProcessMemory( GetCurrentProcess(), (LPVOID)addr, buff,
                        length, (LPDWORD)&bytes );
    return( bytes );
}

trap_retval TRAP_CORE( Write_mem )( void )
/****************************************/
{
    write_mem_ret   *ret;
    write_mem_req   *acc;
    size_t          len;
    void            *data;

    ret = GetOutPtr( 0 );
    ret->len = 0;
    if( TaskLoaded ) {
        acc = GetInPtr( 0 );
        len = GetTotalSizeIn() - sizeof( *acc );
        data = GetInPtr( sizeof( *acc ) );
        switch( acc->mem_addr.segment ) {
        case JVM_DIP_READMEM_SELECTOR:
            ret->len = DoRead( acc->mem_addr.offset, GetOutPtr( 0 ), len );
            break;
        default:
            ret->len = WriteMemory( &acc->mem_addr, data, len );
            break;
        }
    }
    return( sizeof( *ret ) );
}

trap_retval TRAP_CORE( Read_io )( void )
/**************************************/
// never called
{
    return( 0 );
}

trap_retval TRAP_CORE( Write_io )( void )
/***************************************/
{
    write_io_ret *ret;

    ret = GetOutPtr( 0 );
    ret->len = 0;
    return( sizeof( *ret ) );
}

static unsigned runProg( bool single_step )
/*****************************************/
{
    prog_go_ret *       ret;

    ret = GetOutPtr( 0 );
    if( !TaskLoaded ) {
        ret->conditions = COND_TERMINATE;
        return( sizeof( *ret ) );
    }
    if( single_step ) {
        TraceProc( &ret->program_counter );
    } else {
        ResumeProc( &ret->program_counter );
    }
    ret->conditions = ReadFlags() | COND_LIBRARIES;
    ret->stack_pointer.segment = 0;
    ret->stack_pointer.offset = 0;
    return( sizeof( *ret ) );
}

trap_retval TRAP_CORE( Prog_go )( void )
/**************************************/
{
    return( runProg( FALSE ) );
}

trap_retval TRAP_CORE( Prog_step )( void )
/****************************************/
{
    return( runProg( TRUE ) );
}

static char * TrimName( char * name )
/***********************************/
{
    char    *endptr;

    while( isspace( *name ) ) {
        name++;
    }
    endptr = name + strlen( name ) - 1;
    while( isspace( *endptr ) ) {
        *endptr = '\0';
        endptr--;
    }
    return( name );
}

static size_t MergeArgvArray( const char *src, char *dst, size_t len )
{
    char    ch;
    char    *start = dst;

    while( len-- > 0 ) {
        ch = *src++;
        if( ch == '\0' ) {
            if( len == 0 )
                break;
            ch = ' ';
        }
        *dst++ = ch;
    }
    *dst = '\0';
    return( dst - start );
}

trap_retval TRAP_CORE( Prog_load )( void )
/****************************************/
{
    prog_load_ret       *ret;
    prog_load_req       *acc;
    char                *parm;
    char                *clname;
    size_t              len;
    char                buff[_MAX_PATH * 2];
    char                *dst;
    char                *src;
    char                *name;

    ret = GetOutPtr( 0 );
    acc = GetInPtr( 0 );
    parm = GetInPtr( sizeof( prog_load_req ) );
    ret->err = ERR_MSJ_CANT_LOAD;
    if( parm == NULL )
        return sizeof( *ret );
    dst = StrCopyDst( parm, buff ) + 1;
    src = parm;
    while( *src++ != '\0' )
        {}
    MergeArgvArray( src, dst, GetTotalSizeIn() - sizeof( *acc ) - ( src - parm ) );
    name = buff;
    len = strlen( name );
    name = strtok( name, "@" );
    clname = strtok( NULL, "@" );
    if( clname == NULL )
        return( sizeof( *ret ) );
    clname = TrimName( clname );
    if( *clname == '\0' )
        return( sizeof( *ret ) );
    TaskLoaded = StartProc( name, clname );
    InitMappings();
    if( TaskLoaded ) {
        if( DebuggerWindow != NULL )
            SetForegroundWindow( DebuggerWindow );
        ret->err = 0;
        ret->task_id = 0;
        ret->flags = LD_FLAG_IS_BIG | LD_FLAG_IS_PROT | LD_FLAG_HAVE_RUNTIME_DLLS;
    }
    return( sizeof( *ret ) );
}

trap_retval TRAP_CORE( Prog_kill )( void )
/****************************************/
{
    prog_kill_ret *ret;

    FiniMappings();
    EndProc();
    ret = GetOutPtr( 0 );
    ret->err = 0;
    return( sizeof( *ret ) );
}

static void SetBreak( void )
/**************************/
{
    set_break_req *req;

    req = GetInPtr( 0 );
    SetBreakpoint( &req->break_addr );
}

static void ClearBreak( void )
/****************************/
{
    clear_break_req *req;

    req = GetInPtr( 0 );
    ClearBreakpoint( &req->break_addr );
}

trap_retval TRAP_CORE( Set_watch )( void )
/****************************************/
{
    set_watch_ret *ret;

    SetBreak();
    ret = GetOutPtr( 0 );
    ret->err = 0;   // OK
    ret->multiplier = USING_DEBUG_REG;
    return( sizeof( *ret ) );
}

trap_retval TRAP_CORE( Clear_watch )( void )
/******************************************/
{
    ClearBreak();
    return( 0 );
}

trap_retval TRAP_CORE( Set_break )( void )
/****************************************/
{
    set_break_ret *ret;

    SetBreak();
    ret = GetOutPtr( 0 );
    ret->old = 0;
    return( sizeof( *ret ) );
}

trap_retval TRAP_CORE( Clear_break )( void )
/******************************************/
{
    ClearBreak();
    return( 0 );
}

trap_retval TRAP_CORE( Get_next_alias )( void )
/*********************************************/
{
    get_next_alias_ret *ret;

    ret = GetOutPtr( 0 );
    ret->seg = 0;
    ret->alias = 0;
    return( sizeof( *ret ) );
}

trap_retval TRAP_CORE( Set_user_screen )( void )
/**********************************************/
{
    return( 0 );
}

trap_retval TRAP_CORE( Set_debug_screen )( void )
/***********************************************/
{
    ProcessQueuedRepaints();
    return( 0 );
}

trap_retval TRAP_CORE( Get_lib_name )( void )
/*******************************************/
{
    get_lib_name_req    *acc;
    get_lib_name_ret    *ret;
    char                *name;
    size_t              max_len;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    name = GetOutPtr( sizeof( *ret ) );
    max_len = GetTotalSizeOut() - sizeof( *ret ) - 1;
    ret->mod_handle = GetLibName( acc->mod_handle, name, max_len );
    if( ret->mod_handle )
        return( sizeof( *ret ) + strlen( name ) + 1 );
    return( sizeof( *ret ) );
}

static DWORD DoFmtMsg( LPTSTR *p, DWORD err, ... )
{
    va_list     args;
    DWORD       len;
    LPSTR       q;

    va_start( args, err );
    len = FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
        NULL, err, MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ),
        (LPSTR) p, 0, &args );
    while( ( q = strchr( *p, '\r' ) ) != NULL )
        *q = ' ';
    while( ( q = strchr( *p, '\n' ) ) != NULL )
        *q = ' ';
    va_end( args );
    return( len );
}

static char *Errors[] = {
#define pick( a,b,c ) c,
#include "msjepick.h"
#undef pick
};

trap_retval TRAP_CORE( Get_err_text )( void )
/*******************************************/
{

    get_err_text_req    *acc;
    char                *err_txt;
    LPTSTR              lpMessageBuffer;
    DWORD               len;

    acc = GetInPtr( 0 );
    err_txt = GetOutPtr( 0 );

#if 0
    if( acc->err == ERR_MSJ_SAVED_ERROR ) {
        strcpy( err_txt, SavedError );
    } else
#endif
    if( IsUserErr( acc->err ) ) {
        strcpy( err_txt, Errors[ErrIndex( acc->err )] );
    } else {
        len = DoFmtMsg( &lpMessageBuffer, acc->err, "%1","%2","%3","%4" );
        if( len > 0 ) {
            strcpy( err_txt, lpMessageBuffer );
            LocalFree( lpMessageBuffer );
        }
    }
    return( strlen( err_txt ) + 1 );
}

trap_retval TRAP_CORE( Get_message_text )( void )
/***********************************************/
{
    return( 0 );
}

trap_retval TRAP_CORE( Redirect_stdin )( void )
/*********************************************/
{
    return( 0 );
}

trap_retval TRAP_CORE( Redirect_stdout )( void )
/**********************************************/
{
    return( 0 );
}

trap_retval TRAP_CORE( Split_cmd )( void )
/****************************************/
{
    const char          *cmd;
    const char          *start;
    split_cmd_ret       *ret;
    size_t              len;

    cmd = GetInPtr( sizeof( split_cmd_req ) );
    ret = GetOutPtr( 0 );
    ret->parm_start = 0;
    start = cmd;
    len = GetTotalSizeIn() - sizeof( split_cmd_req );
    while( len > 0 ) {
        switch( *cmd ) {
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

trap_retval TRAP_CORE( Read_regs )( void )
/****************************************/
// NYI - we can't fill out this structure
{
    mad_registers _WCUNALIGNED *mr;
    addr48_ptr pc;

    mr = GetOutPtr( 0 );
    memset( mr, 0, sizeof( mr->jvm ) );
    GetPC( &pc );
    mr->jvm.pc.offset = pc.offset;
    mr->jvm.pc.segment = pc.segment;
    return( sizeof( mr->jvm ) );
}

trap_retval TRAP_CORE( Write_regs )( void )
/*****************************************/
// NYI: cannot write registers
{
    return( 0 );
}

trap_retval TRAP_CORE( Machine_data )( void )
/*******************************************/
// NYI: what the hell does this do?
{
//    machine_data_req    *acc;
    machine_data_ret    *ret;
//    machine_data_spec   *data;

//    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
//    data = GetOutPtr( sizeof( *ret ) );
    return( sizeof( *ret ) );
}

trap_retval TRAP_THREAD( get_next )( void )
/*****************************************/
{
    thread_get_next_req *acc;
    thread_get_next_ret *ret;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    ret->thread = GetNextThread( acc->thread, &ret->state );
    return( sizeof( *ret ) );
}

trap_retval TRAP_THREAD( set )( void )
/************************************/
{
    thread_set_req *acc;
    thread_set_ret *ret;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    ret->err = 0;
    ret->old_thread = SetThread( acc->thread );
    return( sizeof( *ret ) );
}

trap_retval TRAP_THREAD( freeze )( void )
/***************************************/
{
    thread_freeze_req   *acc;
    thread_freeze_ret   *ret;

    // nyi
    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );

    // freeze the thread associated with acc->thread

    ret->err = ERR_MSJ_THREADS_NOT_SUPPORTED;
    return( sizeof( *ret ) );
}

trap_retval TRAP_THREAD( thaw )( void )
/*************************************/
{
    thread_thaw_req *acc;
    thread_thaw_ret *ret;

    // nyi
    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    // thaw the thread associated with acc->thread
    ret->err = ERR_MSJ_THREADS_NOT_SUPPORTED;
    return( sizeof( *ret ) );
}

trap_retval TRAP_THREAD( get_extra )( void )
/******************************************/
{
    thread_get_extra_req *acc;
    char *              name;
    char *              origname;

    acc = GetInPtr( 0 );
    name = GetOutPtr( 0 );
    strcpy( name, "" );
    if( acc->thread != 0 ) {
        origname = GetThreadName( acc->thread );
        strcpy( name, origname );
    }
    return( strlen( name ) + 1 );
}

