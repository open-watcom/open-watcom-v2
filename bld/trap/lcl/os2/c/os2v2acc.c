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


#include <string.h>
#include <i86.h>
#define INCL_BASE
#define INCL_DOSDEVICES
#define INCL_DOSMEMMGR
#define INCL_DOSSIGNALS
#define INCL_DOSPROCESS
#undef  INCL_DOSINFOSEG
//#define INCL_DOSSESMGR
#include <wos2.h>
#include <os2dbg.h>
#include "os2v2acc.h"
#include "digcpu.h"
#include "trpld.h"
#include "trpcomm.h"
#include "trpsys.h"
#include "bsexcpt.h"
#include "wdpmhelp.h"
#include "softmode.h"
#include "madregs.h"
#include "trperr.h"
#include "os2err.h"
#include "exedos.h"
#include "exeos2.h"
#include "exeflat.h"
#include "x86cpu.h"
#include "brkptcpu.h"
#include "dbgthrd.h"
#include "os2path.h"
#include "accmisc.h"


#define MAX_OBJECTS         128

#define OBJECT_IS_CODE      0x0004L
#define OBJECT_IS_BIG       0x2000L

#define EXE_IS_FULLSCREEN   0x0100
#define EXE_IS_PMC          0x0200
#define EXE_IS_PM           0x0300

#define MAX_WATCHES     32

/* Structure used internally to set hardware watch points */
typedef struct watch_point {
    uint_64     value;
    addr48_ptr  addr;
    word        size;
} watch_point;

ULONG                   ExceptNum;
dos_debug               Buff;
USHORT                  TaskFS;
bool                    ExpectingAFault;

static bool             stopOnSecond;
static ULONG            ExceptLinear;
static UCHAR            TypeProcess;
static bool             Is32Bit;
static watch_point      WatchPoints[MAX_WATCHES];
static int              WatchCount = 0;
static unsigned_16      lastCS;
static unsigned_16      lastSS;
static unsigned_32      lastEIP;
static unsigned_32      lastESP;
static HMODULE          LastMTE;
static unsigned         NumObjects;
static object_record    ObjInfo[MAX_OBJECTS];
static HMODULE          *ModHandles = NULL;
static unsigned         NumModHandles = 0;
static unsigned         CurrModHandle = 0;

#ifdef DEBUG_OUT

void Out( char *str )
{
    USHORT      written;

    DosWrite( 1, str, _fstrlen( str ), &written );
}

#define NSIZE 20
void OutNum( ULONG i )
{
    char numbuff[NSIZE];
    char *ptr;

    ptr = numbuff + NSIZE;
    *--ptr = '\0';
    if( i == 0 ) {
        *--ptr = '0';
    }
    while( i != 0 ) {
        *--ptr = "0123456789abcdef"[i & 0x0f];
        i >>= 4;
    }
    Out( ptr );
}

#endif

static bool Is32BitSeg( unsigned seg )
{
    if( IsFlatSeg( seg ) )
        return( true );
    if( IsUnknownGDTSeg( seg ) )
        return( true );
    return( false );
}

/*
 * RecordModHandle - save module handle for later reference
 */
static void RecordModHandle( HMODULE value )
{
    SEL         sel;

    if( ModHandles == NULL ) {
        DosAllocSeg( sizeof( ULONG ), (PSEL)&sel, 0 );
        ModHandles = _MK_FP( sel, 0 );
    } else {
        DosReallocSeg( ( NumModHandles + 1 ) * sizeof( HMODULE ), _FP_SEG( ModHandles ) );
    }
    ModHandles[NumModHandles++] = value;
}


/*
 * SeekRead - seek to a file position, and read the data
 */
static bool SeekRead( HFILE handle, ULONG newpos, void *ptr, USHORT size )
{
    USHORT      read;
    ULONG       pos;

    if( DosChgFilePtr( handle, newpos, 0, &pos ) != 0 ) {
        return( false );
    }
    if( DosRead( handle, ptr, size, &read ) != 0 ) {
        return( false );
    }
    if( read != size ) {
        return( false );
    }
    return( true );

} /* SeekRead */


/*
 * FindNewHeader - get a pointer to the new exe header
 */
static bool FindNewHeader( HFILE hdl, ULONG *ne_header_off, USHORT *type )
{
    bool        rc;
    USHORT      data;

    rc = false;
    if( SeekRead( hdl, 0x00, &data, sizeof( data ) )
      && data == EXESIGN_DOS ) {
        if( SeekRead( hdl, DOS_RELOC_OFFSET, &data, sizeof( data ) )
          && NE_HEADER_FOLLOWS( data ) ) {
            if( SeekRead( hdl, NE_HEADER_OFFSET, ne_header_off, sizeof( *ne_header_off ) )
              && SeekRead( hdl, *ne_header_off, type, sizeof( *type ) ) ) {
                rc = true;
            }
        }
    }
    return( rc );

} /* FindNewHeader */

static void GetObjectInfo( HMODULE mte )
{
    HFILE               hdl;
    ULONG               ne_header_off;
    USHORT              type;
    unsigned_32         objoff;
    unsigned_32         numobjs;
    char                buff[256];
    long                open_rc;

    if( mte == LastMTE ) {
        return;
    }
    memset( ObjInfo, 0, sizeof( ObjInfo ) );
    DosGetModName( mte, 144, buff );
    NumObjects = 0;
    open_rc = OpenFile( buff, 0, OPEN_PRIVATE );
    if( open_rc >= 0 ) {
        hdl = open_rc;
        if( FindNewHeader( hdl, &ne_header_off, &type ) ) {
            if( type == EXESIGN_LE || type == EXESIGN_LX ) {
                SeekRead( hdl, ne_header_off + 0x40, &objoff, sizeof( objoff ) );
                SeekRead( hdl, ne_header_off + 0x44, &numobjs, sizeof( numobjs ) );
                if( numobjs <= MAX_OBJECTS ) {
                    SeekRead( hdl, ne_header_off + objoff, ObjInfo, numobjs * sizeof( ObjInfo[0] ) );
                    NumObjects = numobjs;
                }
                LastMTE = mte;
            }
        }
        DosClose( hdl );
    }
}


