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
* Description:  Definition of internal feature flags and manipulation functions.
*
****************************************************************************/


#include "variety.h"
#include "rtcntrl.h"

static __rt_flag    __rt_control = RTFLG_NONE;

#ifdef _M_IX86

_WCRTLINK void _SetLD64bit( void )
{
    __rt_control &= ~RTFLG_LD_80BIT;
}

_WCRTLINK void _SetLD80bit( void )
{
    __rt_control |= RTFLG_LD_80BIT;
}

#endif

_WCRTLINK int _LDisDouble( void )
{
    return( (__rt_control & RTFLG_LD_80BIT) == 0 );
}
