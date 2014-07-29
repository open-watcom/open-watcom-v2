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
* Description:  OS/2 1.x debug core.
*
****************************************************************************/


#include <string.h>
#include <i86.h>
#define INCL_BASE
#define INCL_DOSDEVICES
#define INCL_DOSMEMMGR
#define INCL_DOSSIGNALS
#include <os2.h>
#include <os2dbg.h>
#include "trpimp.h"
#include "trperr.h"
#include "os2trap.h"
#include "madregs.h"
#include "x86cpu.h"
#include "miscx87.h"

typedef void (*excfn)();

extern  void    __far *Automagic( unsigned short );
#pragma aux     Automagic = 0x29 0xc4 /* sub sp,ax */\
                            0x89 0xe0 /* mov ax,sp */\
                            0x8c 0xd2 /* mov dx,ss */\
                            parm caller [ax] \
                            value [ax dx] \
                            modify [sp];


extern  void    LoadThisDLL( void );
extern  void    EndLoadThisDLL( void );

extern PID              Pid;
extern bool             AtEnd;
extern USHORT           SID;
extern bool             Remote;
extern char             UtilBuff[BUFF_SIZE];
extern HFILE            SaveStdIn;
extern HFILE            SaveStdOut;
extern bool             CanExecTask;
extern USHORT           __far *ModHandles;
extern USHORT           NumModHandles;
extern int              CurrModHandle;
extern int              ExceptNum;
extern HMODULE          ThisDLLModHandle;
extern scrtype          Screen;

static TRACEBUF         Buff;
static USHORT           SessionType;
__GINFOSEG              __far *GblInfo;
char                    OS2ExtList[] = { ".exe\0" };


typedef struct watch {
    addr48_ptr  addr;
    dword       value;
} watch;

#define MAX_WP  32
watch   WatchPoints[ MAX_WP ];
short   WatchCount = 0;

#if 0
static void Out( char __far *str )
{
    USHORT      written;

    DosWrite( 1, str, strlen( str ), &written );
}

static void OutNL()
{
    Out( "\r\n" );
}

static void OutNum( unsigned i )
{
    char numbuff[10];
    char __far *ptr;

    ptr = numbuff+10;
    *--ptr = '\0';
    while( i != 0 ) {
        *--ptr = "0123456789abcdef"[ i & 0x0f ];
        i >>= 4;
    }
    Out( ptr );
}


static void OutBuff( TRACEBUF __far *buf )
{
    Out( "pid   = " ); OutNum( buf->pid ); Out( "\r\n" );
    Out( "tid   = " ); OutNum( buf->tid ); Out( "\r\n" );
    Out( "cmd   = " ); OutNum( buf->cmd ); Out( "\r\n" );
    Out( "value = " ); OutNum( buf->value ); Out( "\r\n" );
    Out( "offv  = " ); OutNum( buf->offv ); Out( "\r\n" );
    Out( "segv  = " ); OutNum( buf->segv ); Out( "\r\n" );
    Out( "mte   = " ); OutNum( buf->mte ); Out( "\r\n" );
}
#endif



static USHORT WriteRegs( TRACEBUF __far *buff )
{
    buff->cmd = PT_CMD_WRITE_REGS;
    return( DosPTrace( buff ) );
}


static USHORT ReadRegs( TRACEBUF __far *buff )
{
    buff->cmd = PT_CMD_READ_REGS;
    return( DosPTrace( buff ) );
}


static USHORT WriteBuffer( byte __far *data, USHORT segv, USHORT offv, USHORT size )
{
    USHORT  length;

    length = size;
    if( Pid != 0 ) {
        while( length != 0 ) {
            Buff.cmd = PT_CMD_WRITE_MEM_D;
            if( length == 1 ) {
                Buff.cmd = PT_CMD_READ_MEM_D;
                Buff.offv = offv;
                Buff.segv = segv;
                DosPTrace( &Buff );
                Buff.cmd = PT_CMD_WRITE_MEM_D;
                Buff.offv = offv;
                Buff.segv = segv;
                Buff.value &= 0xff00;
                Buff.value |= *data;
                DosPTrace( &Buff );
                if( Buff.cmd != PT_RET_SUCCESS ) break;
                data++;
                length--;
                offv++;
            } else {
                Buff.value = *data;
                data++;
                Buff.value |= *data << 8;
                data++;
                Buff.offv = offv;
                Buff.segv = segv;
                DosPTrace( &Buff );
                if( Buff.cmd != PT_RET_SUCCESS ) break;
                length -= 2;
                offv += 2;
            }
        }
    }
    return( size - length ); /* return amount written */
}


static USHORT ReadBuffer( byte __far *data, USHORT segv, USHORT offv, USHORT size )
{
    USHORT      length;

    length = size;
    if( Pid != 0 ) {
        while( length != 0 ) {
            Buff.cmd = PT_CMD_READ_MEM_D;
            Buff.offv = offv;
            Buff.segv = segv;
            DosPTrace( &Buff );
            if( Buff.cmd != PT_RET_SUCCESS ) break;
            *data = Buff.value & 0xff;
            data++;
            offv++;
            length--;
            if( length != 0 ) {
                *data = Buff.value >> 8;
                data++;
                offv++;
                length--;
            }
        }
    }
    return( size - length );
}


