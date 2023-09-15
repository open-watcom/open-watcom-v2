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
* Description:  Trap file for QNX interactive debugging.
*
****************************************************************************/


#include <stdlib.h>
#include <unistd.h>
#include <process.h>
#include <signal.h>
#include <fcntl.h>
#include <errno.h>
#include <limits.h>
#include <string.h>
#include <i86.h>
#include <sys/name.h>
#include <sys/proxy.h>
#include <sys/kernel.h>
#include <sys/debug.h>
#include <sys/stat.h>
#include <sys/proc_msg.h>
#include <sys/osinfo.h>
#include <sys/psinfo.h>
#include <sys/seginfo.h>
#include <sys/sched.h>
#include <sys/vc.h>
#include <sys/magic.h>
#include <sys/wait.h>
#include <sys/console.h>
#include <sys/dev.h>
#include <string.h>
#include "digcpu.h"
#include "trpimp.h"
#include "trpcomm.h"
#include "trperr.h"
#include "qnxcomm.h"
#include "qnxpath.h"
#include "miscx87.h"
#include "mad.h"
#include "madregs.h"
#include "walloca.h"


typedef unsigned short  USHORT;
typedef unsigned long   ULONG;

static pid_t            MID;
static pid_t            OrigPGrp;
static unsigned char    UtilBuff[BUFF_SIZE];
static struct {
    unsigned long       in;
    unsigned long       out;
    unsigned long       err;
}                       StdPos;

static int ForceFpu32;

enum {
    MH_NONE,
    MH_DEBUGGEE,
    MH_SLIB,
    MH_PROC
};

typedef struct {
        pid_t       tid;
        unsigned    frozen      : 1;
        unsigned    fork        : 1;
        unsigned    dying       : 1;
} thread_info;

typedef struct {
    unsigned    at_end          : 1;
    unsigned    loaded_proc     : 1;
    unsigned    stopped         : 1;
    unsigned    priv_level      : 2;
    unsigned    dbg32           : 1;
    unsigned    proc32          : 1;
    unsigned    fpu32           : 1;
    unsigned    flat            : 1;
    unsigned    fork            : 1;
    pid_t       pid;
    nid_t       nid;
    pid_t       proc;
    pid_t       mid;
    pid_t       son;
    int         sig;
    unsigned    version;
    long        cpu;
    long        fpu;
    long        sflags;
    int         save_in;
    int         save_out;
    addr48_ptr  magic;
    addr48_off  code_offset;
    addr48_off  data_offset;
    unsigned_32 last_eax;
    unsigned    max_threads;
    thread_info *thread;
}  process_info;


#define THREAD_GROW     10

static process_info ProcInfo;

#define PRIV_MASK   3

#define MAX_WATCHES     32

static struct _watch_struct WatchPoints[MAX_WATCHES];
static int                  WatchCount = 0;

#if 0
void Out( char *str )
{
    write( 1, (char *)str, strlen( str ) );
}

void OutNum( unsigned long i )
{
    char numbuff[16];
    char *ptr;

    ptr = numbuff+10;
    *--ptr = '\0';
    do {
        *--ptr = ( i % 10 ) + '0';
        i /= 10;
    } while( i != 0 );
    Out( ptr );
}
#endif

#define MAX_MEM_TRANS   256
static size_t MoveMem( int op, void *data, addr_seg segv, addr_off offv, size_t req_len )
{
    size_t              len;
    size_t              want;
    size_t              got;
    struct _seginfo     info;

    if( ProcInfo.pid == 0 )
        return( 0 );
    if( qnx_segment_info( ProcInfo.proc, ProcInfo.pid, segv, &info ) == -1 ) {
        info.nbytes = 0;
    }
    if( offv >= info.nbytes ) {
        req_len = 0;
    } else if( offv + req_len > info.nbytes ) {
        req_len = info.nbytes - offv;
    }
    want = MAX_MEM_TRANS;
    len = 0;
    for( ; req_len-- > 0; req_len -= want ) {
        if( want > req_len )
            want = req_len;
        if( __qnx_debug_xfer( ProcInfo.proc, ProcInfo.pid, op, data, want, offv, segv ) != 0 ) {
            /* something went wrong. need to find out how much trans'ed */
            //NYI
            got = 0;
        } else {
            got = want;
        }
        len += got;
        if( got != want ) {
            break;
        }
        data = (char *)data + want;
        offv += want;
    }
    return( len );
}


static size_t WriteBuffer( void *data, addr_seg segv, addr_off offv, size_t size )
{
    return( MoveMem( _DEBUG_MEM_WR, data, segv, offv, size ) );
}


static size_t ReadBuffer( void *data, addr_seg segv, addr_off offv, size_t size )
{
    return( MoveMem( _DEBUG_MEM_RD, data, segv, offv, size ) );
}

static thread_info *find_thread( pid_t tid )
{
    thread_info *thread;
    unsigned    new_count;

    for( thread = ProcInfo.thread; thread < &ProcInfo.thread[ProcInfo.max_threads]; thread++ ) {
        if( thread->tid == tid ) {
            if( tid == 0 )
                thread->dying = false;
            return( thread );
        }
    }
    if( tid == 0 ) {
        /* need to expand the array */
        new_count = ProcInfo.max_threads + THREAD_GROW;
        thread = realloc( ProcInfo.thread, new_count * sizeof( *thread ) );
        if( thread != NULL ) {
            ProcInfo.thread = thread;
            thread = &thread[ProcInfo.max_threads];
            ProcInfo.max_threads = new_count;
            memset( thread, 0, THREAD_GROW * sizeof( *thread ) );
            return( thread );
        }
    }
    return( NULL );
}

#define THREAD_ALL      (-1)
#define NO_TID          (INT_MAX)

static pid_t next_thread( pid_t last, int state ) {
    thread_info         *thread;
    pid_t               tid = NO_TID;
    pid_t               curr;

    for( thread = ProcInfo.thread; thread < &ProcInfo.thread[ProcInfo.max_threads]; thread++ ) {
        if( thread->dying )
            continue;
        curr = thread->tid;
        switch( state ) {
        case THREAD_ALL:
            break;
        case THREAD_THAWED:
            if( thread->frozen && (ProcInfo.pid != curr) )
                continue;
            break;
        case THREAD_FROZEN:
            if( !thread->frozen )
                continue;
            break;
        }
        if( curr > last && curr < tid ) {
            tid = curr;
        }
    }
    if( tid == NO_TID )
        return( 0 );
    return( tid );
}


