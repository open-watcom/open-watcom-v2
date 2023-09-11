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


#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <io.h>
#include <dos.h>
#include <fcntl.h>
#include <stdarg.h>
#include "bool.h"
#include "tinyio.h"
#include "trpimp.h"
#include "trpcomm.h"
#include "packet.h"
#include "dbg386.h"
#include "ioports.h"
#include "adslib.h"
#include "madregs.h"
#include "x86cpu.h"
#include "miscx87.h"
#include "dosredir.h"
#include "trperr.h"
#include "doserr.h"
#include "doscomm.h"
#include "brkptcpu.h"
#include "adsintr.h"
#include "adsacc.h"
#include "int16.h"
#include "segmcpu.h"

#include "clibint.h"


#define GET_LINEAR(s,o) (SegBase( (s) ) + (o))

typedef struct watch_point {
    uint_64     value;
    dword       linear;
    addr48_ptr  addr;
    word        size;
    word        dregs;
} watch_point;

static dword    SegLimit( dword sel );
#pragma aux SegLimit = \
        "lsl eax,eax" \
        "jz short L1" \
        "xor eax,eax" \
    "L1: " \
    __parm [__eax] \
    __value [__eax]

static dword SegBase( dword sel );
#pragma aux SegBase = \
        "mov ax,2508H" \
        "int 21h" \
    __parm [__ebx] __value [__ecx] __modify [__eax]

static bool     WriteOK( word sel );
#pragma aux WriteOK = \
        "verw ax" /* if ok for write */\
        "sete al" /* then return true */\
    __parm [__ax] __value [__al]

static bool     ReadOK( word sel );
#pragma aux ReadOK = \
        "verr ax" /* if ok for read */\
        "sete al" /* then return true */\
    __parm [__ax] __value [__al]

static void     DoReadBytes( word sel, dword offs, void *data, size_t len );
#pragma aux DoReadBytes = \
        "push es" \
        "mov  eax,ds" \
        "mov  es,eax" \
        "mov  ds,edx" \
        "rep movsb" \
        "mov  ds,eax" \
        "pop  es" \
    __parm [__dx] [__esi] [__edi] [__ecx] \
    __modify [__eax]

static void     DoWriteBytes( word sel, dword offs, void *data, size_t len );
#pragma aux DoWriteBytes = \
        "push es" \
        "mov es,edx" \
        "rep movsb" \
        "pop es" \
    __parm [__dx] [__edi] [__esi] [__ecx]

static unsigned GetFL( void );
#pragma aux GetFL = \
        "pushfd" \
        "pop eax" \
    __parm [] __value[__eax]

static void GetSysRegs( void * );
#pragma aux GetSysRegs = \
        "mov    ax,2535h" \
        "mov    ebx,0" \
        "int    21h" \
    __parm [__edx] __modify[__eax __ebx]

static void SetSysRegs( void * );
#pragma aux SetSysRegs = \
        "mov    ax,2535h" \
        "mov    ebx,1" \
        "int    21h" \
    __parm [__edx] __modify[__eax __ebx]

trap_cpu_regs           Regs;
int                     IntNum;
char                    Break;

bool                    FakeBreak;
bool                    AtEnd;
bool                    DoneAutoCAD;

static byte             RealNPXType;
char                    UtilBuff[BUFF_SIZE];
#define ADSSTACK_SIZE   4096
char                    ADSStack[ADSSTACK_SIZE];

#define _DBG3( x ) // MyOut x
#define _DBG2( x ) // MyOut x
#define _DBG1( x ) // MyOut x
#define _DBG0( x ) // MyOut x
#define _DBG( x )  // MyOut x

struct {
    dword   cr0;
    dword   dr0;
    dword   dr1;
    dword   dr2;
    dword   dr3;
    dword   res1;
    dword   res2;
    dword   dr6;
    dword   dr7;
    dword   res3;
    dword   res4;
    dword   res5;
} SysRegs;

#define MAX_WATCHES     8

static watch_point  WatchPoints[MAX_WATCHES];
static int          WatchCount;

static opcode_type  BreakOpcode;

#if 0
/*
 * Debugging output code for ADS
 */

extern void     InitMeg1( void );
extern short    Meg1;

int             _cnt;

static int      _line = 0;

static char __far *GetScreenPointer( void )
{
    InitMeg1();
    return( _MK_FP( Meg1, 0xB0000 ) );
}

void MyClearScreen( void )
{
    size_t  i;

    char __far *scrn = GetScreenPointer();

    for( i = 0; i < 80 * 25; i++ ) {
        scrn[i * 2] = ' ';
        scrn[i * 2 + 1] = 7;
    }
}

