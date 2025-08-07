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
* Description:  Get DOS code page
*
****************************************************************************/


#include "variety.h"
#include <string.h>
#include <dos.h>
#include <i86.h>
#include "tinyio.h"
#ifndef _M_I86
    #include "extender.h"
    #include "dpmi.h"
#endif
#include "getcpdos.h"
#include "dointr.h"


#ifdef _M_I86

extern unsigned short _dos_get_code_page( void );
#pragma aux _dos_get_code_page = \
        "mov ax,6601h"  \
        __INT_21        \
        "jnc short L1"  \
        "xor bx,bx"     \
    "L1:"               \
    __parm      [] \
    __value     [__bx] \
    __modify    [__ax __dx]

#endif

unsigned short dos_get_code_page( void )
/**************************************/
{
    unsigned short  codepage;

#ifdef _M_I86
    codepage = _dos_get_code_page();    /* return active code page */
#else
    codepage = 0;
    if( _IsPharLap() ) {
        pharlap_regs_struct dp;
        int                 cflag;

        memset( &dp, 0, sizeof( dp ) );
        dp.r.x.eax = 0x6601;            /* get extended country info */
        dp.intno = 0x21;                /* DOS call */
        cflag = PharlapSimulateRealModeInterruptExt( &dp );
        if( cflag == 0 ) {
            codepage = dp.r.w.bx;       /* return active code page */
        }
    } else if( _DPMI || _IsRational() ) {
        dpmi_regs_struct    dr;
        int                 cflag;

        memset( &dr, 0, sizeof( dr ) );
        dr.r.x.eax = 0x6601;            /* get extended country info */
        cflag = DPMISimulateRealModeInterrupt( 0x21, 0, 0, &dr ) || (dr.flags & INTR_CF);
        if( cflag == 0 ) {
            codepage = dr.r.w.bx;       /* return active code page */
        }
    }
#endif
    return( codepage );
}
