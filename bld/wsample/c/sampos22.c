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
* Description:  OS/2 2.x performance sampling core.
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
#define INCL_32
#define INCL_BASE
#define INCL_DOSDEVICES
#define INCL_DOSMEMMGR
#define INCL_DOSPROCESS
#define INCL_DOSSESMGR
#include "os2.h"

#include "sample.h"
#include "wmsg.h"
#include "smpstuff.h"


// "Fake" notification codes used internally
#define DBG_N_Breakpoint        -100
#define DBG_N_SStep             -101
#define DBG_N_Signal            -102

#define BUFF_SIZE       2048
#define STACK_SIZE      32768

static char             UtilBuff[BUFF_SIZE];
static uDB_t            Buff;
static PID              Pid;
static USHORT           InitialCS;
static ULONG            MainMod = 0;
static ULONG            SleepTime;
static unsigned         *SampleIndexP;
static unsigned         *SampleCountP;
static samp_block       * FAR_PTR *SamplesP;
static samp_block       * FAR_PTR *CallGraphP;
static unsigned         MaxThread = 1;
static unsigned         ExceptNum;
static int              NewSession;
static int              sleepProcId = 0;

static seg_offset       CommonAddr;

unsigned NextThread( unsigned tid )
{
    if( tid == MaxThread )
        return( 0 );
    Samples = SamplesP[tid];
    SampleIndex = SampleIndexP[tid];
    if( CallGraphMode ) {
        CallGraph = CallGraphP[tid];
        SampleCount = SampleCountP[tid];
    }
    return( tid + 1 );
}

void InitTimerRate( void )
{
    SleepTime = 55;
}

void SetTimerRate( char **cmd )
{
    SleepTime = GetNumber( 1, 1000, cmd, 10 );
}

unsigned long TimerRate( void )
{
    return( 1000L * SleepTime );
}

unsigned SafeMargin( void )
{
    return( Ceiling - 10 );
}

int VersionCheck( void )
{
    return( TRUE );
}

static void GrowArrays( unsigned tid )
{
    unsigned    max;

    max = MaxThread;
    SamplesP = realloc( SamplesP, tid * sizeof( void * ) );
    SampleIndexP = realloc( SampleIndexP, tid * sizeof( void * ) );
    if( CallGraphMode ) {
        CallGraphP = realloc( CallGraphP, tid * sizeof( void * ) );
        SampleCountP = realloc( SampleCountP, tid * sizeof( void * ) );
    }
    while( max < tid ) {
        AllocSamples( max + 1 );
        SamplesP[max] = Samples;
        SampleIndexP[max] = SampleIndex;
        if( CallGraphMode ) {
            CallGraphP[max] = CallGraph;
            SampleCountP[max] = SampleCount;
        }
        ++max;
    }
    MaxThread = max;
}

static void RecordSample( unsigned offset, unsigned short segment, TID tid )
{
    samp_block FAR_PTR *old_samples;
    unsigned old_sample_index;
    unsigned old_sample_count;

    if( tid > MaxThread ) {
        GrowArrays( tid );
    }
    --tid;
    LastSampleIndex = SampleIndexP[tid];
    if( SampleIndexP[tid] == 0 ) {
        SamplesP[tid]->pref.tick = CurrTick;
        if( CallGraphMode ) {
            CallGraphP[tid]->pref.tick = CurrTick;
        }
    }
    ++CurrTick;
    SamplesP[tid]->d.sample.sample[SampleIndexP[tid]].offset = offset;
    SamplesP[tid]->d.sample.sample[SampleIndexP[tid]].segment = segment;
    SampleIndexP[tid]++;
    if( CallGraphMode ) {
        SampleCountP[tid]++;
    }
    if( CallGraphMode && tid == 0 ) {
        old_sample_count = SampleCount;
        old_samples = Samples;                  /* since RecordCGraph isn't */
        old_sample_index = SampleIndex;         /* used to threads, we fool */
        Samples = SamplesP[tid];              /* it into storing the info */
        SampleIndex = SampleIndexP[tid];      /* in the right place by    */
        SampleCount = SampleCountP[tid];
        RecordCGraph();                         /* changing its pointers    */
        SamplesP[tid] = Samples;              /* and restoring them later */
        SampleIndexP[tid] = SampleIndex;
        SampleCountP[tid] = SampleCount;
        Samples = old_samples;
        SampleIndex = old_sample_index;
        SampleCount = old_sample_count;
    }
    if( SampleIndexP[tid] >= Margin ) {
        StopAndSave();
    }
}