void RawOut( char *str )
{
    size_t      len;
    size_t      i;
    char        __far *scr;
    char        __far *scrn = GetScreenPointer();

    len = strlen( str );
    if( len > 80 )
        len = 80;
    scr = &scrn[_line * 80 * 2];
    i = 0;
    while( i < len ) {
        scr[i * 2] = str[i];
        scr[i * 2 + 1] = 7;
        i++;
    }
    while( i < 80 ) {
        scr[i * 2] = ' ';
        scr[i * 2 + 1] = 7;
        i++;
    }
    _line++;
    if( _line > 24 )
        _line = 0;

    scr = &scrn[_line * 80 * 2];
    for( i = 0; i < 80; i++ ) {
        scr[i * 2] = ' ';
        scr[i * 2 + 1] = 7;
    }
}

void MyOut( char *str, ... )
{
    va_list     args;
    char        tmpbuff[128];

    sprintf( tmpbuff, "%03d) ", ++_cnt );
    va_start( args, str );
    vsprintf( &tmpbuff[5], str, args );
    va_end( args );

    RawOut( tmpbuff );
}

#endif

int SetUsrTask( void )
{
    return( 1 );
}

void SetDbgTask( void )
{
}

static  word    LookUp( word sdtseg, word seg, bool global )
{
    dword       sdtoff;
    dword       sdtlim;
    dword       linear;
    word        otherseg;

    sdtlim = SegLimit( sdtseg );
    linear = GET_LINEAR( seg, 0 );
    for( sdtoff = 0; sdtoff < sdtlim; sdtoff += 8 ) {
        if( sdtoff == ( seg & 0xfff8 ) )
            continue;
        if( global ) {
            otherseg = sdtoff;
        } else {
            otherseg = sdtoff + 4;
        }
        if( !WriteOK( otherseg ) )
            continue;
        if( GET_LINEAR( otherseg, 0 ) != linear )
            continue;
        _DBG3(("lookup %4.4x", otherseg));
        return( otherseg );
    }
    return( 0 );
}


static word    AltSegment( word seg )
{
    word        otherseg;

    otherseg = LookUp( 0x30, seg, false );  /* try LDT */
    if( otherseg != 0 )
        return( otherseg );
    otherseg = LookUp( 0x38, seg, true );  /* try GDT */
    if( otherseg != 0 )
        return( otherseg );
    return( seg );
}

static size_t ReadWrite( bool (*r)(addr48_ptr *, void *, size_t), addr48_ptr *addr, void *data, size_t req_len ) {

    size_t      len;

    _DBG2(("Read Write %4.4x:%8.8lx", addr->segment, addr->offset));
    if( !r( addr, data, req_len ) ) {
        _DBG2(("Read Write Done"));
        addr->offset += req_len;
        return( req_len );
    }
    _DBG2(("Read Write One byte at a time for %d", req_len));
    for( len = req_len; len > 0; --len ) {
        if( r( addr, data, 1 ) ) {
            _DBG3(("failed for %4.4x:%8.8lx", addr->segment, addr->offset));
            break;
        }
        addr->offset++;
        data = (char *)data + 1;
    }
    _DBG2(("Read Write Done"));
    return( req_len - len );
}

static bool ReadMemory( addr48_ptr *addr, void *data, size_t len )
{
    addr_seg    segment;

    segment = addr->segment;
    if( !ReadOK( segment ) ) {
        segment = AltSegment( segment );
    }
    if( SegLimit( segment ) >= addr->offset + len - 1 ) {
        DoReadBytes( segment, addr->offset, data, len );
        return( false );
    }
    return( true );
}

static bool WriteMemory( addr48_ptr *addr, void *data, size_t len )
{
    addr_seg    segment;

    segment = addr->segment;
    if( segment == Regs.CS )
        segment = Regs.DS; // hack, ack
    if( !WriteOK( segment ) ) {
        segment = AltSegment( segment );
    }
    if( SegLimit( segment ) >= addr->offset + len - 1 ) {
        DoWriteBytes( segment, addr->offset, data, len );
        return( false );
    }
    return( true );
}

trap_retval TRAP_CORE( Get_sys_config )( void )
{
    get_sys_config_ret  *ret;

    _DBG1(( "AccGetConfig" ));
    ret = GetOutPtr( 0 );
    ret->os = DIG_OS_AUTOCAD;
    ret->osmajor = _osmajor;
    ret->osminor = _osminor;
    ret->cpu = X86CPUType();
    ret->huge_shift = 12;
    ret->fpu = RealNPXType;
    ret->arch = DIG_ARCH_X86;
    return( sizeof( *ret ) );
}


