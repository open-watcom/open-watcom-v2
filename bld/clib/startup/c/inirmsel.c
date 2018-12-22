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
* Description:  Real mode selector setup for DOS extenders and Win386.
*
****************************************************************************/


#include "variety.h"
#include "watcom.h"
#include "extender.h"
#include "dpmi.h"
#include "exitwmsg.h"
#include "rtinit.h"


extern short __get_ds( void );
#pragma aux __get_ds = \
        "mov ax,ds" \
    __parm              [] \
    __value             [__ax] \
    __modify __exact    [__ax]

unsigned short  _ExtenderRealModeSelector;

static void init( void )
{
#ifndef __WINDOWS_386__
    if( _IsFlashTek() ) {
        _ExtenderRealModeSelector = __x386_zero_base_selector;
    } else if( _IsPharLap() || _IsOS386() ) {
        /* use same priv level as DS register */
        _ExtenderRealModeSelector = 0x34 | (__get_ds() & 0x03);
    } else if( _IsRationalZeroBase() || _IsCodeBuilder() ) {
        _ExtenderRealModeSelector = __get_ds();
    } else if( _IsRationalNonZeroBase() ) {
#endif
        long    sel;

        sel = DPMIAllocateLDTDescriptors( 1 );
        if( sel < 0 ) {
            __fatal_runtime_error( "Unable to allocate real mode selector", -1 );
            // never return
        }
        _ExtenderRealModeSelector = sel;
        if( DPMISetSegmentLimit( _ExtenderRealModeSelector, 0xfffff ) ) {
            __fatal_runtime_error( "Unable to set limit of real mode selector", -1 );
            // never return
        }
#ifndef __WINDOWS_386__
    } else {
        _ExtenderRealModeSelector = 0;
    }
#endif
}

static void fini( void )
{
#ifndef __WINDOWS_386__
    if( _IsRationalNonZeroBase() ) {
#endif
        DPMIFreeLDTDescriptor( _ExtenderRealModeSelector );
#ifndef __WINDOWS_386__
    }
#endif
}

AXI( init, INIT_PRIORITY_FPU )
AYI( fini, INIT_PRIORITY_FPU )
