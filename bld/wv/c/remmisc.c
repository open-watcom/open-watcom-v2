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
* Description:  Miscellaneous remote access routines (via trap file).
*
****************************************************************************/


#include "dbgdefn.h"
#if !defined( BUILD_RFX )
#include "dbgdata.h"
#include "dbglit.h"
#include "remthrd.h"
#include "remrtrd.h"
#else
#include "rfxdata.h"
#endif
#include "dbgmem.h"
#include "trpld.h"
#include "trpcore.h"
#include "dbgio.h"
#include "strutil.h"
#include "trapglbl.h"
#include "filermt.h"
#include "remcore.h"
#include "remfile.h"
#include "removl.h"
#include "remasync.h"
#include "remenv.h"
#include "dbginit.h"
#include "dbgerr.h"

#include "clibext.h"


//extern void             TrapErrTranslate( char *, int );

trap_elen           MaxPacketLen;

//NYI: We don't know the size of the incoming err msg.
#define MAX_ERR_MSG_SIZE        (TXT_LEN/2)

#if !defined( BUILD_RFX )

static void TrapFailed( void )
{
    UnLoadTrap();
    StartupErr( LIT_ENG( ERR_REMOTE_LINK_BROKEN ) );
}

void InitSuppServices( void )
{
    if( InitCoreSupp() ) {
        InitFileSupp();
        InitFileInfoSupp();
        InitEnvSupp();
        InitThreadSupp();
        InitRunThreadSupp();
        InitOvlSupp();
        InitAsyncSupp();
        InitCapabilities();
    }
}

void FiniSuppServices( void )
{
    FiniCoreSupp();
}

#endif

static bool InitTrapError;

void InitTrap( const char *parms )
{
    in_mx_entry         in[1];
    mx_entry            out[2];
    connect_req         acc;
    connect_ret         ret;
    char                *error;
    trap_version        ver;
    char                buff[TXT_LEN];

#ifdef ENABLE_TRAP_LOGGING
    OpenTrapTraceFile();
#endif

/* Don't use TxtBuff except for error -- it may have a Finger message in it */

#if !defined( BUILD_RFX )
    TrapSetFailCallBack( TrapFailed );
#endif
    InitTrapError = false;
    RestoreHandlers();
    ver.remote = false;
    if( parms == NULL )
        parms = "";
#if !defined( BUILD_RFX )
    if( stricmp( parms, "dumb" ) == 0 ) {
        error = LoadDumbTrap( &ver );
    } else {
#endif
        switch( LoadTrap( parms, buff, &ver ) ) {
        #define DIGS_ERROR(e,t) case e: error = t; break;
        DIGS_ERRORS( "TRAP Loader: ", buff )
        #undef DIGS_ERROR
        default: error = DIGS_ERRORS_default( "TRAP Loader: " ); break;
        }
#if !defined( BUILD_RFX )
    }
#endif
    GrabHandlers();
    if( error != NULL ) {
        strcpy( buff, error );
        InitTrapError = true;
        StartupErr( buff );
    }
    acc.req = REQ_CONNECT;
    acc.ver.major = TRAP_VERSION_MAJOR;
    acc.ver.minor = TRAP_VERSION_MINOR;
    acc.ver.remote = false;
    in[0].ptr = &acc;
    in[0].len = sizeof( acc );
    out[0].ptr = &ret;
    out[0].len = sizeof( ret );
    buff[0] = NULLCHAR;
    out[1].ptr = buff;
    out[1].len = MAX_ERR_MSG_SIZE;
    TrapAccess( 1, in, 2, out );
    MaxPacketLen = ret.max_msg_size;
    if( buff[0] != NULLCHAR ) {
        UnLoadTrap();
        InitTrapError = true;
        StartupErr( buff );
    }
#if !defined( BUILD_RFX )
    if( !InitTrapError ) {
        InitSuppServices();
    }
#endif
    if( ver.remote ) {
        _SwitchOn( SW_REMOTE_LINK );
    } else {
        _SwitchOff( SW_REMOTE_LINK );
    }
}

trap_shandle GetSuppId( char *name )
{
    in_mx_entry                         in[2];
    mx_entry                            out[1];
    get_supplementary_service_req       acc;
    get_supplementary_service_ret       ret;

    acc.req = REQ_GET_SUPPLEMENTARY_SERVICE;
    in[0].ptr = &acc;
    in[0].len = sizeof( acc );
    in[1].ptr = name;
    in[1].len = (trap_elen)( strlen( name ) + 1 );
    out[0].ptr = &ret;
    out[0].len = sizeof( ret );
    TrapAccess( 2, in, 1, out );
    if( ret.err != 0 )
        return( 0 );
    return( ret.id );
}


void RemoteSuspend( void )
{
    suspend_req         acc;

    acc.req = REQ_SUSPEND;
    TrapSimpleAccess( sizeof( acc ), &acc, 0, NULL );
}

void RemoteResume( void )
{
    resume_req          acc;

    acc.req = REQ_RESUME;
    TrapSimpleAccess( sizeof( acc ), &acc, 0, NULL );
}

void RemoteErrMsg( sys_error err, char *msg )
{
    get_err_text_req    acc;

    acc.req = REQ_GET_ERR_TEXT;
    acc.err = err;
    TrapSimpleAccess( sizeof( acc ), &acc, MAX_ERR_MSG_SIZE, msg );
//    TrapErrTranslate( msg, MAX_ERR_MSG_SIZE );
}

void FiniTrap( void )
{
    disconnect_req      acc;

    acc.req = REQ_DISCONNECT;
    TrapSimpleAccess( sizeof( acc ), &acc, 0, NULL );
    RestoreHandlers();
    UnLoadTrap();
    GrabHandlers();
#if !defined( BUILD_RFX )
    FiniSuppServices();
#endif
#ifdef ENABLE_TRAP_LOGGING
    CloseTrapTraceFile();
#endif
}

#if 0
bool ReInitTrap( const char *parms )
/***************************************/
{
    // only tested under NT - this is here for Lexus/Fusion/What's my name?
    FiniTrap();
    InitTrap( parms );
    return( !InitTrapError );
}
#endif
