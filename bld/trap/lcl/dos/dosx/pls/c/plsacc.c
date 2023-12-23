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
* Description:  Trap file for debugging PharLap DOS extended apps.
*
****************************************************************************/


#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <io.h>
#include <i86.h>
#include <fcntl.h>
#include "pltypes.h"
#include "hw386.h"
#include "dilintf.h"
#include "pharlap.h"
#include "dilproto.h"
#include "trpimp.h"
#include "trpcomm.h"
#include "exedos.h"
#include "exeos2.h"
#include "exepe.h"
#include "madregs.h"
#include "brkptcpu.h"
#include "digcpu.h"
#include "x86cpu.h"
#include "miscx87.h"
#include "dosredir.h"
#include "doscomm.h"
#include "trperr.h"
#include "doserr.h"
#include "plsintr.h"
#include "segmcpu.h"

#include "clibint.h"


#define MAX_OBJECTS     128

#define DBE_BASE        1000

#define MAX_WATCHES     32

#ifdef DEBUG_TRAP
#define _DBG( x )  cputs x
#else
#define _DBG( x )
#endif

typedef struct watch_point {
    uint_64     value;
    dword       linear;
    addr48_ptr  addr;
    word        size;
    word        dregs;
} watch_point;

typedef struct {
    long    have_vmm;
    long    nconvpg;
    long    nbimpg;
    long    nextpg;
    long    extlim;
    long    aphyseg;
    long    alockpg;
    long    sysphyspg;
    long    nfreepg;
    long    beglinaddr;
    long    endlinaddr;
    long    secsincelastvmstat;
    long    pgfaults;
    long    pgswritten;
    long    pgsreclaimed;
    long    vmpages;
    long    pgfilesize;
    long    emspages;
    long    mincomvmem;
    long    maxpagefile;
    long    vmflags;
    long    reserved[4];
} memory_stats;

#pragma aux (metaware)  _dil_global;
#pragma aux (metaware)  _dx_config_inf;


extern  long            _STACKTOP;

opcode_type             OldFakeOpcode;
bool                    FakeBreak;
bool                    AtEnd;
bool                    ReportedAlias;
short                   InitialSS;
short                   InitialCS;

static unsigned         SaveMSW;
static unsigned_8       RealNPXType;
static char             UtilBuff[BUFF_SIZE];
static MSB              Mach;
static SEL_REMAP        SelBlk;
static bool             HavePSP;
static unsigned long    ObjOffReloc[MAX_OBJECTS];

static watch_point      WatchPoints[MAX_WATCHES];
static int              WatchCount = 0;

static opcode_type      BreakOpcode;

#define DBG_ERROR "System error: "
static char *DBErrors[] = {
        "???",
        "???",
        "File error",
        "Invalid protected mode address",
        "???",
        "???",
        "???",
        "???",
        "???",
        "???",
        "???",
        "???",
        "Memory error",
        "Unable to move segments at init time",
        "Internal system error",
        "No child program is present",
        "???",
        "Child program is already present",
        "Invalid DOS-Extender Version number",
        "???",
        "???",
        "Attempt to set segreg to an invalid value",
        "Invalid SS:ESP value",
        "Child's -NISTACK and/or -ISTKSIZE switches",
        "Child's -MINIBUF switch not satisfied",
        "Child's -CALLBUFS switch not satisfied",
        "Bad parameter passed to function",
        "Function not supported under DPMI",
        "No active protected mode state",
        "Child privilege level cannot be satisfied",
        "Current DOS-X is bound, no good",
        "DILIB loadable portion not yet loaded",
        "Incorrect DILIB loadable portion version"
};

int SetUsrTask( void )
{
    if( HavePSP ) {
        SetPSP( _dil_global._chpsp );
        return( 1 );
    }
    return( 0 );
}

void SetDbgTask( void )
{
    SetPSP( _dil_global._psp );
}

trap_retval TRAP_CORE( Get_sys_config )( void )
{
    get_sys_config_ret  *ret;

    _DBG(("AccGetConfig\r\n"));
    ret = GetOutPtr( 0 );
    ret->os = DIG_OS_PHARLAP;
    ret->osmajor = _osmajor;
    ret->osminor = _osminor;
    ret->cpu = X86CPUType();
    ret->huge_shift = 12;
    if( HavePSP && !AtEnd ) {
        if( Mach.msb_cr0 & MSW_EM ) {
            ret->fpu = X86_EMU;
        } else {
            ret->fpu = RealNPXType;
        }
    } else {
        ret->fpu = RealNPXType;
    }
    ret->arch = DIG_ARCH_X86;
    return( sizeof( *ret ) );
}


