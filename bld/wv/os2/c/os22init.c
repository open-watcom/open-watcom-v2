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
* Description:  Mainline for OS/2 32-bit debugger.
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
#include <os2.h>
#include "dbgdefn.h"
#include "dbgdata.h"
#include "autoenv.h"
#include "strutil.h"
#include "dbgmain.h"
#include "envlkup.h"
#include "dbginit.h"


char            *CmdData;

static volatile bool    BrkPending;

#if 0
static void __pascal __far BrkHandler( USHORT sig_arg, USHORT sig_num )
{
    PFNSIGHANDLER   prev_hdl;
    USHORT          prev_act;

    sig_arg = sig_arg;
    BrkPending = true;
    DosSetSigHandler( BrkHandler, &prev_hdl, &prev_act, 4, sig_num );
}
#endif

#include <stdio.h>
void GUImain( void )
{
    char    *buff;
    int     len;

    // fix up env vars if necessary
    watcom_setup_env();

    len = _bgetcmd( NULL, INT_MAX ) + 1;
    buff = malloc( len );
    CmdData = buff;
    getcmd( CmdData );
    //TODO: replace with exception handler
//    DosSetSigHandler( BrkHandler, &prev_hdl, &prev_act, 2, SIG_CTRLBREAK );
    DebugMain();
    free( buff );
}


int GUISysInit( int param )
{
    param = param;
    return 1;
}

void GUISysFini( void )
{
    DebugFini();
}

void WndCleanUp( void )
{
}

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
}

void GrabHandlers( void )
{
}

void RestoreHandlers( void )
{
}

unsigned EnvLkup( const char *name, char *buff, unsigned buff_len )
{
    const char  *env;
    unsigned    len;
    int         output = 0;
    char        c;

    // use getenv() so that autoenv has an effect (we can't
    // reliably modify the "master" process environment on OS/2)
    env = getenv( name );
    if( env == NULL )
        return( 0 );
    if( buff_len != 0 && buff != NULL ) {
        --buff_len;
        output = 1;
    }
    for( len = 0; (c = *env++) != '\0'; ++len ) {
        if( output ) {
            if( len >= buff_len ) {
                break;
            }
            *buff++ = c;
        }
    }
    if( output ) {
        buff[len] = '\0';
    }
    return( len );
}

long _fork( const char *cmd, size_t len )
{
    char        *dst;
    char        *args;
    unsigned    cmd_len;
    RESULTCODES res;
    APIRET      rc;
    HFILE       savestdin;
    HFILE       savestdout;
    HFILE       console;
    HFILE       new;
    ULONG       act;
    char        buff[CCHMAXPATH];

    cmd_len = EnvLkup( "COMSPEC", buff, sizeof( buff ) );
    if( cmd_len == 0 )
        return( ERROR_FILE_NOT_FOUND );
    while( len != 0 && *cmd == ' ' ) {
        ++cmd;
        --len;
    }
    args = buff + cmd_len + 1;
    dst = StrCopy( buff, args ) + 1;
    if( len != 0 ) {
        dst = StrCopy( "/C ", dst );
        memcpy( dst, cmd, len );
        dst += len;
        *dst++ = '\0';
    }
    *dst = '\0';

    savestdin  = 0xffff;
    savestdout = 0xffff;
    DosDupHandle( 0, &savestdin );
    DosDupHandle( 1, &savestdout );
    if (DosOpen( "CON", &console, &act, 0, 0, 0x11, 0x42, 0) == 0 ) {
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
