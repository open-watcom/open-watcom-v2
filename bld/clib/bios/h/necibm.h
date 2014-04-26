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
* Description:  BIOS access routines and constants.
*
****************************************************************************/


#ifndef _BIOSIBM_H_INCLUDED
#define _BIOSIBM_H_INCLUDED

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


/*** Handy macros ***/
#define _IBM_DISK_READ          2
#define _IBM_DISK_WRITE         3
#define _IBM_DISK_VERIFY        4

#define _KEYBRD_READ            0
#define _KEYBRD_READY           1
#define _KEYBRD_SHIFTSTATUS     2

#define _IBM_PRINTER_WRITE      0
#define _IBM_PRINTER_INIT       1
#define _IBM_PRINTER_STATUS     2

#define _IBM_COM_INIT           0
#define _IBM_COM_SEND           1
#define _IBM_COM_RECEIVE        2
#define _IBM_COM_STATUS         3

#define _IBM_COM_CH1            0
#define _IBM_COM_CH2            1
#define _IBM_COM_CH3            2

#define _IBM_COM_CHR7           2
#define _IBM_COM_CHR8           3

#define _IBM_COM_STOP1          0
#define _IBM_COM_STOP2          4

#define _IBM_COM_NOPARITY       0
#define _IBM_COM_ODDPARITY      8
#define _IBM_COM_SPACEPARITY    16
#define _IBM_COM_EVENPARITY     24

#define _IBM_COM_110            0
#define _IBM_COM_150            32
#define _IBM_COM_300            64
#define _IBM_COM_600            96
#define _IBM_COM_1200           128
#define _IBM_COM_2400           160
#define _IBM_COM_4800           192
#define _IBM_COM_9600           224

#define _IBM_TIME_GETCLOCK      0
#define _IBM_TIME_SETCLOCK      1


/*** Function prototypes ***/
extern _WCRTLINK unsigned short __ibm_bios_disk( unsigned cmd, struct _ibm_diskinfo_t *diskinfo );
extern _WCRTLINK unsigned short __ibm_bios_equiplist( void );
extern _WCRTLINK unsigned short __ibm_bios_keybrd( unsigned cmd );
extern _WCRTLINK unsigned short __ibm_bios_memsize( void );
extern _WCRTLINK unsigned short __ibm_bios_printer( unsigned cmd, unsigned port, unsigned data );
extern _WCRTLINK unsigned short __ibm_bios_serialcom( unsigned cmd, unsigned port, unsigned data );
extern _WCRTLINK unsigned short __ibm_bios_timeofday( unsigned cmd, long *timeval );

#endif
