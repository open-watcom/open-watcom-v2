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
#include <process.h>
#include "drwatcom.h"
#include "menu.h"

static DWORD    *noGetList;
static DWORD    noGetUsedSize;
static DWORD    noGetAllocSize;

#define NO_ATTATCH_ALLOC_INCREMENT      30
#define REFRESH_INTERVAL                1000

static int pidComp( const void *p1, const void *p2 ) {
    return( *(DWORD *)p1 - *(DWORD *)p2 );
}

/*
 * dontAttatch
 */
static BOOL dontAttatch( DWORD pid ) {

    void        *node;

    node = bsearch( &pid, noGetList, noGetUsedSize, sizeof( DWORD ),
                    pidComp );
    return( node != NULL );
}

/*
 * addDontAttatch
 */
static void addDontAttatch( DWORD pid ) {

    DWORD               i;

    for( i=0; i < noGetUsedSize; i++ ) {
        if( noGetList[i] > pid ) break;
        if( noGetList[i] == pid ) return;
    }
    noGetUsedSize ++;
    if( noGetUsedSize >= noGetAllocSize ) {
        noGetAllocSize += NO_ATTATCH_ALLOC_INCREMENT;
        noGetList = MemReAlloc( noGetList, noGetAllocSize * sizeof( DWORD ) );
    }
    if( i < noGetUsedSize - 1 ) {
        memmove( noGetList + i + 1, noGetList + i,
                    ( noGetUsedSize - i - 1 ) * sizeof( DWORD ) );
    }
    noGetList[ i ] = pid;
}

/*
 * ErrAdding
 */
void ErrAdding( void *_info ) {

    ProcAttatchInfo   *info = _info;
#ifdef DEBUG
    char        buf[100];

    sprintf( buf, "Unable to Auto add process %08lX", info->info.pid );
    MessageBox( MainHwnd, buf, AppName,
                MB_OK | MB_ICONEXCLAMATION | MB_SETFOREGROUND );
#endif
    addDontAttatch( info->info.pid );
}

#define MAX_NEW_PROC    20
/*
 * doAutoAttatch
 */
static void doAutoAttatch( void ) {

    ProcList            info;
    ProcPlace           place;
    BOOL                rc;
    DWORD               newpid[ MAX_NEW_PROC ];
    DWORD               pidcnt;

    pidcnt = 0;
    rc = GetNextProcess( &info, &place, TRUE );
    while( rc ) {
        if( FindProcess( info.pid ) == NULL ) {
            if( !dontAttatch( info.pid ) ) {
                newpid[ pidcnt ] = info.pid;
                pidcnt++;
                if( pidcnt == MAX_NEW_PROC ) {
                    /* we need to finish the walk in order to leave
                       semaphores in the right state */
                    while( GetNextProcess( &info, &place, FALSE ) );
                    break;
                }
            }
        }
        rc = GetNextProcess( &info, &place, FALSE );
    }
    if( pidcnt > 0 ) {
        for( ;; ) {
            pidcnt --;
            CallProcCtl( MENU_ADD_RUNNING, newpid + pidcnt, ErrAdding );
            if( pidcnt == 0 ) break;
        }
    }
}

static void removeDeadProcs( void ) {

    DWORD       i;

    for( i = 0; i < noGetUsedSize; i++ ) {
        if( !GetProcessInfo( noGetList[i], NULL ) ) {
            memmove( noGetList + i, noGetList + i + 1,
                        ( noGetUsedSize - i - 1 ) * sizeof( DWORD ) );
            i--;
            noGetUsedSize--;
        }
    }
}

//DWORD WINAPI AutoAttatchMain( void *dum ) {
void AutoAttatchMain( void *dum ) {

    HANDLE      hdl;

    dum = dum;
    hdl = GetCurrentThread();
    SetThreadPriority( hdl, THREAD_PRIORITY_IDLE );
    for( ;; ) {
        if( ConfigData.auto_attatch ) {
            if( RefreshInfo() ) {
                removeDeadProcs();
                doAutoAttatch();
            }
        }
        Sleep( REFRESH_INTERVAL );
    }
}

/*
 * InitAutoAttatch
 */
void InitAutoAttatch( void ) {

    ProcList            info;
    ProcPlace           place;
    BOOL                rc;
//    DWORD             threadid;

    RefreshInfo();
    rc = GetNextProcess( &info, &place, TRUE );
    while( rc ) {
        addDontAttatch( info.pid );
        rc = GetNextProcess( &info, &place, FALSE );
    }
//    CreateThread( NULL, 0, AutoAttatchMain, 0, 0, &threadid );
    _beginthread( AutoAttatchMain, 0, 0 );
}

