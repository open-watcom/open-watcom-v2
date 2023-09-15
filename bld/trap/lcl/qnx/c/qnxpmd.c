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
* Description:  Trap file for QNX post-mortem debugging.
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
#include "digcpu.h"
#include "trpimp.h"
#include "trpcomm.h"
#include "qnxpath.h"
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


trap_retval TRAP_CORE( Get_sys_config )( void )
{
    struct  _osinfo     info;
    get_sys_config_ret  *ret;

    ret = GetOutPtr( 0 );
    if( PmdInfo.loaded )  {
        info = PmdInfo.hdr.osdata;
    } else {
        qnx_osinfo( 0, &info );
    }
    ret->arch = DIG_ARCH_X86;
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
    case 686:
        ret->cpu = X86_686;
        break;
    }
    if( info.sflags & _PSF_PROTECTED ) {
        ret->huge_shift = 3;
    } else {
        ret->huge_shift = 12;
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

static addr48_ptr GetSLibTable( bool is_32 )
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

        slib.offset  = _FP_OFF( __f );
        slib.segment = _FP_SEG( __f );
        /*
            There are 3 different SLib segments, one for each priv level.
            Make the assumption that they're all contiguous and figure out
            the right one based on the difference between the debugger's
            priv level and the debuggee's. Ugh. Talk to QSSL about this.
        */
        slib.segment += ((PmdInfo.segs[0].real_seg & PRIV_MASK)
                        - (_FP_SEG( &__f ) & PRIV_MASK)) * 8;
        #endif
    }
    return( slib );
}

trap_retval TRAP_CORE( Map_addr )( void )
{
    map_addr_req        *acc;
    map_addr_ret        *ret;
    unsigned            index;
    addr48_ptr          slib;
    unsigned            seg;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
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
    switch( acc->mod_handle ) {
    case MH_DEBUGGEE:
        if( acc->in_addr.segment == MAP_FLAT_DATA_SELECTOR ) {
            index = 0;
        } else {
            index = seg >> 3;
        }
        if( PmdInfo.loaded && index < PmdInfo.hdr.numsegs ) {
            seg = PmdInfo.segs[seg >> 3].real_seg;
            ret->out_addr.offset += PmdInfo.segs[index].mem_off;
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


trap_retval TRAP_CORE( Checksum_mem )( void )
{
    checksum_mem_ret    *ret;

    ret = GetOutPtr( 0 );
    ret->result = 0;
    return( sizeof( *ret ) );
}

extern unsigned short ver_read( unsigned short );
#pragma aux ver_read = \
        ".386p"         \
        "verr ax"       \
        "jz short L1"   \
        "xor  ax,ax"    \
    "L1:"               \
    __parm  [__ax] \
    __value [__ax]

static unsigned ReadGDT( read_mem_req *acc, unsigned len, void *ret )
{
    struct _seginfo     info;
    unsigned            segment;

    if( !PmdInfo.read_gdts )
        return( 0 );
    segment = acc->mem_addr.segment;
    if( segment & 0x04 )
        return( 0 );
    if( segment == 0 )
        return( 0 );
    if( !ver_read( segment ) )
        return( 0 );
    if( qnx_segment_info( PROC_PID, PROC_PID, segment, &info ) == -1 )
        return( 0 );
    if( acc->mem_addr.offset >= info.nbytes ) {
        len = 0;
    } else if( acc->mem_addr.offset+len > info.nbytes ) {
        len = info.nbytes - acc->mem_addr.offset;
    }
    if( len == 0 )
        return( 0 );
    _fmemcpy( ret, _MK_FP( segment, acc->mem_addr.offset ), len );
    return( len );
}

trap_retval TRAP_CORE( Read_mem )( void )
{
    read_mem_req        *acc;
    void                *ret;
    unsigned            i;
    unsigned            len;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
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
                if( len == -1 ) {
                    len = 0;
                }
            }
            return( len );
        }
    }
    return( ReadGDT( acc, len, ret ) );
}


trap_retval TRAP_CORE( Write_mem )( void )
{
    write_mem_ret       *ret;

    ret = GetOutPtr( 0 );
    ret->len = 0;
    return( sizeof( *ret ) );
}


trap_retval TRAP_CORE( Read_io )( void )
{
    return( 0 );
}


