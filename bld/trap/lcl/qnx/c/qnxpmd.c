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
* Description:  Trap file for QNX post-mortem debugging.
*
****************************************************************************/


#include <stddef.h>
#include <stdlib.h>
#include <unistd.h>
#include <process.h>
#include <signal.h>
#include <fcntl.h>
#include <errno.h>
#include <limits.h>
#include <string.h>
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
#include <sys/dumper.h>
#include <sys/console.h>
#include <sys/dev.h>
#include <sys/name.h>
#include "trpimp.h"
#include "qnxcomm.h"
#include "miscx87.h"
#include "mad.h"
#include "madregs.h"

typedef unsigned short  USHORT;
typedef unsigned long   ULONG;

typedef struct {
    word    real_seg;
    word    is_32;
    dword   seg_len;
    dword   file_off;
    dword   mem_off;
} seg_data;

#define DUMP_SIGNATURE  0x706d7564
#define DUMP_VERSION    100

#define NO_FILE         (-1)

enum {
    MH_NONE,
    MH_DEBUGGEE,
    MH_SLIB,
    MH_PROC
};

struct {
    unsigned            loaded              : 1;
    unsigned            ignore_timestamp    : 1;
    unsigned            fpu32               : 1;
    unsigned            read_gdts           : 1;
    unsigned            enable_read_gdts    : 1;
    unsigned            force_read_gdts     : 1;
    unsigned            dbg32               : 1;
    unsigned            mapping_shared      : 1;
    int                 fd;
    struct _dumper_hdr  hdr;
    seg_data            *segs;
} PmdInfo;

#if 0
static void Out( char *str )
{
    write( 1, str, strlen( str ) );
}

static void OutNum( unsigned i )
{
    char numbuff[10];
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


unsigned ReqGet_sys_config()
{
    struct  _osinfo     info;
    get_sys_config_ret  *ret;

    ret = GetOutPtr(0);
    if( PmdInfo.loaded )  {
        info = PmdInfo.hdr.osdata;
    } else {
        qnx_osinfo( 0, &info );
    }
    ret->sys.mad = MAD_X86;
    ret->sys.os = OS_QNX;
    ret->sys.osmajor = info.version / 100;
    ret->sys.osminor = info.version % 100;
    if( info.sflags & _PSF_EMULATOR_INSTALLED ) {
        ret->sys.fpu = X86_EMU;
    } else if( (info.sflags & _PSF_NDP_INSTALLED) == 0 ) {
        ret->sys.fpu = X86_NO;
    } else {
        switch( info.fpu ) {
        case 87:
            ret->sys.fpu = X86_87;
            break;
        case 287:
            ret->sys.fpu = X86_287;
            break;
        case 387:
        default:
            ret->sys.fpu = X86_387;
            break;
        }
    }
    switch( info.cpu ) {
    case 8088:
        ret->sys.cpu = X86_86;
        break;
    case 186:
        ret->sys.cpu = X86_186;
        break;
    case 286:
        ret->sys.cpu = X86_286;
        break;
    case 386:
    default:
        ret->sys.cpu = X86_386;
        break;
    case 486:
        ret->sys.cpu = X86_486;
        break;
    case 586:
        ret->sys.cpu = X86_586;
        break;
    case 686:
        ret->sys.cpu = X86_686;
        break;
    }
    if( info.sflags & _PSF_PROTECTED ) {
        ret->sys.huge_shift = 3;
    } else {
        ret->sys.huge_shift = 12;
    }
    return( sizeof( *ret ) );
}

#define PRIV_MASK 0x03

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

addr48_ptr GetSLibTable( bool is_32 )
{
    addr48_ptr          slib;
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
        pid = qnx_name_locate( getnid(), _SLIB_NAME, sizeof( msg ), NULL );
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
        #if 0 //NYI: Don't know how to find 16-bit Slib
        extern int (__far * (__far *__f)) ();

        slib.offset  = FP_OFF( __f );
        slib.segment = FP_SEG( __f );
        /*
            There are 3 different SLib segments, one for each priv level.
            Make the assumption that they're all contiguous and figure out
            the right one based on the difference between the debugger's
            priv level and the debuggee's. Ugh. Talk to QSSL about this.
        */
        slib.segment += ((PmdInfo.segs[0].real_seg & PRIV_MASK)
                        - (FP_SEG( &__f ) & PRIV_MASK)) * 8;
        #endif
    }
    return( slib );
}

