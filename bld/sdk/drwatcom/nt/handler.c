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


#include <windows.h>
#include <stdio.h>
#include "drwatcom.h"
#include "srchmsg.h"
#include "mem.h"

#ifndef STATUS_SEGMENT_NOTIFICATION
#define STATUS_SEGMENT_NOTIFICATION 0x40000005
#endif
#define _X86_
#include "vdmdbg.h"

#define EVENT_LEN       15

static char     headerBuf[80];


/*
 * makeHeader
 */
#define EVENT_BUFSIZE   30
static void makeHeader( DWORD eventid, DEBUG_EVENT *dbinfo ) {

    ProcStats           stats;
    char                event[ EVENT_BUFSIZE ];

    CopyRCString( eventid, event, EVENT_BUFSIZE );
    while( !GetProcessInfo( dbinfo->dwProcessId, &stats ) ) {
        Sleep( 100 );
        RefreshInfo();
    }
    sprintf( headerBuf, "%-*s pid = %08lX (%s) tid = %08lX", EVENT_LEN, event,
                dbinfo->dwProcessId, stats.name, dbinfo->dwThreadId );
}

/*
 * procOutputDebugString
 */
static void procOutputDebugString( DEBUG_EVENT *dbinfo ) {

    char                        *buf;
    char                        *buf2;
    WORD                        *src;
    char                        *dst;
    HANDLE                      hp;
    OUTPUT_DEBUG_STRING_INFO    *info;

    Alert();
    info = &(dbinfo->u.DebugString);
    hp = OpenProcess( PROCESS_ALL_ACCESS, FALSE, dbinfo->dwProcessId );
    buf = MemAlloc( info->nDebugStringLength );
    ReadProcessMemory( hp, info->lpDebugStringData, buf,
                        info->nDebugStringLength, NULL );
    if( info->fUnicode ) {
        buf2 = MemAlloc( info->nDebugStringLength / 2 );
        dst = buf2;
        src = (WORD *)buf;
        while( *src != 0 ) {
            *dst = *src;
            dst ++;
            src ++;
        }
        MemFree( buf );
        buf = buf2;
    }
    makeHeader( STR_DEBUG_STRING, dbinfo );
    LBStrPrintf( MainLBox, headerBuf );
    LBStrPrintf( MainLBox, "                (%s)", buf );
    MemFree( buf );
}

/*
 * procException
 */
static DWORD procException( DEBUG_EVENT *dbinfo ) {

    DWORD       type;
    int         action;
    char        buf[100];
    DWORD       faultid;

    type = dbinfo->u.Exception.ExceptionRecord.ExceptionCode;
    if( type <= STATUS_VDM_EVENT || type == DBG_CONTROL_C ||
        type == DBG_CONTROL_BREAK || type == EXCEPTION_BREAKPOINT
        || type == EXCEPTION_SINGLE_STEP ) {
        return( DBG_CONTINUE );
    }

    // NYI - let the user decide if we catch errors on first or last chance
    //       notification

    if( !dbinfo->u.Exception.dwFirstChance ) {
        return( DBG_CONTINUE );
    }
    action = HandleException( dbinfo );
    FormatException( buf, dbinfo->u.Exception.ExceptionRecord.ExceptionCode );
    makeHeader( STR_EXCEPTION, dbinfo );
    LBStrPrintf( MainLBox, headerBuf );
    LBPrintf( MainLBox, STR_ERROR_OCCURRED_AT_X, EVENT_LEN, "", buf,
                dbinfo->u.Exception.ExceptionRecord.ExceptionAddress );
    if( type == EXCEPTION_ACCESS_VIOLATION ) {
        if( dbinfo->u.Exception.ExceptionRecord.ExceptionInformation[0] ) {
            faultid = STR_INV_WRITE_TO_X;
        } else {
            faultid = STR_INV_READ_FROM_X;
        }
        LBPrintf( MainLBox, faultid, EVENT_LEN, "",
            dbinfo->u.Exception.ExceptionRecord.ExceptionInformation[1] );
    }
    Alert();
    return( action );
}