static void RecordModHandle( USHORT value )
{
    SEL         sel;

    if( ModHandles == NULL ) {
        DosAllocSeg( sizeof( USHORT ), (PSEL)&sel, 0 );
        ModHandles = MK_FP( sel, 0 );
    } else {
        DosReallocSeg( (NumModHandles+1)*sizeof( USHORT ),
                       FP_SEG( ModHandles ) );
    }
    ModHandles[ NumModHandles ] = value;
    ++NumModHandles;
}


static void ExecuteCode( TRACEBUF __far *buff )
{
    for( ;; ) {
        buff->cmd = PT_CMD_GO;
        buff->value = 0;
        DosPTrace( buff ); // go here
        if( buff->cmd != PT_RET_LIB_LOADED ) break;
        RecordModHandle( buff->value );
    }
}


#pragma aux DoOpen parm [ dx ax ] [ bx ] [ cx ];
static void DoOpen( char FAR *name, int mode, int flags )
{
    BreakPointParm( OpenFile( name, mode, flags ) );
}


#pragma aux DoClose parm [ ax ];
static void DoClose( HFILE hdl )
{
    BreakPointParm( DosClose( hdl ) );
}


#pragma aux DoDupFile parm [ ax ] [ dx ];
static void DoDupFile( HFILE old, HFILE new )
{
    HFILE       new_t;
    USHORT      rc;

    new_t = new;
    rc = DosDupHandle( old, &new_t );
    if( rc != 0 ) {
        BreakPointParm( NIL_DOS_HANDLE );
    } else {
        BreakPointParm( new_t );
    }
}

#pragma aux DoWritePgmScrn parm [dx ax] [ bx ];
static void DoWritePgmScrn( char __far *buff, USHORT len )
{
    USHORT  written;

    DosWrite( 2, buff, len, &written );
    BreakPointParm( 0 );
}

static void DoGetMSW( void )
{
    BreakPointParm( GetMSW() );
}


static char stack[1024];

static long TaskExecute( excfn rtn )
{
    TRACEBUF    buff;

    if( CanExecTask ) {
        buff = Buff;
        buff.u.r.CS = FP_SEG( rtn );
        buff.u.r.IP = FP_OFF( rtn );
        buff.u.r.SS = FP_SEG( stack );
        buff.u.r.SP = FP_OFF( stack ) + sizeof( stack );
        WriteRegs( &buff );
        ExecuteCode( &buff );
        return( ( (unsigned long)buff.u.r.DX << 16 ) + buff.u.r.AX );
    } else {
        return( -1 );
    }
}


long TaskOpenFile( char __far *name, int mode, int flags ) {

    WriteBuffer( (byte __far *)name, FP_SEG( UtilBuff ), FP_OFF( UtilBuff ), strlen( name ) + 1 );
    Buff.u.r.DX = FP_SEG( UtilBuff );
    Buff.u.r.AX = FP_OFF( UtilBuff );
    Buff.u.r.BX = mode;
    Buff.u.r.CX = flags;
    return( TaskExecute( (excfn)DoOpen ) );
}


long TaskCloseFile( HFILE hdl )
{
    Buff.u.r.AX = hdl;
    return( TaskExecute( (excfn)DoClose ) );
}

HFILE TaskDupFile( HFILE old, HFILE new )
{
    Buff.u.r.AX = old;
    Buff.u.r.DX = new;
    return( TaskExecute( (excfn)DoDupFile ) );
}

trap_retval ReqGet_sys_config( void )
{
    get_sys_config_ret *ret;

    CHAR        npx;
    USHORT      version;
    USHORT      shift;
    long        emu;
    TRACEBUF    buff;
    char        tmp[108];

    ret = GetOutPtr(0);
    ret->sys.os = OS_OS2;
    DosGetVersion( &version );
    ret->sys.osmajor = version >> 8;
    ret->sys.osminor = version & 0xff;
    ret->sys.cpu = X86CPUType();
    DosDevConfig( &npx, 3, 0 );
    if( npx ) {
        if( ret->sys.cpu >= X86_486 ) {
            ret->sys.fpu = ret->sys.cpu & X86_CPU_MASK;
        } else {
            ret->sys.fpu = NPXType();
        }
    } else {
        ret->sys.fpu = X86_NO;
    }
    emu = TaskExecute( (excfn)&DoGetMSW );
    if( emu != -1 && (emu & 0x04) ) { /* if EM bit is on in the MSW */
        ret->sys.fpu = X86_EMU;
    }
    WriteRegs( &Buff );
    if( ret->sys.fpu != X86_NO ) {
        buff.cmd = PT_CMD_READ_8087;
        buff.segv = FP_SEG( tmp );
        buff.offv = FP_OFF( tmp );
        buff.tid = 1;
        buff.pid = Pid;
        DosPTrace( &buff );
        if( buff.cmd != PT_RET_SUCCESS ) ret->sys.fpu = X86_NO;
    }
    DosGetHugeShift( &shift );
    ret->sys.huge_shift = shift;
    ret->sys.mad = MAD_X86;
    return( sizeof( *ret ) );
}


