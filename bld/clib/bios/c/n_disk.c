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
#include <bios98.h>
#include <string.h>
#include "nonibm.h"

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

void _nec_pass_bp( unsigned, RMI_BLK *, unsigned, unsigned, unsigned, union REGS * );
#pragma aux _nec_pass_bp = \
        "push ebp" \
        "mov ebp,esi" \
        "int 0x21" \
        "pop ebp" \
        "mov dword ptr [edi],eax" /* [edi].x.eax,eax */ \
        "rcl eax,1" \
        "and eax,1" \
        "mov dword ptr [edi+24],eax" /* [edi].x.cflag,eax */ \
        parm [eax][edx][ebx][ecx][esi][edi];
#else
unsigned short int1b(unsigned short _ax, unsigned short _dx, unsigned short _bx, unsigned short _cx, unsigned short _bp, unsigned short _es);
#pragma aux int1b = \
        "push bp" \
        "push es" \
        "mov bp,si" \
        "mov es,di" \
        "int 1bh" \
        "mov al,ah" \
        "mov ah,0ffh" \
        "jnc skip" \
        "xor ah,ah" \
        "skip:pop es" \
        "pop bp" \
        parm [ax][dx][bx][cx][si][di] value [ax];
#endif
_WCRTLINK unsigned short __nec98_bios_disk(unsigned __cmd,struct diskinfo_t *__diskinfo)
{
#if defined(__386__)
    DPMI_CALLREGS dr;
    RMI_BLK dp;
    union REGS r, rr;
    struct SREGS rs;
    unsigned long psel = 0, rseg = 0;
    int ret = 0;
    char *result_p;

    if( !__NonIBM )  return( 0x0001 );  // fail if not a NEC 98 machine

    memset( &dr, 0, sizeof( dr ) );
    memset( &rr, 0, sizeof( rr ) );


    rr.h.ah = __cmd;
    rr.h.al = __diskinfo->drive;
    rr.w.dx = __diskinfo->command;
    if( rr.h.dl == _CMD_2D && rr.h.ah == _DISK_FORMATDRIVE )
        rr.h.ah &= 0x7f;
    rr.w.ax |= rr.w.dx;
    rr.w.bx = __diskinfo->data_len;
    if( __diskinfo->command == _CMD_HD ){
        if(__cmd == _DISK_FORMATDRIVE || __cmd == _DISK_FORMATTRACK)
            rr.h.bh = rr.h.bl;
        rr.w.cx = __diskinfo->cylinder;
    } else {
        rr.h.ch = __diskinfo->sector_len;
        rr.h.cl = __diskinfo->cylinder;
    }
    rr.h.dh = __diskinfo->head;
    rr.h.dl = __diskinfo->sector;
    if( __cmd == _DISK_ALTERNATE ) rr.h.dl = 0;

    switch(__cmd) {
      case _DISK_FORMATTRACK:
        if (__diskinfo->command == _CMD_HD) break;
      case _DISK_DIAGNOSTIC:
      case _DISK_WRITE:
      case _DISK_READ:
      case _DISK_ALTERNATE:
      case _DISK_WRITEDDAM:
      case _DISK_READDDAM:
        if( _IsRational() ){
            r.x.ebx = (__diskinfo->data_len + 15)/16;  /* paragraph */
            r.x.eax = 0x100; /* Alloc DOS Memory under DPMI */
            int386( 0x31, &r, &r );
            psel = r.w.dx;
            rseg = r.w.ax;
            memmove( (char *)(rseg<<4), __diskinfo->buffer, __diskinfo->data_len );
        } else if ( _IsPharLap() ) {
            r.x.ebx = (__diskinfo->data_len + 15)/16;  /* paragraph */
            r.x.eax = 0x25c0; /* Alloc DOS Memory under Phar Lap */
            intdos( &r, &r );
            psel = _ExtenderRealModeSelector;
            rseg = r.w.ax;
            _fmemmove( MK_FP(psel, rseg<<4), __diskinfo->buffer, __diskinfo->data_len );
        }
    }
    rs.es = rseg;
    if( _IsRational() ) {
        dr.ebp = 0;
        /* Set true register structure */
        dr.eax = rr.x.eax;
        dr.ebx = rr.x.ebx;
        dr.ecx = rr.x.ecx;
        dr.edx = rr.x.edx;
        dr.es = rs.es;
        /* int 1BH */
        r.x.ecx = 0;  /* no stack for now */
        r.x.edi = (unsigned long) &dr;
        r.x.eax = 0x300;
        r.x.ebx = 0x1b;
        int386( 0x31, &r, &r );

        ret = ( dr.eax >> 8 ) & 0xff;
        if( r.x.cflag ) ret |= 0xff00;
    }else if( _IsPharLap() ) {
        /* Set true register structure */
        dp.eax = rr.x.eax;
        dp.edx = rr.x.edx;
        dp.es = rs.es;
        /* int 1BH */
        dp.inum = 0x1b;
        _nec_pass_bp( 0x2511, &dp, rr.x.ebx, rr.x.ecx, 0, &r );

        ret = ( r.x.eax >> 8 ) & 0xff;
        if( r.x.cflag ) ret |= 0xff00;
    }
    switch( __cmd ){
      case _DISK_SEEK:
        *(char *)__diskinfo->result = dr.ecx & 0xff;
        *((char *)__diskinfo->result + 1 ) = (dr.edx >> 8 ) & 0xff;
        *((char *)__diskinfo->result + 2 ) = dr.edx & 0xff;
        *((char *)__diskinfo->result + 3 ) = (dr.ecx >> 8 ) & 0xff;
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
        if( __diskinfo->command == _CMD_2DD ){
            result_p = (char *)0x5d0;
            memmove( __diskinfo->result, result_p, 16 );
        } else {
            result_p = (char *)0x564 + 8 * __diskinfo->drive;
            memmove( __diskinfo->result, result_p, 8 );
        }
    }

    if( psel ){
        if( _IsRational() ){
            memmove( __diskinfo->buffer, (char *)(rseg<<4), __diskinfo->data_len);
            r.x.edx = psel;
            r.x.eax = 0x101; /* Free DOS Memory under DPMI */
            int386( 0x31, &r, &r );
        } else if( _IsPharLap() ){
            _fmemmove( __diskinfo->buffer, MK_FP( psel, rseg<<4 ), __diskinfo->data_len);
            r.x.ecx = rseg;
            r.x.eax = 0x25c1; /* Free DOS Memory under Phar Lap*/
            intdos( &r, &r );
        }
    }
    return ret;
#else
    unsigned short _ax, _bx, _cx, _dx, _es, _bp;
    int ret = 0;

    char _WCFAR * result_p;

    if( !__NonIBM )  return( 0x0001 );  // fail if not a NEC 98 machine

    _ax = ((unsigned short) __cmd << 8) + __diskinfo->drive;
    _dx = __diskinfo->command;
    if( (_dx & 0xff) == _CMD_2D && ((_ax >> 8) & 0xff) == _DISK_FORMATDRIVE )
        _ax &= 0x7fff;
    _ax |= _dx;
    _bx = __diskinfo->data_len;
    if( __diskinfo->command == _CMD_HD ){
        if(__cmd == _DISK_FORMATDRIVE || __cmd == _DISK_FORMATTRACK)
            _bx <<= 8;
        _cx = __diskinfo->cylinder;
    } else {
        _cx = ((unsigned short)__diskinfo->sector_len<<8) + __diskinfo->cylinder;
    }
    _dx = ((unsigned short)__diskinfo->head << 8) + __diskinfo->sector;
    if( __cmd == _DISK_ALTERNATE ) _dx &= 0xff00;

    _es = FP_SEG(__diskinfo->buffer);
    _bp = FP_OFF(__diskinfo->buffer);
    ret = int1b(_ax,_dx,_bx,_cx,_bp,_es);
    switch( __cmd ){
      case _DISK_SEEK:
        *(char *)__diskinfo->result = _cx & 0xff;
        *((char *)__diskinfo->result + 1 ) = (_dx >> 8 ) & 0xff;
        *((char *)__diskinfo->result + 2 ) = _dx & 0xff;
        *((char *)__diskinfo->result + 3 ) = (_cx >> 8 ) & 0xff;
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
        if( __diskinfo->command == _CMD_2DD ){
            result_p = (char _WCFAR *)MK_FP(0x40, 0x1d0);
            _fmemmove( __diskinfo->result, result_p, 16 );
        } else {
            result_p = (char _WCFAR *)MK_FP(0x40, 0x164) + 8 * __diskinfo->drive;
            _fmemmove( __diskinfo->result, result_p, 8 );
        }
    }

    return ret;
#endif
}
