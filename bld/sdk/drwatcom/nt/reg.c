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


#include <stdio.h>
#include <windows.h>
#include <process.h>
#include "drwatcom.h"
#include <ctype.h>
#include "mem.h"

#define PERFLIB_MAX     "software\\microsoft\\windows nt\\currentversion"\
                        "\\perflib"
#define PERFLIB_STRING  "software\\microsoft\\windows nt\\currentversion"\
                        "\\perflib\\009"

#define WRITE_SEM_NAME          "dr_nt_reg_write_mutex"
#define COSTLY_WRITE_SEM_NAME   "dr_nt_costly_reg_write_mutex"

#define INIT_BUF_SIZE   0x10000
#define BUF_SIZE_INCR   1024

#define N_IMAGE                 "Image"
#define N_THREAD                "Thread"
#define N_THREADID              "ID Thread"
#define N_PROCESS               "Process"
#define N_PROCID                "ID Process"
#define N_BASE_PRIORITY         "Priority Base"
#define N_CUR_PRIORITY          "Priority Current"
#define N_THREAD_STATE          "Thread State"
#define N_WAIT_REASON           "Thread Wait Reason"
#define N_PROCESS_ADDR_SPACE    "Process Address Space"

#define N_MAP_SPACE_NO_ACC      "Mapped Space No Access"
#define N_MAP_SPACE_READ        "Mapped Space Read Only"
#define N_MAP_SPACE_WRITE       "Mapped Space Read/Write"
#define N_MAP_SPACE_COPY        "Mapped Space Write Copy"
#define N_MAP_SPACE_EXEC        "Mapped Space Executable"
#define N_MAP_SPACE_EXECREAD    "Mapped Space Exec Read Only"
#define N_MAP_SPACE_EXECWRITE   "Mapped Space Exec Read/Write"
#define N_MAP_SPACE_EXECCOPY    "Mapped Space Exec Write Copy"

#define N_RES_SPACE_NO_ACC      "Reserved Space No Access"
#define N_RES_SPACE_READ        "Reserved Space Read Only"
#define N_RES_SPACE_WRITE       "Reserved Space Read/Write"
#define N_RES_SPACE_COPY        "Reserved Space Write Copy"
#define N_RES_SPACE_EXEC        "Reserved Space Executable"
#define N_RES_SPACE_EXECREAD    "Reserved Space Exec Read Only"
#define N_RES_SPACE_EXECWRITE   "Reserved Space Exec Read/Write"
#define N_RES_SPACE_EXECCOPY    "Reserved Space Exec Write Copy"

#define N_IMAGE_SPACE_NO_ACC    "Image Space No Access"
#define N_IMAGE_SPACE_READ      "Image Space Read Only"
#define N_IMAGE_SPACE_WRITE     "Image Space Read/Write"
#define N_IMAGE_SPACE_COPY      "Image Space Write Copy"
#define N_IMAGE_SPACE_EXEC      "Image Space Executable"
#define N_IMAGE_SPACE_EXECREAD  "Image Space Exec Read Only"
#define N_IMAGE_SPACE_EXECWRITE "Image Space Exec Read/Write"
#define N_IMAGE_SPACE_EXECCOPY  "Image Space Exec Write Copy"

#define N_NO_ACCESS             "No Access"
#define N_READ_ONLY             "Read Only"
#define N_READ_WRITE            "Read/Write"
#define N_WRITE_COPY            "Write Copy"
#define N_EXEC                  "Executable"
#define N_EXEC_READ             "Exec Read Only"
#define N_EXEC_WRITE            "Exec Read/Write"
#define N_EXEC_COPY             "Exec Write Copy"

static char                     *titleBuf;
static char                     **titleIndex;
static DWORD                    indexSize;
static PERF_DATA_BLOCK          *regData;
static PERF_DATA_BLOCK          *costlyData;
static PERF_OBJECT_TYPE         *processObject;
static PERF_OBJECT_TYPE         *threadObject;
static PERF_OBJECT_TYPE         *memoryObject;
static PERF_OBJECT_TYPE         *imageObject;
static PERF_OBJECT_TYPE         *costlyThreadObject;
static PERF_OBJECT_TYPE         *procAddrObject;

