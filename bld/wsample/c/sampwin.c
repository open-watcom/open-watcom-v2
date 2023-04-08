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
#include <conio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#define INCLUDE_TOOLHELP_H
#include <wwindows.h>
#include "sample.h"
#include "smpstuff.h"
#include "wmsg.h"
#include "sampwin.h"
#include "exeos2.h"
#include "exedos.h"
#include "wclbtool.h"
#include "di386cli.h"
#include "segmem.h"


#define BUFF_SIZE 512

static void PushAll( void );
#pragma aux PushAll = ".386" "pusha"

static void PopAll( void );
#pragma aux PopAll = ".386" "popa" __modify [__ax __bx __cx __dx __sp __bp __di __si]

/* commonui/asm/inth.asm */
void FAR PASCAL IntHandler( void );

volatile WORD           __near WaitForFirst=0;
volatile WORD           __near IsSecondOK=0;
far_address             CommonAddr = { 0, 0 };
bool                    WDebug386 = false;
samp_save               __far * __near SampSave;

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
        OutputMsgNL( MSG_SAMPLE_5 );
        StartSampler();
    }

} /* FlushSamples */

bool VersionCheck( void )
{
    return( true );
}

/*
 * ReadMem - read some memory, using toolhelp or wdebug.386
 */
DWORD ReadMem( WORD sel, DWORD off, LPVOID buff, DWORD size )
{
    DWORD       rc;

    if( WDebug386 ) {
        return( CopyMemory386( _FP_SEG( buff ), _FP_OFF( buff ), sel, off, size  ) );
    } else {
        PushAll();
        rc = MemoryRead( sel, off, buff, size );
        PopAll();
        return( rc );
    }

} /* ReadMem */

/*
 * WriteMem - write some memory, using toolhelp or wdebug.386
 */
