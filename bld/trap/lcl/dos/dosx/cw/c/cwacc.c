/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2009-2023 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  CauseWay trap file protected mode request handling.
*
****************************************************************************/


//#define DEBUG_TRAP

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <i86.h>
#include "roundmac.h"
#include "trpimp.h"
#include "trpcomm.h"
#include "trperr.h"
#include "doserr.h"
#include "madregs.h"
#include "dpmi.h"
#include "x86cpu.h"
#include "miscx87.h"
#include "ioports.h"
#include "dosredir.h"
#include "tinyio.h"
#include "exeos2.h"
#include "exeflat.h"
#include "brkptcpu.h"
#include "cwacc.h"


#define MAX_WATCHES         256
#define MAX_DREGS           3

#define ST_EXECUTING        0x01
#define ST_BREAK            0x02
#define ST_TRACE            0x04
#define ST_WATCH            0x08
#define ST_KEYBREAK         0x10
#define ST_TERMINATE        0x20
#define ST_LOAD_MODULE      0x40
#define ST_UNLOAD_MODULE    0x80

#define FLAT_SEL            0x73

#ifdef DEBUG_TRAP
extern void dos_printf( const char *format, ... );
#define _DBG( ... )   dos_printf( __VA_ARGS__ )
#define _DBG1( ... )   dos_printf( __VA_ARGS__ )
#define _DBG2( ... )   dos_printf( __VA_ARGS__ )
#else
#define _DBG( ... )
#define _DBG1( ... )
#define _DBG2( ... )
#endif

#define GetModuleHandle GetSelBase

typedef unsigned_16 selector;
typedef unsigned_16 segment;

#include "pushpck1.h"
typedef struct epsp_t {
    char            PSP[256];
    selector        parent;
    selector        next;
    void            *resource;
    void            *mcbHead;
    void            *mcbMaxAlloc;
    char            __far *DTA;
    selector        TransProt;
    segment         TransReal;
    unsigned_32     TransSize;
    void            __far *SSESP;
    void            *INTMem;
    selector        DPMIMem;
    unsigned_32     MemBase;
    unsigned_32     MemSize;
    selector        SegBase;
    unsigned_16     SegSize;
    unsigned_32     NearBase;
    segment         RealENV;
    void            *NextPSP;
    void            *LastPSP;
    void            *Exports;
    void            *Imports;
    unsigned_32     Links;
    unsigned_32     ExecCount;
    void            *EntryEIP;
    selector        EntryCS;
    selector        PSPSel;
    char            FileName[256];
} epsp_t;
#include "poppck.h"

typedef struct seg_t {
    unsigned_32 base;
    unsigned_32 size;
    unsigned_32 flags;
    unsigned_32 new_base;
} seg_t;

typedef struct mod_t {
    epsp_t          *epsp;
    bool            loaded;
    int             SegCount;
    seg_t           *ObjInfo;
} mod_t;

typedef struct watch_point {
    uint_64         value;
    dword           linear;
    addr48_ptr      addr;
    word            size;
    word            dregs;
    short           handle[MAX_DREGS];
} watch_point;

void dos_print( char *s );
#pragma aux dos_print = \
        "mov  ah,9" \
        "int  21h" \
    __parm      [__edx] \
    __value     \
    __modify    [__ax]

extern unsigned_32 GetSelBase( unsigned_16 );
#pragma aux GetSelBase = \
        "mov  ax,0FF08h" /* GetSelDet32 */ \
        "int  31h" \
    __parm      [__bx] \
    __value     [__edx] \
    __modify    [__ax __ebx __ecx]

extern int ReleaseSel( unsigned_16 );
#pragma aux ReleaseSel = \
        "mov  ax,0FF04h" /* ReleaseSel */ \
        "int  31h" \
        "sbb  eax,eax" \
    __parm      [__bx] \
    __value     [__eax]

