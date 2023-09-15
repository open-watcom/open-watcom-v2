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
* Description:  Extended DOS trap file main module.
*
****************************************************************************/


//#define DEBUG_TRAP
#include <stdlib.h>
#include <string.h>
#include "digcpu.h"
#include "trpimp.h"
#include "trpcomm.h"
#include "trperr.h"
#include "doserr.h"
#include "packet.h"
#include "tinyio.h"
#include "trapdbg.h"
#include "winchk.h"
#include "madregs.h"
#include "dosxlink.h"
#include "dosxfork.h"
#include "dospath.h"


static char             LinkParms[256];
static char             FailMsg[128];
static const char       *LoadError;
static bool             TaskLoaded;

static unsigned long    OrigVectors[256];       /* original int vectors */
static unsigned long    LoadVectors[256];       /* int vectors after load */
static unsigned long    CurrVectors[256];       /* current int vectors */


static trap_elen DoAccess( void )
{
    trap_elen  left;
    trap_elen  len;
    trap_elen  i;
    trap_elen  piece;

    _DBG_EnterFunc( "DoAccess()" );
    _DBG_Writeln( "StartPacket" );
    StartPacket();
    if( Out_Mx_Num == 0 ) {
        /* Tell the server we're not expecting anything back */
        TRP_REQUEST( In_Mx_Ptr ) |= REQ_WANT_RETURN;
    }
    for( i = 0; i < In_Mx_Num; ++i ) {
        _DBG_Writeln( "AddPacket" );
        AddPacket( In_Mx_Ptr[i].ptr, In_Mx_Ptr[i].len );
    }
    TRP_REQUEST( In_Mx_Ptr ) &= ~REQ_WANT_RETURN;
    _DBG_Writeln( "PutPacket" );
    PutPacket();
    if( Out_Mx_Num != 0 ) {
        _DBG_Writeln( "GetPacket" );
        len = GetPacket();
        left = len;
        for( i = 0; i < Out_Mx_Num && left > 0; ++i ) {
            if( left > Out_Mx_Ptr[i].len ) {
                piece = Out_Mx_Ptr[i].len;
            } else {
                piece = left;
            }
            _DBG_Writeln( "RemovePacket" );
            RemovePacket( Out_Mx_Ptr[i].ptr, piece );
            left -= piece;
        }
    } else {
        len = 0;
        left = 0;
    }
    _DBG_ExitFunc( "DoAccess()" );
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
        ret->fpu = X86_NOFPU;
        ret->huge_shift = 12;
        ret->cpu = X86_386;
        ret->arch = DIG_ARCH_X86;
        return( sizeof( *ret ) );
    }
    return( DoAccess() );
}