trap_retval TRAP_CORE( Map_addr )( void )
{
    map_addr_req        *acc;
    map_addr_ret        *ret;
    unsigned            object;

    _DBG(("AccMapAddr\r\n"));
    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    object = acc->in_addr.segment;
    switch( object ) {
    case MAP_FLAT_DATA_SELECTOR:
        object = 1;
        ret->out_addr.segment = Mach.msb_ds;
        break;
    case MAP_FLAT_CODE_SELECTOR:
        object = 1;
        /* fall through */
    default:
        ret->out_addr.segment = Mach.msb_cs;
        break;
    }
    ret->out_addr.offset = acc->in_addr.offset + ObjOffReloc[object - 1];
    ret->lo_bound = 0;
    ret->hi_bound = ~(addr48_off)0;
    return( sizeof( *ret ) );
}

static bool IsProtSeg( USHORT seg )
{
    CD_DES      desc;

    if( dbg_rdsdes( seg, 1, &desc ) != 0 )
        return( false );
    if( ( desc.arights & AR_CODE ) == AR_CODE )
        return( true );
    if( ( desc.arights & AR_DATA ) == AR_DATA )
        return( true );
    return( false );
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
        data->x86_addr_flags = ( IsProtSeg( acc->addr.segment ) ) ? X86AC_BIG : 0;
        return( sizeof( *ret ) + sizeof( data->x86_addr_flags ) );
    }
    return( sizeof( *ret ) );
}

static ULONG RealAddr( addr48_ptr *addr )
{
    return( ( (ULONG)addr->segment << 16 ) + ( addr->offset & 0xFFFF ) );
}


static int RunProgram( void )
{
    MSB         temp;
    int         rc;
    int         i;

    dbg_rdmsb( &temp );
    temp.msb_cs = Mach.msb_cs;
    temp.msb_ds = Mach.msb_ds;
    temp.msb_es = Mach.msb_es;
    temp.msb_fs = Mach.msb_fs;
    temp.msb_gs = Mach.msb_gs;
    temp.msb_ss = Mach.msb_ss;
    temp.msb_eax = Mach.msb_eax;
    temp.msb_ebx = Mach.msb_ebx;
    temp.msb_ecx = Mach.msb_ecx;
    temp.msb_edx = Mach.msb_edx;
    temp.msb_esi = Mach.msb_esi;
    temp.msb_edi = Mach.msb_edi;
    temp.msb_ebp = Mach.msb_ebp;
    temp.msb_esp = Mach.msb_esp;
    temp.msb_eip = Mach.msb_eip;
    for( i = 0; i <= 7; ++i )
        temp.msb_dreg[i] = Mach.msb_dreg[i];
    temp.msb_eflags = Mach.msb_eflags;
    dbg_wrmsb( &temp );
    rc = dbg_go();
    dbg_rdmsb( &Mach );
    return( rc );
}


static size_t ReadWrite( bool (*r)(addr48_ptr *, void *, size_t), addr48_ptr *addr, void *data, size_t req_len )
{
    size_t      len;

    if( !r( addr, data, req_len ) ) {
        addr->offset += req_len;
        return( req_len );
    }
    for( len = req_len; len > 0; --len ) {
        if( r( addr, data, 1 ) )
            break;
        data = (char *)data + 1;
        addr->offset++;
    }
    return( req_len - len );
}

static bool ReadMemory( addr48_ptr *addr, void *data, size_t len )
{
    if( IsProtSeg( addr->segment ) ) {
        PTR386  paddr;

        paddr.selector = addr->segment;
        paddr.offset = addr->offset;
        return( dbg_pread( &paddr, len, data ) != 0 );
    } else {
        return( dbg_rread( RealAddr( addr ), len, data ) != 0 );
    }
}

