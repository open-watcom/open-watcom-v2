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

#include "digcpu.h"
#include "x86cpu.h"
#include "miscx87.h"
#include "dosredir.h"
#include "doscomm.h"
#include "brkptcpu.h"


#define INT_PRT_SCRN_KEY    0x05
#define MAX_WATCHES         32
#define IsDPMI              (_d16info.swmode == 0)
#define MAX_DREGS           3

typedef struct watch_point {
    uint_64             value;
    dword               linear;
    addr48_ptr          addr;
    word                size;
    word                dregs;
    short               handle[MAX_DREGS];
} watch_point;

extern bool IsSel32bit( unsigned_16 );
#pragma aux IsSel32bit = \
        ".386"              \
        "movzx eax,ax"      \
        "lar   eax,eax"     \
        "and   eax,400000h" \
        "jz short L1"       \
        "mov   ax,1"        \
    "L1:"                   \
    __parm  [__ax] \
    __value [__al]

static struct {
    unsigned_32         size;
    unsigned_32         start;
} *ObjInfo;

static TSF32            Proc;
static opcode_type      BreakOpcode;
static bool             FakeBreak;
static bool             AtEnd;
static unsigned         NumObjects;
static unsigned_8       RealNPXType;

static char             UtilBuff[BUFF_SIZE];

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

static size_t ReadWrite( bool (*r)(addr48_ptr FarPtr, bool, void FarPtr, size_t), addr48_ptr *addr, void FarPtr data, size_t req_len )
{
    size_t  len;

    _DBG_Write( "checking " );
    _DBG_Write16( addr->segment );
    _DBG_Write( ":" );
    _DBG_Write32( addr->offset );
    _DBG_Write( " for 0x" );
    _DBG_Write16( req_len );
    _DBG_Write( " bytes -- " );
    if( !r( addr, false, data, req_len ) ) {
        _DBG_Writeln( "OK" );
        addr->offset += req_len;
        return( req_len );
    }
    _DBG_Writeln(( "Bad" ));
    for( len = req_len; len > 0; len-- ) {
        if( r( addr, false, data, 1 ) )
            break;
        addr->offset++;
        data = (char FarPtr)data + 1;
    }
    return( req_len - len );
}

trap_retval TRAP_CORE( Get_sys_config )( void )
{
    get_sys_config_ret  *ret;

    _DBG_Writeln( "AccGetConfig" );

    ret = GetOutPtr( 0 );
    ret->os = DIG_OS_RATIONAL;
    ret->osmajor = _osmajor;
    ret->osminor = _osminor;
    ret->cpu = X86CPUType();
    ret->huge_shift = 12;
    if( !AtEnd && HAVE_EMU ) {
        ret->fpu = X86_EMU;
    } else {
        ret->fpu = RealNPXType;
    }
    ret->arch = DIG_ARCH_X86;
    return( sizeof( *ret ) );
}


trap_retval TRAP_CORE( Map_addr )( void )
{
    addr48_ptr          fp;
    map_addr_req        *acc;
    map_addr_ret        *ret;
    unsigned            i;

    _DBG_Writeln( "AccMapAddr" );

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    ret->lo_bound = 0;
    ret->hi_bound = ~(addr48_off)0;
    fp.offset = acc->in_addr.offset;
    fp.segment = acc->in_addr.segment;
    switch( fp.segment ) {
    case MAP_FLAT_CODE_SELECTOR:
    case MAP_FLAT_DATA_SELECTOR:
        fp.segment = 1;
        fp.offset += ObjInfo[0].start;
        for( i = 0; i < NumObjects; ++i ) {
            if( ObjInfo[i].start <= fp.offset && ( ObjInfo[i].start + ObjInfo[i].size ) > fp.offset ) {
                fp.segment = i + 1;
                fp.offset -= ObjInfo[i].start;
                ret->lo_bound = ObjInfo[i].start - ObjInfo[0].start;
                ret->hi_bound = ret->lo_bound + ObjInfo[i].size - 1;
                break;
            }
        }
        break;
    }
    D32Relocate( &fp );
    ret->out_addr.segment = fp.segment;
    ret->out_addr.offset = fp.offset;
    return( sizeof( *ret ) );
}

