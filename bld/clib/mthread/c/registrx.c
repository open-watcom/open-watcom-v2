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
* Description:  Multithreaded support extended function registration.
*
****************************************************************************/


#include "variety.h"
#include <process.h>
#include "thread.h"
#include "cthread.h"


static beginnerex   *__BeginThreadEx    = __CBeginThreadEx;
static enderex      *__EndThreadEx      = __CEndThreadEx;

_WCRTLINK unsigned long _beginthreadex(
    void *security,
    unsigned stack_size,
    thread_fnex *start_address,
    void *arglist,
    unsigned initflag,
    unsigned *thrdaddr )
{
    return( __BeginThreadEx( security, stack_size, start_address, arglist,
            initflag, thrdaddr ) );
}

_WCRTLINK void _endthreadex( unsigned retval )
{
    __EndThreadEx( retval );
}