bool DebugExecute( dos_debug *buff, ULONG cmd, bool stop_on_module_load )
{
    EXCEPTIONREPORTRECORD       ex;
    ULONG                       value;
    ULONG                       stopvalue;
    ULONG                       notify=0;
    bool                        got_second_notification;
    ULONG                       fcp;
    CONTEXTRECORD               fcr;

    buff->Cmd = cmd;
    value = buff->Value;
    if( cmd == DBG_C_Go ) {
        value = 0;
    }
    stopvalue = XCPT_CONTINUE_EXECUTION;
    got_second_notification = false;
    if( cmd == DBG_C_Stop ) {
        stopvalue = XCPT_CONTINUE_STOP;
    }

    for( ;; ) {

        buff->Value = value;
        buff->Cmd = cmd;
        CallDosDebug( buff );

        value = stopvalue;
        cmd = DBG_C_Continue;

        /*
         * handle the preemptive notifications
         */
        switch( buff->Cmd ) {
        case DBG_N_ModuleLoad:
            RecordModHandle( buff->Value );
            if( stop_on_module_load )
                return( true );
            break;
        case DBG_N_ModuleFree:
            break;
        case DBG_N_NewProc:
            break;
        case DBG_N_ProcTerm:
            value = XCPT_CONTINUE_STOP;         /* halt us */
            notify = DBG_N_ProcTerm;
            break;
        case DBG_N_ThreadCreate:
            break;
        case DBG_N_ThreadTerm:
            break;
        case DBG_N_AliasFree:
            break;
        case DBG_N_Exception:
            ExceptLinear = buff->Addr;
            if( buff->Value == DBG_X_STACK_INVALID ) {
                value = XCPT_CONTINUE_SEARCH;
                break;
            }
            fcp = buff->Len;
            if( buff->Value == DBG_X_PRE_FIRST_CHANCE ) {
                ExceptNum = buff->Buffer;
                if( ExceptNum == XCPT_BREAKPOINT ) {
                    notify = DBG_N_Breakpoint;
                    value = XCPT_CONTINUE_STOP;
                    break;
                } else if( ExceptNum == XCPT_SINGLE_STEP ) {
                    notify = DBG_N_SStep;
                    value = XCPT_CONTINUE_STOP;
                    break;
                }
            }
            //
            // NOTE: Going to second chance causes OS/2 to report the
            //       exception in the debugee.  However, if you report
            //       the fault at the first chance notification, the
            //       debugee's own fault handlers will not get invoked!
            //
            if( buff->Value == DBG_X_FIRST_CHANCE && !ExpectingAFault ) {
                if( stopOnSecond && !got_second_notification ) {
                    value = XCPT_CONTINUE_SEARCH;
                    break;
                }
            }
            notify = DBG_N_Exception;
            value = XCPT_CONTINUE_STOP;

            /*
             * Buffer contains the ptr to the exception block
             */
            buff->Cmd = DBG_C_ReadMemBuf;
            buff->Addr = buff->Buffer;
            buff->Buffer = MakeLocalPtrFlat( (void *)&ex );
            buff->Len = sizeof( ex );
            CallDosDebug( buff );
            ExceptNum = ex.ExceptionNum;
            if( ExceptNum == XCPT_PROCESS_TERMINATE ||
                ExceptNum == XCPT_ASYNC_PROCESS_TERMINATE ||
                ExceptNum == XCPT_GUARD_PAGE_VIOLATION ||
                ( ExceptNum & XCPT_CUSTOMER_CODE ) ) {
                value = XCPT_CONTINUE_SEARCH;
                break;
            }

            /*
             * get the context record
             */
            buff->Cmd = DBG_C_ReadMemBuf;
            buff->Addr = fcp;
            buff->Buffer = MakeLocalPtrFlat( (void *)&fcr );
            buff->Len = sizeof( fcr );
            CallDosDebug( buff );
            buff->EAX = fcr.ctx_RegEax;
            buff->EBX = fcr.ctx_RegEbx;
            buff->ECX = fcr.ctx_RegEcx;
            buff->EDX = fcr.ctx_RegEdx;
            buff->ESI = fcr.ctx_RegEsi;
            buff->EDI = fcr.ctx_RegEdi;
            buff->ESP = fcr.ctx_RegEsp;
            buff->EBP = fcr.ctx_RegEbp;
            buff->DS = fcr.ctx_SegDs;
            buff->CS = fcr.ctx_SegCs;
            buff->ES = fcr.ctx_SegEs;
            buff->FS = fcr.ctx_SegFs;
            buff->GS = fcr.ctx_SegGs;
            buff->SS = fcr.ctx_SegSs;
            buff->EIP = fcr.ctx_RegEip;
            buff->EFlags = fcr.ctx_EFlags;
            WriteRegs( buff );

            if( ExpectingAFault || got_second_notification ) {
                break;
            }
            if( stopOnSecond ) {
                value = XCPT_CONTINUE_EXECUTION;
                got_second_notification = true;
            }
            break;
        default:
            if( notify != 0 ) {
                buff->Cmd = notify;
            }
            return( false );
        }
    }
//    return( false );
}


void WriteRegs( dos_debug *buff )
{
    buff->Cmd = DBG_C_WriteReg;
    CallDosDebug( buff );
}

void ReadRegs( dos_debug *buff )
{

    buff->Cmd = DBG_C_ReadReg;
    CallDosDebug( buff );
}

void ReadLinear( PVOID data, ULONG lin, USHORT size )
{
    Buff.Cmd = DBG_C_ReadMemBuf;
    Buff.Addr = lin;
    Buff.Buffer = MakeLocalPtrFlat( data );
    Buff.Len = size;
    CallDosDebug( &Buff );
}

void WriteLinear( PVOID data, ULONG lin, USHORT size )
{
    Buff.Cmd = DBG_C_WriteMemBuf;
    Buff.Addr = lin;
    Buff.Buffer = MakeLocalPtrFlat( data );
    Buff.Len = size;
    CallDosDebug( &Buff );
}

