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


#define BDATA_SEG               0x40
/*
 *  BIOS data area
 */
#define BDATA_EQUIP_LIST        0x10
#define BDATA_CURR_VIDEO_MODE   0x49    /* byte */
#define BDATA_REGEN_LEN         0x4c
#define BDATA_SCREEN_OFFSET     0x4e    /* word */
#define BDATA_CURPOS            0x50
#define BDATA_ACT_VPAGE         0x62
#define BDATA_MODE_CTRL         0x65
#define BDATA_SYSTEM_CLOCK      0x6c    /* dword */
#define BDATA_POINT_HEIGHT      0x85    /* byte */
#define BDATA_VID_CTRL1         0x87

#define EXTENDER_RM2PM( s, o )  _MK_FP( _ExtenderRealModeSelector, (((unsigned)(s)) << 4) + (o) )

#ifdef _M_I86
#define RealModeDataPtr( segm, off )    _MK_FP( segm, off )
#define RealModeSegmPtr( segm )         _MK_FP( segm, 0 )
#else
#define RealModeDataPtr( segm, off )    EXTENDER_RM2PM( segm, off )
#define RealModeSegmPtr( segm )         EXTENDER_RM2PM( segm, 0 )
#endif
#define RealModeData( segm, off, type ) *(type __far *)RealModeDataPtr( segm, off )

#define BIOSData( off, type )           RealModeData( BDATA_SEG, off, type )
#define VIDEOData( segm, off )          RealModeData( segm, off, unsigned char )

extern  unsigned short  _ExtenderRealModeSelector;