extern int IsSel32bit( unsigned_16 );
#pragma aux IsSel32bit = \
        "movzx eax,ax" \
        "lar   eax,eax" \
        "and   eax,400000h" \
    __parm  [__ax] \
    __value [__eax]

extern void *malloc( unsigned );
#pragma aux malloc = \
        "mov   ax,0ff11h" \
        "int   31h" \
    __parm      [__ecx] \
    __value     [__esi] \
    __modify    [__ax]

extern void *realloc( void *, unsigned );
#pragma aux realloc = \
        "mov   ax,0ff13h" \
        "int   31h" \
    __parm      [__esi] [__ecx] \
    __value     [__esi] \
    __modify    [__ax]

extern void free( void * );
#pragma aux free = \
        "mov   ax,0ff15h" \
        "int   31h" \
    __parm      [__esi] \
    __value     \
    __modify    [__ax]

extern unsigned short GetPSP( void );
#pragma aux GetPSP = \
        "mov  ah,62h" \
        "int  21h" \
    __parm      [] \
    __value     [__bx] \
    __modify    [__ax]

extern int GetExecCount( unsigned_32 * );
#pragma aux GetExecCount = \
        "push  es" \
        "les   bx,[eax]" \
        "cmp   byte ptr es:[bx],1" \
        "sbb   eax,eax" \
        "pop   es" \
    __parm      [__eax] \
    __value     [__eax] \
    __modify    [__bx]

extern unsigned     MemoryCheck( unsigned_32, unsigned, unsigned );
extern size_t       MemoryRead( unsigned_32, unsigned, void *, size_t );
extern size_t       MemoryWrite( unsigned_32, unsigned, void *, size_t );
extern unsigned     Execute( void );
extern int          DebugLoad( char *prog_name, char *cmdl );
extern int          GrabVectors( void );
extern void         ReleaseVectors( void );

extern unsigned_8       Exception;
extern int              XVersion;
extern trap_cpu_regs    ProcRegs;
extern unsigned_16      ProcPSP;

bool                FakeBreak = false;

static unsigned_8   RealNPXType;
static watch_point  WatchPoints[MAX_WATCHES];
static int          WatchCount = 0;
static mod_t        *ModHandles = NULL;
static int          NumModHandles = 0;

static selector     flatCode = FLAT_SEL;
static selector     flatData = FLAT_SEL;

static opcode_type  BreakOpcode;

#ifdef DEBUG_TRAP
void dos_printf( const char *format, ... )
{
    static char     dbg_buf[256];
    va_list         args;
    size_t          len;

    va_start( args, format );
    vsnprintf( dbg_buf, sizeof( dbg_buf ), format, args );
    // Convert to DOS string
    len = strlen( dbg_buf );
    dbg_buf[len++] = '\r';
    dbg_buf[len++] = '\n';
    dbg_buf[len] = '\$';
    dos_print( dbg_buf );
    va_end( args );
}
#endif

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

static void ClearDebugRegs( void )
{
    int         i;
    int         j;
    watch_point *wp;

    for( wp = WatchPoints, i = WatchCount; i-- > 0; wp++ ) {
        for( j = 0; j < MAX_DREGS; j++ ) {
            if( wp->handle[j] >= 0 ) {
                DPMIClearWatch( wp->handle[j] );
                wp->handle[j] = -1;
            }
        }
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
        _DBG( "Setting Watch On 0x%X", linear );
        for( j = 0; j < wp->dregs; j++ ) {
            rc = DPMISetWatch( linear, size, DPMI_WATCH_WRITE );
            _DBG( "OK %d = %d", j, ( rc >= 0 ) );
            if( rc < 0 ) {
                ClearDebugRegs();
                return( false );
            }
            wp->handle[j] = rc;
            linear += size;
        }
    }
    return( true );
}

static bool CheckWatchPoints( void )
/**********************************/
{
    watch_point *wp;
    int         i;
    uint_64     value;

    for( wp = WatchPoints, i = WatchCount; i-- > 0; wp++ ) {
        value = 0;
        MemoryRead( wp->addr.offset, wp->addr.segment, &value, wp->size );
        if( wp->value != value ) {
            return( true );
        }
    }
    return( false );
}

