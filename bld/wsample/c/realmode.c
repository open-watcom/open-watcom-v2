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
#include <io.h>
#include <ctype.h>
#include <string.h>
#include <process.h>
//#include <malloc.h>
#include <conio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "sample.h"
#include "smpstuff.h"
#include "intrptr.h"
#include "os.h"
#define DEFVARS
#include "timermod.h"


intrptr                 old_timer_handler;

unsigned                Save_Request = FALSE;


extern int              InDOS(void);
/*
 Located int SAMPLE.C
*/
extern void             StopAndSave();
extern intrptr          HookTimer(intrptr);

extern void             RecordSample( union INTPACK FAR_PTR *r );

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

#if defined(_NEC_PC) || defined(_FMR_PC)
    union REGPACK R;
#endif

#if defined(_FMR_PC)
    struct timer_block {
        unsigned short  timer_mode;
        void far *      timer_proc;
        unsigned long   timer_cycle;
    } TimerParmBlock;

    char                TimerMgmtNum;
#endif

#if defined(_NEC_PC)
void interrupt far dummy_rtn()
{
}
#endif

#if defined(_NEC_PC)
    #define INTR interrupt
#elif defined(_FMR_PC)
    #define INTR
#else
    #define INTR interrupt
#endif

void INTR far timer_handler( union INTPACK r )
{

#if defined(_FMR_PC)
    /* nothing special needed */
#elif defined(_NEC_PC)
    /* do a non-specific end-of-interrupt */
    outp( INT_CTRL, EOI );
    /*
     * On the NEC PC, this interrupt occurs every 10 msec.
     * TimerMult > 1 means that samples are taken at some multiple
     * of 10 msec.
     */
    if( --TimerMod == 0 ) {
        TimerMod = TimerMult;
#else
    if( --TimerMod == 0 ) {
        TimerMod = TimerMult;
        _CHAIN_TO( old_timer_handler );
    } else {
        /* end of interrupt (expected by 8259 before you do RETI) */
        outp( INT_CTRL, EOI );
    }
#endif

    if( ! SamplerOff ) {
        if( InsiderTime == 0 ) {
            ++InsiderTime;
            if( SampleIndex == 0 ) {
                Samples->pref.tick = CurrTick;
                if( CallGraphMode ) {
                    CallGraph->pref.tick = CurrTick;
                }
            }
            ++CurrTick;
            #ifdef NETWARE
                /* avoid pointer truncation warning */
                RecordSample( (union INTPACK *)&r );
            #else
                RecordSample( &r );
            #endif
            if( SampleIndex >= Margin ) {
                if( InDOS() ) {
                    Save_Request = TRUE;
                } else {
                    /*
                        We are not in DOS so we can suspend things for a while
                        and save our block of samples
                    */
                    if( Save_Request ) {
                        Save_Request = 0;
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
#if defined(_NEC_PC)
    }
#endif
}


/*
    Following function doesn't start the timer per say. It makes it go through
    our own timer interrupt handler.
*/
extern short GetCS(void);
#pragma aux GetCS = 0x8c 0xc8;

void StartTimer()
{
#if defined(_FMR_PC)
    TimerParmBlock.timer_mode = 0x0000; /* INTERVAL MODE, INTERRUPT MODE */
    TimerParmBlock.timer_proc = &timer_handler;
    TimerParmBlock.timer_cycle = TimerMult;
    R.h.ah = 0x00;              /* register interval timer */
    R.w.bx = FP_OFF( TimerParmBlock );
    R.w.ds = FP_SEG( TimerParmBlock );
    intr( 0x97, &R );
    TimerMgmtNum = R.h.ah;
#else
    TimerMod = TimerMult;
    old_timer_handler = HookTimer( MK_FP( GetCS(), (int)&timer_handler ) );
  #if defined(_NEC_PC)
    R.h.ah = 0x02;              /* set interval timer */
    R.w.cx = 0;                 /* duration to maximum */
    R.w.bx = FP_OFF( dummy_rtn );
    R.w.es = FP_SEG( dummy_rtn );
    intr( 0x1c, &R );
  #else
    outp( TIMER0, DIVISOR & 0xff );
    outp( TIMER0, DIVISOR >> 8 );
  #endif
#endif
}


void StopTimer()    /* undo */
{
#if defined(_FMR_PC)
    R.h.ah = 0x01;              /* cancel interval timer */
    R.h.al = TimerMgmtNum;
    intr( 0x97, &R );
    TimerMgmtNum = R.h.ah;
#elif defined(_NEC_PC)
    unsigned int i;

    _disable();
    /* disable interrupts from the timer */
    i = inp( INT_MASK ) | 0x01;
    outp( INT_MASK, i );
    _enable();
    /* cancel the timer */
    R.h.ah = 0x03;
    R.w.bx = FP_OFF( dummy_rtn );
    R.w.es = FP_SEG( dummy_rtn );
    intr( 0x1c, &R );
    HookTimer( old_timer_handler );
#else
    outp( TIMER0, 0 );
    outp( TIMER0, 0 );
    HookTimer( old_timer_handler );
#endif
}