trap_retval ReqMap_addr( void )
{
    map_addr_req *acc;
    map_addr_ret *ret;

    acc = GetInPtr(0);
    ret = GetOutPtr(0);
    if( Pid != 0 ) {
        Buff.cmd = PT_CMD_SEG_TO_SEL;
        Buff.value = acc->in_addr.segment;
        switch( Buff.value ) {
        case MAP_FLAT_CODE_SELECTOR:
        case MAP_FLAT_DATA_SELECTOR:
            Buff.value = 1;
            break;
        }
        Buff.mte = ModHandles[ acc->handle ];
        DosPTrace( &Buff );
        Buff.mte = ModHandles[ 0 ];
        ret->out_addr.segment = Buff.value;
    }
    ret->out_addr.offset = acc->in_addr.offset;
    ret->lo_bound = 0;
    ret->hi_bound = ~(addr48_off)0;
    return( sizeof( *ret ) );
}


trap_retval ReqAddr_info( void )
{
    addr_info_ret *retblk;

    retblk = GetOutPtr( 0 );
    retblk->is_big = FALSE;
    return( sizeof( *retblk ) );
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
    USHORT              offset;
    USHORT              length;
    ULONG               sum;
    checksum_mem_req    *acc;
    checksum_mem_ret    *ret;

    acc = GetInPtr(0);
    ret = GetOutPtr(0);
    length = acc->len;
    sum = 0;
    if( Pid != 0 ) {
        offset = acc->in_addr.offset;
        while( length != 0 ) {
            Buff.cmd = PT_CMD_READ_MEM_D;
            Buff.offv = offset;
            Buff.segv = acc->in_addr.segment;
            DosPTrace( &Buff );
            if( Buff.cmd != PT_RET_SUCCESS ) break;
            sum += Buff.value & 0xff;
            offset++;
            length--;
            if( length != 0 ) {
                sum += Buff.value >> 8;
                offset++;
                length--;
            }
        }
    }
    ret->result = sum;
    return( sizeof( *ret ) );
}


trap_retval ReqRead_mem( void )
{
    read_mem_req        *acc;
    unsigned            len;

    acc = GetInPtr(0);
    len = ReadBuffer( GetOutPtr(0), acc->mem_addr.segment, acc->mem_addr.offset, acc->len );
    return( len );
}


trap_retval ReqWrite_mem( void )
{
    write_mem_req       *acc;
    write_mem_ret       *ret;
    unsigned            len;

    acc = GetInPtr(0);
    ret = GetOutPtr(0);

    len = GetTotalSize() - sizeof( *acc );

    ret->len = WriteBuffer( GetInPtr( sizeof( *ret ) ), acc->mem_addr.segment, acc->mem_addr.offset, len );
    return( sizeof( *ret ) );
}

static void ReadCPU( struct x86_cpu *r )
{
    r->eax = Buff.u.r.AX;
    r->ebx = Buff.u.r.BX;
    r->ecx = Buff.u.r.CX;
    r->edx = Buff.u.r.DX;
    r->esi = Buff.u.r.SI;
    r->edi = Buff.u.r.DI;
    r->esp = Buff.u.r.SP;
    r->ebp = Buff.u.r.BP;
    r->eip = Buff.u.r.IP;
    r->efl = Buff.u.r.FL;
    r->ds  = Buff.u.r.DS;
    r->cs  = Buff.u.r.CS;
    r->es  = Buff.u.r.ES;
    r->ss  = Buff.u.r.SS;
}

static void WriteCPU( struct x86_cpu *r )
{
    Buff.u.r.AX = r->eax;
    Buff.u.r.BX = r->ebx;
    Buff.u.r.CX = r->ecx;
    Buff.u.r.DX = r->edx;
    Buff.u.r.SI = r->esi;
    Buff.u.r.DI = r->edi;
    Buff.u.r.SP = r->esp;
    Buff.u.r.BP = r->ebp;
    Buff.u.r.IP = r->eip;
    Buff.u.r.FL = r->efl;
    Buff.u.r.DS = r->ds;
    Buff.u.r.CS = r->cs;
    Buff.u.r.ES = r->es;
    Buff.u.r.SS = r->ss;
}

trap_retval ReqRead_cpu( void )
{
    word                *clr;
    read_cpu_ret        *ret;

    ret = GetOutPtr(0);
    for( clr = (word *)&ret->cpu;
        (char *)clr < (char *)&ret->cpu + sizeof(trap_cpu_regs); ++clr ) {
        *clr = 0;
    }
    if( Pid != 0 ) {
        ReadRegs( &Buff );
        ReadCPU( (struct x86_cpu *)&ret->cpu );
    }
    return( sizeof( read_cpu_ret ) );
}

trap_retval ReqRead_fpu( void )
{
    read_fpu_ret        *ret;
    TID                 save;

    ret = GetOutPtr(0);
    Buff.cmd = PT_CMD_READ_8087;
    Buff.segv = FP_SEG( ret );
    Buff.offv = FP_OFF( ret );
    save = Buff.tid;
    Buff.tid = 1;   /*NYI: OS/2 V1.2 gets upset trying to read other tids */
    DosPTrace( &Buff );
    Buff.tid = save;
    if( Buff.cmd == PT_RET_NO_NPX_YET ) return( 0 );
    FPUExpand( ret );
    return( sizeof( *ret ) );
}

trap_retval ReqWrite_cpu( void )
{
    trap_cpu_regs       *regs;

    regs = GetInPtr(sizeof(write_cpu_req));
    if( Pid != 0 ) {
        WriteCPU( (struct x86_cpu *)regs );
        WriteRegs( &Buff );
    }
    return( 0 );
}