static bool WriteMemory( addr48_ptr *addr, void *data, size_t len )
{
    if( IsProtSeg( addr->segment ) ) {
        PTR386  paddr;

        paddr.selector = addr->segment;
        paddr.offset = addr->offset;
        return( dbg_pwrite( &paddr, len, data ) != 0 );
    } else {
        return( dbg_rwrite( RealAddr( addr ), len, data ) != 0 );
    }
}

trap_retval TRAP_CORE( Checksum_mem )( void )
{
    size_t              len;
    size_t              got;
    size_t              i;
    size_t              want;
    checksum_mem_req    *acc;
    checksum_mem_ret    *ret;
    unsigned long       sum;

    _DBG(("AccChkSum\r\n"));
    acc = GetInPtr( 0 );
    want = sizeof( UtilBuff );
    sum = 0;
    for( len = acc->len; len > 0; len -= got ) {
        if( want > len )
            want = len;
        got = ReadWrite( ReadMemory, &acc->in_addr, UtilBuff, want );
        for( i = 0; i < got; ++i ) {
            sum += ((unsigned char *)UtilBuff)[i];
        }
        if( got != want ) {
            break;
        }
    }
    ret = GetOutPtr( 0 );
    ret->result = sum;
    return( sizeof( *ret ) );
}


trap_retval TRAP_CORE( Read_mem )( void )
{
    read_mem_req    *acc;

    _DBG(("ReadMem\r\n"));
    acc = GetInPtr( 0 );
    return( ReadWrite( ReadMemory, &acc->mem_addr, GetOutPtr( 0 ), acc->len ) );
}

trap_retval TRAP_CORE( Write_mem )( void )
{
    write_mem_req       *acc;
    write_mem_ret       *ret;

    _DBG(("WriteMem\r\n"));
    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    ret->len = ReadWrite( WriteMemory, &acc->mem_addr, GetInPtr( sizeof( *acc ) ),
                            GetTotalSizeIn() - sizeof( *acc ) );
    return( sizeof( *ret ) );
}

trap_retval TRAP_CORE( Read_io )( void )
{
    int                 err;
    read_io_req         *acc;
    void                *ret;

    _DBG(("ReadPort\r\n"));
    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    switch( acc->len ) {
    case 1:
        err = dbg_iport( acc->IO_offset, ret, 1 );
        break;
    case 2:
        err = dbg_iport( acc->IO_offset, ret, 2 );
        break;
    case 4:
        err = dbg_iport( acc->IO_offset, ret, 3 );
        break;
    default:
        err = DBE_INVWID;
    }
    if( err != 0 ) {
        return( 0 );
    }
    return( acc->len );
}


trap_retval TRAP_CORE( Write_io )( void )
{
    int             err;
    size_t          len;
    write_io_req    *acc;
    write_io_ret    *ret;
    void            *data;

    _DBG(("WritePort\r\n"));
    acc = GetInPtr( 0 );
    data = GetInPtr( sizeof( *acc ) );
    len = GetTotalSizeIn() - sizeof( *acc );
    switch( len ) {
    case 1:
        err = dbg_oport( acc->IO_offset, *(byte *)data, 1 );
        break;
    case 2:
        err = dbg_oport( acc->IO_offset, *(word *)data, 2 );
        break;
    case 4:
        err = dbg_oport( acc->IO_offset, *(dword *)data, 3 );
        break;
    default:
        err = DBE_INVWID;
    }
    ret = GetOutPtr( 0 );
    ret->len = len;
    if( err != 0 ) {
        ret->len = 0;
    }
    return( sizeof( *ret ) );
}

static void TaskFPExec( ULONG rtn, struct x86_fpu *regs )
{
    long        eax,eip,efl;
    short       cs,ds;

    if( RealNPXType == X86_387 || RealNPXType == X86_287 || (Mach.msb_cr0 & MSW_EM) ) {
        ds = Mach.msb_ds;
        eax = Mach.msb_eax;
        cs = Mach.msb_cs;
        eip = Mach.msb_eip;
        efl = Mach.msb_eflags;
        Mach.msb_ds = GetDS();
        Mach.msb_eax = (ULONG)regs;
        Mach.msb_cs = GetCS();
        Mach.msb_eip = rtn;
        RunProgram();
        Mach.msb_cs = cs;
        Mach.msb_eip = eip;
        Mach.msb_ds = ds;
        Mach.msb_eax = eax;
        Mach.msb_eflags = efl;
    }
}