static void procCreateProcess( DEBUG_EVENT *dbinfo ) {

    ProcStats           stats;
    char                *name;

    AddProcess( dbinfo->dwProcessId, dbinfo->u.CreateProcessInfo.hProcess,
                dbinfo->dwThreadId, dbinfo->u.CreateProcessInfo.hThread );
    makeHeader( STR_PROCESS_CREATED, dbinfo );
    GetProcessInfo( dbinfo->dwProcessId, &stats );
    name = MemAlloc( strlen( stats.name ) + 5 );
    sprintf( name, "%s.exe", stats.name );
    AddModule( dbinfo->dwProcessId, dbinfo->u.CreateProcessInfo.hFile,
                (DWORD)dbinfo->u.CreateProcessInfo.lpBaseOfImage, name );
    LBStrPrintf( MainLBox, headerBuf );
    LBPrintf( MainLBox, STR_PROC_CREATE_FMT_STR,
                EVENT_LEN, "", dbinfo->u.CreateProcessInfo.lpBaseOfImage,
                dbinfo->u.CreateProcessInfo.lpStartAddress );
}

/*
 * DebugEventHandler
 */
DWORD DebugEventHandler( DEBUG_EVENT *dbinfo ) {

    DWORD       ret;
    char        *name;
    ProcNode    *pnode;
    ModuleNode  *mnode;

    ret = DBG_CONTINUE;
    switch( dbinfo->dwDebugEventCode ) {
    case EXCEPTION_DEBUG_EVENT:
        ret = procException( dbinfo );
        break;
    case CREATE_THREAD_DEBUG_EVENT:
        AddThread( dbinfo->dwProcessId, dbinfo->dwThreadId,
                   dbinfo->u.CreateThread.hThread );
        makeHeader( STR_THREAD_CREATED, dbinfo );
        LBPrintf( MainLBox, STR_THRD_CREATE_FMT_STR, headerBuf,
                dbinfo->u.CreateThread.lpStartAddress );
        break;
    case CREATE_PROCESS_DEBUG_EVENT:
        procCreateProcess( dbinfo );
        break;
    case EXIT_THREAD_DEBUG_EVENT:
        RemoveThread( dbinfo->dwProcessId, dbinfo->dwThreadId );
        makeHeader( STR_THREAD_ENDED, dbinfo );
        LBPrintf( MainLBox, STR_THRD_ENDED_FMT_STR, headerBuf,
                   dbinfo->u.ExitThread.dwExitCode );
        break;
    case EXIT_PROCESS_DEBUG_EVENT:
        RemoveProcess( dbinfo->dwProcessId );
        makeHeader( STR_PROCESS_ENDED, dbinfo );
        LBPrintf( MainLBox, STR_PROC_ENDED_FMT_STR, headerBuf,
                    dbinfo->u.ExitProcess.dwExitCode );
        break;
    case LOAD_DLL_DEBUG_EVENT:
        makeHeader( STR_DLL_LOADED, dbinfo );
        name = GetModuleName( dbinfo->u.LoadDll.hFile );
        AddModule( dbinfo->dwProcessId, dbinfo->u.LoadDll.hFile,
                   (DWORD)dbinfo->u.LoadDll.lpBaseOfDll, name );
        if( name == NULL ) name = "???";
        LBStrPrintf( MainLBox, "%s", headerBuf );
        LBPrintf( MainLBox, STR_DLL_LOAD_FMT_STR,
                    EVENT_LEN, "", name, dbinfo->u.LoadDll.lpBaseOfDll );
        break;
    case UNLOAD_DLL_DEBUG_EVENT:
        makeHeader( STR_DLL_UNLOADED, dbinfo );
        pnode = FindProcess( dbinfo->dwProcessId );
        mnode = ModuleFromAddr( pnode, dbinfo->u.UnloadDll.lpBaseOfDll );
        if( mnode == NULL || mnode->name == NULL ) {
            LBPrintf( MainLBox, STR_DLL_UNLOAD_FMT_STR1, headerBuf,
                    dbinfo->u.UnloadDll.lpBaseOfDll );
        } else {
            LBPrintf( MainLBox, STR_DLL_UNLOAD_FMT_STR2, headerBuf,
                      mnode->name );
        }
        RemoveModule( dbinfo->dwProcessId,
                      (DWORD)dbinfo->u.UnloadDll.lpBaseOfDll );
        break;
    case OUTPUT_DEBUG_STRING_EVENT:
        procOutputDebugString( dbinfo );
        break;
    case RIP_EVENT:
        makeHeader( STR_RIP, dbinfo );
        LBStrPrintf( MainLBox, headerBuf );
        Alert();
        break;
    }
    return( ret );
}
