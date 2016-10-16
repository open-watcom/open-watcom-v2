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


#ifdef __SW_BM

#include "cpplib.h"
#include <string.h>
#include <lock.h>
#include "rtinit.h"

__lock::__lock() {
/*****************/
    ::memset( &locksem, 0, sizeof( locksem ) );
}

__lock::~__lock() {
/*****************/
    // JBS 99/10/15
    unsigned lock_count;

    lock_count = locksem.count;
    while( locksem.count > 0 ) {
        _ReleaseSemaphore( &locksem );                  // unlock as many times as required
        if( locksem.count == lock_count ) break;        // quit since it won't unlock
    }
    _CloseSemaphore( &locksem );
}

void __lock::p( void ) {
/**********************/
    _AccessSemaphore( &locksem );
}

void __lock::v( void ) {
/**********************/
    _ReleaseSemaphore( &locksem );
}

__fn_lock::__fn_lock( __lock *fn_lock )
    : fn_lock(fn_lock)
{
    fn_lock->p();
}

__fn_lock::~__fn_lock()
{
    fn_lock->v();
}

static void fini(                       // FINALIZE STATIC INIT SEMAPHORE
    void )
{
    _RWD_StaticInitSema.~__lock();
}

extern "C" YI( CPPLIBDATA( fini_sisema ), fini, INIT_PRIORITY_RUNTIME );

#endif
