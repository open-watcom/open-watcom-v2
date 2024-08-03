/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2024 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  interrupt vector save and restore for VI
*
****************************************************************************/


#include "vi.h"
#include <dos.h>
#include <signal.h>
#include <setjmp.h>
#include "win.h"
#include "pragmas.h"
#include "dosclck.h"


#if defined( PHARLAP )
    #define _FAR_
#else
    #define _FAR_   __far
#endif

void __int24_handler( void );
#if defined( _M_I86 )
#pragma aux __int24_handler = \
        "mov  al,3" \
        "iret"
#elif defined( _M_IX86 )
#pragma aux __int24_handler = \
        "mov  al,3" \
        "iretd"
#endif

typedef struct tick_time {
    char_info   _FAR *ClockStart;
    char        tSec1;
    char        tSec2;
    char        tMin1;
    char        tMin2;
    char        tHour1;
    char        tHour2;
} tick_time;

#if defined( PHARLAP )
typedef struct {
    void __far  *prot;
    void        *real;
} int_vect_32;

static int_vect_32      old1c;
static int_vect_32      old1b;
static int_vect_32      old23;
static int_vect_32      old24;

static bool             noTimer;
#else
static void (__interrupt _FAR_ *oldInt1c)( void );
static void (__interrupt _FAR_ *oldInt1b)( void );
static void (__interrupt _FAR_ *oldInt23)( void );
static void (__interrupt _FAR_ *oldInt24)( void );
#endif

static tick_time    tTick;
static char         cTick1 = 18;
static char         cTick2 = 5;

static __declspec( naked) void _FAR_ HandleInt24( void )
{
    __int24_handler();
}

/*
 * drawClock - draw the clock
 */
static void drawClock( tick_time *clk, bool seconds )
{
    if( seconds ) {
        clk->ClockStart[7].cinfo_char = clk->tSec2;
        clk->ClockStart[6].cinfo_char = clk->tSec1;
        clk->ClockStart[5].cinfo_char = ':';
    }
    clk->ClockStart[4].cinfo_char = clk->tMin2;
    clk->ClockStart[3].cinfo_char = clk->tMin1;
    clk->ClockStart[2].cinfo_char = ':';
    clk->ClockStart[1].cinfo_char = clk->tHour2;
    clk->ClockStart[0].cinfo_char = clk->tHour1;
} /* drawClock */

/*
 * handleInt1c - int 0x1c handler (clock timer)
 */
static void __interrupt handleInt1c( void )
{
    ClockTicks++;
    cTick1--;
    if( cTick1 == 0 ) {
        cTick2--;
        if( cTick2 == 0 ) {
            cTick2 = 5;
            cTick1 = 19;
        } else {
            cTick1 = 18;
        }
        tTick.tSec2++;
        if( tTick.tSec2 > '9' ) {
            tTick.tSec2 = '0';
            tTick.tSec1++;
            if( tTick.tSec1 > '5' ) {
                tTick.tSec1 = '0';
                tTick.tMin2++;
                if( tTick.tMin2 > '9' ) {
                    tTick.tMin2 = '0';
                    tTick.tMin1++;
                    if( tTick.tMin1 > '5' ) {
                        tTick.tMin1 = '0';
                        tTick.tHour2++;
                        if( tTick.tHour2 > '9' ) {
                            tTick.tHour2 = '0';
                            tTick.tHour1++;
                        } else if( tTick.tHour2 == '4'
                          && tTick.tHour1 == '2' ) {
                            tTick.tHour2 = tTick.tHour1 = '0';
                        }
                    }
                }
            }
        }
        if( EditFlags.ClockActive && EditFlags.Clock ) {
            drawClock( &tTick, EditFlags.DisplaySeconds );
        }
    }
    if( EditFlags.ClockActive && EditFlags.SpinningOurWheels && EditFlags.Spinning ) {
        SpinLoc->cinfo_char = SpinData[SpinCount];
        SpinCount++;
        if( SpinCount >= 4 ) {
            SpinCount = 0;
        }
    }

#if defined( PHARLAP )
#else
    _chain_intr( oldInt1c );
#endif

} /* handleInt1c */

/*
 * handleInt1b_23
 */
static void __interrupt handleInt1b_23( void )
{
    if( EditFlags.WatchForBreak ) {
        EditFlags.BreakPressed = true;
    }

} /* handleInt1b_23 */

/*
 * setClockTime - set the current clock time
 */
static void setClockTime( void )
{
    char        date[128];

    GetDateTimeString( date );
    tTick.tSec1 = date[DATE_LEN - 2];
    tTick.tSec2 = date[DATE_LEN - 1];
    tTick.tMin1 = date[DATE_LEN - 5];
    tTick.tMin2 = date[DATE_LEN - 4];
    tTick.tHour1 = date[DATE_LEN - 8];
    tTick.tHour2 = date[DATE_LEN - 7];

} /* setClockTime */

