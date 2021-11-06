/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  NetWare NLM performance sampling core.
*
****************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <io.h>
#include <ctype.h>
#include <string.h>
#include <process.h>
#include <conio.h>
#include <i86.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "sample.h"
#include "smpstuff.h"
#include "wmsg.h"
#include "pathgrp2.h"

#include <ownwthrd.h>

/* NETWARE HOOKS */

#include "miniproc.h"
#include "loader.h"
#include "debugapi.h"
#include "aesproc.h"
#include "event.h"
#include "indos.h"
#include "realmode.h"


typedef struct code_load {
    char                buff[512];
    short               seg;
    long                off;
    struct code_load    *next;
    samp_block_kinds    kind;
} code_load;

extern short GetCS( void );
#pragma aux GetCS = \
        "mov ax,cs" \
    __parm      [] \
    __value     [__ax] \
    __modify    []

#if 0
void CodeLoad( struct LoadDefinitionStructure *loaded, samp_block_kinds kind );
#endif

extern void             SetRestoreRate( const char ** );
extern void             ResolveRateDifferences( void );

extern unsigned long    count_pit0( void );
extern unsigned long    cpuspeed( void );

extern void * ImportSymbol(unsigned long /* handle */, const char * /* symbol_name */);

struct LoadDefinitionStructure      *SampledNLM;
struct ResourceTagStructure         *AllocTag;

static int                          SamplerThread;
static struct AESProcessStructure   AES;
static struct ResourceTagStructure  *AESTag;
static struct ResourceTagStructure  *EventTag;
static struct ResourceTagStructure  *SwitchModeTag;
static bool                         Suspended;
static bool                         Resumed;
static code_load                    *LoadedNLMs;

void SysInit( void )
{
    AllocTag = AllocateResourceTag(
        (void *)GetNLMHandle(),
        (BYTE *)"Open Watcom Sampler Work Area",
        AllocSignature );
    SwitchModeTag = AllocateResourceTag(
        (void*)GetNLMHandle(),
        (BYTE *)"Open Watcom Sampler ModeSwitchMon",
        EventSignature);
}

void StopProg( void )
{
}

#if 0
void CodeLoad( struct LoadDefinitionStructure *loaded, samp_block_kinds kind )
{
    seg_offset          ovl_tbl;
    char                buff[256];

    ovl_tbl.segment = 0;
    ovl_tbl.offset  = 0;
    memcpy( buff, loaded->LDFileName + 1, loaded->LDFileName[0] );
    buff[loaded->LDFileName[0]] = '\0';
    WriteCodeLoad( ovl_tbl, buff, kind );
    WriteAddrMap( 1, GetCS(), loaded->LDCodeImageOffset );
}
#endif

static void RecordCodeLoad( struct LoadDefinitionStructure *loaded, samp_block_kinds kind )
{
    code_load           *new;

    new = my_alloc( sizeof( *new ) );
    new->next = LoadedNLMs;
    LoadedNLMs = new;
    memcpy( new->buff, loaded->LDFileName + 1, loaded->LDFileName[0] );
    new->buff[loaded->LDFileName[0]] = '\0';
    new->seg = GetCS();
    new->off = loaded->LDCodeImageOffset;
    new->kind = kind;
}


static void WriteRecordedLoads( void )
{
    seg_offset          ovl_tbl;
    code_load           *curr,*next;

    ovl_tbl.segment = 0;
    ovl_tbl.offset  = 0;
    for( curr = LoadedNLMs; curr != NULL; curr = next ) {
        next = curr->next;
        WriteCodeLoad( ovl_tbl, curr->buff, curr->kind );
        WriteAddrMap( 1, curr->seg, curr->off );
        my_free( curr );
    }
}

static volatile unsigned nModeSwitched = 0;

static void ModeSwitched( LONG dummy )
{
    /* unused parameters */ (void)dummy;

    nModeSwitched++;
}

static void WakeMeUp( LONG dummy )
{
    static bool                     Already = false;
    struct LoadDefinitionStructure  *loaded;

    /* unused parameters */ (void)dummy;

    if( Already )
        return;

    for( loaded = LoadedList; loaded != NULL; loaded = loaded->LDLink ) {
        RecordCodeLoad( loaded, Already ? SAMP_CODE_LOAD : SAMP_MAIN_LOAD );
        Already = true;
    }
    Already = true;
    Resumed = true;
    if( Suspended ) {
        ResumeThread( SamplerThread );
    }
}


void StartProg( const char *cmd, const char *prog, const char *full_args, char *dos_args )
{
    LONG        events;

    /* unused parameters */ (void)prog; (void)full_args; (void)dos_args;

    AESTag = AllocateResourceTag(
        (void *)GetNLMHandle(),
        (BYTE *)"Open Watcom Execution Sampler Flush Process",
        AESProcessSignature );

    SampleIndex = 0;
    Suspended = false;
    Resumed = false;
    CurrTick  = 0L;

    EventTag = AllocateResourceTag(
        (void *)GetNLMHandle(),
        (BYTE *)"Open Watcom Execution Sampler Events",
        EventSignature );

    events = RegisterForEventNotification(
        EventTag,
        EVENT_MODULE_UNLOAD,
        EVENT_PRIORITY_APPLICATION,
        NULL,
        WakeMeUp );

    /*
    //  Resolve rate differences will use the restore value to calculate
    //  a new timer if we're running slower than the system runs normally
    */
    ResolveRateDifferences();

    StartTimer();

    if( LoadModule( systemConsoleScreen, (BYTE *)cmd, 0 ) != 0 ) {
        StopTimer();
        cputs( GET_MESSAGE( MSG_SAMPLE_1 ) );
        cputs( cmd );
        cputs( "\r\n" );
        fatal();
    }
    SamplerThread = GetThreadID();
    Suspended = true;
    if( !Resumed ) {
        Suspended = true;
        SuspendThread( SamplerThread );
    }
    WriteRecordedLoads();

    StopTimer();

    UnRegisterEventNotification( events );
    report();

    if( Samples != NULL ) {
        my_free( Samples );
    }
}