int CheckTerminate( void )
/************************/
{
    if( (epsp_t *)GetModuleHandle( GetPSP() ) == ModHandles[0].epsp ) {
        return( GetExecCount( &ModHandles[0].epsp->ExecCount ) );
    }
    return( 0 );
}

static trap_conditions MapStateToCond( unsigned state )
/*****************************************************/
{
    trap_conditions rc;

    if( state & ST_TERMINATE ) {
        _DBG( "Condition: TERMINATE" );
        rc = COND_TERMINATE;
    } else if( state & ST_KEYBREAK ) {
        _DBG( "Condition: USER" );
        rc = COND_USER;
    } else if( state & ST_LOAD_MODULE ) {
        _DBG( "Condition: LIBRARIES" );
        rc = COND_LIBRARIES;
    } else if( state & ST_UNLOAD_MODULE ) {
        _DBG( "Condition: LIBRARIES" );
        rc = COND_LIBRARIES;
    } else if( state & ST_WATCH ) {
        _DBG( "Condition: WATCH" );
        rc = COND_WATCH;
    } else if( state & ST_BREAK ) {
        _DBG( "Condition: BREAK" );
        rc = COND_BREAK;
    } else if( state & ST_TRACE ) {
        _DBG( "Condition: TRACE" );
        rc = COND_TRACE;
    } else {
        _DBG( "Condition: EXCEPTION" );
        rc = COND_EXCEPTION;
    }
    return( rc );
}

static void AddModHandle( const char *name, epsp_t *epsp )
/********************************************************/
{
    mod_t           *mod;
    tiny_ret_t      rc;
    int             handle;
    object_record   obj;
    unsigned_32     ne_header_off;
    os2_flat_header os2_hdr;
    addr_off        new_base;
    unsigned        i;

    if( ModHandles == NULL ) {
        ModHandles = malloc( sizeof( mod_t ) );
    } else {
        ModHandles = realloc( ModHandles, ( NumModHandles + 1 ) * sizeof( mod_t ) );
    }
    mod = &ModHandles[NumModHandles];
    ++NumModHandles;
    mod->epsp = epsp;
    mod->loaded = true;
    mod->SegCount = 0;
    mod->ObjInfo = NULL;
    if( XVersion >= 0x404 ) {
        name = epsp->FileName;
    }
    rc = TinyOpen( name, TIO_READ );
    if( TINY_ERROR( rc ) ) {
        return;
    }
    handle = TINY_INFO( rc );
    TinySeek( handle, NE_HEADER_OFFSET, SEEK_SET );
    TinyRead( handle, &ne_header_off, sizeof( ne_header_off ) );
    TinySeek( handle, ne_header_off, SEEK_SET );
    TinyRead( handle, &os2_hdr, sizeof( os2_hdr ) );
    TinySeek( handle, os2_hdr.objtab_off + ne_header_off, SEEK_SET );
    mod->SegCount = os2_hdr.num_objects;
    mod->ObjInfo = malloc( os2_hdr.num_objects * sizeof( seg_t ) );
    new_base = 0;
    for( i = 0; i < os2_hdr.num_objects; ++i ) {
        TinyRead( handle, &obj, sizeof( obj ) );
        mod->ObjInfo[i].flags = obj.flags;
        mod->ObjInfo[i].base = obj.addr;
        mod->ObjInfo[i].size = obj.size;
        mod->ObjInfo[i].new_base = new_base;
        new_base += __ROUND_UP_SIZE_4K( obj.size );
        if( NumModHandles == 1 ) {      // main executable
            if( obj.flags & OBJ_BIG ) {
                if( obj.flags & OBJ_EXECUTABLE ) {
                    if( flatCode == FLAT_SEL ) {
                        flatCode = ( mod->epsp->SegBase + i * 8 ) | 3;
                    }
                } else {
                    if( flatData == FLAT_SEL ) {
                        flatData = ( mod->epsp->SegBase + i * 8 ) | 3;
                    }
                }
            }
        }
    }
    TinyClose( handle );
}

