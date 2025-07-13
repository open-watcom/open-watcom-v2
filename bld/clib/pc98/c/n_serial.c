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
        union REGS              regs;
        int                     intno;
        unsigned short _WCFAR   *vect_src;
        int                     ex_port;
#ifdef _M_I86
        struct SREGS            segregs;
#else
        dpmi_regs_struct        dr;
        pharlap_regs_struct     dp;
        /* Add psel2,psel3 by M/M 30.May.94 */
        static unsigned long    psel = 0;
        static unsigned long    psel2 = 0;
        static unsigned long    psel3 = 0;
        /* Add rseg2,rseg3 by M/M 30.May.94 */
        static unsigned long    rseg = 0;
        static unsigned long    rseg2 = 0;
        static unsigned long    rseg3 = 0;
        unsigned                size_para;
#endif

#ifdef _M_I86
        ex_port = *(unsigned char _WCI86FAR *)MK_FP( 0xa000, 0x3fee ) & 0x10;
        switch( __cmd ) {
        case _COM_INIT:
        case _COM_INITX:
            if( __port != _COM_CH1 ) {
                if( ex_port ) {
                    intno = 0xd4;
                    vect_src = MK_FP( 0xd000, 0x806 );
                    if( __port == _COM_CH3 ) {
                        vect_src += 2; /* Add 4 bytes because of short is 2 bytes*/
                        intno++;
                    }
                    /*** Vecttor set / segment : d000h ***/
                    regs.h.ah = 0x25;
                    regs.h.al = intno;
                    segregs.ds = 0xd000;
                    regs.x.dx = *vect_src;
                    intdosx( &regs, &regs, &segregs );
                } else {
                    return( 0xff00 );
                }
            }
            segregs.es = FP_SEG( __data->buffer );
            regs.x.di = FP_OFF( __data->buffer );
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
            regs.x.dx = __data->size;
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
        switch( __port ) {
        case _COM_CH1:
            intno = 0x19;  /* interrupt no */
            break;
        case _COM_CH2:
            intno = 0xd4;  /* interrupt no */
            break;
        case _COM_CH3:
            intno = 0xd5;  /* interrupt no */
            break;
        }
        int86x( intno, &regs, &regs, &segregs );

        switch( __cmd ) {
        case _COM_GETDTL:
            if( __data )
                __data->size = regs.x.cx;
            break;
        case _COM_RECEIVE:
        case _COM_STATUS:
            if( __data ) {
                *(unsigned char _WCI86FAR *)__data->buffer = regs.h.ch;
                *((unsigned char _WCI86FAR *)__data->buffer + 1) = regs.h.cl;
            }
            break;
        }
        return( regs.h.ah );
#else
        ex_port = 0;
        if( _ExtenderRealModeSelector ) {
            ex_port = *(unsigned char _WCFAR *)MK_FP( _ExtenderRealModeSelector, 0xa3fee ) & 0x10;
        }

        memset( &dr, 0, sizeof( dr ) );
        switch( __cmd ) {
        case _COM_INIT:
        case _COM_INITX:
            size_para = __ROUND_UP_SIZE_TO_PARA( (long)__data->size + 2 + 18 );
            if( __port != _COM_CH1 ) {
                if( ex_port ) {
                    intno = 0xd4;
                    vect_src = MK_FP( _ExtenderRealModeSelector, 0xd0806 );
                    if( __port == _COM_CH3 ) {
                        vect_src += 2; /* Add 4 bytes because of short is 2 bytes*/
                        intno++;
                    }
                    if( _IsPharLap() ) {
                        if( __port == _COM_CH3 ) {
                            if( psel3 ) {/* Allocate BIOS buffer for port 3 by M/M 30.May.94 */
                                regs.x.ecx = rseg3;
                                regs.x.eax = 0x25c1; /* Free DOS Memory under Phar Lap */
                                intdos( &regs, &regs );
                            }
                            regs.x.ebx = size_para;
                            /* Fix : Add the size of interface are and buffer control block by M/M 01.Jun.94 */
                            regs.x.eax = 0x25c0; /* Alloc DOS Memory under Phar Lap */
                            intdos( &regs, &regs );
                            if ( regs.x.cflag )
                                return( 0xff00 ); /* by M/M 01.Jun.94 */
                            psel3 = 0x34;
                            rseg3 = regs.w.ax;
                            dr.es = rseg3;
                            dr.r.x.edi =  0;
                        } else { /* Allocate BIOS buffer for port 2 by M/M 30.May.94 */
                            if( psel2 ) {
                                regs.x.ecx = rseg2;
                                regs.x.eax = 0x25c1; /* Free DOS Memory under Phar Lap */
                                intdos( &regs, &regs );
                            }
                            regs.x.ebx = size_para;
                            /* Fix : Add the size of interface are and buffer control block by M/M 01.Jun.94 */
                            regs.x.eax = 0x25c0; /* Alloc DOS Memory under Phar Lap */
                            intdos( &regs, &regs );
                            if ( regs.x.cflag )
                                return( 0xff00 ); /* by M/M 01.Jun.94 */
                            psel2 = 0x34;
                            rseg2 = regs.w.ax;
                            dr.es = rseg2;
                            dr.r.x.edi = 0;
                        }
                        /*** Vecttor set / segment : d000h ***/
                        regs.x.eax = 0x2505;
                        regs.h.cl = intno;
                        regs.x.ebx = 0xd0000000 + *vect_src;
                        intdos( &regs, &regs );
                    } else if( _DPMI || _IsRational() ) {
                        dpmi_dos_mem_block  dos_mem;

                        if( __port == _COM_CH3 ) {
                            if( psel3 ) { /* Allocate BIOS buffer for port 3 by M/M 30.May.94 */
                                DPMIFreeDOSMemoryBlock( psel3 );
                            }
                            /* Fix : Add the size of interface are and buffer control block */
                            dos_mem = DPMIAllocateDOSMemoryBlock( size_para );
                            if( dos_mem.pm == 0 )
                                return( 0xff00 );
                            psel3 = dos_mem.pm;
                            rseg3 = dos_mem.rm;
                            dr.es = rseg3;
                            dr.r.x.edi =  0;
                        } else { /* Allocate BIOS buffer for port 2 by M/M 30.May.94 */
                            if( psel2 ) {
                                DPMIFreeDOSMemoryBlock( psel2 );
                            }
                            /* Fix : Add the size of interface are and buffer control block */
                            dos_mem = DPMIAllocateDOSMemoryBlock( size_para );
                            if( dos_mem.pm == 0 )
                                return( 0xff00 );
                            psel2 = dos_mem.pm;
                            rseg2 = dos_mem.rm;
                            dr.es = rseg2;
                            dr.r.x.edi = 0;
                        }
                        /*** Vector set / segment : d000h ***/
                        DPMISetRealModeInterruptVector( intno, MK_FP( 0xd000, *vect_src ) );
                    }
                } else {
                    return( 0xff00 );
                }
            } else { /* Allocate BIOS buffer for port 1 by M/M 30.May.94 */
                /*** Need realloc ***/ /* Move from the below to here */
                if( _IsPharLap() ) {
                    if( psel ) {
                        regs.x.ecx = rseg;
                        regs.x.eax = 0x25c1; /* Free DOS Memory under Phar Lap */
                        intdos( &regs, &regs );
                    }

                    regs.x.ebx = size_para;
                    /* Fix : Add the size of interface are and buffer control block by M/M 01.Jun.94 */
                    regs.x.eax = 0x25c0; /* Alloc DOS Memory under Phar Lap */
                    intdos( &regs, &regs );
                    if ( regs.x.cflag )
                        return( 0xff00 ); /* by M/M 01.Jun.94 */
                    psel = 0x34;
                    rseg = regs.w.ax;
                    /* _fmemmove( MK_FP( psel, rseg << 4 ), __data->buffer, __data->size ); */
                    /* No need to trasfer data by M/M 30.May.94 */
                } else if( _DPMI || _IsRational() ) {
                    dpmi_dos_mem_block  dos_mem;

                    if ( psel ) {
                        DPMIFreeDOSMemoryBlock( psel );
                    }
                    /* Fix : Add the size of interface are and buffer control block */
                    dos_mem = DPMIAllocateDOSMemoryBlock( size_para );
                    if( dos_mem.pm == 0 )
                        return( 0xff00 );
                    psel = dos_mem.pm;
                    rseg = dos_mem.rm;
                    /* memmove( (char *)( rseg << 4 ), __data->buffer, __data->size ); */
                    /* No need to trasfer data by M/M 30.May.94 */
                }
                dr.es = rseg;
                dr.r.x.edi =  0;
            }
            dr.r.h.ah = __cmd;
            if( __data->baud == _COM_DEFAULT ) {
                dr.r.h.al = _COM_1200;
            } else {
                dr.r.h.al = __data->baud;
            }
            dr.r.h.bh = __data->tx_time;
            dr.r.h.bl = __data->rx_time;
            if( __data->mode == 0xff ) {
                dr.r.h.ch = (_COM_STOP1 | _COM_CHR7 | 0x02);
            } else {
                dr.r.h.ch = __data->mode | 0x02;
            }
            if( __data->command == 0xff ) {
                dr.r.h.cl = (_COM_ER | _COM_RXEN | _COM_TXEN);
            } else {
                dr.r.h.cl = __data->command;
            }
            dr.r.x.edx = __data->size;
            break;
        case _COM_SEND:
            dr.r.h.ah = __cmd;
            dr.r.h.al = *(unsigned char *)(__data->buffer);
            break;
        case _COM_COMMAND:
            dr.r.h.ah = __cmd;
            if( __data->command == 0xff ) {
                dr.r.h.al = (_COM_ER | _COM_RXEN | _COM_TXEN);
            } else {
                dr.r.h.al = __data->command;
            }
            break;
        default:
            dr.r.h.ah = __cmd;
            break;
        }
        switch( __port ) {
        case _COM_CH1:
            intno = 0x19;  /* interrupt no */
            break;
        case _COM_CH2:
            intno = 0xd4;  /* interrupt no */
            break;
        case _COM_CH3:
            intno = 0xd5;  /* interrupt no */
            break;
        }
        if( _IsPharLap() ) {
            dp.r.x.eax = dr.r.x.eax;
            dp.r.x.edx = dr.r.x.edx;
            dp.ds = dr.ds;
            dp.es = dr.es;
            dp.fs = dr.fs;
            dp.gs = dr.gs;
            dp.intno = intno;
            regs.x.eax = 0x2511;
            regs.x.edx = (unsigned long)&dp;
            regs.x.ebx = dr.r.x.ebx;
            regs.x.ecx = dr.r.x.ecx;
            regs.x.edi = dr.r.x.edi;
            regs.x.esi = dr.r.x.esi;
            intdos( &regs, &regs );
            dr.r.x.ecx = regs.x.ecx;
            dr.r.x.eax = regs.x.eax;
        } else if( _DPMI || _IsRational() ) {
            regs.x.ebx = intno;
            regs.x.ecx = 0;  /* no stack for now */
            regs.x.edi = (unsigned long)&dr;
            regs.x.eax = 0x300;
            int386( 0x31, &regs, &regs );
        }
        switch( __cmd ) {
        case _COM_GETDTL:
            if( __data )
                __data->size = dr.r.x.ecx;
            break;
        case _COM_RECEIVE:
        case _COM_STATUS:
            if( __data ) {
                *( (unsigned char *)__data->buffer + 0 ) = dr.r.h.ch;
                *( (unsigned char *)__data->buffer + 1 ) = dr.r.h.cl;
            }
            break;
        }
        return( dr.r.h.ah );
#endif
    }
    /* IBM PC */
    return( 0 );    // fail if not a NEC PC-98 machine
}