static void ReadCPU( struct x86_cpu *r )
{
    r->eax = Mach.msb_eax;
    r->ebx = Mach.msb_ebx;
    r->ecx = Mach.msb_ecx;
    r->edx = Mach.msb_edx;
    r->esi = Mach.msb_esi;
    r->edi = Mach.msb_edi;
    r->esp = Mach.msb_esp;
    r->ebp = Mach.msb_ebp;
    r->eip = Mach.msb_eip;
    r->ds  = Mach.msb_ds;
    r->cs  = Mach.msb_cs;
    r->es  = Mach.msb_es;
    r->ss  = Mach.msb_ss;
    r->fs  = Mach.msb_fs;
    r->gs  = Mach.msb_gs;
    r->efl = Mach.msb_eflags;
}

static void WriteCPU( struct x86_cpu *r )
{
    Mach.msb_eax = r->eax;
    Mach.msb_ebx = r->ebx;
    Mach.msb_ecx = r->ecx;
    Mach.msb_edx = r->edx;
    Mach.msb_esi = r->esi;
    Mach.msb_edi = r->edi;
    Mach.msb_esp = r->esp;
    Mach.msb_ebp = r->ebp;
    Mach.msb_eip = r->eip;
    Mach.msb_ds = r->ds;
    Mach.msb_cs = r->cs;
    Mach.msb_es = r->es;
    Mach.msb_ss = r->ss;
    Mach.msb_fs = r->fs;
    Mach.msb_gs = r->gs;
    Mach.msb_eflags = r->efl;
}

trap_retval TRAP_CORE( Read_regs )( void )
{
    mad_registers       *mr;

    mr = GetOutPtr( 0 );
    ReadCPU( &mr->x86.cpu );
    TaskFPExec( (ULONG)&Read387, &mr->x86.u.fpu );
    return( sizeof( mr->x86 ) );
}

trap_retval TRAP_CORE( Write_regs )( void )
{
    mad_registers       *mr;

    mr = GetInPtr( sizeof( write_regs_req ) );
    WriteCPU( &mr->x86.cpu );
    TaskFPExec( (ULONG)&Write387, &mr->x86.u.fpu );
    return( 0 );
}

static int map_dbe( int err )
{
    if( err == 0 )
        return( err );
    return( -err + DBE_BASE );
}

