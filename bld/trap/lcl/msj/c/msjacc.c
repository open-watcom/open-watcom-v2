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


#include <windows.h>
#include <malloc.h>
#include <string.h>
#include "trpimp.h"
#define MD_jvm
#include "madregs.h"
#include "msjerr.h"
#include "control.h"
#include "dipshim.h"
#include "msjutil.h"
#include "readwr.h"

static bool     TaskLoaded;
HANDLE          FakeHandle;

HWND            DebuggerWindow;

trap_version TRAPENTRY TrapInit( char *parm, char *err, bool remote )
/*******************************************************************/
{
    trap_version        ver;

    MSJMemInit();
    remote = remote; parm = parm;
    ver.major = TRAP_MAJOR_VERSION;
    ver.minor = TRAP_MINOR_VERSION;
    ver.remote = FALSE;
    TaskLoaded = FALSE;
    if( InitProc() ) {
        err[0] = '\0'; /* all ok */
    } else {
        strcpy( err, "unable to connect to debug manager" );
    }
    FakeHandle = (HANDLE)&TrapInit;
    return ver;
}

void TRAPENTRY TrapFini()
/***********************/
{
    FiniProc();
    MSJMemFini();
}

trap_retval ReqGet_sys_config( void )
/********************************/
{
    get_sys_config_ret *ret;

    ret = GetOutPtr(0);
    ret->sys.cpu = 0;
    ret->sys.fpu = 0;
    ret->sys.mad = MAD_MSJ;
    ret->sys.huge_shift = 3;
    if( !TaskLoaded ) {
        ret->sys.os = OS_IDUNNO;
        ret->sys.osmajor = 0;
        ret->sys.osminor = 0;
    } else {
        ret->sys.os = OS_NT;
        ret->sys.osmajor = 1;
        ret->sys.osminor = 0;
    }
    return sizeof( *ret );
}

trap_retval ReqMap_addr( void )
/**************************/
{
    map_addr_req *      acc;
    map_addr_ret *      ret;

    acc = GetInPtr(0);
    ret = GetOutPtr(0);
    ret->out_addr = acc->in_addr;
    ret->lo_bound = 0;
    ret->hi_bound = ~(addr48_off)0;
    return sizeof( *ret );
}

trap_retval ReqAddr_info( void )
/***************************/
{
    addr_info_ret *     ret;

    ret = GetOutPtr( 0 );
    ret->is_big = TRUE;
    return sizeof( *ret );
}

trap_retval ReqChecksum_mem( void )
/******************************/
{
    unsigned_8 *        buffer;
    checksum_mem_req *  acc;
    checksum_mem_ret *  ret;
    unsigned            actual;
    unsigned            sum;

    ret = GetOutPtr( 0 );
    ret->result = 0;
    if( TaskLoaded ) {
        acc = GetInPtr(0);
        buffer = (unsigned_8 *) alloca( acc->len );
        if( buffer != NULL ) {
            sum = 0;
            actual = ReadMemory( &acc->in_addr, buffer, acc->len );
            while( actual > 0 ) {
                sum += *buffer;
                buffer++;
                actual--;
            }
            ret->result = sum;
        }
    }
    return sizeof( *ret );
}

unsigned DoRead( int addr, char *buff, unsigned length )
/******************************************************/
{
    DWORD               bytes;

    ReadProcessMemory( GetCurrentProcess(), (LPVOID)addr, buff,
                        length, (LPDWORD) &bytes );
    return( bytes );
}

trap_retval ReqRead_mem( void )
/**************************/
{
    read_mem_req *acc;

    if( !TaskLoaded ) return 0;
    acc = GetInPtr(0);
    switch( acc->mem_addr.segment ) {
    case JVM_DIP_GETCUE_SELECTOR:
        return( DipCue( acc->mem_addr.offset, GetOutPtr(0) ) );
    case JVM_DIP_GETFILE_SELECTOR:
        return( DipFileName( acc->mem_addr.offset, GetOutPtr(0) ) );
    case JVM_DIP_GETMODNAME_SELECTOR:
        return( DipModName( acc->mem_addr.offset, GetOutPtr(0) ) );
    case JVM_DIP_GETMODBASE_SELECTOR:
        return( DipModBase( acc->mem_addr.offset, GetOutPtr(0) ) );
    case JVM_DIP_GETMODEND_SELECTOR:
        return( DipModEnd( acc->mem_addr.offset, GetOutPtr(0) ) );
    case JVM_MAD_UPSTACK_SELECTOR:
        return( MadUpStack( &acc->mem_addr, GetOutPtr(0) ) );
    case JVM_DIP_READMEM_SELECTOR:
        return( DoRead( acc->mem_addr.offset, GetOutPtr(0), acc->len ) );
    default:
        return( ReadMemory( &acc->mem_addr, GetOutPtr(0), acc->len ) );
    }
}

unsigned DoWrite( int addr, char *buff, unsigned length )
/*******************************************************/
{
    DWORD               bytes;

    WriteProcessMemory( GetCurrentProcess(), (LPVOID)addr, buff,
                        length, (LPDWORD) &bytes );
    return( bytes );
}

