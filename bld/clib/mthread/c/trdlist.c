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
* Description:  Thread list and thread data management.
*
****************************************************************************/


#include "variety.h"
#include <string.h>
#include "thread.h"
#include "thrdreg.h"
#include "liballoc.h"
#include "trdlist.h"
#include "mthread.h"
#include "exitwmsg.h"

extern void     (*_AccessTDList)(void);
extern void     (*_ReleaseTDList)(void);

typedef struct thread_data_list {
    struct thread_data_list *next;
    TID                      tid;
    thread_data             *data;
    int                      allocated_entry;
} thread_data_list;

static thread_data_list *__thread_data_list;

// lookup thread data
thread_data *__GetThreadData( void )
{
    thread_data *tdata = NULL;
    #ifdef __OS2__
        TID             tid;
        tid = GetCurrentThreadId();
        if( tid <= __MaxThreads ) {
            tdata = __AllocInitThreadData( tdata );
            if( tdata != NULL ) {
                if( __initthread( tdata ) ) {
                    __FreeInitThreadData( tdata );
                    tdata = NULL;
                } else {
                    __ThreadData[tid].data = tdata;
                    __ThreadData[tid].allocated_entry = tdata->__allocated;
                }
            }
        } else {
            thread_data_list *tdl;
            thread_data_list **pprev;

            _AccessTDList();
            tdata = NULL;
            pprev = &__thread_data_list;
            for( tdl = *pprev; tdl != NULL ; tdl = tdl->next ) {
                if( tdl->tid == tid ) {
                    tdata = tdl->data;
                    break;
                }
                pprev = &(tdl->next);
            }
            if( tdata == NULL ) {
                tdata = __AllocInitThreadData( tdata );
                if( tdata != NULL ) {
                    if( !__AddThreadData( tid, tdata ) ) {
                        __FreeInitThreadData( tdata );
                        tdata = NULL;
                    }
                }
            } else if( *pprev ) {
                // promote to front
                *pprev = tdl->next;
                tdl->next = __thread_data_list;
                __thread_data_list = tdl;
                // check for need to resize thread data
                if( tdata->__resize ) {
                    tdata = __ReallocThreadData();
                }
            }
            _ReleaseTDList();
        }
    #elif defined(__NT__)
        if( __NTAddThread( tdata ) ) 
        {
            tdata = (thread_data *)TlsGetValue( __TlsIndex );
        }
    #elif defined(_NETWARE_LIBC)
        if( __LibCAddThread( tdata ) )
        {
            if(0 != NXKeyGetValue(__NXSlotID, (void **) &tdata))
                tdata = NULL;
        }
    #elif defined(__RDOS__)
        if( __RdosAddThread( tdata ) ) 
        {
            tdata = (thread_data *)__tls_get_value( __TlsIndex );
        }
    #endif
    if( tdata == NULL ) {
        __fatal_runtime_error( "Thread has no thread-specific data", 1 );
    }
    return( tdata );
}

// realloc thread data
thread_data *__ReallocThreadData( void )
{
    TID tid;
    thread_data *tdata;

    _AccessTDList();
    tid = GetCurrentThreadId();
    #ifdef __OS2__
        if( tid <= __MaxThreads ) {
            thread_data_vector *tdv;
            tdv = &(__ThreadData[tid]);
            if( tdv->allocated_entry ) 
            {
                #if defined (_NETWARE_LIBC)
                /*
                //  we don't want to lose __FirstThreadData as our global
                //  destructors will try and access it as they are called
                //  from a different thread.
                */
                if(__IsFirstThreadData(tdv->data))
                {
                    tdata = lib_realloc( tdv->data, __ThreadDataSize );
                    __RegisterFirstThreadData(tdata);
                }
                else
                #endif
                    tdata = lib_realloc( tdv->data, __ThreadDataSize );
                if( tdata == NULL ) {
                    __fatal_runtime_error( "Unable to resize thread-specific data", 1 );
                }
                tdv->data = tdata;
            } 
            else 
            {
                tdata = lib_calloc( 1, __ThreadDataSize );
                if( tdata == NULL ) {
                    __fatal_runtime_error( "Unable to resize thread-specific data", 1 );
                }
                memcpy( tdata, tdv->data, tdv->data->__data_size );
                tdv->allocated_entry = 1;
                tdv->data = tdata;
            }
        } else
    #endif
    {
        thread_data_list *tdl;

        for( tdl = __thread_data_list ; tdl != NULL ; tdl = tdl->next ) {
            if( tdl->tid == tid ) {
                break;
            }
        }
        if( tdl == NULL ) {
            __fatal_runtime_error( "Thread has no thread-specific data", 1 );
        }
        if( tdl->allocated_entry ) 
        {
            #if defined(_NETWARE_LIBC)
            if(tdata = lib_malloc( __ThreadDataSize ))
            {
                memcpy(tdata, tdl->data, min(__ThreadDataSize, tdl->data->__data_size));
                lib_free(tdl->data);
            }
            #else
            tdata = lib_realloc( tdl->data, __ThreadDataSize );
            #endif
            if( tdata == NULL ) 
            {
                __fatal_runtime_error( "Unable to resize thread-specific data", 1 );
            }
            tdl->data = tdata;
        } 
        else 
        {
            tdata = lib_calloc( 1, __ThreadDataSize );
            if( tdata == NULL ) 
            {
                __fatal_runtime_error( "Unable to resize thread-specific data", 1 );
            }
            memcpy( tdata, tdl->data, tdl->data->__data_size );
            tdl->allocated_entry = 1;
            tdl->data = tdata;
        }
    }
    tdata->__allocated = 1;
    tdata->__data_size = __ThreadDataSize;
    tdata->__resize = 0;
    #if defined(__NT__)
        TlsSetValue( __TlsIndex, tdata );
    #endif
    #if defined(_NETWARE_LIBC)
        if(0 != NXKeySetValue(__NXSlotID, tdata))
        {
            lib_free(tdata);
            tdata = NULL;
        }
    #endif
    _ReleaseTDList();
    return( tdata );
}

