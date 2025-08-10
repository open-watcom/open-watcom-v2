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


#include "variety.h"
#include <dos.h>
#include <bios98.h>
#include <string.h>
#include "tinyio.h"
#include "realmod.h"
#include "roundmac.h"
#include "rtdata.h"
#ifdef __386__
    #include "extender.h"
    #include "dpmi.h"
#endif


#ifdef _M_I86

unsigned short rm_int1b( unsigned short _ax, unsigned short _dx, unsigned short _bx, unsigned short _cx, unsigned short _bp, unsigned short _es );
#pragma aux int1b = \
        "push bp" \
        "push es" \
        "mov bp,si" \
        "mov es,di" \
        "int 1bh" \
        "mov al,ah" \
        "mov ah,0ffh" \
        "jnc short L1" \
        "xor ah,ah" \
    "L1: pop es" \
        "pop bp" \
    __parm [__ax] [__dx] [__bx] [__cx] [__si] [__di] \
    __value [__ax];

#endif


_WCRTLINK unsigned short __nec98_bios_disk( unsigned __cmd, struct diskinfo_t *__diskinfo )
{
    if( _RWD_isPC98 ) { /* NEC PC-98 */
        unsigned short      ret;
        union REGS          rregs;
#ifndef _M_I86
        dpmi_dos_mem_block  dos_mem;
        void                __far *p;
        unsigned            size_para;
        int                 cflag;
#endif

#ifndef _M_I86
        /*
         * Allocate DOS memory and copy buffer to it
         */
        dos_mem.pm = 0;
        dos_mem.rm = 0;
        switch( __cmd ) {
        case _DISK_FORMATTRACK:
            if( __diskinfo->command == _CMD_HD )
                break;
            /* fall through */
        case _DISK_DIAGNOSTIC:
        case _DISK_WRITE:
        case _DISK_READ:
        case _DISK_ALTERNATE:
        case _DISK_WRITEDDAM:
        case _DISK_READDDAM:
            size_para = __ROUND_UP_SIZE_TO_PARA( __diskinfo->data_len );
            if( _IsPharLap() ) {
                dos_mem.rm = PharlapAllocateDOSMemoryBlock( size_para );
                p = RealModeSegmPtr( dos_mem.rm );
                _fmemmove( p, __diskinfo->buffer, __diskinfo->data_len );
            } else if( _DPMI || _IsRational() ) {
                dos_mem = DPMIAllocateDOSMemoryBlock( size_para );
                p = RealModeSegmPtr( dos_mem.rm );
                _fmemmove( p, __diskinfo->buffer, __diskinfo->data_len );
            }
            break;
        }
#endif

        memset( &rregs, 0, sizeof( rregs ) );
        rregs.h.ah = __cmd;
        rregs.h.al = __diskinfo->drive;
        rregs.w.dx = __diskinfo->command;
        if( rregs.h.dl == _CMD_2D
          && rregs.h.ah == _DISK_FORMATDRIVE )
            rregs.h.ah &= 0x7f;
        rregs.w.ax |= rregs.w.dx;
        rregs.w.bx = __diskinfo->data_len;
        if( __diskinfo->command == _CMD_HD ) {
            if( __cmd == _DISK_FORMATDRIVE
              || __cmd == _DISK_FORMATTRACK )
                rregs.h.bh = rregs.h.bl;
            rregs.w.cx = __diskinfo->cylinder;
        } else {
            rregs.h.ch = __diskinfo->sector_len;
            rregs.h.cl = __diskinfo->cylinder;
        }
        rregs.h.dh = __diskinfo->head;
        rregs.h.dl = __diskinfo->sector;
        if( __cmd == _DISK_ALTERNATE )
            rregs.h.dl = 0;

#if defined( _M_I86 )
        rregs.w.di = FP_SEG( __diskinfo->buffer );
        rregs.w.si = FP_OFF( __diskinfo->buffer );
        ret = rm_int1b( rregs.w.ax, rregs.w.dx, rregs.w.bx, rregs.w.cx, rregs.w.si, rregs.w.di );
#else
        if( _IsPharLap() ) {
            pharlap_regs_struct dp;

            memset( &dp, 0, sizeof( dp ) );
            /* Set true register structure */
            dp.r.x.eax = rregs.x.eax;
            dp.r.x.ebx = rregs.x.ebx;
            dp.r.x.ecx = rregs.x.ecx;
            dp.r.x.edx = rregs.x.edx;
            dp.es = dos_mem.rm;
            dp.intno = 0x1b;
            cflag = PharlapSimulateRealModeInterruptExt( &dp );
            ret = dp.r.h.ah;
            if( cflag ) {
                ret |= 0xff00;
            }
            rregs.x.ecx = dp.r.x.ecx;
            rregs.x.edx = dp.r.x.edx;
        } else if( _DPMI || _IsRational() ) {
            dpmi_regs_struct    dr;

            memset( &dr, 0, sizeof( dr ) );
            /* Set true register structure */
            dr.r.x.eax = rregs.x.eax;
            dr.r.x.ebx = rregs.x.ebx;
            dr.r.x.ecx = rregs.x.ecx;
            dr.r.x.edx = rregs.x.edx;
            dr.es = dos_mem.rm;
            /* int 1BH */
            cflag = DPMISimulateRealModeInterrupt( 0x1b, 0, 0, &dr ) || (dr.flags & INTR_CF);
            ret = dr.r.h.ah;
            if( cflag ) {
                ret |= 0xff00;
            }
            rregs.x.ecx = dr.r.x.ecx;
            rregs.x.edx = dr.r.x.edx;
        } else {
            ret = 0;
        }
#endif

        /*
         * set return values
         */
        switch( __cmd ) {
        case _DISK_SEEK:
            ((char _WCI86FAR *)__diskinfo->result)[0] = rregs.h.cl;
            ((char _WCI86FAR *)__diskinfo->result)[1] = rregs.h.dh;
            ((char _WCI86FAR *)__diskinfo->result)[2] = rregs.h.dl;
            ((char _WCI86FAR *)__diskinfo->result)[3] = rregs.h.ch;
            break;
        case _DISK_READ:
        case _DISK_WRITE:
        case _DISK_VERIFY:
        case _DISK_READID:
        case _DISK_WRITEDDAM:
        case _DISK_READDDAM:
        case _DISK_DIAGNOSTIC:
            if( __diskinfo->command == _CMD_2D || __diskinfo->command == _CMD_HD )
                break;
            if( __diskinfo->command == _CMD_2DD ) {
                _fmemmove( __diskinfo->result, BIOSDataPtr( 0x1d0 ), 16 );
            } else {
                _fmemmove( __diskinfo->result, BIOSDataPtr( 0x164 + 8 * __diskinfo->drive ), 8 );
            }
            break;
        }

#ifndef _M_I86
        /*
         * Copy buffer from DOS memory and free it
         */
        if( dos_mem.rm ) {
            if( _IsPharLap() ) {
                _fmemmove( __diskinfo->buffer, p, __diskinfo->data_len );
                PharlapFreeDOSMemoryBlock( dos_mem.rm );
            } else if( _DPMI || _IsRational() ) {
                _fmemmove( __diskinfo->buffer, p, __diskinfo->data_len );
                DPMIFreeDOSMemoryBlock( dos_mem.pm );
            }
        }
#endif

        return( ret );
    }
    /* IBM PC */
    return( 1 );    // fail if not a NEC PC-98 machine
}