static void RemoveModHandle( epsp_t *epsp )
/******************************************/
{
    int     i;

    for( i = 0; i < NumModHandles; ++i ) {
        if( ModHandles[i].epsp == epsp ) {
            ModHandles[i].loaded = false;
            if( ModHandles[i].SegCount ) {
                free( ModHandles[i].ObjInfo );
                ModHandles[i].ObjInfo = NULL;
                ModHandles[i].SegCount = 0;
            }
            break;
        }
    }
}

static void FreeModsInfo( void )
/******************************/
{
    int     i;

    for( i = 0; i < NumModHandles; ++i ) {
        if( ModHandles[i].loaded ) {
            if( ModHandles[i].SegCount ) {
                free( ModHandles[i].ObjInfo );
                ModHandles[i].ObjInfo = NULL;
                ModHandles[i].SegCount = 0;
            }
        }
    }
    free( ModHandles );
    ModHandles = NULL;
    NumModHandles = 0;
}

static void AddModsInfo( const char *name, epsp_t *epsp )
/*******************************************************/
{
    if( XVersion >= 0x404 ) {
        for( ; epsp != NULL; epsp = epsp->NextPSP ) {
            AddModHandle( NULL, epsp );
        }
    } else {
        AddModHandle( name, epsp );
    }
}

trap_retval TRAP_CORE( Get_sys_config )( void )
/*********************************************/
{
    get_sys_config_ret  *ret;

    _DBG( "AccGetConfig" );
    ret = GetOutPtr( 0 );
    ret->os = DIG_OS_RATIONAL;      // Pretend we're DOS/4G
    ret->osmajor = _osmajor;
    ret->osminor = _osminor;
    ret->cpu = X86CPUType();
    ret->huge_shift = 12;
    ret->fpu = NPXType();       //RealNPXType;
    ret->arch = DIG_ARCH_X86;
    _DBG( "os = %d, cpu=%d, fpu=%d, osmajor=%d, osminor=%d",
        ret->os, ret->cpu, ret->fpu, ret->osmajor, ret->osminor );
    return( sizeof( *ret ) );
}

trap_retval TRAP_CORE( Map_addr )( void )
/***************************************/
{
    map_addr_req    *acc;
    map_addr_ret    *ret;
    unsigned_16     seg;
    seg_t           *seginfo;
    addr_off        off;
    mod_t           *mod;
    int             i;

    _DBG1( "AccMapAddr" );
    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    ret->out_addr.offset = 0;
    ret->out_addr.segment = 0;
    ret->lo_bound = 0;
    ret->hi_bound = 0;
    if( acc->mod_handle < NumModHandles && ModHandles[acc->mod_handle].loaded ) {
        mod = &ModHandles[acc->mod_handle];
        seg = acc->in_addr.segment;
        off = acc->in_addr.offset;
        ret->hi_bound = ~0;
        if( seg == MAP_FLAT_CODE_SELECTOR || seg == MAP_FLAT_DATA_SELECTOR ) {
            seg = 1;
            off += mod->ObjInfo[0].base;
            for( i = 0; i < mod->SegCount; ++i ) {
                seginfo = &mod->ObjInfo[i];
                if( seginfo->base <= off && ( seginfo->base + seginfo->size ) > off ) {
                    seg = i + 1;
                    off -= seginfo->base;
                    ret->lo_bound = seginfo->new_base + mod->epsp->MemBase;
                    ret->hi_bound = ret->lo_bound + seginfo->size - 1;
                    break;
                }
            }
        }
        --seg;
        // convert segment index to selector
        if( mod->ObjInfo[seg].flags & OBJ_BIG ) {
            ret->out_addr.segment = ( mod->ObjInfo[seg].flags & OBJ_EXECUTABLE ) ? flatCode : flatData ;
        } else {
            ret->out_addr.segment = ( mod->epsp->SegBase + seg * 8 ) | 3;
        }
        // convert offset
        ret->out_addr.offset = off + mod->ObjInfo[seg].new_base + mod->epsp->MemBase;
        _DBG( "Map_addr: module=%d %X:%X -> %X:%X", acc->mod_handle, acc->in_addr.segment, acc->in_addr.offset, ret->out_addr.segment, ret->out_addr.offset );
    }
    return( sizeof( *ret ) );
}