trap_retval TRAP_CORE( Get_sys_config )( void )
{
    struct  _osinfo     info;
    get_sys_config_ret  *ret;

    ret = GetOutPtr( 0 );
    if( ProcInfo.pid == 0 )  {
        qnx_osinfo( 0, &info );
    } else {
        info.version = ProcInfo.version;
        info.cpu = ProcInfo.cpu;
        info.fpu = ProcInfo.fpu;
        info.sflags = ProcInfo.sflags;
    }
    ret->os = DIG_OS_QNX;
    ret->osmajor = info.version / 100;
    ret->osminor = info.version % 100;
    if( info.sflags & _PSF_EMULATOR_INSTALLED ) {
        ret->fpu = X86_EMU;
    } else if( (info.sflags & _PSF_NDP_INSTALLED) == 0 ) {
        ret->fpu = X86_NOFPU;
    } else {
        switch( info.fpu ) {
        case 87:
            ret->fpu = X86_87;
            break;
        case 287:
            ret->fpu = X86_287;
            break;
        case 387:
        default:
            ret->fpu = X86_387;
            break;
        }
    }
    switch( info.cpu ) {
    case 8088:
        ret->cpu = X86_86;
        break;
    case 186:
        ret->cpu = X86_186;
        break;
    case 286:
        ret->cpu = X86_286;
        break;
    case 386:
    default:
        ret->cpu = X86_386;
        break;
    case 486:
        ret->cpu = X86_486;
        break;
    case 586:
        ret->cpu = X86_586;
        break;
    }
    if( info.sflags & _PSF_PROTECTED ) {
        ret->huge_shift = 3;
    } else {
        ret->huge_shift = 12;
    }
    ret->arch = DIG_ARCH_X86;
    return( sizeof( *ret ) );
}

struct _slib_register {
    msg_t       msgcode;
    short       rsvd1;
    long        rsvd[3];
};
struct _slib_register_reply {
    msg_t       msgcode;
    addr48_ptr  dispatch;
    long        rsvd[2];
};
#define _SLIB_NAME "qnx/syslib"
#define _SLIB_REGISTER  0x1000

static addr48_ptr GetSLibTable( bool is_32 )
{
    addr48_ptr          slib;
    addr32_ptr          slib16;
    union {
        struct _slib_register   smsg;
        struct _slib_register_reply     rmsg;
    }                   msg;
    pid_t               pid;
    int                 ret;

    slib.segment = 0;
    slib.offset  = 0;
    /* get shared library function table pointer */
    if( is_32 ) {
        pid = qnx_name_locate( ProcInfo.nid, _SLIB_NAME, sizeof( msg ), NULL );
        if( pid == -1 ) {
            return( slib );
        }
        msg.smsg.msgcode = _SLIB_REGISTER;
        ret = Send( pid, &msg, &msg, sizeof( msg.smsg ), sizeof( msg.rmsg ) );
        qnx_vc_detach( pid );
        if( ret == -1 || msg.rmsg.msgcode != EOK ) {
            return( slib );
        }
        slib = msg.rmsg.dispatch;
    } else {
        if( __qnx_debug_xfer( ProcInfo.proc, ProcInfo.pid, _DEBUG_MEM_RD, &slib16,
            sizeof( slib16 ), ProcInfo.magic.offset+offsetof(struct _magic,sptrs[0]),
                                                  ProcInfo.magic.segment ) != 0 ) {
            return( slib );
        }
        slib.segment = slib16.segment;
        slib.offset  = slib16.offset;
    }
    return( slib );
}


trap_retval TRAP_CORE( Map_addr )( void )
{
    map_addr_req        *acc;
    map_addr_ret        *ret;
    addr48_ptr          slib;
    unsigned            seg;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    ret->lo_bound = 0;
    ret->hi_bound = ~(addr48_off)0;
    ret->out_addr.offset = acc->in_addr.offset;
    seg = acc->in_addr.segment;
    switch( seg ) {
    case MAP_FLAT_CODE_SELECTOR:
        seg = 0x04;
        break;
    case MAP_FLAT_DATA_SELECTOR:
        seg = 0x0c;
        break;
    }
    if( ProcInfo.pid != 0 ) {
        switch( acc->mod_handle ) {
        case MH_DEBUGGEE:
            if( ProcInfo.flat ) {
                switch( acc->in_addr.segment & ~PRIV_MASK ) {
                case 0x04:
                case MAP_FLAT_CODE_SELECTOR & ~PRIV_MASK:
                /* case MAP_FLAT_DATA_SELECTOR & ~PRIV_MASK: Same as above */
                    ret->out_addr.offset += ProcInfo.code_offset;
                    break;
                default:
                    ret->out_addr.offset += ProcInfo.data_offset;
                    break;
                }
            }
            break;
        case MH_SLIB:
            slib = GetSLibTable( ProcInfo.dbg32 );
            seg += slib.segment - 4;
            break;
        case MH_PROC:
            seg += 0xE0 - 4;
            break;
        }
    }
    if( ProcInfo.sflags & _PSF_PROTECTED ) {
        ret->out_addr.segment = (seg & ~PRIV_MASK) | ProcInfo.priv_level;
    }
    return( sizeof( *ret ) );
}


trap_retval TRAP_CORE( Checksum_mem )( void )
{
    addr_off            offv;
    word                segv;
    size_t              len;
    size_t              want;
    size_t              i;
    size_t              got;
    ULONG               sum;
    checksum_mem_req    *acc;
    checksum_mem_ret    *ret;

    sum = 0;
    if( ProcInfo.pid != 0 ) {
        acc = GetInPtr( 0 );
        want = sizeof( UtilBuff );
        offv = acc->in_addr.offset;
        segv = acc->in_addr.segment;
        for( len = acc->len; len > 0; len -= want ) {
            if( want > len )
                want = len;
            got = MoveMem( _DEBUG_MEM_RD, UtilBuff, segv, offv, want );
            for( i = 0; i < got; ++i ) {
                sum += UtilBuff[i];
            }
            if( got != want ) {
                break;
            }
            offv += want;
        }
    }
    ret = GetOutPtr( 0 );
    ret->result = sum;
    return( sizeof( *ret ) );
}


trap_retval TRAP_CORE( Read_mem )( void )
{
    read_mem_req        *acc;

    acc = GetInPtr( 0 );
    return( ReadBuffer( GetOutPtr( 0 ), acc->mem_addr.segment, acc->mem_addr.offset, acc->len ) );
}


trap_retval TRAP_CORE( Write_mem )( void )
{
    write_mem_req       *acc;
    write_mem_ret       *ret;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    ret->len = WriteBuffer( GetInPtr( sizeof( *acc ) ), acc->mem_addr.segment,
                        acc->mem_addr.offset, GetTotalSizeIn() - sizeof(*acc) );
    return( sizeof( *ret ) );
}


trap_retval TRAP_CORE( Read_io )( void )
{
    union _port_struct  port;
    read_io_req         *acc;
    void                *data;
    size_t              len;

    acc = GetInPtr( 0 );
    data = GetOutPtr( 0 );
    len = acc->len;
    if( __qnx_debug_xfer( ProcInfo.proc, ProcInfo.pid, _DEBUG_IO_RD, &port, len, acc->IO_offset, 0 ) == 0 ) {
        switch( len ) {
        case 1:
            *(unsigned_8 *)data = port.byte;
            break;
        case 2:
            *(unsigned_16 *)data = port.word;
            break;
        case 4:
            *(unsigned_32 *)data = port.dword;
            break;
        default:
            len = 0;
            break;
        }
        return( len );
    }
    return( 0 );
}