unsigned ReqMap_addr()
{
    map_addr_req        *acc;
    map_addr_ret        *ret;
    unsigned            index;
    addr48_ptr          slib;
    unsigned            seg;

    acc = GetInPtr(0);
    ret = GetOutPtr(0);
    ret->lo_bound = 0;
    ret->hi_bound = ~(addr48_off)9;

    seg = acc->in_addr.segment;
    switch( seg ) {
    case MAP_FLAT_CODE_SELECTOR:
        seg = 0x04;
        break;
    case MAP_FLAT_DATA_SELECTOR:
        seg = 0x0c;
        break;
    }
    seg &= ~PRIV_MASK;

    ret->out_addr.offset = acc->in_addr.offset;
    switch( acc->handle ) {
    case MH_DEBUGGEE:
        if( acc->in_addr.segment == MAP_FLAT_DATA_SELECTOR ) {
            index = 0;
        } else {
            index = seg >> 3;
        }
        if( PmdInfo.loaded && index < PmdInfo.hdr.numsegs ) {
            seg = PmdInfo.segs[ seg >> 3 ].real_seg;
            ret->out_addr.offset += PmdInfo.segs[ index ].mem_off;
        }
        break;
    case MH_SLIB:
        slib = GetSLibTable( PmdInfo.dbg32 );
        seg += slib.segment - 4;
        break;
    case MH_PROC:
        seg += 0xE0 - 4;
        break;
    }
    ret->out_addr.segment = seg | (PmdInfo.segs[0].real_seg & PRIV_MASK);
    return( sizeof( *ret ) );
}


unsigned ReqChecksum_mem()
{
    checksum_mem_ret    *ret;

    ret = GetOutPtr(0);
    ret->result = 0;
    return( sizeof( *ret ) );
}

#pragma aux ver_read =  ".286p"         \
                        "verr   ax"     \
                        "jz     L1"     \
                        "xor    ax,ax"  \
                        "L1:"           \
                        parm [ax] value [ax]

extern unsigned short ver_read( unsigned short );

static unsigned ReadGDT( read_mem_req *acc, unsigned len, void *ret )
{
    struct _seginfo     info;
    unsigned            segment;

    if( !PmdInfo.read_gdts ) return( 0 );
    segment = acc->mem_addr.segment;
    if( segment & 0x04 ) return( 0 );
    if( segment == 0 ) return( 0 );
    if( !ver_read( segment ) ) return( 0 );
    if(qnx_segment_info(PROC_PID,PROC_PID,segment,&info)==-1) return( 0 );
    if( acc->mem_addr.offset >= info.nbytes ) {
        len = 0;
    } else if( acc->mem_addr.offset+len > info.nbytes ) {
        len = info.nbytes - acc->mem_addr.offset;
    }
    if( len == 0 ) return( 0 );
    _fmemcpy( ret, MK_FP( segment, acc->mem_addr.offset ), len );
    return( len );
}

unsigned ReqRead_mem()
{
    read_mem_req        *acc;
    void                *ret;
    unsigned            i;
    unsigned            len;

    acc = GetInPtr(0);
    ret = GetOutPtr(0);
    if( !PmdInfo.loaded ) {
        return( 0 );
    }
    len = acc->len;
    for( i = 0; i < PmdInfo.hdr.numsegs; ++i ) {
        if( PmdInfo.segs[i].real_seg == acc->mem_addr.segment ) {
            if( acc->mem_addr.offset >= PmdInfo.segs[i].seg_len ) {
                len = 0;
            } else if( acc->mem_addr.offset+len > PmdInfo.segs[i].seg_len ) {
                len = PmdInfo.segs[i].seg_len - acc->mem_addr.offset;
            }
            if( len != 0 ) {
                lseek( PmdInfo.fd, PmdInfo.segs[i].file_off + acc->mem_addr.offset,
                         SEEK_SET );
                len = read( PmdInfo.fd, ret, len );
                if( len == -1 ) len = 0;
            }
            return( len );
        }
    }
    return( ReadGDT( acc, len, ret ) );
}


