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
* Description:  Spawn external commands.
*
****************************************************************************/


#include "vi.h"
#include <malloc.h>
#include "fcbmem.h"
#define INCL_DOSPROCESS
#define INCL_DOSERRORS
#include <os2.h>

char _NEAR * _NEAR ExeExtensions[] = { ".cmd", ".exe" };
int ExeExtensionCount = sizeof( ExeExtensions ) / sizeof( char _NEAR * );

char _NEAR * _NEAR InternalCommands[] = {
    "BREAK",
    "CALL",
    "CD",
    "CHCP",
    "CHDIR",
    "CLS",
    "COPY",
    "DATE",
    "DEL",
    "DETACH",
    "DIR",
    "DPATH",
    "ECHO",
    "ENDLOCAL",
    "ERASE",
    "EXIT",
    "EXTPROC",
    "FOR",
    "GOTO",
    "IF",
    "MD",
    "MKDIR",
    "PATH",
    "PAUSE",
    "PROMPT",
    "RD",
    "REM",
    "REN",
    "RENAME",
    "RMDIR",
    "SET",
    "SETLOCAL",
    "SHIFT",
    "START",
    "TIME",
    "TYPE",
    "VER",
    "VERIFY",
    "VOL"
};
int InternalCommandCount = sizeof( InternalCommands ) / sizeof( char _NEAR * );

void ResetSpawnScreen( void )
{
}

int MySpawn( char *cmd )
{
    USHORT              rc;
    RESULTCODES         returncodes;
    char                path[_MAX_PATH];
    char                all[_MAX_PATH + 128];
    char                *dest, *src;
    cmd_struct          cmds;

    GetSpawnCommandLine( path, cmd, &cmds );
    cmds.cmd[cmds.len] = 0x00;

    src = path;
    dest = all;
    while( *dest++ = *src++ );
    src = cmds.cmd;
    while( *dest++ = *src++ );
    *dest = 0;

    rc = DosExecPgm( NULL, 0, EXEC_SYNC, all, NULL, &returncodes, path );
    if( rc != 0 ) {
        return( -1 );
    }
    return( returncodes.codeResult );
}
