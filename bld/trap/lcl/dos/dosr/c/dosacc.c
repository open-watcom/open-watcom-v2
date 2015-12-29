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
* Description:  DOS real mode debugger access functions.
*
****************************************************************************/


//#define DEBUG_ME

#include <stdlib.h>
#include <string.h>
#include <i86.h>
#include "tinyio.h"
#include "dbg386.h"
#include "drset.h"
#include "exedos.h"
#include "exeos2.h"
#include "exephar.h"
#include "trperr.h"
#include "doserr.h"
#include "trpimp.h"
#include "trpcomm.h"
#include "ioports.h"
#include "winchk.h"
#include "madregs.h"
#include "x86cpu.h"
#include "miscx87.h"
#include "dosredir.h"
#include "doscomm.h"
#include "cpuglob.h"
#include "dosextx.h"
#include "dosovl.h"
#include "dosfile.h"


typedef enum {
    EXE_UNKNOWN,
    EXE_DOS,                    /* DOS */
    EXE_OS2,                    /* OS/2 */
    EXE_PHARLAP_SIMPLE,         /* PharLap Simple */
    EXE_PHARLAP_EXTENDED_286,   /* PharLap Extended 286 */
    EXE_PHARLAP_EXTENDED_386,   /* PharLap Extended 386 (may be bound) */
    EXE_RATIONAL_386,           /* Rational DOS/4G app */
    EXE_LAST_TYPE
} EXE_TYPE;

typedef _Packed struct pblock {
    addr_seg    envstring;
    addr32_ptr  commandln;
    addr32_ptr  fcb01;
    addr32_ptr  fcb02;
    addr32_ptr  startsssp;
    addr32_ptr  startcsip;
} pblock;

#define CMD_OFFSET      0x80

//
// NOTE: if you change this structure, you must update DBGTRAP.ASM
//
typedef _Packed struct watch {
    addr32_ptr  addr;
    dword       value;
    dword       linear;
    short       dregs;
    short       len;
} watch;

typedef enum {
    TRAP_SKIP = -1,
    TRAP_NONE,
    TRAP_TRACE_POINT,
    TRAP_BREAK_POINT,
    TRAP_WATCH_POINT,
    TRAP_USER,
    TRAP_TERMINATE,
    TRAP_MACH_EXCEPTION,
    TRAP_OVL_CHANGE_LOAD,
    TRAP_OVL_CHANGE_RET
} trap_types;

/* user modifiable flags */
#define USR_FLAGS (FLG_C | FLG_P | FLG_A | FLG_Z | FLG_S | FLG_I | FLG_D | FLG_O)

extern void MoveBytes( short, short, short, short, short );
#pragma aux MoveBytes =                                  \
/*  MoveBytes( fromseg, fromoff, toseg, tooff, len ); */ \
       " rep    movsb "                                  \
    parm    caller  [ ds ] [ si ] [ es ] [ di ] [ cx ]   \
    modify  [ si di ];

extern unsigned short MyFlags( void );
#pragma aux MyFlags = \
       " pushf  "     \
       " pop ax "     \
    value [ax];

extern addr_seg         DbgPSP(void);
extern tiny_ret_t       DOSLoadProg(char __far *, pblock __far *);
extern addr_seg         DOSTaskPSP(void);
extern void             EndUser(void);
extern unsigned_8       RunProg(trap_cpu_regs *, trap_cpu_regs *);
extern void             SetWatch386( unsigned, watch __far * );
extern void             SetWatchPnt(unsigned, watch __far *);
extern void             SetSingleStep(void);
extern void             SetSingle386(void);
extern void             InitVectors(void);
extern void             FiniVectors(void);
extern void             TrapTypeInit(void);
extern void             ClrIntVecs(void);
extern void             SetIntVecs(void);
extern void             DoRemInt(trap_cpu_regs *, unsigned);
extern char             Have87Emu(void);
extern void             Null87Emu( void );
extern void             Read87EmuState( void __far * );
extern void             Write87EmuState( void __far * );
extern unsigned         StringToFullPath( char * );
extern int              __far NoOvlsHdlr( int, void * );

