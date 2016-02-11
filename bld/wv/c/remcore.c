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
* Description:  Remote access core - trap file interface.
*
****************************************************************************/


#include "dbgdefn.h"
#include "dbgdata.h"
#include "dbgerr.h"
#include "dbgio.h"
#include "dbgmem.h"
#include "tcerr.h"
#include "dbglit.h"
#include "dui.h"
#include "trapaccs.h"
#include "madinter.h"
#include "trapglbl.h"
#include "dbgstk.h"
#include "dbgexpr.h"
#include "dbgovl.h"
#include "remcore.h"
#include "dbgmisc.h"
#include "trpld.h"
#include "removl.h"
#include "addarith.h"
#include "dbginit.h"


extern trap_elen        MaxPacketLen;
extern trap_elen        CurrRegSize;

//NYI: We don't know the size of the incoming err msg. Now assume max is 80.
#define MAX_ERR_MSG_SIZE        80

typedef struct{
    address     addr;
    size_t      len;
    char        *data;
} cache_block;

typedef struct {
    address     addr;
    addr48_off  end;
    unsigned    info;
    unsigned    len;
    unsigned_8  data[1];        /* variable sized */
} machine_data_cache;

static cache_block              Cache;
static machine_data_cache       *MData = NULL;

static bool IsInterrupt( addr_ptr *addr, unsigned size )
{
    return( MADAddrInterrupt( addr, size, &DbgRegs->mr ) == MS_OK );
}

static size_t MemRead( address addr, void *ptr, size_t size )
{
    read_mem_req        acc;
    bool                int_tbl;
    size_t              left;
    trap_elen           piece;
    trap_retval         read_len;

    if( size == 0 )
        return( 0 );
    SectLoad( addr.sect_id );
    acc.req = REQ_READ_MEM;
    AddrFix( &addr );
    acc.mem_addr = addr.mach;
    piece = MaxPacketLen;
    left = size;
    while( left > 0 ) {
        if( piece > left )
            piece = (trap_elen)left;
        acc.len = piece;
        int_tbl = IsInterrupt( &(acc.mem_addr), piece );
        if( int_tbl )
            RestoreHandlers();
        CONV_LE_32( acc.mem_addr.offset );
        CONV_LE_16( acc.mem_addr.segment );
        CONV_LE_16( acc.len );
        read_len = TrapSimpAccess( sizeof( acc ), &acc, piece, ptr );
        if( int_tbl )
            GrabHandlers();
        left -= read_len;
        if( read_len != piece )
            break;
        addr.mach.offset += read_len;
        acc.mem_addr = addr.mach;
        ptr = (char *)ptr + read_len;
    }
    return( size - left );
}

void FiniCache( void )
{
    _Free( Cache.data );
    Cache.data = NULL;
}

void InitCache( address addr, size_t size )
{
    void *ptr;

    if( size == 0 )
        return;
    FiniCache();
    _Alloc( ptr, size );
    if( ptr == NULL )
        return;
    Cache.data = ptr;
    Cache.addr = addr;
    Cache.len = MemRead( addr, ptr, size );
}

bool HaveCache( void )
{
    return( Cache.data != NULL );
}

static bool ReadCache( address addr, char *data, size_t len )
{
    if( Cache.data == NULL )
        return( false );
    if( !SameAddrSpace( Cache.addr, addr ) )
        return( false );
    if( len > Cache.len )
        return( false );
    if( Cache.addr.mach.offset > addr.mach.offset )
        return( false );
    addr.mach.offset -= Cache.addr.mach.offset;
    if( Cache.len - len < addr.mach.offset )
        return( false );
    memcpy( data, &Cache.data[addr.mach.offset], len );
    return( true );
}

size_t ProgPeek( address addr, void *data, size_t len )
{
    if( ReadCache( addr, data, len ) ) {
        return( len );
    } else {
        return( MemRead( addr, data, len ) );
    }
}

