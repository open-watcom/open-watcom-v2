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
#include <string.h>
#include "tinyio.h"
#include "realmod.h"
#include "rtdata.h"
#ifdef __386__
    #include "extender.h"
#endif


#ifdef _M_I86

unsigned short int1b( unsigned short _ax, unsigned short _dx, unsigned short _bx, unsigned short _cx, unsigned short _bp, unsigned short _es );
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

#else

void _nec_pass_bp( unsigned srvno, rmi_struct *rmreg, unsigned _ebx, unsigned _ecx, unsigned _ebp, union REGS *outreg );
#pragma aux _nec_pass_bp = \
        "push ebp" \
        "mov ebp,esi" \
        "int 0x21" \
        "pop ebp" \
        "mov dword ptr [edi],eax" /* [edi].x.eax,eax */ \
        "rcl eax,1" \
        "and eax,1" \
        "mov dword ptr [edi+24],eax" /* [edi].x.cflag,eax */ \
    __parm [__eax] [__edx] [__ebx] [__ecx] [__esi] [__edi];

#endif


_WCRTLINK unsigned short __nec98_bios_disk( unsigned __cmd, struct diskinfo_t *__diskinfo )
{
    if( _RWD_isPC98 ) { /* NEC PC-98 */
        unsigned short  ret;
        char _WCI86FAR  *result_p;
        union REGS      rregs;
#if defined( _M_I86 )
#else
        call_struct     dr;
        rmi_struct      dp;
        union REGS      regs;
        unsigned long   psel;
        unsigned long   rseg;
#endif

        memset( &rregs, 0, sizeof( rregs ) );

        rregs.h.ah = __cmd;
        rregs.h.al = __diskinfo->drive;
        rregs.w.dx = __diskinfo->command;
        if( rregs.h.dl == _CMD_2D && rregs.h.ah == _DISK_FORMATDRIVE )
            rregs.h.ah &= 0x7f;
        rregs.w.ax |= rregs.w.dx;
        rregs.w.bx = __diskinfo->data_len;
        if( __diskinfo->command == _CMD_HD ) {
            if( __cmd == _DISK_FORMATDRIVE || __cmd == _DISK_FORMATTRACK )
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
        ret = int1b( rregs.w.ax, rregs.w.dx, rregs.w.bx, rregs.w.cx, rregs.w.si, rregs.w.di );
        switch( __cmd ) {
        case _DISK_SEEK:
            *( (char _WCI86FAR *)__diskinfo->result + 0 ) = rregs.h.cl;
            *( (char _WCI86FAR *)__diskinfo->result + 1 ) = rregs.h.dh;
            *( (char _WCI86FAR *)__diskinfo->result + 2 ) = rregs.h.dl;
            *( (char _WCI86FAR *)__diskinfo->result + 3 ) = rregs.h.ch;
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
                result_p = (char _WCFAR *)MK_FP( 0x40, 0x1d0 );
                _fmemmove( __diskinfo->result, result_p, 16 );
            } else {
                result_p = (char _WCFAR *)MK_FP( 0x40, 0x164 ) + 8 * __diskinfo->drive;
                _fmemmove( __diskinfo->result, result_p, 8 );
            }
            break;
        }
#else
        psel = 0;
        rseg = 0;
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
            if( _IsRational() ) {
                regs.x.ebx = ( __diskinfo->data_len + 15 ) / 16;  /* paragraph */
                regs.x.eax = 0x100; /* Alloc DOS Memory under DPMI */
                int386( 0x31, &regs, &regs );
                psel = regs.w.dx;
                rseg = regs.w.ax;
                memmove( (char *)( rseg << 4 ), __diskinfo->buffer, __diskinfo->data_len );
            } else if( _IsPharLap() ) {
                regs.x.ebx = ( __diskinfo->data_len + 15 ) / 16;  /* paragraph */
                regs.x.eax = 0x25c0; /* Alloc DOS Memory under Phar Lap */
                intdos( &regs, &regs );
                psel = _ExtenderRealModeSelector;
                rseg = regs.w.ax;
                _fmemmove( MK_FP( psel, rseg << 4 ), __diskinfo->buffer, __diskinfo->data_len );
            }
            break;
        }
        memset( &dr, 0, sizeof( dr ) );
        if( _IsRational() ) {
            dr.ebp = 0;
            /* Set true register structure */
            dr.eax = rregs.x.eax;
            dr.ebx = rregs.x.ebx;
            dr.ecx = rregs.x.ecx;
            dr.edx = rregs.x.edx;
            dr.es = rseg;
            /* int 1BH */
            regs.x.ecx = 0;  /* no stack for now */
            regs.x.edi = (unsigned long)&dr;
            regs.x.eax = 0x300;
            regs.x.ebx = 0x1b;
            int386( 0x31, &regs, &regs );
            ret = dr.ah;
            if( regs.x.cflag ) {
                ret |= 0xff00;
            }
        } else if( _IsPharLap() ) {
            /* Set true register structure */
            dp.eax = rregs.x.eax;
            dp.edx = rregs.x.edx;
            dp.es = rseg;
            /* int 1BH */
            dp.inum = 0x1b;
            _nec_pass_bp( 0x2511, &dp, rregs.x.ebx, rregs.x.ecx, 0, &regs );

            ret = regs.h.ah;
            if( regs.x.cflag ) {
                ret |= 0xff00;
            }
        } else {
            ret = 0;
        }
        switch( __cmd ) {
        case _DISK_SEEK:
            *( (char _WCI86FAR *)__diskinfo->result + 0 ) = dr.cl;
            *( (char _WCI86FAR *)__diskinfo->result + 1 ) = dr.dh;
            *( (char _WCI86FAR *)__diskinfo->result + 2 ) = dr.dl;
            *( (char _WCI86FAR *)__diskinfo->result + 3 ) = dr.ch;
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
                result_p = (char *)0x5d0;
                memmove( __diskinfo->result, result_p, 16 );
            } else {
                result_p = (char *)0x564 + 8 * __diskinfo->drive;
                memmove( __diskinfo->result, result_p, 8 );
            }
            break;
        }

        if( psel ) {
            if( _IsRational() ) {
                memmove( __diskinfo->buffer, (char *)( rseg << 4 ), __diskinfo->data_len);
                regs.x.edx = psel;
                regs.x.eax = 0x101; /* Free DOS Memory under DPMI */
                int386( 0x31, &regs, &regs );
            } else if( _IsPharLap() ) {
                _fmemmove( __diskinfo->buffer, MK_FP( psel, rseg << 4 ), __diskinfo->data_len);
                regs.x.ecx = rseg;
                regs.x.eax = 0x25c1; /* Free DOS Memory under Phar Lap*/
                intdos( &regs, &regs );
            }
        }
#endif
        return( ret );
    }
    /* IBM PC */
    return( 1 );    // fail if not a NEC PC-98 machine
}