trap_retval ReqWrite_fpu( void )
{
    trap_fpu_regs       *regs;
    TID                 save;

    regs = GetInPtr(sizeof(write_fpu_req));
    Buff.cmd = PT_CMD_WRITE_8087;
    Buff.segv = FP_SEG( regs );
    Buff.offv = FP_OFF( regs );
    FPUContract( regs );
    save = Buff.tid;
    Buff.tid = 1;   /*NYI: OS/2 V1.2 gets upset trying to read other tids */
    DosPTrace( &Buff );
    Buff.tid = save;
    return( 0 );
}

trap_retval ReqRead_regs( void )
{
    mad_registers       *mr;
    TID                 save;

    mr = GetOutPtr(0);
    memset( mr, 0, sizeof( mr->x86 ) );
    if( Pid != 0 ) {
        ReadRegs( &Buff );
        ReadCPU( &mr->x86.cpu );

        Buff.cmd = PT_CMD_READ_8087;
        Buff.segv = FP_SEG( mr );
        Buff.offv = FP_OFF( &mr->x86.u.fpu );
        save = Buff.tid;
        Buff.tid = 1;   /*NYI: OS/2 V1.2 gets upset trying to read other tids */
        DosPTrace( &Buff );
        Buff.tid = save;
        if( Buff.cmd != PT_RET_NO_NPX_YET ) {
            FPUExpand( &mr->x86.u.fpu );
        }
    }
    return( sizeof( mr->x86 ) );
}

trap_retval ReqWrite_regs( void )
{
    mad_registers       *mr;
    TID                 save;

    mr = GetInPtr(sizeof(write_regs_req));
    if( Pid != 0 ) {
        WriteCPU( &mr->x86.cpu );
        WriteRegs( &Buff );

        Buff.cmd = PT_CMD_WRITE_8087;
        Buff.segv = FP_SEG( mr );
        Buff.offv = FP_OFF( &mr->x86.u.fpu );
        FPUContract( &mr->x86.u.fpu );
        save = Buff.tid;
        Buff.tid = 1;   /*NYI: OS/2 V1.2 gets upset trying to read other tids */
        DosPTrace( &Buff );
        Buff.tid = save;
    }
    return( 0 );
}

USHORT LibLoadPTrace( TRACEBUF *buff )
{
    int         cmd;
    int         value;
    USHORT      rv;
    USHORT      offv;
    USHORT      segv;

    cmd = buff->cmd;
    value = buff->value;
    segv = buff->segv;
    offv = buff->offv;
    rv = DosPTrace( buff );
    RecordModHandle( buff->mte );
    for( ;; ) {
        if( buff->cmd != PT_RET_LIB_LOADED ) return( rv );
        RecordModHandle( buff->value );
        buff->value = value;
        buff->cmd = cmd;
        buff->offv = offv;
        buff->segv = segv;
        rv = DosPTrace( buff );
    }
}

#define EXE_IS_FULLSCREEN       0x0100
#define EXE_IS_PMC              0x0200
#define EXE_IS_PM               0x0300


static bool GetExeInfo( USHORT __far *pCS, USHORT __far *pIP, USHORT __far *pExeType, char __far *name )
{
    long        open_rc;
    HFILE       handle;
    USHORT      shorty;
    USHORT      read;
    ULONG       new_head;
    ULONG       pos;
    bool        rc;

    open_rc = OpenFile( name, 0, OPEN_PRIVATE );
    if( open_rc < 0 ) return( FALSE );
    handle = open_rc;
    rc = FALSE;
    for( ;; ) { /* guess */
        if( DosChgFilePtr( handle, 0x00, 0, &pos ) != 0 ) break;
        if( DosRead( handle, &shorty, sizeof( shorty ), &read ) != 0 ) break;
        if( read != sizeof( shorty ) ) break;
        if( shorty != 0x5a4d ) break;   /* MZ */

        if( DosChgFilePtr( handle, 0x18, 0, &pos ) != 0 ) break;
        if( DosRead( handle, &shorty, sizeof( shorty ), &read ) != 0 ) break;
        if( read != sizeof( shorty ) ) break;
        if( shorty < 0x40 ) break;      /* offset of relocation header */

        if( DosChgFilePtr( handle, 0x3c, 0, &pos ) != 0 ) break;
        if( DosRead( handle, &new_head, sizeof( new_head ), &read) != 0 ) break;
        if( read != sizeof( new_head ) ) break;

        if( DosChgFilePtr( handle, new_head, 0, &pos ) != 0 ) break;
        if( DosRead( handle, &shorty, sizeof( shorty ), &read ) != 0 ) break;
        if( read != sizeof( shorty ) ) break;
        if( shorty != 0x454e ) break;   /* NE */

        if( DosChgFilePtr( handle, new_head+0x0c, 0, &pos ) != 0 ) break;
        if( DosRead(handle,pExeType,sizeof( *pExeType ),&read) != 0 ) break;
        if( read != sizeof( *pExeType ) ) break;
        *pExeType &= 0x0700;

        if( DosChgFilePtr( handle, new_head+0x14, 0, &pos ) != 0 ) break;
        if( DosRead( handle, pIP, sizeof( *pIP ), &read ) != 0 ) break;
        if( read != sizeof( *pIP ) ) break;

        if( DosChgFilePtr( handle, new_head+0x16, 0, &pos ) != 0 ) break;
        if( DosRead( handle, pCS, sizeof( *pCS ), &read ) != 0 ) break;
        if( read != sizeof( *pCS ) ) break;
        rc = TRUE;
        break;
    }
    DosClose( handle );
    return( rc );
}


