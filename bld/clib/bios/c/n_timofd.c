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
#include <string.h>
#include <bios98.h>
#include "tinyio.h"
#include "realmod.h"
#include "rtdata.h"
#ifdef __386__
    #include "extender.h"
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
            union REGS      r;
#ifdef _M_I86
            struct SREGS    s;

            s.es = FP_SEG( __timeval );
            r.x.bx = FP_OFF( __timeval );
            r.h.ah = __cmd;
            int86x( 0x1c, &r, &r, &s );
#else
            unsigned long   psel;
            unsigned long   rseg;

            if( _IsRational() ) {
                call_struct     dr;

                memset( &dr, 0, sizeof( dr ) );
                r.x.ebx = 1;  /* paragraph */
                r.x.eax = 0x100; /* DPMI DOS Memory Alloc */
                int386( 0x31, &r, &r );
                psel = r.w.dx;
                rseg = r.w.ax;
                memmove( (void *)( rseg << 4 ), __timeval, 6 );

                dr.es = rseg;
                dr.ebx = 0; /* Offset */
                dr.ah = __cmd;
                r.x.ebx = 0x1c;  /* int no */
                r.x.ecx = 0;  /* no stack for now */
                r.x.edi = (unsigned long)&dr;
                r.x.eax = 0x300;
                int386( 0x31, &r, &r );
                memmove( __timeval, (void *)( rseg << 4 ), 6 );

                r.x.edx = psel;
                r.x.eax = 0x101; /* DPMI DOS Memory Free */
                int386( 0x31, &r, &r );
            } else if( _IsPharLap() ) {
                rmi_struct      dp;

                memset( &dp, 0, sizeof( dp ) );
                r.x.ebx = 1;
                r.x.eax = 0x25c0;
                intdos( &r, &r );
                psel = _ExtenderRealModeSelector;
                rseg = r.w.ax;
                _fmemmove( MK_FP( psel, rseg << 4 ), __timeval, 6 );

                dp.es = rseg;
                r.x.ebx = 0; /* Offset */
                dp.ah = __cmd;
                dp.inum = 0x1c;  /* int no */
                r.x.edx = (unsigned long)&dp;
                r.x.eax = 0x2511;
                intdos( &r, &r );
                _fmemmove( __timeval, MK_FP( psel, rseg << 4 ), 6 );

                r.x.ecx = rseg;
                r.x.eax = 0x25c1; /* Free DOS Memory under Phar Lap */
                intdos( &r, &r );
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
