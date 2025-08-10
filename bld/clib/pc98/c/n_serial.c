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
#include <string.h>
#include <bios98.h>
#include "tinyio.h"
#include "roundmac.h"
#include "realmod.h"
#include "rtdata.h"
#ifdef __386__
    #include "extender.h"
    #include "dpmi.h"
#endif


_WCRTLINK unsigned short __nec98_bios_serialcom( unsigned __cmd, unsigned __port, struct com_t *__data )
{
    if( _RWD_isPC98 ) { /* NEC PC-98 */
        union REGS                  regs;
        int                         intno;
        unsigned short              _WCFAR *vect_src;
#ifdef _M_I86
        struct SREGS                segregs;
#else
        dpmi_regs_struct            dr;
        pharlap_regs_struct         dp;
        static dpmi_dos_mem_block   dos_mem1 = { 0, 0 };
        static dpmi_dos_mem_block   dos_mem2 = { 0, 0 };
        static dpmi_dos_mem_block   dos_mem3 = { 0, 0 };
        unsigned                    size_para;
#endif

        memset( &regs, 0, sizeof( regs ) );
#ifdef _M_I86
        memset( &segregs, 0, sizeof( segregs ) );
#else
        if( _IsPharLap() ) {
            memset( &dp, 0, sizeof( dp ) );
        } else if( _DPMI || _IsRational() ) {
            memset( &dr, 0, sizeof( dr ) );
        }
#endif
        switch( __cmd ) {
        case _COM_INIT:
        case _COM_INITX:
#ifndef _M_I86
            /* Fix : Add the size of interface are and buffer control block */
            size_para = __ROUND_UP_SIZE_TO_PARA( (long)__data->size + 2 + 18 );
#endif
            if( __port == _COM_CH1 ) {  /* port 1 */
                intno = 0x19;
#ifdef _M_I86
                /* no changes, default setup */
#else
                /*** Need realloc ***/ /* Move from the below to here */
                if( _IsPharLap() ) {
                    if( dos_mem1.rm ) { /* Allocate BIOS buffer for port 1 */
                        PharlapFreeDOSMemoryBlock( dos_mem1.rm );
                    }
                    dos_mem1.rm = PharlapAllocateDOSMemoryBlock( size_para );
                    if( dos_mem1.rm == 0 ) {
                        return( 0xff00 );
                    }
                    dp.es = dos_mem1.rm;
                } else if( _DPMI || _IsRational() ) {
                    if( dos_mem1.pm ) { /* Allocate BIOS buffer for port 1 */
                        DPMIFreeDOSMemoryBlock( dos_mem1.pm );
                    }
                    dos_mem1 = DPMIAllocateDOSMemoryBlock( size_para );
                    if( dos_mem1.pm == 0 ) {
                        return( 0xff00 );
                    }
                    dr.es = dos_mem1.rm;
                }
#endif
            } else {                    /* port 2 and 3 */
                if( (RealModeData( unsigned char, 0xa000, 0x3fee ) & 0x10) == 0 )
                    return( 0xff00 );
                intno = 0xd4;
                vect_src = RealModeDataPtr( 0xd000, 0x806 );
                if( __port == _COM_CH3 ) {
                    vect_src += 2; /* Add 4 bytes because of short is 2 bytes*/
                    intno++;
                }
                /*** Vector set / segment : d000h ***/
#ifdef _M_I86
                TinySetVect( intno, MK_FP( 0xd000, *vect_src ) );
#else
                if( _IsPharLap() ) {
                    if( __port == _COM_CH3 ) {
                        if( dos_mem3.rm ) {/* Allocate BIOS buffer for port 3 */
                            PharlapFreeDOSMemoryBlock( dos_mem3.rm );
                        }
                        dos_mem3.rm = PharlapAllocateDOSMemoryBlock( size_para );
                        if( dos_mem3.rm == 0 )
                            return( 0xff00 );
                        dp.es = dos_mem3.rm;
                    } else { /* Allocate BIOS buffer for port 2 */
                        if( dos_mem2.rm ) {
                            PharlapFreeDOSMemoryBlock( dos_mem2.rm );
                        }
                        dos_mem2.rm = PharlapAllocateDOSMemoryBlock( size_para );
                        if( dos_mem2.rm == 0 )
                            return( 0xff00 );
                        dp.es = dos_mem2.rm;
                    }
                    /*** Vector set / segment : d000h ***/
                    PharlapSetRealModeInterruptVector( intno, MK_FP( 0xd000, *vect_src ) );
                } else if( _DPMI || _IsRational() ) {
                    if( __port == _COM_CH3 ) {
                        if( dos_mem3.pm ) { /* Allocate BIOS buffer for port 3 */
                            DPMIFreeDOSMemoryBlock( dos_mem3.pm );
                        }
                        dos_mem3 = DPMIAllocateDOSMemoryBlock( size_para );
                        if( dos_mem3.pm == 0 )
                            return( 0xff00 );
                        dr.es = dos_mem3.rm;
                    } else { /* Allocate BIOS buffer for port 2 */
                        if( dos_mem2.pm ) {
                            DPMIFreeDOSMemoryBlock( dos_mem2.pm );
                        }
                        dos_mem2 = DPMIAllocateDOSMemoryBlock( size_para );
                        if( dos_mem2.pm == 0 )
                            return( 0xff00 );
                        dr.es = dos_mem2.rm;
                    }
                    /*** Vector set / segment : d000h ***/
                    DPMISetRealModeInterruptVector( intno, MK_FP( 0xd000, *vect_src ) );
                }
#endif
            }
            regs.w.dx = __data->size;
            regs.h.ah = __cmd;
            if( __data->baud == _COM_DEFAULT ) {
                regs.h.al = _COM_1200;
            } else {
                regs.h.al = __data->baud;
            }
            regs.h.bh = __data->tx_time;
            regs.h.bl = __data->rx_time;
            if( __data->mode == 0xff ) {
                regs.h.ch = (_COM_STOP1 | _COM_CHR7 | 0x02);
            } else {
                regs.h.ch = __data->mode | 0x02;
            }
            if( __data->command == 0xff ) {
                regs.h.cl = (_COM_ER | _COM_RXEN | _COM_TXEN);
            } else {
                regs.h.cl = __data->command;
            }
            break;
        case _COM_SEND:
            regs.h.ah = __cmd;
            regs.h.al = *(unsigned char _WCI86FAR *)(__data->buffer);
            break;
        case _COM_COMMAND:
            regs.h.ah = __cmd;
            if( __data->command == 0xff ) {
                regs.h.al = (_COM_ER | _COM_RXEN | _COM_TXEN);
            } else {
                regs.h.al = __data->command;
            }
            break;
        default:
            regs.h.ah = __cmd;
            regs.h.al = 0;
            break;
        }
#ifdef _M_I86
        segregs.es = FP_SEG( __data->buffer );
        regs.w.di = FP_OFF( __data->buffer );
        int86x( intno, &regs, &regs, &segregs );
#else
        if( _IsPharLap() ) {
            dp.r.x.eax = regs.w.ax;
            dp.r.x.edx = regs.w.dx;
            dp.r.x.ebx = regs.w.bx;
            dp.r.x.ecx = regs.w.cx;
            dp.intno = intno;
            PharlapSimulateRealModeInterruptExt( &dp );
            regs.w.cx = dp.r.w.cx;
            regs.w.ax = dp.r.w.ax;
        } else if( _DPMI || _IsRational() ) {
            dr.r.x.eax = regs.w.ax;
            dr.r.x.edx = regs.w.dx;
            dr.r.x.ebx = regs.w.bx;
            dr.r.x.ecx = regs.w.cx;
            DPMISimulateRealModeInterrupt( intno, 0, 0, &dr );
            regs.w.cx = dr.r.w.cx;
            regs.w.ax = dr.r.w.ax;
        }
#endif
        switch( __cmd ) {
        case _COM_GETDTL:
            if( __data )
                __data->size = regs.w.cx;
            break;
        case _COM_RECEIVE:
        case _COM_STATUS:
            if( __data ) {
                ((unsigned char _WCI86FAR *)__data->buffer)[0] = regs.h.ch;
                ((unsigned char _WCI86FAR *)__data->buffer)[1] = regs.h.cl;
            }
            break;
        }
        return( regs.h.ah );
    }
    /* IBM PC */
    return( 0 );    // fail if not a NEC PC-98 machine
}