trap_retval TRAP_CORE( Write_io )( void )
{
    union _port_struct  port;
    write_io_req        *acc;
    write_io_ret        *ret;
    void                *data;
    size_t              len;

    acc = GetInPtr( 0 );
    data = GetInPtr( sizeof( *acc ) );
    len = GetTotalSizeIn() - sizeof( *acc );
    switch( len ) {
    case 1:
        port.byte = *(unsigned_8 *)data;
        break;
    case 2:
        port.word = *(unsigned_16 *)data;
        break;
    case 4:
        port.dword = *(unsigned_32 *)data;
        break;
    default:
        len = 0;
        break;
    }
    if( len > 0 ) {
        if( __qnx_debug_xfer( ProcInfo.proc, ProcInfo.pid, _DEBUG_IO_WR, &port, len, acc->IO_offset, 0 ) != 0 ) {
            len = 0;
        }
    }
    ret = GetOutPtr( 0 );
    ret->len = len;
    return( sizeof( *ret ) );
}

static void ReadCPU( struct x86_cpu *r )
{
    struct _reg_struct  qnx_regs;

    memset( r, 0, sizeof( *r ) );
    ProcInfo.last_eax = 0;
    if( ProcInfo.pid != 0 && __qnx_debug_xfer( ProcInfo.proc, ProcInfo.pid, _DEBUG_REG_RD, &qnx_regs,
                        sizeof( qnx_regs ), 0, 0 ) == 0 ) {
        ProcInfo.last_eax = qnx_regs.ax;
        r->eax = qnx_regs.ax;
        r->ebx = qnx_regs.bx;
        r->ecx = qnx_regs.cx;
        r->edx = qnx_regs.dx;
        r->esi = qnx_regs.si;
        r->edi = qnx_regs.di;
        r->ebp = qnx_regs.bp;
        r->esp = qnx_regs.sp;
        r->eip = qnx_regs.ip;
        r->efl = qnx_regs.fl;
        r->cs = qnx_regs.cs;
        r->ds = qnx_regs.ds;
        r->ss = qnx_regs.ss;
        r->es = qnx_regs.es;
        r->fs = qnx_regs.fs;
        r->gs = qnx_regs.gs;
    }
}

static void ReadFPU( struct x86_fpu *r )
{
    unsigned    state_size;

    memset( r, 0, sizeof( *r ) );
    if( ProcInfo.fpu32 ) {
        state_size = 108;
    } else {
        state_size = 94;
    }
    if( ProcInfo.pid != 0 && __qnx_debug_xfer( ProcInfo.proc, ProcInfo.pid, _DEBUG_80X87_RD, r,
                        state_size, 0, 0 ) == 0 ) {
        if( !ProcInfo.fpu32 ) {
            FPUExpand( r );
        }
    }
}

trap_retval TRAP_CORE( Read_regs )( void )
{
    mad_registers       *mr;

    mr = GetOutPtr( 0 );

    ReadCPU( &mr->x86.cpu );
    ReadFPU( &mr->x86.u.fpu );
    return( sizeof( mr->x86 ) );
}

static void WriteCPU( struct x86_cpu *r )
{
    struct _reg_struct  qnx_regs;
    unsigned            suppress;

    qnx_regs.ax = r->eax;
    qnx_regs.bx = r->ebx;
    qnx_regs.cx = r->ecx;
    qnx_regs.dx = r->edx;
    qnx_regs.si = r->esi;
    qnx_regs.di = r->edi;
    qnx_regs.bp = r->ebp;
    qnx_regs.sp = r->esp;
    qnx_regs.ip = r->eip;
    qnx_regs.fl = r->efl;
    qnx_regs.cs = r->cs;
    qnx_regs.ds = r->ds;
    qnx_regs.ss = r->ss;
    qnx_regs.es = r->es;
    qnx_regs.fs = r->fs;
    qnx_regs.gs = r->gs;
    suppress = 0;
    if( qnx_regs.ax == ProcInfo.last_eax ) {
        suppress |= 1 << 11;
    }
    __qnx_debug_xfer( ProcInfo.proc, ProcInfo.pid, _DEBUG_REG_WR, &qnx_regs,
                sizeof( qnx_regs ), 0, suppress );
}

static void WriteFPU( struct x86_fpu *r )
{
    unsigned            state_size;

    if( ProcInfo.pid != 0 ) {
        if( !ProcInfo.fpu32 )
            FPUContract( r );
        if( ProcInfo.fpu32 ) {
            state_size = 108;
        } else {
            state_size = 94;
        }
        __qnx_debug_xfer( ProcInfo.proc, ProcInfo.pid, _DEBUG_80X87_WR, r, state_size, 0, 0);
    }
}

trap_retval TRAP_CORE( Write_regs )( void )
{
    mad_registers       *mr;

    mr = GetInPtr( sizeof( write_regs_req ) );
    WriteCPU( &mr->x86.cpu );
    WriteFPU( &mr->x86.u.fpu );
    return( 0 );
}

static int SplitParms( char *p, const char **args, size_t len )
/**************************************************************
 * Break up program arguments passed in as a single string into
 * individual components. Useful for passing argv style array to
 * exec().
 */
{
    int     i;
    char    endc;

    i = 0;
    if( len == 1 )
        goto done;
    for( ;; ) {
        for( ;; ) {
            if( len == 0 )
                goto done;
            if( *p != ' ' && *p != '\t' )
                break;
            ++p;
            --len;
        }
        if( len == 0 )
            goto done;
        if( *p == '"' ) {
            --len;
            ++p;
            endc = '"';
        } else {
            endc = ' ';
        }
        if( args != NULL )
            args[i] = p;
        ++i;
        for( ;; ) {
            if( len == 0 )
                goto done;
            if( *p == endc || *p == '\0' || ( endc == ' ' && *p == '\t' ) ) {
                /*
                 * if output array is not specified then source string
                 * is not changed and it calculates number of parameters only
                 * as soon as output array is specified then source is modified
                 */
                if( args != NULL ) {
                    *p = '\0';
                }
                ++p;
                --len;
                if( len == 0 )
                    goto done;
                break;
            }
            ++p;
            --len;
        }
    }
done:
    return( i );
}

static pid_t RunningProc( nid_t *nid, const char *name, struct _psinfo *info, const char **name_ret )
{
    pid_t       pid;
    pid_t       proc;
    char        ch;
    const char  *ptr;

    ptr = CollectNid( name, strlen( name ), nid );
    for( ;; ) {
        ch = *ptr;
        if( ch != ' ' && ch != '\t' )
            break;
        ++ptr;
    }
    if( name_ret != NULL )
        *name_ret = (char *)ptr;
    pid = 0;
    for( ;; ) {
        ch = *ptr;
        if( ch < '0' || ch > '9' )
            break;
        pid = ( pid * 10 ) + ( ch - '0' );
        ++ptr;
    }
    if( *ptr != '\0' )
        return( 0 );
    for( ;; ) {
        proc = qnx_vc_attach( *nid, PROC_PID,
                     sizeof( struct _proc_psinfo_reply ), 0 );
        info->pid = 0;
        qnx_psinfo( proc, pid, info, 0, 0 );
        qnx_vc_detach( proc );
        if( info->pid != pid || (info->flags & (_PPF_MID | _PPF_VMID)) ) {
            pid = 0;
            break;
        }
        if( (info->flags & _PPF_VID) == 0 )
            break;
        *nid = info->un.vproc.remote_nid;
        pid = info->un.vproc.remote_pid;
    }
    return( pid );
}

