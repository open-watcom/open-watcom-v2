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
* Description:  Implementation of stackavail() for 386 based platforms.
*
****************************************************************************/


#include "variety.h"
#include "rtdata.h"
#include "stacklow.h"

#if defined(__OS2__)
extern  unsigned GetThreadStack( void );
#pragma aux GetThreadStack = \
    "mov eax,fs:[4]" \
    value [eax];
#endif

_WCRTLINK unsigned stackavail( void )
{
#if defined(__OS2__)
#if 0
    unsigned    system_stacklow;

    system_stacklow = GetThreadStack();
    if( system_stacklow > _RWD_stacklow ) {
        return( _SP() - system_stacklow );
    } else {
        _RWD_stacklow = system_stacklow;
        return( _SP() - _RWD_stacklow );
    }
#else
    return( _SP() - GetThreadStack() );
#endif
#else
    return( _SP() - _RWD_stacklow );
#endif
}
