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


#ifndef _P5TIMER_HPP
#define _P5TIMER_HPP

/****************************************************
 *
 * P5Timer
 *
 * Usage:
 *
 * #include "p5timer.hpp"
 *
 * void foo() {
 *     P5Timer  timer;                  // implicitly does a Start()
 *     ...
 *     timer.Restart();
 *     ... expensive stuff ...
 *     cout << timer.Time() << endl;    // time in clock cycles
 * }
 *
 * Improvements:
 *   - correct for rdtsc overhead
 *   - determine the process clock speed
 *
 ****************************************************/

#include <limits.h>

#ifndef __WATCOM_INT64__
#  error 'You must be using a compiler that supports __int64'
#endif

unsigned __int64 P5Timer_ReadTSC();
#pragma aux     P5Timer_ReadTSC =   \
        ".586"                      \
        "rdtsc"                     \
        value [eax edx];



typedef unsigned __int64 p5time_t;

class P5Timer {
    public:
        P5Timer() {
            _start = P5Timer_ReadTSC();
        }

        P5Timer( const P5Timer & o )
            : _start( o._start )
        {}

        P5Timer & operator=( const P5Timer & o ) {
            _start = o._start;
            return *this;
        }

        //
        // Restart
        //
        //  Restart the timer from 0

        void Restart() {
            _start = P5Timer_ReadTSC();
        }

        //
        // Time
        //
        //  Return the time in since the most recent Restart.

        p5time_t Time() {
            return( P5Timer_ReadTSC() - _start );
        }

        //
        // Now
        //
        //  Return the current processor tick count

        static inline p5time_t Now() {
            return( P5Timer_ReadTSC() );
        }


    private:
        p5time_t        _start;
};

#endif // _P5TIMER_HPP