extern word             __based(__segname("_CODE")) SegmentChain;

trap_cpu_regs   TaskRegs;
char            DOS_major;
char            DOS_minor;
bool            BoundAppLoading;
bool            IsBreak[4];

#define MAX_WP  32
watch           WatchPoints[ MAX_WP ];
short           WatchCount;

struct {
    unsigned    Is386       : 1;
    unsigned    IsMMX       : 1;
    unsigned    IsXMM       : 1;
    unsigned    DRsOn       : 1;
    unsigned    com_file    : 1;
    unsigned    NoOvlMgr    : 1;
    unsigned    BoundApp    : 1;
} Flags;

static tiny_handle_t    EXEhandle;
static tiny_ftime_t     EXETime;
static tiny_fdate_t     EXEDate;
#define ReadEXE( x )    TinyRead( EXEhandle, &x, sizeof(x) )
#define WriteEXE( x )   TinyWrite( EXEhandle, &x, sizeof(x) )
#define SeekEXEset( x ) TinySeek( EXEhandle, x, TIO_SEEK_START )
static dword            NEOffset;
static word             NumSegments;
static dword            SegTable;
static dword            StartByte;
static opcode_type      saved_opcode;
static addr48_ptr       BadBreak;
static bool             GotABadBreak;
static int              ExceptNum;

static unsigned_8       RealNPXType;
static unsigned_8       CPUType;

#ifdef DEBUG_ME
int out( char * str )
{
    char *p;

    p = str;
    while( *p ) ++p;
    TinyWrite( 1, str, p - str );
    return 0;
}

#define out0 out

static char hexbuff[80];

char * hex( unsigned long num )
{
    char *p;

    p = &hexbuff[79];
    *p = 0;
    if( num == 0 ) {
      *--p = '0';
      return( p );
    }
    while( num != 0 ) {
        *--p = "0123456789abcdef"[ num & 15 ];
        num >>= 4;
    }
    return( p );

}
#else
    #define out( s )
    #define out0( s ) 0
    #define hex( n )
#endif

trap_retval ReqGet_sys_config( void )
{
    get_sys_config_ret  *ret;

    ret = GetOutPtr(0);
    ret->sys.os = MAD_OS_DOS;
    ret->sys.osmajor = DOS_major;
    ret->sys.osminor = DOS_minor;
    ret->sys.cpu = CPUType;
    if( Have87Emu() ) {
        ret->sys.fpu = X86_EMU;
    } else if( RealNPXType != X86_NO ) {
        if( CPUType < X86_486 ) {
            ret->sys.fpu = RealNPXType;
        } else {
            ret->sys.fpu = CPUType & X86_CPU_MASK;
        }
    } else {
        ret->sys.fpu = X86_NO;
    }
    ret->sys.huge_shift = 12;
    ret->sys.mad = MAD_X86;
    return( sizeof( *ret ) );
}


trap_retval ReqMap_addr( void )
{
    word            seg;
    int             count;
    word            __far *segment;
    map_addr_req    *acc;
    map_addr_ret    *ret;

    acc = GetInPtr(0);
    ret = GetOutPtr(0);
    seg = acc->in_addr.segment;
    switch( seg ) {
    case MAP_FLAT_CODE_SELECTOR:
    case MAP_FLAT_DATA_SELECTOR:
        seg = 0;
        break;
    }
    if( Flags.BoundApp ) {
        count = NumSegments - seg;
        segment = MK_FP( SegmentChain, 14 );
        while( count != 0 ) {
            segment = MK_FP( *segment, 14 );
            --count;
        }
        ret->out_addr.segment = FP_SEG( segment ) + 1;
    } else {
        ret->out_addr.segment = DOSTaskPSP() + seg;
        if( !Flags.com_file ) ret->out_addr.segment += 0x10;
    }
    ret->out_addr.offset = acc->in_addr.offset;
    ret->lo_bound = 0;
    ret->hi_bound = ~(addr48_off)0;
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
    *data = 0;
    return( sizeof( *ret ) + sizeof( *data ) );
}