USHORT WriteBuffer( PBYTE data, USHORT segv, ULONG offv, USHORT size )
{
    USHORT      length;
    bool        is_ugs;
    USHORT      resdata;
    ULONG       flat;

    if( segv < 4 ) {
        return( 0 );
    }

    length = size;
    if( Pid != 0 ) {
        is_ugs = IsUnknownGDTSeg( segv );
        if( !is_ugs ) {
            flat = MakeItFlatNumberOne( segv, offv );
            WriteLinear( data, flat, size );
            if( Buff.Cmd == DBG_N_Success ) {
                return( size );
            }
        }
        while( length > 0 ) {
            Buff.Cmd = DBG_C_WriteMem_D;
            if( length == 1 ) {
                if( is_ugs ) {
                    if( !TaskReadWord( segv, offv, &resdata ) ) {
                        break;
                    }
                    resdata &= 0xff00;
                    resdata |= *data;
                    if( !TaskWriteWord( segv, offv, resdata ) ) {
                        break;
                    }
                } else {
                    Buff.Cmd = DBG_C_ReadMem_D;
                    Buff.Addr = MakeItFlatNumberOne( segv, offv );
                    CallDosDebug( &Buff );
                    Buff.Cmd = DBG_C_WriteMem_D;
                    Buff.Addr = MakeItFlatNumberOne( segv, offv );
                    Buff.Value &= 0xff00;
                    Buff.Value |= *data;
                    CallDosDebug( &Buff );
                    if( Buff.Cmd != DBG_N_Success ) {
                        break;
                    }
                }
                data++;
                length--;
                offv++;
            } else {
                resdata = *data;
                data++;
                resdata |= *data << 8;
                data++;
                if( is_ugs ) {
                    if( !TaskWriteWord( segv, offv, resdata ) ) {
                        break;
                    }
                } else {
                    Buff.Value = resdata;
                    Buff.Addr = MakeItFlatNumberOne( segv, offv );
                    CallDosDebug( &Buff );
                    if( Buff.Cmd != DBG_N_Success ) {
                        break;
                    }
                }
                length -= 2;
                offv += 2;
            }
        }
    }
    return( size - length ); /* return amount written */
}


static USHORT ReadBuffer( PBYTE data, USHORT segv, ULONG offv, USHORT size )
{
    USHORT      length;
    bool        is_ugs;
    USHORT      resdata;
    ULONG       flat;

    if( segv < 4 ) {
        return( 0 );
    }
    length = size;
    if( Pid != 0 ) {
        is_ugs = IsUnknownGDTSeg( segv );
        if( !is_ugs ) {
            flat = MakeItFlatNumberOne( segv, offv );
            ReadLinear( data, flat, size );
            if( Buff.Cmd == DBG_N_Success ) {
                return( size );
            }
        }
        while( length > 0 ) {
            if( is_ugs ) {
                if( !TaskReadWord( segv, offv, &resdata ) ) {
                    break;
                }
            } else {
                Buff.Cmd = DBG_C_ReadMem_D;
                Buff.Addr = MakeItFlatNumberOne( segv, offv );
                CallDosDebug( &Buff );
                if( Buff.Cmd != DBG_N_Success ) {
                    break;
                }
                resdata = Buff.Value;
            }
            *data = resdata & 0xff;
            data++;
            offv++;
            length--;
            if( length != 0 ) {
                *data = resdata >> 8;
                data++;
                offv++;
                length--;
            }
        }
    }
    return( size - length );
}


void DoWritePgmScrn( PCHAR buff, USHORT len )
{
    USHORT  written;

    DosWrite( 2, buff, len, &written );
    BreakPointParm( 0 );
}

trap_retval TRAP_CORE( Get_sys_config )( void )
{
    USHORT        version;
    USHORT        shift;
    dos_debug     buff;
    char          tmp[DBG_CO_SIZE];
    get_sys_config_ret  *ret;

    ret = GetOutPtr( 0 );
    ret->os = DIG_OS_OS2;
    DosGetVersion( &version );
    ret->osminor = version & 0xff;
    ret->osmajor = version >> 8;
    ret->cpu = X86CPUType();
    ret->fpu = ret->cpu & X86_CPU_MASK;
    WriteRegs( &Buff );

    buff.Cmd = DBG_C_ReadCoRegs;
    buff.Buffer = MakeLocalPtrFlat( tmp );
    buff.Tid = 1;
    buff.Pid = Pid;
    buff.Value = DBG_CO_387;    /* for 2.0: DBG_CO_387 */
    buff.Len = DBG_CO_SIZE;     /* for 2.0: size of register state */
    buff.Index = 0;             /* for 2.0: must be 0 */
    CallDosDebug( &buff );
    if( buff.Cmd != DBG_N_Success ) {
        ret->fpu = X86_NOFPU;
    }
    DosGetHugeShift( &shift );
    ret->huge_shift = shift;
    ret->arch = DIG_ARCH_X86;
    return( sizeof( *ret ) );
}


trap_retval TRAP_CORE( Map_addr )( void )
{
    USHORT              seg;
    ULONG               flags;
    map_addr_req        *acc;
    map_addr_ret        *ret;
    unsigned            i;
    addr_off            off;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    ret->lo_bound = 0;
    ret->hi_bound = ~(addr48_off)0;
    if( Pid == 0 ) {
        ret->out_addr = acc->in_addr;
        return( sizeof( *ret ) );
    }

    GetObjectInfo( ModHandles[acc->mod_handle] );

    seg = acc->in_addr.segment;
    off = acc->in_addr.offset;
    switch( seg ) {
    case MAP_FLAT_CODE_SELECTOR:
    case MAP_FLAT_DATA_SELECTOR:
        seg = 1;
        off += ObjInfo[0].addr;
        for( i = 0; i < NumObjects; ++i ) {
            if( ObjInfo[i].addr <= off
             && (ObjInfo[i].addr + ObjInfo[i].size) > off ) {
                seg = i + 1;
                off -= ObjInfo[i].addr;
                ret->lo_bound = ObjInfo[i].addr - ObjInfo[0].addr;
                ret->hi_bound = ret->lo_bound + ObjInfo[i].size - 1;
                break;
            }
        }
        break;
    }

    Buff.MTE = ModHandles[acc->mod_handle];
    Buff.Cmd = DBG_C_NumToAddr;
    Buff.Value = seg;
    CallDosDebug( &Buff );
    Buff.MTE = ModHandles[0];
    flags = ObjInfo[seg - 1].flags;
    if( flags & OBJECT_IS_BIG ) {
        ret->out_addr.segment = (flags & OBJECT_IS_CODE) ? FlatCS : FlatDS;
        ret->out_addr.offset = Buff.Addr + off;
    } else {
        Buff.Cmd = DBG_C_LinToSel;
        CallDosDebug( &Buff );
        ret->out_addr.segment = Buff.Value;
        ret->out_addr.offset = Buff.Index + off;
    }
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
        data->x86_addr_flags = ( Is32BitSeg( acc->addr.segment ) ) ? X86AC_BIG : 0;
        return( sizeof( *ret ) + sizeof( data->x86_addr_flags ) );
    }
    return( sizeof( *ret ) );
}