typedef struct {
        USHORT  phmod[2];               /* offset-segment */
        USHORT  mod_name[2];            /* offset-segment */
        USHORT  fail_len;
        PSZ     fail_name;              /* offset-segment */
        USHORT  hmod;
        CHAR    load_name[2];
} loadstack_t;

#define LOAD_THIS_DLL_SIZE      6

static bool CausePgmToLoadThisDLL( USHORT startCS, USHORT startIP )
{

    byte        savecode[LOAD_THIS_DLL_SIZE];
    USHORT      codesize;
    USHORT      len;
    loadstack_t __far *loadstack;
    USHORT      dll_name_len;
    char        this_dll[BUFF_SIZE];

    /* save a chunk of the program's code */
    if( DosGetModName( ThisDLLModHandle, BUFF_SIZE, this_dll ) != 0 ) {
        return( FALSE );
    }
    codesize = (char *)EndLoadThisDLL - (char *)LoadThisDLL;
    if( codesize > LOAD_THIS_DLL_SIZE )
        return( FALSE );
    len = ReadBuffer( savecode, startCS, startIP, codesize );
    if( Buff.cmd != PT_RET_SUCCESS )
        return( FALSE );
    if( len != codesize )
        return( FALSE );

    /* write the routine LoadThisDLL into program's code */
    len = WriteBuffer( (byte __far *)LoadThisDLL, startCS, startIP, codesize );
    if( len != codesize )
        return( FALSE );

    /* set up the stack for the routine LoadThisDLL */

    dll_name_len = ( strlen( this_dll ) + 1 ) & ~1;
    loadstack = Automagic( sizeof( loadstack_t ) + dll_name_len );
    Buff.u.r.SP -= sizeof( loadstack_t ) + dll_name_len;
    strcpy( loadstack->load_name, this_dll );
    loadstack->fail_name = NULL;
    loadstack->fail_len = 0;
    loadstack->mod_name[0] = Buff.u.r.SP + offsetof( loadstack_t, load_name );
    loadstack->mod_name[1] = Buff.u.r.SS;
    loadstack->phmod[0] = Buff.u.r.SP + offsetof( loadstack_t, hmod );
    loadstack->phmod[1] = Buff.u.r.SS;
    len = WriteBuffer( (byte __far *)loadstack, Buff.u.r.SS, Buff.u.r.SP, sizeof( loadstack_t ) + dll_name_len );
    if( len != sizeof( loadstack_t ) + dll_name_len )
        return( FALSE );

    /* execute LoadThisDLL on behalf of the program */

    WriteRegs( &Buff );
    ExecuteCode( &Buff );
    if( Buff.cmd != PT_RET_BREAK ) {
        WriteBuffer( savecode, startCS, startIP, codesize );
        return( FALSE );
    } else {
        WriteBuffer( savecode, startCS, startIP, codesize );
        return( TRUE );
    }
}


static void ExecuteUntil( USHORT CS, USHORT IP )
{
    opcode_type saved_opcode;
    opcode_type brk_opcode = BRKPOINT;

    ReadBuffer( &saved_opcode, CS, IP, sizeof( saved_opcode ) );
    WriteBuffer( &brk_opcode, CS, IP, sizeof( brk_opcode ) );
    do {
        Buff.cmd = PT_CMD_GO;
        LibLoadPTrace( &Buff );
        ReadRegs( &Buff );
    } while( Buff.u.r.CS != CS || Buff.u.r.IP != IP );
    WriteBuffer( &saved_opcode, CS, IP, sizeof( saved_opcode ) );
}


void AppSession( void )
{
    DosSelectSession( SID, 0 );
}

void DebugSession( void )
{
    DosSelectSession( 0, 0 );
}