trap_retval ReqChecksum_mem( void )
{
    unsigned_8          __far *ptr;
    unsigned long       sum = 0;
    unsigned            len;
    checksum_mem_req    *acc;
    checksum_mem_ret    *ret;

    acc = GetInPtr(0);
    ret = GetOutPtr(0);
    ptr = MK_FP( acc->in_addr.segment, acc->in_addr.offset );
    for( len = acc->len; len != 0; --len ) {
        sum += *ptr++;
    }
    ret->result = sum;
    return( sizeof( *ret ) );
}


static bool IsInterrupt( addr48_ptr addr, unsigned length )
{
    unsigned long start, end;

    start = ((unsigned long)addr.segment << 4) + addr.offset;
    end = start + length;
    return( start < 0x400 || end < 0x400 );
}


trap_retval ReqRead_mem( void )
{
    bool            int_tbl;
    read_mem_req    *acc;
    void            *data;
    trap_elen       len;

    acc = GetInPtr(0);
    data = GetOutPtr( 0 );
    acc->mem_addr.offset &= 0xffff;
    int_tbl = IsInterrupt( acc->mem_addr, acc->len );
    if( int_tbl )
        SetIntVecs();
    len = acc->len;
    if( ( acc->mem_addr.offset + len ) > 0xffff ) {
        len = 0x10000 - acc->mem_addr.offset;
    }
    MoveBytes( acc->mem_addr.segment, acc->mem_addr.offset,
               FP_SEG( data ), FP_OFF( data ), len );
    if( int_tbl )
        ClrIntVecs();
    return( len );
}


trap_retval ReqWrite_mem( void )
{
    bool            int_tbl;
    write_mem_req   *acc;
    write_mem_ret   *ret;
    trap_elen       len;
    void            *data;

    acc = GetInPtr(0);
    ret = GetOutPtr(0);
    data = GetInPtr(sizeof(*acc));
    len = GetTotalSize() - sizeof(*acc);

    acc->mem_addr.offset &= 0xffff;
    int_tbl = IsInterrupt( acc->mem_addr, len );
    if( int_tbl )
        SetIntVecs();
    if( ( acc->mem_addr.offset + len ) > 0xffff ) {
        len = 0x10000 - acc->mem_addr.offset;
    }
    MoveBytes( FP_SEG( data ), FP_OFF( data ),
               acc->mem_addr.segment, acc->mem_addr.offset, len );
    if( int_tbl )
        ClrIntVecs();
    ret->len = len;
    return( sizeof( *ret ) );
}


trap_retval ReqRead_io( void )
{
    read_io_req     *acc;
    void            *data;
    trap_elen       len;

    acc = GetInPtr(0);
    data = GetOutPtr(0);
    if( acc->len == 1 ) {
       *( (byte __far *)data ) = In_b( acc->IO_offset );
       len = 1;
    } else if( acc->len == 2 ) {
       *( (word __far *)data ) = In_w( acc->IO_offset );
       len = 2;
    } else if( Flags.Is386 ) {
       *( (dword __far *)data ) = In_d( acc->IO_offset );
       len = 4;
    } else {
       len = 0;
    }
    return( len );
}


trap_retval ReqWrite_io( void )
{
    write_io_req        *acc;
    write_io_ret        *ret;
    void                *data;
    trap_elen           len;

    acc = GetInPtr(0);
    data = GetInPtr( sizeof( *acc ) );
    len = GetTotalSize() - sizeof( *acc );
    ret = GetOutPtr(0);
    if( len == 1 ) {
        Out_b( acc->IO_offset, *( (byte __far *)data ) );
        ret->len = 1;
    } else if( len == 2 ) {
        Out_w( acc->IO_offset, *( (word __far *)data ) );
        ret->len = 2;
    } else if ( Flags.Is386 ) {
        Out_d( acc->IO_offset, *( (dword __far *)data ) );
        ret->len = 4;
    } else {
        ret->len = 0;
    }
    return( sizeof( *ret ) );
}

