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
* Description:  Win32 performance sampling core.
*
****************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <ctype.h>
#include <string.h>
#include <i86.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "sample.h"
#include "wmsg.h"
#include "smpstuff.h"
#include <windows.h>
#include "exepe.h"
#include "exedos.h"


#define EVENT_NAME      "tickevent"
#define BUFF_SIZE       2048
#define STACK_SIZE      4096
#define CONTEXT_TO_USE  CONTEXT_CONTROL | CONTEXT_INTEGER | CONTEXT_SEGMENTS \
                        | CONTEXT_FLOATING_POINT

typedef struct {
    char        live;
    DWORD       id;
    HANDLE      th;
    unsigned    SampleIndex;
    DWORD       index;
    unsigned    SampleCount;
    samp_block  *Samples;
    samp_block  *CallGraph;
} thread_info;

static char             utilBuff[BUFF_SIZE];
static ULONG            sleepTime;
static far_address      commonAddr;
static thread_info      *threadInfo;
static int              threadCount;
static DEBUG_EVENT      debugEvent;
static HANDLE           processHandle;
static DWORD            taskPid;
static BOOL             doneSample;
static BOOL             timeOut;

/*
 * getThreadInfo - get info about a specific thread id
 */
static thread_info *getThreadInfo( DWORD id )
{
    int i;

    for( i = 0; i < threadCount; i++ ) {
        if( id == threadInfo[i].id ) {
            return( &threadInfo[i] );
        }
    }
    return( NULL );

} /* getThreadInfo */

/*
 * NextThread - set up so that we are at the next thread info
 */
unsigned NextThread( unsigned tid )
{
    if( tid == threadCount ) {
        return( 0 );
    }
    Samples = threadInfo[tid].Samples;
    SampleIndex = threadInfo[tid].SampleIndex;
    if( CallGraphMode ) {
        CallGraph = threadInfo[tid].CallGraph;
        SampleCount = threadInfo[tid].SampleCount;
    }
    return( tid + 1 );

} /* NextThread */

void ResetThread( unsigned tid )
{
    if( tid-- > 0 ) {
        SampleIndex = 0;
        threadInfo[tid].SampleIndex = SampleIndex;
        if( CallGraphMode ) {
            SampleCount = 0;
            LastSampleIndex = 0;
            threadInfo[tid].SampleCount = SampleCount;
        }
    }
}

void InitTimerRate( void )
{
    sleepTime = 55;
}

void SetTimerRate( const char **cmd )
{
    sleepTime = GetNumber( 1, 1000, cmd, 10 );
}

unsigned long TimerRate( void )
{
    return( 1000UL * sleepTime );
}

unsigned SafeMargin( void )
{
    return( Ceiling - 10 );
}

bool VersionCheck( void )
{
    return( true );
}

/*
 * RecordSample - record a sample in a specific thread
 */
static void RecordSample( unsigned offset, unsigned short segment, DWORD real_tid )
{
    samp_block  *old_samples;
    unsigned    old_sample_index;
    unsigned    old_sample_count;
    thread_info *ti;
    DWORD       tid;

    ti = getThreadInfo( real_tid );
    if( ti == NULL ) {
        return;
    }
    tid = ti->index;

    LastSampleIndex = ti->SampleIndex;
    if( ti->SampleIndex == 0 ) {
        ti->Samples->pref.tick = CurrTick;
        if( CallGraphMode ) {
            ti->CallGraph->pref.tick = CurrTick;
        }
    }
    ++CurrTick;
    ti->Samples->d.sample.sample[ti->SampleIndex].offset = offset;
    ti->Samples->d.sample.sample[ti->SampleIndex].segment = segment;
    ti->SampleIndex++;
    if( CallGraphMode ) {
        ti->SampleCount++;
    }
    if( CallGraphMode && tid == 0 ) {
        old_sample_count = SampleCount;
        old_samples = Samples;          /* since RecordCGraph isn't */
        old_sample_index = SampleIndex; /* used to threads, we fool */
        Samples = ti->Samples;          /* it into storing the info */
        SampleIndex = ti->SampleIndex;  /* in the right place by    */
        SampleCount = ti->SampleCount;
        RecordCGraph();                 /* changing its pointers    */
        ti->Samples = Samples;          /* and restoring them later */
        ti->SampleIndex = SampleIndex;
        ti->SampleCount = SampleCount;
        Samples = old_samples;
        SampleIndex = old_sample_index;
        SampleCount = old_sample_count;
    }
    if( ti->SampleIndex >= Margin ) {
        StopAndSave();
    }

} /* RecordSample */

