/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2023 The Open Watcom Contributors. All Rights Reserved.
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
#include "trpcomm.h"
#include "trperr.h"
#include "packet.h"
#include "jvmerr.h"
#include "madregs.h"
#include "jvmxremo.h"


bool            TaskLoaded;
HANDLE          FakeHandle;
HWND            DebuggerHwnd;

static char     LinkParms[256];
static char     SavedError[256];

trap_retval DoAccess( void )
{
    trap_elen  left;
    trap_elen  len;
    trap_elen  i;
    trap_elen  piece;

    StartPacket();
    if( Out_Mx_Num == 0 ) {
        /* Tell the server we're not expecting anything back */
        TRP_REQUEST( In_Mx_Ptr ) |= REQ_WANT_RETURN;
    }
    for( i = 0; i < In_Mx_Num; ++i ) {
        AddPacket( In_Mx_Ptr[i].ptr, In_Mx_Ptr[i].len );
    }
    TRP_REQUEST( In_Mx_Ptr ) &= ~REQ_WANT_RETURN;
    PutPacket();
    if( Out_Mx_Num != 0 ) {
        len = GetPacket();
        left = len;
        for( i = 0; i < Out_Mx_Num; ) {
            if( left > Out_Mx_Ptr[i].len ) {
                piece = Out_Mx_Ptr[i].len;
            } else {
                piece = left;
            }
            RemovePacket( Out_Mx_Ptr[i].ptr, piece );
            i++;
            left -= piece;
            if( left == 0 ) {
                break;
            }
        }
    } else {
        len = 0;
        left = 0;
    }
    return( len - left );
}


trap_retval TRAP_CORE( Get_sys_config )( void )
{
    get_sys_config_ret  *ret;

    if( !TaskLoaded ) {
        ret = GetOutPtr( 0 );
        ret->os = DIG_OS_IDUNNO;
        ret->osmajor = 0;
        ret->osminor = 0;
        ret->fpu = 0;
        ret->huge_shift = 12;
        ret->cpu = 0;
        ret->arch = DIG_ARCH_JVM;
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
    while( ( q = strchr( *p, '\r' ) ) != NULL )
        *q = ' ';
    while( ( q = strchr( *p, '\n' ) ) != NULL )
        *q = ' ';
    va_end( args );
    return( len );
}

static char *Errors[] = {
    #define pick( a,b,c ) c,
    #include "jvmepick.h"
    #undef pick
};

trap_retval TRAP_CORE( Get_err_text )( void )
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
        strcpy( err_txt, Errors[ErrIndex( acc->err )] );
    } else {
        len = DoFmtMsg( &lpMessageBuffer, acc->err, "%1","%2","%3","%4" );
        if( len > 0 ) {
            strcpy( err_txt, lpMessageBuffer );
            LocalFree( lpMessageBuffer );
        }
    }
    return( strlen( err_txt ) + 1 );
}

trap_retval TRAP_CORE( Map_addr )( void )
{
    map_addr_req        *acc;
    map_addr_ret        *ret;

    if( !TaskLoaded ) {
        acc = GetInPtr( 0 );
        ret = GetOutPtr( 0 );
        ret->out_addr = acc->in_addr;
        ret->lo_bound = 0;
        ret->hi_bound = ~(addr48_off)0;
        return( sizeof( *ret ) );
    }
    return( DoAccess() );
}

trap_retval TRAP_CORE( Read_io )( void )
{
    if( !TaskLoaded ) {
        return( 0 );
    }
    return( DoAccess() );
}

trap_retval TRAP_CORE( Write_io )( void )
{
    write_io_ret        *ret;

    if( !TaskLoaded ) {
        ret = GetOutPtr( 0 );
        ret->len = 0;
        return( sizeof( *ret ) );
    }
    return( DoAccess() );
}

trap_retval TRAP_CORE( Read_regs )( void )
{

    if( !TaskLoaded ) {
        mad_registers *mr;
        mr = GetOutPtr( 0 );
        memset( mr, 0, sizeof( mr->jvm ) );
        return( sizeof( mr->jvm ) );
    }
    return( DoAccess() );
}


