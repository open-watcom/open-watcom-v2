/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2009-2013 The Open Watcom Contributors. All Rights Reserved.
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
#include "trpimp.h"
#include "trperr.h"
#include "doserr.h"
#include "madregs.h"
#include "dpmi.h"
#include "x86cpu.h"
#include "miscx87.h"
#include "ioports.h"
#include "dosredir.h"
#include "doscomm.h"
#include "tinyio.h"
#include "exeos2.h"
#include "exeflat.h"
#include "cpuglob.h"
#include "cwacc.h"


#define MAX_WATCHES         256

#define ST_EXECUTING        0x01
#define ST_BREAK            0x02
#define ST_TRACE            0x04
#define ST_WATCH            0x08
#define ST_KEYBREAK         0x10
#define ST_TERMINATE        0x20
#define ST_LOAD_MODULE      0x40
#define ST_UNLOAD_MODULE    0x80

#define ALIGN4K(x)          (((x)+0x0fffL)&~0x0fffL)

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
#define GetLinAddr(x)   GetSelBase(x.segment)+x.offset

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

typedef struct hbrk_t {
    unsigned_32     address;
    unsigned_16     handle;
    char            type;
    unsigned_8      size;
    unsigned        inuse     :1;
    unsigned        installed :1;
} hbrk_t;

typedef struct watch_t {
    unsigned_32     address;
    unsigned_32     check;
    unsigned_8      length;
    unsigned        inuse     :1;
} watch_t;

void dos_print( char *s );
#pragma aux dos_print = \
    "mov  ah,9" \
    "int  21h" \
    parm [edx];

extern unsigned_32 GetSelBase( unsigned_16 );
#pragma aux GetSelBase = \
    "mov  ax,0FF08h" /* GetSelDet32 */ \
    "int  31h" \
    parm [bx] modify [eax ebx ecx] value [edx];

extern int RelSel( unsigned_16 );
#pragma aux RelSel = \
    "mov  ax,0FF04h" /* RelSel */ \
    "int  31h" \
    "sbb  eax,eax" \
    parm [bx] value [eax];

extern int IsSel32bit( unsigned_16 );
#pragma aux IsSel32bit = \
    "movzx eax,ax" \
    "lar   eax,eax" \
    "and   eax,400000h" \
    parm [ax] value [eax];

extern void *malloc( unsigned );
#pragma aux malloc = \
    "mov   ax,0ff11h" \
    "int   31h" \
    parm [ecx] value [esi];

extern void *realloc( void *, unsigned );
#pragma aux realloc = \
    "mov   ax,0ff13h" \
    "int   31h" \
    parm [esi] [ecx] value [esi];

extern void free( void * );
#pragma aux free = \
    "mov   ax,0ff11h" \
    "int   31h" \
    parm [esi];

extern unsigned short GetPSP( void );
#pragma aux GetPSP = \
    "mov  ah,62h" \
    "int  21h" \
    modify [ax] value [bx];

extern int GetExecCount( unsigned_32 * );
#pragma aux GetExecCount = \
    "push  es" \
    "les   bx,[eax]" \
    "cmp   byte ptr es:[bx],1" \
    "sbb   eax,eax" \
    "pop   es" \
    parm [eax] modify [ebx];

extern unsigned     MemoryCheck( unsigned_32, unsigned, unsigned );
extern unsigned     MemoryRead( unsigned_32, unsigned, void *, unsigned );
extern unsigned     MemoryWrite( unsigned_32, unsigned, void *, unsigned );
extern unsigned     Execute( bool );
extern int          DebugLoad( char *prog_name, char *cmdl );
extern int          GrabVectors( void );
extern void         ReleaseVectors( void );

extern unsigned_8       Exception;
extern int              XVersion;
extern trap_cpu_regs    DebugRegs;
extern unsigned_16      DebugPSP;

int                 WatchCount = 0;
bool                FakeBreak = FALSE;

static unsigned_8   RealNPXType;
static hbrk_t       HBRKTable[4];
static watch_t      WatchPoints[MAX_WATCHES];
static mod_t        *ModHandles = NULL;
static int          NumModHandles = 0;

static selector     flatCode = FLAT_SEL;
static selector     flatData = FLAT_SEL;

