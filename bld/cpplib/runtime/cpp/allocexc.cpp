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


#include "cpplib.h"
#include <string.h>
#include "rtexcept.h"
#include "exc_pr.h"
#include "rtmsgs.h"
#include "lock.h"


typedef struct free_area FREE_AREA;
struct free_area                // FREE_AREA -- freed area in exception area
{
    FREE_AREA *next;            // - next free block -- MUST BE FIRST FIELD
    size_t size;                // - size of free area
#if 0   // removed since it is never used (AFS 12-19-97)
#ifdef __MT__
    __lock _semaphore;           // - semaphore for area
#endif
#endif
};


typedef struct exc_area EXC_AREA;
struct exc_area                 // ENTIRE AREA
{   size_t size;                // - size of area
    FREE_AREA *freed;           // - freed blocks - NULL only at start
#ifdef __MT__
    __lock semaphore;           // - semaphore for area
#endif
};

extern "C" EXC_AREA __EXC_AREA;

#define SIZEOF_HDR   (sizeof( ACTIVE_EXC ) - 1)
#define SIZEOF_SPLIT ( ( sizeof( FREE_AREA ) + 3 ) & -4 )


extern "C"
ACTIVE_EXC *CPPLIB( alloc_exc )(// ALLOCATE AN EXCEPTION
    void *object,               // - address of object
    THROW_RO *throw_ro,         // - throw R/O block
    _RTCTL* rtc )               // - R/T control
{
    FREE_AREA *fr;              // - a freed area
    FREE_AREA **owner;          // - previous freed area
    FREE_AREA *next;            // - next freed area
    RT_TYPE_SIG sig;            // - type signature of thrown item
    ACTIVE_EXC *active;         // - active exception
    unsigned size;              // - size required
    THREAD_CTL *thr;            // - thread control

    #ifdef __MT__
        __EXC_AREA.semaphore.p();
    #endif
    if( __EXC_AREA.freed == NULL ) {
        fr = (FREE_AREA*)((char*)&__EXC_AREA + sizeof( EXC_AREA )); // initial free list entry
        fr->next = NULL;
        fr->size = __EXC_AREA.size - sizeof( EXC_AREA );
        __EXC_AREA.freed = fr;
#ifndef NDEBUG
        memset( (char*)fr + sizeof(FREE_AREA)
              , 0xEF
              , fr->size - sizeof(FREE_AREA) );
#endif
    }
    sig = CPPLIB( ts_refed )( throw_ro->cnvs[0].signature );
    size = CPPLIB( ts_size )( sig ) + SIZEOF_HDR + sizeof( size_t );
    if( size < sizeof( FREE_AREA ) ) {
        size = sizeof( FREE_AREA );
    }
    size = ( size + 3 ) & -4;
    for( owner = &__EXC_AREA.freed; ; owner = &fr->next ) {
        fr = *owner;
        if( fr == NULL ) {
            CPPLIB(fatal_runtime_error)( RTMSG_EXC_NO_SPACE, 1 );
        }
        for( ;; ) { // coalesce
            next = fr->next;
            if( (FREE_AREA*)((char*)fr + fr->size) != next ) break;
            fr->size += next->size;
            fr->next = next->next;
        }
        if( fr->size == size ) {
            *owner = fr->next;
            active = (ACTIVE_EXC*)fr;
            break;
        } else if( fr->size > ( size + SIZEOF_SPLIT ) ) {
            fr->size -= size;
            active = (ACTIVE_EXC*)( (char*)fr + fr->size );
            break;
        }
    }
    #ifdef __MT__
        __EXC_AREA.semaphore.v();
    #endif
    *(size_t*)active = size;
    active = (ACTIVE_EXC*)( (char*)active + sizeof( size_t ) );
    active->exc_area = &__EXC_AREA;
    thr = rtc->thr;
    active->prev = thr->excepts;
    thr->excepts = active;
    active->sig = throw_ro->cnvs[0].signature;
    active->throw_ro = throw_ro;
    active->state = EXCSTATE_CTOR;
    active->zero_thrown = false;
    active->dispatch = NULL;
    active->rw = NULL;
    switch( sig->hdr.type ) {
      case THROBJ_SCALAR :
      case THROBJ_PTR_FUN :
        memcpy( active->data, object, sig->scalar.size );
        break;
      case THROBJ_CLASS :
      { _EXC_PR_FREE marker( rtc, 0, EXCSTATE_CTOR, active );
        (*sig->clss.copyctor)( active->data, object );
        marker._exc = 0;
      } break;
      case THROBJ_CLASS_VIRT :
      { _EXC_PR_FREE marker( rtc, 0, EXCSTATE_CTOR, active );
        (*sig->clss_v.copyctor)( active->data, CTOR_NULL, object );
        marker._exc = 0;
      } break;
      case THROBJ_PTR_CLASS :
      case THROBJ_PTR_SCALAR :
      case THROBJ_VOID_STAR :
        memcpy( active->data, object, sizeof( void* ) );
        size = sizeof( void* );
        break;
      default :
        GOOF_EXC( "ALLOCEXC: unexpected type signature" );
    }
    return active;
}