static size_t ReadMemory( addr48_ptr *addr, void *data, size_t len )
/******************************************************************/
{
    return( MemoryRead( addr->offset, addr->segment, data, len ) );
}

static size_t WriteMemory( addr48_ptr *addr, void *data, size_t len )
/*******************************************************************/
{
    return( MemoryWrite( addr->offset, addr->segment, data, len ) );
}

trap_retval TRAP_CORE( Checksum_mem )( void )
/*******************************************/
{
    size_t              len;
    size_t              i;
    size_t              want;
    size_t              got;
    unsigned_32         sum;
    checksum_mem_req    *acc;
    checksum_mem_ret    *ret;
    unsigned char       buffer[256];

    _DBG1(( "AccChkSum" ));

    acc = GetInPtr( 0 );
    want = sizeof( buffer );
    sum = 0;
    for( len = acc->len; len > 0; len -= got ) {
        if( want > len )
            want = len;
        got = ReadMemory( &acc->in_addr, buffer, want );
        for( i = 0; i < got; ++i ) {
            sum += buffer[i];
        }
        acc->in_addr.offset += got;
        if( got != want ) {
            break;
        }
    }
    ret = GetOutPtr( 0 );
    ret->result = sum;
    return( sizeof( ret ) );
}

trap_retval TRAP_CORE( Read_mem )( void )
/***************************************/
{
    read_mem_req        *acc;

    acc = GetInPtr( 0 );
    return( ReadMemory( &acc->mem_addr, GetOutPtr( 0 ), acc->len ) );
}

trap_retval TRAP_CORE( Write_mem )( void )
/****************************************/
{
    write_mem_req       *acc;
    write_mem_ret       *ret;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    ret->len = WriteMemory( &acc->mem_addr, GetInPtr( sizeof( *acc ) ),
                                GetTotalSizeIn() - sizeof( *acc ) );
    return( sizeof( *ret ) );
}

trap_retval TRAP_CORE( Read_io )( void )
/**************************************/
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
/***************************************/
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

static opcode_type place_breakpoint( addr48_ptr *addr )
{
    opcode_type     old_opcode;

    ReadMemory( addr, &old_opcode, sizeof( old_opcode ) );
    WriteMemory( addr, &BreakOpcode, sizeof( BreakOpcode ) );
    return( old_opcode );
}

static int remove_breakpoint( addr48_ptr *addr, opcode_type old_opcode )
{
    return( WriteMemory( addr, &old_opcode, sizeof( old_opcode ) ) != sizeof( old_opcode ) );
}