trap_retval TRAP_CORE( Checksum_mem )( void )
{
    ULONG               offset;
    USHORT              segment;
    USHORT              len;
    ULONG               sum;
    checksum_mem_req    *acc;
    checksum_mem_ret    *ret;

    sum = 0;
    if( Pid != 0 ) {
        acc = GetInPtr( 0 );
        offset = acc->in_addr.offset;
        segment = acc->in_addr.segment;
        for( len = acc->len; len != 0; ) {
            Buff.Cmd = DBG_C_ReadMem_D;
            Buff.Addr = MakeItFlatNumberOne( segment, offset );
            CallDosDebug( &Buff );
            if( Buff.Cmd != DBG_N_Success ) {
                break;
            }
            sum += Buff.Value & 0xff;
            offset++;
            len--;
            if( len > 0 ) {
                sum += Buff.Value >> 8;
                offset++;
                len--;
            }
        }
    }
    ret = GetOutPtr( 0 );
    ret->result = sum;
    return( sizeof( *ret ) );
}


trap_retval TRAP_CORE( Read_mem )( void )
{
    read_mem_req        *acc;

    acc = GetInPtr( 0 );
    return( ReadBuffer( GetOutPtr( 0 ), acc->mem_addr.segment, acc->mem_addr.offset, acc->len ) );
}


trap_retval TRAP_CORE( Write_mem )( void )
{
    write_mem_req       *acc;
    write_mem_ret       *ret;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    ret->len = WriteBuffer( GetInPtr( sizeof( *acc ) ), acc->mem_addr.segment,
                        acc->mem_addr.offset, GetTotalSizeIn() - sizeof( *acc ) );
    return( sizeof( *ret ) );
}

static void ReadCPU( struct x86_cpu *r )
{
    r->eax = Buff.EAX;
    r->ebx = Buff.EBX;
    r->ecx = Buff.ECX;
    r->edx = Buff.EDX;
    r->esi = Buff.ESI;
    r->edi = Buff.EDI;
    r->esp = Buff.ESP;
    r->ebp = Buff.EBP;
    r->ds = Buff.DS;
    r->cs = Buff.CS;
    r->es = Buff.ES;
    r->ss = Buff.SS;
    r->fs = Buff.FS;
    r->gs = Buff.GS;
    r->eip = Buff.EIP;
    r->efl = Buff.EFlags;
    if( !Is32BitSeg( Buff.CS ) ) {
        r->eip &= 0xffff;
    }
    if( !Is32BitSeg( Buff.SS ) ) {
        r->esp &= 0xffff;
        r->ebp &= 0xffff;
    }
}

static void WriteCPU( struct x86_cpu *r )
{
    Buff.EAX = r->eax ;
    Buff.EBX = r->ebx ;
    Buff.ECX = r->ecx ;
    Buff.EDX = r->edx ;
    Buff.ESI = r->esi ;
    Buff.EDI = r->edi ;
    Buff.ESP = r->esp ;
    Buff.EBP = r->ebp ;
    Buff.DS = r->ds ;
    Buff.CS = r->cs ;
    Buff.ES = r->es ;
    Buff.SS = r->ss ;
    Buff.FS = r->fs ;
    Buff.GS = r->gs ;
    Buff.EIP = r->eip ;
    Buff.EFlags = r->efl ;
    lastSS = Buff.SS;
    lastESP = Buff.ESP;
    lastCS = Buff.CS;
    lastEIP = Buff.EIP;
}

trap_retval TRAP_CORE( Read_regs )( void )
{
    mad_registers       *mr;

    mr = GetOutPtr( 0 );
    memset( mr, 0, sizeof( mr->x86 ) );
    if( Pid != 0 ) {
        ReadRegs( &Buff );
        ReadCPU( &mr->x86.cpu );
        Buff.Cmd = DBG_C_ReadCoRegs;
        Buff.Buffer = (ULONG) MakeLocalPtrFlat( &mr->x86.u.fpu );
        Buff.Value = DBG_CO_387;        /* for 2.0: DBG_CO_387 */
        Buff.Len = DBG_CO_SIZE;         /* for 2.0: size of register state */
        Buff.Index = 0;                 /* for 2.0: must be 0 */
        CallDosDebug( &Buff );
    }
    return( sizeof( mr->x86 ) );
}

trap_retval TRAP_CORE( Write_regs )( void )
{
    mad_registers       *mr;

    mr = GetInPtr(sizeof(write_regs_req));
    if( Pid != 0 ) {
        WriteCPU( &mr->x86.cpu );
        WriteRegs( &Buff );
        Buff.Cmd = DBG_C_WriteCoRegs;
        Buff.Buffer = (ULONG) MakeLocalPtrFlat( &mr->x86.u.fpu );
        Buff.Value = DBG_CO_387;        /* for 2.0: DBG_CO_387 */
        Buff.Len = DBG_CO_SIZE;         /* for 2.0: buffer size */
        Buff.Index = 0;                 /* for 2.0: must be zero */
        CallDosDebug( &Buff );
    }
    return( 0 );
}

