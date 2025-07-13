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
#include "rtdata.h"
#ifdef __386__
    #include "extender.h"
    #include "dpmi.h"
#endif


/*
 * FUNCTION: __nec98_bios_timeofday
 * DESC: the valid services are:
 *      _TIME_GETCLOCK - get the current date and time and store it in info
 *      _TIME_SETCLOCK - set the current date and time using information
 *                       stored in info
 * RETURNS: none
 */

_WCRTLINK unsigned short __nec98_bios_timeofday( unsigned __cmd, char *__timeval )
{
    if( _RWD_isPC98 ) { /* NEC PC-98 */
        switch( __cmd ) {
        case _TIME_GETCLOCK:
        case _TIME_SETCLOCK:
          {
            union REGS      regs;
#ifdef _M_I86
            struct SREGS    segregs;
#else
            dpmi_dos_mem_block  dos_mem;
#endif

#ifdef _M_I86
            segregs.es = FP_SEG( __timeval );
            regs.x.bx = FP_OFF( __timeval );
            regs.h.ah = __cmd;
            int86x( 0x1c, &regs, &regs, &segregs );
#else
            if( _IsPharLap() ) {
                pharlap_regs_struct dp;

                regs.x.ebx = 1;
                regs.x.eax = 0x25c0;
                intdos( &regs, &regs );
                dos_mem.rm = regs.w.ax;
                dos_mem.pm = 0;
                _fmemmove( RealModeSegmPtr( dos_mem.rm ), __timeval, 6 );

                memset( &dp, 0, sizeof( dp ) );
                dp.es = dos_mem.rm;
                regs.x.ebx = 0;     /* Offset */
                dp.r.h.ah = __cmd;
                dp.intno = 0x1c;     /* interrupt no */
                regs.x.edx = (unsigned long)&dp;
                regs.x.eax = 0x2511;
                intdos( &regs, &regs );
                _fmemmove( __timeval, RealModeSegmPtr( dos_mem.rm ), 6 );

                regs.x.ecx = dos_mem.rm;
                regs.x.eax = 0x25c1; /* Free DOS Memory under Phar Lap */
                intdos( &regs, &regs );
            } else if( _DPMI || _IsRational() ) {
                dpmi_regs_struct    dr;

                dos_mem = DPMIAllocateDOSMemoryBlock( 1 );
                _fmemmove( RealModeSegmPtr( dos_mem.rm ), __timeval, 6 );

                memset( &dr, 0, sizeof( dr ) );
                dr.r.h.ah = __cmd;
                dr.es = dos_mem.rm;
                dr.r.x.ebx = 0;         /* Offset */
                regs.x.ebx = 0x1c;  /* interrupt no */
                regs.x.ecx = 0;     /* no stack for now */
                regs.x.edi = (unsigned long)&dr;
                regs.x.eax = 0x300;
                int386( 0x31, &regs, &regs );
                _fmemmove( __timeval, RealModeSegmPtr( dos_mem.rm ), 6 );

                DPMIFreeDOSMemoryBlock( dos_mem.pm );
            }
#endif
            return( 0 );
          }
        default:
            return( -1 );
        }
    }
    /* IBM PC */
    return( 0 );    // fail if not a NEC PC-98 machine
}
