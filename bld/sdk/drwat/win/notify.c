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
#include <ctype.h>
#include "drwatcom.h"

typedef struct {
    HANDLE      task;
    DWORD       data;
    union {
        NFYSTARTDLL     startdll;
        NFYRIP          rip;
    };
    MODULEENTRY me;
    TASKENTRY   te;
} notify;

#define MAX_STR         512
#define MAX_ARRAYS      8

static char             outBuff[MAX_STR];
static int              outPos;

/*
 * getNotifyData - get notify data area
 */
static notify *getNotifyData( DWORD data )
{
    static int          currData;
    static notify       notifyData[MAX_ARRAYS];
    notify              *ptr;

    ptr = &notifyData[ currData ];
    currData = (currData+1) % MAX_ARRAYS;
    ptr->task = GetCurrentTask();
    ptr->data = data;
    return( ptr );

} /* getNotifyData */

/*
 * NotifyHandler - call back routine for notifications
 */
BOOL __export FAR PASCAL NotifyHandler( WORD id, DWORD data )
{
    BOOL        rc;
    LPSTR       src;
    notify      *ptr;

    ptr = getNotifyData( data );

    switch( id ) {
    case NFY_OUTSTR:
        src = (LPSTR) data;
        while( *src ) {
            if( outPos < MAX_STR-1 ) {
                outBuff[ outPos++ ] = *src;
            }
            src++;
        }
        break;
    case NFY_STARTDLL:
        memcpy( &ptr->startdll, (LPVOID) data, sizeof( NFYSTARTDLL ) );
        MyModuleFindHandle( &ptr->me, ptr->startdll.hModule );
        break;
    case NFY_DELMODULE:
        MyModuleFindHandle( &ptr->me, (HMODULE)data );
        break;
    case NFY_RIP:
        memcpy( &ptr->rip, (LPVOID) data, sizeof( NFYRIP ) );
        MyTaskFindHandle( &ptr->te, ptr->task );
        MyModuleFindHandle( &ptr->me, ptr->te.hModule );
        break;
    case NFY_STARTTASK:
        MyTaskFindHandle( &ptr->te, ptr->task );
        MyModuleFindHandle( &ptr->me, ptr->te.hModule );
        break;
    }
    PostMessage( MainWindow, WM_USER, id, (DWORD) ptr );
    rc = 0;
    if( id == NFY_INCHAR ) {
        rc = 'i';
    }
    if( WDebug386 && !AlwaysRespondToDebugInChar ) {
        if( IsDebuggerExecuting != NULL && IsDebuggerExecuting() ) {
            rc = 0;
        }
    }
    return( rc );

} /* NotifyHandler */

/*
 * HandleNotify - handle a notification...
 */
void HandleNotify( WORD wparam, DWORD lparam )
{
    char                str[ MAX_STR ];
    char                tmpstr[ MAX_STR ];
    char                *src,*dest;
    notify              *ptr;

    ptr = (notify *) lparam;

    switch( wparam ) {
    case NFY_UNKNOWN:
        break;
    case NFY_LOADSEG:
        break;
    case NFY_FREESEG:
        break;

    case NFY_STARTTASK:
        LBPrintf( ListBox, STR_TASK_START, (WORD)ptr->task, (DWORD)ptr->data );
        LBPrintf( ListBox, STR_TASK_START_2, ptr->me.szModule, ptr->me.szExePath );
        if( DumpDialogHwnd != NULL ) {
            PostMessage( DumpDialogHwnd, WM_COMMAND, TASKCTL_REDRAW, 0L );
        }
        break;

    case NFY_EXITTASK:
        LBPrintf( ListBox, STR_TASK_EXIT, (WORD)ptr->task, (WORD)ptr->data );
        if( DumpDialogHwnd != NULL ) {
            PostMessage( DumpDialogHwnd, WM_COMMAND, TASKCTL_REDRAW, 0L );
        }
        break;

    case NFY_STARTDLL:
        LBPrintf( ListBox, STR_DLL_LOAD, (WORD)ptr->startdll.hModule,
                  ptr->startdll.wCS, ptr->startdll.wIP );
        LBPrintf( ListBox, STR_DLL_LOAD_2, ptr->me.szModule, ptr->me.szExePath );
        break;

    case NFY_DELMODULE:
        LBPrintf( ListBox, STR_MODULE_DEL, (WORD)ptr->data,
                  ptr->me.szModule, ptr->me.szExePath );
        break;

    case NFY_RIP:
        LBPrintf( ListBox, STR_RIP, (WORD)ptr->task, ptr->me.szModule,
                  ptr->rip.wExitCode, ptr->rip.wCS, ptr->rip.wIP );
        Alert();
        break;

    case NFY_INCHAR:
        break;
    case NFY_OUTSTR:
        if( outPos == 0 ) {
            break;
        }
        memcpy( str, outBuff, outPos );
        str[ outPos ] = 0;
        outPos = 0;
        src = str;
        dest = tmpstr;
        while( 1 ) {
            if( *src == '\r' || *src == 0 ) {
                *dest = 0;
                dest = tmpstr;
                if( *dest != 0 ) {
                    LBPrintf( ListBox, STR_DEBUG_OUT, dest );
                }
                if( *src == 0 ) {
                    break;
                }
            } else if( isprint( *src ) ) {
                *dest++ = *src;
            }
            src++;
        }
        if( !AlertOnWarnings ) {
            if( outBuff[0] == 'w' && outBuff[1] == 'n' && outBuff[2] == ' ' ) {
                break;
            }
        }
        Alert();
        break;
    }

} /* HandleNotify */
