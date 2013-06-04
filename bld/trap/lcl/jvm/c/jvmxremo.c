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


#include <windows.h>
#include <string.h>
#include "trpimp.h"
#include "trperr.h"
#include "packet.h"
#include "jvmerr.h"
#define MD_jvm
#include "madregs.h"

static char             LinkParm[256];
bool                    TaskLoaded;
HANDLE                  FakeHandle;
HWND                    DebuggerHwnd;
static char             SavedError[256];

trap_retval DoAccess( void )
{
    trap_elen  left;
    trap_elen  len;
    trap_elen  i;
    trap_elen  piece;

    StartPacket();
    if( Out_Mx_Num == 0 ) {
        /* Tell the server we're not expecting anything back */
        *(access_req *)In_Mx_Ptr[0].ptr |= 0x80;
    }
    for( i = 0; i < In_Mx_Num; ++i ) {
        AddPacket( In_Mx_Ptr[i].len, In_Mx_Ptr[i].ptr );
    }
    *(access_req *)In_Mx_Ptr[0].ptr &= ~0x80;
    PutPacket();
    if( Out_Mx_Num != 0 ) {
        len = GetPacket();
        left = len;
        i = 0;
        for( ;; ) {
            if( i >= Out_Mx_Num ) break;
            if( left > Out_Mx_Ptr[i].len ) {
                piece = Out_Mx_Ptr[i].len;
            } else {
                piece = left;
            }
            RemovePacket( piece, Out_Mx_Ptr[i].ptr );
            i++;
            left -= piece;
            if( left == 0 ) break;
        }
    } else {
        len = 0;
        left = 0;
    }
    return( len - left );
}


trap_retval ReqGet_sys_config( void )
{
    get_sys_config_ret  *ret;

    if( !TaskLoaded ) {
        ret = GetOutPtr(0);
        ret->sys.os = OS_IDUNNO;
        ret->sys.osmajor = 0;
        ret->sys.osminor = 0;
        ret->sys.fpu = 0;
        ret->sys.huge_shift = 12;
        ret->sys.cpu = 0;
        ret->sys.mad = MAD_JVM;
        return( sizeof( *ret ) );
    }
    return( DoAccess() );
}

static DWORD DoFmtMsg( LPTSTR *p, DWORD err, ... )
{
    va_list     args;
    DWORD       len;
    LPSTR       q;

    va_start( args, err );
    len = FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
        NULL, err, MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ),
        (LPSTR) p, 0, &args );
    while( ( q = strchr( *p, '\r' ) ) != NULL ) *q = ' ';
    while( ( q = strchr( *p, '\n' ) ) != NULL ) *q = ' ';
    va_end( args );
    return( len );
}

static char *Errors[] = {
    #define pick( a,b,c ) c,
    #include "jvmepick.h"
    #undef pick
};

trap_retval ReqGet_err_text( void )
{

    get_err_text_req    *acc;
    char                *err_txt;
    LPTSTR              lpMessageBuffer;
    DWORD               len;

    acc = GetInPtr( 0 );
    err_txt = GetOutPtr( 0 );

    if( acc->err == ERR_JVM_SAVED_ERROR ) {
        strcpy( err_txt, SavedError );
    } else if( IsUserErr( acc->err ) ) {
        strcpy( err_txt, Errors[ ErrIndex( acc->err ) ] );
    } else {
        len = DoFmtMsg( &lpMessageBuffer, acc->err, "%1","%2","%3","%4" );
        if( len > 0 ) {
            strcpy( err_txt, lpMessageBuffer );
            LocalFree( lpMessageBuffer );
        }
    }
    return( strlen( err_txt ) + 1 );
}

trap_retval ReqMap_addr( void )
{
    map_addr_req        *acc;
    map_addr_ret        *ret;

    if( !TaskLoaded ) {
        acc = GetInPtr(0);
        ret = GetOutPtr(0);
        ret->out_addr = acc->in_addr;
        ret->lo_bound = 0;
        ret->hi_bound = ~(addr48_off)0;
        return( sizeof( *ret ) );
    }
    return( DoAccess() );
}

