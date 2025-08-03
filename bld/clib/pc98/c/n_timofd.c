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
#ifdef _M_I86
            union REGS      regs;
            struct SREGS    segregs;
#else
            dpmi_dos_mem_block  dos_mem;
            void                __far *p;
#endif

#ifdef _M_I86
            segregs.es = FP_SEG( __timeval );
            regs.x.bx = FP_OFF( __timeval );
            regs.h.ah = __cmd;
            int86x( 0x1c, &regs, &regs, &segregs );
#else
            if( _IsPharLap() ) {
                pharlap_regs_struct dp;

                dos_mem.rm = PharlapAllocateDOSMemoryBlock( 1 );
                dos_mem.pm = 0;
                p = RealModeSegmPtr( dos_mem.rm );
                _fmemmove( p, __timeval, 6 );
                memset( &dp, 0, sizeof( dp ) );
                dp.r.h.ah = __cmd;
                dp.es = dos_mem.rm;
                dp.intno = 0x1c;     /* interrupt no */
                PharlapSimulateRealModeInterrupt( &dp, 0, 0, 0 );
                _fmemmove( __timeval, p, 6 );
                DPMIFreeDOSMemoryBlock( dos_mem.rm );
            } else if( _DPMI || _IsRational() ) {
                dpmi_regs_struct    dr;

                dos_mem = DPMIAllocateDOSMemoryBlock( 1 );
                p = RealModeSegmPtr( dos_mem.rm );
                _fmemmove( p, __timeval, 6 );
                memset( &dr, 0, sizeof( dr ) );
                dr.r.h.ah = __cmd;
                dr.es = dos_mem.rm;
                DPMISimulateRealModeInterrupt( 0x1c, 0, 0, &dr );
                _fmemmove( __timeval, p, 6 );
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
