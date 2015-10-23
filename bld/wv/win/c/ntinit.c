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


#include <stdio.h>
#include <process.h>
#include <stddef.h>
#include <stdlib.h>
#include <malloc.h>
#include <direct.h>
#include <stdarg.h>
#include "wwindows.h"
#include "dbgdefn.h"
#include "dbgdata.h"
#ifdef __GUI__
#include "dbgwind.h"
#endif
#include "dbgmem.h"
#include "autoenv.h"
#include "dbglit.h"
#include "strutil.h"
#include "dbgmain.h"

#ifdef __WATCOMC__
#include "clibint.h"
#endif
#include "clibext.h"

extern void     StartupErr( const char *err );
extern void     NewConsoleTitle( void );

extern volatile bool    BrkPending;
#ifdef __GUI__
extern a_window         *WndMain;
#endif

static char             *CmdData;

#ifdef __AXP__
    //NYI: temp until we can get all the unaligned stuff straightened out.
    #define ERR_MODE (SEM_FAILCRITICALERRORS|SEM_NOALIGNMENTFAULTEXCEPT)
#else
    #define ERR_MODE SEM_FAILCRITICALERRORS
#endif

void GUImain( void )
{
    char        *buff;
    int         len;

    // fix up env vars if necessary
    watcom_setup_env();

    SetErrorMode( ERR_MODE );
#if defined( _M_IX86 )
    _8087 = 0;
#endif
    len = _bgetcmd( NULL, 0 );
    _AllocA( buff, len + 1 );
    getcmd( buff );
    CmdData = buff;

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

void WndCleanUp( void )
{
}

char *GetCmdArg( int num )
{
    if( num != 0 ) return( NULL );
    return( CmdData );
}

void SetCmdArgStart( int num, char *ptr )
{
    num = num;
    CmdData = ptr;
}

void KillDebugger( int ret_code )
{
    ExitProcess( ret_code );
}

void GrabHandlers( void )
{
}

void RestoreHandlers( void )
{
}

long _fork( const char *cmd, size_t len )
{
    //NYI: check out WinExec
    cmd = cmd; len = len;
    return( 0 );
}

bool TBreak( void )
{
    bool    ret;

    ret = BrkPending;
    BrkPending = false;
    return( ret );
}

void PopErrBox( const char *buff )
{
    MessageBox( (HWND) NULL, buff, LIT_ENG( Debugger_Startup_Error ),
            MB_OK | MB_ICONHAND | MB_SYSTEMMODAL );
}