#ifdef DEBUG_TRAP
void dos_printf( const char *format, ... )
{
    static char     dbg_buf[256];
    va_list         args;

    va_start( args, format );
    vsnprintf( dbg_buf, sizeof( dbg_buf ), format, args );
    // Convert to DOS string
    dbg_buf[ strlen( dbg_buf ) ] = '\$';
    dos_print( dbg_buf );
    va_end( args );
}
#endif

static void HBRKInit( void )
/**************************/
{
    int     i;

    for( i = 0; i < 4; ++i ) {
        HBRKTable[i].inuse = FALSE;
    }
}

void SetHBRK( void )
/******************/
{
    int     i;

    // Install hardware break points.
    for( i = 0; i < 4; ++i ) {
        if( HBRKTable[i].inuse ) {
            dpmi_watch_handle   wh;

            wh = _DPMISetWatch( HBRKTable[i].address, HBRKTable[i].size, HBRKTable[i].type );
            if( wh >= 0 ) {
                HBRKTable[i].installed = TRUE;
                HBRKTable[i].handle = wh;
                _DPMIResetWatch( wh );
            }
        }
    }
}

void ResetHBRK( void )
/********************/
{
    int     i;

    // Uninstall hardware break points.
    for( i = 0; i < 4; ++i ) {
        if( HBRKTable[i].inuse && HBRKTable[i].installed ) {
            _DPMIClearWatch( HBRKTable[i].handle );
            HBRKTable[i].installed = FALSE;
        }
    }
}

int IsHardBreak( void )
/*********************/
{
    int     i;

    for( i = 0; i < 4; ++i ) {
        if( HBRKTable[i].inuse && HBRKTable[i].installed ) {
            if( _DPMITestWatch( HBRKTable[i].handle ) ) {
                return( TRUE );
            }
        }
    }
    return( FALSE );
}

int CheckWatchPoints( void )
/**************************/
{
    int         i;
    int         j;
    unsigned_8  *p;
    unsigned_32 sum;

    for( i = 0; i < MAX_WATCHES; ++i ) {
        if( WatchPoints[i].inuse ) {
            p = (unsigned_8 *)WatchPoints[i].address;
            sum = 0;
            for( j = 0; j < WatchPoints[i].length; ++j ) {
                sum += *(p++);
            }
            if( sum != WatchPoints[i].check ) {
                return( TRUE );
            }
        }
    }
    return( FALSE );
}

int CheckTerminate( void )
/************************/
{
    if( (epsp_t *)GetModuleHandle( GetPSP() ) == ModHandles[0].epsp ) {
        return( GetExecCount( &ModHandles[0].epsp->ExecCount ) );
    }
    return( 0 );
}

static int MapStateToCond( unsigned state )
/*****************************************/
{
    int     rc;

    if( state & ST_TERMINATE ) {
        _DBG( "Condition: TERMINATE\r\n" );
        rc = COND_TERMINATE;
    } else if( state & ST_KEYBREAK ) {
        _DBG( "Condition: USER\r\n" );
        rc = COND_USER;
    } else if( state & ST_LOAD_MODULE ) {
        _DBG( "Condition: LIBRARIES\r\n" );
        rc = COND_LIBRARIES;
    } else if( state & ST_UNLOAD_MODULE ) {
        _DBG( "Condition: LIBRARIES\r\n" );
        rc = COND_LIBRARIES;
    } else if( state & ST_WATCH ) {
        _DBG( "Condition: WATCH\r\n" );
        rc = COND_WATCH;
    } else if( state & ST_BREAK ) {
        _DBG( "Condition: BREAK\r\n" );
        rc = COND_BREAK;
    } else if( state & ST_TRACE ) {
        _DBG( "Condition: TRACE\r\n" );
        rc = COND_TRACE;
    } else {
        _DBG( "Condition: EXCEPTION\r\n" );
        rc = COND_EXCEPTION;
    }
    return( rc );
}