trap_retval ReqRead_io( void )
{
    if( !TaskLoaded ) {
        return( 0 );
    }
    return( DoAccess() );
}

trap_retval ReqWrite_io( void )
{
    write_io_ret        *ret;

    if( !TaskLoaded ) {
        ret = GetOutPtr(0);
        ret->len = 0;
        return( sizeof( *ret ) );
    }
    return( DoAccess() );
}

// OBSOLETE - use ReqRead_regs
trap_retval ReqRead_cpu( void )
{
    read_cpu_ret        *ret;

    if( !TaskLoaded ) {
        ret = GetOutPtr(0);
        memset( ret, 0, sizeof( *ret ) );
        return( sizeof( *ret ) );
    }
    return( DoAccess() );
}

// OBSOLETE - use ReqRead_regs
trap_retval ReqRead_fpu( void )
{
    read_fpu_ret        *ret;

    if( !TaskLoaded ) {
        ret = GetOutPtr(0);
        memset( ret, 0, sizeof( *ret ) );
        return( sizeof( *ret ) );
    }
    return( DoAccess() );
}

trap_retval ReqRead_regs( void )
{

    if( !TaskLoaded ) {
        mad_registers *mr;
        mr = GetOutPtr(0);
        memset( mr, 0, sizeof( mr->jvm ) );
        return( sizeof( mr->jvm ) );
    }
    return( DoAccess() );
}


trap_retval ReqChecksum_mem( void )
{
    checksum_mem_ret    *ret;

    if( !TaskLoaded ) {
        ret = GetOutPtr( 0 );
        ret->result = 0;
        return( sizeof( *ret ) );
    }
    return( DoAccess() );
}

trap_retval ReqGet_next_alias( void )
{
    get_next_alias_ret  *ret;

    if( !TaskLoaded ) {
        ret = GetOutPtr( 0 );
        ret->seg = 0;
        ret->alias = 0;
        return( sizeof( *ret ) );
    }
    return( DoAccess() );
}

trap_retval ReqProg_go( void )
{
    prog_go_ret     *ret;
    trap_elen       len;

    if( !TaskLoaded ) {
        ret = GetOutPtr( 0 );
        ret->conditions = COND_TERMINATE;
        return( sizeof( *ret ) );
    }
    len = DoAccess();
    return( len );
}

//OBSOLETE - use ReqMachine_data
trap_retval ReqAddr_info( void )
{
    addr_info_ret       *ret;

    if( !TaskLoaded ) {
        ret = GetOutPtr( 0 );
        ret->is_big = TRUE;
        return( sizeof( *ret ) );
    }
    return( DoAccess() );
}

trap_retval ReqMachine_data( void )
{
    machine_data_ret    *ret;

    if( !TaskLoaded ) {
        ret = GetOutPtr( 0 );
        ret->cache_start = 0;
        ret->cache_end = ~(addr_off)0;
        return( sizeof( *ret ) );
    }
    return( DoAccess() );
}

trap_retval ReqGet_lib_name( void )
{
    get_lib_name_ret    *ret;

    if( !TaskLoaded ) {
        ret = GetOutPtr( 0 );
        ret->handle = 0;
        *(char *)GetOutPtr( sizeof( *ret ) ) = '\0';
        return( sizeof( *ret ) + 1 );
    }
    return( DoAccess() );
}

trap_retval ReqRead_mem( void )
{
    if( !TaskLoaded ) return( 0 );
    return( DoAccess() );
}

trap_retval ReqWrite_mem( void )
{
    write_mem_ret       *ret;

    if( !TaskLoaded ) {
        ret = GetOutPtr( 0 );
        ret->len = 0;
        return( sizeof( *ret ) );
    }
    return( DoAccess() );
}

