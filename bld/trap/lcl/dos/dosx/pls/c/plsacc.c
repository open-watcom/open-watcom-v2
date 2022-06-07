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
* Description:  Trap file for debugging PharLap DOS extended apps.
*
****************************************************************************/


#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <io.h>
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
#include "cpuglob.h"
#include "x86cpu.h"
#include "miscx87.h"
#include "dosredir.h"
#include "doscomm.h"
#include "clibint.h"
#include "trperr.h"
#include "doserr.h"
#include "plsintr.h"


#ifdef DEBUG_TRAP
#define _DBG( x )  cputs x
#else
#define _DBG( x )
#endif

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

extern  long            _STACKTOP;

#pragma aux (metaware)  _dil_global;
#pragma aux (metaware)  _dx_config_inf;


bool                    FakeBreak;
bool                    AtEnd;
bool                    ReportedAlias;
char                    SavedByte;
short                   InitialSS;
short                   InitialCS;

static unsigned         SaveMSW;
static unsigned_8       RealNPXType;
char                    UtilBuff[BUFF_SIZE];
static MSB              Mach;
static SEL_REMAP        SelBlk;
static bool             HavePSP;
#define MAX_OBJECTS     128
static unsigned long    ObjOffReloc[MAX_OBJECTS];

#define DBE_BASE        1000

typedef struct watch_point {
    addr48_ptr  addr;
    dword       value;
    dword       linear;
    word        dregs;
    word        len;
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

#define MAX_WATCHES     32

static watch_point WatchPoints[MAX_WATCHES];
static int         WatchCount;

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
    ret->sys.os = DIG_OS_PHARLAP;
    ret->sys.osmajor = _osmajor;
    ret->sys.osminor = _osminor;
    ret->sys.cpu = X86CPUType();
    ret->sys.huge_shift = 12;
    if( HavePSP && !AtEnd ) {
        if( Mach.msb_cr0 & MSW_EM ) {
            ret->sys.fpu = X86_EMU;
        } else {
            ret->sys.fpu = RealNPXType;
        }
    } else {
        ret->sys.fpu = RealNPXType;
    }
    ret->sys.arch = DIG_ARCH_X86;
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
        return( FALSE );
    if( ( desc.arights & AR_CODE ) == AR_CODE )
        return( TRUE );
    if( ( desc.arights & AR_DATA ) == AR_DATA )
        return( TRUE );
    return( FALSE );
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

static ULONG RealAddr( PTR386 *addr )
{
    return( ( (long)addr->selector << 16 ) + ( addr->offset & 0xFFFF ) );
}


static int RunProgram()
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


static int _ReadMemory( PTR386 *addr, unsigned long req, void *buf )
{
    if( IsProtSeg( addr->selector ) ) {
        return( dbg_pread( addr, req, buf ) );
    } else {
        return( dbg_rread( RealAddr( addr ), req, buf ) );
    }
}

static unsigned short ReadMemory( PTR386 *addr, char *buff, unsigned short requested )
{
    int                 err;
    unsigned short      len;

    err = _ReadMemory( addr, (unsigned long)requested, buff );
    if( err == 0 ) {
        addr->offset += requested;
        return( requested );
    }
    for( len = requested; len != 0; --len ) {
        if( _ReadMemory( addr, 1UL, buff++ ) != 0 )
            break;
        addr->offset++;
    }
    return( requested - len );
}


static int _WriteMemory( PTR386 *addr, unsigned long req, void *buf )
{
    if( IsProtSeg( addr->selector ) ) {
        return( dbg_pwrite( addr, req, buf ) );
    } else {
        return( dbg_rwrite( RealAddr( addr ), req, buf ) );
    }
}

static unsigned short WriteMemory( PTR386 *addr, char *buff, unsigned short requested )
{
    int                 err;
    unsigned short      len;

    err = _WriteMemory( addr, (unsigned long)requested, buff );
    if( err == 0 ) {
        addr->offset += requested;
        return( requested );
    }
    for( len = requested; len != 0; --len ) {
        if( _WriteMemory( addr, 1UL, buff++ ) != 0 )
            break;
        addr->offset++;
    }
    return( requested - len );
}


trap_retval TRAP_CORE( Checksum_mem )( void )
{
    unsigned short      len;
    unsigned short      read;
    PTR386              addr;
    int                 i;
    checksum_mem_req    *acc;
    checksum_mem_ret    *ret;

    _DBG(("AccChkSum\r\n"));
    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    len = acc->len;
    addr.offset = acc->in_addr.offset;
    addr.selector = acc->in_addr.segment;
    ret->result = 0;
    while( len >= BUFF_SIZE ) {
        read = ReadMemory( &addr, UtilBuff, BUFF_SIZE );
        for( i = 0; i < read; ++i ) {
            ret->result += UtilBuff[i];
        }
        if( read != BUFF_SIZE )
            return( sizeof( *ret ) );
        len -= BUFF_SIZE;
    }
    if( len != 0 ) {
        read = ReadMemory( &addr, UtilBuff, len );
        if( read == len ) {
            for( i = 0; i < len; ++i ) {
                ret->result += UtilBuff[i];
            }
        }
    }
    return( sizeof( *ret ) );
}


trap_retval TRAP_CORE( Read_mem )( void )
{
    PTR386            addr;
    read_mem_req        *acc;
    void                *ret;
    unsigned            len;

    _DBG(("ReadMem\r\n"));
    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    addr.offset = acc->mem_addr.offset;
    addr.selector = acc->mem_addr.segment;
    len = ReadMemory( &addr, ret, acc->len );
    return( len );
}

trap_retval TRAP_CORE( Write_mem )( void )
{
    PTR386              addr;
    write_mem_req       *acc;
    write_mem_ret       *ret;

    _DBG(("WriteMem\r\n"));
    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    addr.offset = acc->mem_addr.offset;
    addr.selector = acc->mem_addr.segment;
    ret->len = WriteMemory( &addr, GetInPtr( sizeof( *acc ) ), GetTotalSizeIn() - sizeof( *acc ) );
    return( sizeof( *ret ) );
}

trap_retval TRAP_CORE( Read_io )( void )
{
    int                 err;
    read_io_req         *acc;
    void                *ret;
    unsigned            len;

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
        len = 0;
    } else {
        len = acc->len;
    }
    return( len );
}


