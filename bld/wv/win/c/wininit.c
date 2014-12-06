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


#include <string.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <malloc.h>
#include <direct.h>
#include <stdarg.h>
#include <process.h>
#include "wwindows.h"
#include "dbgdefn.h"
#include "dbgwind.h"
#ifndef __NT__
#include "tinyio.h"
#endif
#include "dbgmem.h"
#include "autoenv.h"

#include "clibint.h"

extern void     DebugMain( void );
extern void     DebugFini( void );
extern char     *StrCopy( char *, char * );
extern void     HookInQueue();
extern void     HookOutQueue();
static BOOL     PASCAL GetCommandData( HWND );
extern HWND     GUIGetSysHandle(gui_window*);
extern void     InitHookFunc(void);
extern void     FiniHookFunc(void);

static char             *CmdData;
extern volatile int             BrkPending;
extern a_window         *WndMain;
bool                    ToldWinHandle = FALSE;

extern HWND     MainHwnd = NULL;

#ifndef __NT__
extern void  (WINAPI *InfoFunction)(HWND);
#endif

void TellWinHandle()
{
    if( !ToldWinHandle && InfoFunction != NULL ) {
        InfoFunction( GUIGetSysHandle( WndGui( WndMain ) ) );
        ToldWinHandle = TRUE;
    }
}

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
    InfoFunction( (HWND)0 );
    FiniHookFunc();
}

char *GetCmdArg( int num )
{
    if( num != 0 ) return( NULL );
    return( CmdData );
}

void SetCmdArgStart( int num, char *ptr )
{
    num = num; /* must be zero */
    CmdData = ptr;
}

void KillDebugger( int ret_code )
{
#ifdef __NT__
    ret_code = ret_code;
    ExitProcess( 0 );
#else
    TinyExit( ret_code );
#endif
}

void GrabHandlers()
{
    if( !ToldWinHandle && InfoFunction != NULL ) {
        InfoFunction( (HWND)0 );
    }
}


void RestoreHandlers()
{
}

long _fork( char *cmd, size_t len )
{
    //NYI: check out WinExec
    cmd = cmd; len = len;
    return( 0 );
}

bool TBreak()
{
    bool    ret;

    ret = BrkPending;
    BrkPending = 0;
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

void __exit_with_msg( char *msg )
{
    PopErrBox( msg );
    exit( 1 );
}
#endif

void PopErrBox( char *buff )
{
    MessageBox( (HWND) NULL, buff, LIT( Debugger_Startup_Error ),
            MB_OK | MB_ICONHAND | MB_SYSTEMMODAL );
}

void SysSetMemLimit()
{
}