void GetCommArea( void )
{
    DWORD       len;

    if( commonAddr.segment == 0 ) {     /* can't get the common region yet */
        Comm.cgraph_top = 0;
        Comm.top_ip = 0;
        Comm.top_cs = 0;
        Comm.pop_no = 0;
        Comm.push_no = 0;
        Comm.in_hook = 1;       /* don't record this sample */
    } else {
        ReadProcessMemory( processHandle, (LPVOID)commonAddr.offset, &Comm, sizeof( Comm ), &len );
    }
}

void ResetCommArea( void )
{
    DWORD       len;

    if( commonAddr.segment != 0 ) {
        Comm.pop_no = 0;
        Comm.push_no = 0;
        WriteProcessMemory( processHandle, (LPVOID)( commonAddr.offset + 11 ), &Comm.pop_no, 4, &len );
    }
}

void GetNextAddr( void )
{
    struct {
        unsigned long   ptr;
        seg             cs;
        off             ip;
    } stack_entry;
    DWORD       len;

    if( commonAddr.segment == 0 ) {
        CGraphOff = 0;
        CGraphSeg = 0;
    } else {
        ReadProcessMemory( processHandle, (LPVOID)Comm.cgraph_top, &stack_entry, sizeof( stack_entry ), &len );
        CGraphOff = stack_entry.ip;
        CGraphSeg = stack_entry.cs;
        Comm.cgraph_top = stack_entry.ptr;
    }
}

void StopProg( void ) {}

#if 0
static void internalError( char *str )
{
    OutputMsgParmNL( MSG_SAMPLE_2, str );
    _exit( -1 );
}
#endif

static void internalErrorMsg( int msg )
{
    OutputMsgParmNL( MSG_SAMPLE_2, GET_MESSAGE( msg ) );
    _exit( -1 );
}

/*
 * seekRead - seek to a specified spot in the file, and read some data
 */
static bool seekRead( HANDLE handle, DWORD newpos, void *buff, WORD size )
{
    int         rc;
    DWORD       bytes;

    if( SetFilePointer( handle, newpos, 0, SEEK_SET ) == INVALID_SET_FILE_POINTER ) {
        return( false );
    }
    rc = ReadFile( handle, buff, size, &bytes, NULL );
    if( !rc ) {
        return( false );
    }
    if( bytes != size ) {
        return( false );
    }
    return( true );

} /* seekRead */

/*
 * getPEHeader - get the header of the .EXE
 */
static bool getPEHeader( HANDLE handle, pe_exe_header *pehdr )
{
    unsigned_16         data;
    unsigned_32         ne_header_off;
    DWORD               bytes;

    if( !seekRead( handle, 0, &data, sizeof( data ) )
      || data != EXESIGN_DOS ) {
        return( false );
    }
    if( !seekRead( handle, DOS_RELOC_OFFSET, &data, sizeof( data ) )
      || !NE_HEADER_FOLLOWS( data ) ) {
        return( false );
    }
    if( !seekRead( handle, NE_HEADER_OFFSET, &ne_header_off, sizeof( ne_header_off ) ) ) {
        return( false );
    }
    if( !seekRead( handle, ne_header_off, pehdr, PE_HDR_SIZE )
      || pehdr->signature != EXESIGN_PE
      || ReadFile( handle, (char *)pehdr + PE_HDR_SIZE, PE_OPT_SIZE( *pehdr ), &bytes, NULL ) == 0
      || bytes != PE_OPT_SIZE( *pehdr ) ) {
        return( false );
    }
    return( true );

} /* getPEHeader */

/*
 * codeLoad - handle the loading of a new DLL/EXE
 */
