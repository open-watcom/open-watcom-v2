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
* Description:  Entries to control exception searching.
*
****************************************************************************/

/*

These entries are allocated on the stack by the run-time system.  They are
linked, in definition order, from the thread data area (exc_pr).

Their purpose is control searching and error detection while searching
the exceptions R/W blocks.  Essentially, they represent an additional
thread through some of the active R/W blocks, recording significant actions
that are in progress:
    - function exception specification failure
        - unexpected called
        - bad_exception thrown
    - construction of exception in allocation area
    - destruction during stack unwind
    - terminate called

The constructor links in the entry; the destructor will unlink it during
stack unwinding.

unexpected processing
---------------------
- only when user-defined routine to be called
- unexpected r/t routine links in an fnexc entry
- if we search to that point on a subsequent throw, then status is set to
  avoid examining R/W blocks until the point of the original fn-exc is reached

other processing
----------------
- designed to catch exceptions leaking out in bad circumstances
    - after unexpected (see above), terminate
    - dtor during stack unwind
    - ctor during copy to allocation area
- if we search to that point on a subsequent throw, special error handling
  will occur

*/

#include "cpplib.h"
#include "rtexcept.h"
#include "exc_pr.h"

//***********************************************************************
// Implementation of _EXC_PR
// - basic entry
// - used as base for _EXC_PR_FNEXC, _EXC_PR_EXC
//***********************************************************************

_EXC_PR::_EXC_PR                // _EXC_PR CONSTRUCTOR
    ( _RTCTL* rtc               // - run-time control
    , RW_DTREG *rw              // - current read/write
    , EXCSTATE state )          // - state
    : _state( state )
    , _rw( rw )
    , _prev( rtc->thr->exc_pr )
    , _rtc( rtc )
#ifndef NDEBUG
    , _type( EXCPR_BASIC )
#endif
{
    rtc->thr->exc_pr = this;
    if( 0 == rw ) {
#ifdef RW_REGISTRATION
        _rw = FsTop();
#else
        _rw = CPPLIB( pd_top )();
#endif
    }
}


_EXC_PR::~_EXC_PR               // _EXC_PR DESTRUCTOR
    ( void )
{
    _rtc->thr->exc_pr = _prev;
}


//***********************************************************************
// Implementation of _EXC_PR_FREE
// - same semantics as _EXC_PR
// - also frees an exception saved in allocation area
//***********************************************************************


_EXC_PR_FREE::_EXC_PR_FREE      // _EXC_PR_FREE CONSTRUCTOR
    ( _RTCTL* rtc               // - run-time control
    , RW_DTREG *rw              // - current read/write
    , EXCSTATE state            // - state
    , ACTIVE_EXC* exc )         // - exception to be destructed
    : _EXC_PR( rtc, rw, state )
    , _exc( exc )
{
#ifndef NDEBUG
    _type = EXCPR_FREE;
#endif
}


_EXC_PR_FREE::~_EXC_PR_FREE     // _EXC_PR_FREE DESTRUCTOR
    ( void )
{
    if( NULL != _exc ) {
        ACTIVE_EXC* exc = _exc;
        _exc = 0;
        CPPLIB( free_exc )( _rtc, exc );
    }
}


//***********************************************************************
// Implementation of _EXC_PR_DTOR
// - same semantics as _EXC_PR
// - also destructs and frees an exception saved in allocation area
//***********************************************************************


_EXC_PR_DTOR::_EXC_PR_DTOR      // _EXC_PR_DTOR CONSTRUCTOR
    ( _RTCTL* rtc               // - run-time control
    , RW_DTREG *rw              // - current read/write
    , EXCSTATE state            // - state
    , ACTIVE_EXC* exc )         // - exception to be destructed
    : _EXC_PR_FREE( rtc, rw, state, exc )
{
#ifndef NDEBUG
    _type = EXCPR_DTOR;
#endif
}


_EXC_PR_DTOR::~_EXC_PR_DTOR     // _EXC_PR_DTOR DESTRUCTOR
    ( void )
{
    ACTIVE_EXC* exc = _exc;
    CPPLIB( dtor_free_exc )( _exc, _rtc );
    _exc = 0;
}


//***********************************************************************
// Implementation of _EXC_PR_FNC
// - entry signifying that unexpected() has been invoked
// - modified when unexpected() throws/rethrows something not in
//   original fn-exc specification
//***********************************************************************


_EXC_PR_FNEXC::_EXC_PR_FNEXC    // _EXC_PR_FNEXC CONSTRUCTOR
    ( _RTCTL* rtc               // - run-time control
    , RW_DTREG *rw              // - current read/write
    , RW_DTREG *fnexc_skip      // - skipping read/write
    , ACTIVE_EXC* exc )         // - exception to be destructed
    : _EXC_PR_DTOR( rtc, rw, EXCSTATE_UNEXPECTED, exc )
    , _fnexc_skip( fnexc_skip == 0 ? _rw : fnexc_skip )
{
#ifndef NDEBUG
    _type = EXCPR_FNEXC;
#endif
}

// Complain about defining trivial destructor inside class
// definition only for warning levels above 8 
#pragma warning 657 9

_EXC_PR_FNEXC::~_EXC_PR_FNEXC   // _EXC_PR_FNEXC DESTRUCTOR
    ( void )
{
}
