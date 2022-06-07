/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2015-2022 The Open Watcom Contributors. All Rights Reserved.
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

//#define DEBUG_TRAP
#include "trapdbg.h"
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <io.h>
#include <dos.h>
#include <fcntl.h>
#include "trpimp.h"
#include "trpcomm.h"
#include "trperr.h"
#include "doserr.h"
#include "wdebug.h"
#include "trapdbg.h"

#include "rsi1632.h"
#include "dbg386.h"
#include "drset.h"
#include "ioports.h"
#include "madregs.h"

#include "exedos.h"
#include "exeos2.h"
#include "exeflat.h"

#include "x86cpu.h"
#include "miscx87.h"
#include "dosredir.h"
#include "doscomm.h"
#include "cpuglob.h"

#define INT_PRT_SCRN_KEY    0x05

#define IsDPMI          (_d16info.swmode == 0)

typedef struct watch_point {
    addr48_ptr          addr;
    dword               value;
    dword               linear;
    word                dregs;
    word                len;
    long                handle;
    long                handle2;
} watch_point;

static struct {
    unsigned_32         size;
    unsigned_32         start;
} *ObjInfo;

static TSF32            Proc;
static opcode_type      Break;
static bool             FakeBreak;
static bool             AtEnd;
static unsigned         NumObjects;
static unsigned_8       RealNPXType;

static char             UtilBuff[BUFF_SIZE];

#define MAX_WATCHES     32

static watch_point      WatchPoints[MAX_WATCHES];
static int              WatchCount;

static void EMURestore( _word seg, const void __far *data )
{
    EMUSaveRestore( seg, (void __far *)data, 0 );
}

static void EMUSave( _word seg, void __far *data )
{
    EMUSaveRestore( seg, data, 1 );
}

int SetUsrTask( void )
{
    return( 1 );
}

void SetDbgTask( void )
{
}

static unsigned short ReadMemory( addr48_ptr *addr, void FarPtr data, unsigned short req_len )
{
    unsigned short  len;

    _DBG_Write( "checking " );
    _DBG_Write16( addr->segment );
    _DBG_Write( ":" );
    _DBG_Write32( addr->offset );
    _DBG_Write( " for 0x" );
    _DBG_Write16( req_len );
    _DBG_Write( " bytes -- " );
    if( rsi_addr32_check( addr->offset, addr->segment, req_len, NULL ) == MEMBLK_VALID ) {
        if( D32DebugRead( addr->offset, addr->segment, 0, data, req_len ) == 0 ) {
            _DBG_Writeln( "OK" );
            addr->offset += req_len;
            return( req_len );
        }
    }
    _DBG_Writeln(( "Bad" ));
    len = 0;
    while( req_len > 0 ) {
        if( rsi_addr32_check( addr->offset, addr->segment, 1, NULL ) != MEMBLK_VALID )
            break;
        if( D32DebugRead( addr->offset, addr->segment, 0, data, 1 ) != 0 )
            break;
        ++addr->offset;
        data = (char FarPtr)data + 1;
        ++len;
        --req_len;
    }
    return( len );
}

static unsigned short WriteMemory( addr48_ptr *addr, const void FarPtr data, unsigned short req_len )
{
    unsigned short  len;

    _DBG_Write( "checking " );
    _DBG_Write16( addr->segment );
    _DBG_Write( ":" );
    _DBG_Write32( addr->offset );
    _DBG_Write( " for 0x" );
    _DBG_Write16( req_len );
    _DBG_Write( " bytes -- " );
    if( rsi_addr32_check( addr->offset, addr->segment, req_len, NULL ) == MEMBLK_VALID ) {
        if( D32DebugWrite( addr->offset, addr->segment, 0, data, req_len ) == 0 ) {
            _DBG_Writeln( "OK" );
            addr->offset += req_len;
            return( req_len );
        }
    }
    _DBG_Writeln(( "Bad" ));
    len = 0;
    while( req_len > 0 ) {
        if( rsi_addr32_check( addr->offset, addr->segment, 1, NULL ) != MEMBLK_VALID )
            break;
        if( D32DebugWrite( addr->offset, addr->segment, 0, data, 1 ) != 0 )
            break;
        ++addr->offset;
        data = (char FarPtr)data + 1;
        ++len;
        --req_len;
    }
    return( len );
}


