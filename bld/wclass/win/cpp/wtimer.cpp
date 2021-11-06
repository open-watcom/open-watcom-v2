/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2020 The Open Watcom Contributors. All Rights Reserved.
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


#include "wtimer.hpp"
#include "wclbproc.h"

WObjectMap WEXPORT WTimer::_timerMap;
static int _timerId = 1;

extern "C" VOID _WEXPORT CALLBACK timerProc( HWND, UINT, UINT_PTR id, DWORD sysTime ) {
/*************************************************************************************/

    WTimer *timer = (WTimer *)WTimer::_timerMap.findThis( (WHANDLE)id );
    if( timer != NULL ) {
        timer->tick( sysTime );
    }
}

WEXPORT WTimer::WTimer( WObject *owner, cbt notify, unsigned stack )
        : _id( 0 )
        , _count( 0 )
        , _owner( owner )
        , _notify( notify )
        , _stackSize( stack ) {
/*****************************/

    _procInst = MakeProcInstance_TIMER( timerProc, GUIMainHInst );
}


WEXPORT WTimer::~WTimer() {
/*************************/

    stop();
    FreeProcInstance_TIMER( _procInst );
}


bool WEXPORT WTimer::start( int interval, int count ) {
/******************************************************/

    _count = count;
    _id = SetTimer( NULL, _timerId++, interval, _procInst );
    _timerMap.setThis( this, (WHANDLE)_id );
    return( _id != 0 );
}


bool WEXPORT WTimer::stop() {
/***************************/

    if( _id == 0 )
        return( true );
    _timerMap.clearThis( this );
    int killed = KillTimer( NULL, _id );
    _id = 0;
    return( killed != 0 );
}


void WEXPORT WTimer::tick( DWORD sysTime ) {
/*******************************************/

    if( (_owner != NULL) && (_notify != NULL) ) {
        (_owner->*_notify)( this, sysTime );
    }
    if( _count != 0 ) {
        _count -= 1;
        if( _count == 0 ) {
            stop();
        }
    }
}
