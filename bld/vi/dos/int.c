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
* Description:  interrupt vector save and restore for VI
*
****************************************************************************/


#include "vi.h"
#include <dos.h>
#include <signal.h>
#include <setjmp.h>
#include "win.h"
#include "pragmas.h"
#if defined( __4G__ )
    #define _FAR_   far
    #undef _FAR
    #define _FAR    far
#else
    #define _FAR_
#endif

#if !defined( __386__ ) || defined( __4G__ )

static void (interrupt _FAR_ *oldInt1c)( void );
static void (interrupt _FAR_ *oldInt1b)( void );
static void (interrupt _FAR_ *oldInt23)( void );
static void (interrupt _FAR_ *oldInt24)( void );
#else
typedef struct {
    void far    *prot;
    void        *real;
} int_vect_32;

static int_vect_32      old1c;
static int_vect_32      old1b;
static int_vect_32      old23;
static int_vect_32      old24;
#endif

static char tSec1, tSec2, tMin1, tMin2, tHour1, tHour2;
static char cTick1 = 18, cTick2 = 5;

void __int24_handler( void );
#ifdef __386__
#pragma aux __int24_handler = \
        "mov    al, 3" \
        "iretd" ;
#else
#pragma aux __int24_handler = \
        "mov    al, 3" \
        "iret" ;
#endif

static void _FAR_ HandleInt24( void )
{
    __int24_handler();
}

/*
 * drawClock - draw the clock
 */
static void drawClock( void )
{
    if( EditFlags.ClockActive && EditFlags.Clock ) {
        if( EditFlags.DisplaySeconds ) {
            ClockStart[7 * 2] = tSec2;
            ClockStart[6 * 2] = tSec1;
            ClockStart[5 * 2] = ':';
        }
        ClockStart[4 * 2] = tMin2;
        ClockStart[3 * 2] = tMin1;
        ClockStart[2 * 2] = ':';
        ClockStart[1 * 2] = tHour2;
        ClockStart[0 * 2] = tHour1;
    }

} /* drawClock */

/*
 * handleInt1c - int 0x1c handler (clock timer)
 */
static void interrupt handleInt1c( void )
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
        tSec2++;
        if( tSec2 > '9' ) {
            tSec2 = '0';
            tSec1++;
            if( tSec1 > '5' ) {
                tSec1 = '0';
                tMin2++;
                if( tMin2 > '9' ) {
                    tMin2 = '0';
                    tMin1++;
                    if( tMin1 > '5' ) {
                        tMin1 = '0';
                        tHour2++;
                        if( tHour2 > '9' ) {
                            tHour2 = '0';
                            tHour1++;
                        } else if( tHour2 == '4' && tHour1 == '2' ) {
                            tHour2 = tHour1 = '0';
                        }
                    }
                }
            }
        }
        drawClock();
    }
    if( EditFlags.ClockActive && EditFlags.SpinningOurWheels && EditFlags.Spinning ) {
        *SpinLoc = SpinData[SpinCount];
        SpinCount++;
        if( SpinCount >= 4 ) {
            SpinCount = 0;
        }
    }

#if !defined( __386__ ) || defined( __4G__ )
    _chain_intr( oldInt1c );
#endif

} /* handleInt1c */

/*
 * handleInt1b_23
 */
static void interrupt handleInt1b_23( void )
{
    if( EditFlags.WatchForBreak ) {
        EditFlags.BreakPressed = TRUE;
    }

} /* handleInt1b_23 */

/*
 * setClockTime - set the current clock time
 */
static void setClockTime( void )
{
    char        date[128];

    GetDateTimeString( date );
    tSec1 = date[DATE_LEN - 2];
    tSec2 = date[DATE_LEN - 1];
    tMin1 = date[DATE_LEN - 5];
    tMin2 = date[DATE_LEN - 4];
    tHour1 = date[DATE_LEN - 8];
    tHour2 = date[DATE_LEN - 7];

} /* setClockTime */

#if defined( __386__ ) && !defined( __4G__ )
static bool     noTimer;

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
    drawClock();

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
    segregs.ds = FP_SEG( vinfo->prot );
    inregs.x.edx = FP_OFF( vinfo->prot );
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
    vinfo->prot = MK_FP( segregs.es, outregs.x.ebx );

    inregs.w.ax = 0x2503;   /* get real mode vect */
    inregs.h.cl = vect;
    intdos( &inregs, &outregs );
    vinfo->real = (void *) outregs.x.ebx;

} /* getIntVect */

/*
 * newIntVect - set a new 32-bit interrupt vector
 */
static void newIntVect( int vect, void far *rtn )
{
    union REGS          inregs, outregs;
    struct SREGS        segregs;

    segread( &segregs );

    inregs.w.ax = 0x2506;  /* always gain control in prot. mode */
    inregs.h.cl = vect;
    segregs.ds = FP_SEG( rtn );
    inregs.x.edx = FP_OFF( rtn );
    intdosx( &inregs, &outregs, &segregs );

} /* newIntVect */

extern void LockMemory( void far *, long size );
#pragma aux LockMemory = \
        "push   es" \
        "mov    ax, gs" \
        "mov    es, ax" \
        "mov    ax, 0252bh" \
        "mov    bh, 5" \
        "mov    bl, 1" \
        "int    21h" \
        "pop    es" \
    parm [gs ecx] [edx];

/*
 * setStupid1c - don't set timer tick interrupt in DOS boxes!!?!?!
 */
static void setStupid1c( void )
{
    if( IsWindows() || _osmajor >= 20 ) {
        noTimer = TRUE;
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
#if !defined( __386__ ) || defined( __4G__ )
    oldInt1c = DosGetVect( 0x1c );
    oldInt1b = DosGetVect( 0x1b );
    oldInt23 = DosGetVect( 0x23 );
    oldInt24 = DosGetVect( 0x24 );
#else
    getIntVect( 0x1b, &old1b );
    getIntVect( 0x1c, &old1c );
    getIntVect( 0x23, &old23 );
    getIntVect( 0x24, &old24 );
#endif

    setClockTime();
#if !defined( __386__ ) || defined( __4G__ )
    DosSetVect( 0x1b, handleInt1b_23 );
    DosSetVect( 0x1c, handleInt1c );
    DosSetVect( 0x23, handleInt1b_23 );
    DosSetVect( 0x24, HandleInt24 );
#else
    newIntVect( 0x1b, handleInt1b_23 );
    setStupid1c();
    newIntVect( 0x23, handleInt1b_23 );
    newIntVect( 0x24, HandleInt24 );
#endif

} /* SetInterrupts */

/*
 * RestoreInterrupts - restore interrupts back to normal
 */
void RestoreInterrupts( void )
{
    _disable();
#if !defined( __386__ ) || defined( __4G__ )
    DosSetVect( 0x1c, oldInt1c );
    DosSetVect( 0x1b, oldInt1b );
    DosSetVect( 0x23, oldInt23 );
    DosSetVect( 0x24, oldInt24 );
#else
    resetIntVect( 0x1b, &old1b );
    resetIntVect( 0x1c, &old1c );
    resetIntVect( 0x23, &old23 );
    resetIntVect( 0x24, &old24 );
#endif
    _enable();

} /* RestoreInterrupts */