static unsigned ProgRun( bool step )
/**********************************/
{
    prog_go_ret *ret;
    unsigned    status;
    epsp_t      *epsp;

    _DBG1( "AccRunProg %X:%X", ProcRegs.CS, ProcRegs.EIP );
    ret = GetOutPtr( 0 );
    if( step ) {
        ProcRegs.EFL |= INTR_TF;
        status = Execute();
        ProcRegs.EFL &= ~INTR_TF;
    } else if( WatchCount > 0 ) {
        if( SetDebugRegs() ) {
            status = Execute();
            ClearDebugRegs();
            if( status & ST_TRACE ) {
                status |= ST_WATCH;
                status &= ~ST_TRACE;
            }
        } else {
            for( ;; ) {
                addr48_ptr  start_addr;
                byte        int_buff[2 + sizeof( opcode_type )];

                start_addr.segment = ProcRegs.CS;
                start_addr.offset = ProcRegs.EIP;
                /*
                 * have to breakpoint across software interrupts because Intel
                 * doesn't know how to design chips
                 */
                if( ReadMemory( &start_addr, int_buff, 2 + sizeof( opcode_type ) ) == 2 + sizeof( opcode_type ) && int_buff[0] == 0xcd ) {
                    opcode_type old_opcode;

                    start_addr.offset += 2;
                    old_opcode = place_breakpoint( &start_addr );
                    status = Execute();
                    start_addr.offset = ProcRegs.EIP;
                    remove_breakpoint( &start_addr, old_opcode );
                } else {
                    ProcRegs.EFL |= INTR_TF;
                    status = Execute();
                    ProcRegs.EFL &= ~INTR_TF;
                }
                if( (status & (ST_TRACE | ST_BREAK)) == 0 )
                    break;
                if( CheckWatchPoints() ) {
                    status |= ST_WATCH;
                    status &= ~(ST_TRACE | ST_BREAK);
                    break;
                }
            }
        }
    } else {
        status = Execute();
    }
    //handle module load/unload
    if( status & ST_LOAD_MODULE ) {
        epsp = (epsp_t *)ProcRegs.EDI;
        if( epsp->EntryCS != 0 ) {
            epsp->EntryCS = flatCode;   // set debugee flat selector for init routine
        }
        AddModHandle( NULL, epsp );
    } else if( status & ST_UNLOAD_MODULE ) {
        RemoveModHandle( (epsp_t *)ProcRegs.EDI );
    }
    ret->conditions = MapStateToCond( status );
    ret->conditions |= COND_CONFIG;
    // Now setup return value to reflect why we stopped execution.
    ret->program_counter.offset = ProcRegs.EIP;
    ret->program_counter.segment = ProcRegs.CS;
    ret->stack_pointer.offset = ProcRegs.ESP;
    ret->stack_pointer.segment = ProcRegs.SS;
    return( sizeof( *ret ) );
}

trap_retval TRAP_CORE( Prog_go )( void )
/**************************************/
{
    return( ProgRun( false ) );
}

