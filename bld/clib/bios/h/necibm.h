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


#ifndef _NECIBM_H_INCLUDED
#define _NECIBM_H_INCLUDED

#include "variety.h"


/*** Data structures ***/
#ifndef _BIOS_H_INCLUDED
#if defined(_M_IX86)
  #pragma pack(__push,1);
#else
  #pragma pack(__push,8);
#endif
struct _ibm_diskinfo_t {
    unsigned        drive;
    unsigned        head;
    unsigned        track;
    unsigned        sector;
    unsigned        nsectors;
    void _WCFAR *   buffer;
};
#pragma pack(__pop);
#endif

#ifndef _BIOS98_H_INCLUDED
struct _nec98_diskinfo_t {
    unsigned        command;
    unsigned        drive;
    unsigned        head;
    unsigned        cylinder;
    unsigned        data_len;
    unsigned        sector_len;
    unsigned        sector;
    unsigned        nsectors;
    void _WCI86FAR *buffer;
    void _WCI86FAR *result;
};

struct com_t {
    unsigned short  baud;
    unsigned short  mode;
    unsigned short  command;
    unsigned short  tx_time;
    unsigned short  rx_time;
    unsigned short  size;
    void _WCI86FAR *buffer;
};
#endif


/*** Handy macros ***/
#define _IBM_DISK_READ          2
#define _IBM_DISK_WRITE         3
#define _IBM_DISK_VERIFY        4
#define _NEC98_DISK_READ        0x06
#define _NEC98_DISK_WRITE       0x05
#define _NEC98_DISK_VERIFY      0x01

#define _KEYBRD_READ            0
#define _KEYBRD_READY           1
#define _KEYBRD_SHIFTSTATUS     2

#define _IBM_PRINTER_WRITE      0
#define _IBM_PRINTER_INIT       1
#define _IBM_PRINTER_STATUS     2
#define _NEC98_PRINTER_WRITE    0x11
#define _NEC98_PRINTER_INIT     0x10
#define _NEC98_PRINTER_STATUS   0x12

#define _IBM_COM_INIT           0
#define _IBM_COM_SEND           1
#define _IBM_COM_RECEIVE        2
#define _IBM_COM_STATUS         3
#define _NEC98_COM_INIT         0x00
#define _NEC98_COM_SEND         0x03
#define _NEC98_COM_RECEIVE      0x04
#define _NEC98_COM_STATUS       0x06

#define _NEC98_COM_DEFAULT_BAUD 0xFF
#define _NEC98_COM_DEFAULT_MODE 0xFF
#define _NEC98_COM_DEFAULT_CMD  0
#define _NEC98_COM_DEFAULT_TIME 0

#define _IBM_COM_CH1            0
#define _IBM_COM_CH2            1
#define _IBM_COM_CH3            2
#define _NEC98_COM_CH1          1
#define _NEC98_COM_CH2          2
#define _NEC98_COM_CH3          3

#define _IBM_COM_CHR7           2
#define _IBM_COM_CHR8           3
#define _NEC98_COM_CHR7         0x08
#define _NEC98_COM_CHR8         0x0C

#define _IBM_COM_STOP1          0
#define _IBM_COM_STOP2          4
#define _NEC98_COM_STOP1        0x40
#define _NEC98_COM_STOP2        0xC0

#define _IBM_COM_NOPARITY       0
#define _IBM_COM_ODDPARITY      8
#define _IBM_COM_SPACEPARITY    16
#define _IBM_COM_EVENPARITY     24
#define _NEC98_COM_NOPARITY     0
#define _NEC98_COM_ODDPARITY    0x10
#define _NEC98_COM_EVENPARITY   0x30

#define _IBM_COM_110            0
#define _IBM_COM_150            32
#define _IBM_COM_300            64
#define _IBM_COM_600            96
#define _IBM_COM_1200           128
#define _IBM_COM_2400           160
#define _IBM_COM_4800           192
#define _IBM_COM_9600           224
#define _NEC98_COM_DEFAULT      0xFF
#define _NEC98_COM_75           0x00
#define _NEC98_COM_150          0x01
#define _NEC98_COM_300          0x02
#define _NEC98_COM_600          0x03
#define _NEC98_COM_1200         0x04
#define _NEC98_COM_2400         0x05
#define _NEC98_COM_4800         0x06
#define _NEC98_COM_9600         0x07

#define _IBM_TIME_GETCLOCK      0
#define _IBM_TIME_SETCLOCK      1
#define _NEC98_TIME_GETCLOCK    0
#define _NEC98_TIME_SETCLOCK    1



/*** Function prototypes ***/
extern _WCRTLINK unsigned short __ibm_bios_disk( unsigned cmd, struct _ibm_diskinfo_t *diskinfo );
extern _WCRTLINK unsigned short __ibm_bios_equiplist( void );
extern _WCRTLINK unsigned short __ibm_bios_keybrd( unsigned cmd );
extern _WCRTLINK unsigned short __ibm_bios_memsize( void );
extern _WCRTLINK unsigned short __ibm_bios_printer( unsigned cmd, unsigned port, unsigned data );
extern _WCRTLINK unsigned short __ibm_bios_serialcom( unsigned cmd, unsigned port, unsigned data );
extern _WCRTLINK unsigned short __ibm_bios_timeofday( unsigned cmd, long *timeval );

extern _WCRTLINK unsigned short __nec98_bios_disk( unsigned cmd, struct _nec98_diskinfo_t *diskinfo );
extern _WCRTLINK unsigned short __nec98_bios_equiplist( void );
extern _WCRTLINK unsigned short __nec98_bios_keybrd( unsigned cmd, unsigned char *buf );
extern _WCRTLINK unsigned short __nec98_bios_memsize( void );
extern _WCRTLINK unsigned short __nec98_bios_printer( unsigned cmd, unsigned char *data );
extern _WCRTLINK unsigned short __nec98_bios_serialcom( unsigned cmd, unsigned port, struct com_t *data );
extern _WCRTLINK unsigned short __nec98_bios_timeofday( unsigned cmd, char *timeval );

#endif