static int net_kill( pid_t proc, pid_t pid, int signum )
{
    union {
        struct _proc_signal         s;
        struct _proc_signal_reply   r;
    } msg;

    msg.s.type = _PROC_SIGNAL;
    msg.s.subtype = _SIGRAISE;
    msg.s.pid = pid;
    msg.s.signum = signum;

    if( Send( proc, &msg.s, &msg.r, sizeof(msg.s), sizeof(msg.r) ) == -1 ) {
        return( -1 );
    }

    if( msg.r.status != EOK ) {
        errno = msg.r.status;
        return( -1 );
    }
    return( 0 );
}

trap_retval TRAP_CORE( Prog_load )( void )
{
    const char                  **args;
    char                        *parms;
    char                        *parm_start;
    int                         i;
    char                        exe_name[PATH_MAX + 1];
    struct _osinfo              info;
    struct _psinfo              proc;
    struct _debug_psinfo        off_info;
    nid_t                       nid;
    const char                  *name;
    pid_t                       save_pgrp;
    prog_load_req               *acc;
    prog_load_ret               *ret;
    bool                        has_flat;
    size_t                      len;
    char                        *p;

    lseek( 0, StdPos.in , SEEK_SET );
    lseek( 1, StdPos.out, SEEK_SET );
    lseek( 2, StdPos.err, SEEK_SET );

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );

    ProcInfo.sig  = -1;
    ProcInfo.proc = PROC_PID;
    ProcInfo.sflags = 0;
    ProcInfo.at_end = false;
    ProcInfo.dbg32  = false;
    ProcInfo.proc32 = false;
    ProcInfo.flat   = false;
    ProcInfo.fpu32  = false;
    ProcInfo.fork   = false;
    memset( ProcInfo.thread, 0, sizeof( ProcInfo.thread[0] ) * ProcInfo.max_threads );
    parms = parm_start = GetInPtr( sizeof( *acc ) );
    len = GetTotalSizeIn() - sizeof( *acc );
    if( acc->true_argv ) {
        i = 1;
        while( len-- > 0 ) {
            if( *parms++ == '\0' ) {
                i++;
            }
        }
        args = walloca( i * sizeof( *args ) );
        parms = parm_start;
        len = GetTotalSizeIn() - sizeof( *acc );
        i = 1;
        while( len-- > 0 ) {
            if( *parms++ == '\0' ) {
                args[i++] = parms;
            }
        }
    } else {
        while( --len, *parms++ != '\0' )
            {}
        i = SplitParms( parms, NULL, len ) + 2;
        args = alloca( i * sizeof( *args ) + len );
        p = memcpy( (void *)( args + i ), parms, len );
        SplitParms( p, args + 1, len );
    }
    args[0] = parm_start;
    args[i - 1] = NULL;
    ProcInfo.pid = RunningProc( &nid, args[0], &proc, &name );
    if( ProcInfo.pid != 0 ) {
        ProcInfo.loaded_proc = false;
    } else {
        args[0] = name;
        FindFilePath( DIG_FILETYPE_EXE, args[0], exe_name );
        save_pgrp = getpgrp();
        setpgid( 0, OrigPGrp );
        ProcInfo.pid = qnx_spawn(0, 0, nid, -1, SCHED_OTHER,
                            _SPAWN_HOLD, //NYI: | _SPAWN_NOZOMBIE,
                            exe_name, (char **)args, dbg_environ, 0, -1);
        setpgid( 0, save_pgrp );
        if( ProcInfo.pid != -1 ) {
            qnx_psinfo( PROC_PID, ProcInfo.pid, &proc, 0, 0 );
        }
        ProcInfo.loaded_proc = true;
    }
    ret->flags = 0;
    if( ProcInfo.pid != -1 ) {
        //NYI: what if the attaches fail?
        ProcInfo.proc = qnx_vc_attach( nid, PROC_PID, 1000, 0 );
        ProcInfo.mid = qnx_proxy_rem_attach( nid, MID );
        //NYI: temp kludge
        if( ProcInfo.mid == PROC_PID )
            ProcInfo.mid = MID;
        ProcInfo.nid = nid;
        if( proc.flags & _PPF_VID ) {
            ProcInfo.pid = proc.un.vproc.remote_pid;
            qnx_psinfo( ProcInfo.proc, ProcInfo.pid, &proc, 0, 0 );
        }
        if( proc.flags & _PPF_32BIT ) {
            ProcInfo.dbg32 = true;
            ret->flags |= LD_FLAG_IS_BIG;
        }
        ProcInfo.priv_level = proc.ss_reg & PRIV_MASK;
        ProcInfo.magic.offset = proc.magic_off;
        ProcInfo.magic.segment = proc.magic_sel;
        if( __qnx_debug_attach( ProcInfo.proc, ProcInfo.pid, ProcInfo.mid ) != 0 ) {
            if( ProcInfo.loaded_proc ) {
                net_kill( ProcInfo.proc, ProcInfo.pid, SIGKILL );
            }
            goto fail;
        }
        if( !ProcInfo.loaded_proc ) {
            __qnx_debug_hold( ProcInfo.proc, ProcInfo.pid );
            ret->flags |= LD_FLAG_IS_STARTED;
        }
        ret->task_id = ProcInfo.pid;
        qnx_osinfo( ProcInfo.nid, &info );
        ProcInfo.cpu = info.cpu;
        ProcInfo.fpu = info.fpu;
        ProcInfo.version = info.version;
        ProcInfo.sflags = info.sflags;
        if( info.sflags & _PSF_PROTECTED ) {
            ret->flags |= LD_FLAG_IS_PROT;
        }
        if( info.sflags & _PSF_32BIT ) {
            ProcInfo.proc32 = true;
        }
        has_flat = false;
        if( __qnx_debug_xfer( ProcInfo.proc, ProcInfo.pid, _DEBUG_PSINFO,
                            &off_info, sizeof( off_info ), 0, 0 ) != -1 ) {
            if( proc.flags & _PPF_FLAT ) {
                ProcInfo.flat = true;
                ProcInfo.code_offset = off_info.codeoff;
                ProcInfo.data_offset = off_info.codeoff + off_info.codesize;
            }
            has_flat = true;
        }
        if( ForceFpu32 < 0 ) {
            ProcInfo.fpu32 = false;
        } else if( ForceFpu32 > 0 ) {
            ProcInfo.fpu32 = true;
        } else if( ProcInfo.dbg32 ) {
            ProcInfo.fpu32 = true;
        } else if( info.sflags & _PSF_EMU16_INSTALLED ) {
            /* 16-bit emulator always writes out 16-bit state */
            ProcInfo.fpu32 = false;
        } else if( !ProcInfo.proc32 ) {
            ProcInfo.fpu32 = false;
        } else if( has_flat ) {
            /* Proc32's that support flat model save 16-bit floating point
               state as the full 32-bit form */
            ProcInfo.fpu32 = true;
        } else {
            ProcInfo.fpu32 = false;
        }

        ProcInfo.thread[0].tid = ProcInfo.pid;
        errno = 0;
    }
fail:
    ret->err = errno;
    if( ret->err != 0 ) {
        qnx_vc_detach( ProcInfo.proc );
        ProcInfo.pid = 0;
    }
    ret->mod_handle = MH_DEBUGGEE;
    return( sizeof( *ret ) );
}

