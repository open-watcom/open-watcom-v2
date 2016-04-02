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
* Description:  thread proccessing functions
*
****************************************************************************/


#include "ftnstd.h"
#include <process.h>
#include <stdlib.h>
#include <stddef.h>
#include "fthread.h"
#include "xfflags.h"
#include "rundat.h"
#include "rmemmgr.h"
#include "thread.h"
#include "ftnapi.h"
#include "fthrdini.h"
#include "rttraps.h"
#include "rtspawn.h"
#include "rt_init.h"


typedef struct {
    void        (*rtn)(void *);
    void        *arglist;
} thread_info;

static  beginner        *__BeginThread;
static  ender           *__EndThread;
static  initializer     *__InitDataThread;

beginner        FBeginThread;
ender           FEndThread;
initializer     FInitDataThread;

static  bool            ThreadsInitialized;

static  unsigned  InitFThreads( void ) {
//================================

    if( ThreadsInitialized )
        return( 0 );
    if( __InitFThreadProcessing() != 0 )
        return( 1 );
    ThreadsInitialized = TRUE;
    RTSysInit();
    return( 0 );
}


static  void    FiniFThreads( void ) {
//==============================

    if( ThreadsInitialized ) {
        __FiniFThreadProcessing();
    }
}


static void     FThreadInit( void ) {
//=============================

    R_TrapInit();
}


static void     FThreadFini( void ) {
//=============================

    R_TrapFini();
}


static  void    ThreadStarter( void ) {
//===============================

    __FTHREADDATAPTR->__rtn( __FTHREADDATAPTR->__arglist );
}


static  void    ThreadHelper( void *arg_ti ) {
//===============================================
    thread_info *ti = arg_ti;

    FThreadInit();
    __FTHREADDATAPTR->__rtn = ti->rtn;
    __FTHREADDATAPTR->__arglist = ti->arglist;
    RMemFree( ti );
    RTSpawn( ThreadStarter );
    FThreadFini();
    __EndThread();
}


int FBeginThread( void (*rtn)(void *), void *stack, unsigned stk_size, void *arglist ) {
//======================================================================================

    thread_info *ti;

    if( InitFThreads() != 0 )
        return( -1 );
    ti = RMemAlloc( sizeof( thread_info ) );
    if( ti == NULL )
        return( -1 );
    ti->rtn = rtn;
    ti->arglist = arglist;

    return( __BeginThread( ThreadHelper, stack, stk_size, ti ) );
}


void FEndThread( void ) {
//=======================

    RTSuicide();
}


int  FInitDataThread( void *td ) {
//================================

    __InitFThreadData( (fthread_data *)((char *)td + __FThreadDataOffset) );
    return( __InitDataThread( td ) );
}


// User-callable thread functions:
// -------------------------------

int     __fortran BEGINTHREAD( void (*rtn)(void *), unsigned long *stk_size ) {
//===========================================================================

#ifdef __NT__
    return( (int)_beginthread( rtn, *stk_size, NULL ) );
#else
    return( _beginthread( rtn, NULL, *stk_size, NULL ) );
#endif
}


void    __fortran ENDTHREAD( void ) {
//===========================

    _endthread();
}


unsigned        __fortran THREADID( void ) {
//==================================

    return( *__threadid() );
}


// Initializer/finalizer for thread processing:
// --------------------------------------------


void    __FiniBeginThread( void ) {
//===========================

    FiniFThreads();
}

#pragma off (check_stack)
void    __InitBeginThread( void ) {
//===========================

    __BeginThread = &FBeginThread;
    __EndThread = &FEndThread;
    __InitDataThread = &FInitDataThread;
    __RegisterThreadData( &__BeginThread, &__EndThread, &__InitDataThread );
    ThreadsInitialized = FALSE;
}