unsigned ReqWrite_mem()
{
    write_mem_ret       *ret;

    ret = GetOutPtr(0);
    ret->len = 0;
    return( sizeof( *ret ) );
}


unsigned ReqRead_io()
{
    return( 0 );
}


unsigned ReqWrite_io()
{
    write_io_ret        *ret;

    ret = GetOutPtr(0);
    ret->len = 0;
    return( sizeof( *ret ) );
}

static void ReadCPU( struct x86_cpu *r )
{
    memset( r, 0, sizeof( *r ) );
    if( PmdInfo.loaded ) {
        r->eax = PmdInfo.hdr.reg.ax;
        r->ebx = PmdInfo.hdr.reg.bx;
        r->ecx = PmdInfo.hdr.reg.cx;
        r->edx = PmdInfo.hdr.reg.dx;
        r->esi = PmdInfo.hdr.reg.si;
        r->edi = PmdInfo.hdr.reg.di;
        r->ebp = PmdInfo.hdr.reg.bp;
        r->esp = PmdInfo.hdr.reg.sp;
        r->eip = PmdInfo.hdr.reg.ip;
        r->efl = PmdInfo.hdr.reg.fl;
        r->cs = PmdInfo.hdr.reg.cs;
        r->ds = PmdInfo.hdr.reg.ds;
        r->ss = PmdInfo.hdr.reg.ss;
        r->es = PmdInfo.hdr.reg.es;
        r->fs = PmdInfo.hdr.reg.fs;
        r->gs = PmdInfo.hdr.reg.gs;
    }
}

static void ReadFPU( struct x86_fpu *r )
{
    memset( r, 0, sizeof( *r ) );
    if( PmdInfo.loaded ) {
        memcpy( r, PmdInfo.hdr.x87, sizeof( PmdInfo.hdr.x87 ) );
        if( !PmdInfo.fpu32 ) FPUExpand( r );
    }
}

unsigned ReqRead_cpu()
{
    ReadCPU( GetOutPtr( 0 ) );
    return( sizeof( struct x86_cpu ) );
}

unsigned ReqRead_fpu()
{
    ReadFPU( GetOutPtr( 0 ) );
    return( sizeof( struct x86_fpu ) );
}

unsigned ReqRead_regs( void )
{
    mad_registers       *mr;

    mr = GetOutPtr( 0 );

    ReadCPU( &mr->x86.cpu );
    ReadFPU( &mr->x86.fpu );
    return( sizeof( mr->x86 ) );
}

unsigned ReqWrite_cpu()
{
    return( 0 );
}

unsigned ReqWrite_fpu()
{
    return( 0 );
}

unsigned ReqWrite_regs()
{
    return( 0 );
}

static bool LoadPmdHeader( char *name )
{
    struct stat     tmp;
    char            result[256];

    if( TryOnePath( ":/usr/dumps", &tmp, name, result ) == 0 ) return( FALSE );
    PmdInfo.fd = open( result, O_RDONLY );
    if( PmdInfo.fd < 0 ) return( FALSE );
    if( read( PmdInfo.fd, &PmdInfo.hdr, sizeof( PmdInfo.hdr ) )
            != sizeof( PmdInfo.hdr ) ) {
        close( PmdInfo.fd );
        PmdInfo.fd = NO_FILE;
        errno = ENOEXEC;
        return( FALSE );
    }
    if( PmdInfo.hdr.signature != DUMP_SIGNATURE
     || PmdInfo.hdr.version != DUMP_VERSION
     || PmdInfo.hdr.errnum != 0 ) {
        close( PmdInfo.fd );
        PmdInfo.fd = NO_FILE;
        errno = ENOEXEC;
        return( FALSE );
    }
    return( TRUE );
}

