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


#ifndef ___EXC_PR_H__
#define ___EXC_PR_H__
#ifndef NDEBUG

enum EXCPR_TYPE
{   EXCPR_BASIC
,   EXCPR_FREE
,   EXCPR_DTOR
,   EXCPR_FNEXC
};

#endif


struct _EXC_PR                  // _EXC_PR -- basic processing
{
    RW_DTREG* _rw;              // - R/W block
    _RTCTL* _rtc;               // - run-time control
    _EXC_PR* _prev;             // - previous control entry
    EXCSTATE _state;            // - state for abort
#ifndef NDEBUG
    EXCPR_TYPE _type;           // - debug: type of entry
#endif

    _EXC_PR                     // - constructor
        ( _RTCTL* rtc           // - - run-time control
        , RW_DTREG *rw          // - - current read/write
        , EXCSTATE state )      // - - state
    ;
    ~_EXC_PR                    // - destructor
        ( void )
    ;
};

struct _EXC_PR_FREE             // _EXC_PR_FREE - basic processing, free exc.
    : public _EXC_PR
{
    ACTIVE_EXC* _exc;           // - exception to be destructed

    _EXC_PR_FREE                // - constructor
        ( _RTCTL* rtc           // - - run-time control
        , RW_DTREG *rw          // - - current read/write
        , EXCSTATE state        // - - state
        , ACTIVE_EXC* exc )     // - - exception to be destructed
    ;
    ~_EXC_PR_FREE               // - destructor
        ( void )
    ;
};

struct _EXC_PR_DTOR             // _EXC_PR_DTOR - basic processing, dtor exc.
    : public _EXC_PR_FREE
{
    _EXC_PR_DTOR                // - constructor
        ( _RTCTL* rtc           // - - run-time control
        , RW_DTREG *rw          // - - current read/write
        , EXCSTATE state        // - - state
        , ACTIVE_EXC* exc )     // - - exception to be destructed
    ;
    ~_EXC_PR_DTOR               // - destructor
        ( void )
    ;
};

struct _EXC_PR_FNEXC            // _EXC_PR_FNEXC -- fnexc processing
    : public _EXC_PR_DTOR
{
    RW_DTREG* _fnexc_skip;      // - R/W block skipped until

    _EXC_PR_FNEXC               // - constructor
        ( _RTCTL* rtc           // - - run-time control
        , RW_DTREG *rw          // - - current read/write
        , RW_DTREG *sk          // - - skipping read/write
        , ACTIVE_EXC* exc )     // - - exception to be destructed
    ;
    ~_EXC_PR_FNEXC              // - destructor
        ( void )
    ;
};


#endif
