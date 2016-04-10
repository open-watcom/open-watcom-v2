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
* Description:  PharLap performance sampling core.
*
****************************************************************************/


#define WATCOM 1
#define CMPLR WATCOM
#define __STKCALL __cdecl
#include <stdio.h>
#include <stdlib.h>
#include <io.h>
#include <dos.h>
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
#include "pltypes.h"
#include "hw386.h"
#include "dilintf.h"
#include "dilproto.h"
#include "timermod.h"
#include "timer.h"
#include "samppls.h"


unsigned short          InitialCS;
unsigned short          InitialSS;
unsigned char           CGBreak;
unsigned char           FakeBreak;
unsigned char           SavedByte;
unsigned char           XVersion;

static MSB              Mach;
static SEL_REMAP        SelBlk;
static PTR386           CommonAddr = { 0, 0 };
static unsigned char    RateChanged;

unsigned NextThread( unsigned tid )
{
    return( !tid );
}

int VersionCheck( void )
{
    return( TRUE );
}

void RecordSample( unsigned offset, unsigned short segment )
{
    if( InsiderTime == 1 && SamplerOff == 0 ) {
        LastSampleIndex = SampleIndex;
        if( SampleIndex == 0 ) {
            Samples->pref.tick = CurrTick;
            CallGraph->pref.tick = CurrTick;
        }
        ++CurrTick;
        Samples->d.sample.sample[SampleIndex].offset = offset;
        Samples->d.sample.sample[SampleIndex].segment = segment;
        ++SampleIndex;
        ++SampleCount;
    }

/*
 *  Call Graph information still has to be written.  If this sample occurred
 *  in the program code (as opposed to OS code which might be called from
 *  the program), and int 3 is placed at the interrupt return address.  This
 *  again triggers the sampler and RecordCGraph() is called.  If the sample
 *  occurred elsewhere, we are unable to get access to the current callgraph
 *  stack, so a dummy callgraph record is written.  The code doing this is
 *  found in timer.asm, and is executed immediately upon returning from
 *  RecordSample().
 */
}


void DummyCGraph( void )
{
    Samples->d.sample.sample[SampleIndex].offset = 0;
    Samples->d.sample.sample[SampleIndex].segment = -1;
    ++SampleIndex;
}


void GetCommArea( void )
{
    if( CommonAddr.selector == 0 ) {    /* can't get the common region yet */
        Comm.cgraph_top = 0;
        Comm.top_ip = 0;
        Comm.top_cs = 0;
        Comm.pop_no = 0;
        Comm.push_no = 0;
        Comm.in_hook = 1;       /* don't record this sample */
    } else {
        dbg_pread( &CommonAddr, sizeof( Comm ), (UCHAR *)&Comm );
    }
}

void ResetCommArea( void )
{
    if( CommonAddr.selector != 0 ) {    /* reset common variables */
        Comm.pop_no = 0;
        Comm.push_no = 0;
        CommonAddr.offset += 11;
        dbg_pwrite( &CommonAddr, 4, (UCHAR *)&Comm.pop_no );
        CommonAddr.offset -= 11;
    }
}


void GetNextAddr( void )
{
    PTR386      addr;
    struct {
        unsigned long   ptr;
        seg             cs;
        off             ip;
    } stack_entry;

    if( CommonAddr.selector == 0 ) {
        CGraphOff = 0;
        CGraphSeg = 0;
    } else {
        addr.selector = CommonAddr.selector;
        addr.offset = Comm.cgraph_top;
        dbg_pread( &addr, sizeof( stack_entry ), (UCHAR *)&stack_entry );
        CGraphOff = stack_entry.ip;
        CGraphSeg = stack_entry.cs;
        Comm.cgraph_top = stack_entry.ptr;
    }
}


static void check( int x )
{
    if( x != 0 ) {
        Output( MsgArray[MSG_SAMPLE_1 - ERR_FIRST_MESSAGE] );
        Output( "\r\n" );
        MsgFini();
        _exit( -1 );
    }
}

void StopProg( void )
{
}

#include "exceptv.h"

#define BSIZE 256

static int bcd2hex( int i )
{
    return( ( i & 0xf ) + ( ( ( i >> 4 ) & 0xf ) * 10 ) );
}