size_t ProgPoke( address addr, const void *data, size_t len )
{
    in_mx_entry         in[2];
    mx_entry            out[1];
    write_mem_req       acc;
    write_mem_ret       ret;
    bool                int_tbl;
    size_t              left;
    trap_elen           piece;

    SectLoad( addr.sect_id );
    acc.req = REQ_WRITE_MEM;
    AddrFix( &addr );
    acc.mem_addr = addr.mach;
    in[0].ptr = &acc;
    in[0].len = sizeof( acc );
    out[0].ptr = &ret;
    out[0].len = sizeof( ret );
    piece = MaxPacketLen - sizeof( acc );
    left = len;
    while( left > 0 ) {
        if( piece > left )
            piece = (trap_elen)left;
        in[1].ptr = data;
        in[1].len = piece;

        int_tbl = IsInterrupt( &(acc.mem_addr), piece );
        if( int_tbl )
            RestoreHandlers();
        CONV_LE_32( acc.mem_addr.offset );
        CONV_LE_16( acc.mem_addr.segment );
        TrapAccess( 2, in, 1, out );
        CONV_LE_16( ret.len );
        if( int_tbl )
            GrabHandlers();

        left -= ret.len;
        if( ret.len != piece )
            break;
        addr.mach.offset += ret.len;
        acc.mem_addr = addr.mach;
        data = (char *)data + ret.len;
    }
    return( len - left );
}


unsigned long ProgChkSum( address addr, trap_elen len )
{

    checksum_mem_req    acc;
    checksum_mem_ret    ret;

    SectLoad( addr.sect_id );
    acc.req = REQ_CHECKSUM_MEM;
    AddrFix( &addr );
    acc.in_addr = addr.mach;
    acc.len = len;
    TrapSimpAccess( sizeof( acc ), &acc, sizeof( ret ), &ret );
    return( ret.result );
}

trap_retval PortPeek( unsigned port, void *data, trap_elen size )
{
    read_io_req         acc;

    acc.req = REQ_READ_IO;
    acc.IO_offset = port;
    acc.len = size;
    return( TrapSimpAccess( sizeof( acc ), &acc, size, data ) );
}

trap_retval PortPoke( unsigned port, const void *data, trap_elen size )
{
    in_mx_entry         in[2];
    mx_entry            out[1];
    write_io_req        acc;
    write_io_ret        ret;

    acc.req = REQ_WRITE_IO;
    acc.IO_offset = port;
    in[0].ptr = &acc;
    in[0].len = sizeof( acc );
    in[1].ptr = data;
    in[1].len = size;
    out[0].ptr = &ret;
    out[0].len = sizeof( ret );
    TrapAccess( 2, in, 1, out );
    return( ret.len );
}

static void ReadRegs( machine_state *state )
{
    read_regs_req       acc;

    acc.req = REQ_READ_REGS;
    TrapSimpAccess( sizeof( acc ), &acc, CurrRegSize, &state->mr );
    MADRegistersHost( &state->mr );
    if( state->ovl != NULL ) {
        RemoteSectTblRead( state->ovl );
    }
}

void ReadDbgRegs( void )
{
    ReadRegs( DbgRegs );
    InitLC( &Context, true );
}

static void WriteRegs( machine_state *state )
{
    in_mx_entry         in[2];
    write_regs_req      acc;
//    mad_status          ms;

//    ms = MADRegistersTarget( &state->mr );
    MADRegistersTarget( &state->mr );
    acc.req = REQ_WRITE_REGS;
    in[0].ptr = &acc;
    in[0].len = sizeof( acc );
    in[1].ptr = &state->mr;
    in[1].len = CurrRegSize;
    TrapAccess( 2, in, 0, NULL );
    // Always convert regs back to host format; might be more
    // efficient to create a local copy instead
    MADRegistersHost( &state->mr );
    if( state->ovl != NULL ) {
        RemoteSectTblWrite( state->ovl );
    }
}

void WriteDbgRegs( void )
{
    WriteRegs( DbgRegs );
}

trap_elen ArgsLen( const char *args )
{
    trap_elen   len = 0;

    while( *args++ != ARG_TERMINATE ) {
        len++;
    }
    return( len );
}

void ClearMachineDataCache( void )
{
    MData->addr = NilAddr;
    MData->end  = 0;
}

/*
 * DoLoad -- load in user program as an overlay
 */

