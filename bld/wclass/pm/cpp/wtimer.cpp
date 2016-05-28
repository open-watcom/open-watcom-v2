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


#include "wtimer.hpp"


WEXPORT WTimer::WTimer( WObject* owner, cbt notify, unsigned stack )
        : _id( 0 )
        , _count( 0 )
        , _owner( owner )
        , _notify( notify )
        , _stackSize( stack ) {
/*****************************/

}


WEXPORT WTimer::~WTimer() {
/*************************/

    stop();
}


void APIENTRY timerThread( WTimer *t ) {
/**************************************/

    QMSG        qmsg;

    t->_anchorBlock = WinInitialize( 0 );
    if( t->_anchorBlock == 0 ) return;
    t->_msgQueue = WinCreateMsgQueue( t->_anchorBlock, 0 );
    if( t->_msgQueue == 0 ) {
        WinTerminate( t->_anchorBlock );
        t->_anchorBlock = 0;
        return;
    }
    t->_id = WinStartTimer( t->_anchorBlock, NULLHANDLE, 0, t->_interval );
    while( WinGetMsg( t->_anchorBlock, &qmsg, NULLHANDLE, 0, 0 ) ) {
        if( (qmsg.msg == WM_TIMER) && (LONGFROMMP( qmsg.mp1 ) == t->_id) ) {
            t->tick();
        } else if( (qmsg.msg == WM_QUIT) && (LONGFROMMP( qmsg.mp1 ) == t->_id) ) {
            break;
        } else {
            WinDispatchMsg( t->_anchorBlock, &qmsg );
        }
    }
    WinDestroyMsgQueue( t->_msgQueue );
    WinTerminate( t->_anchorBlock );
    t->_anchorBlock = 0;
}


bool WEXPORT WTimer::start( int interval, int count ) {
/*****************************************************/

    TID         tid;

    _interval = interval;
    _count = count;
    if( DosCreateThread( &tid, (PFNTHREAD)timerThread, (ULONG)this, 0, _stackSize ) != 0 ) {
        return( false );
    }
    return( true );
}


bool WEXPORT WTimer::stop() {
/***************************/

    if( WinStopTimer( _anchorBlock, NULLHANDLE, _id ) ) {
        WinPostQueueMsg( _msgQueue, WM_QUIT, MPFROMLONG( _id ), 0 );
        return( true );
    }
    return( false );
}


void WEXPORT WTimer::tick() {
/***************************/

    if( (_owner != NULL) && (_notify != NULL) ) {
        (_owner->*_notify)( this, WinGetCurrentTime( _anchorBlock ) );
    }
    if( _count != 0 ) {
        _count -= 1;
        if( _count == 0 ) {
            stop();
        }
    }
}