DWORD WriteMem( WORD sel, DWORD off, LPVOID buff, DWORD size )
{
    DWORD       rc;
    if( WDebug386 ) {
        return( CopyMemory386( sel, off, _FP_SEG( buff ), _FP_OFF( buff ), size ) );
    } else {
        PushAll();
        rc = MemoryWrite( sel, off, buff, size );
        PopAll();
        return( rc );
    }

} /* WriteMem */

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
        ReadMem( _FP_SEG( &Comm.pop_no ), _FP_OFF( &Comm.pop_no ),
                        _MK_FP( CommonAddr.segment, CommonAddr.offset + 9 ),
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
        ReadMem( _FP_SEG( Comm.cgraph_top ), _FP_OFF( Comm.cgraph_top ),
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

void ResetThread( unsigned tid )
{
    if( tid > 0 ) {
        SampleIndex = 0;
        if( CallGraphMode ) {
            SampleCount = 0;
            LastSampleIndex = 0;
        }
    }
}

void InitTimerRate( void )
{
    SleepTime = 55;
}

void SetTimerRate( const char **cmd )
{
    SleepTime = GetNumber( 1, 1000, cmd, 10 );
}

unsigned long TimerRate( void )
{
    return( 1000UL * SleepTime );
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
        GlobalFree( (HGLOBAL)_FP_SEG( SampSave ) );
    }
    if( SharedMemory != NULL ) {        /* JBS 93/03/17 */
        SharedMemory->ShopClosed = TRUE;
    }
    if( MainWindowHandle != NULL ) {
        KillTimer( MainWindowHandle, TIMER_ID );
    }
    MessageLoop();

} /* CloseShop */

#if 0
/*
 * internalError - a fatal internal error occurred
 */
static void internalError( char * str )
{
    OutputMsgParmNL( MSG_SAMPLE_6, str );
    fatal();

} /* InternalError */
#endif

/*
 * internalErrorMsg - a fatal internal error occurred
 */
static void internalErrorMsg( int msg )
{
    OutputMsgParmNL( MSG_SAMPLE_6, GET_MESSAGE( msg ) );
    fatal();

} /* internalErrorMsg */


#define BSIZE 256
#define SIG_OFF 0

/*
 * StartProg - start and execute sampled program
 */
void StartProg( const char *cmd, const char *prog, const char *full_args, char *dos_args )
{
    WORD                timer;
    WORD                mod_count;
    parm_data           pdata;
    command_data        cdata;
    MODULEENTRY         me;
    int                 rc;
    LPFNNOTIFYCALLBACK  notify_fn;
    LPFNINTHCALLBACK    fault_fn;
    char                buffer[10];

    /* unused parameters */ (void)cmd; (void)dos_args;

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
    OutputMsg( MSG_SAMPLE_7 );
    Output( itoa( SleepTime, buffer, 10 ) );
    OutputMsgNL( MSG_SAMPLE_8 );

    /*
     * add existing modules
     */
    OutputMsgNL( MSG_SAMPLE_9 );
    mod_count = 0;
    me.dwSize = sizeof( MODULEENTRY );
    if( !ModuleFirst( &me ) ) {
        internalErrorMsg( MSG_SAMPLE_1 );
    }
    do {
        HandleLibLoad( SAMP_CODE_LOAD, me.hModule );
        me.dwSize = sizeof( MODULEENTRY );
        mod_count++;
    } while( ModuleNext( &me ) );
    OutputMsg( MSG_SAMPLE_10 );
    Output( itoa( mod_count, buffer, 10 ) );
    OutputMsgNL( MSG_SAMPLE_11 );

    /*
     * register as interrupt and notify handler
     */
    fault_fn = MakeProcInstance_INTH( IntHandler, InstanceHandle );
    if( !InterruptRegister( NULL, fault_fn ) ) {
        if( fault_fn != NULL ) {
            FreeProcInstance_INTH( fault_fn );
        }
        internalErrorMsg( MSG_SAMPLE_2 );
    }
    notify_fn = MakeProcInstance_NOTIFY( NotifyHandler, InstanceHandle );
    if( !NotifyRegister( NULL, notify_fn, NF_NORMAL | NF_TASKSWITCH ) ) {
        InterruptUnRegister( NULL );
        if( fault_fn != NULL ) {
            FreeProcInstance_INTH( fault_fn );
        }
        if( notify_fn != NULL ) {
            FreeProcInstance_NOTIFY( notify_fn );
        }
        internalErrorMsg( MSG_SAMPLE_3 );
    }
    Start386Debug();
    if( WDebug386 ) {
        DebuggerIsExecuting( 1 );
        OutputMsgNL( MSG_SAMPLE_12 );
    }

    rc = InitSampler( SampSave, MAX_SAMPLES, SleepTime );
    if( rc ) {
        InterruptUnRegister( NULL );
        NotifyUnRegister( NULL );
        if( fault_fn != NULL ) {
            FreeProcInstance_INTH( fault_fn );
        }
        if( notify_fn != NULL ) {
            FreeProcInstance_NOTIFY( notify_fn );
        }
        DebuggerIsExecuting( -1 );
        Done386Debug();
        internalErrorMsg( MSG_SAMPLE_4 );
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
    pdata.lpReserved = NULL;

    SampledProg = LoadModule( prog, (LPVOID)&pdata );
    while( !SharedMemory->TaskEnded ) {
        MessageLoop();
    }
    DebuggerIsExecuting( -1 );
    Done386Debug();
    InterruptUnRegister( NULL );
    NotifyUnRegister( NULL );
    if( fault_fn != NULL ) {
        FreeProcInstance_INTH( fault_fn );
    }
    if( notify_fn != NULL ) {
        FreeProcInstance_NOTIFY( notify_fn );
    }
    OutputMsg( MSG_SAMPLE_13 );
    Output( itoa( TotalTime/1000, buffer, 10 ) );
    Output( "." );
    Output( itoa( TotalTime % 1000, buffer, 10 ) );
    OutputMsgNL( MSG_SAMPLE_14 );
    FlushSamples( 0 );
    report();


} /* StartProg */


void SysDefaultOptions( void )
{
}

void SysParseOptions( char c, const char **cmd )
{
    if( c != 'r' ) {
        OutputMsgCharNL( MSG_INVALID_OPTION, c );
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