trap_retval ReqProg_load( void )
{
    NEWSTARTDATA        start;
    char                *parms;
    char                *end;
    char                *prog;
    TRACEBUF            save;
    char                exe_name[255];
    USHORT              startCS, startIP;
    USHORT              exe_type;
    prog_load_ret       *ret;
    char                appname[200];

    ret = GetOutPtr( 0 );

    ExceptNum = -1;
    AtEnd = FALSE;
    prog = GetInPtr( sizeof( prog_load_req ) );
    if( FindFilePath( prog, exe_name, OS2ExtList ) != 0 ) {
        exe_name[0] = '\0';
    }
    parms = AddDriveAndPath( exe_name, UtilBuff );
    while( *prog != '\0' ) ++prog;
    ++prog;
    end = (char *)GetInPtr( GetTotalSize()-1 ) + 1;
    MergeArgvArray( prog, parms, end - prog );
    CanExecTask = TRUE;
    if( !GetExeInfo( &startCS, &startIP, &exe_type, UtilBuff ) ) {
        CanExecTask = FALSE;
        exe_type = EXE_IS_FULLSCREEN;
    }
    start.Length = 50;
    start.Related = 1;
    start.FgBg = !Remote;
    start.TraceOpt = 1;
    strcpy( appname, TRP_The_WATCOM_Debugger );
    strcat( appname, ": " );
    strcat( appname, exe_name );
    start.PgmTitle = appname;
    start.PgmName = UtilBuff;
    start.PgmInputs = (PBYTE)parms;
    start.TermQ = 0;
    start.Environment = NULL;
    start.InheritOpt = 1;
    start.IconFile = NULL;
    start.PgmHandle = 0;
    start.PgmControl = 0;
    if( exe_type == EXE_IS_PM ) {
        start.SessionType = SSF_TYPE_PM;
    } else {
        start.SessionType = SessionType; /* set by TaskInit */
    }

    ret->err = DosStartSession( (PSTARTDATA)&start, &SID, &Pid );
    switch( ret->err ) {
    case ERROR_SMG_START_IN_BACKGROUND:
        /* this one's OK */
        ret->err = 0;
        break;
    }
    ret->flags = LD_FLAG_IS_PROT;
    ret->task_id = Pid;
    Buff.pid = Pid;
    Buff.tid = 1;
    if( ret->err != 0 ) {
        Pid = 0;
    } else {
        Buff.cmd = PT_CMD_STOP;
        LibLoadPTrace( &Buff );
        if( Buff.cmd != PT_RET_SUCCESS ) {
            ret->err = 14; /* can't load */
            return( sizeof( *ret ) );
        }
        ReadRegs( &Buff );
        if( CanExecTask ) {
            Buff.cmd = PT_CMD_SEG_TO_SEL;
            Buff.value = startCS;
            Buff.mte = ModHandles[ 0 ];
            DosPTrace( &Buff );
            if( Buff.cmd == PT_RET_SUCCESS ) {
                Buff.mte = ModHandles[ 0 ];
                startCS = Buff.value;
                if( startCS != Buff.u.r.CS || startIP != Buff.u.r.IP ) {
                    ExecuteUntil( startCS, startIP );
                    ReadRegs( &Buff );
                }
                save.pid = Pid;
                save.tid = 1;
                ReadRegs( &save );
                if( !CausePgmToLoadThisDLL( startCS, startIP ) ) {
                    CanExecTask = FALSE;
                }
                WriteRegs( &save );
            } else {
                CanExecTask = FALSE;
            }
        }
        Buff.pid = Pid;
        Buff.tid = 1;
        ReadRegs( &Buff );
    }
    ret->flags |= LD_FLAG_HAVE_RUNTIME_DLLS;
    ret->mod_handle = 0;
    CurrModHandle = 1;
    return( sizeof( *ret ) );
}

trap_retval ReqProg_kill( void )
{
    prog_kill_ret       *ret;

    ret = GetOutPtr( 0 );
    SaveStdIn = NIL_DOS_HANDLE;
    SaveStdOut = NIL_DOS_HANDLE;
    if( Pid != 0 ) {
        Buff.cmd = PT_CMD_TERMINATE;
        DosPTrace( &Buff );
    }
    NumModHandles = 0;
    CurrModHandle = 1;
    Pid = 0;
    ret->err = 0;
    return( sizeof( *ret ) );
}


trap_retval ReqSet_break( void )
{
    opcode_type         brk_opcode;
    set_break_req       *acc;
    set_break_ret       *ret;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );

    ReadBuffer( &brk_opcode, acc->break_addr.segment, acc->break_addr.offset, sizeof( brk_opcode ) );
    ret->old = brk_opcode;
    brk_opcode = BRKPOINT;
    WriteBuffer( &brk_opcode, acc->break_addr.segment, acc->break_addr.offset, sizeof( brk_opcode ) );
    return( sizeof( *ret ) );
}

trap_retval ReqClear_break( void )
{
    clear_break_req     *bp;
    opcode_type         brk_opcode;

    bp = GetInPtr( 0 );
    brk_opcode = bp->old;
    WriteBuffer( &brk_opcode, bp->break_addr.segment, bp->break_addr.offset, sizeof( brk_opcode ) );
    return( 0 );
}

trap_retval ReqSet_watch( void )
{
    set_watch_req       *wp;
    set_watch_ret       *wr;
    dword               buff;

    wp = GetInPtr( 0 );
    wr = GetOutPtr( 0 );
    if( WatchCount < MAX_WP ) { // nyi - artificial limit (32 should be lots)
        WatchPoints[ WatchCount ].addr.segment = wp->watch_addr.segment;
        WatchPoints[ WatchCount ].addr.offset = wp->watch_addr.offset;
        ReadBuffer( (byte __far *)&buff, wp->watch_addr.segment, wp->watch_addr.offset, sizeof( dword ) );
        WatchPoints[ WatchCount ].value = buff;
        ++WatchCount;
    } else {
        wr->err = 84; /* out of structures */
    }
    wr->multiplier = 50000;
    return( sizeof( *wr ) );
}

trap_retval ReqClear_watch( void )
{
    clear_watch_req     *wp;
    watch               *dst;
    watch               *src;
    int                 i;

    wp = GetInPtr( 0 );
    dst = src = WatchPoints;
    for( i = 0; i < WatchCount; ++i ) {
        if( src->addr.segment != wp->watch_addr.segment || src->addr.offset != wp->watch_addr.offset ) {
            *dst = *src;
            ++dst;
        }
        ++src;
    }
    --WatchCount;
    return( 0 );
}

static volatile bool     BrkPending;

static void __pascal __far __loadds BrkHandler( USHORT sig_arg, USHORT sig_num )
{
    PFNSIGHANDLER   prev_hdl;
    USHORT          prev_act;

    sig_arg = sig_arg;
    BrkPending = TRUE;
    DosSetSigHandler( BrkHandler, &prev_hdl, &prev_act, 4, sig_num );
}