trap_retval TRAP_CORE( Prog_kill )( void )
{
    prog_kill_ret       *ret;
    pid_t               pid;

    for( pid = 0; pid = next_thread( pid, THREAD_ALL ); ) {
        if( ProcInfo.loaded_proc && !ProcInfo.at_end ) {
            net_kill( ProcInfo.proc, pid, SIGKILL );
            if( ProcInfo.nid == 0 ) { //NYI: temp kludge
                /* receive the MID signalling task death */
                Receive( 0, 0, 0 );
            }
        }
        __qnx_debug_detach( ProcInfo.proc, pid );
        if( ProcInfo.loaded_proc ) {
            waitpid( -1, NULL, 0 );
        }
    }
    if( ProcInfo.pid ) {
        qnx_proxy_rem_detach( ProcInfo.nid, ProcInfo.mid );
        qnx_vc_detach( ProcInfo.proc );
        ProcInfo.proc = PROC_PID;
    }
    ProcInfo.sig = -1;
    ProcInfo.sflags = 0;
    ProcInfo.at_end = false;
    ProcInfo.save_in = -1;
    ProcInfo.save_out = -1;
    ret = GetOutPtr( 0 );
    ret->err = 0;
    return( sizeof( *ret ) );
}


trap_retval TRAP_CORE( Set_break )( void )
{
    long             opcode;
    set_break_req       *acc;
    set_break_ret       *ret;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );

    __qnx_debug_brk( ProcInfo.proc, ProcInfo.pid, _DEBUG_BRK_SET, &opcode,
                     acc->break_addr.offset, acc->break_addr.segment );
    ret->old = opcode;
    return( sizeof( *ret ) );
}

trap_retval TRAP_CORE( Clear_break )( void )
{
    long                 opcode;
    clear_break_req     *acc;

    acc = GetInPtr( 0 );
    opcode = acc->old;
    __qnx_debug_brk( ProcInfo.proc, ProcInfo.pid, _DEBUG_BRK_CLR, &opcode,
                acc->break_addr.offset, acc->break_addr.segment );
    return( 0 );
}

trap_retval TRAP_CORE( Set_watch )( void )
{
    int             size;
    set_watch_req   *acc;
    set_watch_ret   *ret;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    ret->multiplier = 1000;
    ret->err = 0;   // OK
    for( size = acc->size; size-- > 0; ) {
        if( WatchCount >= MAX_WATCHES ) {
            ret->err = 1;   // failure
            break;
        }
        WatchPoints[WatchCount].seg = acc->watch_addr.segment;
        WatchPoints[WatchCount].off = acc->watch_addr.offset;
#if 0
        ReadBuffer( &WatchPoints[WatchCount].byte,
            WatchPoints[WatchCount].seg, WatchPoints[WatchCount].off, 1 );
#endif
        ++acc->watch_addr.offset;
        ++WatchCount;
    }
    return( sizeof( *ret ) );
}

trap_retval TRAP_CORE( Clear_watch )( void )
{
    /* assume all are cleared at the same time */
    WatchCount = 0;
    return( 0 );
}

static void RunHandler( int sig )
{
    sig = sig;
    ProcInfo.stopped = true;
    /* Running this signal handler will cause the Receive to terminate */
}

static trap_conditions RunIt( unsigned step )
{
    struct _debug_info  info;
    void                (*old)();
    pid_t               pid;
    trap_conditions     conditions;
    thread_info         *thread;
    thread_info         *new;
    unsigned            i;

    ProcInfo.stopped = true;
    for( thread = ProcInfo.thread; thread < &ProcInfo.thread[ProcInfo.max_threads]; thread++ ) {
        if( thread->tid && thread->fork && !thread->frozen ) {
            __qnx_debug_cont( ProcInfo.proc, thread->tid, 0 );
            thread->tid = 0;
            ProcInfo.stopped = false;
        }
    }
    if( !ProcInfo.stopped ) {
        return( COND_THREAD );
    }
    for( thread = ProcInfo.thread; thread < &ProcInfo.thread[ProcInfo.max_threads]; thread++ ) {
        if( thread->tid == 0 )
            continue;
        if( step == 0 || thread->tid == ProcInfo.pid ) {
            if( !thread->frozen ) {
                if( __qnx_debug_cont( ProcInfo.proc, thread->tid, step ) == 0 ) {
                    ProcInfo.stopped = false;
                }
            }
        }
    }
    if( !ProcInfo.stopped ) {
        old = signal( SIGINT, &RunHandler );
        Receive( MID, 0, 0 );
        signal( SIGINT, old );
        if( ProcInfo.stopped ) {
            if( step ) {
                __qnx_debug_hold( ProcInfo.proc, ProcInfo.pid );
            } else {
                for( thread = ProcInfo.thread; thread < &ProcInfo.thread[ProcInfo.max_threads]; thread++ ) {
                    if( thread->tid != 0 ) {
                        __qnx_debug_hold( ProcInfo.proc, thread->tid );
                    }
                }
            }
            return( COND_USER );
        }
        conditions = COND_NONE;
        for( i = 0; i < ProcInfo.max_threads; i++ ) {
            /*
                Don't walk the thread array directly because it might move
                due to a realloc.
            */
            thread = &ProcInfo.thread[i];
            if( thread->frozen )
                continue;
            pid = thread->tid;
            if( pid == 0 )
                continue;
            if( step == 0 || pid == ProcInfo.pid ) {
                __qnx_debug_xfer( ProcInfo.proc, pid, _DEBUG_INFO, &info, sizeof( info ), 0, 0 );
                switch( info.debug_state ) {
                case _DEBUG_STATE_CONT:
                    __qnx_debug_hold( ProcInfo.proc, pid );
                    break;
                case _DEBUG_STATE_HELD:
                    conditions |= COND_USER;
                    if( pid != ProcInfo.pid ) {
                        ProcInfo.pid = pid;
                        conditions |= COND_THREAD;
                    }
                    break;
                case _DEBUG_STATE_TRACE:
                    conditions |= COND_TRACE;
                    if( pid != ProcInfo.pid ) {
                        ProcInfo.pid = pid;
                        conditions |= COND_THREAD;
                    }
                    break;
                case _DEBUG_STATE_BRK:
                    conditions |= COND_BREAK;
                    if( pid != ProcInfo.pid ) {
                        ProcInfo.pid = pid;
                        conditions |= COND_THREAD;
                    }
                    break;
                case _DEBUG_STATE_WATCH:
                    conditions |= COND_WATCH;
                    if( pid != ProcInfo.pid ) {
                        ProcInfo.pid = pid;
                        conditions |= COND_THREAD;
                    }
                    break;
                case _DEBUG_STATE_DEAD:
                    thread->dying = true;
                    if( ( pid = next_thread( 0, THREAD_THAWED ) ) || ( pid = next_thread( 0, THREAD_FROZEN ) ) ) {
                        __qnx_debug_detach( ProcInfo.proc, thread->tid );
                        thread->tid = 0;
                        find_thread(pid)->frozen = false;
                        ProcInfo.pid = pid;
                        conditions |= COND_THREAD;
                    } else {
                        thread->dying = false;
                        if( pid != ProcInfo.pid ) {
                            ProcInfo.pid = pid;
                            conditions |= COND_THREAD;
                        }
                        ProcInfo.at_end = true;
                        conditions |= COND_TERMINATE;
                    }
                    break;
                case _DEBUG_STATE_SIGNAL:
                    __qnx_debug_sigclr( ProcInfo.proc, pid, info.signo );
                    if( info.signo == SIGINT ) {
                        conditions |= COND_USER;
                    } else {
                        ProcInfo.sig = info.signo;
                        conditions = COND_EXCEPTION;
                    }
                    break;
                case _DEBUG_STATE_FORK:
                    ProcInfo.fork = 1;
                    ProcInfo.son = info.messenger;
                    if( new = find_thread( 0 ) ) {
                        new->tid = ProcInfo.son;
                        new->frozen = true;
                        new->fork = true;
                        conditions |= COND_THREAD;
                    }
                    conditions |= COND_MESSAGE;
                    break;
                case _DEBUG_STATE_THREAD:
                    ProcInfo.son = info.messenger;
                    if( new = find_thread( 0 ) ) {
                        if( __qnx_debug_attach( ProcInfo.proc, ProcInfo.son, ProcInfo.mid ) == 0 ) {
                            new->tid = ProcInfo.son;
                            new->frozen = false;
                        }
                    }
                    conditions |= COND_THREAD;
                    break;
                }
            }
        }
        return( conditions );
    }
    ProcInfo.at_end = true;
    return( COND_TERMINATE );
}

