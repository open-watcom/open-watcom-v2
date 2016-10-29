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


// PUREERR.C -- support for trapping non-overridden pure virtual calls
//
// 92/05/18 -- A.F.Scian        -- defined
// 92/10/11 -- J.W.Welch        -- Use RTMSGS.H
// 94/12/12 -- J.W.Welch        -- moved to CPP
#include "cpplib.h"
#include "rtmsgs.h"


extern "C"
_WPRTLINK
void CPPLIB( pure_error )(        // TRAP NON-OVERRIDDEN PURE VIRTUAL CALLS
    void )
{
    if( !_RWD_PureErrorFlag ) {
        _RWD_PureErrorFlag = 1;
        CPPLIB( fatal_runtime_error )( RTMSG_PURE_ERR, 1 );
        // never return
    }
}