trap_retval TRAP_CORE( Get_sys_config )( void )
{
    get_sys_config_ret  *ret;

    _DBG_Writeln( "AccGetConfig" );

    ret = GetOutPtr( 0 );
    ret->sys.os = DIG_OS_RATIONAL;
    ret->sys.osmajor = _osmajor;
    ret->sys.osminor = _osminor;
    ret->sys.cpu = X86CPUType();
    ret->sys.huge_shift = 12;
    if( !AtEnd && HAVE_EMU ) {
        ret->sys.fpu = X86_EMU;
    } else {
        ret->sys.fpu = RealNPXType;
    }
    ret->sys.arch = DIG_ARCH_X86;
    return( sizeof( *ret ) );
}


trap_retval TRAP_CORE( Map_addr )( void )
{
    Fptr32              fp;
    map_addr_req        *acc;
    map_addr_ret        *ret;
    unsigned            i;

    _DBG_Writeln( "AccMapAddr" );

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    ret->lo_bound = 0;
    ret->hi_bound = ~(addr48_off)0;
    fp.off = acc->in_addr.offset;
    fp.sel = acc->in_addr.segment;
    switch( fp.sel ) {
    case MAP_FLAT_CODE_SELECTOR:
    case MAP_FLAT_DATA_SELECTOR:
        fp.sel = 1;
        fp.off += ObjInfo[0].start;
        for( i = 0; i < NumObjects; ++i ) {
            if( ObjInfo[i].start <= fp.off && ( ObjInfo[i].start + ObjInfo[i].size ) > fp.off ) {
                fp.sel = i + 1;
                fp.off -= ObjInfo[i].start;
                ret->lo_bound = ObjInfo[i].start - ObjInfo[0].start;
                ret->hi_bound = ret->lo_bound + ObjInfo[i].size - 1;
                break;
            }
        }
        break;
    }
    D32Relocate(&fp);
    ret->out_addr.segment = fp.sel;
    ret->out_addr.offset = fp.off;
    return( sizeof( *ret ) );
}

extern unsigned long GetLAR( unsigned );
#pragma aux GetLAR = \
        ".386p"         \
        "xor  edx,edx"  \
        "mov  dx,ax"    \
        "lar  eax,edx"  \
        "mov  edx,eax"  \
        "shr  edx,16"   \
    __parm __caller [__ax] \
    __value         [__dx __ax]

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
        data->x86_addr_flags = 0;
        if( rsi_addr32_check( 0, acc->addr.segment, 1, NULL ) == MEMBLK_VALID ) {
            if( GetLAR( acc->addr.segment ) & 0x400000 ) {
                data->x86_addr_flags = X86AC_BIG;
            }
        }
        return( sizeof( *ret ) + sizeof( data->x86_addr_flags ) );
    }
    return( sizeof( *ret ) );
}

trap_retval TRAP_CORE( Checksum_mem )( void )
{
    unsigned short      len;
    int                 i;
    unsigned short      read;
    checksum_mem_req    *acc;
    checksum_mem_ret    *ret;
    unsigned short      buff_len;

    _DBG_Writeln( "AccChkSum" );

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );

    ret->result = 0;
    buff_len = BUFF_SIZE;
    for( len = acc->len; len > 0; len -= buff_len ) {
        if( buff_len > len )
            buff_len = len;
        read = ReadMemory( (addr48_ptr *)&acc->in_addr, &UtilBuff, buff_len );
        for( i = 0; i < read; ++i ) {
            ret->result += UtilBuff[i];
        }
        if( read != buff_len ) {
            break;
        }
    }
    return( sizeof( ret ) );
}


