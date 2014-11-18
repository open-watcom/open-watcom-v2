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
* Description:  Miscellaneous remote access routines (via trap file).
*
****************************************************************************/


#include <string.h>
#include "dbgdefn.h"
#include "dbgtoggl.h"
#include "dbgmem.h"
#include "trpcore.h"
#include "dbgio.h"
#if !defined( BUILD_RFX )
#include "dbglit.h"
#endif

#include "clibext.h"


extern void             RestoreHandlers( void );
extern void             GrabHandlers( void );
extern void             StartupErr( char * );
//extern void             TrapErrTranslate( char *, int );
extern void             FiniCoreSupp( void );
extern bool             InitCoreSupp( void );
extern bool             InitFileSupp( void );
extern bool             InitFileInfoSupp( void );
extern bool             InitEnvSupp( void );
extern bool             InitOvlSupp( void );
extern bool             InitThreadSupp( void );
extern bool             InitRunThreadSupp( void );
extern bool             InitCapabilities( void );
extern bool             InitAsyncSupp( void );
extern void             StartupErr( char *err );
extern char             *DupStr( char * );

#ifdef ENABLE_TRAP_LOGGING
extern int              OpenTrapTraceFile( const char *path, bool flush_flag );
extern int              CloseTrapTraceFile( void );
extern char             *TrpDebugFile;
extern bool             TrpDebugFileFlush;
#endif

extern system_config    SysConfig;
extern char             *TxtBuff;
extern char             *TrpFile;

unsigned int            MaxPacketLen;

//NYI: We don't know the size of the incoming err msg.
#define MAX_ERR_MSG_SIZE        (TXT_LEN/2)

#if !defined( BUILD_RFX )

static void TrapFailed( void )
{
    KillTrap();
    StartupErr( LIT( ERR_REMOTE_LINK_BROKEN ) );
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
void InitTrap( char *trap_file )
{
    mx_entry            in[1];
    mx_entry            out[2];
    connect_req         in_mx;
    connect_ret         out_mx;
    char                *error;
    trap_version        ver;
    char                buff[ TXT_LEN ];

#ifdef ENABLE_TRAP_LOGGING
    if( TrpDebugFile )
        OpenTrapTraceFile( TrpDebugFile, TrpDebugFileFlush );
#endif

/* Don't use TxtBuff except for error -- it may have a Finger message in it */

#if !defined( BUILD_RFX )
    TrapSetFailCallBack( TrapFailed );
#endif
    InitTrapError = FALSE;
    RestoreHandlers();
    ver.remote = FALSE;
#if !defined( BUILD_RFX )
    if( stricmp( trap_file, "dumb" ) == 0 ) {
        error = LoadDumbTrap( &ver );
    } else {
#endif
        error = LoadTrap( trap_file, buff, &ver );
#if !defined( BUILD_RFX )
    }
#endif
    GrabHandlers();
    if( error != NULL ) {
        strcpy( buff, error );
        InitTrapError = TRUE;
        StartupErr( buff );
    }
    in_mx.req = REQ_CONNECT;
    in_mx.ver.major = TRAP_MAJOR_VERSION;
    in_mx.ver.minor = TRAP_MINOR_VERSION;
    in_mx.ver.remote = FALSE;
    in[0].ptr = &in_mx;
    in[0].len = sizeof( in_mx );
    out[0].ptr = &out_mx;
    out[0].len = sizeof( out_mx );
    buff[0] = '\0';
    out[1].ptr = buff;
    out[1].len = MAX_ERR_MSG_SIZE;
    TrapAccess( 1, in, 2, out );
    MaxPacketLen = out_mx.max_msg_size;
    if( buff[0] != '\0' ) {
        KillTrap();
        InitTrapError = TRUE;
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
    mx_entry                            in[2];
    mx_entry                            out[1];
    get_supplementary_service_req       acc;
    get_supplementary_service_ret       ret;

    acc.req = REQ_GET_SUPPLEMENTARY_SERVICE;
    in[0].ptr = &acc;
    in[0].len = sizeof( acc );
    in[1].ptr = name;
    in[1].len = strlen( name ) + 1;
    out[0].ptr = &ret;
    out[0].len = sizeof( ret );
    TrapAccess( 2, in, 1, out );
    if( ret.err != 0 ) return( 0 );
    return( ret.id );
}


void RemoteSuspend( void )
{
    suspend_req         acc;

    acc.req = REQ_SUSPEND;
    TrapSimpAccess( sizeof( acc ), &acc, 0, NULL );
}

void RemoteResume( void )
{
    resume_req          acc;

    acc.req = REQ_RESUME;
    TrapSimpAccess( sizeof( acc ), &acc, 0, NULL );
}

void RemoteErrMsg( sys_error err, char *msg )
{
    get_err_text_req    acc;

    acc.req = REQ_GET_ERR_TEXT;
    acc.err = err;
    TrapSimpAccess( sizeof( acc ), &acc, MAX_ERR_MSG_SIZE, msg );
//    TrapErrTranslate( msg, MAX_ERR_MSG_SIZE );
}

void FiniTrap( void )
{
    disconnect_req      in_mx;

    in_mx.req = REQ_DISCONNECT;
    TrapSimpAccess( sizeof( in_mx ), &in_mx, 0, NULL );
    RestoreHandlers();
    KillTrap();
    GrabHandlers();
#if !defined( BUILD_RFX )
    FiniSuppServices();
#endif
#ifdef ENABLE_TRAP_LOGGING
    CloseTrapTraceFile();
#endif
}

#if 0
bool ReInitTrap( char *trap_file )
/********************************/
{
    // only tested under NT - this is here for Lexus/Fusion/What's my name?
    FiniTrap();
    InitTrap( trap_file );
    return( !InitTrapError );
}
#endif