static CRITICAL_SECTION         readCntSection;
static DWORD                    readCounter;
static HANDLE                   writeMutex;

static CRITICAL_SECTION         costlyReadCntSection;
static DWORD                    costlyReadCounter;
static HANDLE                   costlyWriteMutex;

/*
 * getIndex - fill in the titleIndex array
 */
static DWORD genIndex( void ) {

    DWORD       rc;
    DWORD       type;
    DWORD       datasize;
    DWORD       item;
    char        *begin;
    HKEY        keyhdl;

    rc = RegOpenKeyEx( HKEY_LOCAL_MACHINE, PERFLIB_MAX , 0, KEY_READ,
                       &keyhdl );
    if( rc == ERROR_SUCCESS ) {
        datasize = sizeof( DWORD );
        rc = RegQueryValueEx( keyhdl, "Last Counter", NULL,
                              &type, (char *)&indexSize, &datasize );
    }
    indexSize++;
    if( rc == ERROR_SUCCESS ) {
        titleIndex = MemAlloc( indexSize * sizeof( char * ) );
        memset( titleIndex, 0, indexSize * sizeof( char * ) );
        begin = titleBuf;
        while( *begin != '\0' ) {
            item = atoi( begin );
#ifdef DEBUG
            if( item >= indexSize ) {
                MessageBox( NULL, "Writting past end of title array",
                            "reg.c", MB_OK );
            }
#endif
            while( *begin != '\0' ) begin++;
            begin ++;
            titleIndex[item] = begin;
            while( *begin != '\0' ) begin++;
            begin++;
        }
    }
    return( rc );
}

/*
 * getTitles - get the strings refrerenced by the performance registry
 */
static DWORD getTitles( void ) {

    DWORD       rc;
    DWORD       type;
    DWORD       datasize;
    HKEY        keyhdl;

    rc = RegOpenKeyEx( HKEY_LOCAL_MACHINE, PERFLIB_STRING, 0, KEY_READ,
                       &keyhdl );
    if( rc == ERROR_SUCCESS ) {
        rc = RegQueryValueEx( keyhdl, "Counters", NULL, &type, NULL, &datasize );
    }
    if( rc == ERROR_SUCCESS ) {
        titleBuf = MemAlloc( datasize );
        rc = RegQueryValueEx( keyhdl, "Counters", NULL, &type, titleBuf,
                              &datasize );
    }
    if( rc == ERROR_SUCCESS ) {
        rc = genIndex();
    }
    return( rc );
}

/*
 * findIndex - return the index of the given string
 */
static DWORD findIndex( char *str ) {

    DWORD       i;

    for( i=0; i < indexSize; i++ ) {
        if( titleIndex[i] != NULL ) {
            if( !stricmp( titleIndex[i], str ) ) {
                return( i );
            }
        }
    }
    return( -1 );
}

/*
 * getData - retrieve some data
 */
static DWORD getData( char *name, PERF_DATA_BLOCK **data ) {

    DWORD       datasize;
    DWORD       type;
    WORD        i;
    LONG        rc;

    for( i=1; ; i++ ) {
        datasize = INIT_BUF_SIZE + i * BUF_SIZE_INCR;
        *data = MemAlloc( datasize );
        rc = RegQueryValueEx( HKEY_PERFORMANCE_DATA, name, NULL, &type,
                              (void *)*data, &datasize );
        if( rc != ERROR_MORE_DATA ) {
            break;
        }
        MemFree( *data );
    }
    return( rc );
}

/*
 * findObject
 */
static PERF_OBJECT_TYPE *findObject( char *data, char *str ) {

    DWORD               index;
    PERF_OBJECT_TYPE    *obj;
    PERF_DATA_BLOCK     *dblock;
    DWORD               i;

    index = findIndex( str );
    if( index == -1 ) return( NULL );

    dblock = (PERF_DATA_BLOCK *)data;
    obj = (PERF_OBJECT_TYPE *) ( data + dblock->HeaderLength );
    for( i=0; i < dblock->NumObjectTypes; i++ ) {
        if( obj->ObjectNameTitleIndex == index ) {
            return( obj );
        }
        data = (char *)obj;
        obj = (PERF_OBJECT_TYPE *) ( data + obj->TotalByteLength );
    }
    return( NULL );
}

