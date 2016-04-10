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
* Description:  PC style timer interrupt hook.
*
****************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <io.h>
#include <ctype.h>
#include <string.h>
#include <process.h>
//#include <malloc.h>
#include <conio.h>
#include <i86.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "sample.h"
#include "smpstuff.h"
#include "intrptr.h"
#include "os.h"
#include "timermod.h"
#include "interc.h"
#include "indos.h"
#include "rmhooks.h"
#include "hooks.h"
#include "realmode.h"


#define _CHAIN_TO( x ) (*x)()

extern void             StopAndSave( void );

#ifdef __NETWARE__
static int              Save_Request = FALSE;
#else
int                     Save_Request = FALSE;
#endif
static intrptr          old_timer_handler;

unsigned NextThread( unsigned tid )
{
    return( !tid );
}

/*
    timer_handler is the main working component of this program.
    A PC (or PS/2) has a built-in timer that keeps interrupting processor
    in regular intervals and causes it to execute INT 08H. We substitute
    the regular routine that would be then called, by changing its FAR_PTR pointer
    stored at &INT_LOCATE( 0x08 ). The previous value is saved as
    old_timer_handler to be used from withing our handler so that nothing is
    changed to the function of the interrupt (which is normaly used to update
    tics counter which in turn is essential to operation of some system io
    routines).  Since the hardware timer frequency is modified to suit our
    purposes, a TimerMod is maintained to assure the old_timer_handler is
    invoked with its original frequency.

    Furthermore, since the interrupt is of hardware type, it involves mediation
    of 8259 chip. This chip expects being told when the interrupt processing is
    over so that it may allow subsequent hard interrupts to be acknowledged.
    'old_timer_handler' does that by itself but if it isn't called, an explicit
    outp( INT_CTRL, EOI ) instruction has to be issued. This EOI (End Of
    Interrupt) must be signaled before any call to INT 21H is made (i.e.
    before any io is requested).
    The above was mentionned so that you have an idea of the reasons behind
    the order relationship of instructions in the following procedure.
    NOTE: if you rid this function of all the comment then it really is short
    and things start making sense.
*/

static void __interrupt __far timer_handler( union INTPACK r )
{
    if( --TimerMod == 0 ) {
        TimerMod = TimerMult;
        _CHAIN_TO( old_timer_handler );
    } else {
        /* end of interrupt (expected by 8259 before you do RETI) */
        outp( INT_CTRL, EOI );
    }

    if( !SamplerOff ) {
        if( InsiderTime == 0 ) {
            ++InsiderTime;
            if( SampleIndex == 0 ) {
                Samples->pref.tick = CurrTick;
                if( CallGraphMode ) {
                    CallGraph->pref.tick = CurrTick;
                }
            }
            ++CurrTick;
            RecordSample( &r );
            if( SampleIndex >= Margin ) {
                if( InDOS() ) {
                    Save_Request = TRUE;
                } else {
                    /*
                        We are not in DOS so we can suspend things for a while
                        and save our block of samples
                    */
                    if( Save_Request ) {
                        Save_Request = FALSE;
                    }
                    StopAndSave();
                }
                if( SampleIndex >= Ceiling ) {
                    if( CallGraphMode ) {
                        --SampleCount;
                        SampleIndex = LastSampleIndex;
                    } else {
                        --SampleIndex;
                    }
                    LostData = TRUE;
                }
            }
            --InsiderTime;
        }
    }
}


/*
    Following function doesn't start the timer per say. It makes it go through
    our own timer interrupt handler.
*/
extern short GetCS( void );
#pragma aux GetCS = 0x8c 0xc8;

void StartTimer( void )
{
    TimerMod = TimerMult;
    old_timer_handler = HookTimer( MK_FP( GetCS(), (unsigned)&timer_handler ) );

    /*
    //  Only access the clock if we have overridden the default
    */
    if( DEF_MULT != TimerMult ) {
        outp( TIMER0, DIVISOR & 0xff );
        outp( TIMER0, DIVISOR >> 8 );
    }
}


void StopTimer( void )  /* undo */
{
    /*
    //  Bit worried that NetWare 5 or 6 could interrupt here when switching to
    //  real mode and the timer could get screwed. CLI?
    */
    outp( TIMER0, TimerRestoreValue & 0xFF );
    outp( TIMER0, (TimerRestoreValue >> 8) & 0xFF );
    HookTimer( old_timer_handler );
}
