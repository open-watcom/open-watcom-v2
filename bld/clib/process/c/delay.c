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

/*
 -------------------------------------------
 Tossed old algorithm and wrote new one.
 No longer based on delay loop iteration.
 No initialization delay. Works on
 multi-processing operating systems (even DOS).
 Minimum delay is 50-60 milliseconds.
 Seems to be accurate to within 10 milliseconds.
 -------------------------------------------
 
   The trouble with the old code is that in a multitasking OS, the
 initial calibration may be wildly inaccurate! That's why the algorithm
 was scrapped.
*/ 

#include "variety.h"
#include <stdio.h>
#include <i86.h>

/*
 * Using interupt 21 service 2C, we get the time from DOS
 * in CX and DX as follows:
 *
 *      CH = hours
 *      CL = minutes
 *      DH = seconds
 *      DL = seconds / 100 (but not hundredths accuracy!)
 */
#ifdef __386__
    extern unsigned long GetDosTime(void);
    #pragma aux GetDosTime =    \
        "mov    ah,2ch"         \
        "xor    edx,edx"        \
        "int    21h"            \
        "mov    eax,edx"        \
        "sal    eax,16"         \
        "or     ax,cx"          \
        value [eax]             \
        modify [eax ecx edx];
#else
    extern unsigned long GetDosTime(void);
    #pragma aux GetDosTime =    \
        "mov    ah,2ch"         \
        "int    21h"            \
        "mov    ax,cx"          \
        value [dx ax]           \
        modify [ax cx dx];
#endif


struct dos_clock_b {
    unsigned char min;      /* minutes (0-59) */
    unsigned char hour;     /* hours (0-23) */
    unsigned char hsec;     /* hundredths (0-99) but resolution is less */
    unsigned char sec;      /* seconds (0-59) */
};

struct dos_clock_w {
    unsigned short h_m;
    unsigned short s_h;
};

union dos_clock {
    struct dos_clock_b h;
    struct dos_clock_w w;
};


/*
    Under DOS, the resolution of the clock seems to be about 5/100 to
    6/100 of a second. This means that the hundredths of a second
    reported by DOS jumps by 5/100 to 6/100 whenever it ticks over.
    The old algorithm tried to determine a suitable iteration count
    that would equate to a delay of 1 millisecond. It tried to
    determine this count in 10/100 second but the resolution is 5/100
    to 6/100 so wildly varying counts would be calculated. This just
    didn't work with a clock resolution that was so poor and certainly
    doesn't work on a multi-tasking OS where other factors can slow
    down execution resulting in a poor choice for an iteration count.

    A simple case is DOS, where a program was run from floppy disk.
    The iteration count during floppy spin down was much lower than
    when the floppy had stopped spinning.

    The revised algorithm converts the time of day into a millisecond
    value and then loops while the current time of day (in
    milliseconds) is less than the initial time + the delay desired.
    It accounts for the roll-over at midnight.

    Notes:
        Some NEC PC98 models have a broken hundredth of a second
        clock. The hundredths are always reported as 0.
 */
/*
    Claims that should be verified by Softboat
    ------------------------------------------

    On some NEC machines, DL contains the number of milliseconds with
    the oddity that it only counts from 0 to 99 and then sits on 99
    for the remaining 9/10th of a second.
 */

_WCRTLINK void delay( unsigned milliseconds )
{
    union dos_clock clk;
    unsigned long time;
    signed long start;
    signed long next;
    signed short res;
    unsigned char o_hsec;

    if( milliseconds == 0 ) return;

    time = GetDosTime();
    clk.w.h_m = time;
    clk.w.s_h = time >> 16;

    start = ( (clk.h.hour * 60L * 60) +
              (clk.h.min * 60) +
               clk.h.sec ) * 1000 +
               clk.h.hsec * 10;

//  printf( "clk.h.hour=%u, clk.h.min=%u, clk.h.sec=%u, clk.h.hsec=%u, start=%lu\n",
//           clk.h.hour, clk.h.min, clk.h.sec, clk.h.hsec, start );

    res = 0;
    next = start;
    while( next < (start + milliseconds - res) ) {
        o_hsec = clk.h.hsec;
        time = GetDosTime();
        clk.w.h_m = time;
        clk.w.s_h = time >> 16;
        res = clk.h.hsec - o_hsec;  /* often this evaluates to 0 which is OK */
        /* if we wrapped past 99 add 100 e.g., 2 - 98 + 100 = 4 */
        if( res < 0 ) {
            res += 100;
        };
        /* cancel any wacky variation in resolution due to OS scheduling delays */
        if( res > 10 ) {
            res = 0;
        } else if( res > 1 ) {  /* 5/100 or 6/100 goes to 25/1000 or 30/1000 */
            /* convert res to milliseconds and divide by 2 for average (*10/2) */
            res *= 10 / 2;
        } else if( res > 0 ) {  /* 1/100 goes to 0/100 */
            res = 0;
        }
        next = ( (clk.h.hour * 60L * 60) +
                 (clk.h.min * 60) +
                  clk.h.sec ) * 1000 +
                  clk.h.hsec * 10;
        /* check for midnight roll-over */
        if( next < start ) next += 24 * 60L * 60 * 1000;
    }
//  printf( "clk.h.hour=%u, clk.h.min=%u, clk.h.sec=%u, clk.h.hsec=%u, next=%lu, res=%d\n\n",
//           clk.h.hour, clk.h.min, clk.h.sec, clk.h.hsec, next, res );
}

/*
    The following is code that can be used to test the algorithm
 */

/********************************
void delay_tst( unsigned amount )
{
    unsigned long t_1;
    unsigned long t_2;
    union dos_clock clk;

    t_1 = GetDosTime();
    delay( amount );
    t_2 = GetDosTime();

    clk.w.h_m = t_1;
    clk.w.s_h = t_1 >> 16;

    printf( "clk.h.hour=%u, clk.h.min=%u, clk.h.sec=%u, clk.h.hsec=%u, amount=%u\n",
             clk.h.hour, clk.h.min, clk.h.sec, clk.h.hsec, amount );

    clk.w.h_m = t_2;
    clk.w.s_h = t_2 >> 16;

    printf( "clk.h.hour=%u, clk.h.min=%u, clk.h.sec=%u, clk.h.hsec=%u, amount=%u\n\n",
             clk.h.hour, clk.h.min, clk.h.sec, clk.h.hsec, amount );
}
void main()
{
    int i;
    unsigned current;

    printf( "delay test start\n" );
    for( i = 0; i<3; i++ )
    {
        delay_tst( 10 );
    }
    for( i = 0; i<3; i++ )
    {
        delay_tst( 50 );
    }
    for( i = 0; i<3; i++ )
    {
        delay_tst( 100 );
    }
    for( i = 0; i<3; i++ )
    {
        delay_tst( 500 );
    }
    for( i = 0; i<5; i++ )
    {
        delay_tst( 1000 );
    }
    for( i = 0; i<5; i++ )
    {
        delay_tst( 10000 );
    }
    printf( "delay test end\n" );

}
********************************/
