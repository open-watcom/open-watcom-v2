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


#ifndef __RTDTOR_H__
#define __RTDTOR_H__

// _RTDTOR -- R/T Destructor (used as base for specialized destructors
//            in C++ run-time system
//
// The base has no effect when exceptions are not enabled.


struct _RTDTOR                  // _RTDTOR -- destructor base
{
#ifdef RT_EXC_ENABLED
    unsigned _alive;            // - true ==> needs destruction

    _RTDTOR()                   // - default constructor
        :_alive( true )
    {
    }

    unsigned alive() const      // - test if alive
    {
        return _alive;
    }

    void complete()             // - complete operation ( alive is dead)
    {
        _alive = false;
    }
#else
    _RTDTOR()                   // - default constructor
    {
    }

    unsigned alive() const      // - test if alive
    {
        return true;
    }

    void complete() const       // - complete operation ( alive is dead)
    {
    }
#endif
};

#endif