static void ReadSegData()
{
    int                 i;
    seg_data            *ptr;
    off_t               offset;
    struct _seginfo     seg_info;

    offset = sizeof( PmdInfo.hdr );
    for( ptr = PmdInfo.segs, i = PmdInfo.hdr.numsegs; i != 0; ++ptr, --i ) {
        if( lseek( PmdInfo.fd, offset, SEEK_SET ) != offset ) return;
        if( read( PmdInfo.fd, &seg_info, sizeof( seg_info ) )
                 != sizeof( seg_info ) ) return;
        ptr->is_32 = ((seg_info.flags & _PMF_DBBIT) != 0);
        ptr->file_off = offset + sizeof( seg_info );
        ptr->seg_len = seg_info.nbytes;
        ptr->mem_off = 0;
        if( PmdInfo.hdr.osdata.sflags & _PSF_PROTECTED ) {
            if( seg_info.selector & 0x8000 ) {
                /* flat model */
                ptr[0] = ptr[-1];
                ptr[-1].mem_off = seg_info.addr;
                ptr[ 0].mem_off = seg_info.addr + seg_info.nbytes;
                seg_info.nbytes = 0;
            }
            ptr->real_seg = seg_info.selector & 0x7fff;
        } else {
            ptr->real_seg = seg_info.addr >> 4;
        }
        seg_info.nbytes = (seg_info.nbytes+0xff) & ~0xffUL;
        offset = ptr->file_off + seg_info.nbytes;
    }
}

unsigned ReqProg_load()
{
    prog_load_req       *acc;
    prog_load_ret       *ret;
    char                *argv;
    struct _osinfo      info;

    PmdInfo.dbg32 = FALSE;
    PmdInfo.mapping_shared = FALSE;
    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    argv = GetInPtr( sizeof( *acc ) );
    ret->mod_handle = MH_DEBUGGEE;

    if( argv[0] == '\0' ) {
        ret->task_id = 0;
        ret->err = ENOENT;
        return( sizeof( *ret ) );
    }
    errno = 0;
    if( LoadPmdHeader( argv ) ) {
        PmdInfo.segs = malloc( sizeof( seg_data ) * PmdInfo.hdr.numsegs );
        if( PmdInfo.segs == NULL ) {
            errno = ENOMEM;
        } else {
            errno = 0;
            ReadSegData();
        }
    }
    ret->flags = LD_FLAG_IS_STARTED;
    if( PmdInfo.hdr.osdata.sflags & _PSF_PROTECTED ) {
        ret->flags |= LD_FLAG_IS_PROT;
    }
    if( PmdInfo.hdr.psdata.flags & _PPF_32BIT ) {
        ret->flags |= LD_FLAG_IS_32;
        PmdInfo.dbg32 = TRUE;
    }
    if( PmdInfo.hdr.psdata.flags & _PPF_32BIT ) {
        PmdInfo.fpu32 = TRUE;
    } else if( PmdInfo.hdr.osdata.sflags & _PSF_EMU16_INSTALLED ) {
        PmdInfo.fpu32 = FALSE;
    } else if( PmdInfo.hdr.osdata.sflags & _PSF_32BIT ) {
        PmdInfo.fpu32 = TRUE;
    } else {
        PmdInfo.fpu32 = FALSE;
    }
    qnx_osinfo( 0, &info );
    if( info.version == PmdInfo.hdr.osdata.version
     && info.release == PmdInfo.hdr.osdata.release
     && (info.sflags & _PSF_32BIT) == (PmdInfo.hdr.osdata.sflags & _PSF_32BIT) ) {
        PmdInfo.read_gdts = PmdInfo.enable_read_gdts;
    } else if( PmdInfo.force_read_gdts ) {
        PmdInfo.read_gdts = TRUE;
    } else {
        PmdInfo.read_gdts = FALSE;
    }
    ret->task_id = PmdInfo.hdr.psdata.pid;
    ret->err = errno;
    if( errno == 0 ) {
        PmdInfo.loaded = TRUE;
    } else {
        close( PmdInfo.fd );
        PmdInfo.fd = NO_FILE;
    }
    return( sizeof( *ret ) );
}

