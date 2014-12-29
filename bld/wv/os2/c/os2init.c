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


#include <stddef.h>
#include <stdlib.h>
#include <process.h>
#define INCL_DOSMISC
#define INCL_DOSSIGNALS
#define INCL_DOSPROCESS
#define INCL_DOSERRORS
#include <os2.h>
#include "dbgdefn.h"
#include "dbgdata.h"
#include "strutil.h"

extern void     DebugMain( void );
extern void     DebugFini( void );
extern unsigned EnvLkup( const char *name, char *buff, unsigned buff_len );

static char             *cmdStart;
static volatile bool    BrkPending;

static void __pascal __far BrkHandler( USHORT sig_arg, USHORT sig_num )
{
    PFNSIGHANDLER   prev_hdl;
    USHORT          prev_act;

    sig_arg = sig_arg;
    BrkPending = TRUE;
    DosSetSigHandler( BrkHandler, &prev_hdl, &prev_act, 4, sig_num );
}


void GUImain( void )
{
    char                buff[256];
    PFNSIGHANDLER       prev_hdl;
    USHORT              prev_act;

    DosSetMaxFH( 40 );
    cmdStart = buff;
    getcmd( cmdStart );
    DosSetSigHandler( BrkHandler, &prev_hdl, &prev_act, 2, SIG_CTRLBREAK );
    DebugMain();
}


int GUISysInit( int param )
{
    param=param;
    return( 1 );
}

void GUISysFini( void  )
{
    DebugFini();
}

void WndCleanUp()
{
}

char *GetCmdArg( int num )
{
    if( num != 0 || cmdStart == NULL )
        return( NULL );
    return( cmdStart );
}

void SetCmdArgStart( int num, char *ptr )
{
    num = num; /* must be zero */
    cmdStart = ptr;
}

void KillDebugger( int ret_code )
{
    DosExit( EXIT_PROCESS, ret_code );
}

void GrabHandlers()
{
}

void RestoreHandlers()
{
}

long _fork( char *cmd, size_t len )
{
    char        *dst;
    char        *args;
    unsigned    cmd_len;
    RESULTCODES res;
    USHORT      rc;
    HFILE       savestdin;
    HFILE       savestdout;
    HFILE       console;
    HFILE       new;
    USHORT      act;
    char        buff[ 256 ];

    cmd_len = EnvLkup( "COMSPEC", buff, sizeof( buff ) );
    if( cmd_len == 0 ) return( ERROR_FILE_NOT_FOUND );
    while( len != 0 && *cmd == ' ' ) {
        ++cmd;
        --len;
    }
    args = buff + cmd_len + 1;
    dst = StrCopy( buff, args ) + 1;
    if( len != 0 ) {
        dst = StrCopy( "/C ", dst );
        _fmemcpy( dst, cmd, len );
        dst += len;
        *dst++ = '\0';
    }
    *dst = '\0';

    savestdin = 0xffff;
    savestdout = 0xffff;
    DosDupHandle( 0, &savestdin );
    DosDupHandle( 1, &savestdout );
    if( DosOpen( "CON", &console, &act, 0, 0, 0x11, 0x42, 0 ) == 0 ) {
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

    if( rc == 0 ) rc = res.codeTerminate;
    return( rc );
}

bool TBreak()
{
    bool    ret;

    ret = BrkPending;
    BrkPending = 0;
    return( ret );
}

int _set_errno( int a )
{
    return( a );
}
void SysSetMemLimit()
{
}
