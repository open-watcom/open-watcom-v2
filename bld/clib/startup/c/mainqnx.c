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
* Description:  QNX main routine.
*
****************************************************************************/


// these appear ahead of the include files to avoid collisions
// between the _STACKLOW variable and the _STACKLOW macro

extern unsigned _STACKLOW;
struct thread_data *__FirstThreadData = (struct thread_data *)&_STACKLOW;

#include "variety.h"
#include "osthread.h"
#include "stacklow.h"
#include "rtdata.h"

extern void __InitThreadData( thread_data * );

int *__threadid( void )
{
    return( (int *) &(__THREADDATAPTR->thread_id) );
}

static struct thread_data *__SingleThread()
{
    return( __FirstThreadData );
}

static void __NullAccessRtn( int hdl ) { hdl = hdl; }
static void __NullAccIOBRtn(void) {}
static void __NullAccHeapRtn(void) {}
static void __NullAccTDListRtn(void) {}

_WCRTDATA struct thread_data *(*__GetThreadPtr)() = &__SingleThread;
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

void __QNXInit( void *ptr )
/*************************/
{
    unsigned *tmp;

    __InitThreadData( ptr );
    __FirstThreadData = ptr;

    // following is very tricky _STACKLOW intialization
    tmp = &_STACKLOW;
    #undef _STACKLOW
    *tmp = _STACKLOW;
}