unsigned ReqProg_kill()
{
    prog_kill_ret       *ret;

    if( PmdInfo.loaded ) {
        PmdInfo.loaded = FALSE;
        free( PmdInfo.segs );
        close( PmdInfo.fd );
        PmdInfo.fd = NO_FILE;
    }
    PmdInfo.mapping_shared = FALSE;
    ret = GetOutPtr( 0 );
    ret->err = 0;
    return( sizeof( *ret ) );
}

unsigned ReqSet_break()
{
    set_break_ret       *ret;

    ret = GetOutPtr( 0 );
    ret->old = 0;
    return( sizeof( *ret ) );
}

unsigned ReqClear_break()
{
    return( 0 );
}

unsigned ReqSet_watch()
{
    set_watch_ret       *ret;

    ret = GetOutPtr( 0 );
    ret->err = 0;
    ret->multiplier = USING_DEBUG_REG | 1;
    return( sizeof( *ret ) );
}

unsigned ReqClear_watch()
{
    return( 0 );
}


unsigned ReqProg_go()
{
    prog_go_ret *ret;

    ret = GetOutPtr( 0 );
    ret->conditions = COND_TERMINATE;
    return( sizeof( *ret ) );
}

unsigned ReqProg_step()
{
    return( ReqProg_go() );
}


unsigned ReqGet_message_text()
{
    get_message_text_ret        *ret;
    char                        *err_txt;

    ret = GetOutPtr( 0 );
    err_txt = GetOutPtr( sizeof(*ret) );
    err_txt[0] = '\0';
    ret->flags = MSG_NEWLINE | MSG_ERROR;
    return( sizeof( *ret ) + 1 );
}

unsigned ReqRedirect_stdin()
{
    redirect_stdin_ret  *ret;

    ret = GetOutPtr( 0 );
    ret->err = 0;
    return( sizeof( *ret ) );
}

unsigned ReqRedirect_stdout()
{
    return( ReqRedirect_stdin() );
}

unsigned ReqFile_string_to_fullpath()
{
    struct  stat                chk;
    unsigned_16                 len;
    char                        *name;
    char                        *fullname;
    unsigned                    save_handle;
    file_string_to_fullpath_req *acc;
    file_string_to_fullpath_ret *ret;

    acc = GetInPtr( 0 );
    name = GetInPtr( sizeof( *acc ) );
    ret = GetOutPtr( 0 );
    fullname = GetOutPtr( sizeof( *ret ) );
    fullname[0] = '\0';
    len = 0;
    if( acc->file_type != TF_TYPE_EXE ) {
        len = FindFilePath( FALSE, name, fullname );
    } else if( PmdInfo.mapping_shared ) {
        len = FindFilePath( TRUE, name, fullname );
    } else {
        save_handle = PmdInfo.fd;
        if( LoadPmdHeader( name ) ) {
            name = PmdInfo.hdr.psdata.un.proc.name;
            if( stat( name, &chk ) != 0 ) {
                /* try it without the node number */
                name += 2;
                while( *name != '/' ) ++name;
                if( stat( name, &chk ) != 0 ) {
                    chk.st_mtime = 0;
                }
            }
            if( PmdInfo.ignore_timestamp || chk.st_mtime==PmdInfo.hdr.cmdtime ) {
                len = StrCopy( name, fullname ) - fullname;
            }
            close( PmdInfo.fd );
        }
        PmdInfo.fd = save_handle;
    }
    if( len == 0 ) {
        ret->err = ENOENT;      /* File not found */
    } else {
        ret->err = 0;
    }
    return( sizeof( *ret ) + len + 1 );
}

static bool AddrIs32( addr_seg seg )
{
    unsigned            index;
    bool                is_32;
    struct _seginfo     info;

    is_32 = FALSE;
    index = seg >> 3;
    if( (seg & 0x04) == 0 ) {
        if( PmdInfo.read_gdts
         && qnx_segment_info(PROC_PID,PROC_PID,seg,&info)!=-1 ) {
            if( info.flags & _PMF_DBBIT ) {
                is_32 = TRUE;
            }
         }
    } else if( PmdInfo.loaded && index < PmdInfo.hdr.numsegs ) {
        is_32 = PmdInfo.segs[ index ].is_32;
    }
    return( is_32 );
}

