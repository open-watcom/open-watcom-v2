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


#include "variety.h"
#include "watcom.h"
#include "extender.h"
#include "dpmi.h"
#include "exitwmsg.h"
#include "rtinit.h"

_WCRTLINK unsigned short   _ExtenderRealModeSelector;

#ifndef __WINDOWS_386__

extern short __get_ds( void );
#pragma aux __get_ds = "mov ax,ds" value[ax];

static void init( void ) {
    if( _IsFlashTek() ) {
        _ExtenderRealModeSelector = __x386_zero_base_selector;
    } else if( _IsPharLap() || _IsOS386() ) {
        /* use same priv level as DS register */
        _ExtenderRealModeSelector = 0x34 | (__get_ds() & 0x03);
    } else if( _IsRationalZeroBase() || _IsCodeBuilder() ) {
        _ExtenderRealModeSelector = __get_ds();
    } else if( _IsRationalNonZeroBase() ) {
        long    result;
        result = DPMIAllocateLDTDescriptors( 1 );
        if( result < 0 ) {
            __fatal_runtime_error( "Unable to allocate real mode selector", -1 );
        }
        _ExtenderRealModeSelector = result & 0xffff;
        if( DPMISetSegmentLimit( _ExtenderRealModeSelector, 0xfffff ) ) {
            __fatal_runtime_error( "Unable to set limit of real mode selector", -1 );
        }
    } else {
        _ExtenderRealModeSelector = 0;
    }
}

static void fini( void ) {
    if( _IsRationalNonZeroBase() ) {
        DPMIFreeLDTDescriptor( _ExtenderRealModeSelector );
    }
}

#else

static void init( void )
{
    long                result;

    result = DPMIAllocateLDTDescriptors( 1 );
    if( result < 0 ) {
        __fatal_runtime_error( "Unable to allocate real mode selector", -1 );
    }
    _ExtenderRealModeSelector = result & 0xffff;
    if( DPMISetSegmentLimit( _ExtenderRealModeSelector, 0xfffff ) ) {
        __fatal_runtime_error( "Unable to set limit of real mode selector", -1 );
    }
}

static void fini( void )
{
    DPMIFreeLDTDescriptor( _ExtenderRealModeSelector );
}

#endif

AXI( init, INIT_PRIORITY_RUNTIME )
AYI( fini, INIT_PRIORITY_RUNTIME )