trap_retval TRAP_CORE( Machine_data )( void )
{
    machine_data_req    *acc;
    machine_data_ret    *ret;
    machine_data_spec   *data;
    addr48_ptr          addr;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    ret->cache_start = 0;
    ret->cache_end = ~(addr_off)0;
    if( acc->info_type == X86MD_ADDR_CHARACTERISTICS ) {
        data = GetOutPtr( sizeof( *ret ) );
        data->x86_addr_flags = 0;
        addr.segment = acc->addr.segment;
        addr.offset = 0;
        if( IsSel32bit( acc->addr.segment ) ) {
            data->x86_addr_flags = X86AC_BIG;
        }
        return( sizeof( *ret ) + sizeof( data->x86_addr_flags ) );
    }
    return( sizeof( *ret ) );
}

trap_retval TRAP_CORE( Checksum_mem )( void )
{
    size_t              len;
    size_t              i;
    size_t              got;
    checksum_mem_req    *acc;
    checksum_mem_ret    *ret;
    size_t              want;
    unsigned long       sum;

    _DBG_Writeln( "AccChkSum" );

    acc = GetInPtr( 0 );
    want = sizeof( UtilBuff );
    sum = 0;
    for( len = acc->len; len > 0; len -= want ) {
        if( want > len )
            want = len;
        got = ReadWrite( D32DebugRead, &acc->in_addr, UtilBuff, want );
        for( i = 0; i < got; ++i ) {
            sum += ((unsigned char *)UtilBuff)[i];
        }
        if( got != want ) {
            break;
        }
    }
    ret = GetOutPtr( 0 );
    ret->result = sum;
    return( sizeof( ret ) );
}


trap_retval TRAP_CORE( Read_mem )( void )
{
    read_mem_req        *acc;

    _DBG_Writeln( "ReadMem" );
    acc = GetInPtr( 0 );
    return( ReadWrite( D32DebugRead, &acc->mem_addr, GetOutPtr( 0 ), acc->len ) );
}

trap_retval TRAP_CORE( Write_mem )( void )
{
    write_mem_req       *acc;
    write_mem_ret       *ret;

    _DBG_Writeln( "WriteMem" );
    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    ret->len = ReadWrite( D32DebugWrite, &acc->mem_addr, GetInPtr( sizeof( *acc ) ),
                            GetTotalSizeIn() - sizeof( *acc ) );
    return( sizeof( *ret ) );
}

trap_retval TRAP_CORE( Read_io )( void )
{
    read_io_req         *acc;
    void                *data;

    acc = GetInPtr( 0 );
    data = GetOutPtr( 0 );
    switch( acc->len ) {
    case 1:
        *(byte *)data = In_b( acc->IO_offset );
        break;
    case 2:
        *(word *)data = In_w( acc->IO_offset );
        break;
    case 4:
        *(dword *)data = In_d( acc->IO_offset );
        break;
    default:
        return( 0 );
    }
    return( acc->len );
}