static void SaveOutSamples( void *dummy )
{
    /* unused parameters */ (void)dummy;

    StopAndSave();
    AES.AProcessToCall = NULL;
}


void RecordSample( union INTPACK __far *r )
{
    Samples->d.sample.sample[SampleIndex].offset = r->x.eip;
    Samples->d.sample.sample[SampleIndex].segment = r->x.cs;
    ++SampleIndex;
    if( SampleIndex > Margin ) {
        if( AES.AProcessToCall == NULL ) {
            AES.AProcessToCall = SaveOutSamples;
            AES.AWakeUpDelayAmount = 0;
            AES.AWakeUpTime = 0;
            AES.ARTag = (LONG)AESTag;
            ScheduleSleepAESProcessEvent( &AES );
        }
    }
}

/*
 *  These routines will be used to record callgraph information once int 3
 *  hooks are handled (for CommonAddr passing and for marks).  19-aug-92 CMS
 */

void GetCommArea( void )
{
}

void ResetCommArea( void )
{
}

void GetNextAddr( void )
{
}

bool VersionCheck( void )
{
    return( true );
}

int InDOS( void )
{
    return( true );
}

void GetProg( const char *cmd, size_t len )
{
    pgroup2     pg1;
    pgroup2     pg2;

    memcpy( pg2.buffer, cmd, len );
    pg2.buffer[len] = '\0';
    _splitpath2( pg2.buffer, pg1.buffer, NULL, NULL, &pg1.fname, NULL );
    _splitpath2( SampName, pg2.buffer, &pg2.drive, &pg2.dir, &pg2.fname, &pg2.ext );
    if( pg2.fname[0] == '\0' )
        pg2.fname = pg1.fname;
    if( pg2.ext[0] == '\0' )
        pg2.ext = "smp";
    _makepath( SampName, pg2.drive, pg2.dir, pg2.fname, pg2.ext );
}


void fatal( void )
{
    if( Samples != NULL )
        my_free( Samples );
    if( CallGraph != NULL )
        my_free( CallGraph );
    MsgFini();
    exit( 0 );
}

void SysDefaultOptions( void )
{
}

static unsigned long RoundSpeed(unsigned long speed)
{
    unsigned long   modulo = speed % 10;
    if( modulo == 9 )
        speed++;
    return speed;
}

static unsigned long volatile * pRealModeTimerFlag = NULL;

static void EstimateRate( void )
{
    char            EstRate[16];
    unsigned long   currCount;
    unsigned long   hadSwitch;
    unsigned long   modeSwitch;
    unsigned long   i;

    Output("Calculating...\r");

    modeSwitch = RegisterForEventNotification(
        SwitchModeTag,
        5,  /* EVENT_CHANGE_TO_REAL_MODE */
        EVENT_PRIORITY_APPLICATION,
        NULL,
        ModeSwitched );

    if( NULL == pRealModeTimerFlag ) {
        pRealModeTimerFlag = (unsigned long *)ImportSymbol( GetNLMHandle(), "RealModeTimerFlag" );
    }

    if( pRealModeTimerFlag ) {
        while( 0 != *pRealModeTimerFlag ) {
            delay( 100 );
        }
    }

    for( ;; ) {
        nModeSwitched = 0;
        hadSwitch = nModeSwitched;

        currCount = RoundSpeed( cpuspeed() );

        if( hadSwitch == nModeSwitched ) {
            break;
        } else {
            Output( "Mode switched\n" );
        }
    }

    ultoa( currCount, EstRate, 10 );
    Output( "CPU Speed   - " );
    Output( EstRate );
    Output( " Mhz\n" );

    Output( "Calculating...\r" );
    if( pRealModeTimerFlag ) {
        while( 0 != *pRealModeTimerFlag ) {
            delay( 100 );
        }
    }

    currCount = 0;
    for( i = 0; i < 4; i++ ) {
        unsigned long x = count_pit0();
        if( x > currCount ) {
            currCount = x;
        }
    }
    /*
    //  Don't know if this is really true but I have been informed that there is absolutely NO
    //  way to read from the PIT when at initial count. The best we can do is count-2 so add 2
    //  anyway.
    */
    currCount += 2;
    if( currCount > 0x0000FFFF )      /* count is 16 bit - default is 0 (count of 0x10000) */
        currCount = 0;
    ultoa( currCount, EstRate, 10 );
    Output( "PIT Count   - " );
    Output( EstRate );
    Output( "\n" );

    UnRegisterEventNotification( modeSwitch );
}

void SysParseOptions( char c, const char **cmd )
{
    switch( c ) {
    case 'r':
        SetTimerRate( cmd );
        break;
    case 'o':
        SetRestoreRate( cmd );
        break;
    case 'e':
        EstimateRate();
        fatal();
        break;
    default:
        OutputMsgCharNL( MSG_INVALID_OPTION, c );
        fatal();
    }
}

void OutputNL( void )
{
    Output( "\r\n" );
}
