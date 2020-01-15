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
* Description:  Threaded runtime library init code for Linux.
*
****************************************************************************/


#include "variety.h"
#include <stddef.h>
#include <stdlib.h>
#include <sys/types.h>
#include <process.h>
#include "rtdata.h"
#include "rtstack.h"
#include "stacklow.h"
#include "initfini.h"
#include "snglthrd.h"
#include "fileacc.h"
#include "heapacc.h"
#include "trdlstac.h"
#include "exitwmsg.h"
#include "linuxsys.h"
#include "cinit.h"
#include "mthread.h"

#ifdef __SW_BM
sem_t *__tls_sem;

_WCRTLINK int *__threadid( void )
{
    return( (int *)&(__THREADDATAPTR->thread_id) );
}

static void __NullAccessRtn( int hdl ) { hdl = hdl; }
static void __NullAccIOBRtn(void) {}
static void __NullAccHeapRtn(void) {}
static void __NullAccTDListRtn(void) {}

void                    (*_AccessFileH)(int)     = &__NullAccessRtn;
void                    (*_ReleaseFileH)(int)    = &__NullAccessRtn;
void                    (*_AccessIOB)(void)      = &__NullAccIOBRtn;
void                    (*_ReleaseIOB)(void)     = &__NullAccIOBRtn;
void                    (*_AccessNHeap)(void)    = &__NullAccHeapRtn;
void                    (*_AccessFHeap)(void)    = &__NullAccHeapRtn;
void                    (*_ReleaseNHeap)(void)   = &__NullAccHeapRtn;
void                    (*_ReleaseFHeap)(void)   = &__NullAccHeapRtn;
void                    (*_AccessTDList)(void)   = &__NullAccTDListRtn;
void                    (*_ReleaseTDList)(void)  = &__NullAccTDListRtn;
#endif

void __LinuxInit( struct thread_data *ptr )
/*****************************************/
{
    unsigned    *tmp;

#ifdef __SW_BM
    __tls_sem = (sem_t *)malloc(sizeof(sem_t));
    if(__tls_sem != NULL)
        sem_init( __tls_sem, 0, 1 );

    __InitThreadData( ptr );
    __FirstThreadData = ptr;
#endif

    // following is very tricky _STACKLOW intialization
    tmp = &_STACKLOW;
    #undef _STACKLOW
    *tmp = _STACKLOW;
}

_WCRTLINK _WCNORETURN void __exit( int ret_code )
{
#ifdef __SW_BM
    if( __FirstThreadData != NULL && gettid( ) == __FirstThreadData->thread_id ) {
        __FiniRtns( 0, FINI_PRIORITY_EXIT - 1 );
        _sys_exit_group( ret_code );
        // never return
    } else {
        _sys_exit( ret_code );
        // never return
    }
#else
    __FiniRtns( 0, FINI_PRIORITY_EXIT - 1 );
    _sys_exit( ret_code );
    // never return
#endif
}