void GetCommArea( void )
{
    uDB_t   mybuff;

    if( CommonAddr.segment == 0 ) {     /* can't get the common region yet */
        Comm.cgraph_top = 0;
        Comm.top_ip = 0;
        Comm.top_cs = 0;
        Comm.pop_no = 0;
        Comm.push_no = 0;
        Comm.in_hook = 1;       /* don't record this sample */
    } else {
        mybuff.Pid = Pid;
        mybuff.Tid = 0;         /* callgraph info only supported on thread 0 */
        mybuff.Cmd = DBG_C_ReadMemBuf;
        mybuff.Addr = CommonAddr.offset;
        mybuff.Len = sizeof( Comm );
        mybuff.Buffer = FP_OFF( &Comm );
        DosDebug( &mybuff );
    }
}


void ResetCommArea( void )
{
    uDB_t   mybuff;

    if( CommonAddr.segment != 0 ) {     /* reset common variables */
        Comm.pop_no = 0;
        Comm.push_no = 0;
        mybuff.Pid = Pid;
        mybuff.Tid = 0;         /* callgraph info only supported on thread 0 */
        mybuff.Cmd = DBG_C_WriteMemBuf;
        mybuff.Addr = CommonAddr.offset + 11;
        mybuff.Len = 4;
        mybuff.Buffer = FP_OFF( &Comm.pop_no );
        DosDebug( &mybuff );
    }
}


void GetNextAddr( void )
{
    uDB_t   mybuff;
    struct {
        unsigned long   ptr;
        seg             cs;
        off             ip;
    } stack_entry;

    if( CommonAddr.segment == 0 ) {
        CGraphOff = 0;
        CGraphSeg = 0;
    } else {
        mybuff.Pid = Pid;
        mybuff.Tid = 0;
        mybuff.Cmd = DBG_C_ReadMemBuf;
        mybuff.Addr = Comm.cgraph_top;
        mybuff.Len = sizeof( stack_entry );
        mybuff.Buffer = FP_OFF( &stack_entry );
        DosDebug( &mybuff );
        CGraphOff = stack_entry.ip;
        CGraphSeg = stack_entry.cs;
        Comm.cgraph_top = stack_entry.ptr;
    }
}


void StopProg( void )
{
}


static void CodeLoad( uDB_t FAR_PTR *buff, ULONG mte,
                      char *name, samp_block_kinds kind )
{
    seg_offset  ovl;
    int         i;

    ovl.offset = 0;
    ovl.segment = 0;
    WriteCodeLoad( ovl, name, kind );
    buff->MTE = mte;
    for( i = 1;; ++i ) {
        buff->Cmd = DBG_C_NumToAddr;
        buff->Value = i;
        if( DosDebug( buff ) != 0 )
            break;
        if( buff->Cmd != DBG_N_Success )
            break;
        /* Assume that all 32-bit apps are running on the CS selector value */
        WriteAddrMap( i, FP_SEG( CodeLoad ), buff->Addr );
    }
}


static void InternalError( char * str )
{
    Output( MsgArray[MSG_SAMPLE_2 - ERR_FIRST_MESSAGE] );
    Output( str );
    Output( "\r\n" );
    _exit( -1 );
}


