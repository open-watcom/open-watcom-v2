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
#include <tlhelp32.h>
#include <process.h>
#include "drwatcom.h"
#include <ctype.h>
#include "mem.h"

#define INIT_ALLOCSIZE  20
#define RE_ALLOCSIZE    10
/*
 * getName
 */
void getName( DWORD proc_id, DWORD mod_id, char **name )
{
    HANDLE              hdl;
    BOOL                noerror;
    MODULEENTRY32       modinfo;

    memset( &modinfo, 0, sizeof( MODULEENTRY32 ) );
    modinfo.dwSize =  sizeof( MODULEENTRY32 );
    hdl = CreateToolhelp32Snapshot( TH32CS_SNAPMODULE, proc_id );
    noerror = Module32First( hdl, &modinfo );
    while( noerror ) {
        if( modinfo.th32ModuleID == mod_id ) break;
        noerror = Module32Next( hdl, &modinfo );
    }
    if( noerror ) {
        *name = modinfo.szModule;
    } else {
        *name = NULL;
    }
    CloseHandle( hdl );
}

/*
 * GetNextThread -
 * NB - callers must continue to call this function until it returns FALSE
 */
BOOL GetNextThread( ThreadList *info, ThreadPlace *place,
                    DWORD pid, BOOL first )
{
    BOOL                        noerror;

    noerror = FALSE;
    if( first ) {
        place->thrddata = MemAlloc( sizeof( THREADENTRY32 ) );
        memset( place->thrddata, 0, sizeof( THREADENTRY32 ) );
        place->thrddata->dwSize =  sizeof( THREADENTRY32 );
        place->hdl = CreateToolhelp32Snapshot( TH32CS_SNAPTHREAD, 0 );
        noerror = Thread32First( place->hdl, place->thrddata );
        if( noerror ) {
            place->pid = pid;
        }
    } else {
        noerror = Thread32Next( place->hdl, place->thrddata );
    }
    while( noerror ) {
        if( pid == place->thrddata->th32OwnerProcessID ) break;
        noerror = Thread32Next( place->hdl, place->thrddata );
    }
    if( noerror ) {
        info->tid = place->thrddata->th32ThreadID;
        info->priority = place->thrddata->tpBasePri
                         + place->thrddata->tpDeltaPri;
    }
    if( !noerror ) {
        CloseHandle( place->hdl );
        MemFree( place->thrddata );
    }
    return( noerror );
}

/*
 * GetNextProcess
 * NB - callers must continue to call this function until it returns FALSE
 */
BOOL GetNextProcess( ProcList *info, ProcPlace *place, BOOL first ) {

    BOOL        noerror;
    char        *tmpname;

    tmpname = MemAlloc( MAX_PROC_NAME );
    noerror = TRUE;
    if( first ) {
        place->procdata = MemAlloc( sizeof( PROCESSENTRY32 ) );
        memset( place->procdata, 0, sizeof( PROCESSENTRY32 ) );
        place->procdata->dwSize =  sizeof( PROCESSENTRY32 );
        place->hdl = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );
        noerror = Process32First( place->hdl, place->procdata );
    } else {
        noerror = Process32Next( place->hdl, place->procdata );
    }
    if( noerror ) {
        info->pid = place->procdata->th32ProcessID;
        info->priority = place->procdata->pcPriClassBase;
        getName( place->procdata->th32ProcessID,
                       place->procdata->th32ModuleID, &tmpname );
        strcpy( info->name, tmpname );
        if( info->name == NULL ) {
            noerror = FALSE;
        }
    }
    if( !noerror ) {
        CloseHandle( place->hdl );
        MemFree( place->procdata );
    }
    MemFree( tmpname );
    return( noerror );
}

/*
 * GetThreadInfo
 */
