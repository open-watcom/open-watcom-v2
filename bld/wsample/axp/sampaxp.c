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
#include <stdlib.h>
#include <stddef.h>
#include <io.h>
#include <ctype.h>
#include <string.h>
#include <process.h>
#include <malloc.h>
#include <conio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "sample.h"
#include "smpstuff.h"
#include "wmsg.h"
#include "windows.h"
#include "exepe.h"
#include "exedos.h"

extern void REPORT_TYPE report();
extern void             StopAndSave();
extern unsigned         SampWrite( int, void FAR_PTR *, unsigned );
extern void             WriteCodeLoad( seg_offset, char *, samp_block_kinds );
extern void             WriteAddrMap( seg, seg, off );
extern void             WriteMark( char FAR_PTR *str, seg_offset where );
extern unsigned         GetNumber(unsigned,unsigned,char**,unsigned);
extern void             Output(char*);
extern void             AllocSamples( unsigned );
extern void             SetTimerRate( char ** );
extern void             fatal(void);
extern void             RecordCGraph( void );
extern char             *MsgArray[ERR_LAST_MESSAGE-ERR_FIRST_MESSAGE+1];

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

#define EVENT_NAME      "tickevent"
#define BUFF_SIZE 2048
#define STACK_SIZE 4096
#define CONTEXT_TO_USE  CONTEXT_CONTROL | CONTEXT_INTEGER
#define LODWORD(a) (((unsigned_64 *)&(a))->u._32[0])
#define SEGMENT (0x0001)

static char             utilBuff[BUFF_SIZE];
static ULONG            sleepTime;
static seg_offset       commonAddr;
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

    for( i=0;i<threadCount;i++ ) {
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
    Samples = threadInfo[ tid ].Samples;
    SampleIndex = threadInfo[ tid ].SampleIndex;
    if( CallGraphMode ) {
        CallGraph = threadInfo[ tid ].CallGraph;
        SampleCount = threadInfo[ tid ].SampleCount;
    }
    return( tid+1 );

} /* NextThread */

void InitTimerRate( void )
{
    sleepTime = 55;
}

void SetTimerRate( char **cmd )
{
    sleepTime = GetNumber( 1, 1000, cmd, 10 );
}

unsigned long TimerRate( void )
{
    return( 1000L * sleepTime );
}

unsigned SafeMargin( void )
{
    return( Ceiling-10 );
}

int VersionCheck( void )
{
    return( TRUE );
}

/*
 * RecordSample - record a sample in a specific thread
 */
void RecordSample( unsigned offset, unsigned short segment, DWORD real_tid )
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
    ti->Samples->d.sample.sample[ ti->SampleIndex ].offset = offset;
    ti->Samples->d.sample.sample[ ti->SampleIndex ].segment = segment;
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
        ReadProcessMemory( processHandle, (LPVOID) commonAddr.offset, &Comm,
                                sizeof( Comm ), &len );
    }
}

void ResetCommArea( void )
{
    DWORD       len;

    if( commonAddr.segment != 0 ) {
        Comm.pop_no = 0;
        Comm.push_no = 0;
        WriteProcessMemory( processHandle, (LPVOID) (commonAddr.offset + 11),
                        &Comm.pop_no, 4, &len );
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
        ReadProcessMemory( processHandle, (LPVOID) Comm.cgraph_top,
                        &stack_entry, sizeof( stack_entry ), &len );
        CGraphOff = stack_entry.ip;
        CGraphSeg = stack_entry.cs;
        Comm.cgraph_top = stack_entry.ptr;
    }
}

void StopProg( void ) {}

static void internalError( char *str )
{
    Output( MsgArray[MSG_SAMPLE_2-ERR_FIRST_MESSAGE] );
    Output( str );
    Output( "\r\n" );
    _exit( -1 );
}

/*
 * seekRead - seek to a specified spot in the file, and read some data
 */
static BOOL seekRead( HANDLE handle, DWORD newpos, void *buff, WORD size )
{
    int         rc;
    DWORD       bytes;

    rc = SetFilePointer( handle, newpos, 0, SEEK_SET );
    if( rc == -1 ) {
        return( FALSE );
    }
    rc = ReadFile( handle, buff, size, &bytes, NULL );
    if( !rc ) {
        return( FALSE );
    }
    if( bytes != size ) {
        return( FALSE );
    }
    return( TRUE );

} /* seekRead */

/*
 * getPEHeader - get the header of the .EXE
 */
