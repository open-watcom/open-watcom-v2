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


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <conio.h>
#include <io.h>
#include <dos.h>
#include <fcntl.h>
#include <stdarg.h>

#include "tinyio.h"
#include "trpimp.h"
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
#include "cpuglob.h"

trap_cpu_regs   Regs;
int             IntNum;
char            Break;

extern void DumpDbgRegs(void);


extern  void            GrabVects(void);
extern  void            ReleVects(void);
extern  dword           GetFL(void);
extern  void            DoRunProg(void);
extern  word            DoMapSeg(word);
extern  int             DoReadMem(word,dword,char*);
extern  int             DoWriteMem(word,dword,char*);
extern  dword           GetLinear(word,dword);
extern  dword           SegLimit(word);
extern  bool            WriteOk(word);

bool                    FakeBreak;
bool                    AtEnd;
bool                    DoneAutoCAD;

static unsigned_8       RealNPXType;
#define BUFF_SIZE       256
char                    UtilBuff[BUFF_SIZE];
#define ADSSTACK_SIZE      4096
char                    ADSStack[ADSSTACK_SIZE];

typedef struct watch {
    addr48_ptr  addr;
    dword       value;
    dword       linear;
    short       dregs;
    short       len;
} watch;

#define MAX_WP  8
watch   WatchPoints[ MAX_WP ];
int     WatchCount;

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

extern void GetSysRegs( void * );
extern void SetSysRegs( void * );

#if 0
/*
 * Debugging output code for ADS
 */

int _cnt;
static int _line=0;

extern void InitMeg1();
char __far *GetScreenPointer( void )
{
    extern      short           Meg1;
    InitMeg1();
    return( MK_FP( Meg1, 0xB0000 ) );
}

void MyClearScreen()
{
    int i;

    char __far *scrn = GetScreenPointer();

    for( i=0;i<80*25;i++ ) {
        scrn[i*2] = ' ';
        scrn[i*2+1] = 7;
    }
}

void RawOut( char *str )
{
    int         len,i;
    char        __far *scr;
    char        __far *scrn = GetScreenPointer();

    len = strlen( str );
    scr = &scrn[_line*80*2];
    for( i=0;i<len;i++ ) {
        scr[i*2] = str[i];
        scr[i*2+1] = 7;
    }
    for( i=len;i<80;i++ ) {
        scr[i*2] = ' ';
        scr[i*2+1] = 7;
    }
    _line++;
    if( _line > 24 ) _line = 0;

    scr = &scrn[_line*80*2];
    for( i=0;i<80;i++ ) {
        scr[i*2] = ' ';
        scr[i*2+1] = 7;
    }
}