static void codeLoad( HANDLE handle, DWORD image_base, const char *name, samp_block_kinds kind )
{
    far_address         ovl;
    int                 i;
    pe_object           obj;
    WORD                seg;
    DWORD               bytes;
    pe_exe_header       pehdr;

    ovl.offset = 0;
    ovl.segment = 0;
    WriteCodeLoad( ovl, name, kind );
    if( getPEHeader( handle, &pehdr ) ) {
        for( i = 0; i < pehdr.fheader.num_objects; i++ ) {
            ReadFile( handle, &obj, sizeof( obj ), &bytes, NULL );
            if( obj.flags & (PE_OBJ_CODE | PE_OBJ_EXECUTABLE) ) {
                seg = _FP_SEG( codeLoad );
            } else {
                seg = _FP_SEG( &seg );
            }
            WriteAddrMap( i + 1, seg, (DWORD)image_base + obj.rva );
        }
    }

} /* codeLoad */

/*
 * newThread - a new thread has been created
 */
static void newThread( DWORD id, HANDLE th )
{
    threadInfo = realloc( threadInfo, ( threadCount + 1 ) * sizeof( thread_info ) );
    threadInfo[threadCount].id = id;
    threadInfo[threadCount].th = th;
    threadInfo[threadCount].live = TRUE;
    threadInfo[threadCount].index = threadCount;

    AllocSamples( id );
    threadInfo[threadCount].Samples = Samples;
    threadInfo[threadCount].SampleIndex = SampleIndex;
    if( CallGraphMode ) {
        threadInfo[threadCount].CallGraph = CallGraph;
        threadInfo[threadCount].SampleCount = SampleCount;
    }
    threadCount++;

} /* newThread */


/*
 * deadThread - a thread is dead
 */
static void deadThread( DWORD id )
{
    thread_info *ti;

    ti = getThreadInfo( id );
    if( ti != NULL ) {
        ti->live = FALSE;
    }

} /* deadThread */

/*
 * loadProg - load the task to sample
 */
static void loadProg( const char *exe, char *cmdline )
{
    STARTUPINFO         sinfo;
    PROCESS_INFORMATION pinfo;
    int                 rc;

    memset( &sinfo, 0, sizeof( sinfo ) );
    sinfo.cb = sizeof( sinfo );
    // set ShowWindow default value for nCmdShow parameter
    sinfo.dwFlags = STARTF_USESHOWWINDOW;
    sinfo.wShowWindow = SW_SHOWNORMAL;
    rc = CreateProcess( NULL,           /* application name */
                        cmdline,        /* command line */
                        NULL,           /* process attributes */
                        NULL,           /* thread attributes */
                        FALSE,          /* inherit handles */
                        DEBUG_ONLY_THIS_PROCESS, /* creation flags */
                        NULL,           /* environment block */
                        NULL,           /* starting directory */
                        &sinfo,         /* startup info */
                        &pinfo          /* process info */
                    );
    if( !rc ) {
        internalErrorMsg( MSG_SAMPLE_3 );
    }
    rc = WaitForDebugEvent( &debugEvent, INFINITE );
    if( !rc || (debugEvent.dwDebugEventCode != CREATE_PROCESS_DEBUG_EVENT)
            || (debugEvent.dwProcessId != pinfo.dwProcessId) ) {
        internalErrorMsg( MSG_SAMPLE_3 );
    }
    taskPid = debugEvent.dwProcessId;
    processHandle = debugEvent.u.CreateProcessInfo.hProcess;
    newThread( debugEvent.dwThreadId, debugEvent.u.CreateProcessInfo.hThread );
    codeLoad( debugEvent.u.CreateProcessInfo.hFile,
                (DWORD)debugEvent.u.CreateProcessInfo.lpBaseOfImage,
                exe,
                SAMP_MAIN_LOAD );

} /* loadProg */

/*
 * myGetThreadContext - get a thread's context
 */
static void myGetThreadContext( DWORD id, CONTEXT *pc )
{
    thread_info *ti;

    pc->ContextFlags = CONTEXT_TO_USE;
    ti = getThreadInfo( id );
    if( ti != NULL ) {
        GetThreadContext( ti->th, pc );
    }

} /* myGetThreadContext */