static int getPEHeader( HANDLE handle, pe_header *peh )
{
    WORD                data;
    WORD                sig;
    DWORD               nh_offset;

    if( !seekRead( handle, 0x00, &data, sizeof( data ) ) ) {
        return( FALSE );
    }
    if( data != DOS_SIGNATURE ) {
        return( FALSE );
    }

    if( !seekRead( handle, 0x18, &data, sizeof( data ) ) ) {
        return( FALSE );
    }

    if( !seekRead( handle, 0x3c, &nh_offset, sizeof( unsigned_32 ) ) ) {
        return( FALSE );
    }

    if( !seekRead( handle, nh_offset, &sig, sizeof( sig ) ) ) {
        return( FALSE );
    }
    if( sig != PE_SIGNATURE ) {
        return( FALSE );
    }

    if( !seekRead( handle, nh_offset, peh, sizeof( pe_header ) ) ) {
        return( FALSE );
    }
    return( TRUE );

} /* getPEHeader */

/*
 * codeLoad - handle the loading of a new DLL/EXE
 */
static void codeLoad( HANDLE handle, DWORD base, char *name,
                        samp_block_kinds kind )
{
    seg_offset          ovl;
    int                 i;
    pe_object           obj;
    DWORD               offset;
    DWORD               bytes;
    pe_header           peh;

    name = name;

    ovl.offset = 0;
    ovl.segment = 0;
    WriteCodeLoad( ovl, name, kind );
    if( !getPEHeader( handle, &peh ) ) {
        return;
    }
    for( i=0;i<peh.num_objects;i++ ) {
        ReadFile( handle, &obj, sizeof( obj ), &bytes, NULL );
        offset = (DWORD) base + obj.rva;
        WriteAddrMap( i+1, SEGMENT, offset );
    }

} /* codeLoad */

/*
 * newThread - a new thread has been created
 */
