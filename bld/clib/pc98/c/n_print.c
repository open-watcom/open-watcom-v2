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
#include "realmod.h"
#include "roundmac.h"
#include "rtdata.h"
#ifndef _M_I86
    #include "extender.h"
    #include "dpmi.h"
#endif


/*
 * FUNCTION: __nec98_bios_printer
 * DESC: handles output to the printer
 *      _PRINTER_INIT - initializes the printer
 *      _PRINTER_STATUS - gets the status of the printer
 *      _PRINTER_WRITE - sends the character at *data to the printer
 *      _PRINTER_WRITE_STRING - sends the first len characters
 *                      of the string pointed to by data to the printer
 *
 * RETURN:  Only the high byte is significant except for _PRINTER_WRITE_STRING
 *          where the whole word is significant
 *      _PRINTER_INIT - returns 0x10 if ready to recieve data or
 *                      0 otherwise
 *      _PRINTER_STATUS - as _PRINTER_INIT
 *      _PRINTER_WRITE - bit 8 is 1 if successful or 0 otherwise
 *                     - bit 9 is 1 if a time out occured or 0 otherwise
 *      _PRINTER_WRITE_STRING - the number of bytes not sent to the printer
 *                              0 for normal termination
 *                            - if no printer is connected or the printer is
 *                              off 0 is returned.
 *
 */

_WCRTLINK unsigned short __nec98_bios_printer( unsigned __cmd, unsigned char *__data )
{
    if( _RWD_isPC98 ) { /* NEC PC-98 */
        unsigned short      ret;
#ifdef _M_I86
        union REGS          regs;
        struct SREGS        segregs;
#else
        unsigned            len;
        dpmi_dos_mem_block  dos_mem;
        unsigned            size_para;
#endif

        switch( __cmd ) {
        case _PRINTER_WRITE:
        case _PRINTER_INIT:
        case _PRINTER_STATUS:
#ifdef _M_I86
            regs.h.ah = __cmd;
            regs.h.al = ( __cmd == _PRINTER_WRITE ) ? *__data : 0;
            int86( 0x1a, &regs, &regs );
            ret = regs.w.ax;
#else
            if( _IsPharLap() ) {
                pharlap_regs_struct dp;

                memset( &dp, 0, sizeof( dp ) );
                dp.r.h.ah = __cmd;
                if( __cmd == _PRINTER_WRITE )
                    dp.r.h.al = *__data;
                dp.intno = 0x1a;
                PharlapSimulateRealModeInterrupt( &dp, 0, 0, 0 );
                ret = dp.r.h.ah;
            } else if( _DPMI || _IsRational() ) {
                dpmi_regs_struct    dr;

                memset( &dr, 0, sizeof( dr ) );
                dr.r.h.ah = __cmd;
                if( __cmd == _PRINTER_WRITE )
                    dr.r.h.al = *__data;
                DPMISimulateRealModeInterrupt( 0x1a, 0, 0, &dr );
                ret = dr.r.h.ah;
            } else {
                ret = 0;
            }
#endif
            break;
        case _PRINTER_WRITE_STRING:
#ifdef _M_I86
            regs.w.ax = _PRINTER_WRITE_STRING << 8;
            regs.w.cx = strlen( (char *)__data );
            regs.w.bx = FP_OFF( __data );
            segregs.es = FP_SEG( __data );
            int86x( 0x1a, &regs, &regs, &segregs );
            ret = regs.w.cx;
#else
            len = strlen( (char *)__data );
            size_para = ( len > 0xffff ) ? 0x1000 : __ROUND_UP_SIZE_TO_PARA( len );   /* paragraph */
            if( _IsPharLap() ) {
                pharlap_regs_struct dp;

                dos_mem.pm = 0;
                dos_mem.rm = PharlapAllocateDOSMemoryBlock( size_para );
                for( ; len > 0xffff; len -= 0xffff ) {
                    _fmemmove( RealModeSegmPtr( dos_mem.rm ), __data, 0xffff );
                    __data += 0xffff;
                    memset( &dp, 0, sizeof( dp ) );
                    dp.r.x.eax = _PRINTER_WRITE_STRING << 8;
                    dp.r.x.ecx = 0xffff;
                    dp.es = dos_mem.rm;
                    dp.intno = 0x1a;
                    PharlapSimulateRealModeInterruptExt( &dp );
                    if( dp.r.x.eax ) {
                        ret = dp.r.x.ecx;
                        break;
                    }
                }
                _fmemmove( RealModeSegmPtr( dos_mem.rm ), __data, len );
                memset( &dp, 0, sizeof( dp ) );
                dp.r.x.eax = _PRINTER_WRITE_STRING << 8;
                dp.r.x.ecx = len;
                dp.es = dos_mem.rm;
                dp.intno = 0x1a;
                PharlapSimulateRealModeInterruptExt( &dp );
                ret = dp.r.x.ecx;
                if( dos_mem.rm ){
                    PharlapFreeDOSMemoryBlock( dos_mem.rm );
                }
            } else if( _DPMI || _IsRational() ) {
                dpmi_regs_struct    dr;

                dos_mem = DPMIAllocateDOSMemoryBlock( size_para );
                for( ; len > 0xffff; len -= 0xffff ) {
                    _fmemmove( RealModeSegmPtr( dos_mem.rm ), __data, 0xffff );
                    __data += 0xffff;
                    memset( &dr, 0, sizeof( dr ) );
                    dr.r.x.eax = _PRINTER_WRITE_STRING << 8;
                    dr.r.x.ecx = 0xffff;
                    dr.es = dos_mem.rm;
                    DPMISimulateRealModeInterrupt( 0x1a, 0, 0, &dr );
                    if( dr.r.x.eax ) {
                        ret = dr.r.x.ecx;
                        break;
                    }
                }
                _fmemmove( RealModeSegmPtr( dos_mem.rm ), __data, len );
                memset( &dr, 0, sizeof( dr ) );
                dr.r.x.eax = _PRINTER_WRITE_STRING << 8;
                dr.r.x.ecx = len;
                dr.es = dos_mem.rm;
                DPMISimulateRealModeInterrupt( 0x1a, 0, 0, &dr );
                ret = dr.r.x.ecx;
                if( dos_mem.pm ){
                    DPMIFreeDOSMemoryBlock( dos_mem.pm );
                }
            } else {
                ret = 0;
            }
#endif
            break;
        default:
            ret = 0;
            break;
        }
        return( ret );
    }
    /* IBM PC */
    return( 0 );    // fail if not a NEC PC-98 machine
}