// add to list of thread data
int __AddThreadData( TID tid, thread_data *tdata )
{
    int             retn = 1;
    thread_data_list *tdl;

    _AccessTDList();
    tdl = lib_calloc( 1, sizeof( *tdl ) );
    if( tdl != NULL ) 
    {
        /*
        //  Thin doesn't need to call __initthread
        */
#if !defined(_THIN_LIB)
        if( __initthread( tdata ) ) 
        {
            lib_free( tdl );
            retn = 0;
        } 
        else 
#endif
        {
            tdl->data = tdata;
            tdl->tid = tid;
            tdl->allocated_entry = tdata->__allocated;
            tdl->next = __thread_data_list;
            __thread_data_list = tdl;
        }
    } 
    else 
    {
        retn = 0;
    }
    _ReleaseTDList();
    return( retn );
}

// remove from list of thread data
void __RemoveThreadData( TID tid )
{
    thread_data_list *tdl;
    thread_data_list **pprev;

    _AccessTDList();
    pprev = &__thread_data_list;
    for( tdl = *pprev; tdl != NULL ; tdl = tdl->next ) {
        if( tdl->tid == tid ) {
            if( tdl->allocated_entry ) 
            {
                lib_free( tdl->data );
                #ifdef __RUNTIME_CHECKS__
                    tdl->data = (void *)1;
                #endif
            }
            *pprev = tdl->next;
            lib_free( tdl );
            break;
        }
        pprev = &(tdl->next);
    }
    _ReleaseTDList();
}

#if defined _NETWARE_LIBC
void __RemoveAllThreadData( void )
{
    thread_data_list *tdl;
    thread_data_list *next;

    _AccessTDList();

    next = NULL;

    for( tdl = __thread_data_list; tdl != NULL ; ) 
    {
        next = tdl->next;

        if( tdl->allocated_entry ) 
        {
            lib_free( tdl->data );
            #ifdef __RUNTIME_CHECKS__
            tdl->data = (void *)1;
            #endif
        }
        lib_free( tdl );

        tdl = next;
    }

    _ReleaseTDList();
}
#endif

// mark for resize in list of thread data
void __ResizeThreadDataList( void )
{
    thread_data_list *tdl;

    _AccessTDList();
    #ifdef __OS2__
    {
        int i;
        thread_data_vector *tdv;

        tdv = __ThreadData;
        if( tdv != NULL ) {
            for( i = 0 ; i <= __MaxThreads ; i++ ) {
                if( tdv->data ) {
                    tdv->data->__resize = 1;
                }
                tdv++;
            }
        }
    }
    #endif

    tdl = __thread_data_list;
    while( tdl != NULL ) {
        tdl->data->__resize = 1;
        tdl = tdl->next;
    }
    _ReleaseTDList();
}

// clean up list of thread data
// don't need semaphores because shutdown only has one thread executing
void __FreeThreadDataList( void )
{
    thread_data_list *tdl;
    thread_data_list *next;

    tdl = __thread_data_list;
    while( tdl != NULL ) {
        next = tdl->next;
        if( tdl->allocated_entry ) {
            lib_free( tdl->data );
        }
        lib_free( tdl );
        tdl = next;
    }
}

// a sanity check routine that can be called from a termination routine
int __ActiveThreads( void )
{

    thread_data_list *tdl;
    int threads;

    threads = 0;
    for( tdl = __thread_data_list ; tdl != NULL ; tdl = tdl->next ) {
      threads++;
    }
    return( threads );
}