static void newThread( DWORD id, HANDLE th )
{
    threadInfo = realloc( threadInfo, (threadCount+1)* sizeof( thread_info ) );
    threadInfo[ threadCount ].id = id;
    threadInfo[ threadCount ].th = th;
    threadInfo[ threadCount ].live = TRUE;
    threadInfo[ threadCount ].index = threadCount;

    AllocSamples( id );
    threadInfo[ threadCount ].Samples = Samples;
    threadInfo[ threadCount ].SampleIndex = SampleIndex;
    if( CallGraphMode ) {
        threadInfo[ threadCount ].CallGraph = CallGraph;
        threadInfo[ threadCount ].SampleCount = SampleCount;
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
static void loadProg( char *exe, char *cmdline )
{
    STARTUPINFO         sinfo;
    PROCESS_INFORMATION pinfo;
    int                 rc;

    memset( &sinfo, 0, sizeof( sinfo ) );
    sinfo.wShowWindow = SW_NORMAL;
    rc = CreateProcess( NULL,           /* application name */
                        cmdline,        /* command line */
                        NULL,           /* process attributes */
                        NULL,           /* thread attributes */
                        FALSE,          /* inherit handles */
                        DEBUG_PROCESS,//DEBUG_ONLY_THIS_PROCESS, /* creation flags */
                        NULL,           /* environment block */
                        NULL,           /* starting directory */
                        &sinfo,         /* startup info */
                        &pinfo          /* process info */
                    );
    if( !rc ) {
        internalError( MsgArray[MSG_SAMPLE_3-ERR_FIRST_MESSAGE] );
    }
    rc = WaitForDebugEvent( &debugEvent, -1 );
    if( !rc || (debugEvent.dwDebugEventCode != CREATE_PROCESS_DEBUG_EVENT) ||
                (debugEvent.dwProcessId != pinfo.dwProcessId) ) {
        internalError( MsgArray[MSG_SAMPLE_3-ERR_FIRST_MESSAGE] );
    }
    taskPid = debugEvent.dwProcessId;
    processHandle = debugEvent.u.CreateProcessInfo.hProcess;
    newThread( debugEvent.dwThreadId, debugEvent.u.CreateProcessInfo.hThread );
    codeLoad( debugEvent.u.CreateProcessInfo.hFile,
                (DWORD) debugEvent.u.CreateProcessInfo.lpBaseOfImage,
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

static void mySetThreadContext( DWORD id, CONTEXT *pc )
{
    thread_info *ti;
    pc->ContextFlags = CONTEXT_TO_USE;
    ti = getThreadInfo( id );
    if( ti != NULL ) {
        SetThreadContext( ti->th, pc );
    }

} /* mySetThreadContext */

/*
 * TimerThread - handle timer ticks
 */
DWORD __stdcall TimerThread( LPVOID parms )
{
    CONTEXT con;
    int i;
    uint_32 Fir;

    parms = parms;
    while( 1 ) {
        Sleep( sleepTime );
        if( doneSample ) {
            break;
        }
        timeOut = TRUE;
        for( i=0;i<threadCount;i++ ) {
            if( threadInfo[i].live ) {
                myGetThreadContext( threadInfo[i].id, &con );
                Fir = LODWORD( con.Fir );
                RecordSample( Fir, SEGMENT, threadInfo[i].id );
                timeOut = FALSE;
            }
        }
    }
    return( 0 );

} /* TimerThread */

/*
 * SkipBreakpoint - increment Fir past a breakpoint
 */
static void SkipBreakpoint( DWORD tid )
{
    CONTEXT con;
    uint_32 Fir;
    unsigned_64 newFir;

    myGetThreadContext( tid, &con );
    Fir = LODWORD( con.Fir );
    newFir.u._32[0] = Fir+4;
    newFir.u._32[1] = 0;
    con.Fir = *((CDWORDLONG *)&newFir);
    mySetThreadContext( tid, &con );

} /* SkipBreakpoint */


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
            if( max <= 1 ) break;
            if( !ReadProcessMemory( processHandle, p, buff, 2, NULL ) ) break;
            if( *(wchar_t *)buff == '\0' ) break;
            buff += sizeof( wchar_t );
            p = (wchar_t *)p + 1;
            max -= sizeof( wchar_t );
            len += sizeof( wchar_t );
        }
        *(wchar_t *)buff = '\0';
    } else {
        for( ;; ) {
            if( max == 0 ) break;
            if( !ReadProcessMemory( processHandle, p, buff, 1, NULL ) ) break;
            if( *(char *)buff == '\0' ) break;
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
    if( ld->lpImageName == 0 ) return( FALSE );
    ReadProcessMemory( processHandle, ld->lpImageName, &name, sizeof( name ), &len );
    if( len != sizeof( name ) ) return( FALSE );
    if( name == 0 ) return( FALSE );
    len = GetString( ld->fUnicode, name, buff, max );
    if( len == 0 ) return( FALSE );
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
void StartProg( char *cmd, char *prog, char *args )
{
    DWORD       code;
    DWORD       tid;
    CONTEXT     con;
    BOOL        waiting_for_first_bp;
    DWORD       len;
    DWORD       continue_how;
    BOOL        rc;
    DWORD       ttid;
    HANDLE      tth;
    int         i;
    uint_32     Fir;

    cmd = cmd;

    strcpy( utilBuff, prog );
    len = strlen( utilBuff );
    utilBuff[ len++ ] = ' ';
    for( i=0;i<args[0];i++ ) {
        utilBuff[len++] = args[i+1];
    }
    utilBuff[len] = 0;

    loadProg( prog, utilBuff );
    tid = debugEvent.dwThreadId;

    tth = CreateThread( NULL, 2048, (LPVOID) TimerThread, NULL,
                0, &ttid );
    if( !tth ) {
        internalError( MsgArray[MSG_SAMPLE_3-ERR_FIRST_MESSAGE] );
    }
    /* Attempt to ensure that we can record our samples in one shot */
    SetThreadPriority( tth, THREAD_PRIORITY_TIME_CRITICAL );

    Output( MsgArray[MSG_SAMPLE_1-ERR_FIRST_MESSAGE] );
    Output( prog );
    Output( "\r\n" );

    waiting_for_first_bp = TRUE;
    continue_how = DBG_CONTINUE;

    while( 1 ) {
        ContinueDebugEvent( taskPid, tid, continue_how );
        rc = WaitForDebugEvent( &debugEvent, -1 );
        continue_how = DBG_CONTINUE;
        tid = debugEvent.dwThreadId;
        switch( debugEvent.dwDebugEventCode ) {
        case EXCEPTION_DEBUG_EVENT:
            code = debugEvent.u.Exception.ExceptionRecord.ExceptionCode;
            switch( code ) {
            case STATUS_SINGLE_STEP:
                if( timeOut ) {
                    myGetThreadContext( tid, &con );
                    Fir = LODWORD( con.Fir );
                    RecordSample( Fir, SEGMENT, tid );
                    timeOut = FALSE;
                }
                break;
            case STATUS_BREAKPOINT:
                /* Skip past the breakpoint in the startup code */
                if( waiting_for_first_bp ) {
                    SkipBreakpoint( tid );
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
                    Output( MsgArray[MSG_SAMPLE_4-ERR_FIRST_MESSAGE] );
                    Output( "\r\n" );
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

void SysParseOptions( char c, char **cmd )
{
    char buff[2];

    switch( c ) {
    case 'r':
        SetTimerRate( cmd );
        break;
    default:
        Output( MsgArray[MSG_INVALID_OPTION-ERR_FIRST_MESSAGE] );
        buff[0] = c;
        buff[1] = '\0';
        Output( buff );
        Output( "\r\n" );
        fatal();
        break;
    }
}