extern "C"
void CPPLIB( free_exc )(        // FREE AN EXCEPTION
    _RTCTL* rtc,                // - R/T control
    ACTIVE_EXC *active )        // - exception to be freed
{
    FREE_AREA *done;            // - completed item
    FREE_AREA *fr;              // - a freed area
    FREE_AREA **owner;          // - previous freed area
    EXC_AREA* exc_area;         // - exception area for exception
    THREAD_CTL *thr;            // - thread control
    ACTIVE_EXC** pred;          // - addr[ exception ]

    thr = rtc->thr;
    for( pred = &thr->excepts; active != *pred; pred = &(*pred)->prev );
    *pred = active->prev;
    exc_area = (EXC_AREA*)active->exc_area;
    #ifdef __MT__
        exc_area->semaphore.p();
    #endif
    done = (FREE_AREA*)( (char*)active - sizeof( size_t ) );
    done->size = *(size_t*)done;
#ifndef NDEBUG
    memset( (char*)done + sizeof(FREE_AREA)
          , 0xEF
          , done->size - sizeof(FREE_AREA) );
#endif
    for( owner = &exc_area->freed; ; owner = &fr->next ) {
        fr = *owner;
        if( fr == NULL || fr > done ) {
            *owner = done;
            done->next = fr;
            break;
        }
    }
    #ifdef __MT__
        exc_area->semaphore.v();
    #endif
}


extern "C"
ACTIVE_EXC* CPPLIB( find_active )( // FIND EXCEPTION FOR A POSITION
    _RTCTL* rtc,                // - R/T control
    RW_DTREG* rw,               // - current r/w block
    DTOR_CMD* cmd )             // - command within it
{
    ACTIVE_EXC* exc;            // - current exception

    for( exc = rtc->thr->excepts
       ; ( exc != NULL ) && ( rw != exc->rw || cmd != exc->cat_try )
       ; exc = exc->prev );
    return exc;
}


extern "C"
void CPPLIB( dtor_free_exc )    // DESTRUCT AND FREE EXCEPTION
    ( ACTIVE_EXC* active        // - exception
    , _RTCTL* rtc )             // - R/T control
{
    if( 0 != active ) {
        RT_TYPE_SIG sig = CPPLIB( ts_refed )( active->sig );
        if( sig->hdr.type == THROBJ_CLASS
         || sig->hdr.type == THROBJ_CLASS_VIRT ) {
            pFUNdtor dtor = sig->clss.dtor;
            if( dtor != NULL ) {
                _EXC_PR_FREE marker( rtc, 0, EXCSTATE_DTOR, active );
                active->state = EXCSTATE_DTOR;
                (*dtor)( active->data, DTOR_NULL );
                marker._exc = 0;
            }
        }
        CPPLIB( free_exc )( rtc, active );
    }
}