static void AddModHandle( char *name, epsp_t *epsp )
/**************************************************/
{
    mod_t           *mod;
    tiny_ret_t      rc;
    int             handle;
    object_record   obj;
    unsigned_32     off;
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
    mod->loaded = TRUE;
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
    TinySeek( handle, OS2_NE_OFFSET, SEEK_SET );
    TinyRead( handle, &off, sizeof( off ) );
    TinySeek( handle, off, SEEK_SET );
    TinyRead( handle, &os2_hdr, sizeof( os2_hdr ) );
    TinySeek( handle, os2_hdr.objtab_off + off, SEEK_SET );
    mod->SegCount = os2_hdr.num_objects;
    mod->ObjInfo = malloc( os2_hdr.num_objects * sizeof( seg_t ) );
    new_base = 0;
    for( i = 0; i < os2_hdr.num_objects; ++i ) {
        TinyRead( handle, &obj, sizeof( obj ) );
        mod->ObjInfo[i].flags = obj.flags;
        mod->ObjInfo[i].base = obj.addr;
        mod->ObjInfo[i].size = obj.size;
        mod->ObjInfo[i].new_base = new_base;
        new_base += ALIGN4K( obj.size );
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
        if( ModHandles[ i ].epsp == epsp ) {
            ModHandles[ i ].loaded = FALSE;
            if( ModHandles[ i ].SegCount ) {
                free( ModHandles[ i ].ObjInfo );
                ModHandles[ i ].ObjInfo = NULL;
                ModHandles[ i ].SegCount = 0;
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
        if( ModHandles[ i ].loaded ) {
            if( ModHandles[ i ].SegCount ) {
                free( ModHandles[ i ].ObjInfo );
                ModHandles[ i ].ObjInfo = NULL;
                ModHandles[ i ].SegCount = 0;
            }
        }
    }
    free( ModHandles );
    ModHandles = NULL;
    NumModHandles = 0;
}

static void AddModsInfo( char *name, epsp_t *epsp )
/*************************************************/
{
    if( XVersion >= 0x404 ) {
        for( ; epsp != NULL; epsp = epsp->NextPSP ) {
            AddModHandle( NULL, epsp );
        }
    } else {
        AddModHandle( name, epsp );
    }
}

trap_retval ReqGet_sys_config( void )
/********************************/
{
    get_sys_config_ret  *ret;

    _DBG( "AccGetConfig\r\n" );
    ret = GetOutPtr( 0 );
    ret->sys.os = MAD_OS_RATIONAL;      // Pretend we're DOS/4G
    ret->sys.osmajor = _osmajor;
    ret->sys.osminor = _osminor;
    ret->sys.cpu = X86CPUType();
    ret->sys.huge_shift = 12;
    ret->sys.fpu = NPXType();       //RealNPXType;
    ret->sys.mad = MAD_X86;
    _DBG( "os = %d, cpu=%d, fpu=%d, osmajor=%d, osminor=%d\r\n",
        ret->sys.os, ret->sys.cpu, ret->sys.fpu, ret->sys.osmajor, ret->sys.osminor );
    return( sizeof( *ret ) );
}

trap_retval ReqMap_addr( void )
/**************************/
{
    map_addr_req    *acc;
    map_addr_ret    *ret;
    unsigned_16     seg;
    seg_t           *seginfo;
    addr_off        off;
    mod_t           *mod;
    int             i;

    _DBG1( "AccMapAddr\r\n" );
    acc = GetInPtr(0);
    ret = GetOutPtr(0);
    ret->out_addr.offset = 0;
    ret->out_addr.segment = 0;
    ret->lo_bound = 0;
    ret->hi_bound = 0;
    if( acc->handle < NumModHandles && ModHandles[acc->handle].loaded ) {
        mod = &ModHandles[acc->handle];
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
        _DBG( "Map_addr: module=%d %X:%X -> %X:%X\n", acc->handle, acc->in_addr.segment, acc->in_addr.offset, ret->out_addr.segment, ret->out_addr.offset );
    }
    return( sizeof( *ret ) );
}

static trap_elen ReadMemory( addr48_ptr *addr, void *data, trap_elen len )
/************************************************************************/
{
    return( MemoryRead( addr->offset, addr->segment, data, len ) );
}

static trap_elen WriteMemory( addr48_ptr *addr, void *data, trap_elen len )
/*************************************************************************/
{
    return( MemoryWrite( addr->offset, addr->segment, data, len ) );
}

trap_retval ReqChecksum_mem( void )
/******************************/
{
    trap_elen           len;
    int                 i;
    trap_elen           read;
    checksum_mem_req    *acc;
    checksum_mem_ret    *ret;
    char                buffer[256];

    _DBG1(( "AccChkSum\n" ));

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    len = acc->len;
    ret->result = 0;
    while( len >= sizeof( buffer ) ) {
        read = ReadMemory( &acc->in_addr, buffer, sizeof( buffer ) );
        for( i = 0; i < read; ++i ) {
            ret->result += buffer[ i ];
        }
        if( read != sizeof( buffer ) )
            return( sizeof( *ret ) );
        len -= sizeof( buffer );
        acc->in_addr.offset += sizeof( buffer );
    }
    if( len != 0 ) {
        read = ReadMemory( &acc->in_addr, buffer, len );
        for( i = 0; i < read; ++i ) {
            ret->result += buffer[ i ];
        }
    }
    return( sizeof( ret ) );
}

trap_retval ReqRead_mem( void )
/**************************/
{
    read_mem_req        *acc;

    acc = GetInPtr( 0 );
    return( ReadMemory( &acc->mem_addr, GetOutPtr( 0 ), acc->len ) );
}

trap_retval ReqWrite_mem( void )
/***************************/
{
    write_mem_req       *acc;
    write_mem_ret       *ret;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    ret->len = WriteMemory( &acc->mem_addr, GetInPtr( sizeof( *acc ) ), GetTotalSize() - sizeof( *acc ) );
    return( sizeof( *ret ) );
}

trap_retval ReqRead_io( void )
/*************************/
{
    read_io_req         *acc;
    void                *data;

    acc = GetInPtr(0);
    data = GetOutPtr(0);
    if( acc->len == 1 ) {
        *(byte *)data = In_b( acc->IO_offset );
    } else if( acc->len == 2 ) {
        *(word *)data = In_w( acc->IO_offset );
    } else {
        *(dword *)data = In_d( acc->IO_offset );
    }
    return( acc->len );
}

trap_retval ReqWrite_io( void )
/**************************/
{
    trap_elen           len;
    write_io_req        *acc;
    write_io_ret        *ret;
    void                *data;

    acc = GetInPtr(0);
    data = GetInPtr( sizeof( *acc ) );
    len = GetTotalSize() - sizeof( *acc );
    ret = GetOutPtr(0);
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

static unsigned ProgRun( bool step )
/**********************************/
{
    prog_go_ret *ret;
    unsigned    status;
    epsp_t      *epsp;

    _DBG1( "AccRunProg %X:%X\n", DebugRegs.CS, DebugRegs.EIP );
    ret = GetOutPtr( 0 );
    status = Execute( step );
    //handle module load/unload
    if( status & ST_LOAD_MODULE ) {
        epsp = (epsp_t *)DebugRegs.EDI;
        if( epsp->EntryCS != 0 ) {
            epsp->EntryCS = flatCode;   // set debugee flat selector for init routine
        }
        AddModHandle( NULL, epsp );
    } else if( status & ST_UNLOAD_MODULE ) {
        RemoveModHandle( (epsp_t *)DebugRegs.EDI );
    }
    ret->conditions = MapStateToCond( status );
    ret->conditions |= COND_CONFIG;
    // Now setup return value to reflect why we stopped execution.
    ret->program_counter.offset = DebugRegs.EIP;
    ret->program_counter.segment = DebugRegs.CS;
    ret->stack_pointer.offset = DebugRegs.ESP;
    ret->stack_pointer.segment = DebugRegs.SS;
    return( sizeof( *ret ) );
}

trap_retval ReqProg_go( void )
/*************************/
{
    return( ProgRun( FALSE ) );
}

trap_retval ReqProg_step( void )
/***************************/
{
    return( ProgRun( TRUE ) );
}

trap_retval ReqProg_load( void )
/***************************/
{
    char            *src;
    char            *dst;
    char            *name;
    char            ch;
    prog_load_ret   *ret;
    unsigned        len;
    int             rc;
    char            cmdl[128];

    _DBG1( "AccLoadProg\r\n" );
    ret = GetOutPtr( 0 );
    src = name = GetInPtr( sizeof( prog_load_req ) );
    while( *src++ != '\0' ) {}
    len = GetTotalSize() - ( src - name ) - sizeof( prog_load_req );
    if( len > 126 )
        len = 126;
    dst = cmdl + 1;
    for( ; len > 0; --len ) {
        ch = *src++;
        if( ch == '\0' ) {
            if( len == 1 )
                break;   
            ch = ' ';
        }
        *dst++ = ch;
    }
    *dst = '\0';
    *cmdl = dst - cmdl - 1;
    rc = DebugLoad( name, cmdl );
    _DBG1( "back from debugload - %d\r\n", rc );
    ret->flags = LD_FLAG_IS_BIG | LD_FLAG_IS_PROT | LD_FLAG_DISPLAY_DAMAGED | LD_FLAG_HAVE_RUNTIME_DLLS;
    ret->mod_handle = 0;
    if( rc == 0 ) {
        ret->err = 0;
        ret->task_id = DebugPSP;
        AddModsInfo( name, (epsp_t *)GetModuleHandle( DebugPSP ) );
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
    _DBG1( "done AccLoadProg\r\n" );
    return( sizeof( *ret ) );
}

trap_retval ReqProg_kill( void )
/***************************/
{
    prog_kill_req       *acc;
    prog_kill_ret       *ret;

    _DBG( "AccKillProg\r\n" );
    acc = GetInPtr(0);
    ret = GetOutPtr( 0 );
    RedirectFini();
    FreeModsInfo();
    ret->err = ( RelSel( acc->task_id ) ) ? ERR_INVALID_HANDLE : 0;
    return( sizeof( *ret ) );
}

trap_retval ReqSet_watch( void )
/***************************/
{
    set_watch_req   *acc;
    set_watch_ret   *ret;
    int             i;
    int             j;
    unsigned_8      *p;
    unsigned_32     sum;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    if( acc->size == 1 || acc->size == 2 || acc->size == 4 ) {
        for( i = 0; i < 4; ++i ) {
            if( HBRKTable[i].inuse == FALSE ) {
                HBRKTable[i].inuse = TRUE;
                HBRKTable[i].installed = FALSE;
                HBRKTable[i].address = GetLinAddr( acc->watch_addr );
                HBRKTable[i].size = acc->size;
                HBRKTable[i].type = DPMI_WATCH_WRITE;
                ret->err = 0;
                ret->multiplier = 10 | USING_DEBUG_REG;
                return( sizeof( *ret ) );
            }
        }
    }
    if( WatchCount < MAX_WATCHES ) {
        for( i = 0; i < MAX_WATCHES; ++i ) {
            if( WatchPoints[i].inuse == FALSE ) {
                WatchPoints[i].inuse = TRUE;
                WatchPoints[i].address = GetLinAddr( acc->watch_addr );
                WatchPoints[i].length = acc->size;
                p = (unsigned_8 *)WatchPoints[i].address;
                sum = 0;
                for( j = 0; j < acc->size; ++j ) {
                    sum += *(p++);
                }
                WatchPoints[i].check = sum;
                ++WatchCount;
                ret->err = 0;
                ret->multiplier = 5000;
                return( sizeof( *ret ) );
            }
        }
    }
    ret->err = ERR_INVALID_DATA;
    ret->multiplier = 0;
    return( sizeof( *ret ) );
}

trap_retval ReqClear_watch( void )
/*****************************/
{
    clear_watch_req     *acc;
    int                 i;
    unsigned_32         watch_addr;

    acc = GetInPtr( 0 );
    watch_addr = GetLinAddr( acc->watch_addr );
    for( i = 0; i < 4; i++ ) {
        if( HBRKTable[i].inuse ) {
            if( HBRKTable[i].address == watch_addr ) {
                if( HBRKTable[i].size == acc->size ) {
                    HBRKTable[i].inuse = FALSE;
                    return( 0 );
                }
            }
        }
    }
    if( WatchCount ) {
        for( i = 0; i < MAX_WATCHES; ++i ) {
            if( WatchPoints[i].inuse ) {
                if( WatchPoints[i].address == watch_addr ) {
                    if( WatchPoints[i].length == acc->size ) {
                        WatchPoints[i].inuse = FALSE;
                        --WatchCount;
                        break;
                    }
                }
            }
        }
    }
    return( 0 );
}

trap_retval ReqSet_break( void )
/******************************/
{
    opcode_type     brk_opcode;
    set_break_req   *acc;
    set_break_ret   *ret;

    _DBG( "AccSetBreak\r\n" );
    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    ReadMemory( &acc->break_addr, &brk_opcode, sizeof( brk_opcode ) );
    ret->old = brk_opcode;
    brk_opcode = BRKPOINT;
    WriteMemory( &acc->break_addr, &brk_opcode, sizeof( brk_opcode ) );
    return( sizeof( *ret ) );
}

trap_retval ReqClear_break( void )
/*****************************/
{
    opcode_type     brk_opcode;
    clear_break_req *acc;

    _DBG( "AccClearBreak\r\n" );
    acc = GetInPtr( 0 );
    brk_opcode = acc->old;
    WriteMemory( &acc->break_addr, &brk_opcode, sizeof( brk_opcode ) );
    return( 0 );
}

trap_retval ReqGet_next_alias( void )
/***********************************/
{
    get_next_alias_req  *acc;
    get_next_alias_ret  *ret;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    ret->seg = 0;
    ret->alias = 0;
    return( sizeof( *ret ) );
}

trap_retval ReqGet_lib_name( void )
/******************************/
{
    char                *name;
    get_lib_name_req    *acc;
    get_lib_name_ret    *ret;
    int                 handle;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    ret->handle = 0;
    name = GetOutPtr( sizeof( *ret ) );
    *name = '\0';
    handle = acc->handle + 1;
    if( handle < NumModHandles ) {
        if( ModHandles[ handle ].loaded )
            strcpy( name, ModHandles[ handle ].epsp->FileName );
        ret->handle = handle;
    }
    return( sizeof( *ret ) + strlen( name ) + 1 );
}

trap_retval ReqGet_err_text( void )
/******************************/
{
    static char *DosErrMsgs[] = {
        #define pick(a,b)   b,
        #include "dosmsgs.h"
        #undef pick
    };
    get_err_text_req    *acc;
    char                *err_txt;

    _DBG( "AccErrText\r\n" );
    acc = GetInPtr( 0 );
    err_txt = GetOutPtr( 0 );
    if( acc->err < ERR_LAST ) {
        strcpy( err_txt, DosErrMsgs[ acc->err ] );
        _DBG( "After strcpy\r\n" );
    } else {
        _DBG( "After acc->error_code > MAX_ERR_CODE" );
        strcpy( err_txt, TRP_ERR_unknown_system_error );
        ultoa( acc->err, err_txt + strlen( err_txt ), 16 );
        _DBG( "After utoa()\r\n" );
    }
    return( strlen( err_txt ) + 1 );
}

trap_retval ReqGet_message_text( void )
/**********************************/
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

trap_retval ReqRead_regs( void )
/***************************/
{
    mad_registers       *mr;

    mr = GetOutPtr( 0 );
    *(&mr->x86.cpu) = DebugRegs;
    Read387( &mr->x86.u.fpu );
    return( sizeof( mr->x86 ) );
}

trap_retval ReqWrite_regs( void )
/****************************/
{
    mad_registers       *mr;

    mr = GetInPtr( sizeof( write_regs_req ) );
    DebugRegs = *(&mr->x86.cpu);
    Write387( &mr->x86.u.fpu );
    return( 0 );
}

trap_retval ReqMachine_data( void )
/******************************/
{
    machine_data_req    *acc;
    machine_data_ret    *ret;
    union {
        unsigned_8      charact;
    } *data;
    trap_elen           len;

    _DBG( "AccMachineData\r\n" );
    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    ret->cache_start = 0;
    ret->cache_end = 0;
    len = 0;
    if( acc->info_type == X86MD_ADDR_CHARACTERISTICS ) {
        ret->cache_end = ~(addr_off)0;
        data = GetOutPtr( sizeof( *ret ) );
        len = sizeof( data->charact );
        data->charact = 0;
        if( IsSel32bit( acc->addr.segment ) ) {
            data->charact = X86AC_BIG;
        }
    }
    _DBG( "address %x:%x is %s\r\n", acc->addr.segment, acc->addr.offset, data->charact ? "32-bit" : "16-bit" );
    return( sizeof( *ret ) + len );
}

trap_version TRAPENTRY TrapInit( const char *parms, char *err, bool remote )
/**************************************************************************/
{
    trap_version    ver;
    char            ver_msg[] = "CauseWay API version = 0.00\r\n$";

    parms=parms;remote=remote;
    err[0] = '\0'; /* all ok */
    ver.major = TRAP_MAJOR_VERSION;
    ver.minor = TRAP_MINOR_VERSION;
    ver.remote = FALSE;
    RedirectInit();
    RealNPXType = NPXType();
    HBRKInit();
    WatchCount = 0;
    FakeBreak = FALSE;
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
