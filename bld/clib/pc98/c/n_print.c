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
#ifdef __386__
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
        unsigned short  ret;
        union REGS      regs;
#if defined( _M_I86 )
        struct SREGS    segregs;

        switch( regs.h.ah = __cmd ) {
        case _PRINTER_WRITE:
            regs.h.al = *__data;
            /* fall through */
        case _PRINTER_INIT:
        case _PRINTER_STATUS:
            int86x( 0x1a, &regs, &regs, &segregs );
            ret = regs.w.ax;
            break;
        case _PRINTER_WRITE_STRING:
            regs.w.cx = strlen( (char *)__data );
            regs.w.bx = FP_OFF( __data );
            segregs.es = FP_SEG( __data );
            int86x( 0x1a, &regs, &regs, &segregs );
            ret = regs.w.cx;
            break;
        default:
            ret = 0;
            break;
        }
#else
        dpmi_regs_struct    dr;
        pharlap_regs_struct dp;
        unsigned            len;
        dpmi_dos_mem_block  dos_mem;
        unsigned            size_para;

        memset( &dr, 0, sizeof( dr ) );

        switch( regs.h.ah = __cmd ) {
        case _PRINTER_WRITE:
            regs.h.al = *__data;
            /* fall through */
        case _PRINTER_INIT:
        case _PRINTER_STATUS:
            int386( 0x1a, &regs, &regs );
            ret = regs.h.ah;
            break;
        case _PRINTER_WRITE_STRING:
            len = strlen( (char *)__data );
            size_para = ( len > 0xffff ) ? 0x1000 : __ROUND_UP_SIZE_TO_PARA( len );   /* paragraph */
            if( _IsPharLap() ) {
                dos_mem.rm = PharlapAllocateDOSMemoryBlock( size_para );
                dos_mem.pm = 0;
                for( ; len > 0xffff; len -= 0xffff ) {
                    _fmemmove( RealModeSegmPtr( dos_mem.rm ), __data, 0xffff );
                    __data += 0xffff;
                    dp.r.x.eax = 0x3000;
                    regs.x.ecx = 0xffff;
                    dp.es = dos_mem.rm;
                    regs.x.ebx = 0;
                    regs.x.edx = (unsigned long)&dp;
                    regs.x.eax = 0x2511;
                    dp.intno = 0x1a;
                    intdos( &regs, &regs );
                    if( dr.r.x.eax ) {
                        ret = dr.r.w.cx;
                        break;
                    }
                }
                _fmemmove( RealModeSegmPtr( dos_mem.rm ), __data, len );
                dp.r.x.eax = 0x3000;
                regs.x.ecx = len;
                dp.es = dos_mem.rm;
                regs.x.ebx = 0;
                regs.x.edx = (unsigned long)&dr;
                regs.x.eax = 0x2511;
                dp.intno = 0x1a;
                intdos( &regs, &regs );
                ret = regs.w.cx;
                if( dos_mem.rm ){
                    PharlapFreeDOSMemoryBlock( dos_mem.rm );
                }
            } else if( _DPMI || _IsRational() ) {
                dos_mem = DPMIAllocateDOSMemoryBlock( size_para );
                for( ; len > 0xffff; len -= 0xffff ) {
                    _fmemmove( RealModeSegmPtr( dos_mem.rm ), __data, 0xffff );
                    __data += 0xffff;
                    dr.r.x.eax = 0x3000;
                    dr.r.x.ecx = 0xffff;
                    dr.es = dos_mem.rm;
                    dr.r.x.ebx = 0;
                    regs.x.ecx = 0;  /* no stack for now */
                    regs.x.edi = (unsigned long)&dr;
                    regs.x.eax = 0x300;
                    regs.x.ebx = 0x001a;
                    int386( 0x31, &regs, &regs );
                    if( dr.r.x.eax ) {
                        ret = dr.r.w.cx;
                        break;
                    }
                }
                _fmemmove( RealModeSegmPtr( dos_mem.rm ), __data, len );
                dr.r.x.eax = 0x3000;
                dr.r.x.ecx = len;
                dr.es = dos_mem.rm;
                dr.r.x.ebx = 0;
                regs.x.ecx = 0;  /* no stack for now */
                regs.x.edi = (unsigned long)&dr;
                regs.x.eax = 0x300;
                regs.x.ebx = 0x001a;
                int386( 0x31, &regs, &regs );
                if( dos_mem.pm ){
                    DPMIFreeDOSMemoryBlock( dos_mem.pm );
                }
                ret = dr.r.w.cx;
            } else {
                ret = 0;
            }
            break;
        default:
            ret = 0;
            break;
        }
#endif
        return( ret );
    }
    /* IBM PC */
    return( 0 );    // fail if not a NEC PC-98 machine
}