static unsigned ProgRun( bool step )
{
    struct _reg_struct  regs;
    prog_go_ret         *ret;
    pid_t               pid;

    ret = GetOutPtr( 0 );
    if( ProcInfo.at_end ) {
        ret->conditions = COND_TERMINATE;
    } else if( step ) {
        ret->conditions = RunIt( 1 );
    } else {
#if 0
        {
            int i;

            for( i = 0; i < WatchCount; ++i ) {
                Out( "watch at " );
                OutNum( WatchPoints[i].seg );
                Out( ":" );
                OutNum( WatchPoints[i].off );
                Out( "\n" );
            }
        }
#endif
        for( pid = 0; pid = next_thread( pid, THREAD_THAWED ); ) {
            __qnx_debug_xfer( ProcInfo.proc, pid, _DEBUG_WATCH_WR, WatchPoints,
                    WatchCount * sizeof( struct _watch_struct ), 0, 0 );
        }
        ret->conditions = RunIt( 0 );
    }
    if( (ret->conditions & COND_TERMINATE) == 0 ) {
        __qnx_debug_xfer( ProcInfo.proc, ProcInfo.pid, _DEBUG_REG_RD, &regs, sizeof( regs ), 0, 0 );
#if 0
        Out( "stopped at " );
        OutNum( regs.ip );
        Out( " because of " );
        OutNum( ret->conditions );
        Out( "\n" );
#endif
        ret->program_counter.offset = regs.ip;
        ret->program_counter.segment = regs.cs;
        ret->stack_pointer.offset = regs.sp;
        ret->stack_pointer.segment = regs.ss;
    }
    ret->conditions |= COND_CONFIG;
    return( sizeof( *ret ) );
}

trap_retval TRAP_CORE( Prog_step )( void )
{
    return( ProgRun( true ) );
}

trap_retval TRAP_CORE( Prog_go )( void )
{
    return( ProgRun( false ) );
}

#define STK_ALIGN( v, type )    ((v)+(sizeof(type)-1) & ~(sizeof(type)-1))

static unsigned_16 Redir32( bool input )
{
    struct  _reg_struct save, new;
    int                 *save_hdl;
    unsigned            len;
    redirect_stdin_ret  *ret;
    char                *file_name;
    dword               code;
    addr48_ptr          slib;

    ret = GetOutPtr( 0 );
    file_name = GetInPtr( sizeof( redirect_stdin_req ) );
    ret->err = 1;
    if( ProcInfo.pid == 0 ) {
        return( sizeof( *ret ) );
    }
    slib = GetSLibTable( true );
    if( slib.segment == 0 ) {
        return( sizeof( *ret ) );
    }
    if( __qnx_debug_xfer( ProcInfo.proc, ProcInfo.pid, _DEBUG_REG_RD,
                          &save, sizeof( save ), 0, 0 ) != 0 ) {
        return( sizeof( *ret ) );
    }
    new = save;
    if( input ) {
        new.ax = 0;
        save_hdl = &ProcInfo.save_in;
    } else {
        new.ax = 1;
        save_hdl = &ProcInfo.save_out;
    }
    ret->err = 0;
    if( *save_hdl != -1 ) {
        new.cs = slib.segment & ~PRIV_MASK;
        new.cs |= save.cs & PRIV_MASK;
        new.ip = slib.offset;
        new.dx = *save_hdl;
        new.sp -= 3*sizeof( dword );
        code = 256;
        __qnx_debug_xfer( ProcInfo.proc, ProcInfo.pid, _DEBUG_MEM_WR, &code,
            sizeof( code ), new.sp, new.ss );
        __qnx_debug_xfer( ProcInfo.proc, ProcInfo.pid, _DEBUG_REG_WR, &new,
            sizeof( new ), 0, 0 );
        __qnx_debug_cont( ProcInfo.proc, ProcInfo.pid, 0 );
        Receive( 0, 0, 0 );
        *save_hdl = -1;
    }
    if( file_name[0] != '\0' ) {
        new.cs = slib.segment & ~PRIV_MASK;
        new.cs |= save.cs & PRIV_MASK;
        new.ip = slib.offset;
        len = strlen( file_name ) + 1;
        new.sp -= STK_ALIGN( len, dword );
        new.dx = new.sp;
        __qnx_debug_xfer( ProcInfo.proc, ProcInfo.pid, _DEBUG_MEM_WR, file_name,
            len, new.sp, new.ss );
        new.sp -= 3*sizeof( dword );
        code = 257;
        __qnx_debug_xfer( ProcInfo.proc, ProcInfo.pid, _DEBUG_MEM_WR, &code,
            sizeof( code ), new.sp, new.ss );
        __qnx_debug_xfer( ProcInfo.proc, ProcInfo.pid, _DEBUG_REG_WR, &new,
            sizeof( new ), 0, 0 );
        __qnx_debug_cont( ProcInfo.proc, ProcInfo.pid, 0 );
        Receive( 0, 0, 0 );
        __qnx_debug_xfer( ProcInfo.proc, ProcInfo.pid, _DEBUG_REG_RD, &new,
            sizeof( new ), 0, 0 );
        if( (unsigned)new.ax == -1 )
            ret->err = 1;
        *save_hdl = new.ax;
    }
    __qnx_debug_xfer( ProcInfo.proc, ProcInfo.pid, _DEBUG_REG_WR, &save,
            sizeof( save ), 0, 0 );
    return( sizeof( *ret ) );
}