trap_retval TRAP_CORE( Map_addr )( void )
{
    map_addr_req        *acc;
    map_addr_ret        *ret;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    if( acc->in_addr.segment == MAP_FLAT_DATA_SELECTOR ) {
        ret->out_addr.segment = Regs.DS;
    } else {
        ret->out_addr.segment = Regs.CS;
    }
    ret->out_addr.offset = acc->in_addr.offset;
    ret->lo_bound = 0;
    ret->hi_bound = ~(addr48_off)0;
    return( sizeof( *ret ) );
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
        data->x86_addr_flags = X86AC_BIG;
        return( sizeof( *ret ) + sizeof( data->x86_addr_flags ) );
    }
    return( sizeof( *ret ) );
}


trap_retval TRAP_CORE( Checksum_mem )( void )
{
    size_t              len;
    size_t              want;
    size_t              i;
    size_t              got;
    checksum_mem_req    *acc;
    checksum_mem_ret    *ret;
    unsigned long       sum;

    _DBG1(( "AccChkSum" ));
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
    read_mem_req        *acc;

    _DBG1(( "ReadMem" ));
    acc = GetInPtr( 0 );
    return( ReadWrite( ReadMemory, &acc->mem_addr, GetOutPtr( 0 ), acc->len ) );
}

trap_retval TRAP_CORE( Write_mem )( void )
{
    write_mem_req       *acc;
    write_mem_ret       *ret;

    _DBG1(( "WriteMem" ));
    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    ret->len = ReadWrite( WriteMemory, &acc->mem_addr, GetInPtr( sizeof( *acc ) ),
                            GetTotalSizeIn() - sizeof( *acc ) );
    return( sizeof( *ret ) );
}

