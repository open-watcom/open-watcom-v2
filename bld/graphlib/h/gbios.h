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
* Description:  BIOS related constants and function prototypes.
*
****************************************************************************/


/* BIOS Variable Locations */
#pragma pack(push, 1);

#define _BIOS_data( p ) MK_FP( _BiosSeg, _BiosOff + p )

#define EQUIP_FLAGS     0x0010      /* equipment flags */
#define CRT_MODE        0x0049      /* current CRT mode */
#define CRT_COLS        0x004a      /* number of columns on screen  */
#define CRT_LEN         0x004c      /* size of video buffer */
#define CURSOR_POSN     0x0050      /* cursor for each page */
#define CURSOR_MODE     0x0060      /* cursor shape */
#define ROWS            0x0084      /* number of text rows - 1 */
#define POINTS          0x0085      /* height of character */
#define INFO            0x0087      /* miscellaneous info */
#define INFO_3          0x0088      /* more miscellaneous info */


/* BIOS Functions */

#define _BIOS_SET_MODE      0x0000
#define _BIOS_CURSOR_SIZE   0x0100
#define _BIOS_CURSOR_POSN   0x0200
#define _BIOS_CURSOR_STATUS 0x0300
#define _BIOS_VIDEO_PAGE    0x0500
#define _BIOS_GET_CHAR      0x0800
#define _BIOS_PUT_CHAR      0x0900
#define _BIOS_SET_OVERSCAN  0x0b00
#define _BIOS_GET_MODE      0x0f00
#define _BIOS_SET_PALETTE   0x1000
#define _BIOS_CHAR_GEN      0x1100
#define _BIOS_ALT_SELECT    0x1200
#define _BIOS_VIDEO_DCC     0x1a00


/* Video Interrupt Routines */

extern short            VideoInt( short, short, short, short );
extern short            VideoInt_bx( short, short, short, short );
extern short            VideoInt_cx( short, short, short, short );

#pragma aux             VideoInt = \
                        "push    bp   ", \
                        "int     10h  ", \
                        "pop     bp   ", \
                        parm caller [ax] [bx] [cx] [dx] value [ax];
#pragma aux             VideoInt_bx = \
                        "push    bp   ", \
                        "int     10h  ", \
                        "pop     bp   ", \
                        parm caller [ax] [bx] [cx] [dx] value [bx];
#pragma aux             VideoInt_cx = \
                        "push    bp   ", \
                        "int     10h  ", \
                        "pop     bp   ", \
                        parm caller [ax] [bx] [cx] [dx] value [cx];

#define GetVideoMode()  ( VideoInt( _BIOS_GET_MODE, 0, 0, 0 ) & 0x7f )
#define EGA_Memory()    ( VideoInt_bx( 0x1200, 0x0010, 0, 0 ) )

#pragma pack (pop);
