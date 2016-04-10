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
* Description:  Win16 performance samping core.
*
****************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <direct.h>
#include <io.h>
#include <ctype.h>
#include <string.h>
#include <stdarg.h>
#include <dos.h>
#include <malloc.h>
#include <conio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "commonui.h"
#include "sample.h"
#include "smpstuff.h"
#include "wmsg.h"
#include "sampwin.h"
#include "exeos2.h"
#include "exedos.h"

#define BUFF_SIZE 512

unsigned short win386sig[] = { 0xDEAD,0xBEEF };
unsigned short win386sig2[] = { 0xBEEF,0xDEAD };

/*
 * FlushSamples - flush the contents of the sample buffer
 */
void FlushSamples( WORD limit )
{
    WORD        i, si;
    DWORD       count, ctick;

    si = GetSampleCount();
    if( si > limit ) {
        StopSampler( &count );
        if( SampleIndex == 0 ) {
            GetSample0Tick( &ctick );
            Samples->pref.tick = ctick;
        }
        for( i = 0; i < (WORD)count; i++ ) {
            Samples->d.sample.sample[SampleIndex].offset = SampSave[i].offset;
            Samples->d.sample.sample[SampleIndex].segment = SampSave[i].seg;
            SampleIndex++;
        }
        SaveSamples();
        MyOutput( MsgArray[MSG_SAMPLE_5 - ERR_FIRST_MESSAGE] );
        StartSampler();
    }

} /* FlushSamples */

int VersionCheck( void )
{
    return( TRUE );
}

void GetCommArea( void )
{
    if( CommonAddr.offset == 0 ) {      /* can't get the common region yet */
        Comm.cgraph_top = 0;
        Comm.top_ip = 0;
        Comm.top_cs = 0;
        Comm.pop_no = 0;
        Comm.push_no = 0;
        Comm.in_hook = 1;               /* don't record sample */
    } else {
        ReadMem( CommonAddr.segment, CommonAddr.offset, &Comm, sizeof( Comm ) );
    }
}

void ResetCommArea( void )
{
    if( CommonAddr.offset != 0 ) {      /* reset common variables */
        Comm.pop_no = 0;
        Comm.push_no = 0;
        ReadMem( FP_SEG( &Comm.pop_no ), FP_OFF( &Comm.pop_no ),
                        MK_FP( CommonAddr.segment, CommonAddr.offset + 9 ),
                        4 );
    }
}

void GetNextAddr( void )
{
    struct {
        unsigned long   ptr;
        seg             cs;
        off             ip;
    } stack_entry;

    if( CommonAddr.offset == 0 ) {
        CGraphOff = 0;
        CGraphSeg = 0;
    } else {
        ReadMem( FP_SEG( Comm.cgraph_top ), FP_OFF( Comm.cgraph_top ),
                        &stack_entry, sizeof( stack_entry ) );
        CGraphOff = stack_entry.ip;
        CGraphSeg = stack_entry.cs;
        Comm.cgraph_top = stack_entry.ptr;
    }
}


unsigned NextThread( unsigned tid )
{
    return( !tid );
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
    return( Ceiling - 20 );
}
void StopProg( void )
{
}

/*
 * CloseShop - finished sampling
 */
void CloseShop( void )
{
    if( SampSave != NULL ) {
        GlobalFree( (HGLOBAL)FP_SEG( SampSave ) );
    }
    if( SharedMemory != NULL ) {        /* JBS 93/03/17 */
        SharedMemory->ShopClosed = TRUE;
    }
    if( MainWindowHandle != NULL ) {
        KillTimer( MainWindowHandle, TIMER_ID );
    }
    MessageLoop();

} /* CloseShop */

/*
 * internalError - a fatal internal error occurred
 */
static void internalError( char * str )
{
    MyOutput( MsgArray[MSG_SAMPLE_6 - ERR_FIRST_MESSAGE], str );
    fatal();

} /* InternalError */


#define BSIZE 256
#define SIG_OFF 0

/*
 * StartProg - start and execute sampled program
 */