/*
 * findCounter
 */

static PERF_COUNTER_DEFINITION *findCounter( PERF_OBJECT_TYPE *obj, char *str )
{
    PERF_COUNTER_DEFINITION     *counter;
    DWORD                       i;
    DWORD                       index;

    index = findIndex( str );
    if( index == -1 ) return( NULL );

    counter = (PERF_COUNTER_DEFINITION *)
              ( (char *)obj + obj->HeaderLength );
    for( i=0; i < obj->NumCounters; i++ ) {
        if( counter->CounterNameTitleIndex == index ) {
            return( counter );
        }
        counter = (PERF_COUNTER_DEFINITION *)
                  ( (char *)counter + counter->ByteLength );
    }
    return( NULL );
}

/*
 * getCounterDWORD
 */
static DWORD getCounterDWORD( PERF_INSTANCE_DEFINITION *inst,
                              PERF_COUNTER_DEFINITION *counterinfo )
{
    DWORD       *ret;

    ret = (DWORD *)
          ( (char *)inst + inst->ByteLength + counterinfo->CounterOffset );
    return( *ret );
}

/*
 * getNextInstance - get the next instance
 *      NB. this does not check for reading past the end of the list
 */
static PERF_INSTANCE_DEFINITION *getNextInstance( PERF_INSTANCE_DEFINITION *inst )
{
    PERF_INSTANCE_DEFINITION    *ret;
    PERF_COUNTER_BLOCK          *cntblock;

    cntblock = (PERF_COUNTER_BLOCK *) ( (char *)inst + inst->ByteLength );
    ret = (PERF_INSTANCE_DEFINITION *)
               ( (char *)cntblock + cntblock->ByteLength );
    return( ret );
}

/*
 * getFirstInstance
 */
static PERF_INSTANCE_DEFINITION *getFirstInstance( PERF_OBJECT_TYPE *obj ) {

    PERF_INSTANCE_DEFINITION    *inst;

    if( obj->NumInstances == 0 ) return( NULL );
    inst = (PERF_INSTANCE_DEFINITION *)
           ( (char *)obj + obj->DefinitionLength );
    return( inst );
}

/*
 * initObj
 */
static void initObj( PERF_DATA_BLOCK **data, PERF_OBJECT_TYPE **obj,
                     char *objname )
{
    if( *obj != NULL ) return;
    *obj = findObject( (char *)*data, objname );
}

/*
 * beginRead
 */
static void beginRead( BOOL costly ) {
    HANDLE              mut;
    CRITICAL_SECTION    *cntsect;
    DWORD               *counter;

    if( costly ) {
        mut = costlyWriteMutex;
        cntsect = &costlyReadCntSection;
        counter = &costlyReadCounter;
    } else {
        mut = writeMutex;
        cntsect = &readCntSection;
        counter = &readCounter;
    }
    EnterCriticalSection( cntsect );
    if( *counter == 0 ) {
        WaitForSingleObject( mut, INFINITE );
    }
    *counter = *counter + 1;
    LeaveCriticalSection( cntsect );
}

/*
 * endRead
 */
static void endRead( BOOL costly ) {
    HANDLE              mut;
    CRITICAL_SECTION    *cntsect;
    DWORD               *counter;

    if( costly ) {
        mut = costlyWriteMutex;
        cntsect = &costlyReadCntSection;
        counter = &costlyReadCounter;
    } else {
        mut = writeMutex;
        cntsect = &readCntSection;
        counter = &readCounter;
    }
    EnterCriticalSection( cntsect );
    *counter = *counter - 1;
    if( *counter == 0 ) {
        ReleaseMutex( mut );
    }
    LeaveCriticalSection( cntsect );
}


/*
 * GetNextThread -
 * NB - callers must continue to call this function until it returns FALSE
 */
