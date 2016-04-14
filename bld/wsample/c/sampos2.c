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
* Description:  OS/2 1.x performance sampling core.
*
****************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include <i86.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "sample.h"
#include "wmsg.h"
#include "smpstuff.h"
#define INCL_BASE
#define INCL_DOSDEVICES
#define INCL_DOSMEMMGR
#define INCL_DOSPROCESS
#include "os2.h"
#include "os2dbg.h"


#define BUFF_SIZE 512
static char             UtilBuff[BUFF_SIZE];
static TRACEBUF         Buff;
static USHORT           Pid;
static USHORT           InitialCS;
static USHORT           MainMod = 0;
static USHORT           SleepTime;
static unsigned         *SampleIndexP;
static unsigned         *SampleCountP;
static samp_block       * FAR_PTR *SamplesP;
static samp_block       * FAR_PTR *CallGraphP;
static unsigned         MaxThread = 1;
static USHORT           OSVer;
static int              NewSession;

static seg_offset       CommonAddr;

#define STACK_SIZE 4096
static unsigned char    __near Stack[STACK_SIZE];

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

#define EXE_LX  0x584c
#define EXE_NE  0x454e

static int IsLX( void )
{
    int                 f;
    unsigned_32         offset;
    unsigned_16         sig;
    unsigned_16         flags;

    f = open( ExeName, O_BINARY | O_RDONLY );
    if( f == -1 )
        return( 0 );
    if( lseek( f, 0x3c, SEEK_SET ) != 0x3c )
        return( 0 );
    if( read( f, &offset, sizeof( offset ) ) != sizeof( offset ) )
        return( 0 );
    if( lseek( f, offset, SEEK_SET ) != offset )
        return( 0 );
    if( read( f, &sig, sizeof( sig ) ) != sizeof( sig ) )
        return( 0 );
    if( sig == EXE_NE ) {
        offset += 12;
        if( lseek( f, offset, SEEK_SET ) != offset )
            return( 0 );
        if( read( f, &flags, sizeof( flags ) ) != sizeof( flags ) )
            return( 0 );
        if( (flags & 0x0300) == 0x0300 ) {
            /* PM app */
            NewSession = 1;
        }
    }
    close( f );
    return( sig == EXE_LX );
}

static void InternalError( char * str )
{
    Output( MsgArray[MSG_SAMPLE_2 - ERR_FIRST_MESSAGE] );
    Output( str );
    Output( "\r\n" );
    _exit( -1 );
}

#define OS22SAMPLER "WSMPOS22.EXE"