static unsigned Redir16( bool input )
{
    struct  _reg_struct save, new;
    addr48_ptr          slib;
    addr32_ptr          func;
    int                 *save_hdl;
    unsigned            len;
    redirect_stdin_ret  *ret;
    char                *file_name;

    ret = GetOutPtr( 0 );
    file_name = GetInPtr( sizeof( redirect_stdin_req ) );
    ret->err = 1;
    if( ProcInfo.pid == 0 ) {
        return( sizeof( *ret ) );
    }
    slib = GetSLibTable( false );
    if( slib.offset == 0 && slib.segment == 0 ) {
        return( sizeof( *ret ) );
    }
    if( __qnx_debug_xfer( ProcInfo.proc, ProcInfo.pid, _DEBUG_REG_RD,
                          &save, sizeof( save ), 0, 0 ) != 0 ) {
        return( sizeof( *ret ) );
    }
    new = save;
    if( input ) {
        new.ax = 0;
        save_hdl = &ProcInfo.save_in;
    } else {
        new.ax = 1;
        save_hdl = &ProcInfo.save_out;
    }
    ret->err = 0;
    if( *save_hdl != -1 ) {
        __qnx_debug_xfer( ProcInfo.proc, ProcInfo.pid, _DEBUG_MEM_RD, &func,
            sizeof( func ), slib.offset+4*70, slib.segment );
        new.cs = func.segment & ~PRIV_MASK;
        new.cs |= save.cs & PRIV_MASK;
        new.ip = func.offset;
        new.dx = *save_hdl;
        __qnx_debug_xfer( ProcInfo.proc, ProcInfo.pid, _DEBUG_REG_WR, &new,
            sizeof( new ), 0, 0 );
        __qnx_debug_cont( ProcInfo.proc, ProcInfo.pid, 0 );
        Receive( 0, 0, 0 );
        *save_hdl = -1;
    }
    if( file_name[0] != '\0' ) {
        __qnx_debug_xfer( ProcInfo.proc, ProcInfo.pid, _DEBUG_MEM_RD, &func,
            sizeof( func ), slib.offset+4*69, slib.segment );
        new.cs = func.segment & ~PRIV_MASK;
        new.cs |= save.cs & PRIV_MASK;
        new.ip = func.offset;
        len = strlen( file_name ) + 1;
        new.sp -= STK_ALIGN( len, word );
        new.bx = new.sp;
        new.cx = new.ss;
        __qnx_debug_xfer( ProcInfo.proc, ProcInfo.pid, _DEBUG_MEM_WR, file_name,
            len, new.sp, new.ss );
        __qnx_debug_xfer( ProcInfo.proc, ProcInfo.pid, _DEBUG_REG_WR, &new,
            sizeof( new ), 0, 0 );
        __qnx_debug_cont( ProcInfo.proc, ProcInfo.pid, 0 );
        Receive( 0, 0, 0 );
        __qnx_debug_xfer( ProcInfo.proc, ProcInfo.pid, _DEBUG_REG_RD, &new,
            sizeof( new ), 0, 0 );
        if( (unsigned)new.ax == -1 )
            ret->err = 1;
        *save_hdl = new.ax;
    }
    __qnx_debug_xfer( ProcInfo.proc, ProcInfo.pid, _DEBUG_REG_WR, &save,
            sizeof( save ), 0, 0 );
    return( sizeof( *ret ) );
}

trap_retval TRAP_CORE( Redirect_stdin )( void )
{
    if( ProcInfo.dbg32 ) {
        return( Redir32( true ) );
    } else {
        return( Redir16( true ) );
    }
}

trap_retval TRAP_CORE( Redirect_stdout )( void )
{
    if( ProcInfo.dbg32 ) {
        return( Redir32( false ) );
    } else {
        return( Redir16( false ) );
    }
}

trap_retval TRAP_FILE( file_to_fullpath )( void )
{
    struct _psinfo      proc;
    pid_t               pid;
    nid_t               nid;
    size_t              len;
    const char          *name;
    char                *fullname;
    file_string_to_fullpath_req *acc;
    file_string_to_fullpath_ret *ret;

/*     -- convert string in acc->name to full path (searchpath style)
 *        in acc->info (MAXIMUM len acc->info_len).
 */
    pid = 0;
    acc = GetInPtr( 0 );
    name = GetInPtr( sizeof( *acc ) );
    ret = GetOutPtr( 0 );
    ret->err = 0;
    fullname = GetOutPtr( sizeof( *ret ) );
    if( acc->file_type == DIG_FILETYPE_EXE ) {
        pid = RunningProc( &nid, name, &proc, &name );
    }
    if( pid != 0 ) {
        len = StrCopyDst( proc.un.proc.name, fullname ) - fullname;
    } else {
        len = FindFilePath( acc->file_type, name, fullname );
    }
    if( len == 0 ) {
        ret->err = ENOENT;      /* File not found */
    }
    return( sizeof( *ret ) + len + 1 );
}

trap_retval TRAP_CORE( Get_message_text )( void )
{
    static const char *const ExceptionMsgs[] = {
        "",
        TRP_QNX_hangup,
        TRP_QNX_user_interrupt,
        TRP_QNX_quit,
        TRP_EXC_illegal_instruction,
        TRP_QNX_trap,
        //"I/O trap",
        TRP_QNX_abort,
        TRP_QNX_emt,
        TRP_QNX_floating_point_error,
        TRP_QNX_process_killed,
        TRP_QNX_bus_error,
        TRP_EXC_access_violation,
        TRP_QNX_sys,
        TRP_QNX_broken_pipe,
        TRP_QNX_alarm,
        TRP_QNX_process_termination,
        TRP_QNX_user_signal_1,
        TRP_QNX_user_signal_2,
        TRP_QNX_child_stopped,
        TRP_QNX_power_fail,
        TRP_QNX_winch,
        TRP_QNX_urgent,
        TRP_QNX_poll,
        TRP_QNX_process_stopped,
        "",
        TRP_QNX_process_continued,
        TRP_QNX_device_ready,
        "",
        ""
    };
    get_message_text_ret        *ret;
    char                        *err_txt;

    ret = GetOutPtr( 0 );
    err_txt = GetOutPtr( sizeof(*ret) );
    if( ProcInfo.fork ) {
        ProcInfo.fork = false;
        strcpy( err_txt, TRP_QNX_PROC_FORK );
        ret->flags = MSG_NEWLINE | MSG_WARNING;
    } else {
        if( ProcInfo.sig == -1 ) {
            err_txt[0] = '\0';
        } else if( ProcInfo.sig > ( ( sizeof( ExceptionMsgs ) / sizeof( char * ) - 1 ) ) ) {
            strcpy( err_txt, TRP_EXC_unknown );
        } else {
            strcpy( err_txt, ExceptionMsgs[ProcInfo.sig] );
        }
        ProcInfo.sig = -1;
        ret->flags = MSG_NEWLINE | MSG_ERROR;
    }
    return( sizeof( *ret ) + strlen( err_txt ) + 1 );
}

trap_retval TRAP_CORE( Machine_data )( void )
{
    machine_data_req    *acc;
    machine_data_ret    *ret;
    machine_data_spec   *data;
    struct _seginfo     info;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    ret->cache_start = 0;
    ret->cache_end = ~(addr_off)0;
    if( acc->info_type == X86MD_ADDR_CHARACTERISTICS ) {
        data = GetOutPtr( sizeof( *ret ) );
        data->x86_addr_flags = 0;
        if( ProcInfo.proc32 ) {
            qnx_segment_info( ProcInfo.proc, ProcInfo.pid, acc->addr.segment, &info );
            if( info.flags & _PMF_DBBIT ) {
                data->x86_addr_flags = X86AC_BIG;
            }
        }
        return( sizeof( *ret ) + sizeof( data->x86_addr_flags ) );
    }
    return( sizeof( *ret ) );
}