trap_retval ReqRead_regs( void )
{
    mad_registers       *mr;

    mr = GetOutPtr(0);
    mr->x86.cpu = *(struct x86_cpu *)&TaskRegs;
    if( Have87Emu() ) {
        Read87EmuState( &mr->x86.u.fpu );
    } else if( RealNPXType != X86_NO ) {
        Read8087( &mr->x86.u.fpu );
    } else {
        memset( &mr->x86.u.fpu, 0, sizeof( mr->x86.u.fpu ) );
    }
    return( sizeof( mr->x86 ) );
}

trap_retval ReqWrite_regs( void )
{
    mad_registers       *mr;

    mr = GetInPtr(sizeof(write_regs_req));
    *(struct x86_cpu *)&TaskRegs = mr->x86.cpu;
    if( Have87Emu() ) {
        Write87EmuState( &mr->x86.u.fpu );
    } else if( RealNPXType != X86_NO ) {
        Write8087( &mr->x86.u.fpu );
    }
    return( 0 );
}

static EXE_TYPE CheckEXEType( char *name )
{
    tiny_ret_t            rc;
    union {
        tiny_ret_t        rc;
        tiny_file_stamp_t stamp;
    } exe_time;
    word                  value;
    opcode_type           brk_opcode;
    static dos_exe_header head;
    static os2_exe_header os2_head;

    Flags.com_file = FALSE;
    EXEhandle = 0;
    rc = TinyOpen( name, TIO_READ_WRITE );
    if( TINY_ERROR( rc ) )
        return( EXE_UNKNOWN );
    EXEhandle = TINY_INFO( rc );
    exe_time.rc = TinyGetFileStamp( EXEhandle );
    EXETime = exe_time.stamp.time;
    EXEDate = exe_time.stamp.date;
    if( TINY_ERROR( ReadEXE( head ) ) )
        return( EXE_UNKNOWN );    /* MZ Signature */
    switch( head.signature ) {
    case SIMPLE_SIGNATURE: // mp
    case REX_SIGNATURE: // mq
    case EXTENDED_SIGNATURE: // 'P3'
        return( EXE_PHARLAP_SIMPLE );
    case DOS_SIGNATURE:
        if( head.reloc_offset != OS2_EXE_HEADER_FOLLOWS )
            return( EXE_DOS );
        if( TINY_ERROR( SeekEXEset( OS2_NE_OFFSET ) ) )
            return( EXE_UNKNOWN );/* offset of new exe */
        if( TINY_ERROR( ReadEXE( NEOffset ) ) )
            return( EXE_UNKNOWN );
        if( TINY_ERROR( SeekEXEset( NEOffset ) ) )
            return( EXE_UNKNOWN );
        if( TINY_ERROR( ReadEXE( os2_head ) ) )
            return( EXE_UNKNOWN );/* NE Signature */
        if( os2_head.signature == RAT_SIGNATURE_WORD )
            return( EXE_RATIONAL_386 );
        if( os2_head.signature != OS2_SIGNATURE_WORD )
            return( EXE_UNKNOWN );
        NumSegments = os2_head.segments;
        SegTable = NEOffset + os2_head.segment_off;
        if( os2_head.align == 0 )
            os2_head.align = 9;
        SeekEXEset( SegTable + ( os2_head.entrynum - 1 ) * 8 );
        ReadEXE( value );
        StartByte = ( (long)value << os2_head.align ) + os2_head.IP;
        SeekEXEset( StartByte );
        ReadEXE( saved_opcode );
        SeekEXEset( StartByte );
        brk_opcode = BRKPOINT;
        rc = WriteEXE( brk_opcode );
        return( EXE_OS2 );
    default:
        Flags.com_file = TRUE;
        return( EXE_UNKNOWN );
    }
}


