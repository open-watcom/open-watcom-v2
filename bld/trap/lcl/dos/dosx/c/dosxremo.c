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


//#define DEBUG_TRAP
#include <string.h>
#include "trpimp.h"
#include "trperr.h"
#include "packet.h"
#include "tinyio.h"
#include "trapdbg1.h"
#include "trapdbg2.h"
#include "winchk.h"
#include "madregs.h"

extern void             SaveVectors(unsigned long *);
extern void             RestoreVectors(unsigned long *);
extern tiny_ret_t       FindFilePath( char *, char *, char * );

static char             LinkParm[256];
static char             FailMsg[128];
static char             *LoadError;
static bool             TaskLoaded;

static unsigned long    OrigVectors[256];       /* original int vectors */
static unsigned long    LoadVectors[256];       /* int vectors after load */
static unsigned long    CurrVectors[256];       /* current int vectors */

static unsigned DoAccess()
{
    unsigned    left;
    unsigned    len;
    unsigned    i;
    unsigned    piece;

    _DBG_EnterFunc( "DoAccess()" );
    _DBG_Writeln( "StartPacket" );
    StartPacket();
    if( Out_Mx_Num == 0 ) {
        /* Tell the server we're not expecting anything back */
        *(access_req *)In_Mx_Ptr[0].ptr |= 0x80;
    }
    for( i = 0; i < In_Mx_Num; ++i ) {
        _DBG_Writeln( "AddPacket" );
        AddPacket( In_Mx_Ptr[i].len, In_Mx_Ptr[i].ptr );
    }
    *(access_req *)In_Mx_Ptr[0].ptr &= ~0x80;
    _DBG_Writeln( "PutPacket" );
    PutPacket();
    if( Out_Mx_Num != 0 ) {
        _DBG_Writeln( "GetPacket" );
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
            _DBG_Writeln( "RemovePacket" );
            RemovePacket( piece, Out_Mx_Ptr[i].ptr );
            i++;
            left -= piece;
            if( left == 0 ) break;
        }
    } else {
        len = 0;
        left = 0;
    }
    _DBG_ExitFunc( "DoAccess()" );
    return( len - left );
}


unsigned ReqGet_sys_config()
{
    get_sys_config_ret  *ret;

    if( !TaskLoaded ) {
        ret = GetOutPtr(0);
        ret->sys.os = OS_PHARLAP;
        ret->sys.osmajor = 0;
        ret->sys.osminor = 0;
        ret->sys.fpu = X86_NO;
        ret->sys.huge_shift = 12;
        ret->sys.cpu = 3;
        ret->sys.mad = MAD_X86;
        return( sizeof( *ret ) );
    }
    return( DoAccess() );
}

unsigned ReqGet_err_text()
{
    static char *DosErrMsgs[] = {
#include "dosmsgs.h"
    };
    get_err_text_req    *acc;
    char                *err_txt;

    if( !TaskLoaded ) {
        acc = GetInPtr( 0 );
        err_txt = GetOutPtr( 0 );
        if( LoadError != NULL ) {
            strcpy( err_txt, LoadError );
            LoadError = NULL;
        } else if( acc->err > (sizeof(DosErrMsgs) / sizeof(char *) - 1) ) {
            strcpy( err_txt, TRP_ERR_unknown_system_error );
        } else {
            strcpy( err_txt, DosErrMsgs[ acc->err ] );
        }
        return( strlen( err_txt ) + 1 );
    }
    return( DoAccess() );
}

unsigned ReqMap_addr()
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

unsigned ReqRead_io()
{
    if( !TaskLoaded ) {
        return( 0 );
    }
    return( DoAccess() );
}

unsigned ReqWrite_io()
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
unsigned ReqRead_cpu()
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
unsigned ReqRead_fpu()
{
    read_fpu_ret        *ret;

    if( !TaskLoaded ) {
        ret = GetOutPtr(0);
        memset( ret, 0, sizeof( *ret ) );
        return( sizeof( *ret ) );
    }
    return( DoAccess() );
}

unsigned ReqRead_regs()
{
    mad_registers       *mr;

    if( !TaskLoaded ) {
        mr = GetOutPtr(0);
        memset( mr, 0, sizeof( mr->x86 ) );
        return( sizeof( mr->x86 ) );
    }
    return( DoAccess() );
}


unsigned ReqChecksum_mem()
{
    checksum_mem_ret    *ret;

    if( !TaskLoaded ) {
        ret = GetOutPtr( 0 );
        ret->result = 0;
        return( sizeof( *ret ) );
    }
    return( DoAccess() );
}

unsigned ReqGet_next_alias()
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