trap_retval ReqWrite_mem( void )
/***************************/
{
    write_mem_ret *     ret;
    write_mem_req *     acc;
    unsigned            length;
    void *              data;

    ret = GetOutPtr( 0 );
    ret->len = 0;
    if( TaskLoaded ) {
        acc = GetInPtr(0);
        length = GetTotalSize() - sizeof(*acc);
        data = GetInPtr( sizeof(*acc) );
        switch( acc->mem_addr.segment ) {
        case JVM_DIP_READMEM_SELECTOR:
            ret->len = DoRead( acc->mem_addr.offset, GetOutPtr(0), length );
            break;
        default:
            ret->len = WriteMemory( &acc->mem_addr, data, length );
            break;
        }
    }
    return sizeof( *ret );
}

trap_retval ReqRead_io( void )
/*************************/
// never called
{
    return( 0 );
}

trap_retval ReqWrite_io( void )
/**************************/
{
    write_io_ret *ret;

    ret = GetOutPtr(0);
    ret->len = 0;
    return sizeof( *ret );
}

// OBSOLETE - use ReqRead_regs

trap_retval ReqRead_cpu( void )
/**************************/
{
    read_cpu_ret *ret;

    ret = GetOutPtr(0);
    memset( ret, 0, sizeof( *ret ) );
    return sizeof( *ret );
}

trap_retval ReqRead_fpu( void )
/**************************/
{
    read_fpu_ret * ret;

    ret = GetOutPtr(0);
    memset( ret, 0, sizeof( *ret ) );
    return sizeof( *ret );
}

trap_retval ReqWrite_cpu( void )
/***************************/
{
    return 0;
}

trap_retval ReqWrite_fpu( void )
/***************************/
{
    return 0;
}

static unsigned runProg( bool single_step )
/*****************************************/
{
    prog_go_ret *       ret;

    ret = GetOutPtr( 0 );
    if( !TaskLoaded ) {
        ret = GetOutPtr( 0 );
        ret->conditions = COND_TERMINATE;
        return sizeof( *ret );
    }
    if( single_step ) {
        TraceProc( &ret->program_counter );
    } else {
        ResumeProc( &ret->program_counter );
    }
    ret->conditions = ReadFlags() | COND_LIBRARIES;
    ret->stack_pointer.segment = 0;
    ret->stack_pointer.offset = 0;
    return sizeof( *ret );
}

trap_retval ReqProg_go( void )
/*************************/
{
    return runProg( FALSE );
}

trap_retval ReqProg_step( void )
/***************************/
{
    return runProg( TRUE );
}

static char * TrimName( char * name )
/***********************************/
{
    char * endptr;

    while( isspace( *name ) ) {
        name++;
    }
    endptr = name + strlen(name) - 1;
    while( isspace(*endptr) ) {
        *endptr = '\0';
        endptr--;
    }
    return name;
}

trap_retval ReqProg_load( void )
/***************************/
{
    prog_load_ret *     ret;
    prog_load_req *     acc;
    char *              parm;
    char *              clname;
    unsigned            len;
    char                buff[_MAX_PATH*2];
    char                *dst,*src,ch;
    char                *name;

    ret = GetOutPtr( 0 );
    acc = GetInPtr( 0 );
    parm = GetInPtr( sizeof( prog_load_req ) );
    ret->err = ERR_MSJ_CANT_LOAD;
    if( parm == NULL ) return sizeof( *ret );
    strcpy( buff, parm );
    dst = &buff[strlen(buff)];
    src = parm;
    while( *src != 0 ) {
        ++src;
    }
    len = &parm[ GetTotalSize() - sizeof( *acc ) ] - src;
    for( ;; ) {
        if( len == 0 ) break;
        ch = *src;
        if( ch == 0 ) {
            ch = ' ';
        }
        *dst = ch;
        ++dst;
        ++src;
        --len;
    }
    *dst = 0;
    name = buff;
    len = strlen(name);
    name = strtok( name, "@" );
    clname = strtok( NULL, "@" );
    if( clname == NULL ) return sizeof(*ret);
    clname = TrimName( clname );
    if( *clname == '\0' ) return sizeof(*ret);
    TaskLoaded = StartProc( name, clname );
    InitMappings();
    if( TaskLoaded ) {
        if( DebuggerWindow != NULL ) SetForegroundWindow( DebuggerWindow );
        ret->err = 0;
        ret->task_id = 0;
        ret->flags = LD_FLAG_IS_BIG | LD_FLAG_IS_PROT | LD_FLAG_HAVE_RUNTIME_DLLS;
    }
    return sizeof( *ret );
}