static void DebugExecute( uDB_t *buff, ULONG cmd )
{
//    EXCEPTIONREPORTRECORD     ex;
    ULONG                       value;
    ULONG                       stopvalue;
    ULONG                       notify=0;
    char                        name[BUFF_SIZE];

    buff->Cmd = cmd;
    value = buff->Value;
    if( cmd == DBG_C_Go )
        value = 0;
    stopvalue = XCPT_CONTINUE_EXECUTION;
    if( cmd == DBG_C_Stop )
        stopvalue = XCPT_CONTINUE_STOP;

    for( ;; ) {

        buff->Value = value;
        buff->Cmd = cmd;
        if( DosDebug( buff ) ) {
            InternalError( MsgArray[MSG_SAMPLE_7 - ERR_FIRST_MESSAGE] );
        }
        if( MainMod == 0 )
            MainMod = buff->MTE;
        value = stopvalue;
        cmd = DBG_C_Continue;

        /*
         * handle the preemptive notifications
         */
        switch( buff->Cmd ) {
        case DBG_N_ModuleLoad:
            DosQueryModuleName( buff->Value, BUFF_SIZE, name );
            CodeLoad( buff, buff->Value, name,
                (buff->Value == MainMod) ? SAMP_MAIN_LOAD : SAMP_CODE_LOAD );
            buff->MTE = MainMod;
            break;
        case DBG_N_ModuleFree:
            break;
        case DBG_N_NewProc:
            break;
        case DBG_N_ProcTerm:
            value = XCPT_CONTINUE_STOP;         /* halt us */
            notify = DBG_N_ProcTerm;
            break;
        case DBG_N_ThreadCreate:
            break;
        case DBG_N_ThreadTerm:
            break;
        case DBG_N_AliasFree:
            break;
        case DBG_N_Exception:
#if 0
            /*
             * Value should contain a pointer to the user exception
             * block
             */
            buff->Cmd = DBG_C_ReadMemBuf;
            buff->Buffer = &ex;
            buff->Addr = buff->Value;
            buff->Len = sizeof( ex );
            DosDebug( buff );
            ExceptNum = ex.ExceptionNum;
#endif

            /*
             * Buffer appears to hold the exception number for int 1
             * and int 3
             */
            ExceptNum = buff->Buffer;
            switch( ExceptNum ) {
            case XCPT_BREAKPOINT:
                notify = DBG_N_Breakpoint;
                value = XCPT_CONTINUE_STOP;
                break;
            case XCPT_SINGLE_STEP:
                notify = DBG_N_SStep;
                value = XCPT_CONTINUE_STOP;
                break;
            case XCPT_DATATYPE_MISALIGNMENT:
            case XCPT_ACCESS_VIOLATION:
            case XCPT_ILLEGAL_INSTRUCTION:
            case XCPT_FLOAT_DENORMAL_OPERAND:
            case XCPT_FLOAT_DIVIDE_BY_ZERO:
            case XCPT_FLOAT_INEXACT_RESULT:
            case XCPT_FLOAT_INVALID_OPERATION:
            case XCPT_FLOAT_OVERFLOW:
            case XCPT_FLOAT_STACK_CHECK:
            case XCPT_FLOAT_UNDERFLOW:
            case XCPT_INTEGER_DIVIDE_BY_ZERO:
            case XCPT_INTEGER_OVERFLOW:
            case XCPT_PRIVILEGED_INSTRUCTION:
                notify = DBG_N_Exception;
                value = XCPT_CONTINUE_STOP;
                break;
            default:
//              value = XCPT_CONTINUE_SEARCH;
                /*
                 * Index should contain which pass into the exception
                 * handler, but value appears to hold it
                 */
                if( buff->Value == 1 ) {
                    value = XCPT_CONTINUE_SEARCH;
                    break;
                }
                notify = DBG_N_Exception;
                value = XCPT_CONTINUE_STOP;
                break;
            }
            break;
        default:
            if( notify != 0 )
                buff->Cmd = notify;
            return;
        }
    }
}


static void APIENTRY Sleeper( unsigned long parm )
{
    static uDB_t    mybuff;

    parm = parm;
    for( ;; ) {
        DosSleep( SleepTime );
        mybuff.Pid = Pid;
        mybuff.Cmd = DBG_C_Stop;
        if( DosDebug( &mybuff ) != 0 ) {
#if 0
/* Only reason to fail is if the process has already died/stopped.
   This can happen if it takes us a long time to write out sample file
*/
            InternalError( MsgArray[MSG_SAMPLE_8 - ERR_FIRST_MESSAGE] );
#endif
        }
        if( mybuff.Cmd == -1 && mybuff.Value == ERROR_INVALID_PROCID ) {
            sleepProcId = -1;
            return;
        }
    }
}

static void LoadProg( char *cmd, char *cmd_tail )
{
    RESULTCODES         res;
    STARTDATA           start;
    ULONG               SID;
    ULONG               flags;

    if( DosQueryAppType( cmd, &flags ) == 0 ) {
        if( (flags & FAPPTYP_EXETYPE) == FAPPTYP_WINDOWAPI ) {
            NewSession = 1;
        }
    }
    if( NewSession ) {
        start.Length = offsetof( STARTDATA, IconFile );
        start.Related = 1;
        start.FgBg = 0;
        start.TraceOpt = 1;
        start.PgmTitle = cmd;
        start.PgmName = cmd;
        start.PgmInputs = (PBYTE)cmd_tail;
        start.TermQ = 0;
        start.Environment = NULL;
        start.InheritOpt = 1;
        start.SessionType = 0;
        if( DosStartSession( &start, &SID, &Pid ) != 0 ) {
            InternalError( MsgArray[MSG_SAMPLE_3 - ERR_FIRST_MESSAGE] );
        }
    } else {
        if( DosExecPgm( NULL, 0, EXEC_TRACE, cmd, NULL, &res, cmd ) != 0 ) {
            InternalError( MsgArray[MSG_SAMPLE_3 - ERR_FIRST_MESSAGE] );
        }
        Pid = res.codeTerminate;
    }
}


