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
#include "ispc98.h"
#include "realmod.h"
#ifdef __386__
    #include "extender.h"
#endif

_WCRTLINK unsigned short __nec98_bios_serialcom( unsigned __cmd, unsigned __port, struct com_t *__data )
{
    if( __isPC98 ) {    /* NEC PC-98 */
#ifdef _M_I86
        union REGS r;
        struct SREGS s;
        int int_no;
        unsigned short _WCFAR *vect_src;
        int ex_port;

        ex_port = *(unsigned char _WCI86FAR *)MK_FP( 0xa000, 0x3fee ) & 0x10;
        switch( __cmd ) {
        case _COM_INIT:
        case _COM_INITX:
            if( __port != _COM_CH1 ) {
                if( ex_port ) {
                    int_no = 0xd4;
                    vect_src = MK_FP( 0xd000, 0x806 );
                    if( __port == _COM_CH3 ) {
                        vect_src += 2; /* Add 4 bytes because of short is 2 bytes*/
                        int_no++;
                    }
                    /*** Vecttor set / segment : d000h ***/
                    r.h.ah = 0x25;
                    r.h.al = int_no;
                    s.ds = 0xd000;
                    r.x.dx = *vect_src;
                    intdosx( &r, &r, &s );
                } else {
                    return( 0xff00 );
                }
            }
            s.es = FP_SEG( __data->buffer );
            r.x.di = FP_OFF( __data->buffer );
            r.h.ah = __cmd;
            if( __data->baud == _COM_DEFAULT ) {
                r.h.al = _COM_1200;
            } else {
                r.h.al = __data->baud;
            }
            r.h.bh = __data->tx_time;
            r.h.bl = __data->rx_time;
            if( __data->mode == 0xff ) {
                r.h.ch = (_COM_STOP1 | _COM_CHR7 | 0x02);
            } else {
                r.h.ch = __data->mode | 0x02;
            }
            if( __data->command == 0xff ) {
                r.h.cl = (_COM_ER | _COM_RXEN | _COM_TXEN);
            } else {
                r.h.cl = __data->command;
            }
            r.x.dx = __data->size;
            break;
        case _COM_SEND:
            r.h.ah = __cmd;
            r.h.al = *(unsigned char _WCI86FAR *)(__data->buffer);
            break;
        case _COM_COMMAND:
            r.h.ah = __cmd;
            if( __data->command == 0xff ) {
                r.h.al = (_COM_ER | _COM_RXEN | _COM_TXEN);
            } else {
                r.h.al = __data->command;
            }
            break;
        default:
            r.h.ah = __cmd;
            r.h.al = 0;
            break;
        }
        switch( __port ) {
        case _COM_CH1:
            int_no = 0x19;  /* int no */
            break;
        case _COM_CH2:
            int_no = 0xd4;  /* int no */
            break;
        case _COM_CH3:
            int_no = 0xd5;  /* int no */
            break;
        }
        int86x( int_no, &r, &r, &s );

        switch( __cmd ) {
        case _COM_GETDTL:
            if( __data )
                __data->size = r.x.cx;
            break;
        case _COM_RECEIVE:
        case _COM_STATUS:
            if( __data ) {
                *(unsigned char _WCI86FAR *)__data->buffer = r.h.ch;
                *((unsigned char _WCI86FAR *)__data->buffer + 1) = r.h.cl;
            }
            break;
        }
        return( r.h.ah );
#else
        union REGS              r;
        call_struct             dr;
        rmi_struct              dp;
        /* Add psel2,psel3 by M/M 30.May.94 */
        static unsigned long    psel = 0;
        static unsigned long    psel2 = 0;
        static unsigned long    psel3 = 0;
        /* Add rseg2,rseg3 by M/M 30.May.94 */
        static unsigned long    rseg = 0;
        static unsigned long    rseg2 = 0;
        static unsigned long    rseg3 = 0;
        int int_no;
        unsigned short _WCFAR   *vect_src;
        int                     ex_port;

        ex_port = 0;
        if( _ExtenderRealModeSelector ) {
            ex_port = *(unsigned char _WCFAR *)MK_FP( _ExtenderRealModeSelector, 0xa3fee ) & 0x10;
        }

        memset( &dr, 0, sizeof( dr ) );
        switch( __cmd ) {
        case _COM_INIT:
        case _COM_INITX:
            if( __port != _COM_CH1 ) {
                if( ex_port ) {
                    int_no = 0xd4;
                    vect_src = MK_FP( _ExtenderRealModeSelector, 0xd0806 );
                    if( _IsRational() ) {
                        if( __port == _COM_CH3 ) {
                            vect_src += 2; /* Add 4 bytes because of short is 2 bytes*/
                            int_no++;
                            if( psel3 ) { /* Allocate BIOS buffer for port 3 by M/M 30.May.94 */
                                r.x.edx = psel3;
                                r.x.eax = 0x101; /* DPMI DOS Memory Free */
                                int386( 0x31, &r, &r );
                            }
                            r.x.ebx = ( (long)__data->size + 2 + 18 + 15 ) / 16;  /* paragraph */
                            /* Fix : Add the size of interface are and buffer control block by M/M 01.Jun.94 */
                            r.x.eax = 0x100; /* DPMI DOS Memory Alloc */
                            int386( 0x31, &r, &r );
                            if ( r.x.cflag )
                                return( 0xff00 ); /* by M/M 01.Jun.94 */
                            psel3 = r.w.dx;
                            rseg3 = r.w.ax;
                            dr.es = rseg3;
                            dr.edi =  0;
                        } else { /* Allocate BIOS buffer for port 2 by M/M 30.May.94 */
                            if( psel2 ) {
                                r.x.edx = psel2;
                                r.x.eax = 0x101; /* DPMI DOS Memory Free */
                                int386( 0x31, &r, &r );
                            }
                            r.x.ebx = ( (long)__data->size + 2 + 18 + 15 ) / 16;  /* paragraph */
                            /* Fix : Add the size of interface are and buffer control block by M/M 01.Jun.94 */
                            r.x.eax = 0x100; /* DPMI DOS Memory Alloc */
                            int386( 0x31, &r, &r );
                            if ( r.x.cflag )
                                return( 0xff00 ); /* by M/M 01.Jun.94 */
                            psel2 = r.w.dx;
                            rseg2 = r.w.ax;
                            dr.es = rseg2;
                            dr.edi =  0;
                        }
                        /*** Vecttor set / segment : d000h ***/
                        r.x.eax = 0x201;
                        r.h.bl = int_no;
                        r.x.ecx = 0xd000;
                        r.x.edx = *vect_src;
                        int386( 0x31, &r, &r );
                    } else if( _IsPharLap() ) {
                        if( __port == _COM_CH3 ) {
                            vect_src += 2; /* Add 4 bytes because of short is 2 bytes*/
                            int_no++;
                            if( psel3 ) {/* Allocate BIOS buffer for port 3 by M/M 30.May.94 */
                                r.x.ecx = rseg3;
                                r.x.eax = 0x25c1; /* Free DOS Memory under Phar Lap */
                                intdos( &r, &r );
                            }
                            r.x.ebx = ( (long)__data->size + 2 + 18 + 15 ) / 16;  /* paragraph */
                            /* Fix : Add the size of interface are and buffer control block by M/M 01.Jun.94 */
                            r.x.eax = 0x25c0; /* Alloc DOS Memory under Phar Lap */
                            intdos( &r, &r );
                            if ( r.x.cflag )
                                return( 0xff00 ); /* by M/M 01.Jun.94 */
                            psel3 = 0x34;
                            rseg3 = r.w.ax;
                            dr.es = rseg3;
                            dr.edi =  0;
                        } else { /* Allocate BIOS buffer for port 2 by M/M 30.May.94 */
                            if( psel2 ) {
                                r.x.ecx = rseg2;
                                r.x.eax = 0x25c1; /* Free DOS Memory under Phar Lap */
                                intdos( &r, &r );
                            }
                            r.x.ebx = ( (long)__data->size + 2 + 18 + 15 ) / 16;  /* paragraph */
                            /* Fix : Add the size of interface are and buffer control block by M/M 01.Jun.94 */
                            r.x.eax = 0x25c0; /* Alloc DOS Memory under Phar Lap */
                            intdos( &r, &r );
                            if ( r.x.cflag )
                                return( 0xff00 ); /* by M/M 01.Jun.94 */
                            psel2 = 0x34;
                            rseg2 = r.w.ax;
                            dr.es = rseg2;
                            dr.edi =  0;
                        }
                        /*** Vecttor set / segment : d000h ***/
                        r.x.eax = 0x2505;
                        r.h.cl = int_no;
                        r.x.ebx = 0xd0000000 + *vect_src;
                        intdos( &r, &r );
                    }
                } else {
                    return( 0xff00 );
                }
            } else { /* Allocate BIOS buffer for port 1 by M/M 30.May.94 */
                /*** Need realloc ***/ /* Move from the below to here */
                if( _IsRational() ) {
                    if ( psel ) {
                        r.x.edx = psel;
                        r.x.eax = 0x101; /* DPMI DOS Memory Free */
                        int386( 0x31, &r, &r );
                    }

                    r.x.ebx = ( (long)__data->size + 2 + 18 + 15 ) / 16;  /* paragraph */
                    /* Fix : Add the size of interface are and buffer control block by M/M 01.Jun.94 */
                    r.x.eax = 0x100; /* DPMI DOS Memory Alloc */
                    int386( 0x31, &r, &r );
                    if ( r.x.cflag )
                        return( 0xff00 ); /* by M/M 01.Jun.94 */
                    psel = r.w.dx;
                    rseg = r.w.ax;
                    /* memmove( (char *)( rseg << 4 ), __data->buffer, __data->size ); */
                    /* No need to trasfer data by M/M 30.May.94 */
                } else if( _IsPharLap() ) {
                    if( psel ) {
                        r.x.ecx = rseg;
                        r.x.eax = 0x25c1; /* Free DOS Memory under Phar Lap */
                        intdos( &r, &r );
                    }

                    r.x.ebx = ( (long)__data->size + 2 + 18 + 15 ) / 16;  /* paragraph */
                    /* Fix : Add the size of interface are and buffer control block by M/M 01.Jun.94 */
                    r.x.eax = 0x25c0; /* Alloc DOS Memory under Phar Lap */
                    intdos( &r, &r );
                    if ( r.x.cflag )
                        return( 0xff00 ); /* by M/M 01.Jun.94 */
                    psel = 0x34;
                    rseg = r.w.ax;
                    /* _fmemmove( MK_FP( psel, rseg << 4 ), __data->buffer, __data->size ); */
                    /* No need to trasfer data by M/M 30.May.94 */
                }
                dr.es = rseg;
                dr.edi =  0;
            }
            dr.ah = __cmd;
            if( __data->baud == _COM_DEFAULT ) {
                dr.al = _COM_1200;
            } else {
                dr.al = __data->baud;
            }
            dr.bh = __data->tx_time;
            dr.bl = __data->rx_time;
            if( __data->mode == 0xff ) {
                dr.ch = (_COM_STOP1 | _COM_CHR7 | 0x02);
            } else {
                dr.ch = __data->mode | 0x02;
            }
            if( __data->command == 0xff ) {
                dr.cl = (_COM_ER | _COM_RXEN | _COM_TXEN);
            } else {
                dr.cl = __data->command;
            }
            dr.edx = __data->size;
            break;
        case _COM_SEND:
            dr.ah = __cmd;
            dr.al = *(unsigned char *)(__data->buffer);
            break;
        case _COM_COMMAND:
            dr.ah = __cmd;
            if( __data->command == 0xff ) {
                dr.al = (_COM_ER | _COM_RXEN | _COM_TXEN);
            } else {
                dr.al = __data->command;
            }
            break;
        default:
            dr.ah = __cmd;
            break;
        }
        switch( __port ) {
        case _COM_CH1:
            r.x.ebx = 0x19;  /* int no */
            break;
        case _COM_CH2:
            r.x.ebx = 0xd4;  /* int no */
            break;
        case _COM_CH3:
            r.x.ebx = 0xd5;  /* int no */
            break;
        }
        if( _IsRational() ) {
            r.x.ecx = 0;  /* no stack for now */
            r.x.edi = (unsigned long)&dr;
            r.x.eax = 0x300;
            int386( 0x31, &r, &r );
        } else if( _IsPharLap() ) {
            dp.eax = dr.eax;
            dp.edx = dr.edx;
            dp.ds = dr.ds;
            dp.es = dr.es;
            dp.fs = dr.fs;
            dp.gs = dr.gs;
            dp.inum = r.x.ebx;
            r.x.eax = 0x2511;
            r.x.edx = (unsigned long)&dp;
            r.x.ebx = dr.ebx;
            r.x.ecx = dr.ecx;
            r.x.edi = dr.edi;
            r.x.esi = dr.esi;
            intdos( &r, &r );
            dr.ecx = r.x.ecx;
            dr.eax = r.x.eax;
        }
        switch( __cmd ) {
        case _COM_GETDTL:
            if( __data )
                __data->size = dr.ecx;
            break;
        case _COM_RECEIVE:
        case _COM_STATUS:
            if( __data ) {
                *( (unsigned char *)__data->buffer + 0 ) = dr.ch;
                *( (unsigned char *)__data->buffer + 1 ) = dr.cl;
            }
            break;
        }
        return( dr.ah );
#endif
    }
    /* IBM PC */
    return( 0 );    // fail if not a NEC PC-98 machine
}
