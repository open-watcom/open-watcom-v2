/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2018 The Open Watcom Contributors. All Rights Reserved.
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
#ifndef _M_I86
    #include "dpmi.h"
    #include "extender.h"
#endif
#include "getltdos.h"


#ifndef _M_I86
#pragma pack(__push,1);
typedef union {
    unsigned long   x;
    unsigned short  w;
    struct {
        unsigned char   l;
        unsigned char   h;
    }               b;
} regx;

typedef struct {
    unsigned short  int_num;
    unsigned short  real_ds;
    unsigned short  real_es;
    unsigned short  real_fs;
    unsigned short  real_gs;
    regx            real_eax;
    regx            real_edx;
} PHARLAP_block;
#pragma pack(__pop);
#endif

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
        "int 21h"       \
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
        PHARLAP_block   pblock;
        union REGS      regs;
        struct SREGS    sregs;

        memset( &pblock, 0, sizeof( pblock ) );
        memset( &regs, 0, sizeof( regs ) );
        memset( &sregs, 0, sizeof( sregs ) );
        pblock.real_eax.x = 0x6300;             /* get DBCS vector table */
        pblock.int_num = 0x21;                  /* DOS call */
        regs.x.eax = 0x2511;                    /* issue real-mode interrupt */
        regs.x.edx = FP_OFF( &pblock );         /* DS:EDX -> parameter block */
        sregs.ds = FP_SEG( &pblock );
        intdosx( &regs, &regs, &sregs );
        if( regs.x.cflag == 0 && pblock.real_eax.b.l == 0 ) {
            if( pblock.real_ds != 0xFFFF ) {    /* weird OS/2 value */
                return( EXTENDER_RM2PM( pblock.real_ds, regs.w.si ) );
            }
        }
    } else if( _IsRational() ) {
        rm_call_struct  dblock;

        memset( &dblock, 0, sizeof( dblock ) );
        dblock.eax = 0x6300;                /* get DBCS vector table */
        DPMISimulateRealModeInterrupt( 0x21, 0, 0, &dblock );
        if( (dblock.flags & 1) == 0 && ((regx *)&dblock.eax)->b.l == 0 ) {
            if( dblock.ds ) {
                return( EXTENDER_RM2PM( dblock.ds, dblock.esi ) );
            }
        }
    }
#endif
    return( NULL );
}