unsigned ReqProg_go()
{
    prog_go_ret         *ret;
    unsigned            len;

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

//OBSOLETE - use ReqMachine_data
unsigned ReqAddr_info()
{
    addr_info_ret       *ret;

    if( !TaskLoaded ) {
        ret = GetOutPtr( 0 );
        ret->is_32 = TRUE;
        return( sizeof( *ret ) );
    }
    return( DoAccess() );
}

unsigned ReqMachine_data()
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

unsigned ReqGet_lib_name()
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

unsigned ReqRead_mem()
{
    if( !TaskLoaded ) {
        return( 0 );
    }
    return( DoAccess() );
}

unsigned ReqWrite_mem()
{
    write_mem_ret       *ret;

    if( !TaskLoaded ) {
        ret = GetOutPtr( 0 );
        ret->len = 0;
        return( sizeof( *ret ) );
    }
    return( DoAccess() );
}

static char DosXExtList[] = {
/* Don't put any commas in this list. It's supposed to be all one string. */
#ifndef DOS4G
    ".exp\0"
    ".rex\0"
#endif
    ".exe\0"
};

char *GetExeExtensions()
{
    return( DosXExtList );
}

unsigned ReqProg_load()
{
    char                buffer[160];
    char                *src;
    char                *dst;
    char                *name;
    char                *endparm;
    char                *err;
    tiny_ret_t          rc;
    prog_load_ret       *ret;
    unsigned_16         len;

    SaveVectors( OrigVectors );
    _DBG_EnterFunc( "AccLoadProg()" );
    ret = GetOutPtr( 0 );
    src = name = GetInPtr( sizeof( prog_load_req ) );
    rc = FindFilePath( src, (char *)buffer, DosXExtList );
    endparm = LinkParm + strlen( LinkParm ) + 1;
    strcpy( endparm, buffer );
    err = RemoteLink( LinkParm, 0 );
    if( err != NULL ) {
        _DBG_Writeln( "Can't RemoteLink" );
        TinyWrite( 2, LoadError, strlen( LoadError ) );
        LoadError = err;
        ret->err = 1;
    } else {
        while( *src != '\0' ) ++src;
        if( rc == 0 ) {
            ++src;
            len = GetTotalSize() - (src - name) - sizeof( prog_load_req );
            dst = (char *)buffer;
            while( *dst != '\0' ) ++dst;
            ++dst;
            memcpy( dst, src, len );
            dst += len;
            _DBG_Writeln( "StartPacket" );
            StartPacket();
            _DBG_Writeln( "AddPacket" );
            AddPacket( sizeof( prog_load_req ), In_Mx_Ptr[0].ptr );
            _DBG_Writeln( "AddPacket" );
            AddPacket( dst - buffer, buffer );
            _DBG_Writeln( "PutPacket" );
            PutPacket();
            _DBG_Writeln( "GetPacket" );
            len = GetPacket();
            _DBG_Writeln( "RemovePacket" );
            RemovePacket( sizeof( *ret ), ret );
        } else {
            len = DoAccess();
        }
        _DBG_Writeln( "Linked --" );
        if( ret->err != 0 ) {
            get_err_text_req    erracc;
            prog_kill_req       killacc;
            int                 msg_len;

            _DBG_Writeln( "loadret->errcode != 0" );
            if( LoadError == NULL ) {
                _DBG_Writeln( "making a REQ_GET_ERR_TEXT request" );
                erracc.req = REQ_GET_ERR_TEXT;
                erracc.err = ret->err;
                _DBG_Writeln( "StartPacket" );
                StartPacket();
                _DBG_Writeln( "AddPacket" );
                AddPacket( sizeof( erracc ), &erracc );
                _DBG_Writeln( "PutPacket" );
                PutPacket();
                _DBG_Writeln( "GetPacket" );
                msg_len = GetPacket();
                _DBG_Writeln( "RemovePacket" );
                RemovePacket( msg_len, FailMsg );
                _DBG_Write( "FailMsg :  " );
                _DBG_NoTabWriteln( FailMsg );
                LoadError = FailMsg;
            }

            _DBG_Writeln( "making a REQ_PROG_KILL request" );
            killacc.req = REQ_PROG_KILL;
            _DBG_Writeln( "StartPacket" );
            StartPacket();
            _DBG_Writeln( "AddPacket" );
            AddPacket( sizeof( killacc ), &killacc );
            _DBG_Writeln( "PutPacket" );
            PutPacket();
            _DBG_Writeln( "GetPacket" );
            GetPacket();
            //RemovePacket( msg_len, &erracc );
            RemoteUnLink();

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

unsigned ReqProg_kill()
{
    int         len;
    prog_kill_ret       *ret;

    if( !TaskLoaded ) {
        ret = GetOutPtr( 0 );
        ret->err = 0;
        return( sizeof( *ret ) );
    }
    len = DoAccess();
    RemoteUnLink();
    TaskLoaded = FALSE;
    RestoreVectors( OrigVectors );
    return( len );
}

// OBSOLETE - use ReqWrite_regs
unsigned ReqWrite_cpu()
{
    return( DoAccess() );
}

// OBSOLETE - use ReqWrite_regs
unsigned ReqWrite_fpu()
{
    return( DoAccess() );
}
unsigned ReqWrite_regs()
{
    return( DoAccess() );
}

unsigned ReqSet_watch()
{
    return( DoAccess() );
}

unsigned ReqClear_watch()
{
    return( DoAccess() );
}

unsigned ReqSet_break()
{
    return( DoAccess() );
}

unsigned ReqClear_break()
{
    return( DoAccess() );
}

unsigned ReqGet_message_text()
{
    return( DoAccess() );
}

unsigned ReqRedirect_stdin()
{
    return( DoAccess() );
}

unsigned ReqRedirect_stdout()
{
    return( DoAccess() );
}

unsigned ReqProg_step()
{
    return( ReqProg_go() );
}

#pragma off(unreferenced);
trap_version TRAPENTRY TrapInit( char *parm, char *error,
                                       bool remote )
#pragma on(unreferenced);
{
    trap_version    ver;
    extern     void InitPSP();

    ver.remote = FALSE;
    ver.major = TRAP_MAJOR_VERSION;
    ver.minor = TRAP_MINOR_VERSION;
    if( !remote && DPMIVersion() == 90 ) {
        strcpy( error, TRP_ERR_bad_dpmi );
        return( ver );
    }
    _DBG_EnterFunc( "TrapInit()" );
    InitPSP();
    LoadError = NULL;
    error[0] = '\0';
    strcpy( LinkParm, parm );
    TaskLoaded = FALSE;
    _DBG_ExitFunc( "TrapInit()" );
    return( ver );
}

void TRAPENTRY TrapFini()
{
    _DBG_EnterFunc( "TrapFini()" );
    RemoteDisco(); // just for debugging
    _DBG_ExitFunc( "TrapFini()" );
}