trap_retval TRAP_CORE( Checksum_mem )( void )
{
    checksum_mem_ret    *ret;

    if( !TaskLoaded ) {
        ret = GetOutPtr( 0 );
        ret->result = 0;
        return( sizeof( *ret ) );
    }
    return( DoAccess() );
}

trap_retval TRAP_CORE( Get_next_alias )( void )
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

trap_retval TRAP_CORE( Prog_go )( void )
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

trap_retval TRAP_CORE( Machine_data )( void )
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

trap_retval TRAP_CORE( Get_lib_name )( void )
{
    get_lib_name_ret    *ret;

    if( !TaskLoaded ) {
        ret = GetOutPtr( 0 );
        ret->mod_handle = 0;
        *(char *)GetOutPtr( sizeof( *ret ) ) = '\0';
        return( sizeof( *ret ) + 1 );
    }
    return( DoAccess() );
}

trap_retval TRAP_CORE( Read_mem )( void )
{
    if( !TaskLoaded )
        return( 0 );
    return( DoAccess() );
}

trap_retval TRAP_CORE( Write_mem )( void )
{
    write_mem_ret       *ret;

    if( !TaskLoaded ) {
        ret = GetOutPtr( 0 );
        ret->len = 0;
        return( sizeof( *ret ) );
    }
    return( DoAccess() );
}

trap_retval TRAP_CORE( Prog_load )( void )
{
    char                buffer[160];
    char                *src;
    char                *dst;
    char                *name;
    char                *endparm;
    const char          *err;
    prog_load_ret       *ret;
    size_t              len;

    ret = GetOutPtr( 0 );
    src = name = GetInPtr( sizeof( prog_load_req ) );
    strcpy( buffer, src );
    endparm = LinkParms;
    while( *endparm++ != '\0' )
        {}
    strcpy( endparm, buffer );
    if( *name == '\0' ) {
        ret->err = ERROR_FILE_NOT_FOUND;
        return( sizeof( *ret ) );
    }
    err = RemoteLink( LinkParms, FALSE );
    if( err != NULL ) {
        strcpy( SavedError, err );
        ret->err = ERR_JVM_SAVED_ERROR;
        return( sizeof( *ret ) );
    }
    while( *src++ != '\0' )
        {}
    len = GetTotalSizeIn() - sizeof( prog_load_req ) - ( src - name );
    dst = buffer;
    while( *dst++ != '\0' )
        {}
    memcpy( dst, src, len );
    dst += len;
    StartPacket();
    AddPacket( In_Mx_Ptr[0].ptr, sizeof( prog_load_req ) );
    AddPacket( buffer, dst - buffer );
    PutPacket();
    len = GetPacket();
    RemovePacket( ret, sizeof( *ret ) );
    if( ret->err == 0 ) {
        TaskLoaded = TRUE;
        if( DebuggerHwnd != NULL )
            SetForegroundWindow( DebuggerHwnd );
        return( len );
    } else {
        prog_kill_req   killacc;

        killacc.req = REQ_PROG_KILL;
        StartPacket();
        AddPacket( &killacc, sizeof( killacc ) );
        PutPacket();
        GetPacket();
        //RemovePacket( &erracc, msg_len );
        RemoteUnLink();

        TaskLoaded = FALSE;
        return( sizeof( *ret ) );
    }
}

trap_retval TRAP_CORE( Prog_kill )( void )
{
    prog_kill_ret       *ret;

    if( TaskLoaded ) {
        DoAccess();
        TaskLoaded = FALSE;
        RemoteUnLink();
    }
    ret = GetOutPtr( 0 );
    ret->err = 0;
    return( sizeof( *ret ) );
}

trap_retval TRAP_CORE( Write_regs )( void )
{
    if( !TaskLoaded )
        return( 0 );
    return( DoAccess() );
}

trap_retval TRAP_CORE( Set_watch )( void )
{
    if( !TaskLoaded )
        return( 0 );
    return( DoAccess() );
}

trap_retval TRAP_CORE( Clear_watch )( void )
{
    if( !TaskLoaded )
        return( 0 );
    return( DoAccess() );
}

trap_retval TRAP_CORE( Set_break )( void )
{
    if( !TaskLoaded )
        return( 0 );
    return( DoAccess() );
}