trap_retval TRAP_CORE( Get_lib_name )( void )
{
    get_lib_name_req    *acc;
    get_lib_name_ret    *ret;
    char                *name;
    size_t              max_len;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    if( acc->mod_handle != 0 ) {
        CurrModHandle = acc->mod_handle + 1;
    }
    if( CurrModHandle >= NumModHandles ) {
        ret->mod_handle = 0;
        return( sizeof( *ret ) );
    }
    Buff.Value = ModHandles[CurrModHandle];
    max_len = GetTotalSizeOut() - sizeof( *ret ) - 1;
    name = GetOutPtr( sizeof( *ret ) );
    DosGetModName( ModHandles[CurrModHandle], max_len + 1, name );
    name[max_len] = '\0';
    ret->mod_handle = CurrModHandle;
    return( sizeof( *ret ) + strlen( name ) + 1 );
}

static USHORT GetEXEFlags( const char *name )
{
    HFILE       hdl;
    USHORT      type;
    ULONG       ne_header_off;
    USHORT      exeflags;
    long        open_rc;

    exeflags = 0;
    open_rc = OpenFile( (char *)name, 0, OPEN_PRIVATE );
    if( open_rc >= 0 ) {
        hdl = open_rc;
        if( FindNewHeader( hdl, &ne_header_off, &type ) ) {
            if( type == EXESIGN_NE ) {
                SeekRead( hdl, ne_header_off + 0x0c, &exeflags, sizeof( exeflags ) );
                exeflags &= 0x0700;
            } else if( type == EXESIGN_LE || type == EXESIGN_LX ) {
                SeekRead( hdl, ne_header_off + 0x10, &exeflags, sizeof( exeflags ) );
                exeflags &= 0x0700;
            }
        }
        DosClose( hdl );
    }
    return( exeflags );

} /* GetEXEFlags */

static bool setExeInfo( ULONG *pLin, ULONG objnum, ULONG eip )
{
    Buff.Cmd = DBG_C_NumToAddr;
    Buff.Value = objnum;
    Buff.MTE = ModHandles[0];
    CallDosDebug( &Buff );
    if( Buff.Cmd != DBG_N_Success ) {
        return( false );;
    }
    *pLin = eip + Buff.Addr;
    Buff.MTE = ModHandles[0];

    return( true );
}

static bool FindLinearStartAddress( ULONG *pLin, const char *name )
{
    bool        rc;
    HFILE       hdl;
    USHORT      type;
    ULONG       ne_header_off;
    ULONG       objnum;
    USHORT      sobjn;
    ULONG       eip;
    USHORT      ip;
    long        open_rc;

    rc = false;
    open_rc = OpenFile( (char *)name, 0, OPEN_PRIVATE );
    if( open_rc >= 0 ) {
        hdl = open_rc;
        if( FindNewHeader( hdl, &ne_header_off, &type ) ) {
            if( type == EXESIGN_NE ) {
                if( SeekRead( hdl, ne_header_off + 0x14, &ip, sizeof( ip ) ) ) {
                    if( SeekRead( hdl, ne_header_off + 0x16, &sobjn, sizeof( sobjn ) ) ) {
                        Is32Bit = FALSE;
                        rc = setExeInfo( pLin, sobjn, ip );
                    }
                }
            } else if( type == EXESIGN_LE || type == EXESIGN_LX ) {
                if( SeekRead( hdl, ne_header_off + 0x1c, &eip, sizeof( eip ) ) ) {
                    if( SeekRead( hdl, ne_header_off + 0x18, &objnum, sizeof( objnum ) ) ) {
                        Is32Bit = TRUE;
                        rc = setExeInfo( pLin, objnum, eip );
                    }
                }
            }
        }
        DosClose( hdl );
    }
    return( rc );

} /* FindLinearStartAddress */

static bool ExecuteUntilLinearAddressHit( ULONG lin )
{
    opcode_type saved_opcode;
    opcode_type brk_opcode = BRKPOINT;
    bool        rc = true;

    ReadLinear( &saved_opcode, lin, sizeof( saved_opcode ) );
    WriteLinear( &brk_opcode, lin, sizeof( brk_opcode ) );
    do {
        ExceptNum = 0;
        DebugExecute( &Buff, DBG_C_Go, true );
        if( ExceptNum == 0 ) {
            rc = true; // dll loaded
            break;
        }
        if( ExceptNum != XCPT_BREAKPOINT ) {
            rc = false;
            break;
        }
    } while( ExceptLinear != lin );
    WriteLinear( &saved_opcode, lin, sizeof( saved_opcode ) );
    return( rc );
}

void AppSession( void )
{
    if( !IsPMDebugger() ) {
        DosSelectSession( SID, 0 );
    }
}

void DebugSession( void )
{
    if( !IsPMDebugger() ) {
        DosSelectSession( 0, 0 );
    }
}

