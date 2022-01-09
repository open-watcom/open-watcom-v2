/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2022 The Open Watcom Contributors. All Rights Reserved.
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
#include "rtdata.h"
#ifdef __386__
    #include "extender.h"
    #include "realmod.h"
#endif
#include "ispc98.h"

_WCRTLINK unsigned short __nec98_bios_equiplist( void )
{
    if( __isPC98 ) {    /* NEC PC-98 */
        unsigned short  ret;
        int             count;
        unsigned short  disk_info;
        unsigned char   gpib_info;
        unsigned char   rs232_info;
        union REGS      r;
        unsigned short  mouse_seg;
        unsigned short  mouse_off;
#if defined( _M_I86 )
        struct SREGS    s;
#endif

#if defined( _M_I86 )
        disk_info = *(unsigned short _WCFAR *)MK_FP(0x40,0x15c);
        gpib_info = *(unsigned char _WCFAR *)MK_FP(0xa000, 0x3fee);
        rs232_info = *(unsigned char _WCFAR *)MK_FP(0xd800, 0);
#else
        if( _ExtenderRealModeSelector ){
            disk_info = *(unsigned short _WCFAR *)MK_FP( _ExtenderRealModeSelector, 0x55c );
            gpib_info = *(unsigned char _WCFAR *)MK_FP( _ExtenderRealModeSelector, 0xa3fee );
            rs232_info = *(unsigned char _WCFAR *)MK_FP( _ExtenderRealModeSelector, 0xd8000 );
        }
#endif
        ret = (unsigned short)_RWD_real87;
        for( count = 0; disk_info != 0; disk_info <<= 1 ) {
            if( 0x8000 & disk_info ) {
                count++;
            }
        }
        ret |= count << 3;
#if defined( _M_I86 )
        /* Check mouse */
        r.h.ah = 0x35;
        r.h.al = 0x33;
        intdosx( &r, &r, &s );
        mouse_seg = s.es;
        mouse_off = r.x.bx;
        r.h.ah = 0x35;
        r.h.al = 0x34;
        intdosx( &r, &r, & s );
        if( mouse_seg != s.es || mouse_off != r.x.bx ) {
            r.x.ax = 0;
            int86( 0x33, &r, &r );
            if( r.x.ax ) {
                ret |= 0x100;
            }
        }
#else
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
            if( r.w.ax ) {
                ret |= 0x100;
            }
        }
#endif
        if( gpib_info & 0x20 )
            ret |= 0x1000;
        if( gpib_info & 0x08 )
            ret |= 0x2000;
        if( gpib_info & 0x10 ) {
            if( rs232_info == 0xff ) {
                ret |= 0x0200;
            } else {
                ret |= ((rs232_info + 1) && 7 ) << 9;
            }
        } else {
            ret |= 0x0200;
        }
        ret |= 0x4000; /* Printer is always 1 */
        return( ret );
    }
    /* IBM PC */
    return( 0 );    // fail if not a NEC PC-98 machine
}
