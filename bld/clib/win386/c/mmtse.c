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


#define INCLUDE_MMSYSTEM_H
#include "cover.h"

#define GetTimerCallBack        WINFUNCTION253ADDR
#pragma aux     WINFUNCTION253ADDR      "*"

extern LPTIMECALLBACK (__far *GetTimerCallBack)();
static LPTIMECALLBACK   CallBackFunc;

extern  void __InvokeCallBack();
#pragma aux  __InvokeCallBack = \
        "and    esp,0000FFFFh"  \
        "jmp    eax"            \
        parm [eax];

void __TimerCallBack()
{
    __InvokeCallBack( CallBackFunc );
}

UINT PASCAL _Cover_timeSetEvent(UINT uDelay, UINT uResolution,
    LPTIMECALLBACK lpFunction, DWORD dwUser, UINT uFlags)
{
    CallBackFunc = lpFunction;
    return( timeSetEvent( uDelay, uResolution,
        GetTimerCallBack( __TimerCallBack ),
        dwUser, uFlags ) );
}
