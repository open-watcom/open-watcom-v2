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


#include <ctype.h>
#include <stdio.h>
#include "bool.h"
#include "drwatcom.h"
#include "mem.h"
#include "menu.h"

#define PIPE_BUF_SIZE   1024

/*
 * AppDebuggerErrMsg
 */
static void AppDebuggerErrMsg( void *_info ) {

    ProcAttatchInfo *info = _info;
    char        buf[100];
    ProcStats   stats;

    if( GetProcessInfo( info->info.pid, &stats ) ) {
        RCsprintf( buf, STR_CANT_DEBUG_PROC_X_NAME, info->info.pid,
                   stats.name );
    } else {
        RCsprintf( buf, STR_CANT_DEBUG_PROC_X, info->info.pid );
    }
    MessageBox( MainHwnd, buf, AppName,
                MB_OK | MB_ICONEXCLAMATION | MB_SETFOREGROUND );
}

void ProcessCommandLine( char *cmdline ) {

    char                *ptr;
    char                *name;
    DWORD               pid;

    if( cmdline != NULL && *cmdline != '\0' ) {
        ptr = cmdline;
        while( isspace( *ptr ) ) ptr ++;
        if( *ptr == '-' || *ptr == '/' ) {
            ptr++;
            if( ptr == '\0' ) return;
            switch( *ptr ) {
                case 'p':
                    ptr++;
                    pid = atol( ptr );
                    CallProcCtl( MENU_ADD_RUNNING, &pid, AppDebuggerErrMsg );
                    break;
            }
        } else {
            name = MemAlloc( strlen( ptr ) );
            strcpy( name, ptr );
            CallProcCtl( MENU_NEW_TASK, name, NULL );
        }
    }
}