trap_retval TRAP_CORE( Prog_load )( void )
{
    STARTDATA           start;
    char                *parms;
    char                *src;
    char                *name;
    char                exe_name[255];
    char                appname[200];
    ULONG               startLinear;
    prog_load_ret       *ret;

    LastMTE = 0;
    ExceptNum = -1;
    ret = GetOutPtr( 0 );
    ret->err = 0;
    AtEnd = false;
    TaskFS = 0;
    name = GetInPtr( sizeof( prog_load_req ) );
    FindFilePath( DIG_FILETYPE_EXE, name, exe_name );
    parms = AddDriveAndPath( exe_name, UtilBuff ) + 1;
    src = name;
    while( *src++ != '\0' )
        {}
    MergeArgvArray( src, parms, GetTotalSizeIn() - sizeof( prog_load_req ) - ( src - name ) );

    strcpy( appname, TRP_The_WATCOM_Debugger );
    strcat( appname, ": " );
    strcat( appname, exe_name );

    start.Length = offsetof( STARTDATA, IconFile ); /* default for the rest */
    start.Related = 1;
    start.FgBg = !Remote;
    start.TraceOpt = 1;
    start.PgmTitle = (PSZ)appname;
    start.PgmName = UtilBuff;
    start.PgmInputs = (PBYTE)parms;
    start.TermQ = 0;
    start.Environment = NULL;
    start.InheritOpt = 1;
    if( GetEXEFlags( UtilBuff ) == EXE_IS_PM ) {
        if( TypeProcess == PT_WINDOWABLEVIO ) {
            ret->err = ERROR_OS2_TRAP( ERROR_NOT_IN_WINDOW );
        } else {
            start.SessionType = SSF_TYPE_PM;
            if( !IsPMDebugger() ) {
                StartPMHelp();
            }
        }
    } else if( TypeProcess == PT_WINDOWABLEVIO || TypeProcess == PT_PM ) {
        start.SessionType = SSF_TYPE_WINDOWABLEVIO;
    } else if( TypeProcess == PT_FULLSCREEN ) {
        start.SessionType = SSF_TYPE_FULLSCREEN;
    }
    if( ret->err == 0 ) {
        ret->err = DosStartSession( (void *)&start, &SID, &Pid );
    }
    if( ret->err == ERROR_SMG_START_IN_BACKGROUND ) {
        ret->err = 0;
    }
    if( ret->err != 0 ) {
        Pid = 0;
        /* may need to do this
        ret->task_id = Pid;
        ret->thread = 1;
        ret->flags = LD_FLAG_IS_PROT;
        */
    } else {
        ret->task_id = Pid;
        ret->flags = LD_FLAG_IS_PROT;
        Buff.Pid = Pid;
        Buff.Tid = 0;
        Buff.Cmd = DBG_C_Connect;
        Buff.Value = DBG_L_386;
        CallDosDebug( &Buff );

        Buff.Pid = Pid;
        Buff.Tid = 1;
        DebugExecute( &Buff, DBG_C_Stop, false );
        if( Buff.Cmd != DBG_N_Success ) {
            ret->err = 14; /* can't load */
            return( sizeof( *ret ) );
        }
        ReadRegs( &Buff );
        CanExecTask = false;
        if( FindLinearStartAddress( &startLinear, UtilBuff ) ) {
            if( Is32Bit ) {
                ret->flags |= LD_FLAG_IS_BIG;
            }
            CanExecTask = ExecuteUntilLinearAddressHit( startLinear );
            ReadRegs( &Buff );
        }
#if 0
        if( CanExecTask ) {
            dos_debug   save;

            save.Pid = Pid;
            save.Tid = 1;
            ReadRegs( &save );
            if( !CausePgmToLoadThisDLL( startLinear ) ) {
                CanExecTask = false;
            }
            WriteRegs( &save );
        }
#endif
        Buff.Pid = Pid;
        Buff.Tid = 1;
        ReadRegs( &Buff );
        TaskFS = Buff.FS;
    }
    ret->flags |= LD_FLAG_HAVE_RUNTIME_DLLS;
    ret->mod_handle = 0;
    CurrModHandle = 1;
    return( sizeof( *ret ) );
}

trap_retval TRAP_CORE( Prog_kill )( void )
{
    prog_kill_ret       *ret;

    ret = GetOutPtr( 0 );
    ret->err = 0;
    SaveStdIn = NIL_DOS_HANDLE;
    SaveStdOut = NIL_DOS_HANDLE;
    if( Pid != 0 ) {
        Buff.Cmd = DBG_C_Term;
        Buff.Pid = Pid;
        CallDosDebug( &Buff );
    }
    NumModHandles = 0;
    CurrModHandle = 1;
    Pid = 0;
    DosSleep( 500 ); // Without this, it seems that restarts happen too fast
                     // and we end up running a 2nd instance of a dead task
                     // or some such sillyness.  I don't really know, but
                     // this DosSleep avoids problems when restarting a PM app
                     // ( ... Yes, this is a Hail Mary ... )
    StopPMHelp();
    return( sizeof( *ret ) );
}

trap_retval TRAP_CORE( Set_break )( void )
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

trap_retval TRAP_CORE( Clear_break )( void )
{
    clear_break_req     *acc;
    opcode_type         brk_opcode;

    acc = GetInPtr( 0 );
    brk_opcode = acc->old;
    WriteBuffer( &brk_opcode, acc->break_addr.segment, acc->break_addr.offset, sizeof( brk_opcode ) );
    return( 0 );
}

static int DRegsCount( void )
{
    int     needed;
    int     i;
    word    size;

    needed = 0;
    for( i = 0; i < WatchCount; i++ ) {
        needed++;
        size = WatchPoints[i].size;
        if( size == 8 ) {
            needed++;
            size = 4;
        }
        if( WatchPoints[i].addr.offset & ( size - 1 ) ) {
            needed++;
        }
    }
    return( needed );
}

trap_retval TRAP_CORE( Set_watch )( void )
{
    set_watch_req       *acc;
    set_watch_ret       *ret;
    watch_point         *wp;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    ret->multiplier = 50000;
    ret->err = 1;       // failure
    if( WatchCount < MAX_WATCHES ) { // nyi - artificial limit (32 should be lots)
        ret->err = 0;   // OK
        wp = WatchPoints + WatchCount;
        wp->addr.segment = acc->watch_addr.segment;
        wp->addr.offset = acc->watch_addr.offset;
        wp->size = acc->size;
        wp->value = 0;
        ReadBuffer( (PBYTE)&wp->value, wp->addr.segment, wp->addr.offset, wp->size );

        ++WatchCount;
        if( DRegsCount() <= 4 ) {
            ret->multiplier |= USING_DEBUG_REG;
        }
    }
    return( sizeof( *ret ) );
}

trap_retval TRAP_CORE( Clear_watch )( void )
{
    clear_watch_req *acc;
    watch_point     *dst;
    watch_point     *src;
    int             i;


    acc = GetInPtr( 0 );
    dst = src = WatchPoints;
    for( i = 0; i < WatchCount; ++i ) {
        if( src->addr.segment != acc->watch_addr.segment
          || src->addr.offset != acc->watch_addr.offset ) {
            *dst = *src;
            ++dst;
        } else {
            DebugExecute( &Buff, DBG_C_Stop, false );
            Buff.Cmd = DBG_C_ClearWatch;
            Buff.Index = 0; // src->id;
            CallDosDebug( &Buff );
        }
        ++src;
    }
    --WatchCount;
    return( 0 );
}