trap_retval TRAP_CORE( Write_io )( void )
{
    write_io_ret        *ret;

    ret = GetOutPtr( 0 );
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
        if( !PmdInfo.fpu32 ) {
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

trap_retval TRAP_CORE( Write_regs )( void )
{
    return( 0 );
}

static bool LoadPmdHeader( char *name )
{
    struct stat     tmp;
    char            result[PATH_MAX + 1];

    strcpy( result, name );
    if( stat( result, &tmp ) != 0 ) {
        #define DUMPSDIR    "/usr/dumps/"
        strcpy( result, DUMPSDIR );
        strcpy( result + sizeof( DUMPSDIR ) - 1, name );
        #undef DUMPSDIR
        if( stat( result, &tmp ) != 0 ) {
            return( false );
        }
    }
    PmdInfo.fd = open( result, O_RDONLY );
    if( PmdInfo.fd < 0 )
        return( false );
    if( read( PmdInfo.fd, &PmdInfo.hdr, sizeof( PmdInfo.hdr ) ) != sizeof( PmdInfo.hdr ) ) {
        close( PmdInfo.fd );
        PmdInfo.fd = NO_FILE;
        errno = ENOEXEC;
        return( false );
    }
    if( PmdInfo.hdr.signature != DUMP_SIGNATURE
      || PmdInfo.hdr.version != DUMP_VERSION
      || PmdInfo.hdr.errnum != 0 ) {
        close( PmdInfo.fd );
        PmdInfo.fd = NO_FILE;
        errno = ENOEXEC;
        return( false );
    }
    return( true );
}

static void ReadSegData( void )
{
    int                 i;
    seg_data            *ptr;
    off_t               offset;
    struct _seginfo     seg_info;

    offset = sizeof( PmdInfo.hdr );
    for( ptr = PmdInfo.segs, i = PmdInfo.hdr.numsegs; i != 0; ++ptr, --i ) {
        if( lseek( PmdInfo.fd, offset, SEEK_SET ) != offset )
            return;
        if( read( PmdInfo.fd, &seg_info, sizeof( seg_info ) ) != sizeof( seg_info ) )
            return;
        ptr->is_32 = ((seg_info.flags & _PMF_DBBIT) != 0);
        ptr->file_off = offset + sizeof( seg_info );
        ptr->seg_len = seg_info.nbytes;
        ptr->mem_off = 0;
        if( PmdInfo.hdr.osdata.sflags & _PSF_PROTECTED ) {
            if( seg_info.selector & 0x8000 ) {
                /* flat model */
                ptr[0] = ptr[-1];
                ptr[-1].mem_off = seg_info.addr;
                ptr[0].mem_off = seg_info.addr + seg_info.nbytes;
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

trap_retval TRAP_CORE( Prog_load )( void )
{
    prog_load_req       *acc;
    prog_load_ret       *ret;
    char                *argv;
    struct _osinfo      info;

    PmdInfo.dbg32 = false;
    PmdInfo.mapping_shared = false;
    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    ret->err = 0;
    ret->mod_handle = MH_DEBUGGEE;
    argv = GetInPtr( sizeof( *acc ) );
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
        ret->flags |= LD_FLAG_IS_BIG;
        PmdInfo.dbg32 = true;
    }
    if( PmdInfo.hdr.psdata.flags & _PPF_32BIT ) {
        PmdInfo.fpu32 = true;
    } else if( PmdInfo.hdr.osdata.sflags & _PSF_EMU16_INSTALLED ) {
        PmdInfo.fpu32 = false;
    } else if( PmdInfo.hdr.osdata.sflags & _PSF_32BIT ) {
        PmdInfo.fpu32 = true;
    } else {
        PmdInfo.fpu32 = false;
    }
    qnx_osinfo( 0, &info );
    if( info.version == PmdInfo.hdr.osdata.version
     && info.release == PmdInfo.hdr.osdata.release
     && (info.sflags & _PSF_32BIT) == (PmdInfo.hdr.osdata.sflags & _PSF_32BIT) ) {
        PmdInfo.read_gdts = PmdInfo.enable_read_gdts;
    } else if( PmdInfo.force_read_gdts ) {
        PmdInfo.read_gdts = true;
    } else {
        PmdInfo.read_gdts = false;
    }
    ret->task_id = PmdInfo.hdr.psdata.pid;
    ret->err = errno;
    if( errno == 0 ) {
        PmdInfo.loaded = true;
    } else {
        close( PmdInfo.fd );
        PmdInfo.fd = NO_FILE;
    }
    return( sizeof( *ret ) );
}

trap_retval TRAP_CORE( Prog_kill )( void )
{
    prog_kill_ret       *ret;

    if( PmdInfo.loaded ) {
        PmdInfo.loaded = false;
        free( PmdInfo.segs );
        close( PmdInfo.fd );
        PmdInfo.fd = NO_FILE;
    }
    PmdInfo.mapping_shared = false;
    ret = GetOutPtr( 0 );
    ret->err = 0;
    return( sizeof( *ret ) );
}

trap_retval TRAP_CORE( Set_break )( void )
{
    set_break_ret       *ret;

    ret = GetOutPtr( 0 );
    ret->old = 0;
    return( sizeof( *ret ) );
}

trap_retval TRAP_CORE( Clear_break )( void )
{
    return( 0 );
}

trap_retval TRAP_CORE( Set_watch )( void )
{
    set_watch_ret       *ret;

    ret = GetOutPtr( 0 );
    ret->err = 0;   // OK
    ret->multiplier = USING_DEBUG_REG | 1;
    return( sizeof( *ret ) );
}

trap_retval TRAP_CORE( Clear_watch )( void )
{
    return( 0 );
}


trap_retval TRAP_CORE( Prog_go )( void )
{
    prog_go_ret *ret;

    ret = GetOutPtr( 0 );
    ret->conditions = COND_TERMINATE;
    return( sizeof( *ret ) );
}

trap_retval TRAP_CORE( Prog_step )( void )
{
    return( TRAP_CORE( Prog_go )() );
}


trap_retval TRAP_CORE( Get_message_text )( void )
{
    get_message_text_ret        *ret;
    char                        *err_txt;

    ret = GetOutPtr( 0 );
    err_txt = GetOutPtr( sizeof(*ret) );
    err_txt[0] = '\0';
    ret->flags = MSG_NEWLINE | MSG_ERROR;
    return( sizeof( *ret ) + 1 );
}

trap_retval TRAP_CORE( Redirect_stdin )( void )
{
    redirect_stdin_ret  *ret;

    ret = GetOutPtr( 0 );
    ret->err = 0;
    return( sizeof( *ret ) );
}

trap_retval TRAP_CORE( Redirect_stdout )( void )
{
    return( TRAP_CORE( Redirect_stdin )() );
}

trap_retval TRAP_FILE( file_to_fullpath )( void )
{
    struct  stat                chk;
    size_t                      len;
    char                        *name;
    char                        *fullname;
    unsigned                    save_handle;
    file_string_to_fullpath_req *acc;
    file_string_to_fullpath_ret *ret;

    acc = GetInPtr( 0 );
    name = GetInPtr( sizeof( *acc ) );
    ret = GetOutPtr( 0 );
    ret->err = 0;
    fullname = GetOutPtr( sizeof( *ret ) );
    fullname[0] = '\0';
    len = 0;
    if( acc->file_type != DIG_FILETYPE_EXE || PmdInfo.mapping_shared ) {
        len = FindFilePath( acc->file_type, name, fullname );
    } else {
        save_handle = PmdInfo.fd;
        if( LoadPmdHeader( name ) ) {
            name = PmdInfo.hdr.psdata.un.proc.name;
            if( stat( name, &chk ) != 0 ) {
                /* try it without the node number */
                name += 2;
                while( *name != '/' )
                    ++name;
                if( stat( name, &chk ) != 0 ) {
                    chk.st_mtime = 0;
                }
            }
            if( PmdInfo.ignore_timestamp || chk.st_mtime == PmdInfo.hdr.cmdtime ) {
                len = StrCopyDst( name, fullname ) - fullname;
            }
            close( PmdInfo.fd );
        }
        PmdInfo.fd = save_handle;
    }
    if( len == 0 )
        ret->err = ENOENT;      /* File not found */
    return( sizeof( *ret ) + len + 1 );
}

static bool AddrIs32( addr_seg seg )
{
    unsigned            index;
    bool                is_32;
    struct _seginfo     info;

    is_32 = false;
    index = seg >> 3;
    if( (seg & 0x04) == 0 ) {
        if( PmdInfo.read_gdts
         && qnx_segment_info(PROC_PID,PROC_PID,seg,&info)!=-1 ) {
            if( info.flags & _PMF_DBBIT ) {
                is_32 = true;
            }
         }
    } else if( PmdInfo.loaded && index < PmdInfo.hdr.numsegs ) {
        is_32 = PmdInfo.segs[index].is_32;
    }
    return( is_32 );
}

trap_retval TRAP_CORE( Machine_data )( void )
{
    machine_data_req    *acc;
    machine_data_ret    *ret;
    machine_data_spec   *data;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    ret->cache_start = 0;
    ret->cache_end = ~(addr_off)0;
    if( acc->info_type == X86MD_ADDR_CHARACTERISTICS ) {
        data = GetOutPtr( sizeof( *ret ) );
        data->x86_addr_flags = ( AddrIs32( acc->addr.segment ) ) ? X86AC_BIG : 0;
        return( sizeof( *ret ) + sizeof( data->x86_addr_flags ) );
    }
    return( sizeof( *ret ) );
}

trap_retval TRAP_CORE( Get_lib_name )( void )
{
    get_lib_name_req    *acc;
    get_lib_name_ret    *ret;
    char                *name;
    const char          *p;
    size_t              name_maxlen;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    ret->mod_handle = 0;
    if( PmdInfo.read_gdts == 0 )
        return( sizeof( *ret ) );
    switch( acc->mod_handle ) {
    case MH_NONE:
    case MH_DEBUGGEE:
        ret->mod_handle = MH_SLIB;
        if( PmdInfo.dbg32 ) {
            p = "/boot/sys/Slib32";
        } else {
            p = "/boot/sys/Slib16";
        }
        break;
    case MH_SLIB:
        ret->mod_handle = MH_PROC;
        if( PmdInfo.hdr.osdata.sflags & _PSF_32BIT ) {
            p = "/boot/sys/Proc32";
        } else {
            p = "/boot/sys/Proc16";
        }
        break;
    default:
        return( sizeof( *ret ) );
    }
    name_maxlen = GetTotalSizeOut() - sizeof( *ret ) - 1;
    name = GetOutPtr( sizeof( *ret ) );
    strncpy( name, p, name_maxlen );
    name[name_maxlen] = '\0';
    PmdInfo.mapping_shared = true;
    return( sizeof( *ret ) + strlen( name ) + 1 );
}

trap_retval TRAP_THREAD( get_next )( void )
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

trap_retval TRAP_THREAD( set )( void )
{
    thread_set_ret      *ret;

    ret = GetOutPtr( 0 );
    ret->err = 0;
    ret->old_thread = PmdInfo.hdr.psdata.pid;
    return( sizeof( *ret ) );
}

trap_retval TRAP_THREAD( freeze )( void )
{
    thread_freeze_ret   *ret;

    ret = GetOutPtr( 0 );
    ret->err = 0;
    return( sizeof( *ret ) );
}

trap_retval TRAP_THREAD( thaw )( void )
{
    thread_thaw_ret     *ret;

    ret = GetOutPtr( 0 );
    ret->err = 0;
    return( sizeof( *ret ) );
}

trap_retval TRAP_THREAD( get_extra )( void )
{
    char                    *ret;

    ret = GetOutPtr( 0 );
    ret[0] = '\0';
    return( strlen( ret ) + 1 );
}

trap_version TRAPENTRY TrapInit( const char *parms, char *err, bool remote )
{
    trap_version    ver;
    char            ch;

    /* unused parameters */ (void)remote;

    PmdInfo.fd = NO_FILE;
    PmdInfo.enable_read_gdts = true;
    PmdInfo.force_read_gdts  = false;
    while( (ch = *parms++) != '\0' ) {
        switch( ch ) {
        case 'I':
        case 'i':
            PmdInfo.ignore_timestamp = true;
            break;
        case 'G':
        case 'g':
            PmdInfo.force_read_gdts = true;
            break;
        case 'd':
        case 'D':
            PmdInfo.enable_read_gdts = false;
            break;
        }
    }
    err[0] = '\0'; /* all ok */
    ver.major = TRAP_VERSION_MAJOR;
    ver.minor = TRAP_VERSION_MINOR;
    ver.remote = false;
    return( ver );
}

void TRAPENTRY TrapFini( void )
{
}
