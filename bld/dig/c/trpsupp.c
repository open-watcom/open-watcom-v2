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
* Description:  Trap file access wrapper routine.
*
****************************************************************************/


#include <stddef.h>

//for trap file backwards compatability
#ifndef MD_x86
#define MD_x86
#endif
#include "madregs.h"

#include "trpimp.h"

#ifdef ENABLE_TRAP_LOGGING

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <winbase.h>    /* For GetSystemTime */

static FILE *TrapTraceFileHandle    = NULL;
static bool TrapTraceFileFlush      = FALSE;

int OpenTrapTraceFile( const char *path, bool flush_flag )
{
    if( TrapTraceFileHandle )
        return( -1 );
    if( NULL == ( TrapTraceFileHandle = fopen( path, "wb" ) ) )
        return( -1 );

    TrapTraceFileFlush = flush_flag;
    return( 0 );
}

int CloseTrapTraceFile( void )
{
    if( TrapTraceFileHandle ){
        fclose( TrapTraceFileHandle );
        TrapTraceFileHandle = NULL;
    }
    return 0;   
}
#endif

extern          void    DoHardModeCheck(void);

trap_version    TrapVer;
unsigned        ( TRAPENTRY *ReqFunc )( unsigned, mx_entry *,
                                        unsigned, mx_entry * );
unsigned        ReqFuncProxy( unsigned, mx_entry *,
                                        unsigned, mx_entry * );
static          void (*pFailure)(void) = NULL;
static          void (*pAccess)(void) = NULL;

static void Failure( void )
{
    if( pFailure ) pFailure();
}

static void Access( void )
{
    if( pAccess ) pAccess();
}

void TrapSetFailCallBack( void (*func)(void) )
{
    pFailure = func;
}

void TrapSetAccessCallBack( void (*func)(void) )
{
    pAccess = func;
}

void TrapFailAllRequests()
{
    ReqFunc = NULL;
}

unsigned TrapSimpAccess( unsigned in_len, void *in_data,
                        unsigned out_len, void *out_data )
{
    mx_entry            in[1];
    mx_entry            out[1];
    unsigned            len;

    in[0].ptr = in_data;
    in[0].len = in_len;
    if( out_len != 0 ) {
        out[0].ptr = out_data;
        out[0].len = out_len;
        len = TrapAccess( 1, in, 1, out );
    } else {
        len = TrapAccess( 1, in, 0, NULL );
    }
    return( len );
}


unsigned TrapAccess( unsigned num_in_mx,  mx_entry *mx_in,
                 unsigned num_out_mx, mx_entry *mx_out  )
{
    unsigned    len;
#if !defined(SERVER)
    unsigned    len2;
    union {
        read_cpu_req            cpu;
        read_fpu_req            fpu;
        addr_info_req           addr;
        get_sys_config_req      config;
        connect_req             connect;
    }                   acc;
    system_config       config;
    mad_registers       *mr;
    machine_data_ret    *md;
    addr_info_ret       ret;
    static unsigned_8   load_flags;
    mx_entry            in[1];
#endif

    if( ReqFunc == NULL )
        return( REQUEST_FAILED );

#if !defined(SERVER)
    switch( *(access_req *)mx_in[0].ptr ) {
    case REQ_CONNECT:
        if( TrapVer.minor > OLD_TRAP_MINOR_VERSION ) break;
        acc.connect.req = REQ_CONNECT;
        acc.connect.ver.major = TRAP_MAJOR_VERSION;
        acc.connect.ver.minor = OLD_TRAP_MINOR_VERSION;
        in[0].ptr = &acc;
        in[0].len = sizeof( acc.connect );
        mx_in = in;
        num_in_mx = 1;
        break;
    case REQ_READ_REGS:
        if( TrapVer.minor > OLD_TRAP_MINOR_VERSION ) break;
        mr = mx_out[0].ptr;
        acc.cpu.req = REQ_READ_CPU;
        len = TrapSimpAccess( sizeof(acc.cpu), &acc, sizeof( mr->x86.cpu ), &mr->x86.cpu );
        if( len == REQUEST_FAILED ) return( len );
        acc.fpu.req = REQ_READ_FPU;
        len2 = TrapSimpAccess( sizeof(acc.fpu), &acc, sizeof( mr->x86.fpu ), &mr->x86.fpu );
        if( len2 == REQUEST_FAILED ) return( len2 );
        return( len+len2 );
    case REQ_WRITE_REGS:
        if( TrapVer.minor > OLD_TRAP_MINOR_VERSION ) break;
        mr = mx_in[1].ptr;
        mx_in[1].ptr = &mr->x86.cpu;
        mx_in[1].len = sizeof( mr->x86.cpu );
        *(access_req *)mx_in[0].ptr = REQ_WRITE_CPU;
        len = ReqFuncProxy( num_in_mx, mx_in, num_out_mx, mx_out );
        if( len == REQUEST_FAILED ) return( len );
        mx_in[1].ptr = &mr->x86.fpu;
        mx_in[1].len = sizeof( mr->x86.fpu );
        *(access_req *)mx_in[0].ptr = REQ_WRITE_FPU;
        len2 = ReqFuncProxy( num_in_mx, mx_in, num_out_mx, mx_out );
        if( len2 == REQUEST_FAILED ) return( len2 );
        return( len+len2 );
    case REQ_GET_SYS_CONFIG:
        if( TrapVer.minor > OLD_TRAP_MINOR_VERSION ) break;
        len = ReqFuncProxy( num_in_mx, mx_in, num_out_mx, mx_out );
        if( len == REQUEST_FAILED ) return( len );
        ((system_config *)mx_out[0].ptr)->mad = MAD_X86;
        return( len );
    case REQ_MACHINE_DATA:
        if( TrapVer.minor > OLD_TRAP_MINOR_VERSION ) break;
        acc.addr.req = REQ_ADDR_INFO;
        acc.addr.in_addr = ((machine_data_req *)mx_in[0].ptr)->addr;
        len = TrapSimpAccess( sizeof( acc.addr ), &acc.addr, sizeof( ret ), &ret );
        if( len == REQUEST_FAILED ) return( len );
        md = mx_out[0].ptr;
        md->cache_start = 0;
        md->cache_end   = ~(addr_off)0;
        *((unsigned_8 *)mx_out[1].ptr) = 0;
        if( ret.is_32 ) {
            *((unsigned_8 *)mx_out[1].ptr) |= X86AC_BIG;
        }
        if( !(load_flags & LD_FLAG_IS_PROT) ) {
            *((unsigned_8 *)mx_out[1].ptr) |= X86AC_REAL;
        }
        return( sizeof( machine_data_ret ) + 1 );
    case REQ_PROG_LOAD:
        if( TrapVer.minor > OLD_TRAP_MINOR_VERSION ) break;
        len = ReqFuncProxy( num_in_mx, mx_in, num_out_mx, mx_out );
        if( len == REQUEST_FAILED ) return( len );
        acc.config.req = REQ_GET_SYS_CONFIG;
        len2 = TrapSimpAccess( sizeof( acc.config ), &acc.config, sizeof( config ), &config );
        if( len2 == REQUEST_FAILED ) return( len2 );
        switch( config.os ) {
        case OS_AUTOCAD:
        case OS_RATIONAL:
            ((prog_load_ret *)mx_out[0].ptr)->flags |= LD_FLAG_DISPLAY_DAMAGED;
            break;
        case OS_OS2:
        case OS_NT:
        case OS_WINDOWS:
            ((prog_load_ret *)mx_out[0].ptr)->flags |= LD_FLAG_HAVE_RUNTIME_DLLS;
            break;
        }
        load_flags = ((prog_load_ret *)mx_out[0].ptr)->flags;
        return( len );
    }
#endif
    len = ReqFuncProxy( num_in_mx, mx_in, num_out_mx, mx_out );
    if( len == REQUEST_FAILED )
        Failure();
    Access();
#if !defined(SERVER)
#if defined(__WINDOWS__) && defined( _M_I86 )
    DoHardModeCheck();
#endif
#endif
    return( len );
}