trap_retval ReqProg_load( void )
{
    addr_seg        psp;
    pblock          parmblock;
    tiny_ret_t      rc;
    char            *parm;
    char            *name;
    char            __far *dst;
    char            exe_name[128];
    char            ch;
    EXE_TYPE        exe;
    prog_load_ret   *ret;
    unsigned        len;

    ExceptNum = -1;
    ret = GetOutPtr( 0 );
    memset( &TaskRegs, 0, sizeof( TaskRegs ) );
    TaskRegs.EFL = MyFlags() & ~USR_FLAGS;
    /* build a DOS command line parameter in our PSP command area */
    Flags.BoundApp = FALSE;
    psp = DbgPSP();
    parm = name = GetInPtr( sizeof( prog_load_req ) );
    if( TINY_ERROR( FindFilePath( name, exe_name, DosExtList ) ) ) {
        exe_name[0] = '\0';
    }
    while( *parm++ != '\0' ) {}     // skip program name
    len = GetTotalSize() - ( parm - name ) - sizeof( prog_load_req );
    if( len > 126 )
        len = 126;
    dst = MK_FP( psp, CMD_OFFSET + 1 );
    for( ; len > 0; --len ) {
        ch = *parm++;
        if( ch == '\0' ) {
            if( len == 1 )
                break;
            ch = ' ';
        }
        *dst++ = ch;
    }
    *dst = '\r';
    *(byte __far *)MK_FP( psp, CMD_OFFSET ) = FP_OFF( dst ) - ( CMD_OFFSET + 1 );
    parmblock.envstring = 0;
    parmblock.commandln.segment = psp;
    parmblock.commandln.offset =  CMD_OFFSET;
    parmblock.fcb01.segment = psp;
    parmblock.fcb02.segment = psp;
    parmblock.fcb01.offset  = 0x5C;
    parmblock.fcb02.offset  = 0x6C;
    exe = CheckEXEType( exe_name );
    if( EXEhandle != 0 ) {
        TinyClose( EXEhandle );
        EXEhandle = 0;
    }
    switch( exe ) {
    case EXE_RATIONAL_386:
        ret->err = ERR_RATIONAL_EXE;
        return( sizeof( *ret ) );
    case EXE_PHARLAP_SIMPLE:
        ret->err = ERR_PHARLAP_EXE;
        return( sizeof( *ret ) );
    }
    SegmentChain = 0;
    BoundAppLoading = FALSE;
    rc = DOSLoadProg( exe_name, &parmblock );
    if( TINY_OK( rc ) ) {
        TaskRegs.SS = parmblock.startsssp.segment;
        /* for some insane reason DOS returns a starting SP two less then
           normal */
        TaskRegs.ESP = parmblock.startsssp.offset + 2;
        TaskRegs.CS = parmblock.startcsip.segment;
        TaskRegs.EIP = parmblock.startcsip.offset;
        psp = DOSTaskPSP();
    } else {
        psp = TinyAllocBlock( TinyAllocBlock( 0xffff ) );
        TinyFreeBlock( psp );
        TaskRegs.SS = psp + 0x10;
        TaskRegs.ESP = 0xfffe;
        TaskRegs.CS = psp + 0x10;
        TaskRegs.EIP = 0x100;
    }
    TaskRegs.DS = psp;
    TaskRegs.ES = psp;
    if( TINY_OK( rc ) ) {
        if( Flags.NoOvlMgr || !CheckOvl( parmblock.startcsip ) ) {
            if( exe == EXE_OS2 ) {
                opcode_type __far *loc_brk_opcode;

                BoundAppLoading = TRUE;
                RunProg( &TaskRegs, &TaskRegs );
                loc_brk_opcode = MK_FP(TaskRegs.CS, TaskRegs.EIP);
                if( *loc_brk_opcode == BRKPOINT ) {
                    *loc_brk_opcode = saved_opcode;
                }
                BoundAppLoading = FALSE;
                rc = TinyOpen( exe_name, TIO_READ_WRITE );
                if( TINY_OK( rc ) ) {
                    EXEhandle = TINY_INFO( rc );
                    SeekEXEset( StartByte );
                    WriteEXE( saved_opcode );
                    TinySetFileStamp( EXEhandle, EXETime, EXEDate );
                    TinyClose( EXEhandle );
                    EXEhandle = 0;
                    Flags.BoundApp = TRUE;
                }
            }
        }
    }
    ret->err = TINY_ERROR( rc ) ? TINY_INFO( rc ) : 0;
    ret->task_id = psp;
    ret->flags = 0;
    ret->mod_handle = 0;
    return( sizeof( *ret ) );
}