/*
 * TimerThread - handle timer ticks
 */
static DWORD WINAPI TimerThread( LPVOID parms )
{
    CONTEXT con;
    int i;

    parms = parms;
    for( ;; ) {
        Sleep( sleepTime );
        if( doneSample ) {
            break;
        }
        timeOut = TRUE;
        for( i = 0; i < threadCount; i++ ) {
            if( threadInfo[i].live ) {
                myGetThreadContext( threadInfo[i].id, &con );
                RecordSample( con.Eip, con.SegCs, threadInfo[i].id );
                timeOut = FALSE;
            }
        }
    }
    return( 0 );

} /* TimerThread */


static unsigned GetString( int unicode, LPVOID p, char *buff, unsigned max )
{
    DWORD       len;

    --max;
    if( ReadProcessMemory( processHandle, p, buff, max, &len ) ) {
        buff[len] = '\0';
        return( len );
    }
    len = 0;
    if( unicode ) {
        for( ;; ) {
            if( max <= 1 )
                break;
            if( !ReadProcessMemory( processHandle, p, buff, 2, NULL ) )
                break;
            if( *(wchar_t *)buff == '\0' )
                break;
            buff += sizeof( wchar_t );
            p = (wchar_t *)p + 1;
            max -= sizeof( wchar_t );
            len += sizeof( wchar_t );
        }
        *(wchar_t *)buff = '\0';
    } else {
        for( ;; ) {
            if( max == 0 )
                break;
            if( !ReadProcessMemory( processHandle, p, buff, 1, NULL ) )
                break;
            if( *(char *)buff == '\0' )
                break;
            buff += sizeof( char );
            p = (char *)p + 1;
            max -= sizeof( char );
            len += sizeof( char );
        }
        *(char *)buff = '\0';
    }
    return( len );
}

static int GetDllName( LOAD_DLL_DEBUG_INFO *ld, char *buff, unsigned max )
{
    LPVOID      name;
    DWORD       len;
    wchar_t     *p;

    //NYI: spiffy up to scrounge around in the image
    if( ld->lpImageName == 0 )
        return( FALSE );
    ReadProcessMemory( processHandle, ld->lpImageName, &name, sizeof( name ), &len );
    if( len != sizeof( name ) )
        return( FALSE );
    if( name == NULL )
        return( FALSE );
    len = GetString( ld->fUnicode, name, buff, max );
    if( len == 0 )
        return( FALSE );
    if( ld->fUnicode ) {
        for( p = (wchar_t *)buff; *p != '\0'; ++p, ++buff ) {
            *buff = *p;
        }
        *buff = '\0';
    }
    return( TRUE );
}


/*
 * StartProg - start sampling a program
 */