trap_retval TRAP_CORE( Read_mem )( void )
{
    read_mem_req        *acc;
    void                FarPtr buff;

    _DBG_Writeln( "ReadMem" );
    acc = GetInPtr( 0 );
    buff = GetOutPtr( 0 );
    return( ReadMemory( (addr48_ptr *)&acc->mem_addr, buff, acc->len ) );
}

trap_retval TRAP_CORE( Write_mem )( void )
{
    write_mem_req       *acc;
    write_mem_ret       *ret;

    _DBG_Writeln( "WriteMem" );
    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    ret->len = WriteMemory( (addr48_ptr *)&acc->mem_addr, GetInPtr( sizeof( *acc ) ), GetTotalSizeIn() - sizeof( *acc ) );
    return( sizeof( *ret ) );
}

trap_retval TRAP_CORE( Read_io )( void )
{
    read_io_req         *acc;
    void                *data;

    acc = GetInPtr( 0 );
    data = GetOutPtr( 0 );
    if( acc->len == 1 ) {
        *(byte *)data = In_b( acc->IO_offset );
    } else if( acc->len == 2 ) {
        *(word *)data = In_w( acc->IO_offset );
    } else {
        *(dword *)data = In_d( acc->IO_offset );
    }
    return( acc->len );
}

trap_retval TRAP_CORE( Write_io )( void )
{
    trap_elen           len;
    write_io_req        *acc;
    write_io_ret        *ret;
    void                *data;

    acc = GetInPtr( 0 );
    data = GetInPtr( sizeof( *acc ) );
    len = GetTotalSizeIn() - sizeof( *acc );
    ret = GetOutPtr( 0 );
    if( len == 1 ) {
        Out_b( acc->IO_offset, *(byte *)data );
    } else if( len == 2 ) {
        Out_w( acc->IO_offset, *(word *)data );
    } else {
        Out_d( acc->IO_offset, *(dword *)data );
    }
    ret->len = len;
    return( sizeof( *ret ) );
}

static void ReadCPU( struct x86_cpu *r )
{
    r->efl = Proc.eflags;
    r->eax = Proc.eax;
    r->ebx = Proc.ebx;
    r->ecx = Proc.ecx;
    r->edx = Proc.edx;
    r->esi = Proc.esi;
    r->edi = Proc.edi;
    r->esp = Proc.esp;
    r->ebp = Proc.ebp;
    r->eip = Proc.eip;
    r->ds = Proc.ds;
    r->cs = Proc.cs;
    r->es = Proc.es;
    r->ss = Proc.ss;
    r->fs = Proc.fs;
    r->gs = Proc.gs;
}

static void WriteCPU( const struct x86_cpu *r )
{
    Proc.eflags = r->efl;
    Proc.eax = r->eax;
    Proc.ebx = r->ebx;
    Proc.ecx = r->ecx;
    Proc.edx = r->edx;
    Proc.esi = r->esi;
    Proc.edi = r->edi;
    Proc.esp = r->esp;
    Proc.ebp = r->ebp;
    Proc.eip = r->eip;
    Proc.ds = r->ds;
    Proc.cs = r->cs;
    Proc.es = r->es;
    Proc.ss = r->ss;
    Proc.fs = r->fs;
    Proc.gs = r->gs;
}

static void ReadFPU( struct x86_fpu *r )
{
    if( HAVE_EMU ) {
        if( CheckWin386Debug() == WGOD_VERSION ) {
            EMUSave( Proc.cs, r );
        } else {
            Read387( r );
        }
    } else if( RealNPXType != X86_NO ) {
        if( _d16info.cpumod >= 3 ) {
            Read387( r );
        } else {
            Read8087( r );
        }
    }
}

