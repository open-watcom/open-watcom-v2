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
* Description:  DOS/4G(W) performance sampling core.
*
****************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "sample.h"
#include "smpstuff.h"
#include "wmsg.h"
#include "timermod.h"
#include "rsi1632.h"


static TSF32        Proc;
static opcode_type  BreakOpcode;

static addr48_ptr   CommonAddr = { 0, 0 };

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

bool VersionCheck( void )
{
    return( true );
}

static void RecordSample( unsigned long offset, unsigned short segment )
{
    if( InsiderTime == 1 && SamplerOff == 0 ) {
        LastSampleIndex = SampleIndex;
        if( SampleIndex == 0 ) {
            Samples->pref.tick = CurrTick;
            if( CallGraphMode ) {
                CallGraph->pref.tick = CurrTick;
            }
        }
        ++CurrTick;
        Samples->d.sample.sample[SampleIndex].offset = offset;
        Samples->d.sample.sample[SampleIndex].segment = segment;
        ++SampleIndex;
        ++SampleCount;
        if( CallGraphMode ) {
            RecordCGraph();
        }
    }
}


void GetCommArea( void )
{
    if( CommonAddr.segment == 0 ) {     /* can't get the common region yet */
        Comm.cgraph_top = 0;
        Comm.top_ip = 0;
        Comm.top_cs = 0;
        Comm.pop_no = 0;
        Comm.push_no = 0;
        Comm.in_hook = 1;               /* don't record this sample */
    } else {
        D32DebugRead( &CommonAddr, false, &Comm, sizeof( Comm ) );
    }
}


void GetNextAddr( void )
{
    addr48_ptr  addr;
    struct {
        unsigned long   ptr;
        seg             cs;
        off             ip;
    } stack_entry;

    if( CommonAddr.segment == 0 ) {
        CGraphOff = 0;
        CGraphSeg = 0;
    } else {
        addr.segment = CommonAddr.segment;
        addr.offset = Comm.cgraph_top;
        D32DebugRead( &addr, false, &stack_entry, sizeof( stack_entry ) );
        CGraphOff = stack_entry.ip;
        CGraphSeg = stack_entry.cs;
        Comm.cgraph_top = stack_entry.ptr;
    }
}


void ResetCommArea( void )
{
    if( CommonAddr.segment != 0 ) {     /* reset common variables */
        Comm.pop_no = 0;
        Comm.push_no = 0;
        CommonAddr.offset += 11;
        D32DebugWrite( &CommonAddr, false, &Comm.pop_no, 4 );
        CommonAddr.offset -= 11;
    }
}


void StopProg( void )
{
}

#include "exceptv.h"

#define BSIZE 256

void StartProg( const char *cmd, const char *prog, const char *full_args, char *dos_args )
{
    far_address where;
    addr48_ptr  addr;
    bool        err;
    char        buff[BSIZE];
    addr48_ptr  fp;
    short       initial_cs;
    int         len;

    /* unused parameters */ (void)cmd; (void)dos_args;

    SampleIndex = 0;
    CurrTick  = 0L;

    D32HookTimer( TimerMult );          /* ask for timer - before D32DebugInit!! */
    D32DebugBreakOp( &BreakOpcode );    /* Get the 1 byte break op */

    err = D32DebugInit( &Proc, -1 );
    if( !err ) {
        strcpy( buff, full_args );
        err = ( D32DebugLoad( prog, buff, &Proc ) != 0 );
    }
    if( err ) {
        OutputMsgParmNL( MSG_SAMPLE_2, prog );
        MsgFini();
        exit(1);
    }

    where.offset = 0;
    where.segment = 0;
    WriteCodeLoad( where, ExeName, SAMP_MAIN_LOAD );

    fp.segment = 1;
    fp.offset = 0;
    D32Relocate( &fp );
    WriteAddrMap( 1, fp.segment, fp.offset );

    initial_cs = Proc.cs;
    for( ;; ) {
        D32DebugRun( &Proc );
        if( SampleIndex > Margin && Proc.cs == initial_cs ) {
            StopAndSave();
        }
        if( Proc.int_id == 8 ) {
            ++InsiderTime;
            RecordSample( Proc.eip, Proc.cs );
            --InsiderTime;
        } else if( Proc.int_id == 3 && (Proc.edx & 0xffff) != 0 ) {
            len = 0;                                    /* this is a mark */
            addr.segment = Proc.edx & 0xffff;
            addr.offset = Proc.eax;
            for( ;; ) {
                if( D32DebugRead( &addr, false, &buff[len], 1 ) )
                    break;
                if( len == BSIZE )
                    break;
                if( buff[len] == '\0' )
                    break;
                len++;
                addr.offset++;
            }
            buff[len] = '\0';
            where.segment = Proc.cs;
            where.offset = Proc.eip;
            WriteMark( buff, where );
            Proc.eip++;
        } else if( Proc.int_id == 3 ) {         /* remember common storage */
            CommonAddr.segment = Proc.ecx & 0xffff;            /* area ... */
            CommonAddr.offset = Proc.ebx;
            Proc.eip++;
        } else {
            break;
        }
    }
    D32UnHookTimer();
    if( Proc.int_id != 0x21 ) {
        OutputMsgParmNL( MSG_SAMPLE_1, MsgExcArray[Exceptions[Proc.int_id]] );
    }
    D32DebugTerm();
    report();
}

void SysDefaultOptions( void )
{
}

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
    Output( "\n" );
}