trap_retval ReqProg_load( void )
{
    char                buffer[160];
    char                *src;
    char                *dst;
    char                *name;
    char                *endparm;
    char                *err;
    prog_load_ret       *ret;
    trap_elen           len;
    char                *loaderr;

    ret = GetOutPtr( 0 );
    src = name = GetInPtr( sizeof( prog_load_req ) );
    strcpy( (char *)buffer, src );
    endparm = LinkParm + strlen( LinkParm ) + 1;
    strcpy( endparm, buffer );
    if( *name == '\0' ) {
        ret->err = ERROR_FILE_NOT_FOUND;
        return( sizeof( *ret ) );
    }
    err = RemoteLink( LinkParm, 0 );
    if( err != NULL ) {
        loaderr = err;
        strcpy( SavedError, err );
        ret->err = ERR_JVM_SAVED_ERROR;
        return( sizeof( *ret ) );
    }
    while( *src != '\0' ) ++src;
    ++src;
    len = GetTotalSize() - (src - name) - sizeof( prog_load_req );
    dst = (char *)buffer;
    while( *dst != '\0' ) ++dst;
    ++dst;
    memcpy( dst, src, len );
    dst += len;
    StartPacket();
    AddPacket( sizeof( prog_load_req ), In_Mx_Ptr[0].ptr );
    AddPacket( dst - buffer, buffer );
    PutPacket();
    len = GetPacket();
    RemovePacket( sizeof( *ret ), ret );
    if( ret->err == 0 ) {
        TaskLoaded = TRUE;
        if( DebuggerHwnd != NULL ) SetForegroundWindow( DebuggerHwnd );
        return( len );
    } else {
        prog_kill_req   killacc;

        killacc.req = REQ_PROG_KILL;
        StartPacket();
        AddPacket( sizeof( killacc ), &killacc );
        PutPacket();
        GetPacket();
        //RemovePacket( msg_len, &erracc );
        RemoteUnLink();

        TaskLoaded = FALSE;
        return( sizeof( *ret ) );
    }
}

trap_retval ReqProg_kill( void )
{
    prog_kill_ret       *ret;

    ret = GetOutPtr( 0 );
    ret->err = 0;
    if( TaskLoaded ) {
        DoAccess();
        TaskLoaded = FALSE;
        RemoteUnLink();
    }
    return( sizeof( *ret ) );
}

// OBSOLETE - use ReqWrite_regs
trap_retval ReqWrite_cpu( void )
{
    if( !TaskLoaded ) return( 0 );
    return( DoAccess() );
}

// OBSOLETE - use ReqWrite_regs
trap_retval ReqWrite_fpu( void )
{
    if( !TaskLoaded ) return( 0 );
    return( DoAccess() );
}
trap_retval ReqWrite_regs( void )
{
    if( !TaskLoaded ) return( 0 );
    return( DoAccess() );
}

trap_retval ReqSet_watch( void )
{
    if( !TaskLoaded ) return( 0 );
    return( DoAccess() );
}

trap_retval ReqClear_watch( void )
{
    if( !TaskLoaded ) return( 0 );
    return( DoAccess() );
}

trap_retval ReqSet_break( void )
{
    if( !TaskLoaded ) return( 0 );
    return( DoAccess() );
}

trap_retval ReqClear_break( void )
{
    if( !TaskLoaded ) return( 0 );
    return( DoAccess() );
}

trap_retval ReqGet_message_text( void )
{
    if( !TaskLoaded ) return( 0 );
    return( DoAccess() );
}

trap_retval ReqRedirect_stdin( void )
{
    if( !TaskLoaded ) return( 0 );
    return( DoAccess() );
}

trap_retval ReqRedirect_stdout( void )
{
    if( !TaskLoaded ) return( 0 );
    return( DoAccess() );
}

trap_retval ReqProg_step( void )
{
    return( ReqProg_go() );
}

