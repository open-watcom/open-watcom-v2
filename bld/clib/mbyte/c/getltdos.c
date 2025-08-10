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
* Description:  Get DOS dbcs lead table
*
****************************************************************************/


#include "variety.h"
#include <stddef.h>
#include <string.h>
#include <dos.h>
#include <i86.h>
#include "tinyio.h"
#ifndef _M_I86
    #include "extender.h"
    #include "dpmi.h"
#endif
#include "realmod.h"
#include "getltdos.h"


/*
 * Note:
 * Some DOS implementation do nothing but do not return an error (NTVDM).
 * Some versions report success but do not modify registers (US DOS 3.3).
 * We set DS to zero and if it stays unchanged, consider that a failure.
 */

#ifdef _M_I86
extern unsigned short __far *_dos_get_dbcs_lead_table( void );
#pragma aux _dos_get_dbcs_lead_table = \
        "push bp"       \
        "push ds"       \
        "xor ax,ax"     \
        "mov ds,ax"     \
        "mov ah,63h"    \
        __INT_21        \
        "mov di,ds"     \
        "jc short L1"   \
        "test al,al"    \
        "jnz short L1"  \
        "test di,di"    \
        "jnz short L2"  \
    "L1: xor di,di"     \
        "xor si,si"     \
    "L2: pop ds"        \
        "pop bp"        \
    __value     [__di __si] \
    __modify    [__ax __bx __cx __dx]
#endif

unsigned short __far *dos_get_dbcs_lead_table( void )
/***************************************************/
{
#ifdef _M_I86
    return( _dos_get_dbcs_lead_table() );
#else
    if( _IsPharLap() ) {
        pharlap_regs_struct dp;
        int                 cflag;

        memset( &dp, 0, sizeof( dp ) );
        dp.r.x.eax = 0x6300;                 /* get DBCS vector table */
        dp.intno = 0x21;                    /* DOS call */
        cflag = PharlapSimulateRealModeInterruptExt( &dp );
        if( cflag == 0 && dp.r.h.al == 0 ) {
            /*
             * check if DS not 0 or weird OS/2 value 0xFFFF
             * otherwise it is invalid
             */
            if( dp.ds != 0 && dp.ds != 0xFFFF ) {
                return( RealModeDataPtr( dp.ds, dp.r.x.esi ) );
            }
        }
    } else if( _DPMI || _IsRational() ) {
        dpmi_regs_struct    dr;
        int                 cflag;

        memset( &dr, 0, sizeof( dr ) );
        dr.r.x.eax = 0x6300;                /* get DBCS vector table */
        cflag = DPMISimulateRealModeInterrupt( 0x21, 0, 0, &dr ) || (dr.flags & INTR_CF);
        if( cflag == 0 && dr.r.h.al == 0 ) {
            /*
             * check if DS not 0
             * otherwise it is invalid
             */
            if( dr.ds != 0 && dr.ds != 0xFFFF ) {
                return( RealModeDataPtr( dr.ds, dr.r.x.esi ) );
            }
        }
    }
#endif
    return( NULL );
}