static volatile bool     BrkPending;

void SetBrkPending( void )
{
    BrkPending = true;
}

static void EXPENTRY BrkHandler( USHORT sig_arg, USHORT sig_num )
{
    PFNSIGHANDLER   prev_hdl;
    USHORT          prev_act;

    sig_arg = sig_arg;
    SetBrkPending();
    DosSetSigHandler( BrkHandler, &prev_hdl, &prev_act, 4, sig_num );
}

static trap_conditions MapReturn( trap_conditions conditions )
{
    if( BrkPending ) {
        /* Get CS:EIP & SS:ESP correct */
        ReadRegs( &Buff );
        return( conditions | COND_USER );
    }
//    Out( "Map Return - " );
//    OutNum( Buff.Cmd );
//    Out( "\r\n" );
    switch( Buff.Cmd ) {
    case DBG_N_Success:
        return( conditions );
    case DBG_N_AsyncStop:
        return( conditions | COND_USER );
//    case DBG_N_Signal:
//        return( TRAP_USER );
    case DBG_N_SStep:
        return( conditions | COND_TRACE );
    case DBG_N_Breakpoint:
        return( conditions | COND_BREAK );
    case DBG_N_Exception:
        switch( ExceptNum ) {
        case XCPT_PROCESS_TERMINATE:
        case XCPT_ASYNC_PROCESS_TERMINATE:
            return( conditions | COND_TERMINATE );
        default:
            return( conditions | COND_EXCEPTION );
        }
    case DBG_N_Watchpoint:
        return( conditions | COND_WATCH );
    case DBG_N_ModuleLoad:
        return( conditions | COND_LIBRARIES );
    case DBG_N_ThreadTerm:
        return( conditions );
    case DBG_N_Error: // must terminate application - system semaphore locked
        Buff.Cmd = DBG_C_Term;
        Buff.Pid = Pid;
        CallDosDebug( &Buff );
    default:
        AtEnd = true;
        CanExecTask = false;
        return( conditions | COND_TERMINATE );
    }
}

static bool setDebugRegs( void )
{
    watch_point *wp;
    int         i;
    word        size;
    word        dregs;
    dword       linear;

    if( DRegsCount() > 4 ) {
        return( false );
    }
    for( wp = WatchPoints, i = WatchCount; i-- > 0; wp++ ) {
        size = WatchPoints[i].size;
        dregs = 1;
        if( size == 8 ) {
            size = 4;
            dregs++;
        }
        if( WatchPoints[i].addr.offset & ( size - 1 ) )
            dregs++;
        linear = MakeItFlatNumberOne( WatchPoints[i].addr.segment, WatchPoints[i].addr.offset & ~( size - 1 ) );
        while( dregs-- > 0 ) {
            Buff.Cmd = DBG_C_SetWatch;
            Buff.Addr = linear;
            Buff.Len = size;
            Buff.Index = 0;
            Buff.Value = DBG_W_Write | DBG_W_Local;
            CallDosDebug( &Buff );
            linear += size;
        }
    }
    return( true );
}

static bool CheckWatchPoints( void )
{
    dos_debug       save;
    watch_point     *wp;
    int             i;
    uint_64         value;

    for( wp = WatchPoints, i = WatchCount; i-- > 0; wp++ ) {
        ReadRegs( &save );
        value = 0;
        ReadBuffer( (PBYTE)&value, wp->addr.segment, wp->addr.offset, wp->size );
        WriteRegs( &save );
        if( wp->value != value ) {
            return( true );
        }
    }
    return( false );
}

static void watchSingleStep( void )
{
    DebugExecute( &Buff, DBG_C_SStep, true );
    while( Buff.Cmd == DBG_N_SStep ) {
        if( CheckWatchPoints() ) {
            Buff.Cmd = DBG_N_Watchpoint;
            break;
        }
        DebugExecute( &Buff, DBG_C_SStep, true );
    }
}

static unsigned progRun( bool step )
{
    PFNSIGHANDLER       prev_brk_hdl;
    PFNSIGHANDLER       prev_intr_hdl;
    USHORT              prev_intr_act;
    USHORT              prev_brk_act;
    prog_go_ret         *ret;

    ret = GetOutPtr( 0 );
    if( NumModHandles > CurrModHandle ) {
        ret->conditions = COND_LIBRARIES;
        ret->stack_pointer.segment = lastSS;
        ret->stack_pointer.offset = lastESP;
        ret->program_counter.segment = lastCS;
        ret->program_counter.offset = lastEIP;
        return( sizeof( *ret ) );
    }
    BrkPending = false;
    DosSetSigHandler( BrkHandler, &prev_intr_hdl, &prev_intr_act, 2, SIG_CTRLC );
    DosSetSigHandler( BrkHandler, &prev_brk_hdl, &prev_brk_act, 2, SIG_CTRLBREAK );

    if( AtEnd ) {
        Buff.Cmd = DBG_N_ProcTerm;
    } else if( step ) {
        DebugExecute( &Buff, DBG_C_SStep, true );
    } else if( !setDebugRegs() ) {
        watchSingleStep();
    } else {
        DebugExecute( &Buff, DBG_C_Go, true );
        if( Buff.Cmd == DBG_N_Success ) {
            Buff.Cmd = DBG_N_ProcTerm;
        }
    }
    DosSetSigHandler( prev_brk_hdl, &prev_brk_hdl, &prev_brk_act,
                       prev_brk_act, SIG_CTRLBREAK );
    DosSetSigHandler( prev_brk_hdl, &prev_intr_hdl, &prev_intr_act,
                       prev_brk_act, SIG_CTRLC );
    ret->conditions = ( COND_CONFIG | COND_THREAD );
    if( NumModHandles > CurrModHandle ) {
        ret->conditions |= COND_LIBRARIES;
    }
    ret->conditions = MapReturn( ret->conditions );
    lastSS = ret->stack_pointer.segment = Buff.SS;
    lastESP = ret->stack_pointer.offset = Buff.ESP;
    lastCS = ret->program_counter.segment = Buff.CS;
    lastEIP = ret->program_counter.offset = Buff.EIP;
    //runret->thread = Buff.Tid;
    //if( runret->returnvalue == TRAP_TERMINATE ) {
    //    AtEnd = true;
    //    CanExecTask = false;
    //}
    return( sizeof( *ret ) );
}

