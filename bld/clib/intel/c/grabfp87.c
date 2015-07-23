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
* Description:  Set up OS-specific floating-point exception intercepts.
*
****************************************************************************/


#include "variety.h"
#include "rtdata.h"
#ifndef __WINDOWS__
    #include "extender.h"
    #include "dpmi.h"
    #include "dpmihost.h"
#endif
#include "grabfp87.h"

void __GrabFP87( void )
{
#ifndef __WINDOWS__
    if( _RWD_FPE_handler_exit != __Fini_FPE_handler ) {
#ifdef __DOS_386__
        if( !_IsPharLap() && ( __DPMI_hosted() == 1 )) {
            DPMILockLinearRegion((long)&__FPEHandlerStart_,
                ((long)&__FPEHandlerEnd_ - (long)&__FPEHandlerStart_));
        }
#endif
        __Init_FPE_handler();
        _RWD_FPE_handler_exit = __Fini_FPE_handler;
    }
#endif
}
