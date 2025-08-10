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
#include "tinyio.h"
#include "int33.h"
#include "rtdata.h"
#include "realmod.h"
#ifdef __386__
    #include "extender.h"
    #include "dpmi.h"
#endif


_WCRTLINK unsigned short __nec98_bios_equiplist( void )
{
    if( _RWD_isPC98 ) { /* NEC PC-98 */
        unsigned short  ret;
        int             count;
        unsigned short  disk_info;
        unsigned char   gpib_info;
        unsigned char   rs232_info;
        void            __far *p1;
        void            __far *p2;

        disk_info  = RealModeData( unsigned short, 0x40,   0x15c );
        gpib_info  = RealModeData( unsigned char,  0xa000, 0x3fee );
        rs232_info = RealModeData( unsigned char,  0xd800, 0 );

        /* Check FPU */
        ret = (unsigned short)_RWD_real87;
        /* Check disk drives */
        for( count = 0; disk_info != 0; disk_info <<= 1 ) {
            if( 0x8000 & disk_info ) {
                count++;
            }
        }
        ret |= count << 3;
        /* Check mouse */
#if defined( _M_I86 )
        p1 = TinyGetVect( 0x33 );
        p2 = TinyGetVect( 0x34 );
#else
        if( _IsPharLap() ) {
            p1 = PharlapGetRealModeInterruptVector( 0x33 );
            p2 = PharlapGetRealModeInterruptVector( 0x34 );
        } else if( _DPMI || _IsRational() ) {
            p1 = DPMIGetRealModeInterruptVector( 0x33 );
            p2 = DPMIGetRealModeInterruptVector( 0x34 );
        } else {
            p1 = p2 = 0;
        }
#endif
        if( p1 != p2 ) {
            if( _BIOSMouseDriverReset() ) {
                ret |= 0x100;
            }
        }
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