static unsigned MapReturn( unsigned changes )
{

    if( BrkPending ) {
        ReadRegs( &Buff );
        return( changes | COND_USER );
    }

    switch( Buff.cmd ) {
    case PT_RET_SUCCESS:
        return( changes );
    case PT_RET_SIGNAL:
        return( changes | COND_USER );
    case PT_RET_STEP:
        return( changes | COND_TRACE );
    case PT_RET_BREAK:
        return( changes | COND_BREAK );
    case PT_RET_PARITY:
        ExceptNum = 2;
        return( changes | COND_EXCEPTION );
    case PT_RET_FAULT:
        ExceptNum = 13;
        return( changes | COND_EXCEPTION );
    case PT_RET_WATCH:
        return( changes | COND_WATCH );
    case PT_RET_LIB_LOADED:
        RecordModHandle( Buff.value );
        return( changes | COND_LIBRARIES );
    case PT_RET_TRD_TERMINATE:
        return( changes );
    // Combined PT_RET_FUNERAL & PT_RET_ERROR with default
    // case PT_RET_FUNERAL:
    // case PT_RET_ERROR:
    default:
        CanExecTask = FALSE;
        AtEnd = TRUE;
        return( changes | COND_TERMINATE );
    }
}

static unsigned ProgRun( bool step )
{
    watch              *wp;
    int                 i;
    PFNSIGHANDLER       prev_brk_hdl;
    USHORT              prev_brk_act;
    PFNSIGHANDLER       prev_intr_hdl;
    USHORT              prev_intr_act;
    prog_go_ret         *ret;

    ExceptNum = -1;
    ret = GetOutPtr( 0 );
    if( NumModHandles > CurrModHandle ) {
        ret->conditions = COND_LIBRARIES;
        return( sizeof( *ret ) );
    }
    BrkPending = FALSE;
    DosSetSigHandler( BrkHandler, &prev_intr_hdl, &prev_intr_act, 2, SIG_CTRLC );
    DosSetSigHandler( BrkHandler, &prev_brk_hdl, &prev_brk_act, 2, SIG_CTRLBREAK );
    if( AtEnd ) {
        Buff.cmd = PT_RET_FUNERAL;
    } else if( step ) {
        Buff.cmd = PT_CMD_SINGLE_STEP;
        DosPTrace( &Buff );
    } else if( WatchCount != 0 ) {
        for( ;; ) {
            Buff.cmd = PT_CMD_SINGLE_STEP;
            DosPTrace( &Buff );
            if( Buff.cmd != PT_RET_STEP ) break;
            for( wp = WatchPoints,i = WatchCount; i > 0; ++wp, --i ) {
                Buff.cmd = PT_CMD_READ_MEM_D;
                Buff.segv = wp->addr.segment;
                Buff.offv = wp->addr.offset;
                DosPTrace( &Buff );
                if( Buff.value != wp->value ) {
                    Buff.cmd = PT_RET_WATCH;
                    goto leave;
                }
            }
        }
    } else {
        Buff.cmd = PT_CMD_GO;
        Buff.value = 0;
        if( DosPTrace( &Buff ) == ERROR_INTERRUPT ) {
            BrkPending = TRUE;
        }
        if( Buff.cmd == PT_RET_SUCCESS ) { /* a successful GO means pgm done! */
            Buff.cmd = PT_RET_FUNERAL;
        }
    }
leave:
    DosSetSigHandler( prev_brk_hdl, &prev_brk_hdl, &prev_brk_act,
                        prev_brk_act, SIG_CTRLBREAK );
    DosSetSigHandler( prev_brk_hdl, &prev_intr_hdl, &prev_intr_act,
                        prev_brk_act, SIG_CTRLC );
    ret->conditions = COND_CONFIG | COND_THREAD;
    if( NumModHandles > CurrModHandle ) {
        ret->conditions |= COND_LIBRARIES;
    }
    ret->conditions = MapReturn( ret->conditions );
    ret->program_counter.offset = Buff.u.r.IP;
    ret->program_counter.segment = Buff.u.r.CS;
    ret->stack_pointer.offset = Buff.u.r.SP;
    ret->stack_pointer.segment = Buff.u.r.SS;
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

trap_retval ReqFile_write_console( void )
{
    USHORT       len;
    USHORT       written_len;
    unsigned     save_ax;
    unsigned     save_dx;
    unsigned     save_bx;
    byte         *ptr;
    unsigned            curr;
    file_write_console_ret      *ret;

    ptr = GetInPtr( sizeof( file_write_console_req ) );
    len = GetTotalSize() - sizeof( file_write_console_req );
    ret = GetOutPtr( 0 );
    if( CanExecTask ) {
        ret->len = len;
        ret->err = 0;
        /* print/program request */
        save_ax = Buff.u.r.AX;
        save_dx = Buff.u.r.DX;
        save_bx = Buff.u.r.BX;
        ret->err = 0;
        while( len != 0 ) {
            curr = len;
            if( len > sizeof( UtilBuff ) ) len = sizeof( UtilBuff );
            WriteBuffer( ptr, FP_SEG( UtilBuff ), FP_OFF( UtilBuff ), curr );
            Buff.u.r.AX = FP_OFF( UtilBuff );
            Buff.u.r.DX = FP_SEG( UtilBuff );
            Buff.u.r.BX = curr;
            TaskExecute( (excfn)DoWritePgmScrn );
            ptr += curr;
            len -= curr;
        }
        Buff.u.r.AX = save_ax;
        Buff.u.r.DX = save_dx;
        Buff.u.r.BX = save_bx;
    } else {
        ret->err = DosWrite( 2, ptr, len, &written_len );
        ret->len = written_len;
    }
    return( sizeof( *ret ) );
}


static int ValidThread( TID thread )
{
    struct thd_state state;
    TID save;

    save = Buff.tid;
    Buff.tid = thread;
    Buff.cmd = PT_CMD_THREAD_STAT;
    Buff.segv = FP_SEG( &state );
    Buff.offv = FP_OFF( &state );
    DosPTrace( &Buff );
    Buff.tid = save;
    return( Buff.cmd == PT_RET_SUCCESS );
}

trap_retval ReqThread_get_next( void )
{
    thread_get_next_req *acc;
    thread_get_next_ret *ret;
    TID            thread;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );

    if( Pid != 0 ) {
        thread = acc->thread;
        while( ++thread <= 256 ) {
            if( ValidThread( thread ) ) {
                ret->thread = thread;
                //NYI:Assume all threads can be run
                ret->state = THREAD_THAWED;
                return( sizeof( *ret ) );
            }
        }
    }
    if( ret->thread == 0 ) {
        ret->thread = 1;
    } else {
        ret->thread = 0;
    }
    return( sizeof( *ret ) );
}