trap_retval ReqProg_kill( void )
/***************************/
{
    prog_kill_ret *ret;

    FiniMappings();
    EndProc();
    ret = GetOutPtr( 0 );
    ret->err = 0;
    return sizeof( *ret );
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

trap_retval ReqSet_watch( void )
/*********************/
{
    set_watch_ret *ret;

    SetBreak();
    ret = GetOutPtr( 0 );
    ret->err = 0;
    ret->multiplier = USING_DEBUG_REG;
    return sizeof( *ret );
}

trap_retval ReqClear_watch( void )
/***********************/
{
    ClearBreak();
    return 0;
}

trap_retval ReqSet_break( void )
/*********************/
{
    set_break_ret *ret;

    SetBreak();
    ret = GetOutPtr( 0 );
    ret->old = 0;
    return sizeof(*ret);
}

trap_retval ReqClear_break( void )
/***********************/
{
    ClearBreak();
    return 0;
}

trap_retval ReqGet_next_alias( void )
/********************************/
{
    get_next_alias_ret *ret;

    ret = GetOutPtr( 0 );
    ret->seg = 0;
    ret->alias = 0;
    return sizeof( *ret );
}

trap_retval ReqSet_user_screen( void )
/*********************************/
{
    return( 0 );
}

trap_retval ReqSet_debug_screen( void )
/**********************************/
{
    ProcessQueuedRepaints();
    return( 0 );
}

trap_retval ReqGet_lib_name( void )
/******************************/
{
    get_lib_name_req    *acc;
    get_lib_name_ret    *ret;
    char                *name;

    acc = GetInPtr(0);
    ret = GetOutPtr(0);
    name = GetOutPtr( sizeof( *ret ) );
    ret->handle = GetLibName( acc->handle, name );
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
    while( ( q = strchr( *p, '\r' ) ) != NULL ) *q = ' ';
    while( ( q = strchr( *p, '\n' ) ) != NULL ) *q = ' ';
    va_end( args );
    return( len );
}

static char *Errors[] = {
#define pick( a,b,c ) c,
#include "msjepick.h"
#undef pick
};

trap_retval ReqGet_err_text( void )
/******************************/
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
        strcpy( err_txt, Errors[ ErrIndex( acc->err ) ] );
    } else {
        len = DoFmtMsg( &lpMessageBuffer, acc->err, "%1","%2","%3","%4" );
        if( len > 0 ) {
            strcpy( err_txt, lpMessageBuffer );
            LocalFree( lpMessageBuffer );
        }
    }
    return( strlen( err_txt ) + 1 );
}

trap_retval ReqGet_message_text( void )
/**********************************/
{
    return 0;
}

trap_retval ReqRedirect_stdin( void )
/********************************/
{
    return 0;
}

trap_retval ReqRedirect_stdout( void )
/*********************************/
{
    return 0;
}

trap_retval ReqSplit_cmd( void )
/***************************/
{
    char                *cmd;
    char                *start;
    split_cmd_ret       *ret;
    unsigned            len;

    cmd = GetInPtr( sizeof( split_cmd_req ) );
    ret = GetOutPtr( 0 );
    start = cmd;
    len = GetTotalSize() - sizeof( split_cmd_req );
    for( ;; ) {
        if( len == 0 ) goto done;
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
            ret->parm_start = cmd - start + 1;
            ret->cmd_end = cmd - start;
            return( sizeof( *ret ) );
        }
        ++cmd;
        --len;
    }
done:
    ret->parm_start = cmd - start;
    ret->cmd_end = cmd - start;
    return( sizeof( *ret ) );
}

trap_retval ReqRead_regs( void )
/***************************/
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

trap_retval ReqWrite_regs( void )
/****************************/
// NYI: cannot write registers
{
    return( 0 );
}

trap_retval ReqMachine_data( void )
/******************************/
// NYI: what the hell does this do?
{
    machine_data_req *  acc;
    machine_data_ret *  ret;
    unsigned_8 *        data;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    data = GetOutPtr( sizeof( *ret ) );
    return sizeof( *ret );
}

trap_retval ReqThread_get_next( void )
/*********************************/
{
    thread_get_next_req *acc;
    thread_get_next_ret *ret;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    ret->thread = GetNextThread( acc->thread, &ret->state );
    return sizeof( *ret );
}

trap_retval ReqThread_set( void )
/****************************/
{
    thread_set_req *acc;
    thread_set_ret *ret;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    ret->err = 0;
    ret->old_thread = SetThread( acc->thread );
    return sizeof( *ret );
}

trap_retval ReqThread_freeze( void )
/*******************************/
{
    thread_freeze_req   *acc;
    thread_freeze_ret   *ret;

    // nyi
    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );

    // freeze the thread associated with acc->thread

    ret->err = ERR_MSJ_THREADS_NOT_SUPPORTED;
    return sizeof( *ret );
}

trap_retval ReqThread_thaw( void )
/*****************************/
{
    thread_thaw_req *acc;
    thread_thaw_ret *ret;

    // nyi
    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    // thaw the thread associated with acc->thread
    ret->err = ERR_MSJ_THREADS_NOT_SUPPORTED;
    return sizeof( *ret );
}

trap_retval ReqThread_get_extra( void )
/**********************************/
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

