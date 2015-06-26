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
* Description:  Extended DOS trap file main module.
*
****************************************************************************/


//#define DEBUG_TRAP
#include <stdlib.h>
#include <string.h>
#include "trpimp.h"
#include "trperr.h"
#include "doserr.h"
#include "packet.h"
#include "tinyio.h"
#include "trapdbg.h"
#include "winchk.h"
#include "madregs.h"
#include "doscomm.h"
#include "dosxlink.h"
#include "dosextx.h"
#include "dosfile.h"


extern void             SaveVectors(unsigned long *);
extern void             RestoreVectors(unsigned long *);
extern void             InitPSP( void );

static char             LinkParm[256];
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
        *(access_req *)In_Mx_Ptr[0].ptr |= 0x80;
    }
    for( i = 0; i < In_Mx_Num; ++i ) {
        _DBG_Writeln( "AddPacket" );
        AddPacket( In_Mx_Ptr[i].ptr, In_Mx_Ptr[i].len );
    }
    *(access_req *)In_Mx_Ptr[0].ptr &= ~0x80;
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


trap_retval ReqGet_sys_config( void )
{
    get_sys_config_ret  *ret;

    if( !TaskLoaded ) {
        ret = GetOutPtr(0);
        ret->sys.os = MAD_OS_IDUNNO;
        ret->sys.osmajor = 0;
        ret->sys.osminor = 0;
        ret->sys.fpu = X86_NO;
        ret->sys.huge_shift = 12;
        ret->sys.cpu = X86_386;
        ret->sys.mad = MAD_X86;
        return( sizeof( *ret ) );
    }
    return( DoAccess() );
}

trap_retval ReqGet_err_text( void )
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
            strcpy( err_txt, DosErrMsgs[ acc->err ] );
        } else {
            strcpy( err_txt, TRP_ERR_unknown_system_error );
            ultoa( acc->err, err_txt + strlen( err_txt ), 16 );
        }
        return( strlen( err_txt ) + 1 );
    }
    return( DoAccess() );
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

trap_retval ReqRead_regs( void )
{
    mad_registers       *mr;

    if( !TaskLoaded ) {
        mr = GetOutPtr(0);
        memset( mr, 0, sizeof( mr->x86 ) );
        return( sizeof( mr->x86 ) );
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
    RestoreVectors( CurrVectors );
    len = DoAccess();
    SaveVectors( CurrVectors );
    RestoreVectors( LoadVectors );
    return( len );
}

trap_retval ReqMachine_data( void )
{
    machine_data_ret    *ret;
    unsigned_8          *data;

    if( !TaskLoaded ) {
        ret = GetOutPtr( 0 );
        data = GetOutPtr( sizeof( *ret ) );
        ret->cache_start = 0;
        ret->cache_end = ~(addr_off)0;
        *data = X86AC_BIG;
        return( sizeof( *ret ) + sizeof( *data ) );
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
    if( !TaskLoaded ) {
        return( 0 );
    }
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
    const char          *err;
    tiny_ret_t          rc;
    prog_load_ret       *ret;
    trap_elen           len;

    SaveVectors( OrigVectors );
    _DBG_EnterFunc( "AccLoadProg()" );
    ret = GetOutPtr( 0 );
    src = name = GetInPtr( sizeof( prog_load_req ) );
    rc = FindFilePath( src, buffer, DosExtList );
    endparm = LinkParm;
    while( *endparm++ != '\0' ) {}      // skip trap parameters
    strcpy( endparm, buffer );          // add command line
    // result is as follow
    // "trap parameters string"+"\0"+"command line string"+"\0"
    err = RemoteLinkX( LinkParm, FALSE );
    if( err != NULL ) {
        _DBG_Writeln( "Can't RemoteLink" );
        TinyWrite( TINY_ERR, err, strlen( err ) );
        LoadError = err;
        ret->err = 1;
        len = 0;
    } else {
        if( TINY_OK( rc ) ) {
            while( *src++ != '\0' ) {}
            len = GetTotalSize() - ( src - name ) - sizeof( prog_load_req );
            dst = (char *)buffer;
            while( *dst++ != '\0' ) {};
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
            RemoteUnLinkX();

            TaskLoaded = FALSE;
        }
    }
    if( ret->err == 0 ) {
        _DBG_Writeln( "loadret->error_code == 0" );
        TaskLoaded = TRUE;
    }
    SaveVectors( LoadVectors );
    SaveVectors( CurrVectors );
    _DBG_ExitFunc( "AccLoadProg()" );
    return( len );
}

trap_retval ReqProg_kill( void )
{
    trap_elen       len;
    prog_kill_ret   *ret;

    if( !TaskLoaded ) {
        ret = GetOutPtr( 0 );
        ret->err = 0;
        return( sizeof( *ret ) );
    }
    len = DoAccess();
    RemoteUnLinkX();
    TaskLoaded = FALSE;
    RestoreVectors( OrigVectors );
    return( len );
}

trap_retval ReqWrite_regs( void )
{
    return( DoAccess() );
}

trap_retval ReqSet_watch( void )
{
    return( DoAccess() );
}

trap_retval ReqClear_watch( void )
{
    return( DoAccess() );
}

trap_retval ReqSet_break( void )
{
    return( DoAccess() );
}

trap_retval ReqClear_break( void )
{
    return( DoAccess() );
}

trap_retval ReqGet_message_text( void )
{
    return( DoAccess() );
}

trap_retval ReqRedirect_stdin( void )
{
    return( DoAccess() );
}

trap_retval ReqRedirect_stdout( void )
{
    return( DoAccess() );
}

trap_retval ReqProg_step( void )
{
    return( ReqProg_go() );
}

trap_version TRAPENTRY TrapInit( const char *parms, char *error, bool remote )
{
    trap_version    ver;

    remote = remote;
    ver.remote = FALSE;
    ver.major = TRAP_MAJOR_VERSION;
    ver.minor = TRAP_MINOR_VERSION;
    if( !remote && DPMIVersion() == 90 && !DOSEMUCheck() ) {
        strcpy( error, TRP_ERR_bad_dpmi );
        return( ver );
    }
    _DBG_EnterFunc( "TrapInit()" );
    InitPSP();
    LoadError = NULL;
    error[0] = '\0';
    strcpy( LinkParm, parms );      // save trap parameters
    TaskLoaded = FALSE;
    _DBG_ExitFunc( "TrapInit()" );
    return( ver );
}

void TRAPENTRY TrapFini( void )
{
    _DBG_EnterFunc( "TrapFini()" );
    RemoteDisco(); // just for debugging
    _DBG_ExitFunc( "TrapFini()" );
}