BOOL GetNextThread( ThreadList *info, ThreadPlace *place,
                    DWORD pid, BOOL first )
{

    DWORD                       curpid;
    PERF_COUNTER_DEFINITION     *counter;

    if( first ) {
        beginRead( FALSE );
        initObj( &regData, &threadObject, N_THREAD );
        place->index = 0;
        place->obj = threadObject;
        place->inst = getFirstInstance( threadObject );
        if( place->inst == NULL ) {
            endRead( FALSE );
            return( FALSE );
        }
        place->pid = pid;
    } else {
        place->index ++;
        if( place->index < place->obj->NumInstances ) {
            place->inst = getNextInstance( place->inst );
        }
    }
    counter = findCounter( place->obj, N_PROCID );
    for( ; place->index < place->obj->NumInstances; place->index ++ ) {
        curpid = getCounterDWORD( place->inst, counter );
        if( curpid == place->pid ) break;
        place->inst = getNextInstance( place->inst );
    }
    if( place->index >= place->obj->NumInstances ) {
        endRead( FALSE );
        return( FALSE );
    }
    counter = findCounter( place->obj, N_THREADID );
    info->tid = getCounterDWORD( place->inst, counter );
    counter = findCounter( place->obj, N_BASE_PRIORITY );
    info->priority = getCounterDWORD( place->inst, counter );
    return( TRUE );
}

/*
 * GetNextProcess
 * NB - callers must continue to call this function until it returns FALSE
 */
BOOL GetNextProcess( ProcList *info, ProcPlace *place, BOOL first ) {

    PERF_COUNTER_DEFINITION     *counter;

    if( first ) {
        beginRead( FALSE );
        initObj( &regData, &processObject, N_PROCESS );
        place->index = 0;
        place->obj = processObject;
        place->inst = getFirstInstance( processObject );
        if( place->inst == NULL ) {
            endRead( FALSE );
            return( FALSE );
        }
    } else {
        place->index ++;
        if( place->index >= processObject->NumInstances ) {
            endRead( FALSE );
            return( FALSE );
        }
        place->inst = getNextInstance( place->inst );
    }
    counter = findCounter( place->obj, N_PROCID );
    info->pid = getCounterDWORD( place->inst, counter );
    counter = findCounter( place->obj, N_BASE_PRIORITY );
    info->priority = getCounterDWORD( place->inst, counter );
    wsprintf( info->name, "%ls",
              (char *)( place->inst ) + place->inst->NameOffset );
    return( TRUE );
}

/*
 * GetThreadInfo
 */
BOOL GetThreadInfo( DWORD pid, DWORD tid, ThreadStats *info ) {

    PERF_COUNTER_DEFINITION     *pid_counter;
    PERF_COUNTER_DEFINITION     *tid_counter;
    PERF_COUNTER_DEFINITION     *counter;
    PERF_INSTANCE_DEFINITION    *inst;
    DWORD                       curid;
    DWORD                       i;
    BOOL                        ret;

    beginRead( FALSE );
    initObj( &regData, &threadObject, N_THREAD );
    pid_counter = findCounter( threadObject, N_PROCID );
    tid_counter = findCounter( threadObject, N_THREADID );
    inst = getFirstInstance( threadObject );
    for( i=0; i < threadObject->NumInstances; i++ ) {
        curid = getCounterDWORD( inst, tid_counter );
        if( curid == tid ) {
            curid = getCounterDWORD( inst, pid_counter );
            if( curid == pid ) break;
        }
        inst = getNextInstance( inst );
    }
    if( i == threadObject->NumInstances ) {
        ret = FALSE;
    } else {
        ret = TRUE;
        info->tid = tid;
        info->pid = pid;
        counter = findCounter( threadObject, N_BASE_PRIORITY );
        info->base_pri = getCounterDWORD( inst, counter );
        counter = findCounter( threadObject, N_CUR_PRIORITY );
        info->cur_pri = getCounterDWORD( inst, counter );
        counter = findCounter( threadObject, N_THREAD_STATE );
        info->state = getCounterDWORD( inst, counter );
        counter = findCounter( threadObject, N_WAIT_REASON );
        info->wait_reason = getCounterDWORD( inst, counter );
    }
    endRead( FALSE );
    return( ret );
}

/*
 * GetProcessInfo
 */