trap_retval TRAP_CORE( Write_io )( void )
{
    size_t              len;
    write_io_req        *acc;
    write_io_ret        *ret;
    void                *data;

    acc = GetInPtr( 0 );
    data = GetInPtr( sizeof( *acc ) );
    len = GetTotalSizeIn() - sizeof( *acc );
    switch( len ) {
    case 1:
        Out_b( acc->IO_offset, *(byte *)data );
        break;
    case 2:
        Out_w( acc->IO_offset, *(word *)data );
        break;
    case 4:
        Out_d( acc->IO_offset, *(dword *)data );
        break;
    default:
        len = 0;
        break;
    }
    ret = GetOutPtr( 0 );
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
    } else if( RealNPXType != X86_NOFPU ) {
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
    } else if( RealNPXType != X86_NOFPU ) {
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
    unsigned_32         ne_header_off;
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
    if( head.dos.signature != EXESIGN_DOS ) {
        close( handle );
        return;
    }
    lseek( handle, NE_HEADER_OFFSET, SEEK_SET );
    read( handle, &ne_header_off, sizeof( ne_header_off ) );
    lseek( handle, ne_header_off, SEEK_SET );
    read( handle, &head.os2, sizeof( head.os2 ) );
    switch( head.os2.signature ) {
    case EXESIGN_LE:
    case EXESIGN_LX:
        lseek( handle, head.os2.objtab_off + ne_header_off, SEEK_SET );
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
    prog_load_ret   *ret;
    size_t          len;

    _DBG_Writeln( "AccLoadProg" );
    AtEnd = false;
    src = name = GetInPtr( sizeof( prog_load_req ) );
    ret = GetOutPtr( 0 );
    while( *src++ != '\0' )
        {}
    len = GetTotalSizeIn() - sizeof( prog_load_req ) - ( src - name );
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
    RedirectFini();
    AtEnd = true;
    ret = GetOutPtr( 0 );
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

static word GetDRInfo( word segment, dword offset, word size, dword *plinear )
{
    word    dregs;
    dword   linear;

    linear = DPMIGetSegmentBaseAddress( segment ) + offset;
    dregs = 1;
    if( size == 8 ) {
        size = 4;
        dregs++;
    }
    if( linear & ( size - 1 ) )
        dregs++;
    if( plinear != NULL )
        *plinear = linear & ~( size - 1 );
    return( dregs );
}

trap_retval TRAP_CORE( Set_watch )( void )
{
    set_watch_req   *acc;
    set_watch_ret   *ret;
    watch_point     *wp;
    int             i;

    _DBG_Writeln( "AccSetWatch" );

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    ret->multiplier = 5000;
    ret->err = 1;       // failure
    if( WatchCount < MAX_WATCHES ) {
        ret->err = 0;   // OK
        wp = WatchPoints + WatchCount;
        wp->addr.segment = acc->watch_addr.segment;
        wp->addr.offset = acc->watch_addr.offset;
        wp->size = acc->size;
        wp->value = 0;
        D32DebugRead( &wp->addr, false, &wp->value, wp->size );

        wp->dregs = GetDRInfo( wp->addr.segment, wp->addr.offset, wp->size, &wp->linear );
        for( i = 0; i < MAX_DREGS; i++ ) {
            wp->handle[i] = -1;
        }

        WatchCount++;
        if( DRegsCount() <= 4 ) {
            ret->multiplier |= USING_DEBUG_REG;
        }
    }
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
    opcode_type         old_opcode;

    _DBG_Writeln( "AccSetBreak" );

    acc = GetInPtr( 0 );
    D32DebugSetBreak( &acc->break_addr, false, &BreakOpcode, &old_opcode );
    ret = GetOutPtr( 0 );
    ret->old = old_opcode;
    return( sizeof( *ret ) );
}


trap_retval TRAP_CORE( Clear_break )( void )
{
    clear_break_req     *acc;
    opcode_type         old_opcode;

    acc = GetInPtr( 0 );
    _DBG_Writeln( "AccRestoreBreak" );
    /* assume all breaks removed at same time */
    old_opcode = acc->old;
    D32DebugSetBreak( &acc->break_addr, false, &old_opcode, &old_opcode );
    return( 0 );
}

static dword SetDRn( int dr, dword linear, word type )
{
    SetDRx( dr, linear );
    return( ( (dword)type << DR7_RWLSHIFT( dr ) )
//          | ( DR7_GEMASK << DR7_GLSHIFT( dr ) )
          | ( DR7_LEMASK << DR7_GLSHIFT( dr ) ) );
}

static void ClearDebugRegs( void )
{
    int         i;
    int         j;
    watch_point *wp;

    if( IsDPMI ) {
        for( wp = WatchPoints, i = WatchCount; i-- > 0; wp++ ) {
            for( j = 0; j < MAX_DREGS; j++ ) {
                if( wp->handle[j] >= 0 ) {
                    DPMIClearWatch( wp->handle[j] );
                    wp->handle[j] = -1;
                }
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
    int                 j;
    long                rc;
    watch_point         *wp;
    dword               linear;
    word                size;

    if( DRegsCount() > 4 )
        return( false );
    if( IsDPMI ) {
        for( i = 0; i < WatchCount; ++i ) {
            for( j = 0; j < MAX_DREGS; j++ ) {
                WatchPoints[i].handle[j] = -1;
            }
        }
        for( wp = WatchPoints, i = WatchCount; i-- > 0; wp++ ) {
            linear = wp->linear;
            size = wp->size;
            if( size == 8 )
                size = 4;
            _DBG_Write( "Setting Watch On " );
            _DBG_Write32( linear );
            _DBG_NewLine();
            for( j = 0; j < wp->dregs; j++ ) {
                rc = DPMISetWatch( linear, size, DPMI_WATCH_WRITE );
                _DBG_Write( "OK " );
                _DBG_Write16( j );
                _DBG_Write( " = " );
                _DBG_Write16( rc >= 0 );
                _DBG_NewLine();
                if( rc < 0 ) {
                    ClearDebugRegs();
                    return( false );
                }
                wp->handle[j] = rc;
                linear += size;
            }
        }
    } else {
        int         dr;
        dword       dr7;
        word        type;

        dr = 0;
        dr7 = /* DR7_GE | */ DR7_LE;
        for( wp = WatchPoints, i = WatchCount; i-- > 0; wp++ ) {
            linear = wp->linear;
            size = wp->size;
            if( size == 8 )
                size = 4;
            type = DRLen( size ) | DR7_BWR;
            for( j = 0; j < wp->dregs; j++ ) {
                dr7 |= SetDRn( dr, linear, type );
                ++dr;
                linear += size;
            }
        }
        SetDR7( dr7 );
    }
    return( true );
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
    case 0x0d:
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
    watch_point *wp;
    int         i;
    uint_64     value;

    for( wp = WatchPoints, i = WatchCount; i-- > 0; wp++ ) {
        value = 0;
        D32DebugRead( &wp->addr, false, &value, wp->size );
        if( wp->value != value ) {
            return( true );
        }
    }
    return( false );
}

static unsigned ProgRun( bool step )
{
    prog_go_ret *ret;
    byte        int_buff[2 + sizeof( opcode_type )];
    addr48_ptr  start_addr;
    opcode_type old_opcode;

    _DBG_Writeln( "AccRunProg" );

    ret = GetOutPtr( 0 );

    if( step ) {
        Proc.eflags |= INTR_TF;
        ret->conditions = DoRun();
        Proc.eflags &= ~INTR_TF;
    } else if( WatchCount > 0 ) {
        if( SetDebugRegs() ) {
            ret->conditions = DoRun();
            ClearDebugRegs();
            if( ret->conditions & COND_TRACE ) {
                ret->conditions |= COND_WATCH;
                ret->conditions &= ~COND_TRACE;
            }
        } else {
            for( ;; ) {
                start_addr.segment = Proc.cs;
                start_addr.offset = Proc.eip;
                /*
                 * have to breakpoint across software interrupts because Intel
                 * doesn't know how to design chips
                 */
                if( !D32DebugRead( &start_addr, false, int_buff, 2 + sizeof( opcode_type ) ) && int_buff[0] == 0xcd ) {
                    start_addr.offset += 2;
                    D32DebugSetBreak( &start_addr, false, &BreakOpcode, &old_opcode );
                    ret->conditions = DoRun();
                    start_addr.offset = Proc.eip;
                    D32DebugSetBreak( &start_addr, false, &old_opcode, &old_opcode );
                } else {
                    Proc.eflags |= INTR_TF;
                    ret->conditions = DoRun();
                    Proc.eflags &= ~INTR_TF;
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

    /* unused parameters */ (void)remote; (void)parms;

    _DBG_Writeln( "TrapInit" );
    err[0] = '\0'; /* all ok */
    ver.major = TRAP_VERSION_MAJOR;
    ver.minor = TRAP_VERSION_MINOR;
    ver.remote = false;
    RedirectInit();
    RealNPXType = NPXType();
    WatchCount = 0;
    FakeBreak = false;
    if( D32DebugInit( &Proc, INT_PRT_SCRN_KEY ) ) {
        _DBG_Writeln( "D32DebugInit() failed:" );
        exit(1);
    }
    Proc.int_id = -1;
    D32DebugBreakOp( &BreakOpcode );  /* Get the break opcode */
    return( ver );
}

void TRAPENTRY TrapFini( void )
{
    _DBG_Writeln( "TrapFini" );
    D32DebugTerm();
}