void StartProg( char *cmd, char *prog, char *full_args, char *dos_args )
{
    WORD                timer;
    WORD                mod_count;
    parm_data           pdata;
    command_data        cdata;
    MODULEENTRY         me;
    int                 rc;
    FARPROC             notify_fn;
    FARPROC             fault_fn;


    cmd = cmd;

    /*
     * initialize sampler VxD
     */
    timer = GetTimerTick();
    if( timer > SleepTime ) {
        timer = SleepTime/2;
        if( timer == 0 )
            timer = 1;
        SetTimerTick( timer );
    }
    MyOutput( MsgArray[MSG_SAMPLE_7 - ERR_FIRST_MESSAGE], SleepTime );

    /*
     * add existing modules
     */
    MyOutput( MsgArray[MSG_SAMPLE_8 - ERR_FIRST_MESSAGE] );
    mod_count = 0;
    me.dwSize = sizeof( MODULEENTRY );
    if( !ModuleFirst( &me ) ) {
        internalError( MsgArray[MSG_SAMPLE_1 - ERR_FIRST_MESSAGE] );
    }
    do {
        HandleLibLoad( SAMP_CODE_LOAD, me.hModule );
        me.dwSize = sizeof( MODULEENTRY );
        mod_count++;
    } while( ModuleNext( &me ) );
    MyOutput( MsgArray[MSG_SAMPLE_9 - ERR_FIRST_MESSAGE], mod_count );

    /*
     * register as interrupt and notify handler
     */
    fault_fn = MakeProcInstance( (FARPROC)IntHandler, InstanceHandle );
    notify_fn = MakeProcInstance( (FARPROC)NotifyHandler, InstanceHandle );
    if( !InterruptRegister( NULL, fault_fn ) ) {
        internalError( MsgArray[MSG_SAMPLE_2 - ERR_FIRST_MESSAGE] );
    }
    if( !NotifyRegister( NULL, (LPFNNOTIFYCALLBACK)notify_fn, NF_NORMAL | NF_TASKSWITCH ) ) {
        InterruptUnRegister( NULL );
        internalError( MsgArray[MSG_SAMPLE_3 - ERR_FIRST_MESSAGE] );
    }
    Start386Debug();
    if( WDebug386 ) {
        DebuggerIsExecuting( 1 );
        MyOutput( MsgArray[MSG_SAMPLE_10 - ERR_FIRST_MESSAGE] );
    }

    rc = InitSampler( SampSave, MAX_SAMPLES, SleepTime );
    if( rc ) {
        InterruptUnRegister( NULL );
        NotifyUnRegister( NULL );
        DebuggerIsExecuting( -1 );
        Done386Debug();
        internalError( MsgArray[MSG_SAMPLE_4 - ERR_FIRST_MESSAGE] );
    }
    WaitForFirst = FALSE;
    MessageLoop();

    /*
     * build a parm block, and start our guy
     */
    cdata.always2= 2;
    cdata.nCmdShow = SW_NORMAL;
    pdata.wEnvSeg = 0;
    pdata.lpCmdLine = (char __far *)full_args;   /* Must be < 120 chars according to SDK */
    pdata.lpCmdShow = (void __far *)&cdata;
    pdata.dwReserved = 0;

    SampledProg = LoadModule( prog, (LPVOID)&pdata );
    while( !SharedMemory->TaskEnded ) {
        MessageLoop();
    }
    DebuggerIsExecuting( -1 );
    Done386Debug();
    InterruptUnRegister( NULL );
    NotifyUnRegister( NULL );
    MyOutput( MsgArray[MSG_SAMPLE_11 - ERR_FIRST_MESSAGE],
            TotalTime/1000, (WORD)( TotalTime - ( 1000 * ( TotalTime / 1000 ) ) ) );
    FlushSamples( 0 );
    report();


} /* StartProg */


void SysDefaultOptions( void )
{
}

void SysParseOptions( char c, char **cmd )
{
    char buff[2];

    if( c != 'r' ) {
        MyOutput( MsgArray[MSG_INVALID_OPTION - ERR_FIRST_MESSAGE] );
        buff[0] = c;
        buff[1] = '\0';
        MyOutput( buff );
        MyOutput( "\r\n" );
        fatal();
    }
    SetTimerRate( cmd );
}

DWORD WinGetCurrTick( void )
{
    DWORD       ctick;

    GetCurrTick( &ctick );
    CurrTick = ctick;
    return( ctick );
}
