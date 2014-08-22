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

#if 0
#include "vi.h"
#include <dos.h>
#include <signal.h>
#include <setjmp.h>
#include "win.h"
#if defined( __4G__ )
    #define _FAR_   __far
#else
    #define _FAR_
#endif

#if defined( _M_I86 ) || defined( __4G__ )
static void (__interrupt _FAR_ *oldInt1c)();
static void (__interrupt _FAR_ *oldInt1b)();
static void (__interrupt _FAR_ *oldInt23)();
static void (__interrupt _FAR_ *oldInt24)();
#else
typedef struct {
    void __far  *prot;
    void        *real;
} int_vect_32;

static int_vect_32      old1c;
static int_vect_32      old1b;
static int_vect_32      old23;
static int_vect_32      old24;
#endif

static char tSec1, tSec2, tMin1, tMin2, tHour1, tHour2;
static char cTick1 = 18, cTick2 = 5;

extern void HandleInt24( void );


/*
 * drawClock - draw the clock
 */
static void drawClock( void )
{
    if( EditFlags.ClockActive && EditFlags.Clock ) {
        ClockStart[7].cinfo_char = tSec2;
        ClockStart[6].cinfo_char = tSec1;
        ClockStart[5].cinfo_char = ':';
        ClockStart[4].cinfo_char = tMin2;
        ClockStart[3].cinfo_char = tMin1;
        ClockStart[2].cinfo_char = ':';
        ClockStart[1].cinfo_char = tHour2;
        ClockStart[0].cinfo_char = tHour1;
    }

} /* drawClock */

/*
 * handleInt1c - int 0x1c handler (clock timer)
 */
static void __interrupt handleInt1c()
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
        SpinLoc->cinfo_char = SpinData[SpinCount];
        SpinCount++;
        if( SpinCount >= 4 ) {
            SpinCount = 0;
        }
    }

#if defined( _M_I86 ) || defined( __4G__ )
    _chain_intr( oldInt1c );
#endif

} /* handleInt1c */

/*
 * handleInt1b_23
 */
static void __interrupt handleInt1b_23()
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

    GetDateString( date );
    tSec1 = date[DATE_LEN - 2];
    tSec2 = date[DATE_LEN - 1];
    tMin1 = date[DATE_LEN - 5];
    tMin2 = date[DATE_LEN - 4];
    tHour1 = date[DATE_LEN - 8];
    tHour2 = date[DATE_LEN - 7];

} /* setClockTime */

#if !defined( _M_I86 ) && !defined( __4G__ )
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
static void newIntVect( int vect, void __far *rtn )
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

extern void LockMemory( void __far *, long size );
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
#if defined( _M_I86 ) || defined( __4G__ )
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
#if defined( _M_I86 ) || defined( __4G__ )
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
#if defined( _M_I86 ) || defined( __4G__ )
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
#endif

void SetInterrupts( void ) {}
void RestoreInterrupts( void ) {}

