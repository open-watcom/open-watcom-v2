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
#include "sample.h"
#include "smpstuff.h"
#include "sampwin.h"

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


static HANDLE   ourTask = NULL;

#define MAX_STR         512
#define MAX_ARRAYS      8

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
    notify      *ptr;

    if( id == NFY_TASKIN || id == NFY_TASKOUT ) {
        if( GetCurrentTask() == ourTask ) {
            if( id == NFY_TASKIN ) {
                UnPauseSampler();
            } else {
                PauseSampler();
            }
        }
        return( FALSE );
    }
    PauseSampler();

    ptr = getNotifyData( data );

    switch( id ) {
    case NFY_STARTDLL:
        _fmemcpy( &ptr->startdll, (LPVOID) data, sizeof( NFYSTARTDLL ) );
        MyModuleFindHandle( &ptr->me, ptr->startdll.hModule );
        HandleLibLoad( SAMP_CODE_LOAD, ptr->startdll.hModule );
        break;
    case NFY_STARTTASK:
        if( ourTask == NULL ) { // handle spawned tasks
            ourTask = GetCurrentTask();
            MyTaskFindHandle( &ptr->te, ptr->task );
            MyModuleFindHandle( &ptr->me, ptr->te.hModule );
            HandleLibLoad( SAMP_MAIN_LOAD, ptr->te.hModule );
            StartSampler();
        } else {
            MyTaskFindHandle( &ptr->te, ptr->task );
            MyModuleFindHandle( &ptr->me, ptr->te.hModule );
            HandleLibLoad( SAMP_CODE_LOAD, ptr->te.hModule );
        }
        break;
    case NFY_EXITTASK:
        if( GetCurrentTask() == ourTask ) { // handle spawned tasks
            QuitSampler( &TotalTime );
            SharedMemory->TaskEnded = TRUE;
        }
        break;
    }
    UnPauseSampler();
    return( FALSE );

} /* NotifyHandler */

/*
 * HandleNotify - handle a notification...
 */
void HandleNotify( WORD wparam, DWORD lparam )
{
    notify              *ptr;

    ptr = (notify *) lparam;

    switch( wparam ) {
    case NFY_STARTTASK:
        break;

    case NFY_EXITTASK:
        break;

    case NFY_STARTDLL:
        break;
    }

} /* HandleNotify */