/*
    If we're running on OS/2 2.x and it's an LX style file, spawn off the
    OS/2 2.x sampler to collect things
*/
bool VersionCheck( void )
{
    USHORT          env_sel;
    USHORT          cmd_off;
    RESULTCODES     res;

    DosGetVersion( &OSVer );
    if( OSVer >= 0x1400 && IsLX() ) {
        if( DosSearchPath( 0x0003, "PATH", OS22SAMPLER, (unsigned char *)UtilBuff, sizeof( UtilBuff ) ) ) {
            InternalError( MsgArray[MSG_SAMPLE_8 - ERR_FIRST_MESSAGE] );
        }
        DosGetEnv( &env_sel, &cmd_off );
        if( DosExecPgm( NULL, 0, EXEC_ASYNC, MK_FP( env_sel, cmd_off ), NULL, &res, UtilBuff ) != 0 ) {
            InternalError( MsgArray[MSG_SAMPLE_9 - ERR_FIRST_MESSAGE] );
        }
        _exit( 0 );
    }
    return( OSVer >= 0x0a14 );
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

static void RecordSample( unsigned offset, unsigned short segment, USHORT tid )
{
    samp_block  FAR_PTR *old_samples;
    unsigned    old_sample_index;
    unsigned    old_sample_count;

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


static void readMemory( seg_offset *from, int size, char *to )
{
    static TRACEBUF     mybuff;

    mybuff.pid = Pid;
    mybuff.tid = 0;     /* NB: callgraph info only supported on thread 0 */
    mybuff.segv = from->segment;
    mybuff.offv = from->offset + size - 1;
    while( size ) {
        mybuff.cmd = PT_CMD_READ_MEM_D;
        DosPTrace( &mybuff );
        size--;
        mybuff.offv--;
        to[size] = mybuff.value;
    }
}


static void writeMemory( seg_offset *to, int size, char *from )
{
    static TRACEBUF     mybuff;

    mybuff.pid = Pid;
    mybuff.tid = 0;     /* NB: callgraph info only supported on thread 0 */
    mybuff.segv = to->segment;
    mybuff.offv = to->offset + size - 1;
    while( size ) {
        size--;
        mybuff.value = from[size];
        mybuff.cmd = PT_CMD_WRITE_MEM_D;
        DosPTrace( &mybuff );
        mybuff.offv--;
    }
}


void GetCommArea( void )
{
    if( CommonAddr.segment == 0 ) {             /* can't get the common */
        Comm.cgraph_top = 0;                    /* region yet.          */
        Comm.top_ip = 0;
        Comm.top_cs = 0;
        Comm.pop_no = 0;
        Comm.push_no = 0;
        Comm.in_hook = 1;       /* don't record this sample */
    } else {
        readMemory( &CommonAddr, sizeof( Comm ), (char *)&Comm );
    }
}

void ResetCommArea( void )
{
    if( CommonAddr.segment != 0 ) {     /* reset common variables */
        Comm.pop_no = 0;
        Comm.push_no = 0;
        CommonAddr.offset += 9;
        writeMemory( &CommonAddr, 4, (char *)&Comm.pop_no );
        CommonAddr.offset -= 9;
    }
}

void GetNextAddr( void )
{
    seg_offset  addr;
    struct {
        unsigned long   ptr;
        seg             cs;
        off             ip;
    } stack_entry;

    if( CommonAddr.segment == 0 ) {
        CGraphOff = 0;
        CGraphSeg = 0;
    } else {
        addr.segment = FP_SEG( Comm.cgraph_top );
        addr.offset = FP_OFF( Comm.cgraph_top );
        readMemory( &addr, sizeof( stack_entry ), (char *)&stack_entry );
        CGraphOff = stack_entry.ip;
        CGraphSeg = stack_entry.cs;
        Comm.cgraph_top = stack_entry.ptr;
    }
}

void StopProg( void )
{
}


static void CodeLoad( TRACEBUF FAR_PTR *buff, USHORT mte,
                      char *name, samp_block_kinds kind )
{
    seg_offset  ovl;
    int         i;

    ovl.offset = 0;
    ovl.segment = 0;
    WriteCodeLoad( ovl, name, kind );
    buff->mte = mte;
    for( i = 1;; ++i ) {
        buff->cmd = PT_CMD_SEG_TO_SEL;
        buff->value = i;
        if( DosPTrace( buff ) != 0 )
            break;
        if( buff->cmd != PT_RET_SUCCESS )
            break;
        WriteAddrMap( i, buff->value, 0 );
    }
}


static USHORT LibLoadPTrace( TRACEBUF FAR_PTR *buff )
{
    int         cmd;
    int         value;
    USHORT      rv;
    USHORT      offv;
    USHORT      segv;
    char        name[BUFF_SIZE];

    cmd = buff->cmd;
    value = buff->value;
    segv = buff->segv;
    offv = buff->offv;
    for( ;; ) {
        rv = DosPTrace( buff );
        if( MainMod == 0 )
            MainMod = buff->mte;
        if( buff->cmd != PT_RET_LIB_LOADED )
            return( rv );
        buff->cmd = PT_CMD_GET_LIB_NAME;
        buff->segv = FP_SEG( name );
        buff->offv = FP_OFF( name );
        DosPTrace( buff );
        CodeLoad( buff, buff->value, name,
                 (MainMod == buff->value) ? SAMP_MAIN_LOAD : SAMP_CODE_LOAD );
        buff->value = value;
        buff->cmd = cmd;
        buff->offv = offv;
        buff->segv = segv;
    }
}

static void __far Sleeper( void )
{
    static TRACEBUF     mybuff;

    for( ;; ) {
        DosSleep( SleepTime );
        mybuff.pid = Pid;
        mybuff.tid = 1;
        mybuff.cmd = PT_CMD_STOP;
        if( DosPTrace( &mybuff ) != 0 ) {
#if 0
            InternalError( "DosPTrace( STOP ) failed" );
#endif
        }
    }
}

typedef struct _NEWSTARTDATA {  /* stdata */
    USHORT  Length;
    USHORT  Related;
    USHORT  FgBg;
    USHORT  TraceOpt;
    PSZ     PgmTitle;
    PSZ     PgmName;
    PBYTE   PgmInputs;
    PBYTE   TermQ;
    PBYTE   Environment;
    USHORT  InheritOpt;
    USHORT  SessionType;
    PSZ     IconFile;
    ULONG   PgmHandle;
    USHORT  PgmControl;
    USHORT  InitXPos;
    USHORT  InitYPos;
    USHORT  InitXSize;
    USHORT  InitYSize;
    USHORT  Reserved;
    PSZ     ObjectBuffer;
    ULONG   ObjectBuffLen;
} NEWSTARTDATA;

static void LoadProg( char *cmd, char *cmd_tail )
{
    RESULTCODES         res;
    NEWSTARTDATA        start;
    USHORT              SID;

    if( NewSession ) {
        start.Length = 50;
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
        start.IconFile = NULL;
        start.PgmHandle = 0;
        start.PgmControl = 0;
        if( DosStartSession( (void __far *)&start, &SID, &Pid ) != 0 ) {
            InternalError( MsgArray[MSG_SAMPLE_3 - ERR_FIRST_MESSAGE] );
        }
    } else {
        if( DosExecPgm( NULL, 0, EXEC_TRACE, cmd, NULL, &res, cmd ) != 0 ) {
            InternalError( MsgArray[MSG_SAMPLE_3 - ERR_FIRST_MESSAGE] );
        }
        Pid = res.codeTerminate;
    }
}


#define BSIZE 256

void StartProg( char *cmd, char *prog, char *full_args, char *dos_args )
{

    char        *src;
    char        *dst;
    USHORT      drive;
    ULONG       map;
    USHORT      len;
    USHORT      tid;
    USHORT      rc;
    char        buff[BSIZE];
    seg_offset  where;
    char        *cmd_tail;

    cmd = cmd;
    MaxThread = 0;
    GrowArrays( 1 );
    src = prog;
    dst = UtilBuff;
    DosQCurDisk( &drive, &map );
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
        DosQCurDir( drive, (PBYTE)dst, &len );
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
    strcpy( dst, full_args );
    dst += strlen( dst );
    *++dst = '\0';      /* Need two nulls at end */
    LoadProg( UtilBuff, cmd_tail );
    Output( MsgArray[MSG_SAMPLE_1 - ERR_FIRST_MESSAGE] );
    Output( UtilBuff );
    Output( "\r\n" );
    Buff.pid = Pid;
    Buff.tid = 1;
    Buff.cmd = PT_CMD_STOP;
    LibLoadPTrace( &Buff );
    if( OSVer < 0x1400 ) {
        /* OS/2 2.x already dumped out MainMod as a Module load */
        CodeLoad( &Buff, MainMod, ExeName, SAMP_MAIN_LOAD );
    }
    InitialCS = Buff.u.r.CS;
    rc = DosCreateThread( Sleeper, (PUSHORT)&tid, Stack + STACK_SIZE );
    if( rc != 0 ) {
        InternalError( MsgArray[MSG_SAMPLE_4 - ERR_FIRST_MESSAGE] );
    }
    rc = DosSetPrty( PRTYS_THREAD, PRTYC_TIMECRITICAL, 0, tid );
    if( rc != 0 ) {
        InternalError( MsgArray[MSG_SAMPLE_5 - ERR_FIRST_MESSAGE] );
    }
    Buff.pid = Pid;
    Buff.tid = 1;
    for( ;; ) {
        Buff.cmd = PT_CMD_GO;
        if( LibLoadPTrace( &Buff ) != 0 ) {
            InternalError( MsgArray[MSG_SAMPLE_7 - ERR_FIRST_MESSAGE] );
        }
        if( Buff.cmd == PT_RET_BREAK && Buff.u.r.DX != 0 ) {    /* a mark */
            len = 0;
            Buff.segv = Buff.u.r.DX;
            Buff.offv = Buff.u.r.AX;
            for( ;; ) {
                Buff.cmd = PT_CMD_READ_MEM_D;
                DosPTrace( &Buff );
                buff[len] = Buff.value;
                if( Buff.cmd != PT_RET_SUCCESS )
                    buff[len] = '\0';
                if( len == BSIZE )
                    buff[len] = '\0';
                if( buff[len] == '\0' )
                    break;
                ++len;
                Buff.offv++;
            }
            where.segment = Buff.u.r.CS;
            where.offset = Buff.u.r.IP;
            WriteMark( buff, where );
            Buff.cmd = PT_CMD_READ_REGS;
            DosPTrace( &Buff );
            Buff.u.r.IP++;
            Buff.cmd = PT_CMD_WRITE_REGS;
            DosPTrace( &Buff );
            continue;
        } else if( Buff.cmd == PT_RET_BREAK ) {         /* common info pass */
            CommonAddr.segment = Buff.u.r.CX;
            CommonAddr.offset = Buff.u.r.BX;
            Buff.cmd = PT_CMD_READ_REGS;
            DosPTrace( &Buff );
            Buff.u.r.IP++;
            Buff.cmd = PT_CMD_WRITE_REGS;
            DosPTrace( &Buff );
            continue;
        }
        if( Buff.cmd == PT_RET_FUNERAL )
            break;
        if( Buff.cmd != PT_RET_LIB_LOADED
          && Buff.cmd != PT_RET_STOPPED
          && Buff.cmd != PT_RET_TRD_TERMINATE ) {
            InternalError( MsgArray[MSG_SAMPLE_6 - ERR_FIRST_MESSAGE] );
            break;
        }
        RecordSample( Buff.u.r.IP, Buff.u.r.CS, Buff.tid );
    }
    report();
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