BOOL GetProcessInfo( DWORD pid, ProcStats *info ) {

    DWORD                       i;
    PERF_COUNTER_DEFINITION     *counter;
    PERF_INSTANCE_DEFINITION    *inst;
    DWORD                       curpid;


    beginRead( FALSE );
    initObj( &regData, &processObject, N_PROCESS );
    counter = findCounter( processObject, N_PROCID );
    inst = getFirstInstance( processObject );
    for( i=0; i < processObject->NumInstances; i++ ) {
        curpid = getCounterDWORD( inst, counter );
        if( curpid == pid ) break;
        inst = getNextInstance( inst );
    }
    if( curpid == pid && info != NULL ) {
        info->pid = curpid;
        counter = findCounter( processObject, N_BASE_PRIORITY );
        info->priority = getCounterDWORD( inst, counter );
        wsprintf( info->name, "%ls",
                  (char *)inst + inst->NameOffset );
    }
    endRead( FALSE );
    return( curpid == pid );
}

/*
 * FreeModuleList
 */
void FreeModuleList( char **ptr, DWORD cnt ) {

    DWORD       i;

    for( i=0; i < cnt; i++ ) {
        MemFree( ptr[i] );
    }
    MemFree( ptr );
}

/*
 * getProcessIndex
 */
BOOL getProcessIndex( DWORD pid, DWORD *indexout ) {

    DWORD                       curpid;
    PERF_COUNTER_DEFINITION     *counter;
    PERF_INSTANCE_DEFINITION    *inst;
    DWORD                       index;

    counter = findCounter( processObject, N_PROCID );
    inst = getFirstInstance( processObject );
    for( index=0; index < processObject->NumInstances; index++ ) {
        curpid = getCounterDWORD( inst, counter );
        if( curpid == pid ) {
            *indexout = index;
            return( TRUE );
        }
        inst = getNextInstance( inst );
    }
    return( FALSE );
}

/*
 * GetModuleList
 */
char **GetModuleList( DWORD pid, DWORD *cnt ) {

    DWORD                       allocsize;
    DWORD                       index;
    DWORD                       i;
    char                        **ret;
    PERF_INSTANCE_DEFINITION    *inst;

    beginRead( FALSE );
    beginRead( TRUE );
    initObj( &costlyData, &imageObject, N_IMAGE );
    initObj( &regData, &processObject, N_PROCESS );
    allocsize = 20;
    if( !getProcessIndex( pid, &index ) ) {
        ret = NULL;
    } else {
        ret = MemAlloc( allocsize * sizeof( char * ) );
        inst = getFirstInstance( imageObject );
        *cnt = 0;
        for( i=0; i < imageObject->NumInstances; i += 1 ) {
            if( inst->ParentObjectInstance == index ) {
                ret[*cnt] = MemAlloc( inst->NameLength / 2 );
                wsprintf( ret[*cnt], "%ls", (char *)inst + inst->NameOffset );
                *cnt += 1;
                if( *cnt == allocsize ) {
                    allocsize += 10;
                    ret = MemReAlloc( ret, allocsize * sizeof( char * ) );
                }
            }
            inst = getNextInstance( inst );
        }
    }
    endRead( FALSE );
    endRead( TRUE );
    if( *cnt == 0 && ret != NULL ) {
        MemFree( ret );
        ret = NULL;
    }
    return( ret );
}

/*
 * GetImageMemInfo
 */