trap_retval ReqThread_set( void )
{
    thread_set_req      *acc;
    thread_set_ret      *ret;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    ret->err = 0;
    ret->old_thread = Buff.tid;
    if( acc->thread != 0 && ValidThread( acc->thread ) ) {
        Buff.tid = acc->thread;
        Buff.cmd = PT_CMD_READ_REGS;
        DosPTrace( &Buff );
    }
    return( sizeof( *ret ) );
}

static unsigned DoThread( trace_codes code )
{
    TID                 save;
    thread_thaw_req     *acc;
    thread_thaw_ret     *ret;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    if( ValidThread( acc->thread ) ) {
        save = Buff.tid;
        Buff.tid = acc->thread;
        Buff.cmd = code;
        DosPTrace( &Buff );
        Buff.tid = save;
        ret->err = 0;
    } else {
        ret->err = 1;   // failed
    }
    return( sizeof( *ret ) );
}

trap_retval ReqThread_freeze( void )
{
    return( DoThread( PT_CMD_FREEZE ) );
}

trap_retval ReqThread_thaw( void )
{
    return( DoThread( PT_CMD_RESUME ) );
}



trap_retval ReqGet_lib_name( void )
{
    get_lib_name_req    *acc;
    get_lib_name_ret    *ret;
    char                *name;

    acc = GetInPtr(0);
    ret = GetOutPtr(0);
    if( acc->handle != 0 ) {
        CurrModHandle = acc->handle + 1;
    }
    if( CurrModHandle >= NumModHandles ) {
        ret->handle = 0;
        return( sizeof( *ret ) );
    }
    name = GetOutPtr( sizeof(*ret) );
    Buff.value = ModHandles[ CurrModHandle ];
    Buff.segv = FP_SEG( name );
    Buff.offv = FP_OFF( name );
    Buff.cmd = PT_CMD_GET_LIB_NAME;
    DosPTrace( &Buff );
    ret->handle = CurrModHandle;
    return( sizeof( *ret ) + strlen( name ) + 1 );
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
    } else if( ExceptNum < sizeof( ExceptionMsgs ) / sizeof( ExceptionMsgs[0] ) ) {
        strcpy( err_txt, ExceptionMsgs[ExceptNum] );
    } else {
        strcpy( err_txt, TRP_EXC_unknown );
    }
    ExceptNum = -1;
    ret->flags = MSG_NEWLINE | MSG_ERROR;
    return( sizeof( *ret ) + strlen( err_txt ) + 1 );
}

trap_retval ReqGet_next_alias( void )
{
    get_next_alias_ret  *ret;

    ret = GetOutPtr( 0 );
    ret->seg = 0;
    ret->alias = 0;
    return( sizeof( *ret ) );
}

trap_version TRAPENTRY TrapInit( char *parm, char *err, unsigned_8 remote )
{
    trap_version        ver;
    USHORT              os2ver;
    SEL                 li,gi;
    __LINFOSEG          __far *linfo;

    parm = parm;
    Remote = remote;
    err[0] = '\0';
    ver.major = TRAP_MAJOR_VERSION;
    ver.minor = TRAP_MINOR_VERSION;
    ver.remote = FALSE;
    SaveStdIn = NIL_DOS_HANDLE;
    SaveStdOut = NIL_DOS_HANDLE;
    Screen = DEBUG_SCREEN;

    DosGetVersion( &os2ver );
    if( os2ver < 0x114 ) {
        strcpy( err, TRP_OS2_not_1p2 );
        return( ver );
    }
    if( DosGetInfoSeg( &gi, &li ) != 0  ) {
        strcpy( err, TRP_OS2_no_info );
        return( ver );
    }
    GblInfo = MK_FP( gi, 0 );
    linfo = MK_FP( li, 0 );
    if( linfo->typeProcess == _PT_FULLSCREEN ) {
        SessionType = SSF_TYPE_FULLSCREEN;
    } else {
        SessionType = SSF_TYPE_WINDOWABLEVIO;
    }
    ExceptNum = -1;
    return( ver );
}

void TRAPENTRY TrapFini()
{
}