trap_retval TRAP_CORE( Prog_go )( void )
{
    unsigned    rc;

    PMUnLock();
    rc = progRun( false );
    PMLock( Buff.Pid, Buff.Tid );
    return( rc );
}

trap_retval TRAP_CORE( Prog_step )( void )
{
    unsigned    rc;

    PMUnLock();
    rc = progRun( true );
    PMLock( Buff.Pid, Buff.Tid );
    return( rc );
}

trap_retval TRAP_FILE( write_console )( void )
{
    size_t                  len;
    USHORT                  written_len;
    byte                    *ptr;
    file_write_console_ret  *ret;

    ptr = GetInPtr( sizeof( file_write_console_req ) );
    len = GetTotalSizeIn() - sizeof( file_write_console_req );
    ret = GetOutPtr( 0 );
    ret->err = 0;
    if( CanExecTask ) {
        /* print/program request */
        ret->len = len;
        TaskPrint( ptr, len );
    } else {
        ret->err = DosWrite( 2, ptr, len, &written_len );
        ret->len = written_len;
    }
    return( sizeof( *ret ) );
}

static int ValidThread( TID thread )
{
    struct thd_state    state;
    TID                 save;

    if( thread == 0 )
        return( 0 );
    save = Buff.Tid;
    Buff.Tid = thread;
    Buff.Cmd = DBG_C_ThrdStat;
    Buff.Buffer = MakeLocalPtrFlat( &state );
    Buff.Len = 4;
    CallDosDebug( &Buff );
    Buff.Tid = save;
    return( Buff.Cmd == DBG_N_Success );
}

trap_retval TRAP_THREAD( get_next )( void )
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
    ret->thread = ( acc->thread == 0 ) ? 1 : 0;
    return( sizeof( *ret ) );
}

trap_retval TRAP_THREAD( set )( void )
{
    thread_set_req      *acc;
    thread_set_ret      *ret;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    ret->err = 0;
    ret->old_thread = Buff.Tid;
    if( ValidThread( acc->thread ) ) {
        Buff.Pid = Pid;
        Buff.Tid = acc->thread;
        Buff.Cmd = DBG_C_ReadReg;
        CallDosDebug( &Buff );
    } else if( acc->thread != 0 ) {
        ret->err = 1;
    }
    return( sizeof( *ret ) );
}

static unsigned DoThread( trace_codes code )
{
    TID             save;
    thread_thaw_req *acc;
    thread_thaw_ret *ret;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    ret->err = 1;       // fail
    if( ValidThread( acc->thread ) ) {
        ret->err = 0;   // OK
        save = Buff.Tid;
        Buff.Pid = Pid;
        Buff.Tid = acc->thread;
        Buff.Cmd = code;
        CallDosDebug( &Buff );
        Buff.Tid = save;
    }
    return( sizeof( *ret ) );
}

trap_retval TRAP_THREAD( freeze )( void )
{
    return( DoThread( DBG_C_Freeze ) );
}

trap_retval TRAP_THREAD( thaw )( void )
{
    return( DoThread( DBG_C_Resume ) );
}

trap_retval TRAP_CORE( Get_message_text )( void )
{
    get_message_text_ret        *ret;
    char                        *err_txt;

    ret = GetOutPtr( 0 );
    err_txt = GetOutPtr( sizeof(*ret) );
    if( ExceptNum == -1 ) {
        err_txt[0] = '\0';
    } else {
        strcpy( err_txt, GetExceptionText() );
    }
    ExceptNum = -1;
    ret->flags = MSG_NEWLINE | MSG_ERROR;
    return( sizeof( *ret ) + strlen( err_txt ) + 1 );
}

trap_retval TRAP_CORE( Get_next_alias )( void )
{
    get_next_alias_req  *acc;
    get_next_alias_ret  *ret;

    ret = GetOutPtr( 0 );
    ret->seg = 0;
    ret->alias = 0;
    acc = GetInPtr( 0 );
    if( Is32Bit && acc->seg == 0 ) {
        ret->seg = FlatCS;
        ret->alias = FlatDS;
    }
    return( sizeof( *ret ) );
}

void TRAPENTRY_FUNC( TellHandles )( HAB hab, HWND hwnd )
{
    TellSoftModeHandles( hab, hwnd );
}

char TRAPENTRY_FUNC( TellHardMode )( char hard )
{
    return( SetHardMode( hard ) );
}

trap_version TRAPENTRY TrapInit( const char *parms, char *err, bool remote )
{
    trap_version        ver;
    USHORT              os2ver;
    SEL                 li,gi;
    __LINFOSEG          *linfo;

    Remote = remote;
    err[0] = '\0';
    ver.major = TRAP_VERSION_MAJOR;
    ver.minor = TRAP_VERSION_MINOR;
    ver.remote = false;
    SaveStdIn = NIL_DOS_HANDLE;
    SaveStdOut = NIL_DOS_HANDLE;
    Screen = DEBUG_SCREEN;
    if( *parms == '2' ) {
        stopOnSecond = true;
    }

    DosGetVersion( &os2ver );
    if( os2ver < 0x200 ) {
        StrCopyDst( TRP_OS2_Wrong_Version, err );
        return( ver );
    }
    if( DosGetInfoSeg( &gi, &li ) != 0  ) {
        StrCopyDst( TRP_OS2_no_info, err );
        return( ver );
    }
    GblInfo = _MK_FP( gi, 0 );
    linfo = _MK_FP( li, 0 );
    TypeProcess = linfo->typeProcess;
    if( !GetDos32Debug( err ) ) {
        return( ver );
    }
    InitSoftDebug();
    InitDebugThread();

    return( ver );
}

void TRAPENTRY TrapFini( void )
{
}