BOOL GetImageMemInfo( DWORD procid, char *imagename, MemByType *imageinfo ) {

    DWORD                       index;
    DWORD                       i;
    BOOL                        ret;
    char                        buf[ _MAX_PATH ];
    PERF_COUNTER_DEFINITION     *counter;
    PERF_INSTANCE_DEFINITION    *inst;

    ret = FALSE;
    beginRead( FALSE );
    beginRead( TRUE );
    initObj( &costlyData, &imageObject, N_IMAGE );
    initObj( &regData, &processObject, N_PROCESS );
    if( getProcessIndex( procid, &index ) ) {
        inst = getFirstInstance( imageObject );
        for( i=0; i < imageObject->NumInstances; i += 1 ) {
            if( inst->ParentObjectInstance == index ) {
                wsprintf( buf, "%ls", (char *)inst + inst->NameOffset );
                if( !strcmp( buf, imagename ) ) {
                    counter = findCounter( imageObject, N_NO_ACCESS );
                    imageinfo->noaccess = getCounterDWORD( inst, counter );
                    counter = findCounter( imageObject, N_READ_ONLY );
                    imageinfo->read = getCounterDWORD( inst, counter );
                    counter = findCounter( imageObject, N_READ_WRITE );
                    imageinfo->write = getCounterDWORD( inst, counter );
                    counter = findCounter( imageObject, N_WRITE_COPY );
                    imageinfo->copy = getCounterDWORD( inst, counter );
                    counter = findCounter( imageObject, N_EXEC );
                    imageinfo->exec = getCounterDWORD( inst, counter );
                    counter = findCounter( imageObject, N_EXEC_READ );
                    imageinfo->execread = getCounterDWORD( inst, counter );
                    counter = findCounter( imageObject, N_EXEC_WRITE );
                    imageinfo->execwrite = getCounterDWORD( inst, counter );
                    counter = findCounter( imageObject, N_EXEC_COPY );
                    imageinfo->execcopy = getCounterDWORD( inst, counter );
                    imageinfo->tot = imageinfo->noaccess + imageinfo->read
                                + imageinfo->write + imageinfo->copy
                                + imageinfo->exec + imageinfo->execread
                                + imageinfo->execwrite + imageinfo->execcopy;
                    ret = TRUE;
                    break;
                }
            }
            inst = getNextInstance( inst );
        }
    }
    endRead( TRUE );
    endRead( FALSE );
    return( ret);
}

/*
 * GetMemInfo
 */
BOOL GetMemInfo( DWORD procid, MemInfo *info ) {

    PERF_COUNTER_DEFINITION     *counter;
    PERF_INSTANCE_DEFINITION    *inst;
    DWORD                       curpid;
    DWORD                       i;

    beginRead( TRUE );
    initObj( &costlyData, &procAddrObject, N_PROCESS_ADDR_SPACE );
    info->modlist = GetModuleList( procid, &info->modcnt );
    if( info->modlist == NULL ) {
        endRead( TRUE );
        return( FALSE );
    }

    counter = findCounter( procAddrObject, N_PROCID );
    inst = getFirstInstance( procAddrObject );
    for( i=0; i < procAddrObject->NumInstances; i++ ) {
        curpid = getCounterDWORD( inst, counter );
        if( curpid == procid ) break;
        inst = getNextInstance( inst );
    }
    if( i == procAddrObject->NumInstances ) {
        FreeModuleList( info->modlist, info->modcnt );
        endRead( TRUE );
        return( FALSE );
    }
    counter = findCounter( procAddrObject, N_MAP_SPACE_NO_ACC );
    info->mapped.noaccess = getCounterDWORD( inst, counter );

    counter = findCounter( procAddrObject, N_MAP_SPACE_READ );
    info->mapped.read = getCounterDWORD( inst, counter );

    counter = findCounter( procAddrObject, N_MAP_SPACE_WRITE );
    info->mapped.write = getCounterDWORD( inst, counter );

    counter = findCounter( procAddrObject, N_MAP_SPACE_COPY );
    info->mapped.copy = getCounterDWORD( inst, counter );

    counter = findCounter( procAddrObject, N_MAP_SPACE_EXEC );
    info->mapped.exec = getCounterDWORD( inst, counter );

    counter = findCounter( procAddrObject, N_MAP_SPACE_EXECREAD );
    info->mapped.execread = getCounterDWORD( inst, counter );

    counter = findCounter( procAddrObject, N_MAP_SPACE_EXECWRITE );
    info->mapped.execwrite = getCounterDWORD( inst, counter );

    counter = findCounter( procAddrObject, N_MAP_SPACE_EXECCOPY );
    info->mapped.execcopy = getCounterDWORD( inst, counter );

    info->mapped.tot = info->mapped.noaccess + info->mapped.read
                        + info->mapped.write + info->mapped.copy
                        + info->mapped.exec + info->mapped.execread
                        + info->mapped.execwrite + info->mapped.execcopy;


    counter = findCounter( procAddrObject, N_RES_SPACE_NO_ACC );
    info->res.noaccess = getCounterDWORD( inst, counter );

    counter = findCounter( procAddrObject, N_RES_SPACE_READ );
    info->res.read = getCounterDWORD( inst, counter );

    counter = findCounter( procAddrObject, N_RES_SPACE_WRITE );
    info->res.write = getCounterDWORD( inst, counter );

    counter = findCounter( procAddrObject, N_RES_SPACE_COPY );
    info->res.copy = getCounterDWORD( inst, counter );

    counter = findCounter( procAddrObject, N_RES_SPACE_EXEC );
    info->res.exec = getCounterDWORD( inst, counter );

    counter = findCounter( procAddrObject, N_RES_SPACE_EXECREAD );
    info->res.execread = getCounterDWORD( inst, counter );

    counter = findCounter( procAddrObject, N_RES_SPACE_EXECWRITE );
    info->res.execwrite = getCounterDWORD( inst, counter );

    counter = findCounter( procAddrObject, N_RES_SPACE_EXECCOPY );
    info->res.execcopy = getCounterDWORD( inst, counter );

    info->res.tot = info->res.noaccess + info->res.read
                    + info->res.write + info->res.copy
                    + info->res.exec + info->res.execread
                    + info->res.execwrite + info->res.execcopy;


    counter = findCounter( procAddrObject, N_IMAGE_SPACE_NO_ACC );
    info->image.noaccess = getCounterDWORD( inst, counter );

    counter = findCounter( procAddrObject, N_IMAGE_SPACE_READ );
    info->image.read = getCounterDWORD( inst, counter );

    counter = findCounter( procAddrObject, N_IMAGE_SPACE_WRITE );
    info->image.write = getCounterDWORD( inst, counter );

    counter = findCounter( procAddrObject, N_IMAGE_SPACE_COPY );
    info->image.copy = getCounterDWORD( inst, counter );

    counter = findCounter( procAddrObject, N_IMAGE_SPACE_EXEC );
    info->image.exec = getCounterDWORD( inst, counter );

    counter = findCounter( procAddrObject, N_IMAGE_SPACE_EXECREAD );
    info->image.execread = getCounterDWORD( inst, counter );

    counter = findCounter( procAddrObject, N_IMAGE_SPACE_EXECWRITE );
    info->image.execwrite = getCounterDWORD( inst, counter );

    counter = findCounter( procAddrObject, N_IMAGE_SPACE_EXECCOPY );
    info->image.execcopy = getCounterDWORD( inst, counter );

    info->image.tot = info->image.noaccess + info->image.read
                    + info->image.write + info->image.copy
                    + info->image.exec + info->image.execread
                    + info->image.execwrite + info->image.execcopy;

    endRead( TRUE );
    return( TRUE );
}