error_idx DoLoad( const char *args, unsigned long *phandle )
{
    in_mx_entry         in[2];
    mx_entry            out[1];
    prog_load_req       acc;
    prog_load_ret       ret;

    ClearMachineDataCache();
    acc.req = REQ_PROG_LOAD;
    acc.true_argv = _IsOn( SW_TRUE_ARGV );
    in[0].ptr = &acc;
    in[0].len = sizeof( acc );
    in[1].ptr = args;
    in[1].len = ArgsLen( args );
    out[0].ptr = &ret;
    out[0].len = sizeof( ret );
    ret.flags = 0;      /* in case of error */
    ret.mod_handle = 0;
    ret.task_id = 0;
    RestoreHandlers();
    FiniSuppServices();
    OnAnotherThreadAccess( 2, in, 1, out );
    InitSuppServices();
    GrabHandlers();
    GetSysConfig();
    CheckMADChange();
    ReadDbgRegs();
    DbgRegs->tid = RemoteSetThread( 0 );
    CheckForNewThreads( true );
    TaskId = ret.task_id;
    if( ret.flags & LD_FLAG_IGNORE_SEGMENTS ) {
        _SwitchOn( SW_IGNORE_SEGMENTS );
    } else {
        _SwitchOff( SW_IGNORE_SEGMENTS );
    }
    if( ret.flags & LD_FLAG_IS_STARTED ) {
        _SwitchOn( SW_PROC_ALREADY_STARTED );
    } else {
        _SwitchOff( SW_PROC_ALREADY_STARTED );
    }
    if( ret.flags & LD_FLAG_HAVE_RUNTIME_DLLS ) {
        _SwitchOn( SW_HAVE_RUNTIME_DLLS );
    } else {
        _SwitchOff( SW_HAVE_RUNTIME_DLLS );
    }
    if( (ret.flags & LD_FLAG_DISPLAY_DAMAGED) && _IsOff( SW_REMOTE_LINK ) ) {
        DUIDirty();
    }
    *phandle = ret.mod_handle;
    return( StashErrCode( ret.err, OP_REMOTE ) );
}


bool KillProgOvlay( void )
{
    prog_kill_req       acc;
    prog_kill_ret       ret;

    _SwitchOff( SW_PROC_ALREADY_STARTED );
    acc.req = REQ_PROG_KILL;
    acc.task_id = TaskId;
    RestoreHandlers();
    FiniSuppServices();
    OnAnotherThreadSimpAccess( sizeof( acc ), &acc, sizeof( ret ), &ret );
    CONV_LE_32( ret.err );
    InitSuppServices();
    _SwitchOff( SW_HAVE_TASK );
    GrabHandlers();
    FreeThreads();
    GetSysConfig();
    ClearMachineDataCache();
    return( ( ret.err == 0 ) );
}


unsigned MakeProgRun( bool single )
{
    prog_go_req         acc;
    prog_go_ret         ret;
    addr_ptr            tmp;

    acc.req = single ? REQ_PROG_STEP : REQ_PROG_GO;
    RestoreHandlers();
    DUIExitCriticalSection();
    OnAnotherThreadSimpAccess( sizeof( acc ), &acc, sizeof( ret ), &ret );
    CONV_LE_32( ret.stack_pointer.offset );
    CONV_LE_16( ret.stack_pointer.segment );
    CONV_LE_32( ret.program_counter.offset );
    CONV_LE_16( ret.program_counter.segment );
    CONV_LE_16( ret.conditions );
    DUIEnterCriticalSection();
    GrabHandlers();
    if( ret.conditions & COND_CONFIG ) {
        GetSysConfig();
        CheckMADChange();
    }
    DbgRegs->mad = SysConfig.mad;
    /* Use 'tmp' because of alignment problems */
    tmp = ret.stack_pointer;
    MADRegSpecialSet( MSR_SP, &DbgRegs->mr, &tmp );
    tmp = ret.program_counter;
    MADRegSpecialSet( MSR_IP, &DbgRegs->mr, &tmp );
    if( ret.conditions & COND_THREAD ) {
        DbgRegs->tid = RemoteSetThread( 0 );
    }
    return( ret.conditions );
}

bool Redirect( bool input, char *hndlname )
{
    in_mx_entry         in[2];
    mx_entry            out[1];
    redirect_stdin_req  acc;
    redirect_stdin_ret  ret;
    open_access         loc;

    acc.req = input ? REQ_REDIRECT_STDIN : REQ_REDIRECT_STDOUT;
    in[0].ptr = &acc;
    in[0].len = sizeof( acc );
    in[1].ptr = FileLoc( hndlname, &loc );
    in[1].len = (trap_elen)( strlen( in[1].ptr ) + 1 );
    out[0].ptr = &ret;
    out[0].len = sizeof( ret );
    TrapAccess( 2, in, 1, out );
    return( ( ret.err == 0 ) );
}


