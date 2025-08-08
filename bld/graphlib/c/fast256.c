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
#if !defined( _M_I86 ) && !defined( __QNX__ )
    #include "extender.h"
    #include "roundmac.h"
    #include "realmod.h"
    #include "rmalloc.h"
#endif


struct rgb {
    char                red;
    char                green;
    char                blue;
};


#if defined( _M_I86 ) || defined( __QNX__ )

bool _FastMap( long _WCI86FAR *colours, short num )
//=============================================

{
    short               i;
    unsigned long       colour;
    struct rgb          *rgb;

    i = _RoundUp( sizeof( struct rgb ) * num );
    if( _stackavail() - i <= 0x100 ) {
        return( false );
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
    return( true );
}


#else


bool _FastMap( long _WCI86FAR *colours, short num )
//==================================================

{
    short               i;
    unsigned long       colour;
    struct rgb __far    *rgb;
    dpmi_dos_mem_block  dos_mem;

    if( _RMAlloc( num * sizeof( struct rgb ), &dos_mem ) ) {
        rgb = RealModeSegmPtr( dos_mem.rm );
        for( i = 0; i < num; ++i ) {
            colour = *colours;
            rgb[i].blue = COLOR_BLUE( colour );
            rgb[i].green = COLOR_GREEN( colour );
            rgb[i].red = COLOR_RED( colour );
            ++colours;
        }
        _RMVideoInt( VIDEOINT_SET_PALETTE + 0x12, 0, num, 0, dos_mem.rm, 0 );
        _RMFree( &dos_mem );
        return( true );
    }
    return( false );
}


bool _RMAlloc( int size, dpmi_dos_mem_block *dos_mem )
//=================================================
{
    int             size_para;

    size_para = __ROUND_UP_SIZE_TO_PARA( size );
    if( _IsPharLap() ) {
        dos_mem->pm = 0;
        dos_mem->rm = PharlapAllocateDOSMemoryBlock( size_para );
        if( dos_mem->rm != 0 ) {
            return( true );
        }
    } else if( _DPMI || _IsRational() ) {
        *dos_mem = DPMIAllocateDOSMemoryBlock( size_para );
        if( dos_mem->rm != 0 ) {
            return( true );
        }
    }
    return( false );
}


void _RMFree( dpmi_dos_mem_block *dos_mem )
//=====================================

{
    if( dos_mem->rm ) {
        if( _IsPharLap() ) {
            PharlapFreeDOSMemoryBlock( dos_mem->rm );
        } else if( _DPMI || _IsRational() ) {
            DPMIFreeDOSMemoryBlock( dos_mem->pm );
        }
        dos_mem->pm = 0;
        dos_mem->rm = 0;
    }
}


short _RMVideoInt( short ax, short bx, short cx, short dx, short es, short di )
//==============================================================

{
    if( _IsPharLap() ) {
        pharlap_regs_struct dp;

        memset( &dp, 0, sizeof( dp ) );
        dp.intno = 0x10;
        dp.r.x.eax = ax;
        dp.r.x.edx = dx;
        dp.es = es;
        PharlapSimulateRealModeInterrupt( &dp, bx, cx, di );
        return( dp.r.w.ax );
    } else if( _DPMI || _IsRational() ) {
        dpmi_regs_struct    dr;

        memset( &dr, 0, sizeof( dr ) );
        dr.r.x.eax = ax;
        dr.r.x.ebx = bx;
        dr.r.x.ecx = cx;
        dr.r.x.edx = dx;
        dr.es = es;
        dr.r.x.edi = di;
        DPMISimulateRealModeInterrupt( 0x10, 0, 0, &dr );
        return( dr.r.w.ax );
    }
    return( 0 );
}

#endif
