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
* Description:  Process Toolhelp notifications.
*
****************************************************************************/


#include <stddef.h>
#include <stdio.h>
#include <dos.h>
#include "wdebug.h"
#include "stdwin.h"
#include "dbgrmsg.h"
#include "initfini.h"


#ifdef DEBUG
char *notify_msgs[] = {
    "NFY_UNKNOWN",
    "NFY_LOADSEG",
    "NFY_FREESEG",
    "NFY_STARTDLL",
    "NFY_STARTTASK",
    "NFY_EXITTASK",
    "NFY_DELMODULE",
    "NFY_RIP",
    "NFY_TASKIN",
    "NFY_TASKOUT",
    "NFY_INCHAR",
    "NFY_OUTSTR",
    "NFY_LOGERROR",
    "NFY_LOGPARAMERROR"
};
#endif

static HTASK    TaskAtNotify;

/*
 * doLoadSeg:
 *
 * handle NFY_LOADSEG notification
 *
 * If we get a segment load, then we make sure that
 * any breakpoints we may have set in the segment are re-planted
 */
static bool doLoadSeg( DWORD data )
{
    NFYLOADSEG  *ls;

    ls = (NFYLOADSEG *)data;
    if( DebuggerState != LOADING_DEBUGEE ) {
        ResetBreakpoints( ls->wSelector );
    }
    return( false );

} /* doLoadSeg */

/*
 * doStartTask:
 *
 * handle NFY_STARTTASK notification
 *
 * If we are waiting for the debuggee to load, then this is the task.
 * We remember the module and task ID's, and plant a breakpoint at the
 * starting address of the application.  We can do that now, since we
 * now know the code selector for the segment with the start address.
 *
 * Otherwise, we simply record it so that we can notify the debugger
 * about a loaded module later.
 */
static bool doStartTask( DWORD data )
{
    TASKENTRY   te;

    te.dwSize = sizeof( te );
    TaskFindHandle( &te, TaskAtNotify );

    if( DebuggerState == LOADING_DEBUGEE ) {
        DebugeeModule = te.hModule;
        DebugeeTask = TaskAtNotify;
        StopNewTask.addr.segment = HIWORD( data );
//        StopNewTask.addr.offset = LOWORD( data );
        StopNewTask.old_opcode = place_breakpoint( &StopNewTask.addr );
        Out((OUT_RUN,"           wrote breakpoint at %04x:%04lx, oldbyte=%02x(is now %02x)",
                    StopNewTask.addr.segment, StopNewTask.addr.offset, StopNewTask.old_opcode, BreakOpcode ));
        Out((OUT_RUN,"   StartTask: cs:ip = %Fp", data ));
        ToDebugger( TASK_LOADED );
    } else {
        AddModuleLoaded( te.hModule, false );
    }
    return( false );

} /* doStartTask */

/*
 * doStartDLL:
 *
 * handle a NFY_STARTDLL notification
 *
 * Record the module to notify the debugger of it later.
 */
static bool doStartDLL( DWORD data )
{
    NFYSTARTDLL *sd;

    sd = (NFYSTARTDLL *)data;
    AddModuleLoaded( sd->hModule, true );
    if( DebuggerState == RUNNING_DEBUGEE ) {
        DLLLoad.addr.segment = sd->wCS;
        DLLLoad.addr.offset = sd->wIP;
        DLLLoad.old_opcode = place_breakpoint( &DLLLoad.addr );
    }
    Out((OUT_ERR,"DLL Loaded '%4.4x:%4.4x'",sd->wCS,sd->wIP));
    return( false );

} /* doStartDLL */


/*
 * doOutStr:
 *
 * handle a NFY_OUTSTR notification
 *
 * We return to the debugger so that it can display the string for the
 * user to see.
 */
static bool doOutStr( DWORD data )
{
    char        *src;

    if( DebuggerState != RUNNING_DEBUGEE ) {
        Out((OUT_ERR,"Debugger was bad! '%s'",OutBuff));
        return( false );
    }
    for( src = (LPSTR)data; *src != '\0'; src++ ) {
        if( *src == '\r' || OutPos == (MAX_STR-1) ) {
            OutBuff[OutPos++] = '\0';
            Out((OUT_RUN,"Going to debugger for OUT_STR '%s'",OutBuff));
            if( DebugeeTask == NULL )
                return( false );
            ToDebugger( OUT_STR );
        }
        if( *src != '\n' && *src != '\t' ) {
            OutBuff[OutPos++] = *src;
        }
    }
    return( false );
} /* doOutStr */

/*
 * doInChar:
 *
 * handle a NFY_INCHAR notification.
 *
 * the kernel wants a character (typically after a fatal exit), so
 * we flip back to the debugger to get a character from the user.
 */
static char doInChar( void )
{

    if( DebuggerState != RUNNING_DEBUGEE )
        return( 'i' ); // cover our ass!
    Out((OUT_RUN,"Going to debugger for GET_CHAR"));
    return( 'b' );
//  ToDebugger( GET_CHAR );
//  return( IntResult.EAX );

} /* doInChar */

/*
 * doExitTask:
 *
 * handle NFY_EXITTASK notification
 *
 * If the task was the debuggee, we notify the debugger that the
 * debuggee has terminated.
 */
static bool doExitTask( DWORD data )
{
    data = data;

    if( TaskAtNotify == DebugeeTask ) {
        TerminateCSIP = TaskGetCSIP( DebugeeTask );
        PostAppMessage( DebuggerTask, WM_NULL, TASK_ENDED, MAGIC_COOKIE );
    }
    return( false );

} /* doExitTask */

/*
 * NotifyHandler - recieves notifications from toolhelp and processes them
 */
BOOL FAR PASCAL NotifyHandler( WORD id, DWORD data )
{
    bool        rc;

    TaskAtNotify = GetCurrentTask();
    if( id == NFY_LOADSEG || id == NFY_LOGERROR || id == NFY_FREESEG ||
                id == NFY_LOGPARAMERROR ) {
        return( false );
    }
    if( id == NFY_OUTSTR ) {
        Out((OUT_RUN,"%s(%d): Task=%04x, data=%08lx",
                notify_msgs[id], id, TaskAtNotify, data ));
        Out((OUT_RUN,"        \"%Fs\"", data ));
    } else {
        Out((OUT_RUN,"%s(%d): Task=%04x, data=%08lx",
                notify_msgs[id], id, TaskAtNotify, data));
    }

    switch( id ) {
    case NFY_OUTSTR:
        rc = doOutStr( data );
        break;
    case NFY_INCHAR:
        return( doInChar() );   // return character
    case NFY_STARTTASK:
        rc = doStartTask( data );
        break;
    case NFY_EXITTASK:
        rc = doExitTask( data );
        break;
    case NFY_STARTDLL:
        rc = doStartDLL( data );
        break;
    case NFY_LOADSEG:
        rc = doLoadSeg( data );
        break;
    default:
        rc = false;
        break;
    }
    return( rc );

} /* NotifyHandler */