void RemoteMapAddr( addr_ptr *addr, addr_off *lo_bound,
                        addr_off *hi_bound, unsigned long handle )
{
    map_addr_req        acc;
    map_addr_ret        ret;

    acc.req = REQ_MAP_ADDR;
    acc.in_addr = *addr;
    acc.handle = handle;
    CONV_LE_32( acc.in_addr.offset );
    CONV_LE_16( acc.in_addr.segment );
    CONV_LE_32( acc.handle );
    TrapSimpAccess( sizeof( acc ), &acc, sizeof( ret ), &ret );
    CONV_LE_32( ret.out_addr.offset );
    CONV_LE_16( ret.out_addr.segment );
    CONV_LE_32( ret.lo_bound );
    CONV_LE_32( ret.hi_bound );
    *addr = ret.out_addr;
    *lo_bound = ret.lo_bound;
    *hi_bound = ret.hi_bound;
}

void RemoteSetUserScreen( void )
{
    set_user_screen_req         acc;

    acc.req = REQ_SET_USER_SCREEN;
    TrapSimpAccess( sizeof( acc ), &acc, 0, NULL );
}

void RemoteSetDebugScreen( void )
{
    set_debug_screen_req        acc;

    acc.req = REQ_SET_DEBUG_SCREEN;
    TrapSimpAccess( sizeof( acc ), &acc, 0, NULL );
}

unsigned RemoteReadUserKey( unsigned wait )
{
    read_user_keyboard_req      acc;
    read_user_keyboard_ret      ret;

    acc.req = REQ_READ_USER_KEYBOARD;
    acc.wait = wait;
    CONV_LE_16( acc.wait );
    TrapSimpAccess( sizeof( acc ), &acc, sizeof( ret ), &ret );
    return( ret.key );
}

unsigned long RemoteGetLibName( unsigned long lib_hdl, char *buff, trap_elen buff_len )
{
    in_mx_entry         in[1];
    mx_entry            out[2];
    get_lib_name_req    acc;
    get_lib_name_ret    ret;

    acc.req = REQ_GET_LIB_NAME;
    acc.handle = lib_hdl;
    in[0].ptr = &acc;
    in[0].len = sizeof( acc );
    out[0].ptr = &ret;
    out[0].len = sizeof( ret );
    out[1].ptr = buff;
    out[1].len = buff_len;
    ret.handle = 0;
    CONV_LE_32( acc.handle );
    TrapAccess( 1, in, 2, out );
    CONV_LE_32( ret.handle );
    return( ret.handle );
}

unsigned RemoteGetMsgText( char *buff, trap_elen buff_len )
{
    in_mx_entry                 in[1];
    mx_entry                    out[2];
    get_message_text_req        acc;
    get_message_text_ret        ret;

    acc.req = REQ_GET_MESSAGE_TEXT;
    in[0].ptr = &acc;
    in[0].len = sizeof( acc );
    out[0].ptr = &ret;
    out[0].len = sizeof( ret );
    out[1].ptr = buff;
    out[1].len = buff_len;
    TrapAccess( 1, in, 2, out );
//    TrapErrTranslate( msg, MAX_ERR_MSG_SIZE );
    return( ret.flags );
}

unsigned RemoteMachineData( address addr, uint_8 info_type,
                        dig_elen in_size,  const void *inp,
                        dig_elen out_size, void *outp )
{
    in_mx_entry                 in[2];
    mx_entry                    out[2];
    machine_data_req            acc;
    machine_data_ret            ret;
    unsigned                    len;
    machine_data_cache          *new;

    if( info_type == MData->info
      && addr.mach.offset >= MData->addr.mach.offset
      && addr.mach.offset <  MData->end
      && SameAddrSpace( addr, MData->addr ) ) {
        memcpy( outp, MData->data, out_size );
        return( out_size );
    }
    acc.req = REQ_MACHINE_DATA;
    acc.info_type = info_type;
    acc.addr = addr.mach;
    in[0].ptr = &acc;
    in[0].len = sizeof( acc );
    in[1].ptr = (void *)inp;
    in[1].len = in_size;
    out[0].ptr = &ret;
    out[0].len = sizeof( ret );
    out[1].ptr = outp;
    out[1].len = out_size;
    len = TrapAccess( 2, in, 2, out );
    if( len == 0 )
        return( 0 );
    len -= sizeof( ret );
    if( len > MData->len ) {
        new = MData;
        _Realloc( new, len + sizeof( *MData ) );
        if( new == NULL )
            return( len );
        MData = new;
    }
    memcpy( MData->data, outp, len );
    MData->addr = addr;
    MData->addr.mach.offset = ret.cache_start;
    MData->end = ret.cache_end;
    MData->info = info_type;
    MData->len = len;
    return( len );
}