static void FixTime( void )
{
    int hour,min,sec,count;

    if( !RateChanged )
        return;
    ReadRealClk( &hour, &min, &sec );
    hour = bcd2hex( hour );
    min  = bcd2hex( min );
    sec  = bcd2hex( sec );
    count = ( hour * 65543 )
          + ( hour / 3 )
          + ( min * 1092 )
          + ( min / 3 )
          + ( sec * 18 )
          + ( sec / 5 );
    SetBiosClk( count );
}

void StartProg( char *cmd, char *prog, char *full_args, char *dos_args )
{
    PTR386      addr;
    char        buff[BSIZE];
    int         len;
    seg_offset  where;

    cmd = cmd;
    SampleIndex = 0;
    CurrTick  = 0L;
    FakeBreak = 0;
    CGBreak = 0;

    TimerMod = TimerMult;
    check( dbg_init( &SelBlk ) );
    check( dbg_edebug() );
    GrabVects();
    FixTime();
    if( dbg_load( prog, NULL, dos_args ) != 0 ) {
        Output( MsgArray[MSG_SAMPLE_2 - ERR_FIRST_MESSAGE] );
        Output( prog );
        Output( "\r\n" );
        ReleVects();
        MsgFini();
        _exit( -1 );
    }
    check( dbg_rdmsb( &Mach ) );
    where.offset = 0;
    where.segment = 0;
    WriteCodeLoad( where, ExeName, SAMP_MAIN_LOAD );
    WriteAddrMap( 1, Mach.msb_cs, 0 );
    InitialCS = Mach.msb_cs;
    InitialSS = Mach.msb_ss;
    outp( TIMER0, DIVISOR & 0xff );
    outp( TIMER0, DIVISOR >> 8 );
    for( ;; ) {
        check( dbg_go() );
        check( dbg_rdmsb( &Mach ) );
        if( Mach.msb_event == EV_SLBA )
            continue;
        FixTime();
        if( Mach.msb_event != EV_BKPT )
            break;
        if( FakeBreak || CGBreak ) {
            if( CGBreak ) {
                RecordCGraph();
            }
            if( FakeBreak ) {
                StopAndSave();
            }
            Mach.msb_eip--;
            dbg_wrmsb( &Mach );
            addr.selector = Mach.msb_cs;
            addr.offset = Mach.msb_eip;
            dbg_pwrite( &addr, 1, &SavedByte );
            FakeBreak = 0;
        } else if( (Mach.msb_edx & 0xffff) != 0 ) {    /* this is a mark */
            len = 0;
            addr.selector = Mach.msb_edx & 0xffff;
            addr.offset = Mach.msb_eax;
            for( ;; ) {
                dbg_pread( &addr, 1, (unsigned char *)buff + len );
                if( len == BSIZE )
                    buff[len] = '\0';
                if( buff[len] == '\0' )
                    break;
                ++len;
                addr.offset++;
            }
            where.segment = Mach.msb_cs;
            where.offset = Mach.msb_eip;
            WriteMark( buff, where );
        } else {                                /* remember common storage */
            CommonAddr.selector = Mach.msb_ecx & 0xffff; /* area ... */
            CommonAddr.offset = Mach.msb_ebx;
        }
    }
    outp( TIMER0, 0 );
    outp( TIMER0, 0 );
    FixTime();
    if( Mach.msb_event <= 16 ) {
        Output( MsgArray[MSG_SAMPLE_3 - ERR_FIRST_MESSAGE] );
        Output( MsgArray[Exceptions[Mach.msb_event]+MSG_EXCEPT_0 - ERR_FIRST_MESSAGE] );
        Output( "\r\n" );
    } else if( Mach.msb_event != EV_TERM ) {
        char buff[10];
        Output( MsgArray[MSG_SAMPLE_4 - ERR_FIRST_MESSAGE] );
        Output( itoa( Mach.msb_event, buff, 10 ) );
        Output( "\r\n" );
    }
    ReleVects();
    dbg_exit();
    report();
}

void SysDefaultOptions( void )
{
    RateChanged = 0;
}

void SysParseOptions( char c, char **cmd )
{
    char buff[2];

    switch( c ) {
    case 'r':
        RateChanged = 1;
        SetTimerRate( cmd );
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