void StartProg( char *cmd, char *prog, char *full_args, char *dos_args )
{

    char        *src;
    char        *dst;
    ULONG       drive;
    ULONG       map;
    ULONG       len;
    seg_offset  where;
    TID         tid;
    ULONG       rc;
    char        *cmd_tail;

    cmd = cmd;
    MaxThread = 0;
    GrowArrays( 1 );
    src = prog;
    dst = UtilBuff;
    DosQueryCurrentDisk( &drive, &map );
    if( src[0] == '\0' || src[1] == '\0' || src[1] != ':' ) {
        *dst++ = drive - 1 + 'A';
        *dst++ = ':';
    } else {
        *dst++ = *src++;
        *dst++ = *src++;
    }
    if( src[0] != '\\' ) {
        ++dst;
        len = BUFF_SIZE - ( dst - UtilBuff );
        DosQueryCurrentDir( drive, (PBYTE)dst, &len );
        dst[-1] = '\\';
        if( *dst == '\\' || *dst == '\0' ) {
            *dst = '\0';
        } else {
            while( *dst != '\0' ) {
                ++dst;
            }
            *dst++ = '\\';
        }
    }
    strcpy( dst, src );
    dst = UtilBuff + strlen( UtilBuff ) + 1;
    cmd_tail = dst;
    strcat( dst, full_args );
    dst += strlen( dst );
    *++dst= '\0';       /* Need two nulls at end */
    LoadProg( UtilBuff, cmd_tail );
    Buff.Pid = Pid;
    Buff.Tid = 0;
    Buff.Cmd = DBG_C_Connect;
    Buff.Value = DBG_L_386;
    if( DosDebug( &Buff ) != 0 ) {
        InternalError( MsgArray[MSG_SAMPLE_9 - ERR_FIRST_MESSAGE] );
    }
    Output( MsgArray[MSG_SAMPLE_1 - ERR_FIRST_MESSAGE] );
    Output( UtilBuff );
    Output( "\r\n" );
    DebugExecute( &Buff, DBG_C_Stop );
    InitialCS = Buff.CS;
    rc = DosCreateThread( &tid, Sleeper, 0, FALSE, STACK_SIZE );
    if( rc != 0 ) {
        InternalError( MsgArray[MSG_SAMPLE_4 - ERR_FIRST_MESSAGE] );
    }
    rc = DosSetPriority( PRTYS_THREAD, PRTYC_TIMECRITICAL, 0, tid );
    if( rc != 0 ) {
        InternalError( MsgArray[MSG_SAMPLE_5 - ERR_FIRST_MESSAGE] );
    }
    Buff.Pid = Pid;
    for( ;; ) {
        DebugExecute( &Buff, DBG_C_Go );
        switch( Buff.Cmd ) {
        case DBG_N_AsyncStop:
            RecordSample( Buff.EIP, Buff.CS, Buff.Tid );
            break;
        case DBG_N_Breakpoint:
            if( (Buff.EDX & 0xffff) != 0 ) {   /* this is a mark */
                Buff.Cmd = DBG_C_ReadReg;
                DosDebug( &Buff );
                len = 0;
                for( ;; ) {
                    if( len >= BUFF_SIZE - 1 )
                        break;
                    Buff.Cmd = DBG_C_ReadMemBuf;
                    Buff.Addr = Buff.EAX + len;
                    Buff.Len = 1;
                    Buff.Buffer = FP_OFF( &UtilBuff[len] );
                    DosDebug( &Buff );
                    if( UtilBuff[len] == '\0' )
                        break;
                    ++len;
                }
                UtilBuff[len] = '\0';
                where.segment = FP_SEG( CodeLoad );
                where.offset = Buff.EIP;
                WriteMark( UtilBuff, where );
            } else {            /* this passes CommonAddr */
                CommonAddr.segment = Buff.ECX & 0xffff;
                CommonAddr.offset = Buff.EBX;
            }
            Buff.EIP++;
            Buff.Cmd = DBG_C_WriteReg;
            DosDebug( &Buff );
            break;
        case DBG_N_Exception:
            Output( MsgArray[MSG_SAMPLE_10 - ERR_FIRST_MESSAGE] );
            Output( "\r\n" );
            /* fall through */
        case DBG_N_ProcTerm:
            Buff.Cmd = DBG_C_Term;
            DosDebug( &Buff );
            report();
            return;
        case DBG_N_ThreadTerm:
            break;
        default:
            if( sleepProcId ) {
                report();
                return;
            }
            InternalError( MsgArray[MSG_SAMPLE_6 - ERR_FIRST_MESSAGE] );
        }
    }
}


void SysDefaultOptions( void )
{
}


void SysParseOptions( char c, char **cmd )
{
    char buff[2];

    switch( c ) {
    case 'r':
        SetTimerRate( cmd );
        break;
    case 's':
        NewSession = 1;
        break;
    default:
        Output( MsgArray[MSG_INVALID_OPTION - ERR_FIRST_MESSAGE] );
        buff[0] = c;
        buff[1] = '\0';
        Output( buff );
        Output( "\r\n" );
        fatal();
        break;
    }
}