BOOL GetThreadInfo( DWORD pid, DWORD tid, ThreadStats *info ) {

    BOOL                        noerror;
    HANDLE                      hdl;
    THREADENTRY32               thrddata;

    noerror = FALSE;
    memset( &thrddata, 0, sizeof( THREADENTRY32 ) );
    thrddata.dwSize =  sizeof( THREADENTRY32 );
    hdl = CreateToolhelp32Snapshot( TH32CS_SNAPTHREAD, 0 );
    noerror = Thread32First( hdl, &thrddata );
    while( noerror ) {
        if( tid == thrddata.th32ThreadID ) {
            if( pid == thrddata.th32OwnerProcessID ) {
                break;
            }
        }
        noerror = Thread32Next( hdl, &thrddata );
    }
    if( noerror ) {
        info->tid = tid;
        info->pid = pid;
        info->base_pri = thrddata.tpBasePri;
        info->cur_pri = thrddata.tpBasePri + thrddata.tpDeltaPri;
        info->state = -1;
        info->wait_reason = -1;
    }
    CloseHandle( hdl );
    return( noerror );
}

/*
 * GetProcessInfo
 */
BOOL GetProcessInfo( DWORD pid, ProcStats *info ) {

    BOOL                noerror;
    HANDLE              hdl;
    PROCESSENTRY32      procinfo;
    char                *tmpname;

    tmpname = MemAlloc( MAX_PROC_NAME );
    noerror = FALSE;
    memset( &procinfo, 0, sizeof( PROCESSENTRY32 ) );
    procinfo.dwSize = sizeof( PROCESSENTRY32 );
    hdl = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );
    noerror = Process32First( hdl, &procinfo );
    while( noerror ) {
        if( pid == procinfo.th32ProcessID ) break;
        noerror = Process32Next( hdl, &procinfo );
    }
    if( noerror && info != NULL ) {
        info->pid = procinfo.th32ProcessID;
        info->priority = procinfo.pcPriClassBase;
        getName( procinfo.th32ProcessID, procinfo.th32ModuleID, &tmpname );
        if( tmpname != NULL ){
            strcpy( info->name, tmpname );
        } else {
            noerror = FALSE;
        }
    }
    MemFree( tmpname );
    CloseHandle( hdl );
    return( noerror );
}

/*
 * FreeModuleList
 */
void FreeModuleList( char **ptr, DWORD cnt ) {

    DWORD       i;

    if( ptr == NULL ) return;
    for( i=0; i < cnt; i++ ) {
        MemFree( ptr[i] );
    }
    MemFree( ptr );
}

/*
 * GetModuleList
 */
char **GetModuleList( DWORD pid, DWORD *cnt ) {

    DWORD               allocsize;
    char                **ret;
    BOOL                noerror;
    BOOL                memerr;
    HANDLE              hdl;
    MODULEENTRY32       modinfo;

    noerror = TRUE;
    memerr = FALSE;
    ret = NULL;
    allocsize = INIT_ALLOCSIZE;
    memset( &modinfo, 0, sizeof( MODULEENTRY32 ) );
    modinfo.dwSize = sizeof( MODULEENTRY32 );
    hdl = CreateToolhelp32Snapshot( TH32CS_SNAPMODULE, pid );
    ret = MemAlloc( allocsize * sizeof( char * ) );
    if( ret == NULL ) {
        memerr = TRUE;
    } else {
        memset( ret, 0, allocsize * sizeof( char * ) );
        noerror = Module32First( hdl, &modinfo );
    }
    for( *cnt = 0; noerror && !memerr; *cnt += 1 ) {
        if( *cnt == allocsize ) {
            allocsize += RE_ALLOCSIZE;
            ret = MemReAlloc( ret, allocsize * sizeof( char * ) );
            if( ret == NULL ) {
                memerr = FALSE;
                break;
            }
            memset( ret + ( allocsize - RE_ALLOCSIZE ), 0,
                            RE_ALLOCSIZE * sizeof( char* ) );
        }
        ret[*cnt] = MemAlloc( strlen( modinfo.szModule ) + 1 );
        strcpy( ret[*cnt], modinfo.szModule );
        noerror = Module32Next( hdl, &modinfo);
    }
    if( memerr ) {
        *cnt = 0;
        FreeModuleList(ret, allocsize);
        ret = NULL;
    }
    if( ret == NULL ) *cnt = 0;
    CloseHandle( hdl );
    return( ret );
}

/*
 * RefreshCostlyInfo
 */
void RefreshCostlyInfo( void ) {

}

/*
 * RefreshInfo
 */
BOOL RefreshInfo( void ) {
    return( TRUE );
}

void InitReg( void ) {

}
