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


#ifndef __RTARCTOR_H_
#define __RTARCTOR_H_

// RTARCTOR -- run-time array ctoring/dtoring control
//
// Destructor is defined in STABADT.CPP.
//
// There is no destructor when exceptions are disabled.
//

// 95/05/25 -- J.W.Welch        -- defined

#include "rtdtor.h"


struct _RTARCTOR
    : public _RTDTOR
{
    unsigned _count;            // - # elements ctored
    RT_TYPE_SIG _sig;           // - type signature for an element
    void* _array;               // - array memory

    _RTARCTOR                   // - constructor
        ( unsigned count        // - - # elements
        , RT_TYPE_SIG sig       // - - type signature
        , void *array )         // - - array
        : _count( count )
        , _sig( sig )
        , _array( array )
    {
    }

#ifdef RT_EXC_ENABLED
    ~_RTARCTOR                  // - destructor
        ( void )
    ;
#endif
#ifndef NDEBUG
    _RTARCTOR                   // - copy ctor
        ( _RTARCTOR const & )
    ;
#endif

    void operator++             // - used to increment # elements
        ( void )
    {
        ++ _count;
    }

    void operator--             // - used to decrement # elements
        ( void )
    {
        -- _count;
    }
};



#endif
