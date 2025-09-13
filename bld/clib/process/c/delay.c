/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2025 The Open Watcom Contributors. All Rights Reserved.
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
#include "tinyio.h"


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
    tiny_time_t     clk;
    long            start;
    long            next;
    short           res;
    unsigned char   o_hsec;

    if( milliseconds == 0 )
        return;

    clk = TinyGetTime();

    start = ( (clk.hour * 60L * 60L) +
              (clk.minutes * 60L) +
               clk.seconds ) * 1000L +
               clk.hundredths * 10L;

//    printf( "hour=%u, min=%u, sec=%u, hsec=%u, start=%lu\n",
//             (unsigned)clk.hour, (unsigned)clk.minutes, (unsigned)clk.seconds, (unsigned)clk.hundredths, start );

    res = 0;
    next = start;
    while( next < (start + milliseconds - res) ) {
        o_hsec = clk.hundredths;
        clk = TinyGetTime();
        res = clk.hundredths - o_hsec;  /* often this evaluates to 0 which is OK */
        /* if we wrapped past 99 add 100 e.g., 2 - 98 + 100 = 4 */
        if( res < 0 ) {
            res += 100;
        }
        /* cancel any wacky variation in resolution due to OS scheduling delays */
        if( res > 10 ) {
            res = 0;
        } else if( res > 1 ) {  /* 5/100 or 6/100 goes to 25/1000 or 30/1000 */
            /* convert res to milliseconds and divide by 2 for average (*10/2) */
            res *= 10 / 2;
        } else if( res > 0 ) {  /* 1/100 goes to 0/100 */
            res = 0;
        }
        next = ( (clk.hour * 60L * 60L) +
                 (clk.minutes * 60L) +
                  clk.seconds ) * 1000L +
                  clk.hundredths * 10L;
        /* check for midnight roll-over */
        if( next < start ) {
            next += 24L * 60L * 60L * 1000L;
        }
    }
//    printf( "hour=%u, min=%u, sec=%u, hsec=%u, next=%lu, res=%d\n\n",
//             (unsigned)clk.hour, (unsigned)clk.minutes, (unsigned)clk.seconds, (unsigned)clk.hundredths, next, (int)res );
}

/*
    The following is code that can be used to test the algorithm
 */

/********************************
void delay_tst( unsigned amount )
{
    tiny_time_t     clk_1;
    tiny_time_t     clk_2;

    clk_1 = TinyGetTime();
    delay( amount );
    clk_2 = TinyGetTime();

    printf( "hour=%u, min=%u, sec=%u, hsec=%u, amount=%u\n",
             (unsigned)clk1.hour, (unsigned)clk1.minutes, (unsigned)clk1.seconds, (unsigned)clk1.hundredths, amount );

    printf( "hour=%u, min=%u, sec=%u, hsec=%u, amount=%u\n\n",
             (unsigned)clk2.hour, (unsigned)clk2.minutes, (unsigned)clk2.seconds, (unsigned)clk2.hundredths, amount );
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