trap_retval TRAP_CORE( Clear_break )( void )
{
    if( !TaskLoaded )
        return( 0 );
    return( DoAccess() );
}

trap_retval TRAP_CORE( Get_message_text )( void )
{
    if( !TaskLoaded )
        return( 0 );
    return( DoAccess() );
}

trap_retval TRAP_CORE( Redirect_stdin )( void )
{
    if( !TaskLoaded )
        return( 0 );
    return( DoAccess() );
}

trap_retval TRAP_CORE( Redirect_stdout )( void )
{
    if( !TaskLoaded )
        return( 0 );
    return( DoAccess() );
}

trap_retval TRAP_CORE( Prog_step )( void )
{
    return( TRAP_CORE( Prog_go )() );
}

trap_retval TRAP_CORE( Set_user_screen )( void )
{
    return( 0 );
}

trap_retval TRAP_CORE( Set_debug_screen )( void )
{
    return( 0 );
}

trap_retval TRAP_THREAD( get_next )( void )
{
    if( !TaskLoaded ) {
        thread_get_next_ret *ret;
        ret = GetOutPtr( 0 );
        ret->thread = 0;
        return( sizeof( *ret ) );
    }
    return( DoAccess() );
}

trap_retval TRAP_THREAD( set )( void )
{
    if( !TaskLoaded )
        return( 0 );
    return( DoAccess() );
}

trap_retval TRAP_THREAD( freeze )( void )
{
    if( !TaskLoaded )
        return( 0 );
    return( DoAccess() );
}

trap_retval TRAP_THREAD( thaw )( void )
{
    if( !TaskLoaded )
        return( 0 );
    return( DoAccess() );
}

trap_retval TRAP_THREAD( get_extra )( void )
{
    if( !TaskLoaded )
        return( 0 );
    return( DoAccess() );
}

static trap_retval (* const FileRequests[])(void) = {
    #define REQ_FILE_DEF(sym,func)      TRAP_FILE( func ),
    REQ_FILE_DEFS()
    #undef REQ_FILE_DEF
};

typedef struct {
    const char *name;
    const void *vectors;
} service_entry;

trap_retval TRAP_CORE( Get_supplementary_service )( void )
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
    if( stricmp( QUOTED( FILE_SUPP_NAME ), name ) == 0 ) {
        out->id = (unsigned_32)FileRequests;
    }
    return( sizeof( *out ) );
}

trap_retval TRAP_CORE( Perform_supplementary_service )( void )
{
    unsigned    (* const * _WCUNALIGNED *vectors)(void);
    trap_req    *sup_req;

    if( TaskLoaded ) {
        return( DoAccess() );
    }
    vectors = GetInPtr( sizeof( trap_req ) );
    sup_req = GetInPtr( sizeof( supp_prefix ) );
    return( (*vectors)[*sup_req]() );
}

trap_retval TRAP_CORE( Split_cmd )( void )
{
    const char          *cmd;
    const char          *start;
    split_cmd_ret       *ret;
    size_t              len;

    start = cmd = GetInPtr( sizeof( split_cmd_req ) );
    ret = GetOutPtr( 0 );
    ret->parm_start = 0;
    len = GetTotalSizeIn() - sizeof( split_cmd_req );
    while( len > 0 ) {
        switch( *cmd ) {
        CASE_SEPS
            ret->parm_start = 1;
            /* fall through */
        case '/':
        case '=':
        case '(':
        case ';':
        case ',':
            len = 0;
            continue;
        }
        ++cmd;
        --len;
    }
    ret->parm_start += cmd - start;
    ret->cmd_end = cmd - start;
    return( sizeof( *ret ) );
}

trap_version TRAPENTRY TrapInit( const char *parms, char *err, bool remote )
{
    trap_version    ver;

    /* unused parameters */ (void)remote;

    ver.remote = FALSE;
    ver.major = TRAP_VERSION_MAJOR;
    ver.minor = TRAP_VERSION_MINOR;
    FakeHandle = GetStdHandle( STD_INPUT_HANDLE );
    err[0] = '\0';
    strcpy( LinkParms, parms );
    TaskLoaded = FALSE;
    return( ver );
}

void TRAPENTRY TrapFini( void )
{
    RemoteDisco(); // just for debugging
    CloseHandle( FakeHandle );
}
