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
* Description:  DOS implementation of clock().
*
****************************************************************************/


#include "variety.h"
#include <time.h>
#include "rtinit.h"

#ifdef __386__

#include "extender.h"

unsigned long   __ibm_biosint_get_ticks( void );
#pragma aux __ibm_biosint_get_ticks =   \
    "mov    ah,0"                       \
    "int    1ah"                        \
    "shl    ecx,16"                     \
    "mov    cx,dx"                      \
    value [ecx] modify exact [eax ecx edx] nomemory;

unsigned long __ibm_bios_get_ticks( void )
{
    volatile unsigned long  *tick_count_ptr;

    // For DOS/4GW and compatibles, read the memory directly. For
    // Nonzero-based DOS/4G executables, PharLap, etc., call the BIOS.
    if( _IsRationalZeroBase() ) {
        // Read tick count from BIOS Data Area
        tick_count_ptr = (unsigned long *)(0x400 + 0x6C);
        return( *tick_count_ptr );
    } else {
        return( __ibm_biosint_get_ticks() );
    }
}

#else

#include <dos.h>

unsigned long __ibm_bios_get_ticks( void )
{
    volatile unsigned short _WCFAR  *tick_count_ptr;
    unsigned short                  lo_word, hi_word;

    tick_count_ptr = (unsigned short _WCFAR *)MK_FP( 0x40, 0x6c );
    // Read tick count in a loop to ensure the value is consistent
    do {
        lo_word = tick_count_ptr[0];
        hi_word = tick_count_ptr[1];
    } while( lo_word != tick_count_ptr[0] );
    return( ((unsigned long)hi_word << 16) | lo_word );
}

#endif

static unsigned long    init_milliseconds;

static unsigned long __get_tick( void )
{
    static unsigned long    prev_count = 0;
    static unsigned long    tick_offset = 0;
    unsigned long           tick_count;

    tick_count = __ibm_bios_get_ticks();

    // Convert ~18.2 Hz clock to milliseconds; The root frequency is
    // 1.19318 MHz, with divider of 65536 this works out to about
    // 54.9255 milliseconds per tick
    tick_count *= 55;
    tick_count -= tick_count / 738;

    // Check for counter wraparound (midnight)
    if( tick_count < prev_count )
       tick_offset += 24ul * 60 * 60 * 1000;    // millseconds in a day

    prev_count = tick_count;

    return( tick_offset + tick_count );
}

/* The system millisecond counter will wrap after ~49 days. This
 * is is not considered a real problem.
 */
_WCRTLINK clock_t clock( void )
{
    return( (clock_t)(__get_tick() - init_milliseconds) );
}

static void __clock_init( void )
{
    init_milliseconds = __get_tick();
}

AXI( __clock_init, INIT_PRIORITY_LIBRARY )