trap_retval TRAP_CORE( Get_lib_name )( void )
{
    get_lib_name_req    *acc;
    get_lib_name_ret    *ret;
    char                *name;
    char                *p;
    size_t              name_maxlen;

#if 0
    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    switch( acc->mod_handle ) {
    case MH_NONE:
    case MH_DEBUGGEE:
        ret->mod_handle = MH_SLIB;
        if( ProcInfo.dbg32 ) {
            p = "/boot/sys/Slib32";
        } else {
            p = "/boot/sys/Slib16";
        }
        break;
    case MH_SLIB:
        ret->mod_handle = MH_PROC;
        if( ProcInfo.proc32 ) {
            p = "/boot/sys/Proc32";
        } else {
            p = "/boot/sys/Proc16";
        }
        break;
    default:
        ret->mod_handle = 0;
        return( sizeof( *ret ) );
    }
    name_maxlen = GetTotalSizeOut() - sizeof( *ret ) - 1;
    name = GetOutPtr( sizeof( *ret ) );
    strncpy( name, p, name_maxlen );
    name[name_maxlen] = '\0';
#else
    pid_t               pid, vid, proc;
    struct _psinfo      info;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    p = NULL;
    switch( acc->mod_handle ) {
    case MH_NONE:
    case MH_DEBUGGEE:
        ret->mod_handle = MH_SLIB;
        if( ProcInfo.dbg32 ) {
            if( ( pid = vid = qnx_name_locate(ProcInfo.nid, _SLIB_NAME, 0, 0 ) ) != -1 ) {
                qnx_psinfo( proc = PROC_PID, pid, &info, 0, 0 );
                if( info.flags & _PPF_VID ) {
                    pid = info.un.vproc.remote_pid;
                    proc = qnx_vc_attach( ProcInfo.nid, PROC_PID, 0, 0 );
                }
                if( proc != -1 ) {
                    if( qnx_psinfo( proc, pid, &info, 0, 0 ) == pid ) {
                        p = info.un.proc.name;
                        qnx_vc_detach( proc );
                    }
                }
                qnx_vc_detach( vid );
            }
            if( p == NULL ) {
                p = "sys/Slib32";
            }
        } else {
            p = "sys/Slib16";
        }
        break;
    case MH_SLIB:
        ret->mod_handle = MH_PROC;
        if( ( proc = qnx_vc_attach(ProcInfo.nid, PROC_PID, 0, 0 ) ) != -1 ) {
            qnx_psinfo( proc, PROC_PID, &info, 0, 0 );
            p = info.un.proc.name;
            qnx_vc_detach( proc );
        } else if( ProcInfo.proc32 ) {
            p = "sys/Proc32";
        } else {
            p = "sys/Proc16";
        }
        break;
    default:
        ret->mod_handle = 0;
        return( sizeof( *ret ) );
    }
    name = GetOutPtr( sizeof( *ret ) );
    *name = '\0';
    if( p != NULL ) {
        name_maxlen = GetTotalSizeOut() - sizeof( *ret ) - 1;
        if( p[0] == '/' ) {
            if( p[1] == '/' ) {
                for( p += 2; *p >= '0' && *p <= '9'; p++ ) {
                    {}
                }
            }
            strncpy( name, p, name_maxlen );
        } else {
            strncpy( name, "/boot/", name_maxlen );
            name[name_maxlen] = '\0';
            strncat( name, p, name_maxlen - strlen( name ) );
        }
        name[name_maxlen] = '\0';
    }
#endif
    return( sizeof( *ret ) + strlen( name ) + 1 );
}

trap_retval TRAP_THREAD( get_next )( void )
{
    thread_get_next_req *req;
    thread_get_next_ret *ret;
    pid_t               pid;

    req = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    if( pid = next_thread( req->thread, THREAD_ALL ) ) {
        ret->thread = pid;
        ret->state = find_thread( pid )->frozen ? THREAD_FROZEN : THREAD_THAWED;
    } else {
        ret->thread = 0;
    }
    return( sizeof( *ret ) );
}

trap_retval TRAP_THREAD( set )( void )
{
    thread_set_req      *req;
    thread_set_ret      *ret;
    pid_t               pid;
    thread_info         *thread;

    req = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    ret->err = 0;
    if( pid = req->thread ) {
        if( ( thread = find_thread( pid ) ) && !thread->fork ) {
            ret->old_thread = ProcInfo.pid;
            ProcInfo.pid = pid;
        } else {
            ret->err = EINVAL;
            ret->old_thread = ProcInfo.pid;
        }
    } else {
        ret->old_thread = ProcInfo.pid;
    }
    return( sizeof( *ret ) );
}

trap_retval TRAP_THREAD( freeze )( void )
{
    thread_freeze_req   *req;
    thread_freeze_ret   *ret;
    thread_info         *thread;

    req = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    ret->err = 0;
    if( thread = find_thread( req->thread ) ) {
        thread->frozen = true;
    } else {
        ret->err = EINVAL;
    }
    return( sizeof( *ret ) );
}

trap_retval TRAP_THREAD( thaw )( void )
{
    thread_thaw_req     *req;
    thread_thaw_ret     *ret;
    thread_info         *thread;

    req = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    ret->err = 0;
    if( thread = find_thread( req->thread ) ) {
        thread->frozen = false;
    } else {
        ret->err = EINVAL;
    }
    return( sizeof( *ret ) );
}

trap_retval TRAP_THREAD( get_extra )( void )
{
    thread_get_extra_req    *req;
    char                    *ret;
    struct _psinfo          info;
    thread_info             *thread;

    req = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    ret[0] = '\0';
    if( req->thread ) {
        if( thread = find_thread( req->thread ) ) {
            if( thread->fork ) {
                strcpy( ret, "Forked Process" );
            } else {
                if( qnx_psinfo( ProcInfo.proc, req->thread, &info, 0, 0 ) == req->thread ) {
                    strcpy( ret, info.un.proc.name );
                }
            }
        }
    } else {
        strcpy( ret, "Thread Name" );
    }
    return( strlen( ret ) + 1 );
}

trap_version TRAPENTRY TrapInit( const char *parms, char *err, bool remote )
{
    trap_version ver;

    /* unused parameters */ (void)remote;

    ForceFpu32 = 0;
    switch( *parms ) {
    case 'f':
        ForceFpu32 = -1;
        break;
    case 'F':
        ForceFpu32 = 1;
        break;
    }
    ProcInfo.save_in = -1;
    ProcInfo.save_out = -1;
    ProcInfo.thread = NULL;
    ProcInfo.max_threads = 0;
    find_thread( 0 );   /* allocate initial thread array */
    err[0] = '\0'; /* all ok */
    StdPos.in  = lseek( 0, 0, SEEK_CUR );
    StdPos.out = lseek( 1, 0, SEEK_CUR );
    StdPos.err = lseek( 2, 0, SEEK_CUR );
    ver.major = TRAP_VERSION_MAJOR;
    ver.minor = TRAP_VERSION_MINOR;
    ver.remote = false;
    //ver.is_32 = false;
    OrigPGrp = getpgrp();
    MID = qnx_proxy_attach( 0, 0, 0, 0 );
    if( MID == 0 ) {
        strcpy( err, TRP_QNX_no_proxy );
    }
    return( ver );
}

void TRAPENTRY TrapFini( void )
{
}
