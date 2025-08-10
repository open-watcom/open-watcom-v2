/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2024 The Open Watcom Contributors. All Rights Reserved.
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


#define BDATA_SEG               0x40
/*
 *  BIOS data area
 */
#define BDATA_PRINTER_BASE      0x08
#define BDATA_EQUIP_LIST        0x10
#define BDATA_KEYB_FLAG_0       0x17
#define BDATA_KEYB_FLAG_1       0x18
#define BDATA_CURR_VIDEO_MODE   0x49    /* current video mode (byte) */
#define BDATA_VIDEO_COLUMNS     0x4a    /* number of columns on screen (word) */
#define BDATA_REGEN_LEN         0x4c    /* size of current video buffer in bytes (word) */
#define BDATA_SCREEN_OFFSET     0x4e    /* offset of current video page (word) */
#define BDATA_CURSOR_POS        0x50    /* cursor position for each video page (word) */
#define BDATA_CURSOR_MODE       0x60    /* cursor shape (word) */
#define BDATA_ACTIVE_VIDEO_PAGE 0x62    /* active video page number (byte) */
#define BDATA_MODE_CTRL         0x65
#define BDATA_SYSTEM_CLOCK      0x6c    /* daily timer ticks (dword) */
#define BDATA_VIDEO_ROWS        0x84    /* video number of text rows - 1 (byte) */
#define BDATA_POINT_HEIGHT      0x85    /* video point height of character matrix (word) */
#define BDATA_VIDEO_INFO_0      0x87    /* video miscellaneous info 0 (byte) */
#define BDATA_VIDEO_INFO_1      0x88    /* video miscellaneous info 1 (byte) */

#define EXTENDER_RM2PM( s, o )  _MK_FP( _ExtenderRealModeSelector, (((unsigned)(s)) << 4) + (o) )

#ifdef _M_I86
#define RealModeDataPtr( segm, off )    _MK_FP( segm, off )
#define RealModeSegmPtr( segm )         _MK_FP( segm, 0 )
#else
#define RealModeDataPtr( segm, off )    _MK_FP( _ExtenderRealModeSelector, (((unsigned)(segm)) << 4) + (off) )
#define RealModeSegmPtr( segm )         _MK_FP( _ExtenderRealModeSelector, (((unsigned)(segm)) << 4) )
#endif
#define RealModeData( type, segm, off ) *(type __far *)RealModeDataPtr( segm, off )

#define BIOSDataPtr( off )              RealModeDataPtr( BDATA_SEG, off )
#define BIOSData( type, off )           RealModeData( type, BDATA_SEG, off )
#define VIDEODataPtr( segm, off )       RealModeDataPtr( segm, off )
#define VIDEOData( segm, off )          RealModeData( unsigned char, segm, off )

extern  unsigned short  _ExtenderRealModeSelector;
