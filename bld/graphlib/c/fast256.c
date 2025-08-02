/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2025 The Open Watcom Contributors. All Rights Reserved.
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


#include "gdefn.h"
#include "walloca.h"
#include "gbios.h"
#include "stkavail.h"
#include "roundmac.h"
#include "realmod.h"


struct rgb {
    char                red;
    char                green;
    char                blue;
};


#if defined( _M_I86 ) || defined( __QNX__ )

short _FastMap( long _WCI86FAR *colours, short num )
//=============================================

{
    short               i;
    unsigned long       colour;
    struct rgb          *rgb;

    i = _RoundUp( sizeof( struct rgb ) * num );
    if( _stackavail() - i <= 0x100 ) {
        return( FALSE );
    } else {
        rgb = __alloca( i );
    }
    for( i = 0; i < num; ++i ) {
        colour = *colours;
        rgb[i].blue = COLOR_BLUE( colour );
        rgb[i].green = COLOR_GREEN( colour );
        rgb[i].red = COLOR_RED( colour );
        ++colours;
    }
    VideoInt2( VIDEOINT_SET_PALETTE + 0x12, 0, num, rgb );
    return( TRUE );
}


#else


#include "extender.h"
#include "dpmi.h"
#include "rmalloc.h"


short _FastMap( long _WCI86FAR *colours, short num )
//=============================================

{
    short               i;
    unsigned long       colour;
    struct rgb __far    *rgb;
    RM_ALLOC            mem;

    if( _RMAlloc( num * sizeof( struct rgb ), &mem ) ) {
        rgb = mem.pm_ptr;
        for( i = 0; i < num; ++i ) {
            colour = *colours;
            rgb[i].blue = COLOR_BLUE( colour );
            rgb[i].green = COLOR_GREEN( colour );
            rgb[i].red = COLOR_RED( colour );
            ++colours;
        }
        _RMInterrupt( 0x10, VIDEOINT_SET_PALETTE + 0x12, 0, num, 0, mem.rm_seg, 0 );
        _RMFree( &mem );
        return( TRUE );
    }
    return( FALSE );
}


extern short PharlapAlloc( short );
#pragma aux PharlapAlloc = \
        "mov  ax,25c0h" \
        "int 21h"       \
        "jnc short L1"  \
        "sub  ax,ax"    \
    "L1:"               \
    __parm      [__bx] \
    __value     [__ax] \
    __modify    []

extern void PharlapFree( short );
#pragma aux PharlapFree = \
        "mov  ax,25c1h" \
        "int 21h"       \
    __parm      [__cx] \
    __value     \
    __modify    [__ax]

extern short PharlapRMI( pharlap_regs_struct __far *dp, short bx, short cx, short di );
#pragma aux PharlapRMI = \
        "push ds"       \
        "push fs"       \
        "pop  ds"       \
        "mov  ax,2511h" \
        "int 21h"       \
        "pop  ds"       \
    __parm __caller     [__fs __edx] [__ebx] [__ecx] [__edi] \
    __value             [__ax] \
    __modify            []


short _RMAlloc( int size, RM_ALLOC *stg )
//=======================================
{
    int                 size_para;
    dpmi_dos_mem_block  dos_mem;

    size_para = __ROUND_UP_SIZE_TO_PARA( size );
    if( _IsPharLap() ) {
        dos_mem.rm = PharlapAlloc( size_para );
        if( dos_mem.rm != 0 ) {
            stg->rm_seg = dos_mem.rm;
            stg->pm_ptr = RealModeSegmPtr( dos_mem.rm );
            return( TRUE );
        }
    } else if( _DPMI || _IsRational() ) {
        dos_mem = DPMIAllocateDOSMemoryBlock( size_para );
        if( dos_mem.pm ) {
            stg->rm_seg = dos_mem.rm;
            stg->pm_ptr = RealModeSegmPtr( dos_mem.rm );
            return( TRUE );
        }
    }
    return( FALSE );
}


void _RMFree( RM_ALLOC *stg )
//===========================
{
    if( _IsPharLap() ) {
        PharlapFree( stg->rm_seg );
    } else if( _DPMI || _IsRational() ) {
        DPMIFreeDOSMemoryBlock( _FP_SEG( stg->pm_ptr ) );
    }
}


short _RMInterrupt( short intno, short ax, short bx, short cx,
                                  short dx, short es, short di )
//==============================================================

{
    if( _IsPharLap() ) {
        pharlap_regs_struct dp;

        memset( &dp, 0, sizeof( dp ) );
        dp.intno = intno;
        dp.r.x.eax = ax;
        dp.r.x.edx = dx;
        dp.es = es;
        return( PharlapRMI( &dp, bx, cx, di ) );
    } else if( _DPMI || _IsRational() ) {
        dpmi_regs_struct    dr;

        memset( &dr, 0, sizeof( dr ) );
        dr.r.x.eax = ax;
        dr.r.x.ebx = bx;
        dr.r.x.ecx = cx;
        dr.r.x.edx = dx;
        dr.es = es;
        dr.r.x.edi = di;
        DPMISimulateRealModeInterrupt( intno, 0, 0, &dr );
        return( dr.r.x.eax );
    }
    return( 0 );
}

#endif
