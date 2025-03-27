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
* Description:  Mainline for OS/2 debugger.
*
****************************************************************************/


#include <stddef.h>
#include <stdlib.h>
#include <limits.h>
#include <process.h>
#define INCL_DOSMISC
#define INCL_DOSSIGNALS
#define INCL_DOSPROCESS
#define INCL_DOSERRORS
#include <wos2.h>
#include "dbgdefn.h"
#include "dbgdata.h"
#include "autoenv.h"
#include "strutil.h"
#include "dbgmain.h"
#include "envlkup.h"
#include "dbginit.h"
#include "dbgcmdln.h"
#ifndef __NOUI__
#include "aui.h"
#endif


char            *CmdData;

static volatile bool    BrkPending;

#ifndef __NOUI__

#ifdef _M_I86

static void __pascal __far BrkHandler( USHORT sig_arg, USHORT sig_num )
{
    PFNSIGHANDLER   prev_hdl;
    USHORT          prev_act;

    /* unused parameters */ (void)sig_arg;

    BrkPending = true;
    DosSetSigHandler( BrkHandler, &prev_hdl, &prev_act, 4, sig_num );
}

void GUImain( void )
{
    char                cmd_line[256];
    PFNSIGHANDLER       prev_hdl;
    USHORT              prev_act;

    DosSetMaxFH( 40 );
    _bgetcmd( cmd_line, sizeof( cmd_line ) );
    CmdData = cmd_line;
    DosSetSigHandler( BrkHandler, &prev_hdl, &prev_act, 2, SIG_CTRLBREAK );
    DebugMain();
}

#else

void GUImain( void )
{
    int     cmd_len;
    char    *cmd_line;

    // fix up env vars if necessary
    watcom_setup_env();

    cmd_len = _bgetcmd( NULL, 0 ) + 1;
    cmd_line = malloc( cmd_len );
    _bgetcmd( cmd_line, cmd_len );
    CmdData = cmd_line;
    //TODO: replace with exception handler
//    DosSetSigHandler( BrkHandler, &prev_hdl, &prev_act, 2, SIG_CTRLBREAK );
    DebugMain();
    free( cmd_line );
}

#endif


bool GUISysInit( init_mode install )
{
    /* unused parameters */ (void)install;

    return( true );
}

void GUISysFini( void )
{
    DebugFini();
}

void WndCleanUp( void )
{
}

#endif

char *GetCmdArg( int num )
{
    if( num != 0 || CmdData == NULL )
        return( NULL );

    return( CmdData );
}

void SetCmdArgStart( int num, char *ptr )
{
    num = num; /* must be zero */
    CmdData = ptr;
}

void KillDebugger( int ret_code )
{
    DosExit( EXIT_PROCESS, ret_code );
    // never return
}

void GrabHandlers( void )
{
}

void RestoreHandlers( void )
{
}

long _fork( const char *cmd, size_t len )
{
    char        *dst;
    char        *args;
    size_t      cmd_len;
    RESULTCODES res;
    APIRET      rc;
    HFILE       savestdin;
    HFILE       savestdout;
    HFILE       console;
    HFILE       new;
    OS_UINT     act;
    char        buff[CCHMAXPATH];

    cmd_len = EnvLkup( "COMSPEC", buff, sizeof( buff ) );
    if( cmd_len == 0 )
        return( ERROR_FILE_NOT_FOUND );
    while( len != 0 && *cmd == ' ' ) {
        ++cmd;
        --len;
    }
    args = buff + cmd_len + 1;
    dst = StrCopyDst( buff, args ) + 1;
    if( len != 0 ) {
        dst = StrCopyDst( "/C ", dst );
#ifdef _M_I86
        _fmemcpy( dst, cmd, len );
#else
        memcpy( dst, cmd, len );
#endif
        dst += len;
        *dst++ = NULLCHAR;
    }
    *dst = NULLCHAR;

    savestdin  = 0xffff;
    savestdout = 0xffff;
    DosDupHandle( 0, &savestdin );
    DosDupHandle( 1, &savestdout );
    if( DosOpen( "CON", &console, &act, 0,
            FILE_NORMAL,
            OPEN_ACTION_CREATE_IF_NEW | OPEN_ACTION_OPEN_IF_EXISTS,
            OPEN_SHARE_DENYNONE | OPEN_ACCESS_READWRITE,
            0 ) == 0 ) {
        new = 0;
        DosDupHandle( console, &new );
        new = 1;
        DosDupHandle( console, &new );
        DosClose( console );
    }

    rc = DosExecPgm( NULL, 0,           /* don't care about fail name */
                EXEC_SYNC,              /* execflags */
                args,                   /* args */
                NULL,                   /* inherit environment */
                &res,                   /* result codes */
                buff );                 /* pgmname */

    new = 0;
    DosDupHandle( savestdin, &new );
    DosClose( savestdin );
    new = 1;
    DosDupHandle( savestdout, &new );
    DosClose( savestdout );

    if( rc == 0 )
        rc = res.codeTerminate;

    return( rc );
}

bool TBreak( void )
{
    bool    ret;

    ret = BrkPending;
    BrkPending = false;
    return( ret );
}