trap_retval ReqProg_kill( void )
{
    prog_kill_ret       *ret;

out( "in AccKillProg\r\n" );
    ret = GetOutPtr( 0 );
    RedirectFini();
    if( DOSTaskPSP() != NULL ) {
out( "enduser\r\n" );
        EndUser();
out( "done enduser\r\n" );
    }
out( "null87emu\r\n" );
    Null87Emu();
    NullOvlHdlr();
    ExceptNum = -1;
    ret->err = 0;
out( "done AccKillProg\r\n" );
    return( sizeof( *ret ) );
}


trap_retval ReqSet_watch( void )
{
    watch               *curr;
    set_watch_req       *wp;
    set_watch_ret       *wr;
    int                 i,needed;

    wp = GetInPtr( 0 );
    wr = GetOutPtr( 0 );
    wr->err = 1;
    wr->multiplier = 0;
    if( WatchCount < MAX_WP ) {
        wr->err = 0;
        curr = WatchPoints + WatchCount;
        curr->addr.segment = wp->watch_addr.segment;
        curr->addr.offset = wp->watch_addr.offset;
        curr->value = *(dword __far *)MK_FP( wp->watch_addr.segment, wp->watch_addr.offset );
        curr->linear = ( (unsigned long)wp->watch_addr.segment << 4 ) + wp->watch_addr.offset;
        curr->len = wp->size;
        curr->linear &= ~(curr->len-1);
        curr->dregs = ( wp->watch_addr.offset & (curr->len-1) ) ? 2 : 1;
        ++WatchCount;
        if( Flags.DRsOn ) {
            needed = 0;
            for( i = 0; i < WatchCount; ++i ) {
                needed += WatchPoints[ i ].dregs;
            }
            if( needed <= 4 ) {
                wr->multiplier |= USING_DEBUG_REG;
            }
        }
    }
    wr->multiplier |= 200;
    return( sizeof( *wr ) );
}

trap_retval ReqClear_watch( void )
{
    WatchCount = 0;
    return( 0 );
}

trap_retval ReqSet_break( void )
{
    opcode_type     __far *loc_brk_opcode;
    set_break_req   *acc;
    set_break_ret   *ret;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );

    loc_brk_opcode = MK_FP( acc->break_addr.segment, acc->break_addr.offset );
    ret->old = *loc_brk_opcode;
    *loc_brk_opcode = BRKPOINT;
    if( *loc_brk_opcode != BRKPOINT ) {
        BadBreak = acc->break_addr;
        GotABadBreak = TRUE;
    }
    return( sizeof( *ret ) );
}