trap_retval ReqSet_user_screen( void )
{
    return( 0 );
}

trap_retval ReqSet_debug_screen( void )
{
    return( 0 );
}

trap_retval ReqThread_get_next( void )
{
    if( !TaskLoaded ) {
        thread_get_next_ret *ret;
        ret = GetOutPtr( 0 );
        ret->thread = 0;
        return( sizeof( *ret ) );
    }
    return( DoAccess() );
}

trap_retval ReqThread_set( void )
{
    if( !TaskLoaded ) return( 0 );
    return( DoAccess() );
}

trap_retval ReqThread_freeze( void )
{
    if( !TaskLoaded ) return( 0 );
    return( DoAccess() );
}

trap_retval ReqThread_thaw( void )
{
    if( !TaskLoaded ) return( 0 );
    return( DoAccess() );
}

trap_retval ReqThread_get_extra( void )
{
    if( !TaskLoaded ) return( 0 );
    return( DoAccess() );
}

static trap_retval (* const FileRequests[])(void) = {
    ReqFile_get_config,
    ReqFile_open,
    ReqFile_seek,
    ReqFile_read,
    ReqFile_write,
    ReqFile_write_console,
    ReqFile_close,
    ReqFile_erase,
    ReqFile_string_to_fullpath,
    ReqFile_run_cmd,
};

typedef struct {
    const char *name;
    const void *vectors;
} service_entry;

trap_retval ReqGet_supplementary_service( void )
{
    char                                *name;
    get_supplementary_service_ret       *out;

    if( TaskLoaded ) {
        return( DoAccess() );
    }
    name = GetInPtr( sizeof( get_supplementary_service_req ) );
    out = GetOutPtr( 0 );
    out->err = 0;
    out->id = 0;
    if( stricmp( FILE_SUPP_NAME, name ) == 0 ) {
        out->id = (unsigned_32)FileRequests;
    }
    return( sizeof( *out ) );
}

trap_retval ReqPerform_supplementary_service( void )
{
    unsigned    (* const * _WCUNALIGNED *vectors)(void);
    access_req  *sup_req;

    if( TaskLoaded ) {
        return( DoAccess() );
    }
    vectors = GetInPtr( sizeof( access_req ) );
    sup_req = GetInPtr( sizeof( supp_prefix ) );
    return( (*vectors)[*sup_req]() );
}

trap_retval ReqSplit_cmd( void )
{
    char                *cmd;
    char                *start;
    split_cmd_ret       *ret;
    trap_elen           len;

    cmd = GetInPtr( sizeof( split_cmd_req ) );
    ret = GetOutPtr( 0 );
    start = cmd;
    len = GetTotalSize() - sizeof( split_cmd_req );
    for( ;; ) {
        if( len == 0 ) goto done;
        switch( *cmd ) {
        case '/':
        case '=':
        case '(':
        case ';':
        case ',':
            goto done;
        case '\0':
        case ' ':
        case '\t':
            ret->parm_start = cmd - start + 1;
            ret->cmd_end = cmd - start;
            return( sizeof( *ret ) );
        }
        ++cmd;
        --len;
    }
done:
    ret->parm_start = cmd - start;
    ret->cmd_end = cmd - start;
    return( sizeof( *ret ) );
}

trap_version TRAPENTRY TrapInit( char *parm, char *error, bool remote )
{
    trap_version    ver;
    extern     void InitPSP();

    remote = remote;
    ver.remote = FALSE;
    ver.major = TRAP_MAJOR_VERSION;
    ver.minor = TRAP_MINOR_VERSION;
    FakeHandle = GetStdHandle( STD_INPUT_HANDLE );
    error[0] = '\0';
    strcpy( LinkParm, parm );
    TaskLoaded = FALSE;
    return( ver );
}

void TRAPENTRY TrapFini( void )
{
    RemoteDisco(); // just for debugging
    CloseHandle( FakeHandle );
}