void MyOut( char *str, ... )
{
    va_list     al;
    char        tmpbuff[128];

    sprintf( tmpbuff,"%03d) ",++_cnt );
    va_start( al, str );
    vsprintf( &tmpbuff[5],str, al );

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

static  word    LookUp( word sdtseg, word seg, word global )
{
    dword       sdtoff;
    dword       sdtlim;
    dword       linear;
    word        otherseg;

    sdtlim = SegLimit( sdtseg );
    linear = GetLinear( seg, 0 );
    for( sdtoff = 0; sdtoff < sdtlim; sdtoff += 8 ) {
        if( sdtoff == ( seg & 0xfff8 ) ) continue;
        otherseg = sdtoff + ( global ? 0 : 4 );
        if( !WriteOk( otherseg ) ) continue;
        if( GetLinear( otherseg, 0 ) != linear ) continue;
                                                                          _DBG3(("lookup %4.4x", otherseg));
        return( otherseg );
    }
    return( 0 );
}


word    AltSegment( word seg )
{
    word        otherseg;

    otherseg = LookUp( 0x30, seg, 0 );  /* try LDT */
    if( otherseg != 0 ) return( otherseg );
    otherseg = LookUp( 0x38, seg, 1 );  /* try GDT */
    if( otherseg != 0 ) return( otherseg );
    return( seg );
}

static int ReadWrite( int (*r)(word,dword,char*), addr48_ptr *addr, char *data, int req ) {

    int         len;
    word        segment;
    dword       offset;

    offset = addr->offset;
    segment = addr->segment;
                                                                          _DBG2(("Read Write %4.4x:%8.8lx",segment,offset));
    if( SegLimit( segment ) >= offset + req - 1 ) {
                                                                          _DBG2(("Read Write Ok for %d", req));
        if( !r( segment, offset, data ) ) {
            segment = AltSegment( segment );
        }
        if( SegLimit( segment ) < offset + req - 1 ) {
                                                                          _DBG3(("Gosh, we're in trouble dudes"));
            if( SegLimit( segment ) == 0 ) {
                                                                          _DBG3(("Gosh, we're in SERIOUS trouble dudes"));
            }
        } else {
            len = req;
            while( --len >= 0 ) {
                if( !r( segment, offset++, data++ ) ) {
                                                                          _DBG3(("failed for %4.4x:%8.8lx", segment, offset-1));
                }
            }
                                                                          _DBG2(("Read Write Done"));
            return( req );
        }
    }
    len = 0;
                                                                          _DBG2(("Read Write One byte at a time for %d", req));
    while( --req >= 0 ) {
        if( SegLimit( segment ) < offset ) break;
        if( !r( segment, offset, data ) ) {
            segment = AltSegment( segment );
        }
        if( !r( segment, offset++, data++ ) ) {
                                                                          _DBG3(("failed for %4.4x:%8.8lx", segment, offset-1));
        }
        ++len;
    }
                                                                          _DBG2(("Read Write Done"));
    return( len );
}


static int ReadMemory( addr48_ptr *addr, byte *data, int len )
{
    return( ReadWrite( DoReadMem, addr, (char *)data, len ) );
}

static int WriteMemory( addr48_ptr *addr, byte *data, int len )
{
    if( addr->segment == Regs.CS ) addr->segment = Regs.DS; // hack, ack
    return( ReadWrite( DoWriteMem, addr, (char *)data, len ) );
}

trap_retval ReqGet_sys_config( void )
{
    get_sys_config_ret  *ret;

                                                                          _DBG1(( "AccGetConfig" ));
    ret = GetOutPtr(0);
    ret->sys.os = OS_AUTOCAD;
    ret->sys.osmajor = _osmajor;
    ret->sys.osminor = _osminor;
    ret->sys.cpu = X86CPUType();
    ret->sys.huge_shift = 12;
    ret->sys.fpu = RealNPXType;
    ret->sys.mad = MAD_X86;
    return( sizeof( *ret ) );
}


trap_retval ReqMap_addr( void )
{
    map_addr_req        *acc;
    map_addr_ret        *ret;

    acc = GetInPtr(0);
    ret = GetOutPtr(0);
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

//OBSOLETE - use ReqMachine_data
trap_retval ReqAddr_info( void )
{
    addr_info_ret       *ret;

                                                                          _DBG1(( "AccAddrInfo" ));
    ret = GetOutPtr( 0 );
    ret->is_big = 1;
    return( sizeof( *ret ) );
}

trap_retval ReqMachine_data( void )
{
    machine_data_ret    *ret;
    unsigned_8          *data;

    ret = GetOutPtr( 0 );
    data = GetOutPtr( sizeof( *ret ) );
    ret->cache_start = 0;
    ret->cache_end = ~(addr_off)0;
    *data = X86AC_BIG;
    return( sizeof( *ret ) + sizeof( *data ) );
}


trap_retval ReqChecksum_mem( void )
{
    unsigned       len;
    int            i;
    int            read;
    checksum_mem_req    *acc;
    checksum_mem_ret    *ret;

                                                                          _DBG1(( "AccChkSum" ));
    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    len = acc->len;
    ret->result = 0;
    while( len >= BUFF_SIZE ) {
        read = ReadMemory( &acc->in_addr, (byte *)&UtilBuff, BUFF_SIZE );
        acc->in_addr.offset += BUFF_SIZE;
        for( i = 0; i < read; ++i ) {
            ret->result += UtilBuff[ i ];
        }
        if( read != BUFF_SIZE ) return( sizeof( *ret ) );
        len -= BUFF_SIZE;
    }
    if( len != 0 ) {
        read = ReadMemory( &acc->in_addr, (byte *)&UtilBuff, len );
        if( read == len ) {
            for( i = 0; i < len; ++i ) {
                ret->result += UtilBuff[ i ];
            }
        }
    }
    return( sizeof( *ret ) );
}


trap_retval ReqRead_mem( void )
{
    read_mem_req        *acc;
    void                *ret;
    unsigned            len;
                                                                          _DBG1(( "ReadMem" ));
    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    len = ReadMemory( &acc->mem_addr, ret, acc->len );
    return( len );
}

trap_retval ReqWrite_mem( void )
{
    write_mem_req       *acc;
    write_mem_ret       *ret;
                                                                          _DBG1(( "WriteMem" ));
    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    ret->len = WriteMemory( &acc->mem_addr, GetInPtr( sizeof(*acc) ),
                            GetTotalSize() - sizeof(*acc) );
    return( sizeof( *ret ) );
}

trap_retval ReqRead_io( void )
{
    read_io_req *acc;
    void        *ret;

    acc = GetInPtr(0);
    ret = GetOutPtr(0);
    if( acc->len == 1 ) {
        *( (byte *)ret ) = In_b( acc->IO_offset );
    } else if( acc->len == 2 ) {
        *( (word *)ret ) = In_w( acc->IO_offset );
    } else {
        *( (dword *)ret ) = In_d( acc->IO_offset );
    }
    return( acc->len );
}


trap_retval ReqWrite_io( void )
{
    int              len;
    write_io_req        *acc;
    write_io_ret        *ret;
    void                *data;

    acc = GetInPtr(0);
    data = GetInPtr( sizeof( *acc ) );
    len = GetTotalSize() - sizeof( *acc );
    ret = GetOutPtr(0);
    if( len == 1 ) {
        Out_b( acc->IO_offset, *( (byte *)data ) );
    } else if( len == 2 ) {
        Out_w( acc->IO_offset, *( (word *)data ) );
    } else {
        Out_d( acc->IO_offset, *( (dword *)data ) );
    }
    ret->len = len;
    return( sizeof( *ret ) );
}

//OBSOLETE - use ReqRead_regs
trap_retval ReqRead_cpu( void )
{
    trap_cpu_regs       *regs;

    regs = GetOutPtr( 0 );
    *regs = Regs;
    return( sizeof( *regs ) );
}

//OBSOLETE - use ReqRead_regs
trap_retval ReqRead_fpu( void )
{
    Read387( (trap_fpu_regs *)GetOutPtr( 0 ) );
    return( sizeof( trap_fpu_regs ) );
}

trap_retval ReqRead_regs( void )
{
    mad_registers       *mr;

    mr = GetOutPtr(0);
    mr->x86.cpu = *(struct x86_cpu *)&Regs;
    Read387( &mr->x86.u.fpu );
    return( sizeof( mr->x86 ) );
}

//OBSOLETE - use ReqWrite_regs
trap_retval ReqWrite_cpu( void )
{
    Regs = *( trap_cpu_regs *)GetInPtr( sizeof( write_cpu_req ) );
    return( 0 );
}

//OBSOLETE - use ReqWrite_regs
trap_retval ReqWrite_fpu( void )
{
    Write387( (trap_fpu_regs *)GetInPtr( sizeof( write_fpu_req ) ) );
    return( 0 );
}

trap_retval ReqWrite_regs( void )
{
    mad_registers       *mr;

    mr = GetInPtr(sizeof(write_regs_req));
    *(struct x86_cpu *)&Regs = mr->x86.cpu;
    Write387( &mr->x86.u.fpu );
    return( 0 );
}

#pragma aux StackCheck "__STK";

void StackCheck()
{
}


void ADSLoop()
{
    short scode = RSRSLT;             /* Normal result code (default) */
    int stat;

    for ( ;; ) {                      /* Request/Result loop */
//                                                                        _DBG0(("ADS Loop"));

        if ((stat = ads_link(scode)) < 0) {
            cputs( "ADSHELP: bad status from ads_link()\r\n");
            exit(1);
        }

        scode = RSRSLT;               /* Reset result code */

        switch (stat) {

        case RQSUBR:                  /* Handle external function requests */
            scode = RSERR;
            break;

        case RQXUNLD:                 /* Handle external function requests */
            DoneAutoCAD = TRUE;
            AtEnd = TRUE;
            BreakPoint();             /* you figure it out */
            break;
        }
    }
}


trap_retval ReqProg_load( void )
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
    regs=regs;
    _DBG0(("EAX=%8.8x EBX=%8.8x ECX=%8.8x EDX=%8.8x",
    regs->EAX, regs->EBX, regs->ECX, regs->EDX ));
    _DBG0(("ESI=%8.8x EDI=%8.8x ESP=%8.8x EBP=%8.8x",
    regs->ESI, regs->EDI, regs->ESP, regs->EBP ));
    _DBG0(("DS=%4.4x ES=%4.4x FS=%4.4x GS=%4.4x",
    regs->DS, regs->ES, regs->FS, regs->GS ));
    _DBG0(("CS=%4.4x EIP=%8.8x EFL=%8.8x SS=%4.4x",
    regs->CS, regs->EIP, regs->EFL, regs->SS ));
}

static void MyRunProg()
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

trap_retval ReqProg_kill( void )
{
    prog_kill_ret       *ret;
                                                                          _DBG1(( "AccKillProg" ));
    ret = GetOutPtr( 0 );
    RedirectFini();
    AtEnd = TRUE;
    if( !DoneAutoCAD ) {
        AtEnd = TRUE;
        cputs( "*** Please quit AUTOCAD in order to restart debugger ***\r\n" );
        MyRunProg();
    }
    ret->err = 0;
    return( sizeof( *ret ) );
}


trap_retval ReqSet_watch( void )
{
    watch        *curr;
    set_watch_req       *acc;
    set_watch_ret       *ret;
    int          i,needed;

    _DBG0(( "AccSetWatch" ));
    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    ret->multiplier = 20000;
    ret->err = 1;
    if( WatchCount < MAX_WP ) {
        ret->err = 0;
        curr = WatchPoints + WatchCount;
        curr->addr = acc->watch_addr;
        ReadMemory( &acc->watch_addr, (byte *)&curr->value, 4 );
        ++WatchCount;
        curr->linear = GetLinear( curr->addr.segment, curr->addr.offset );
        curr->len = acc->size;
        curr->dregs = ( curr->linear & (curr->len-1) ) ? 2 : 1;
        curr->linear &= ~(curr->len-1);
        needed = 0;
        for( i = 0; i < WatchCount; ++i ) {
            needed += WatchPoints[ i ].dregs;
        }
        if( needed <= 4 ) ret->multiplier |= USING_DEBUG_REG;
        _DBG0((
        "addr %4.4x:%8.8lx "
        "linear %8.8x "
        "len %d "
        "needed %d "
        ,curr->addr.segment,curr->addr.offset
        ,curr->linear
        ,curr->len
        ,needed
        ));
    }
    return( sizeof( *ret ) );
}

trap_retval ReqClear_watch( void )
{
    _DBG0(( "AccRestoreWatch" ));
    /* assume all watches removed at same time */
    WatchCount = 0;
    return( 0 );
}

trap_retval ReqSet_break( void )
{
    set_break_req       *acc;
    set_break_ret       *ret;
    opcode_type         brk_opcode; /* cause maybe SS != DS */

_DBG1(( "AccSetBreak" ));
    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    if( ReadMemory( &acc->break_addr, &brk_opcode, sizeof( brk_opcode ) ) == 0 ) {
        ret->old = 0;
    } else {
        ret->old = brk_opcode;
        brk_opcode = BRKPOINT;
        WriteMemory( &acc->break_addr, &brk_opcode, sizeof( brk_opcode ) );
    }
    return( sizeof( *ret ) );
}

trap_retval ReqClear_break( void )
{
    clear_break_req     *acc;
    opcode_type         brk_opcode;

    acc = GetInPtr( 0 );
    brk_opcode = acc->old;
    WriteMemory( &acc->break_addr, &brk_opcode, sizeof( brk_opcode ) );
    _DBG1(( "AccRestoreBreak" ));
    return( 0 );
}

static dword *DR[] = { &SysRegs.dr0, &SysRegs.dr1, &SysRegs.dr2, &SysRegs.dr3 };

static void SetDRnBW( int dr, dword linear, int len ) /* Set DRn for break on write */
{
    *DR[ dr ] = linear;
    SysRegs.dr7 |= ( ( DRLen( len )+DR7_BWR) << DR7_RWLSHIFT( dr ) )
                     | ( DR7_GEMASK << DR7_GLSHIFT( dr ) );
}


static bool SetDebugRegs()
{
    int         needed;
    int         i;
    int         dr;
    watch       *wp;

    needed = 0;
    for( i = WatchCount, wp = WatchPoints; i != 0; --i, ++wp ) {
        needed += wp->dregs;
    }
    if( needed > 4 ) return( FALSE );
    dr = 0;
    SysRegs.dr7 = DR7_GE;
    for( i = WatchCount, wp = WatchPoints; i != 0; --i, ++wp ) {
        SetDRnBW( dr, wp->linear, wp->len );
        ++dr;
        if( wp->dregs == 2 ) {
            SetDRnBW( dr, wp->linear+4, wp->len );
            ++dr;
        }
    }
    return( TRUE );
}

static unsigned ProgRun( bool step )
{
    watch       *wp;
    long        trace;
    int         i;
    dword       value;
    prog_go_ret *ret;

                                                                          _DBG1(( "ProgRun" ));
    ret = GetOutPtr( 0 );
    ret->conditions = COND_CONFIG;
    trace = step ? TRACE_BIT : 0;
    Regs.EFL |= trace;
    if( AtEnd ) {
                                                                          _DBG2(("No RunProg"));
        ;
    } else if( !trace && WatchCount != 0 ) {
                                                                          _DBG2(("All that trace goop"));
        if( SetDebugRegs() ) {
            MyRunProg();
            SysRegs.dr6 = 0;
            SysRegs.dr7 = 0;
        } else {
            for( ;; ) {
                Regs.EFL |= TRACE_BIT;
                MyRunProg();
                if( DoneAutoCAD ) break;
                if( IntNum != 1 ) break;
                if( !( SysRegs.dr6 & DR6_BS ) ) break;
                for( wp = WatchPoints, i = WatchCount; i > 0; ++wp, --i ) {
                    ReadMemory( &wp->addr, (void *)&value, 4 );
                    if( value != wp->value ) {
                        ret->conditions |= COND_WATCH;
                        goto leave;
                    }
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
        AtEnd = TRUE;
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
leave:
    Regs.EFL &= ~trace;
    ret->program_counter.offset = Regs.EIP;
    ret->program_counter.segment = Regs.CS;
    ret->stack_pointer.offset = Regs.ESP;
    ret->stack_pointer.segment = Regs.SS;
    WatchCount = 0;
    return( sizeof( *ret ) );
}

trap_retval ReqProg_go( void )
{
    return( ProgRun( FALSE ) );
}

trap_retval ReqProg_step( void )
{
    return( ProgRun( TRUE ) );
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

trap_retval ReqGet_next_alias( void )
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
extern int GtKey();

#pragma aux GtKey =                                            \
0X30 0XE4       /* xor    ah,ah                         */      \
0XCD 0X16       /* int    16                            */      \
modify [ ax ];


static unsigned_16 AccReadUserKey()
{
    rd_key_return FAR *retblk;

    retblk = GetOutPtr( 0 );
    retblk->key = GtKey();
    return( sizeof( rd_key_return ) );
}
#endif

trap_retval ReqGet_err_text( void )
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
        strcpy( err_txt, DosErrMsgs[ acc->err ] );
    } else {
        strcpy( err_txt, TRP_ERR_unknown_system_error );
        ultoa( acc->err, err_txt + strlen( err_txt ), 16 );
    }
    return( strlen( err_txt ) + 1 );
}

trap_retval ReqGet_lib_name( void )
{
    char        *ch;
    get_lib_name_ret    *ret;

    ret = GetOutPtr( 0 );
    ret->handle = 0;
    ch = GetOutPtr( sizeof( *ret ) );
    *ch = '\0';
    return( sizeof( *ret ) + 1 );
}


trap_retval ReqGet_message_text( void )
{
    static const char * const ExceptionMsgs[] = {
        #define pick(a,b) b,
        #include "x86exc.h"
        #undef pick
    };
    get_message_text_ret        *ret;
    char                        *err_txt;

    ret = GetOutPtr( 0 );
    err_txt = GetOutPtr( sizeof(*ret) );
    if( IntNum == -1 ) {
        err_txt[0] = '\0';
    } else {
        if( except < sizeof( ExceptionMsgs ) / sizeof( ExceptionMsgs[0] ) ) {
            strcpy( err_txt, ExceptionMsgs[except] );
        } else {
            strcpy( err_txt, TRP_EXC_unknown );
        }
        IntNum = -1;
    }
    ret->flags = MSG_NEWLINE | MSG_ERROR;
    return( sizeof( *ret ) + strlen( err_txt ) + 1 );
}

trap_version TRAPENTRY TrapInit( char *parm, char *err, bool remote )
{
    trap_version        ver;

                                                                          _DBG0(( "TrapInit" ));
    remote = remote; parm = parm;
    err[0] = '\0'; /* all ok */
    ver.major = TRAP_MAJOR_VERSION;
    ver.minor = TRAP_MINOR_VERSION;
    ver.remote = FALSE;
    RedirectInit();
    RealNPXType = NPXType();
    WatchCount = 0;
    FakeBreak = FALSE;
    GrabVects();
                                                                          _DBG0(( "Done TrapInit" ));
    return( ver );
}

void LetACADDie()
{
    if( DoneAutoCAD ) {
        DoneAutoCAD = FALSE;
        MyRunProg();    /* we're history! */
    }
}

void TRAPENTRY TrapFini()
{
                                                                          _DBG0(( "TrapFini" ));
    ReleVects();
                                                                          _DBG0(( "Done TrapFini" ));
}

void GotInt3()
{
    _DBG0(( "Got Int 3!!!" ));
}