unsigned ReqAddr_info()
{
    addr_info_req       *acc;
    addr_info_ret       *ret;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    ret->is_32 = AddrIs32( acc->in_addr.segment );
    return( sizeof( *ret ) );
}

unsigned ReqMachine_data()
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
    if( AddrIs32( acc->addr.segment ) ) {
        *data |= X86AC_BIG;
    }
    return( sizeof( *ret ) + sizeof( *data ) );
}

unsigned ReqGet_lib_name()
{
    get_lib_name_req    *acc;
    get_lib_name_ret    *ret;
    char                *name;

    acc = GetInPtr(0);
    ret = GetOutPtr( 0 );
    name = GetOutPtr( sizeof( *ret ) );
    switch( acc->handle ) {
    case MH_NONE:
    case MH_DEBUGGEE:
        ret->handle = MH_SLIB;
        if( PmdInfo.dbg32 ) {
            strcpy( name, "/boot/sys/Slib32" );
        } else {
            strcpy( name, "/boot/sys/Slib16" );
        }
        break;
    case MH_SLIB:
        ret->handle = MH_PROC;
        if( PmdInfo.hdr.osdata.sflags & _PSF_32BIT ) {
            strcpy( name, "/boot/sys/Proc32" );
        } else {
            strcpy( name, "/boot/sys/Proc16" );
        }
        break;
    default:
        ret->handle = MH_NONE;
        name[0] = '\0';
        break;
    }
    if( PmdInfo.read_gdts ) {
        PmdInfo.mapping_shared = TRUE;
    } else {
        name[0] = '\0';
        ret->handle = MH_NONE;
    }
    return( sizeof( *ret ) + 1 + strlen( name ) );
}

unsigned ReqThread_get_next()
{
    thread_get_next_req *req;
    thread_get_next_ret *ret;

    req = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    if( req->thread == 0 ) {
        ret->thread = PmdInfo.hdr.psdata.pid;
        ret->state = THREAD_THAWED;
    } else {
        ret->thread = 0;
    }
    return( sizeof( *ret ) );
}

unsigned ReqThread_set()
{
    thread_set_ret      *ret;

    ret = GetOutPtr( 0 );
    ret->err = 0;
    ret->old_thread = PmdInfo.hdr.psdata.pid;
    return( sizeof( *ret ) );
}

unsigned ReqThread_freeze()
{
    thread_freeze_ret   *ret;

    ret = GetOutPtr( 0 );
    ret->err = 0;
    return( sizeof( *ret ) );
}

unsigned ReqThread_thaw()
{
    thread_thaw_ret     *ret;

    ret = GetOutPtr( 0 );
    ret->err = 0;
    return( sizeof( *ret ) );
}

unsigned ReqThread_get_extra()
{
    char                    *ret;

    ret = GetOutPtr( 0 );
    ret[0] = '\0';
    return( strlen( ret ) + 1 );
}

trap_version TRAPENTRY TrapInit( char *parm, char *err, bool remote )
{
    trap_version ver;

    remote = remote;
    PmdInfo.fd = NO_FILE;
    PmdInfo.enable_read_gdts = TRUE;
    PmdInfo.force_read_gdts  = FALSE;
    if( parm != NULL ) {
        while( *parm != '\0' ) {
            switch( *parm ) {
            case 'I':
            case 'i':
                PmdInfo.ignore_timestamp = TRUE;
                break;
            case 'G':
            case 'g':
                PmdInfo.force_read_gdts = TRUE;
                break;
            case 'd':
            case 'D':
                PmdInfo.enable_read_gdts = FALSE;
                break;
            }
            ++parm;
        }
    }
    err[0] = '\0'; /* all ok */
    ver.major = TRAP_MAJOR_VERSION;
    ver.minor = TRAP_MINOR_VERSION;
    ver.remote = FALSE;
    return( ver );
}

void TRAPENTRY TrapFini()
{
}