#if defined( PHARLAP )

/*
 * UpdateDOSClock - update the clock, if we couldn't hook timer interrupts
 */
void UpdateDOSClock( void )
{
    if( !noTimer ) {
        return;
    }
    setClockTime();
    ClockTicks++;
    if( EditFlags.ClockActive && EditFlags.Clock ) {
        drawClock( &tTick, EditFlags.DisplaySeconds );
    }

} /* UpdateDOSClock */

/*
 * resetIntVect - reset a 32-bit interrupt vector
 */
static void resetIntVect( int vect, int_vect_32 *vinfo )
{
    union REGS          inregs, outregs;
    struct SREGS        segregs;

    segread( &segregs );

    inregs.w.ax = 0x2507;   /* set prot. and real mode vect */
    inregs.h.cl = vect;
    segregs.ds = _FP_SEG( vinfo->prot );
    inregs.x.edx = _FP_OFF( vinfo->prot );
    inregs.x.ebx = (unsigned long) vinfo->real;
    intdosx( &inregs, &outregs, &segregs );

} /* resetIntVect */

/*
 * getIntVect - get a 32-bit interrupt vector
 */
static void getIntVect( int vect, int_vect_32 *vinfo )
{
    union REGS          inregs, outregs;
    struct SREGS        segregs;

    segread( &segregs );

    inregs.w.ax = 0x2502;   /* get prot. mode vect */
    inregs.h.cl = vect;
    intdosx( &inregs, &outregs, &segregs );
    vinfo->prot = _MK_FP( segregs.es, outregs.x.ebx );

    inregs.w.ax = 0x2503;   /* get real mode vect */
    inregs.h.cl = vect;
    intdos( &inregs, &outregs );
    vinfo->real = (void *) outregs.x.ebx;

} /* getIntVect */

/*
 * newIntVect - set a new 32-bit interrupt vector
 */
static void newIntVect( int vect, void __far *rtn )
{
    union REGS          inregs, outregs;
    struct SREGS        segregs;

    segread( &segregs );

    inregs.w.ax = 0x2506;  /* always gain control in prot. mode */
    inregs.h.cl = vect;
    segregs.ds = _FP_SEG( rtn );
    inregs.x.edx = _FP_OFF( rtn );
    intdosx( &inregs, &outregs, &segregs );

} /* newIntVect */

/*
 * setStupid1c - don't set timer tick interrupt in DOS boxes!!?!?!
 */
static void setStupid1c( void )
{
    if( IsWindows() || _osmajor >= 20 ) {
        noTimer = true;
        return;
    }
    newIntVect( 0x1c, handleInt1c );

} /* setStupid1c */

#endif

/*
 * SetInterrupts - set all interrupt handlers
 */
void SetInterrupts( void )
{
#if defined( PHARLAP )
    getIntVect( 0x1b, &old1b );
    getIntVect( 0x1c, &old1c );
    getIntVect( 0x23, &old23 );
    getIntVect( 0x24, &old24 );
#else
    oldInt1c = DosGetVect( 0x1c );
    oldInt1b = DosGetVect( 0x1b );
    oldInt23 = DosGetVect( 0x23 );
    oldInt24 = DosGetVect( 0x24 );
#endif
    setClockTime();
#if defined( PHARLAP )
    newIntVect( 0x1b, handleInt1b_23 );
    setStupid1c();
    newIntVect( 0x23, handleInt1b_23 );
    newIntVect( 0x24, HandleInt24 );
#else
    DosSetVect( 0x1b, handleInt1b_23 );
    DosSetVect( 0x1c, handleInt1c );
    DosSetVect( 0x23, handleInt1b_23 );
    DosSetVect( 0x24, HandleInt24 );
#endif
} /* SetInterrupts */

/*
 * RestoreInterrupts - restore interrupts back to normal
 */
void RestoreInterrupts( void )
{
    _disable();
#if defined( PHARLAP )
    resetIntVect( 0x1b, &old1b );
    resetIntVect( 0x1c, &old1c );
    resetIntVect( 0x23, &old23 );
    resetIntVect( 0x24, &old24 );
#else
    DosSetVect( 0x1c, oldInt1c );
    DosSetVect( 0x1b, oldInt1b );
    DosSetVect( 0x23, oldInt23 );
    DosSetVect( 0x24, oldInt24 );
#endif
    _enable();

} /* RestoreInterrupts */

/*
 * GetClockStart - get clock start position
 */
void GetClockStart( void )
{
    tTick.ClockStart = &Scrn[EditVars.ClockX + EditVars.ClockY * EditVars.WindMaxWidth];

} /* GetClockStart */
