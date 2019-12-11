/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2019 The Open Watcom Contributors. All Rights Reserved.
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


#include <string.h>
#include <stddef.h>
#include <stdlib.h>
#include <direct.h>
#include <stdarg.h>
#include <process.h>
#include "wwindows.h"
#include "dbgdefn.h"
#include "dbgwind.h"
#include "tinyio.h"
#include "dbgmem.h"
#include "autoenv.h"
#include "strutil.h"
#include "dbgmain.h"
#include "dbginit.h"
#include "dbgcmdln.h"
#include "trptypes.h"
#include "trpsys.h"
#include "wininit.h"
#include "guigsysh.h"

#include "clibint.h"


extern void     HookInQueue( void );
extern void     HookOutQueue( void );
extern HWND     MainHwnd = NULL;

volatile bool   BrkPending;
bool            ToldWinHandle = false;

static BOOL     PASCAL GetCommandData( HWND );

static char     *CmdData;

#ifndef __NOUI__

#if defined( GUI_IS_GUI )
void TellWinHandle( void )
{
    if( !ToldWinHandle && TRAP_EXTFUNC( InfoFunction )( GUIGetSysHandle( WndGui( WndMain ) ) ) ) {
        ToldWinHandle = true;
    }
}
#endif

void GUImain( void )
{
    char buff[256];

    // fix up env vars if necessary
    watcom_setup_env();

    SetErrorMode( SEM_FAILCRITICALERRORS );
    SetHandleCount( 60 );
    _8087 = 0;
    CmdData=buff;
    getcmd( CmdData );
    DebugMain();
    InitHookFunc();
}


bool GUISysInit( init_mode install )
{
    /* unused parameters */ (void)install;

    return( true );
}

void GUISysFini( void  )
{
    DebugFini();
}

void WndCleanUp( void )
{
    TRAP_EXTFUNC( InfoFunction )( (HWND)0 );
    FiniHookFunc();
}

#endif

char *GetCmdArg( int num )
{
    if( num != 0 )
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
    TinyExit( ret_code );
    // never return
}

void GrabHandlers( void )
{
    if( !ToldWinHandle ) {
        TRAP_EXTFUNC( InfoFunction )( (HWND)0 );
    }
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

#if 0
void __assert( int value, char *expr, char *fn, int line_num )
{
    value=value;expr=expr;fn=fn;line_num=line_num;
}

void exit( int code )
{
    KillDebugger( code );
}

void __exit_with_msg( const char *msg )
{
    PopErrBox( msg );
    exit( 1 );
}
#endif

void PopErrBox( const char *buff )
{
    MessageBox( (HWND)NULL, buff, LIT_ENG( Debugger_Startup_Error ),
            MB_OK | MB_ICONHAND | MB_SYSTEMMODAL );
}