static void CheckForPE( char *name )
{
    int                 handle;
    unsigned_32         ne_header_off;
    unsigned            i;
    pe_object           obj;
    union {
        dos_exe_header  dos;
        pe_exe_header   pe;
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
    read( handle, &head.pe, PE_HDR_SIZE );
    read( handle, (char *)&head.pe + PE_HDR_SIZE, PE_OPT_SIZE( head.pe ) );
    switch( head.pe.signature ) {
    case EXESIGN_PE:
    case EXESIGN_PL:
        for( i = 0; i < head.pe.fheader.num_objects; ++i ) {
            read( handle, &obj, sizeof( obj ) );
            ObjOffReloc[i] = Mach.msb_eip - PE( head.pe, entry_rva ) + obj.rva;
        }
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
    *dst = '\r';
    return( dst - start );
}

trap_retval TRAP_CORE( Prog_load )( void )
{
    char            *src;
    char            *name;
    prog_load_ret   *ret;
    size_t          len;

    _DBG(("AccLoadProg\r\n"));
    memset( ObjOffReloc, 0, sizeof( ObjOffReloc ) );
    AtEnd = false;
    ReportedAlias = false;
    ret = GetOutPtr( 0 );
    src = name = GetInPtr( sizeof( prog_load_req ) );
    while( *src++ != '\0' )
        {}
    len = GetTotalSizeIn() - sizeof( prog_load_req ) - ( src - name );
    if( len > 126 )
        len = 126;
    UtilBuff[0] = MergeArgvArray( src, UtilBuff + 1, len );
    ret->err = map_dbe( dbg_load( (unsigned char *)name, NULL, (unsigned char *)UtilBuff ) );
    if( ret->err == 0 ) {
        HavePSP = true;
    }
    FakeBreak = 1;
//  _dil_global._action_flags |= ACT_DIS387;
    FakeBreak = 0;
    dbg_rdmsb( &Mach );
    if( Mach.msb_protmf ) {
        //loadret->flags = LD_FLAG_IS_BIG | LD_FLAG_IS_FLAT | LD_FLAG_IS_PROT;
        _DBG(("LD_FLAG_IS_PROT!!!\r\n"));
        ret->flags = LD_FLAG_IS_BIG | LD_FLAG_IS_PROT;
        ret->task_id = 4;
        CheckForPE( name );
    } else {
        _DBG(("Not Protected mode?!!!\r\n"));
        ret->flags = LD_FLAG_IS_BIG;
        ret->task_id = Mach.msb_ds;
    }
    ret->mod_handle = 0;
    InitialSS = Mach.msb_ss;
    InitialCS = Mach.msb_cs;
    Mach.msb_event = (USHORT)-1;
    return( sizeof( *ret ) );
}

extern void finit( void );
#pragma aux finit = \
        ".387"  \
        "finit" \
        "wait"  \
    __parm      [] \
    __value     \
    __modify    []

trap_retval TRAP_CORE( Prog_kill )( void )
{
    prog_kill_ret       *ret;

    _DBG(("AccKillProg\r\n"));
    RedirectFini();
    AtEnd = true;
    if( RealNPXType != X86_NOFPU ) {
        /* mask ALL floating point exception bits */
        finit();
        Mach.msb_87ctrl = 0x37f;
        dbg_wrmsb( &Mach );
    }
    SetMSW( SaveMSW );
    dbg_kill();
    HavePSP = false;
    Mach.msb_event = (USHORT)-1;
    ret = GetOutPtr( 0 );
    ret->err = 0;
    return( sizeof( *ret ) );
}


static opcode_type place_breakpoint( addr48_ptr *addr )
{
    opcode_type old_opcode;

    if( !ReadMemory( addr, &old_opcode, sizeof( old_opcode ) ) ) {
        WriteMemory( addr, &BreakOpcode, sizeof( BreakOpcode ) );
        return( old_opcode );
    }
    return( 0 );
}

static int remove_breakpoint( addr48_ptr *addr, opcode_type old_opcode )
{
    return( WriteMemory( addr, &old_opcode, sizeof( old_opcode ) ) );
}

static trap_conditions MapReturn( void )
{
    switch( Mach.msb_event )
    {
    case 1:
        if( Mach.msb_dreg[6] & DR6_BS ) {
            return( COND_TRACE );
        } else if( Mach.msb_dreg[6] & ( DR6_B0 + DR6_B1 + DR6_B2 + DR6_B3 ) ) {
            return( COND_WATCH );
        } else {
            return( COND_EXCEPTION );
        }
    case 3:
        Mach.msb_eip--;
        return( COND_BREAK );
    case 32:
    case 33:
        AtEnd = true;
        return( COND_TERMINATE );
    case 34: /* new linear base address */
        return( COND_NONE );
    default:
        return( COND_EXCEPTION );
    }
}


static void FixFakeBreak( void )
{
    addr48_ptr  addr;

    addr.segment = Mach.msb_cs;
    addr.offset = Mach.msb_eip;
    remove_breakpoint( &addr, OldFakeOpcode );
}


static trap_conditions Execute( void )
{
    int             err;
    trap_conditions conditions;

    if( !GrabVects() )
        return( COND_EXCEPTION );
    err = RunProgram();
    if( err != 0 ) {
        AtEnd = true;
        return( COND_EXCEPTION );
    }
    if( dbg_rdmsb( &Mach ) != 0 ) {
        AtEnd = true;
        return( COND_EXCEPTION );
    }
    ReleVects();
    conditions = MapReturn();
    if( FakeBreak ) {
        FixFakeBreak();
        FakeBreak = false;
        if( !AtEnd ) {
            conditions = COND_USER;
        }
    }
    if( AtEnd )
        return( COND_TERMINATE );
    return( conditions );
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
    ULONG   linear;
    PTR386  addr;

    addr.offset = offset;
    addr.selector = segment;
    linear = 0;
    dbg_ptolin( &addr, &linear );
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

    _DBG(("AccSetWatch\r\n"));
    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    ret->multiplier = 10000;
    ret->err = 1;       // failure
    if( WatchCount < MAX_WATCHES ) {
        ret->err = 0;   // OK
        wp = WatchPoints + WatchCount;
        wp->addr.segment = acc->watch_addr.segment;
        wp->addr.offset = acc->watch_addr.offset;
        wp->size = acc->size;
        wp->value = 0;
        ReadMemory( &wp->addr, &wp->value, wp->size );

        wp->dregs = GetDRInfo( wp->addr.segment, wp->addr.offset, wp->size, &wp->linear );

        WatchCount++;
        if( DRegsCount() <= 4 ) {
            ret->multiplier |= USING_DEBUG_REG;
        }
    }
    return( sizeof( *ret ) );
}

trap_retval TRAP_CORE( Clear_watch )( void )
{
    /* assume all watches removed at same time */
    WatchCount = 0;
    return( 0 );
}

trap_retval TRAP_CORE( Set_break )( void )
{
    set_break_req       *acc;
    set_break_ret       *ret;

    _DBG(("AccSetBreak\r\n"));
    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    ret->old = place_breakpoint( &acc->break_addr );
    return( sizeof( *ret ) );
}


trap_retval TRAP_CORE( Clear_break )( void )
{
    clear_break_req     *acc;

    _DBG(("AccRestoreBreak\r\n"));
    acc = GetInPtr( 0 );
    remove_breakpoint( &acc->break_addr, acc->old );
    return( 0 );
}

static void SetDRn( int dr, dword linear, word type )
{
    Mach.msb_dreg[dr] = linear;
    Mach.msb_dreg[7] |= ( (dword)type << DR7_RWLSHIFT( dr ) )
                       | ( DR7_GEMASK << DR7_GLSHIFT( dr ) );
}


static bool SetDebugRegs( void )
{
    int         i;
    int         j;
    int         dr;
    watch_point *wp;
    dword       linear;
    word        size;
    word        type;

    if( DRegsCount() > 4 )
        return( false );
    dr = 0;
    Mach.msb_dreg[7] = DR7_GE;
    for( wp = WatchPoints, i = WatchCount; i-- > 0; ++wp ) {
        linear = wp->linear;
        size = wp->size;
        if( size == 8 )
            size = 4;
        type = DRLen( size ) | DR7_BWR;
        for( j = 0; j < wp->dregs; j++ ) {
            SetDRn( dr, linear, type );
            ++dr;
            linear += size;
        }
    }
    return( true );
}

static bool CheckWatchPoints( void )
{
    watch_point *wp;
    int         i;
    uint_64     value;

    for( wp = WatchPoints, i = WatchCount; i-- > 0; wp++ ) {
        value = 0;
        ReadMemory( &wp->addr, &value, wp->size );
        if( wp->value != value ) {
            return( true );
        }
    }
    return( false );
}

static unsigned ProgRun( bool step )
{
    prog_go_ret *ret;

    ret = GetOutPtr( 0 );
    Mach.msb_dreg[6] = 0;
    Mach.msb_dreg[7] = 0;
    if( AtEnd ) {
        ret->conditions = COND_TERMINATE;
    } else if( step ) {
        Mach.msb_eflags |= INTR_TF;
        ret->conditions = Execute();
        Mach.msb_eflags &= ~INTR_TF;
    } else if( WatchCount > 0 ) {
        if( SetDebugRegs() ) {
            ret->conditions = Execute();
            Mach.msb_dreg[6] = 0;
            Mach.msb_dreg[7] = 0;
        } else {
            for( ;; ) {
                Mach.msb_eflags |= INTR_TF;
                ret->conditions = Execute();
                if( ret->conditions & COND_TERMINATE )
                    break;
                if( Mach.msb_event != 1 )
                    break;
                if( ( Mach.msb_dreg[6] & DR6_BS ) == 0 )
                    break;
                if( CheckWatchPoints() ) {
                    ret->conditions = COND_WATCH;
                    Mach.msb_eflags &= ~INTR_TF;
                    break;
                }
            }
        }
    } else {
        ret->conditions = Execute();
    }
    ret->conditions |= COND_CONFIG;
    ret->program_counter.offset = Mach.msb_eip;
    ret->program_counter.segment = Mach.msb_cs;
    ret->stack_pointer.offset = Mach.msb_esp;
    ret->stack_pointer.segment = Mach.msb_ss;
    WatchCount = 0;
    return( sizeof( *ret ) );
}

trap_retval TRAP_CORE( Prog_go )( void )
{
    _DBG(("AccProgGo\r\n"));
    return( ProgRun( false ) );
}

trap_retval TRAP_CORE( Prog_step )( void )
{
    _DBG(("AccProgStep\r\n"));
    return( ProgRun( true ) );
}

trap_retval TRAP_CORE( Get_next_alias )( void )
{
    get_next_alias_ret  *ret;

    _DBG(("AccGetNextAlias\r\n"));
    ret = GetOutPtr( 0 );
    if( !ReportedAlias ) {
        _DBG(("acc->seg == 0\r\n"));
        ret->seg = Mach.msb_cs;
        ret->alias = Mach.msb_ds;
        ReportedAlias = true;
    } else {
        _DBG(("acc->seg == other\r\n"));
        ret->seg = 0;
        ret->alias = 0;
    }
    return( sizeof( *ret ) );
}

trap_retval TRAP_CORE( Get_err_text )( void )
{
    static char *DosErrMsgs[] = {
        #define pick(a,b)   b,
        #include "dosmsgs.h"
        #undef pick
    };
    int                 err;
    get_err_text_req    *acc;
    char                *err_txt;

    _DBG(("AccErrText\r\n"));
    acc = GetInPtr( 0 );
    err_txt = GetOutPtr( 0 );
    if( acc->err < ERR_LAST ) {
        strcpy( err_txt, DosErrMsgs[acc->err] );
    } else {
        err = acc->err - DBE_BASE;
        strcpy( err_txt, DBG_ERROR );
        err_txt += sizeof( DBG_ERROR ) - 1;
        if( err < -DBE_MIN && err > -DBE_MAX ) {
            strcpy( err_txt, DBErrors[err] );
        } else {
            ultoa( acc->err, err_txt, 16 );
        }
    }
    return( strlen( GetOutPtr( 0 ) ) + 1 );
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
    if( Mach.msb_event == (USHORT)-1 ) {
        err_txt[0] = '\0';
    } else {
        if( Mach.msb_event < sizeof( ExceptionMsgs ) / sizeof( ExceptionMsgs[0] ) ) {
            strcpy( err_txt, ExceptionMsgs[Mach.msb_event] );
        } else {
            strcpy( err_txt, TRP_EXC_unknown );
        }
        Mach.msb_event = (USHORT)-1;
    }
    ret->flags = MSG_NEWLINE | MSG_ERROR;
    return( sizeof( *ret ) + strlen( err_txt ) + 1 );
}

trap_retval TRAP_CORE( Get_lib_name )( void )
{
    get_lib_name_ret    *ret;

    ret = GetOutPtr( 0 );
    ret->mod_handle = 0;
    return( sizeof( *ret ) );
}


trap_version TRAPENTRY TrapInit( const char *parms, char *err, bool remote )
{
    trap_version        ver;
    int                 error_num;

    /* unused parameters */ (void)remote; (void)parms;

    _DBG(( "in TrapInit\r\n" ));
    err[0] = '\0'; /* all ok */
    ver.major = TRAP_VERSION_MAJOR;
    ver.minor = TRAP_VERSION_MINOR;
    ver.remote = false;
    //ver.is_32 = true;
    RedirectInit();
    SaveMSW = GetMSW();
    RealNPXType = NPXType();
    _8087 = 0;
    HavePSP = false;
    WatchCount = 0;
    FakeBreak = false;
    BreakOpcode = BRKPOINT;
    if( getenv( "DBDB" ) ) {
        _DBG(( "Calling dbg_edebug()\r\n" ));
        dbg_edebug();
    }
    _DBG(( "about to dbg_init\r\n" ));
    error_num = dbg_init( &SelBlk );
    _DBG(( "back from dbg_init\r\n" ));
    GrabPrtScrn();
    if( error_num != 0 ) {
        strcpy( err, DBG_ERROR );
        strcpy( err + sizeof( DBG_ERROR )-1, DBErrors[-error_num] );
    }
    Mach.msb_event = (USHORT)-1;
    _DBG(( "outof TrapInit\r\n" ));
    return( ver );
}

void TRAPENTRY TrapFini( void )
{
    RelePrtScrn();
    dbg_exit();
}
