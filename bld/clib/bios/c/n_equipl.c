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
#include <dos.h>
#include <bios98.h>
#include "nonibm.h"

#ifdef __386__
#include "extender.h"

int chk_87( unsigned short int *ndp_wk );
#pragma aux chk_87 = \
        ".387" \
        "xor edx,edx" \
        "finit" \
        "xor eax,eax" \
        "mov [ebx],ax" \
        "fstcw [ebx]" \
        "push ecx" \
        "mov ecx,000AH" \
        "L1: loop L1" \
        "pop ecx" \
        "and word ptr [ebx],0f3fH" \
        "cmp word ptr [ebx],033fH" \
        "jne L3" \
        "fstsw [ebx]" \
        "push ecx" \
        "mov ecx,0005H" \
        "L2: loop L2" \
        "pop ecx" \
        "test word ptr [ebx],0b8bfH" \
        "jne L3" \
        "or dx,0002H" \
        "L3: mov eax,edx" \
        parm [ebx]  value [eax] modify [eax ebx edx];
#else
int chk_87( unsigned short );
#pragma aux chk_87 = \
        ".8087" \
        "mov bx,ax" \
        "xor dx,dx" \
        "finit" \
        "xor ax,ax" \
        "mov word ptr ss:[bx],ax" \
        "fstcw word ptr ss:[bx]" \
        "push cx" \
        "mov cx,000AH" \
        "L1: loop L1" \
        "pop cx" \
        "and word ptr ss:[bx],0f3fH" \
        "cmp word ptr ss:[bx],033fH" \
        "jne L3" \
        "fstsw word ptr ss:[bx]" \
        "push cx" \
        "mov cx,0005H" \
        "L2: loop L2" \
        "pop cx" \
        "test word ptr ss:[bx],0b8bfH" \
        "jne L3" \
        "or dx,0002H" \
        "L3: mov ax,dx" \
        parm [ax] value [ax] modify [ax bx dx];
#endif
_WCRTLINK unsigned short __nec98_bios_equiplist(void)
{
#if defined(__386__)
    unsigned short ndp_wk, ret, disk_info;
    unsigned char gpib_info;
    unsigned char rs232_info;
    int count;
    union REGS r;
    unsigned short mouse_seg, mouse_off;

    if( !__NonIBM )  return( 0x0000 );  // fail if not a NEC 98 machine

    if( _ExtenderRealModeSelector ){
        disk_info = *(unsigned short _WCFAR *)
                    MK_FP( _ExtenderRealModeSelector, 0x55c );
        gpib_info = *(unsigned char _WCFAR *)
                    MK_FP( _ExtenderRealModeSelector, 0xa3fee );
        rs232_info = *(unsigned char _WCFAR *)
                    MK_FP( _ExtenderRealModeSelector, 0xd8000 );
    }

    ret = (unsigned short)chk_87( &ndp_wk );
    for( count = 0; disk_info; disk_info <<= 1 ){
        if( 0x8000 & disk_info ) count++;
    }
    ret |= count << 3;
    /* Check mouse */
    if( _IsRational() ) {
        r.w.ax = 0x200;         /* Get real mode interrupt vector */
        r.h.bl = 0x33;
        int386( 0x31, &r, &r );
        mouse_seg = r.w.cx;
        mouse_off = r.w.dx;
        r.w.ax = 0x200;
        r.h.bl = 0x34;
        int386( 0x31, &r, &r );
     } else if( _IsPharLap() ) {
        r.w.ax = 0x2503;         /* Get real mode interrupt vector */
        r.h.cl = 0x33;
        intdos( &r, &r );
        mouse_seg = ( r.x.ebx >> 16 ) & 0xffff;
        mouse_off = r.x.ebx & 0xffff;
        r.w.ax = 0x2503;
        r.h.cl = 0x34;
        intdos( &r, &r );
        r.w.cx = ( r.x.ebx >> 16 ) & 0xffff;
        r.w.dx = r.x.ebx & 0xffff;
    }
    if( mouse_seg != r.w.cx || mouse_off != r.w.dx ) {
        r.x.eax = 0;
        int386( 0x33, &r, &r );
        if( r.w.ax ) ret |= 0x100;
    }
    if( gpib_info & 0x20 ) ret |= 0x1000;
    if( gpib_info & 0x08 ) ret |= 0x2000;
    if( gpib_info & 0x10 ) {
        if( rs232_info == 0xff )
            ret |= 0x0200;
        else
            ret |= ((rs232_info + 1) && 7 ) << 9;
    }else {
        ret |= 0x0200;
    }
    ret |= 0x4000; /* Printer is always 1 */
    return ret;
#else
    unsigned short ndp_wk, ret, disk_info;
    unsigned char gpib_info;
    unsigned char rs232_info;
    int count;
    union REGS r;
    struct SREGS s;
    unsigned short mouse_seg, mouse_off;

    if( !__NonIBM )  return( 0x0000 );  // fail if not a NEC 98 machine

    disk_info = *(unsigned short _WCFAR *)MK_FP(0x40,0x15c);
    gpib_info = *(unsigned char _WCFAR *)MK_FP(0xa000, 0x3fee);
    rs232_info = *(unsigned char _WCFAR *)MK_FP(0xd800, 0);

    ret = (unsigned short)chk_87( FP_OFF(&ndp_wk) ); /* ndp_wk must be auto variable */
    for( count = 0; disk_info; disk_info <<= 1 ){
        if( 0x8000 & disk_info ) count++;
    }
    ret |= count << 3;
    /* Check mouse */
    r.h.ah = 0x35;
    r.h.al = 0x33;
    intdosx( &r, &r, & s );
    mouse_seg = s.es;
    mouse_off = r.x.bx;
    r.h.ah = 0x35;
    r.h.al = 0x34;
    intdosx( &r, &r, & s );
    if( mouse_seg != s.es || mouse_off != r.x.bx ) {
        r.x.ax = 0;
        int86( 0x33, &r, &r );
        if( r.x.ax ) ret |= 0x100;
    }
    if( gpib_info & 0x20 ) ret |= 0x1000;
    if( gpib_info & 0x08 ) ret |= 0x2000;
    if( gpib_info & 0x10 ) {
        if( rs232_info == 0xff )
            ret |= 0x0200;
        else
            ret |= ((rs232_info + 1) && 7 ) << 9;
    }else {
        ret |= 0x0200;
    }
    ret |= 0x4000; /* Printer is always 1 */
    return ret;
#endif
}
