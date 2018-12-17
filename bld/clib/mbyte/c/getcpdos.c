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
* Description:  Get DOS code page
*
****************************************************************************/


#include "variety.h"
#include <string.h>
#include <dos.h>
#include <i86.h>
#ifndef _M_I86
    #include "dpmi.h"
    #include "extender.h"
#endif
#include "getcpdos.h"
#include "dointr.h"


/* new code using DOS fn 0x6601 */
#ifdef _M_I86
extern unsigned short _dos_get_code_page( void );
#pragma aux _dos_get_code_page = \
        "mov ax,6601h"  \
        "int 21h"       \
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
    codepage = _dos_get_code_page();                /* return active code page */
#else
    codepage = 0;
    if( _IsPharLap() ) {
        union REGPACK   regs;

        memset( &regs, 0, sizeof( regs ) );
        regs.w.ax = 0x6601;                         /* get extended country info */
        _DoINTR( 0x21, &regs, 0 );
        if( (regs.w.flags & 1) == 0 ) {
            codepage = regs.w.bx;                   /* return active code page */
        }
    } else if( _IsRational() ) {
        rm_call_struct  dblock;

        memset( &dblock, 0, sizeof( dblock ) );
        dblock.eax = 0x6601;                        /* get extended country info */
        DPMISimulateRealModeInterrupt( 0x21, 0, 0, &dblock );
        if( (dblock.flags & 1) == 0 ) {
            codepage = (unsigned short)dblock.ebx;  /* return active code page */
        }
    }
#endif
    return( codepage );
}

#if 0
/* old code using DOS fn 0x6501 */
#ifdef _M_I86
#if 0
unsigned short dos_get_code_page( void )
/**************************************/
{
    union REGS          regs;
    struct SREGS        sregs;
    unsigned char       buf[7];

    regs.w.ax = 0x6501;                         /* get international info */
    regs.w.bx = 0xFFFF;                         /* global code page */
    regs.w.cx = 7;                              /* buffer size */
    regs.w.dx = 0xFFFF;                         /* current country */
    regs.w.di = FP_OFF( (void __far *)buf );    /* buffer offset */
    sregs.es = FP_SEG( (void __far *)buf );     /* buffer segment */
    sregs.ds = 0;                               /* in protected mode (dos16m) DS must be initialized */
    intdosx( &regs, &regs, &sregs );            /* call DOS */
    if( regs.w.cflag )
        return( 0 );                            /* ensure function succeeded */
    return( *(unsigned short *)( buf + 5 ) );   /* return code page */
}
#else
extern unsigned short _dos_get_code_page( void );
#pragma aux _dos_get_code_page = \
        "push ds"       \
        "push bp"       \
        "mov bp,sp"     \
        "sub sp,8"      \
        "xor ax,ax"     \
        "mov ds,ax"     \
        "mov ax,6501h"  /* get international info */ \
        "mov bx,-1"     /* global code page */ \
        "mov cx,0007h"  /* buffer size */ \
        "mov dx,-1"     /* current country */ \
        "lea di,[bp-8]" /* buffer offset */ \
        "push ss"       \
        "pop es"        /* buffer segment */ \
        "int 21h"       /* call DOS */ \
        "mov ax,[bp-8+5]" /* code page */ \
        "jnc short NoError" \
        "xor ax,ax"     \
    "NoError:"      \
        "mov sp,bp"     \
        "pop bp"        \
        "pop ds"        \
    __parm      [] \
    __value     [__ax] \
    __modify    [__ax __bx __cx __dx __di __es]
#endif
unsigned short dos_get_code_page( void )
{
    return( _dos_get_code_page() );
}
#else
#pragma pack(__push,1);
typedef struct {
    unsigned short  int_num;
    unsigned short  real_ds;
    unsigned short  real_es;
    unsigned short  real_fs;
    unsigned short  real_gs;
    unsigned long   real_eax;
    unsigned long   real_edx;
} PHARLAP_block;
#pragma pack(__pop);

unsigned short dos_get_code_page( void )
/**************************************/
{
    unsigned short      real_seg;
    unsigned short      codepage = 0;


    /*** Get the code page ***/
    if( _IsPharLap() ) {
        union REGS      r;
        struct SREGS    sregs;
        PHARLAP_block   pblock;

        /*** Alloc DOS Memory under Phar Lap ***/
        memset( &r, 0, sizeof( r ) );
        memset( &sregs, 0, sizeof( sregs ) );
        r.x.ebx = 1;
        r.x.eax = 0x25c0;
        intdosx( &r, &r, &sregs );
        real_seg = r.w.ax;

        memset( &pblock, 0, sizeof( pblock ) );
        pblock.real_eax = 0x6501;           /* get international info */
        pblock.real_edx = 0xFFFF;           /* current country */
        pblock.real_es = real_seg;          /* buffer segment */
        r.x.ebx = 0xFFFF;                   /* global code page */
        r.x.ecx = 7;                        /* buffer size */
        r.x.edi = 0;                        /* buffer offset */
        pblock.int_num = 0x21;              /* DOS call */
        r.x.eax = 0x2511;                   /* issue real-mode interrupt */
        r.x.edx = FP_OFF( &pblock );        /* DS:EDX -> parameter block */
        sregs.ds = FP_SEG( &pblock );
        intdosx( &r, &r, &sregs );
        if( pblock.real_ds != 0xFFFF ) {    /* weird OS/2 value */
            codepage = *(unsigned short __far *)EXTENDER_RM2PM( real_seg, 5 );
        }

        /*** Free DOS Memory under Phar Lap ***/
        r.x.ecx = real_seg;
        r.x.eax = 0x25c1;
        intdosx( &r, &r, &sregs );
    } else if( _IsRational() ) {
        unsigned long       dpmi_rc;
        unsigned short      selector;
        rm_call_struct      dblock;

        /*** Allocate some DOS memory with DPMI ***/
        dpmi_rc = DPMIAllocateDOSMemoryBlock( 1 );      /* one paragraph is enough */
        real_seg = (unsigned short) dpmi_rc;
        selector = (unsigned short) (dpmi_rc>>16);

        memset( &dblock, 0, sizeof( dblock ) );
        dblock.eax = 0x6501;                /* get international info */
        dblock.ebx = 0xFFFF;                /* global code page */
        dblock.ecx = 7;                     /* buffer size */
        dblock.edx = 0xFFFF;                /* current country */
        dblock.edi = 0;                     /* buffer offset */
        dblock.es = real_seg;               /* buffer segment */
        DPMISimulateRealModeInterrupt( 0x21, 0, 0, &dblock );
        if( (dblock.flags & 1) == 0 ) {
            codepage = *(unsigned short __far *)EXTENDER_RM2PM( real_seg, 5 );
        }
        /*** Free DOS memory with DPMI ***/
        DPMIFreeDOSMemoryBlock( selector );
    }

    return( codepage );
}
#endif
#endif