/*
 * DoCostlyRefresh
 */
//DWORD WINAPI DoCostlyRefresh( void *dum ) {
void DoCostlyRefresh( void *dum ) {

    dum = dum;
    WaitForSingleObject( costlyWriteMutex, INFINITE );
    MemFree( costlyData );
    memoryObject = NULL;
    imageObject = NULL;
    costlyThreadObject = NULL;
    if( getData( "Costly", &costlyData ) != ERROR_SUCCESS ) {
        costlyData = NULL;
    }
    ReleaseMutex( costlyWriteMutex );
//    return( 0 );
}

/*
 * RefreshCostlyInfo -
 */
BOOL RefreshCostlyInfo( void ) {

    _beginthread( DoCostlyRefresh, NULL, 0, NULL );
    Sleep( 0 );         /* make sure the new thread gets the semaphore */
    return( TRUE );
}

/*
 * RefreshInfo - refresh all but costly info
 */
BOOL RefreshInfo( void ) {

    WaitForSingleObject( writeMutex, INFINITE );
    MemFree( titleIndex );
    MemFree( titleBuf );
    MemFree( regData );
    processObject = NULL;
    threadObject = NULL;
    getTitles();
    if( getData( "Global", &regData ) != ERROR_SUCCESS ) {
        regData = NULL;
    }
    ReleaseMutex( writeMutex );
    return( TRUE );
}

void InitReg( void ) {
    InitializeCriticalSection( &readCntSection );
    InitializeCriticalSection( &costlyReadCntSection );
    writeMutex = CreateMutex( NULL, FALSE, WRITE_SEM_NAME );
    costlyWriteMutex = CreateMutex( NULL, FALSE, COSTLY_WRITE_SEM_NAME );
    RefreshInfo();
    RefreshCostlyInfo();
}