unsigned ReqFuncProxy( unsigned num_in_mx, mx_entry * mx_in, unsigned num_out_mx, mx_entry * mx_out )
{
    unsigned result;

    if( ReqFunc == NULL )
        return( REQUEST_FAILED );

#ifdef ENABLE_TRAP_LOGGING
    if( TrapTraceFileHandle ) {
        unsigned        ix;
        unsigned short  rectype = 4;   /* Request */
        unsigned short  length = 0;
        SYSTEMTIME      st;

        GetSystemTime(&st);

        for( ix = 0 ; ix < num_in_mx ; ix++ ) {
            length += mx_in[ix].len;
        }
        fwrite( &rectype, sizeof( rectype ), 1, TrapTraceFileHandle );
        fwrite( &st.wHour, sizeof(WORD), 1, TrapTraceFileHandle );
        fwrite( &st.wMinute, sizeof(WORD), 1, TrapTraceFileHandle );
        fwrite( &st.wSecond, sizeof(WORD), 1, TrapTraceFileHandle );
        fwrite( &st.wMilliseconds, sizeof(WORD), 1, TrapTraceFileHandle );

        fwrite( &length, sizeof( length ), 1, TrapTraceFileHandle );
        for( ix = 0 ; ix < num_in_mx ; ix++ ) {
            fwrite( mx_in[ix].ptr, mx_in[ix].len, 1, TrapTraceFileHandle );
        }
        if( TrapTraceFileFlush ) {
            fflush( TrapTraceFileHandle );
        }
    }
#endif

    result = ReqFunc( num_in_mx, mx_in, num_out_mx, mx_out );

#ifdef ENABLE_TRAP_LOGGING
    if( TrapTraceFileHandle ) {
        /* result is the length of data returned or REQUEST_FAILED */
        /* Only worth tracing if there is data though */
        if( result > 0 ) {
            unsigned        ix;
            unsigned short  rectype = 5;   /* reply*/
            unsigned short  length = result;
            SYSTEMTIME      st;

            GetSystemTime(&st);

            fwrite( &rectype, sizeof( rectype ), 1, TrapTraceFileHandle );
            fwrite( &st.wHour, sizeof(WORD), 1, TrapTraceFileHandle );
            fwrite( &st.wMinute, sizeof(WORD), 1, TrapTraceFileHandle );
            fwrite( &st.wSecond, sizeof(WORD), 1, TrapTraceFileHandle );
            fwrite( &st.wMilliseconds, sizeof(WORD), 1, TrapTraceFileHandle );
            fwrite( &length, sizeof( length ), 1, TrapTraceFileHandle );

            for( ix = 0 ; ix < num_out_mx ; ix++ ) {
                unsigned to_write = __min( mx_out[ix].len, length );
                fwrite( mx_out[ix].ptr, to_write, 1, TrapTraceFileHandle );
                length -= to_write;
                if( 0 == length ) {
                    break;
                }
            }
            if( TrapTraceFileFlush ) {
                fflush( TrapTraceFileHandle );
            }
        }
    }
#endif

    return( result );
}