trap_retval TRAP_CORE( Prog_step )( void )
/****************************************/
{
    return( ProgRun( true ) );
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
/****************************************/
{
    char            *src;
    char            *name;
    prog_load_ret   *ret;
    size_t          len;
    int             rc;
    char            cmdl[128];

    _DBG1( "AccLoadProg" );
    ret = GetOutPtr( 0 );
    ret->err = 0;
    src = name = GetInPtr( sizeof( prog_load_req ) );
    while( *src++ != '\0' )
        {}
    len = GetTotalSizeIn() - sizeof( prog_load_req ) - ( src - name );
    if( len > 126 )
        len = 126;
    *cmdl = MergeArgvArray( src, cmdl + 1, len );
    rc = DebugLoad( name, cmdl );
    _DBG1( "back from debugload - %d", rc );
    ret->flags = LD_FLAG_IS_BIG | LD_FLAG_IS_PROT | LD_FLAG_DISPLAY_DAMAGED | LD_FLAG_HAVE_RUNTIME_DLLS;
    ret->mod_handle = 0;
    if( rc == 0 ) {
        ret->task_id = ProcPSP;
        AddModsInfo( name, (epsp_t *)GetModuleHandle( ProcPSP ) );
    } else {
        ret->task_id = 0;
        if( rc == 1 ) {
            ret->err = ERR_ACCESS_DENIED;
        } else if( rc == 2 ) {
            ret->err = ERR_INVALID_FORMAT;
        } else if( rc == 3 ) {
            ret->err = ERR_INSUFFICIENT_MEMORY;
        } else {
            ret->err = rc;
        }
    }
    _DBG1( "done AccLoadProg" );
    return( sizeof( *ret ) );
}

trap_retval TRAP_CORE( Prog_kill )( void )
/****************************************/
{
    prog_kill_req       *acc;
    prog_kill_ret       *ret;

    _DBG( "AccKillProg" );
    acc = GetInPtr( 0 );
    RedirectFini();
    FreeModsInfo();
    ret = GetOutPtr( 0 );
    ret->err = 0;
    if( ReleaseSel( acc->task_id ) )
        ret->err = ERR_INVALID_HANDLE;
    return( sizeof( *ret ) );
}

static word GetDRInfo( word segment, dword offset, word size, dword *plinear )
{
    word    dregs;
    dword   linear;

    linear = GetSelBase( segment ) + offset;
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
/****************************************/
{
    set_watch_req   *acc;
    set_watch_ret   *ret;
    int             i;
    watch_point     *wp;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    ret->multiplier = 0;
    ret->err = ERR_INVALID_DATA;
    if( WatchCount < MAX_WATCHES ) {
        ret->err = 0;   // OK
        ret->multiplier = 5000;
        wp = WatchPoints + WatchCount;
        wp->addr.segment = acc->watch_addr.segment;
        wp->addr.offset = acc->watch_addr.offset;
        wp->size = acc->size;
        wp->value = 0;
        MemoryRead( wp->addr.offset, wp->addr.segment, &wp->value, wp->size );

        wp->dregs = GetDRInfo( wp->addr.segment, wp->addr.offset, wp->size, &wp->linear );
        for( i = 0; i < MAX_DREGS; i++ ) {
            wp->handle[i] = -1;
        }

        WatchCount++;
        if( DRegsCount() <= 4 ) {
            ret->multiplier = 10 | USING_DEBUG_REG;
        }
    }
    return( sizeof( *ret ) );
}

trap_retval TRAP_CORE( Clear_watch )( void )
/******************************************/
{
    /* assume all watches removed at same time */
    WatchCount = 0;
    return( 0 );
}

trap_retval TRAP_CORE( Set_break )( void )
/****************************************/
{
    set_break_req   *acc;
    set_break_ret   *ret;

    _DBG( "AccSetBreak" );
    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    ret->old = place_breakpoint( &acc->break_addr );
    return( sizeof( *ret ) );
}

trap_retval TRAP_CORE( Clear_break )( void )
/******************************************/
{
    clear_break_req *acc;

    _DBG( "AccClearBreak" );
    acc = GetInPtr( 0 );
    remove_breakpoint( &acc->break_addr, acc->old );
    return( 0 );
}

trap_retval TRAP_CORE( Get_next_alias )( void )
/*********************************************/
{
    get_next_alias_req  *acc;
    get_next_alias_ret  *ret;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    ret->seg = 0;
    ret->alias = 0;
    return( sizeof( *ret ) );
}

trap_retval TRAP_CORE( Get_lib_name )( void )
/*******************************************/
{
    char                *name;
    get_lib_name_req    *acc;
    get_lib_name_ret    *ret;
    int                 handle;
    size_t              name_maxlen;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    handle = acc->mod_handle + 1;
    if( handle >= NumModHandles ) {
        ret->mod_handle = 0;
        return( sizeof( *ret ) );
    }
    name = GetOutPtr( sizeof( *ret ) );
    *name = '\0';
    if( ModHandles[handle].loaded ) {
        name_maxlen = GetTotalSizeOut() - sizeof( *ret ) - 1;
        strncpy( name, ModHandles[handle].epsp->FileName, name_maxlen );
        name[name_maxlen] = '\0';
    }
    ret->mod_handle = handle;
    return( sizeof( *ret ) + strlen( name ) + 1 );
}

trap_retval TRAP_CORE( Get_err_text )( void )
/********************************************/
{
    static char *DosErrMsgs[] = {
        #define pick(a,b)   b,
        #include "dosmsgs.h"
        #undef pick
    };
    get_err_text_req    *acc;
    char                *err_txt;

    _DBG( "AccErrText" );
    acc = GetInPtr( 0 );
    err_txt = GetOutPtr( 0 );
    if( acc->err < ERR_LAST ) {
        strcpy( err_txt, DosErrMsgs[acc->err] );
        _DBG( "After strcpy" );
    } else {
        _DBG( "After acc->error_code > MAX_ERR_CODE" );
        strcpy( err_txt, TRP_ERR_unknown_system_error );
        ultoa( acc->err, err_txt + strlen( err_txt ), 16 );
        _DBG( "After utoa()" );
    }
    return( strlen( err_txt ) + 1 );
}

trap_retval TRAP_CORE( Get_message_text )( void )
/***********************************************/
{
    static const char * const ExceptionMsgs[] = {
        #define pick(a,b) b,
        #include "x86exc.h"
        #undef pick
    };
    get_message_text_ret    *ret;
    char                    *err_txt;

    ret = GetOutPtr( 0 );
    err_txt = GetOutPtr( sizeof(*ret) );
    if( Exception < sizeof( ExceptionMsgs ) / sizeof( ExceptionMsgs[0] ) ) {
        strcpy( err_txt, ExceptionMsgs[Exception] );
    } else {
        strcpy( err_txt, TRP_EXC_unknown );
    }
    ret->flags = MSG_NEWLINE | MSG_ERROR;
    return( sizeof( *ret ) + strlen( err_txt ) + 1 );
}

trap_retval TRAP_CORE( Read_regs )( void )
/****************************************/
{
    mad_registers       *mr;

    mr = GetOutPtr( 0 );
    *(&mr->x86.cpu) = ProcRegs;
    Read387( &mr->x86.u.fpu );
    return( sizeof( mr->x86 ) );
}

trap_retval TRAP_CORE( Write_regs )( void )
/*****************************************/
{
    mad_registers       *mr;

    mr = GetInPtr( sizeof( write_regs_req ) );
    ProcRegs = *(&mr->x86.cpu);
    Write387( &mr->x86.u.fpu );
    return( 0 );
}

trap_retval TRAP_CORE( Machine_data )( void )
/*******************************************/
{
    machine_data_req    *acc;
    machine_data_ret    *ret;
    machine_data_spec   *data;

    _DBG( "AccMachineData" );
    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    ret->cache_start = 0;
    ret->cache_end = ~(addr_off)0;
    if( acc->info_type == X86MD_ADDR_CHARACTERISTICS ) {
        data = GetOutPtr( sizeof( *ret ) );
        data->x86_addr_flags = ( IsSel32bit( acc->addr.segment ) ) ? X86AC_BIG : 0;
        _DBG( "address %x:%x is %s", acc->addr.segment, acc->addr.offset, data->x86_addr_flags ? "32-bit" : "16-bit" );
        return( sizeof( *ret ) + sizeof( data->x86_addr_flags ) );
    }
    _DBG( "address %x:%x", acc->addr.segment, acc->addr.offset );
    return( sizeof( *ret ) );
}

trap_version TRAPENTRY TrapInit( const char *parms, char *err, bool remote )
/**************************************************************************/
{
    trap_version    ver;
    char            ver_msg[] = "CauseWay API version = 0.00\r\n$";

    /* unused parameters */ (void)parms; (void)remote;

    err[0] = '\0'; /* all ok */
    ver.major = TRAP_VERSION_MAJOR;
    ver.minor = TRAP_VERSION_MINOR;
    ver.remote = false;
    RedirectInit();
    RealNPXType = NPXType();
    WatchCount = 0;
    FakeBreak = false;
    BreakOpcode = BRKPOINT;
    XVersion = GrabVectors();
    ver_msg[23] = XVersion / 256 + '0';
    ver_msg[25] = ( XVersion % 256 ) / 10 + '0';
    ver_msg[26] = XVersion % 256 % 10 + '0';
    dos_print( ver_msg );
    return( ver );
}

void TRAPENTRY TrapFini( void )
/*****************************/
{
    // Restore old interrupt/exception handlers
    ReleaseVectors();
}