trap_retval TRAP_CORE( Get_err_text )( void )
{
    static char *DosErrMsgs[] = {
        #define pick(a,b)   b,
        #include "dosmsgs.h"
        #undef pick
    };
    get_err_text_req    *acc;
    char                *err_txt;

    if( !TaskLoaded ) {
        acc = GetInPtr( 0 );
        err_txt = GetOutPtr( 0 );
        if( LoadError != NULL ) {
            strcpy( err_txt, LoadError );
            LoadError = NULL;
        } else if( acc->err < ERR_LAST ) {
            strcpy( err_txt, DosErrMsgs[acc->err] );
        } else {
            strcpy( err_txt, TRP_ERR_unknown_system_error );
            ultoa( acc->err, err_txt + strlen( err_txt ), 16 );
        }
        return( strlen( err_txt ) + 1 );
    }
    return( DoAccess() );
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
    mad_registers       *mr;

    if( !TaskLoaded ) {
        mr = GetOutPtr( 0 );
        memset( mr, 0, sizeof( mr->x86 ) );
        return( sizeof( mr->x86 ) );
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
    RestoreVectors( CurrVectors );
    len = DoAccess();
    SaveVectors( CurrVectors );
    RestoreVectors( LoadVectors );
    return( len );
}

trap_retval TRAP_CORE( Machine_data )( void )
{
    machine_data_req    *acc;
    machine_data_ret    *ret;
    machine_data_spec   *data;

    if( !TaskLoaded ) {
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
    return( DoAccess() );
}

trap_retval TRAP_CORE( Get_lib_name )( void )
{
    get_lib_name_ret    *ret;

    if( !TaskLoaded ) {
        ret = GetOutPtr( 0 );
        ret->mod_handle = 0;
        return( sizeof( *ret ) );
    }
    return( DoAccess() );
}

trap_retval TRAP_CORE( Read_mem )( void )
{
    if( !TaskLoaded ) {
        return( 0 );
    }
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
    bool                prog_ok;
    prog_load_ret       *ret;
    size_t              len;

    SaveVectors( OrigVectors );
    _DBG_EnterFunc( "AccLoadProg()" );
    ret = GetOutPtr( 0 );
    ret->err = 0;
    src = name = GetInPtr( sizeof( prog_load_req ) );
    prog_ok = ( FindFilePath( DIG_FILETYPE_EXE, src, buffer ) != 0 );
    endparm = LinkParms;
    while( *endparm++ != '\0' )         // skip trap parameters
        {}
    strcpy( endparm, buffer );          // add command line
    // result is as follow
    // "trap parameters string"+"\0"+"command line string"+"\0"
    err = RemoteLink( LinkParms, false );
    if( err != NULL ) {
        _DBG_Writeln( "Can't RemoteLink" );
        TinyWrite( TINY_ERR, err, strlen( err ) );
        LoadError = err;
        ret->err = 1;
        len = 0;
    } else {
        if( prog_ok ) {
            while( *src++ != '\0' )
                {}
            len = GetTotalSizeIn() - sizeof( prog_load_req ) - ( src - name );
            dst = (char *)buffer;
            while( *dst++ != '\0' )
                {}
            memcpy( dst, src, len );
            dst += len;
            _DBG_Writeln( "StartPacket" );
            StartPacket();
            _DBG_Writeln( "AddPacket" );
            AddPacket( In_Mx_Ptr[0].ptr, sizeof( prog_load_req ) );
            _DBG_Writeln( "AddPacket" );
            AddPacket( buffer, dst - buffer );
            _DBG_Writeln( "PutPacket" );
            PutPacket();
            _DBG_Writeln( "GetPacket" );
            len = GetPacket();
            _DBG_Writeln( "RemovePacket" );
            RemovePacket( ret, sizeof( *ret ) );
        } else {
            len = DoAccess();
        }
        _DBG_Writeln( "Linked --" );
        if( ret->err != 0 ) {
            get_err_text_req    erracc;
            prog_kill_req       killacc;
            trap_elen           msg_len;

            _DBG_Writeln( "loadret->errcode != 0" );
            if( LoadError == NULL ) {
                _DBG_Writeln( "making a REQ_GET_ERR_TEXT request" );
                erracc.req = REQ_GET_ERR_TEXT;
                erracc.err = ret->err;
                _DBG_Writeln( "StartPacket" );
                StartPacket();
                _DBG_Writeln( "AddPacket" );
                AddPacket( &erracc, sizeof( erracc ) );
                _DBG_Writeln( "PutPacket" );
                PutPacket();
                _DBG_Writeln( "GetPacket" );
                msg_len = GetPacket();
                _DBG_Writeln( "RemovePacket" );
                RemovePacket( FailMsg, msg_len );
                _DBG_Write( "FailMsg :  " );
                _DBG_NoTabWriteln( FailMsg );
                LoadError = FailMsg;
            }

            _DBG_Writeln( "making a REQ_PROG_KILL request" );
            killacc.req = REQ_PROG_KILL;
            _DBG_Writeln( "StartPacket" );
            StartPacket();
            _DBG_Writeln( "AddPacket" );
            AddPacket( &killacc, sizeof( killacc ) );
            _DBG_Writeln( "PutPacket" );
            PutPacket();
            _DBG_Writeln( "GetPacket" );
            GetPacket();
            //RemovePacket( &erracc, msg_len );
            RemoteUnLink();

            TaskLoaded = false;
        }
    }
    if( ret->err == 0 ) {
        _DBG_Writeln( "loadret->error_code == 0" );
        TaskLoaded = true;
    }
    SaveVectors( LoadVectors );
    SaveVectors( CurrVectors );
    _DBG_ExitFunc( "AccLoadProg()" );
    return( len );
}

trap_retval TRAP_CORE( Prog_kill )( void )
{
    trap_elen       len;
    prog_kill_ret   *ret;

    if( !TaskLoaded ) {
        ret = GetOutPtr( 0 );
        ret->err = 0;
        return( sizeof( *ret ) );
    }
    len = DoAccess();
    RemoteUnLink();
    TaskLoaded = false;
    RestoreVectors( OrigVectors );
    return( len );
}

trap_retval TRAP_CORE( Write_regs )( void )
{
    return( DoAccess() );
}

trap_retval TRAP_CORE( Set_watch )( void )
{
    return( DoAccess() );
}

trap_retval TRAP_CORE( Clear_watch )( void )
{
    return( DoAccess() );
}

trap_retval TRAP_CORE( Set_break )( void )
{
    return( DoAccess() );
}

trap_retval TRAP_CORE( Clear_break )( void )
{
    return( DoAccess() );
}

trap_retval TRAP_CORE( Get_message_text )( void )
{
    return( DoAccess() );
}

trap_retval TRAP_CORE( Redirect_stdin )( void )
{
    return( DoAccess() );
}

trap_retval TRAP_CORE( Redirect_stdout )( void )
{
    return( DoAccess() );
}

trap_retval TRAP_CORE( Prog_step )( void )
{
    return( TRAP_CORE( Prog_go )() );
}

trap_version TRAPENTRY TrapInit( const char *parms, char *err, bool remote )
{
    trap_version    ver;

    ver.remote = false;
    ver.major = TRAP_VERSION_MAJOR;
    ver.minor = TRAP_VERSION_MINOR;
    if( !remote && DPMIVersion() == 90 && !DOSEMUCheck() ) {
        strcpy( err, TRP_ERR_bad_dpmi );
        return( ver );
    }
    _DBG_EnterFunc( "TrapInit()" );
    InitPSP();
    LoadError = NULL;
    err[0] = '\0';
    strcpy( LinkParms, parms );      // save trap parameters
    TaskLoaded = false;
    _DBG_ExitFunc( "TrapInit()" );
    return( ver );
}

void TRAPENTRY TrapFini( void )
{
    _DBG_EnterFunc( "TrapFini()" );
    RemoteDisco(); // just for debugging
    _DBG_ExitFunc( "TrapFini()" );
}
