/****************************************************************************
*
*                            Open Watcom Project
*
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
#include "nonibm.h"

/*
 * FUNCTION: __nec98_bios_timeofday
 * DESC: the valid services are:
 *      _TIME_GETCLOCK - get the current date and time and store it in info
 *      _TIME_SETCLOCK - set the current date and time using information
 *                       stored in info
 * RETURNS: none
 */

#ifdef __386__
#include "extender.h"

/* Declaration for DOS/4G */
typedef unsigned long DWORD;
typedef unsigned short WORD;
typedef struct _dpmi_callregs
{
    DWORD edi, esi, ebp, esp;
    DWORD ebx, edx, ecx, eax;
    WORD flags;
    WORD es, ds;
    WORD fs, gs;
    WORD ip, cs;
    WORD sp, ss;
} DPMI_CALLREGS;

/* Declaration for Phar Lap */
typedef unsigned long ULONG;
typedef unsigned short USHORT;
#pragma pack(__push,1);
typedef struct
{
    USHORT inum;      /* Interrupt number */
    USHORT ds;        /* DS register */
    USHORT es;        /* ES register */
    USHORT fs;        /* FS register */
    USHORT gs;        /* GS register */
    ULONG eax;        /* EAX register */
    ULONG edx;        /* EDX register */
} RMI_BLK;
#pragma pack(__pop);
#endif

_WCRTLINK unsigned short __nec98_bios_timeofday( int __cmd, char *__timeval )
{
#ifdef __386__
    union REGS r;
    DPMI_CALLREGS dr;
    RMI_BLK dp;
    unsigned long psel, rseg;

    if( !__NonIBM )  return( 0x0000 );  // fail if not a NEC 98 machine

    memset( &dr, 0, sizeof( dr ) );

    if( (__cmd != 0) && (__cmd != 1) ) return -1;

    if( _IsRational() ) {
        r.x.ebx = 1;  /* paragraph */
        r.x.eax = 0x100; /* DPMI DOS Memory Alloc */
        int386( 0x31, &r, &r );
        psel = r.w.dx;
        rseg = r.w.ax;
        memmove( (void *)(rseg<<4), __timeval, 6 );

        dr.es = rseg;
        dr.ebx = 0; /* Offset */
        dr.eax = __cmd << 8;
        r.x.ebx = 0x1c;  /* int no */
        r.x.ecx = 0;  /* no stack for now */
        r.x.edi = (unsigned long) &dr;
        r.x.eax = 0x300;
        int386( 0x31, &r, &r );
        memmove( __timeval, (void *)(rseg<<4), 6 );

        r.x.edx = psel;
        r.x.eax = 0x101; /* DPMI DOS Memory Free */
        int386( 0x31, &r, &r );
    } else if( _IsPharLap() ) {
        r.x.ebx = 1;
        r.x.eax = 0x25c0;
        intdos( &r, &r );
        psel = _ExtenderRealModeSelector;
        rseg = r.w.ax;
        _fmemmove( MK_FP( psel, rseg<<4 ), __timeval, 6 );

        dp.es = rseg;
        r.x.ebx = 0; /* Offset */
        dp.eax = __cmd << 8;
        dp.inum = 0x1c;  /* int no */
        r.x.edx = (unsigned long) &dp;
        r.x.eax = 0x2511;
        intdos( &r, &r );
        _fmemmove( __timeval, MK_FP( psel, rseg<<4 ), 6 );

        r.x.ecx = rseg;
        r.x.eax = 0x25c1; /* Free DOS Memory under Phar Lap */
        intdos( &r, &r );
    }
    return 0;
#else
    union REGS r;
    struct SREGS s;

    if( !__NonIBM )  return( 0x0000 );  // fail if not a NEC 98 machine

    if( (__cmd != 0) && (__cmd != 1) ) return -1;

    s.es = FP_SEG(__timeval);
    r.x.bx = FP_OFF(__timeval);
    r.h.ah = __cmd;
    int86x( 0x1c, &r, &r, &s );
    return 0;
#endif
}