static void WriteFPU( const struct x86_fpu *r )
{
    if( HAVE_EMU ) {
        if( CheckWin386Debug() == WGOD_VERSION ) {
            EMURestore( Proc.cs, r );
        } else {
            Write387( r );
        }
    } else if( RealNPXType != X86_NO ) {
        if( _d16info.cpumod >= 3 ) {
            Write387( r );
        } else {
            Write8087( r );
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
    const mad_registers *mr;

    mr = GetInPtr( sizeof( write_regs_req ) );
    WriteCPU( &mr->x86.cpu );
    WriteFPU( &mr->x86.u.fpu );
    return( 0 );
}

static void GetObjectInfo( char *name )
{
    int                 handle;
    unsigned_32         off;
    unsigned            i;
    object_record       obj;
    union {
        dos_exe_header  dos;
        os2_flat_header os2;
    }   head;

    handle = open( name, O_BINARY | O_RDONLY, 0 );
    if( handle == -1 )
        return;
    read( handle, &head.dos, sizeof( head.dos ) );
    if( head.dos.signature != DOS_SIGNATURE ) {
        close( handle );
        return;
    }
    lseek( handle, OS2_NE_OFFSET, SEEK_SET );
    read( handle, &off, sizeof( off ) );
    lseek( handle, off, SEEK_SET );
    read( handle, &head.os2, sizeof( head.os2 ) );
    switch( head.os2.signature ) {
    case OSF_FLAT_SIGNATURE:
    case OSF_FLAT_LX_SIGNATURE:
        lseek( handle, head.os2.objtab_off + off, SEEK_SET );
        NumObjects = head.os2.num_objects;
        ObjInfo = realloc( ObjInfo, NumObjects * sizeof( *ObjInfo ) );
        for( i = 0; i < head.os2.num_objects; ++i ) {
            read( handle, &obj, sizeof( obj ) );
            ObjInfo[i].size = obj.size;
            ObjInfo[i].start = obj.addr;
        }
        break;
    default:
        NumObjects = 0;
        break;
    }
    close( handle );
}

static size_t MergeArgvArray( const char *src, char *dst, size_t len )
/********************************************************************/
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
{
    char            *src;
    char            *name;
    char            ch;
    prog_load_ret   *ret;
    unsigned        len;

    _DBG_Writeln( "AccLoadProg" );
    AtEnd = false;
    src = name = GetInPtr( sizeof( prog_load_req ) );
    ret = GetOutPtr( 0 );
    while( *src++ != '\0' )
        {}
    len = GetTotalSizeIn() - sizeof( prog_load_req ) - (src - name);
    if( len > 126 )
        len = 126;
    MergeArgvArray( src, UtilBuff, len );
    _DBG_Writeln( "about to debugload" );
    _DBG_Write( "Name : " );
    _DBG_Writeln( name );
    _DBG_Write( "UtilBuff : " );
    _DBG_Writeln( UtilBuff );
    GetObjectInfo( name );
    ret->err = D32DebugLoad( name, UtilBuff, &Proc );
    _DBG_Write( "back from debugload - " );
    _DBG_Write16( ret->err );
    _DBG_NewLine();
    ret->flags = LD_FLAG_IS_BIG | LD_FLAG_IS_PROT | LD_FLAG_DISPLAY_DAMAGED;
    if( ret->err == 0 ) {
        ret->task_id = Proc.es;
    } else {
        ret->task_id = 0;
    }
    ret->mod_handle = 0;
    Proc.int_id = -1;
    _DBG_Writeln( "done AccLoadProg" );
    return( sizeof( *ret ) );
}

trap_retval TRAP_CORE( Prog_kill )( void )
{
    prog_kill_ret       *ret;

    _DBG_Writeln( "AccKillProg" );
    ret = GetOutPtr( 0 );
    RedirectFini();
    AtEnd = true;
    ret->err = 0;
    return( sizeof( *ret ) );
}

static int DRegsCount( void )
{
    int     needed;
    int     i;

    needed = 0;
    for( i = 0; i < WatchCount; i++ ) {
        needed += WatchPoints[i].dregs;
    }
    return( needed );
}

trap_retval TRAP_CORE( Set_watch )( void )
{
    watch_point     *curr;
    set_watch_req   *acc;
    set_watch_ret   *ret;

    _DBG_Writeln( "AccSetWatch" );

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    ret->multiplier = 5000;
    ret->err = 0;
    curr = WatchPoints + WatchCount;
    curr->addr.segment = acc->watch_addr.segment;
    curr->addr.offset = acc->watch_addr.offset;
    curr->linear = DPMIGetSegmentBaseAddress( curr->addr.segment ) + curr->addr.offset;
    curr->len = acc->size;
    curr->dregs = ( curr->linear & ( curr->len - 1 ) ) ? 2 : 1;
    curr->handle = -1;
    curr->handle2 = -1;
    curr->value = 0;
    ReadMemory( (addr48_ptr *)&acc->watch_addr, &curr->value, curr->len );
    ++WatchCount;
    if( DRegsCount() <= 4 )
        ret->multiplier |= USING_DEBUG_REG;
    return( sizeof( *ret ) );
}


trap_retval TRAP_CORE( Clear_watch )( void )
{
    _DBG_Writeln( "AccRestoreWatch" );
    /* assume all watches removed at same time */
    WatchCount = 0;
    return( 0 );
}

trap_retval TRAP_CORE( Set_break )( void )
{
    set_break_req       *acc;
    set_break_ret       *ret;
    opcode_type         brk_opcode;

    _DBG_Writeln( "AccSetBreak" );

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    brk_opcode = ret->old;
    D32DebugSetBreak( acc->break_addr.offset, acc->break_addr.segment, false, &Break, &brk_opcode );
    return( sizeof( *ret ) );
}


trap_retval TRAP_CORE( Clear_break )( void )
{
    clear_break_req     *acc;
    opcode_type         dummy;
    opcode_type         brk_opcode;

    acc = GetInPtr( 0 );
    _DBG_Writeln( "AccRestoreBreak" );
    /* assume all breaks removed at same time */
    brk_opcode = acc->old;
    D32DebugSetBreak( acc->break_addr.offset, acc->break_addr.segment, false, &brk_opcode, &dummy );
    return( 0 );
}

static unsigned long SetDRn( int i, unsigned long linear, long type )
{
    switch( i ) {
    case 0:
        SetDR0( linear );
        break;
    case 1:
        SetDR1( linear );
        break;
    case 2:
        SetDR2( linear );
        break;
    case 3:
        SetDR3( linear );
        break;
    }
    return( ( type << DR7_RWLSHIFT(i) )
//          | ( DR7_GEMASK << DR7_GLSHIFT(i) ) | DR7_GE
          | ( DR7_LEMASK << DR7_GLSHIFT(i) ) | DR7_LE );
}


static void ClearDebugRegs( void )
{
    int         i;
    watch_point *wp;

    if( IsDPMI ) {
        for( i = WatchCount, wp = WatchPoints; i != 0; --i, ++wp ) {
            if( wp->handle >= 0 ) {
                DPMIClearWatch( wp->handle );
                wp->handle = -1;
            }
            if( wp->handle2 >= 0 ) {
                DPMIClearWatch( wp->handle2 );
                wp->handle2 = -1;
            }
        }
    } else {
        SetDR6( 0 );
        SetDR7( 0 );
    }
}


static bool SetDebugRegs( void )
{
    int                 i;
    watch_point         *wp;
    bool                success;
    long                rc;

    if( DRegsCount() > 4 )
        return( false );
    if( IsDPMI ) {
        success = true;
        for( i = WatchCount, wp = WatchPoints; i != 0; --i, ++wp ) {
            wp->handle = -1;
            wp->handle2 = -1;
        }
        for( i = WatchCount, wp = WatchPoints; i != 0; --i, ++wp ) {
            _DBG_Write( "Setting Watch On " );
            _DBG_Write32( wp->linear );
            _DBG_NewLine();
            success = false;
            rc = DPMISetWatch( wp->linear, wp->len, DPMI_WATCH_WRITE );
            _DBG_Write( "OK 1 = " );
            _DBG_Write16( rc >= 0 );
            _DBG_NewLine();
            if( rc < 0 )
                break;
            wp->handle = rc;
            if( wp->dregs == 2 ) {
                rc = DPMISetWatch( wp->linear + 4, wp->len, DPMI_WATCH_WRITE );
                _DBG_Write( "OK 2 = " );
                _DBG_Write16( rc >= 0 );
                _DBG_NewLine();
                if( rc < 0 )
                    break;
                wp->handle2 = rc;
            }
            success = true;
        }
        if( !success ) {
            ClearDebugRegs();
        }
        return( success );
    } else {
        int             dr;
        unsigned long   dr7;

        dr = 0;
        dr7 = 0;
        for( i = WatchCount, wp = WatchPoints; i != 0; --i, ++wp ) {
            dr7 |= SetDRn( dr, wp->linear, DRLen( wp->len ) | DR7_BWR );
            ++dr;
            if( wp->dregs == 2 ) {
                dr7 |= SetDRn( dr, wp->linear + 4, DRLen( wp->len ) | DR7_BWR );
                ++dr;
            }
        }
        SetDR7( dr7 );
        return( true );
    }
}

static trap_conditions DoRun( void )
{
    D32DebugRun( &Proc );
    switch( Proc.int_id ) {
    case 1:
        return( COND_TRACE );
    case 3:
        return( COND_BREAK );
    case 5:
        return( COND_USER );
    case 6:
    case 7:
    case 0xd:
        return( COND_EXCEPTION );
    case 0x21:
        return( COND_TERMINATE );
    case 0x23:
        return( COND_EXCEPTION ); // should be TRAP_USER
    default:
        return( COND_EXCEPTION );
    }
}

static bool CheckWatchPoints( void )
{
    addr48_ptr  addr;
    dword       val;
    watch_point *wp;

    for( wp = WatchPoints; wp < WatchPoints + WatchCount; ++wp ) {
        addr.segment = wp->addr.segment;
        addr.offset = wp->addr.offset;
        val = 0;
        if( ReadMemory( &addr, &val, wp->len ) != wp->len ) {
            return( true );
        }
        if( val != wp->value ) {
            return( true );
        }
    }
    return( false );
}

static unsigned ProgRun( bool step )
{
    prog_go_ret *ret;
    byte        int_buff[2];
    addr48_ptr  addr;
    opcode_type brk_opcode;
    opcode_type saved_opcode;

    _DBG_Writeln( "AccRunProg" );

    ret = GetOutPtr( 0 );

    if( step ) {
        Proc.eflags |= TRACE_BIT;
        ret->conditions = DoRun();
        Proc.eflags &= ~TRACE_BIT;
    } else if( WatchCount != 0 ) {
        if( SetDebugRegs() ) {
            ret->conditions = DoRun();
            ClearDebugRegs();
            if( ret->conditions & COND_TRACE ) {
                ret->conditions |= COND_WATCH;
                ret->conditions &= ~COND_TRACE;
            }
        } else {
            for( ;; ) {
                addr.segment = Proc.cs;
                addr.offset = Proc.eip;

                /* have to breakpoint across software interrupts because Intel
                    doesn't know how to design chips */
                if( ReadMemory( &addr, int_buff, 2 ) == 2 && int_buff[0] == 0xcd ) {
                    addr.offset += 2;
                } else {
                    int_buff[0] = 0;
                }
                if( int_buff[0] != 0 && ReadMemory( &addr, &saved_opcode, sizeof( saved_opcode ) ) == sizeof( saved_opcode ) ) {
                    brk_opcode = BRKPOINT;
                    WriteMemory( &addr, &brk_opcode, sizeof( brk_opcode ) );
                    ret->conditions = DoRun();
                    addr.offset = Proc.eip;
                    WriteMemory( &addr, &saved_opcode, sizeof( saved_opcode ) );
                } else {
                    Proc.eflags |= TRACE_BIT;
                    ret->conditions = DoRun();
                    Proc.eflags &= ~TRACE_BIT;
                }
                if( (ret->conditions & (COND_TRACE | COND_BREAK)) == 0 )
                    break;
                if( CheckWatchPoints() ) {
                    ret->conditions |= COND_WATCH;
                    ret->conditions &= ~(COND_TRACE|COND_BREAK);
                    break;
                }
            }
        }
    } else {
        ret->conditions = DoRun();
    }

    ret->conditions |= COND_CONFIG;
    ret->program_counter.offset = Proc.eip;
    ret->program_counter.segment = Proc.cs;
    ret->stack_pointer.offset = Proc.esp;
    ret->stack_pointer.segment = Proc.ss;
    return( sizeof( *ret ) );
}

trap_retval TRAP_CORE( Prog_go )( void )
{
    return( ProgRun( false ) );
}

trap_retval TRAP_CORE( Prog_step )( void )
{
    return( ProgRun( true ) );
}

trap_retval TRAP_CORE( Get_next_alias )( void )
{
    get_next_alias_ret  *ret;

    ret = GetOutPtr( 0 );
    ret->seg = 0;
    ret->alias = 0;
    return( sizeof( *ret ) );
}


trap_retval TRAP_CORE( Get_err_text )( void )
{
    static char *DosErrMsgs[] = {
        #define pick(a,b)   b,
        #include "dosmsgs.h"
        #undef pick
    };
    get_err_text_req    *acc;
    char                *err_txt;

    _DBG_Writeln( "AccErrText" );
    acc = GetInPtr( 0 );
    err_txt = GetOutPtr( 0 );
    if( acc->err < ERR_LAST ) {
        strcpy( err_txt, DosErrMsgs[acc->err] );
        _DBG_Writeln( "After strcpy" );
    } else {
        _DBG_Writeln( "After acc->error_code > MAX_ERR_CODE" );
        strcpy( err_txt, TRP_ERR_unknown_system_error );
        ultoa( acc->err, err_txt + strlen( err_txt ), 16 );
        _DBG_Writeln( "After utoa()" );
    }
    return( strlen( err_txt ) + 1 );
}

trap_retval TRAP_CORE( Get_lib_name )( void )
{
    get_lib_name_ret    *ret;

    ret = GetOutPtr( 0 );
    ret->mod_handle = 0;
    return( sizeof( *ret ) );
}

trap_retval TRAP_CORE( Get_message_text )( void )
{
    static const char * const ExceptionMsgs[] = {
        #define pick(a,b) b,
        #include "x86exc.h"
        #undef pick
    };
    get_message_text_ret        *ret;
    char                        *err_txt;

    ret = GetOutPtr( 0 );
    err_txt = GetOutPtr( sizeof( *ret ) );
    if( Proc.int_id == -1 ) {
        err_txt[0] = '\0';
    } else {
        if( Proc.int_id < sizeof( ExceptionMsgs ) / sizeof( ExceptionMsgs[0] ) ) {
            strcpy( err_txt, ExceptionMsgs[Proc.int_id] );
        } else {
            strcpy( err_txt, TRP_EXC_unknown );
        }
        Proc.int_id = -1;
    }
    ret->flags = MSG_NEWLINE | MSG_ERROR;
    return( sizeof( *ret ) + strlen( err_txt ) + 1 );
}

trap_version TRAPENTRY TrapInit( const char *parms, char *err, bool remote )
{
    trap_version        ver;
    int                 error_num;

    _DBG_Writeln( "TrapInit" );
    remote = remote; parms = parms;
    err[0] = '\0'; /* all ok */
    ver.major = TRAP_MAJOR_VERSION;
    ver.minor = TRAP_MINOR_VERSION;
    ver.remote = false;
    RedirectInit();
    RealNPXType = NPXType();
    WatchCount = 0;
    FakeBreak = false;
    error_num = D32DebugInit( &Proc, INT_PRT_SCRN_KEY );
    if( error_num ) {
        _DBG_Writeln( "D32DebugInit() failed:" );
        exit(1);
    }
    Proc.int_id = -1;
    D32DebugBreakOp( &Break );  /* Get the 1 byte break op */
    return( ver );
}

void TRAPENTRY TrapFini( void )
{
    _DBG_Writeln( "TrapFini" );
    D32DebugTerm();
}