trap_retval TRAP_CORE( Read_io )( void )
{
    read_io_req *acc;
    void        *ret;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    switch( acc->len ) {
    case 1:
        *(byte *)ret = In_b( acc->IO_offset );
        break;
    case 2:
        *(word *)ret = In_w( acc->IO_offset );
        break;
    case 4:
        *(dword *)ret = In_d( acc->IO_offset );
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

trap_retval TRAP_CORE( Read_regs )( void )
{
    mad_registers       *mr;

    mr = GetOutPtr( 0 );
    mr->x86.cpu = *(struct x86_cpu *)&Regs;
    Read387( &mr->x86.u.fpu );
    return( sizeof( mr->x86 ) );
}

trap_retval TRAP_CORE( Write_regs )( void )
{
    mad_registers       *mr;

    mr = GetInPtr( sizeof( write_regs_req ) );
    *(struct x86_cpu *)&Regs = mr->x86.cpu;
    Write387( &mr->x86.u.fpu );
    return( 0 );
}

static void ADSLoop( void )
{
    short scode = RSRSLT;             /* Normal result code (default) */
    int stat;

    for( ;; ) {                       /* Request/Result loop */
//        _DBG0(("ADS Loop"));
        if( (stat = ads_link( scode )) < 0 ) {
            cputs( "ADSHELP: bad status from ads_link()\r\n" );
            exit( 1 );
        }

        scode = RSRSLT;               /* Reset result code */
        switch( stat ) {
        case RQSUBR:                  /* Handle external function requests */
            scode = RSERR;
            break;
        case RQXUNLD:                 /* Handle external function requests */
            DoneAutoCAD = true;
            AtEnd = true;
            BreakPoint();             /* you figure it out */
            break;
        }
    }
}


trap_retval TRAP_CORE( Prog_load )( void )
{
    prog_load_ret       *ret;

    _DBG(("We're in AccLoadProg"));
    GetSysRegs( &SysRegs );
    ret = GetOutPtr( 0 );
    ret->err = 0;
    ret->flags = LD_FLAG_IS_BIG | LD_FLAG_IS_PROT | LD_FLAG_IS_STARTED | LD_FLAG_DISPLAY_DAMAGED;
    ret->task_id = 4;
    ret->mod_handle = 0;
    Regs.DS = Regs.ES = Regs.FS = Regs.GS = GetDS();
    Regs.SS = GetSS();
    Regs.ESP = (dword)ADSStack + ADSSTACK_SIZE - sizeof( dword );
    Regs.CS = GetCS();
    Regs.EIP = (dword)&ADSLoop;
    Regs.EFL = GetFL();
    Regs.EAX = Regs.EBX = Regs.ECX = Regs.EDX = Regs.ESI = 0;
    Regs.EDI = Regs.EBP = 0;
    IntNum = -1;
    _DBG(("We're back from AccLoadProg"));
    return( sizeof( *ret ) );
}

void DumpRegs( trap_cpu_regs *regs )
{
    /* unused parameters */ (void)regs;

    _DBG0(("EAX=%8.8x EBX=%8.8x ECX=%8.8x EDX=%8.8x",
    regs->EAX, regs->EBX, regs->ECX, regs->EDX ));
    _DBG0(("ESI=%8.8x EDI=%8.8x ESP=%8.8x EBP=%8.8x",
    regs->ESI, regs->EDI, regs->ESP, regs->EBP ));
    _DBG0(("DS=%4.4x ES=%4.4x FS=%4.4x GS=%4.4x",
    regs->DS, regs->ES, regs->FS, regs->GS ));
    _DBG0(("CS=%4.4x EIP=%8.8x EFL=%8.8x SS=%4.4x",
    regs->CS, regs->EIP, regs->EFL, regs->SS ));
}

static void MyRunProg( void )
{
    _DBG0(( "RunProg - Regs" ));
    DumpRegs( &Regs );
    _DBG0(( "RunProg - dbgregs" ));
    SetSysRegs( &SysRegs );
    DoRunProg();
    GetSysRegs( &SysRegs );
    _DBG0(( "AfterRunProg (%d) - Regs", IntNum ));
    DumpRegs( &Regs );
    _DBG0(( "AfterRunProg - dbgregs" ));
    DumpDbgRegs();
}

trap_retval TRAP_CORE( Prog_kill )( void )
{
    prog_kill_ret       *ret;

    _DBG1(( "AccKillProg" ));
    RedirectFini();
    AtEnd = true;
    if( !DoneAutoCAD ) {
        AtEnd = true;
        cputs( "*** Please quit AUTOCAD in order to restart debugger ***\r\n" );
        MyRunProg();
    }
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

    linear = GET_LINEAR( segment, offset );
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
    int             needed;

    _DBG0(( "AccSetWatch" ));
    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    ret->multiplier = 20000;
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
        needed = DRegsCount();
        if( needed <= 4 ) {
            ret->multiplier |= USING_DEBUG_REG;
        }
        _DBG0(("addr %4.4x:%8.8lx " "linear %8.8x " "len %d " "needed %d ",
               wp->addr.segment, wp->addr.offset, wp->linear, wp->size, wp->dreg));
    }
    return( sizeof( *ret ) );
}

trap_retval TRAP_CORE( Clear_watch )( void )
{
    _DBG0(( "AccRestoreWatch" ));
    /* assume all watches removed at same time */
    WatchCount = 0;
    return( 0 );
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

trap_retval TRAP_CORE( Set_break )( void )
{
    set_break_req       *acc;
    set_break_ret       *ret;

    _DBG1(( "AccSetBreak" ));
    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    ret->old = place_breakpoint( &acc->break_addr );
    return( sizeof( *ret ) );
}

trap_retval TRAP_CORE( Clear_break )( void )
{
    clear_break_req     *acc;

    acc = GetInPtr( 0 );
    remove_breakpoint( &acc->break_addr, acc->old );
    _DBG1(( "AccRestoreBreak" ));
    return( 0 );
}

static dword *DR[] = { &SysRegs.dr0, &SysRegs.dr1, &SysRegs.dr2, &SysRegs.dr3 };

static void SetDRn( int dr, dword linear, word type )
{
    *DR[dr] = linear;
    SysRegs.dr7 |= ( (dword)type << DR7_RWLSHIFT( dr ) )
                     | ( DR7_GEMASK << DR7_GLSHIFT( dr ) );
}


static bool SetDebugRegs( void )
{
    watch_point *wp;
    int         i;
    int         j;
    int         dr;
    dword       linear;
    word        size;
    word        type;

    if( DRegsCount() > 4 )
        return( false );
    dr = 0;
    SysRegs.dr7 = DR7_GE;
    for( wp = WatchPoints, i = WatchCount; i-- > 0; wp++ ) {
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
    long        trace;
    prog_go_ret *ret;

    _DBG1(( "ProgRun" ));
    ret = GetOutPtr( 0 );
    ret->conditions = COND_CONFIG;
    trace = step ? INTR_TF : 0;
    Regs.EFL |= trace;
    if( AtEnd ) {
        _DBG2(("No RunProg"));
    } else if( !trace && WatchCount > 0 ) {
        _DBG2(("All that trace goop"));
        if( SetDebugRegs() ) {
            MyRunProg();
            SysRegs.dr6 = 0;
            SysRegs.dr7 = 0;
        } else {
            for( ;; ) {
                Regs.EFL |= INTR_TF;
                MyRunProg();
                if( DoneAutoCAD )
                    break;
                if( IntNum != 1 )
                    break;
                if( ( SysRegs.dr6 & DR6_BS ) == 0 )
                    break;
                if( CheckWatchPoints() ) {
                    break;
                }
            }
        }
    } else {
        MyRunProg();
    }
    if( AtEnd ) {
        ret->conditions |= COND_TERMINATE;
    } else if( DoneAutoCAD ) {
        ret->conditions = COND_TERMINATE;
        AtEnd = true;
    } else if( IntNum == 1 ) {
        if( trace ) {
            ret->conditions |= COND_TRACE;
        } else {
            ret->conditions |= COND_WATCH;
        }
    } else if( IntNum == 3 ) {
        ret->conditions |= COND_BREAK;
        Regs.EIP--;
    } else {
        ret->conditions |= COND_EXCEPTION;
    }
    Regs.EFL &= ~trace;
    ret->program_counter.offset = Regs.EIP;
    ret->program_counter.segment = Regs.CS;
    ret->stack_pointer.offset = Regs.ESP;
    ret->stack_pointer.segment = Regs.SS;
    WatchCount = 0;
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

#if 0
static void DOSEnvLkup( char *src, char *dst )
{
    char    *p;

    p = getenv( src );
    if( p == NULL ) {
        *dst = '\0';
    } else {
        strcpy( dst, p );
    }
}
#endif

trap_retval TRAP_CORE( Get_next_alias )( void )
{
    get_next_alias_req  *acc;
    get_next_alias_ret  *ret;

    _DBG(("AccGetNextAlias"));
    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    if( acc->seg == 0 ) {
        _DBG(("acc->seg == 0"));
        ret->seg = Regs.CS;
        ret->alias = Regs.DS;
    } else {
        _DBG(("acc->seg == other"));
        ret->seg = 0;
        ret->alias = 0;
    }
    return( sizeof( *ret ) );
}

#if 0
static unsigned_16 AccReadUserKey( void )
{
    rd_key_return FAR *retblk;

    retblk = GetOutPtr( 0 );
    retblk->key = _BIOSKeyboardGet( KEYB_STD );
    return( sizeof( rd_key_return ) );
}
#endif

trap_retval TRAP_CORE( Get_err_text )( void )
{
    static char *DosErrMsgs[] = {
        #define pick(a,b)   b,
        #include "dosmsgs.h"
        #undef pick
    };
    get_err_text_req    *acc;
    char                *err_txt;

    _DBG1(( "AccErrText" ));
    acc = GetInPtr( 0 );
    err_txt = GetOutPtr( 0 );
    if( acc->err < ERR_LAST ) {
        strcpy( err_txt, DosErrMsgs[acc->err] );
    } else {
        strcpy( err_txt, TRP_ERR_unknown_system_error );
        ultoa( acc->err, err_txt + strlen( err_txt ), 16 );
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
    if( IntNum == -1 ) {
        err_txt[0] = '\0';
    } else {
        if( IntNum < sizeof( ExceptionMsgs ) / sizeof( ExceptionMsgs[0] ) ) {
            strcpy( err_txt, ExceptionMsgs[IntNum] );
        } else {
            strcpy( err_txt, TRP_EXC_unknown );
        }
        IntNum = -1;
    }
    ret->flags = MSG_NEWLINE | MSG_ERROR;
    return( sizeof( *ret ) + strlen( err_txt ) + 1 );
}

trap_version TRAPENTRY TrapInit( const char *parms, char *err, bool remote )
{
    trap_version        ver;

    /* unused parameters */ (void)remote; (void)parms;

    _DBG0(( "TrapInit" ));
    err[0] = '\0'; /* all ok */
    ver.major = TRAP_VERSION_MAJOR;
    ver.minor = TRAP_VERSION_MINOR;
    ver.remote = false;
    RedirectInit();
    RealNPXType = NPXType();
    WatchCount = 0;
    FakeBreak = false;
    BreakOpcode = BRKPOINT;
    GrabVects();
    _DBG0(( "Done TrapInit" ));
    return( ver );
}

void ACADInit( void )
{
    _DBG0( ( "Calling ads_init()" ) );
    ads_init( _argc, _argv );
    _DBG0( ( "After ads_init()" ) );
}

void LetACADDie( void )
{
    if( DoneAutoCAD ) {
        DoneAutoCAD = false;
        MyRunProg();    /* we're history! */
    }
}

void TRAPENTRY TrapFini( void )
{
    _DBG0(( "TrapFini" ));
    ReleVects();
    _DBG0(( "Done TrapFini" ));
}

#if 0
void GotInt3( void )
{
    _DBG0(( "Got Int 3!!!" ));
}
#endif