void StartProg( const char *cmd, const char *prog, const char *full_args, char *dos_args )
{
    DWORD       code;
    DWORD       tid;
    CONTEXT     con;
    BOOL        waiting_for_first_bp;
    DWORD       len;
    DWORD       continue_how;
    BOOL        rc;
    far_address where;
    DWORD       ttid;
    HANDLE      tth;

    /* unused parameters */ (void)cmd; (void)dos_args;

    strcpy( utilBuff, prog );
    strcat( utilBuff, " " );
    strcat( utilBuff, full_args );

    loadProg( prog, utilBuff );
    tid = debugEvent.dwThreadId;

    tth = CreateThread( NULL, 1024, TimerThread, NULL, 0, &ttid );
    if( tth == NULL ) {
        internalErrorMsg( MSG_SAMPLE_3 );
    }
    /* attempt to ensure that we can get all of our samples in one shot */
    SetThreadPriority( tth, THREAD_PRIORITY_TIME_CRITICAL );

    OutputMsgParmNL( MSG_SAMPLE_1, prog );

    waiting_for_first_bp = TRUE;
    continue_how = DBG_CONTINUE;

    for( ;; ) {
        ContinueDebugEvent( taskPid, tid, continue_how );
        rc = WaitForDebugEvent( &debugEvent, INFINITE );
        continue_how = DBG_CONTINUE;
        tid = debugEvent.dwThreadId;
        switch( debugEvent.dwDebugEventCode ) {
        case EXCEPTION_DEBUG_EVENT:
            code = debugEvent.u.Exception.ExceptionRecord.ExceptionCode;
            switch( code ) {
            case STATUS_SINGLE_STEP:
                if( timeOut ) {
                    myGetThreadContext( tid, &con );
                    RecordSample( con.Eip, con.SegCs, tid );
                    timeOut = FALSE;
                }
                break;
            case STATUS_BREAKPOINT:
                if( !waiting_for_first_bp ) {
                    myGetThreadContext( tid, &con );
                    con.Eip--;
                    if( (con.Edx & 0xffff) != 0 ) {    /* this is a mark */
                        ReadProcessMemory( processHandle, (LPVOID)con.Eax, utilBuff, BUFF_SIZE - 1, &len );
                        utilBuff[len] = '\0';
                        where.segment = con.SegCs;
                        where.offset = con.Eip;
                        WriteMark( utilBuff, where );
                    } else {            /* this passes CommonAddr */
                        commonAddr.segment = con.Ecx & 0xffff;
                        commonAddr.offset = con.Ebx;
                    }
                } else {
                    waiting_for_first_bp = FALSE;
                }
                break;
            case STATUS_DATATYPE_MISALIGNMENT:
            case STATUS_ACCESS_VIOLATION:
            case STATUS_IN_PAGE_ERROR:
            case STATUS_NO_MEMORY:
            case STATUS_ILLEGAL_INSTRUCTION:
            case STATUS_NONCONTINUABLE_EXCEPTION:
            case STATUS_INVALID_DISPOSITION:
            case STATUS_ARRAY_BOUNDS_EXCEEDED:
            case STATUS_FLOAT_DENORMAL_OPERAND:
            case STATUS_FLOAT_DIVIDE_BY_ZERO:
            case STATUS_FLOAT_INVALID_OPERATION:
            case STATUS_FLOAT_OVERFLOW:
            case STATUS_FLOAT_STACK_CHECK:
            case STATUS_FLOAT_UNDERFLOW:
            case STATUS_INTEGER_DIVIDE_BY_ZERO:
            case STATUS_INTEGER_OVERFLOW:
            case STATUS_PRIVILEGED_INSTRUCTION:
            case STATUS_STACK_OVERFLOW:
            case STATUS_CONTROL_C_EXIT:
                if( debugEvent.u.Exception.dwFirstChance ) {
                    continue_how = DBG_EXCEPTION_NOT_HANDLED;
                } else {
                    OutputMsgNL( MSG_SAMPLE_4 );
                    doneSample = TRUE;
                    TerminateProcess( processHandle, 0 );
                    report();
                    return;
                }
                break;
            default:
                continue_how = DBG_EXCEPTION_NOT_HANDLED;
                break;
            }
            break;
        case LOAD_DLL_DEBUG_EVENT:
            if( GetDllName( &debugEvent.u.LoadDll, utilBuff, sizeof( utilBuff ) ) ) {
                codeLoad( debugEvent.u.LoadDll.hFile,
                            (DWORD) debugEvent.u.LoadDll.lpBaseOfDll,
                            utilBuff,
                            SAMP_CODE_LOAD );
            }
            break;
        case CREATE_THREAD_DEBUG_EVENT:
            newThread( debugEvent.dwThreadId, debugEvent.u.CreateThread.hThread );
            break;
        case EXIT_THREAD_DEBUG_EVENT:
            deadThread( debugEvent.dwThreadId );
            break;
        case EXIT_PROCESS_DEBUG_EVENT:
            doneSample = TRUE;
//          TerminateProcess( processHandle, 0 ); - already gone!!
            report();
            return;
        }
    }

} /* StartProg */

void SysDefaultOptions( void ) { }

void SysParseOptions( char c, const char **cmd )
{
    switch( c ) {
    case 'r':
        SetTimerRate( cmd );
        break;
    default:
        OutputMsgCharNL( MSG_INVALID_OPTION, c );
        fatal();
        break;
    }
}

void OutputNL( void )
{
    Output( "\r\n" );
}