trap_retval ReqClear_break( void )
{
    clear_break_req     *bp;

    bp = GetInPtr( 0 );
    *(opcode_type __far *)MK_FP( bp->break_addr.segment, bp->break_addr.offset ) = bp->old;
    GotABadBreak = FALSE;
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


static int ClearDebugRegs( int trap )
{
    long        dr6;
    int         i;

    if( Flags.DRsOn ) {
        out( "tr=" ); out( hex( trap ) );
        out( " dr6=" ); out( hex( GetDR6() ) );
        out( "\r\n" );
        if( trap == TRAP_WATCH_POINT ) { /* could be a 386 break point */
            dr6 = GetDR6();
            if( ( ( dr6 & DR6_B0 ) && IsBreak[0] )
             || ( ( dr6 & DR6_B1 ) && IsBreak[1] )
             || ( ( dr6 & DR6_B2 ) && IsBreak[2] )
             || ( ( dr6 & DR6_B3 ) && IsBreak[3] ) ) {
                 trap = TRAP_BREAK_POINT;
             }
        }
        for( i = 0; i < 4; ++i ) {
            IsBreak[ i ] = FALSE;
        }
        SetDR6( 0 );
        SetDR7( 0 );
    }
    return( trap );
}


static bool SetDebugRegs( void )
{
    int                 needed;
    int                 i;
    int                 dr;
    unsigned long       dr7;
    unsigned long       linear;
    watch               *wp;
    bool                watch386;

    if( !Flags.DRsOn )
        return( FALSE );
    needed = 0;
    for( i = WatchCount, wp = WatchPoints; i != 0; --i, ++wp ) {
        needed += wp->dregs;
    }
    dr  = 0;
    dr7 = 0;
    if( needed > 4 ) {
        watch386 = FALSE;
    } else {
        for( i = WatchCount, wp = WatchPoints; i != 0; --i, ++wp ) {
            dr7 |= SetDRn( dr, wp->linear, DRLen( wp->len ) | DR7_BWR );
            ++dr;
            if( wp->dregs == 2 ) {
                dr7 |= SetDRn( dr, wp->linear+4, DRLen( wp->len ) | DR7_BWR );
                ++dr;
            }
            watch386 = TRUE;
        }
    }
    if( GotABadBreak && dr < 4 ) {
        linear = ( (unsigned long)BadBreak.segment << 4 ) + BadBreak.offset;
        dr7 |= SetDRn( dr, linear, DR7_L1 | DR7_BINST );
        IsBreak[ dr ] = TRUE;
        ++dr;
    }
    SetDR7( dr7 );
    return( watch386 );
}

static unsigned MapReturn( int trap )
{
    out( "cond=" );
    switch( trap ) {
    case TRAP_TRACE_POINT:
        out( "trace point" );
        return( COND_TRACE );
    case TRAP_BREAK_POINT:
        out( "break point" );
        return( COND_BREAK );
    case TRAP_WATCH_POINT:
        out( "watch point" );
        return( COND_WATCH );
    case TRAP_USER:
        out( "user" );
        return( COND_USER );
    case TRAP_TERMINATE:
        out( "terminate" );
        return( COND_TERMINATE );
    case TRAP_MACH_EXCEPTION:
        out( "exception" );
        ExceptNum = 0;
        return( COND_EXCEPTION );
    case TRAP_OVL_CHANGE_LOAD:
        out( "overlay load" );
        return( COND_SECTIONS );
    case TRAP_OVL_CHANGE_RET:
        out( "overlay ret" );
        return( COND_SECTIONS );
    default:
        break;
    }
    out( "none" );
    return( 0 );
}

static trap_elen ProgRun( bool step )
{
    bool        watch386;
    prog_go_ret *ret;

    ret = GetOutPtr( 0 );
    if( Flags.DRsOn ) {
        SetSingle386();
    } else {
        SetSingleStep();
    }
    if( step ) {
        TaskRegs.EFL |= FLG_T;
    } else  {
        watch386 = SetDebugRegs();
        if( WatchCount != 0 && !watch386 ) {
            if( Flags.DRsOn ) {
                SetWatch386( WatchCount, WatchPoints );
            } else {
                SetWatchPnt( WatchCount, WatchPoints );
            }
            TaskRegs.EFL |= FLG_T;
        }
    }
    out( "in CS:EIP=" ); out( hex( TaskRegs.CS ) ); out(":" ); out( hex( TaskRegs.EIP ) );
    out( " SS:ESP=" ); out( hex( TaskRegs.SS ) ); out(":" ); out( hex( TaskRegs.ESP ) );
    out( "\r\n" );
    ret->conditions = MapReturn( ClearDebugRegs( RunProg( &TaskRegs, &TaskRegs ) ) );
    ret->conditions |= COND_CONFIG;
//    out( "cond=" ); out( hex( ret->conditions ) );
    out( " CS:EIP=" ); out( hex( TaskRegs.CS ) ); out(":" ); out( hex( TaskRegs.EIP ) );
    out( " SS:ESP=" ); out( hex( TaskRegs.SS ) ); out(":" ); out( hex( TaskRegs.ESP ) );
    out( "\r\n" );
    ret->stack_pointer.segment = TaskRegs.SS;
    ret->stack_pointer.offset  = TaskRegs.ESP;
    ret->program_counter.segment = TaskRegs.CS;
    ret->program_counter.offset  = TaskRegs.EIP;
    TaskRegs.EFL &= ~FLG_T;
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

trap_retval ReqGet_next_alias( void )
{
    get_next_alias_ret  *ret;

    ret = GetOutPtr( 0 );
    ret->seg = 0;
    ret->alias = 0;
    return( sizeof( *ret ) );
}

trap_retval ReqGet_lib_name( void )
{
    get_lib_name_ret    *ret;

    ret = GetOutPtr( 0 );
    ret->handle = 0;
    return( sizeof( *ret ) );
}

trap_retval ReqGet_err_text( void )
{
    static const char *const DosErrMsgs[] = {
        #define pick( a, b )    b,
        #include "dosmsgs.h"
        #undef pick
    };
    get_err_text_req    *acc;
    char                *err_txt;

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
    if( ExceptNum == -1 ) {
        err_txt[0] = '\0';
    } else {
        if( ExceptNum < sizeof( ExceptionMsgs ) / sizeof( ExceptionMsgs[0] ) ) {
            strcpy( err_txt, ExceptionMsgs[ExceptNum] );
        } else {
            strcpy( err_txt, TRP_EXC_unknown );
        }
        ExceptNum = -1;
    }
    ret->flags = MSG_NEWLINE | MSG_ERROR;
    return( sizeof( *ret ) + strlen( err_txt ) + 1 );
}

trap_version TRAPENTRY TrapInit( const char *parms, char *err, bool remote )
{
    trap_version ver;

out( "in TrapInit\r\n" );
out( "    checking environment:\r\n" );
    CPUType = X86CPUType();
    Flags.Is386 = ( CPUType >= X86_386 );
    if( *parms == 'D' || *parms == 'd' ) {
        Flags.DRsOn = FALSE;
        ++parms;
    } else if( out0( "    CPU type\r\n" ) || ( Flags.Is386 == 0 ) ) {
        Flags.DRsOn = FALSE;
    } else if( out0( "    WinEnh\r\n" ) || ( EnhancedWinCheck() & 0x7f ) ) {
        /* Enhanced Windows 3.0 VM kernel messes up handling of debug regs */
        Flags.DRsOn = FALSE;
    } else if( out0( "    DOSEMU\r\n" ) || DOSEMUCheck() ) {
        /* no fiddling with debug regs in Linux DOSEMU either */
        Flags.DRsOn = FALSE;
    } else {
        Flags.DRsOn = TRUE;
    }
    if( *parms == 'O' || *parms == 'o' ) {
        Flags.NoOvlMgr = TRUE;
    }
out( "    done checking environment\r\n" );
    err[0] = '\0'; /* all ok */

    Flags.IsMMX = ( ( CPUType & X86_MMX ) != 0 );
    Flags.IsXMM = ( ( CPUType & X86_XMM ) != 0 );

    /* NPXType initializes '87, so check for it before a program
       starts using the thing */
    RealNPXType = NPXType();
    InitVectors();
    if( DOS_major >= 20 ) {
        /* In an OS/2 2.0 DOS box. It doesn't let us fiddle the debug
        registers. The check is done here because InitVectors is the
        routine that sets up DOS_major */
        Flags.DRsOn = FALSE;
    }
    Null87Emu();
    NullOvlHdlr();
    TrapTypeInit();
    RedirectInit();
    ExceptNum = -1;
    WatchCount = 0;
    ver.major = TRAP_MAJOR_VERSION;
    ver.minor = TRAP_MINOR_VERSION;
    ver.remote = FALSE;
out( "done TrapInit\r\n" );
    return( ver );
}

void TRAPENTRY TrapFini( void )
{
out( "in TrapFini\r\n" );
    FiniVectors();
out( "done TrapFini\r\n" );
}