trap_retval TRAP_CORE( Write_io )( void )
{
    int             err;
    int             len;
    write_io_req    *acc;
    write_io_ret    *ret;
    void            *data;

    _DBG(("WritePort\r\n"));
    acc = GetInPtr( 0 );
    data = GetInPtr( sizeof( *acc ) );
    len = GetTotalSizeIn() - sizeof( *acc );
    ret = GetOutPtr( 0 );
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
    if( err != 0 ) {
        ret->len = 0;
    } else {
        ret->len = len;
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
    unsigned_32         off;
    unsigned            i;
    pe_object           obj;
    union {
        dos_exe_header  dos;
        pe_header       pe;
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
    read( handle, &head.pe, sizeof( head.pe ) );
    switch( head.pe.signature ) {
    case PE_SIGNATURE:
    case PL_SIGNATURE:
        for( i = 0; i < head.pe.num_objects; ++i ) {
            read( handle, &obj, sizeof( obj ) );
            ObjOffReloc[i] = Mach.msb_eip - head.pe.entry_rva + obj.rva;
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
    char            ch;
    char            *src;
    char            *name;
    prog_load_ret   *ret;
    unsigned        len;

    _DBG(("AccLoadProg\r\n"));
    memset( ObjOffReloc, 0, sizeof( ObjOffReloc ) );
    AtEnd = FALSE;
    ReportedAlias = FALSE;
    ret = GetOutPtr( 0 );
    src = name = GetInPtr( sizeof( prog_load_req ) );
    while( *src++ != '\0' )
        {}
    len = GetTotalSizeIn() - sizeof( prog_load_req ) - ( src - name );
    if( len > 126 )
        len = 126;
    UtilBuff[0] = MergeArgvArray( src, UtilBuff + 1, len )
    ret->err = map_dbe( dbg_load( (unsigned char *)name, NULL, (unsigned char *)UtilBuff ) );
    if( ret->err == 0 ) {
        HavePSP = TRUE;
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
    Mach.msb_event = -1;
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
    ret = GetOutPtr( 0 );
    RedirectFini();
    AtEnd = TRUE;
    if( RealNPXType != X86_NO ) {
        /* mask ALL floating point exception bits */
        finit();
        Mach.msb_87ctrl = 0x37f;
        dbg_wrmsb( &Mach );
    }
    SetMSW( SaveMSW );
    dbg_kill();
    HavePSP = FALSE;
    ret->err = 0;
    Mach.msb_event = -1;
    return( sizeof( *ret ) );
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
        AtEnd = TRUE;
        return( COND_TERMINATE );
    case 34: /* new linear base address */
        return( 0 );
    default:
        return( COND_EXCEPTION );
    }
}


static void FixFakeBreak( void )
{
    PTR386      addr;

    addr.selector = Mach.msb_cs;
    addr.offset = Mach.msb_eip;
    WriteMemory( &addr, (char *)&SavedByte, 1 );
}


static trap_conditions Execute( void )
{
    int             err;
    trap_conditions conditions;

    if( !GrabVects() )
        return( COND_EXCEPTION );
    err = RunProgram();
    if( err != 0 ) {
        AtEnd = TRUE;
        return( COND_EXCEPTION );
    }
    if( dbg_rdmsb( &Mach ) != 0 ) {
        AtEnd = TRUE;
        return( COND_EXCEPTION );
    }
    ReleVects();
    conditions = MapReturn();
    if( FakeBreak ) {
        FixFakeBreak();
        FakeBreak = FALSE;
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

trap_retval TRAP_CORE( Set_watch )( void )
{
    dword           l;
    watch_point     *curr;
    set_watch_req   *acc;
    set_watch_ret   *ret;
    ULONG           linear;
    PTR386          addr;

    _DBG(("AccSetWatch\r\n"));
    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    ret->err = 1;
    ret->multiplier = 10000;
    if( WatchCount < MAX_WATCHES ) {
        ret->err = 0;
        addr.offset = acc->watch_addr.offset;
        addr.selector = acc->watch_addr.segment;
        _ReadMemory( &addr, 4UL, &l );
        curr = WatchPoints + WatchCount;
        curr->addr.segment = acc->watch_addr.segment;
        curr->addr.offset = acc->watch_addr.offset;
        curr->value = l;
        dbg_ptolin( &addr, &linear );
        curr->linear = linear;
        curr->len = acc->size;
        curr->linear &= ~( curr->len - 1 );
        curr->dregs = ( linear & ( curr->len - 1 ) ) ? 2 : 1;
        ++WatchCount;
        if( DRegsCount() <= 4 ) {
            ret->multiplier |= USING_DEBUG_REG;
        }
    }
    return( sizeof( *ret ) );
}

trap_retval TRAP_CORE( Clear_watch )( void )
{
    WatchCount = 0;
    return( 0 );
}

trap_retval TRAP_CORE( Set_break )( void )
{
    opcode_type         brk_opcode;
    set_break_req       *acc;
    set_break_ret       *ret;
    PTR386              addr;

    _DBG(("AccSetBreak\r\n"));
    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    addr.offset = acc->break_addr.offset;
    addr.selector = acc->break_addr.segment;
    _ReadMemory( &addr, sizeof( brk_opcode ), &brk_opcode );
    ret->old = brk_opcode;
    brk_opcode = BRKPOINT;
    _WriteMemory( &addr, sizeof( brk_opcode ), &brk_opcode );
    return( sizeof( *ret ) );
}


trap_retval TRAP_CORE( Clear_break )( void )
{
    opcode_type         brk_opcode;
    clear_break_req     *acc;
    PTR386              addr;

    _DBG(("AccRestoreBreak\r\n"));
    acc = GetInPtr( 0 );
    addr.offset = acc->break_addr.offset;
    addr.selector = acc->break_addr.segment;
    brk_opcode = acc->old;
    _WriteMemory( &addr, sizeof( brk_opcode ), &brk_opcode );
    return( 0 );
}

static void SetDRnBW( int dr, ULONG linear, int len ) /* Set DRn for break on write */
{
    Mach.msb_dreg[dr] = linear;
    Mach.msb_dreg[7] |= ( (DRLen( len )+DR7_BWR) << DR7_RWLSHIFT( dr ) )
                       |  ( DR7_GEMASK << DR7_GLSHIFT( dr ) );
}


static bool SetDebugRegs()
{
    int         i;
    int         dr;
    watch_point *wp;

    if( DRegsCount() > 4 )
        return( FALSE );
    dr = 0;
    Mach.msb_dreg[7] = DR7_GE;
    for( i = WatchCount, wp = WatchPoints; i != 0; --i, ++wp ) {
        SetDRnBW( dr, wp->linear, wp->len );
        ++dr;
        if( wp->dregs == 2 ) {
            SetDRnBW( dr, wp->linear + wp->len, wp->len );
            ++dr;
        }
    }
    return( TRUE );
}


static unsigned ProgRun( bool step )
{
    watch_point *wp;
    int         i;
    dword       value;
    prog_go_ret *ret;
    PTR386      addr;

    ret = GetOutPtr( 0 );
    Mach.msb_dreg[6] = 0;
    Mach.msb_dreg[7] = 0;
    if( AtEnd ) {
        ret->conditions = COND_TERMINATE;
    } else if( step ) {
        Mach.msb_eflags |= EF_TF;
        ret->conditions = Execute();
        Mach.msb_eflags &= ~EF_TF;
    } else if( WatchCount != 0 ) {
        if( SetDebugRegs() ) {
            ret->conditions = Execute();
            Mach.msb_dreg[6] = 0;
            Mach.msb_dreg[7] = 0;
        } else {
            for( ;; ) {
                Mach.msb_eflags |= EF_TF;
                ret->conditions = Execute();
                if( ret->conditions & COND_TERMINATE )
                    break;
                if( Mach.msb_event != 1 )
                    break;
                if( ( Mach.msb_dreg[6] & DR6_BS ) == 0 )
                    break;
                for( wp = WatchPoints, i = WatchCount; i > 0; ++wp, --i ) {
                    addr.offset = wp->addr.offset;
                    addr.selector = wp->addr.segment;
                    _ReadMemory( &addr, 4UL, &value );
                    if( value != wp->value ) {
                        ret->conditions = COND_WATCH;
                        Mach.msb_eflags &= ~EF_TF;
                        goto leave;
                    }
                }
            }
        }
    } else {
        ret->conditions = Execute();
    }
leave:
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
    return( ProgRun( FALSE ) );
}

trap_retval TRAP_CORE( Prog_step )( void )
{
    _DBG(("AccProgStep\r\n"));
    return( ProgRun( TRUE ) );
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
        ReportedAlias = TRUE;
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
        Mach.msb_event = -1;
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

    _DBG(( "in TrapInit\r\n" ));
    remote = remote; parms = parms;
    err[0] = '\0'; /* all ok */
    ver.major = TRAP_MAJOR_VERSION;
    ver.minor = TRAP_MINOR_VERSION;
    ver.remote = FALSE;
    //ver.is_32 = TRUE;
    RedirectInit();
    SaveMSW = GetMSW();
    RealNPXType = NPXType();
    _8087 = 0;
    HavePSP = FALSE;
    WatchCount = 0;
    FakeBreak = FALSE;
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
    Mach.msb_event = -1;
    _DBG(( "outof TrapInit\r\n" ));
    return( ver );
}

void TRAPENTRY TrapFini( void )
{
    RelePrtScrn();
    dbg_exit();
}
