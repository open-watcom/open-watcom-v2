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
* Description:  NetWare LibC threading support routines.
*
****************************************************************************/


#include "variety.h"
#include <process.h>
#include <stddef.h>
#include <string.h>
#include <malloc.h>
#include <i86.h>
#include "liballoc.h"
#include "rtdata.h"
#include "stacklow.h"
#include "sigtab.h"
#include "exitwmsg.h"
#include "thread.h"
#include "trdlist.h"
#include "mthread.h"
#include "cthread.h"
#include "snglthrd.h"

#if !defined (_NETWARE_LIBC)
#error This file is for the NetWare LibC based library only
#endif

extern void BreakPointInt3(void);
#pragma aux BreakPointInt3 = 0xCC;

static void __LibCKeyValueDestructor(void * pPerThreadData)
{
    /* what to do here with the data! */

    thread_data *tdata = (thread_data *) pPerThreadData;

    if(NULL != tdata)
    {
        __RemoveThreadData(tdata->thread_id);
    }
}

int __LibCThreadInit( void )
/**************************/
{
    int err = 0;
    if( __NXSlotID == NO_INDEX )
    {
        err = NXKeyCreate(__LibCKeyValueDestructor, (void *) NULL, &__NXSlotID);
    }
    if((0 != err) || ( __NXSlotID == NO_INDEX ))
    {
        return( FALSE );
    }
    return( TRUE );
}

extern void __LibCThreadFini( void )
/********************************/
{
    if( __NXSlotID != NO_INDEX )
    {
        NXKeyDelete(__NXSlotID);
        __NXSlotID = NO_INDEX;
    }
}

int __LibCAddThread( thread_data *tdata )
/***************************************/
{
    if( __NXSlotID == NO_INDEX )
    {
        return( FALSE );
    }

    tdata = __AllocInitThreadData( tdata );
    if( tdata == NULL )
    {
        return( FALSE );
    }
    if( !__AddThreadData( tdata->thread_id, tdata ) )
    {
        lib_free( tdata );
        return( FALSE );
    }
    if(0 != NXKeySetValue(__NXSlotID, tdata ))
    {
        lib_free( tdata );
        return( FALSE );
    }

    return( TRUE );
}


void __LibCRemoveThread( int close_handle )
/***************************************/
{
    thread_data *tdata = NULL;

    if( __NXSlotID != NO_INDEX )
    {
        int ccode = NXKeyGetValue(__NXSlotID, (void **)&tdata);
        if(0 != ccode)
            return;
        #if defined( __RUNTIME_CHECKS__ ) && defined( _M_IX86 )
            if( tdata == (thread_data *)2 )
                return;
        #else
            if( tdata == NULL )
                return;
        #endif
        __RemoveThreadData( tdata->thread_id );
        #if defined( __RUNTIME_CHECKS__ ) && defined( _M_IX86 )
            NXKeySetValue(__NXSlotID, (void *) 2);
        #else
            NXKeySetValue(__NXSlotID, NULL);
        #endif
    }
}

#if !defined (_NETWARE_LIBC)

might still need this!
static void __ThreadExit()
/************************/
{
    __LibCRemoveThread( TRUE );
    __LibCThreadFini();
}
#endif

#if 1
typedef struct {
    thread_fn           *start_addr;
    void                *arglist;
    void                *stack_bottom;
    TID                 tid;
    NXSema_t *          semaphore;
    NXThreadId_t        nxtid;      /* NKS TID */
    NXContext_t         cx;         /* NKS Context */
} begin_thread_data;

static void begin_thread_helper( void *the_arg )
{
    thread_fn           *start_addr;
    void                *arglist;
    void                *stack_bottom;

    thread_data         *tdata;
    TID                 newtid;
    begin_thread_data   *data = the_arg;

    tdata = alloca( __ThreadDataSize );
    newtid = __GetSystemWideUniqueTID();
    if( 0 != newtid)
    {
        data->tid       = newtid;
        start_addr      = data->start_addr;
        arglist         = data->arglist;
        stack_bottom    = data->stack_bottom;

        NXSemaPost( data->semaphore );
        /* we aren't handling NX_INVALID_THREAD_ID return here !?!?! */
        memset( tdata, 0, __ThreadDataSize );
        // tdata->__allocated = 0;
        tdata->__data_size = __ThreadDataSize;
        tdata->__randnext = 1;
        _RWD_stacklow = FP_OFF( stack_bottom );
        (*start_addr)( arglist );
        _endthread();
    }
    else
    {
        data->tid = -1;
        NXSemaPost( data->semaphore );
    }
}

extern int __CBeginThread(
    thread_fn *     start_addr,
    void *          stack_bottom,
    unsigned        stack_size,
    void *          arglist
    )
{
    begin_thread_data   data;
    int                 error;

    if( __NXSlotID == NO_INDEX ){
        __InitMultipleThread();
    }

    data.start_addr     = start_addr;
    data.stack_bottom   = stack_bottom;
    data.arglist        = arglist;
    if(NULL == (data.semaphore = NXSemaAlloc(0, NULL)))
        return -1;

    /*
    //  We create the thread as detached (non-joinable) and
    //  to automatically clear context
    */

    if (data.cx = NXContextAlloc((void (*)(void *)) &begin_thread_helper, &data,NX_PRIO_MED, __SYS_ALLOCD_STACK, NX_CTX_NORMAL, &error))
    {
        error = NXThreadCreate(data.cx, NX_THR_DETACHED |NX_THR_BIND_CONTEXT, &data.nxtid);
    }

    if(0 == error)
    {
        NXSemaWait( data.semaphore );
    }
    else
    {
        /* should we set errno here? */
        data.tid = -1;
    }
    NXSemaFree( data.semaphore );
    return( data.tid );
}

extern void __CEndThread( void )
{
#pragma message ("TODO: Allow real return codes")
    __LibCRemoveThread( TRUE );
    NXThreadExit( NULL );
}
#endif

/*
//  CurrentProcess() is a THREADS.NLM (CLIB) export though it returns the
//  underlying NetWare TCO pointer. As unique as I can get on NetWare currently
*/
extern unsigned long CurrentProcess(void);
extern unsigned long __GetSystemWideUniqueTID(void)
{
    return(CurrentProcess());
}

extern int __CreateFirstThreadData(void)
{
    thread_data * tdata = lib_calloc(1, __ThreadDataSize);

    if(NULL == tdata)
        return 0;

    tdata->__allocated = 1;
    tdata->__data_size = __ThreadDataSize;
    __FirstThreadData = tdata;
    return 1;
}

extern int __RegisterFirstThreadData(thread_data * tdata)
{
    __FirstThreadData = tdata;
    return 0;
}

extern int __IsFirstThreadData(thread_data * tdata)
{
    return(__FirstThreadData == tdata);
}

/*
//  This is all new and partially untested code to help support _beginthread() and _threadid macro
*/
_WCRTLINK int *__threadid( void )
{
//extern int *__threadid( void )

    static int BadThreadId = -1L;
    thread_data *tdata = NULL;

    if( __NXSlotID != NO_INDEX ){
        int ccode = NXKeyGetValue(__NXSlotID, (void **)&tdata);
        if(0 != ccode)
            return &BadThreadId;
        #if defined( __RUNTIME_CHECKS__ ) && defined( _M_IX86 )
            if( tdata == (thread_data *)2 )
                return &BadThreadId;
        #else
            if( tdata == NULL )
                return &BadThreadId;
        #endif

        return ( (int *) &(tdata->thread_id) );
    }
    return &BadThreadId;
}