dword RemoteSetBreak( address addr )
{
    set_break_req       acc;
    set_break_ret       ret;

    acc.req = REQ_SET_BREAK;
    AddrFix( &addr );
    acc.break_addr = addr.mach;
    CONV_LE_32( acc.break_addr.offset );
    CONV_LE_16( acc.break_addr.segment );
    TrapSimpAccess( sizeof( acc ), &acc, sizeof( ret ), &ret );
    return( ret.old );
}

void RemoteRestoreBreak( address addr, dword value )
{
    clear_break_req     acc;

    acc.req = REQ_CLEAR_BREAK;
    AddrFix( &addr );
    acc.break_addr = addr.mach;
    acc.old = value;
    CONV_LE_32( acc.break_addr.offset );
    CONV_LE_16( acc.break_addr.segment );
    TrapSimpAccess( sizeof( acc ), &acc, 0, NULL );
}

bool RemoteSetWatch( address addr, uint_8 size, unsigned long *mult )
{
    set_watch_req       acc;
    set_watch_ret       ret;

    acc.req = REQ_SET_WATCH;
    AddrFix( &addr );
    acc.watch_addr = addr.mach;
    acc.size = size;
    TrapSimpAccess( sizeof( acc ), &acc, sizeof( ret ), &ret ); //
    *mult = ret.multiplier & ~USING_DEBUG_REG;
    return( (ret.multiplier & USING_DEBUG_REG) != 0 );
}

void RemoteRestoreWatch( address addr, uint_8 size )
{
    clear_watch_req     acc;

    acc.req = REQ_CLEAR_WATCH;
    AddrFix( &addr );
    acc.watch_addr = addr.mach;
    acc.size = size;
    TrapSimpAccess( sizeof( acc ), &acc, 0, NULL );
}

void RemoteSplitCmd( char *cmd, char **end, char **parm )
{
    in_mx_entry         in[2];
    mx_entry            out[1];
    split_cmd_req       acc;
    split_cmd_ret       ret;

    acc.req = REQ_SPLIT_CMD;
    in[0].ptr = &acc;
    in[0].len = sizeof( acc );
    in[1].ptr = cmd;
    in[1].len = ArgsLen( cmd );
    out[0].ptr = &ret;
    out[0].len = sizeof( ret );
    TrapAccess( 2, in, 1, out );
    CONV_LE_16( ret.cmd_end );
    CONV_LE_16( ret.parm_start );
    *end = &cmd[ ret.cmd_end ];
    *parm = &cmd[ ret.parm_start ];
}


void CheckSegAlias( void )
{
    get_next_alias_req  acc;
    get_next_alias_ret  ret;

    acc.req = REQ_GET_NEXT_ALIAS;
    acc.seg = 0;
    for( ;; ) {
        ret.seg = 0;
        CONV_LE_16( acc.seg );
        TrapSimpAccess( sizeof( acc ), &acc, sizeof( ret ), &ret );
        CONV_LE_16( ret.seg );
        CONV_LE_16( ret.alias );
        if( ret.seg == 0 )
            break;
        AddAliasInfo( ret.seg, ret.alias );
        acc.seg = ret.seg;
    }
}

void GetSysConfig( void )
{
    get_sys_config_req  acc;

    acc.req = REQ_GET_SYS_CONFIG;
    TrapSimpAccess( sizeof( acc ), &acc, sizeof( SysConfig ), &SysConfig );
    CONV_LE_16( SysConfig.mad );
}

bool InitCoreSupp( void )
{
    if( MData == NULL ) {
        _Alloc( MData, sizeof( *MData ) );
        MData->len = sizeof( MData->data );
        ClearMachineDataCache();
        GetSysConfig();
        CheckMADChange();
        return( true );
    } else {
        return( false );
    }
}

void FiniCoreSupp( void )
{
    _Free( MData );
    MData = NULL;
}

char    *TrapClientString( unsigned tc )
{
    switch( tc ) {
    case TC_BAD_TRAP_FILE:      return( LIT_ENG( BAD_TRAP_FILE ) );
    case TC_CANT_LOAD_TRAP:     return( LIT_ENG( CANT_LOAD_TRAP_FILE ) );
    case TC_WRONG_TRAP_VERSION: return( LIT_ENG( INCORRECT_TRAP_FILE_VERSION ) );
    case TC_OUT_OF_DOS_MEMORY:  return( LIT_ENG( OUT_OF_DOS_MEMORY ) );
    }
    return( NULL );
}
