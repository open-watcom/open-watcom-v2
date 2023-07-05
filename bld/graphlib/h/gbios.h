/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2023 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Video BIOS related constants and function prototypes.
*
****************************************************************************/


/* BIOS Functions */

#define VIDEOINT_SET_MODE       0x0000
#define VIDEOINT_CURSOR_SIZE    0x0100
#define VIDEOINT_CURSOR_POSN    0x0200
#define VIDEOINT_CURSOR_STATUS  0x0300
#define VIDEOINT_VIDEO_PAGE     0x0500
#define VIDEOINT_GET_CHAR       0x0800
#define VIDEOINT_PUT_CHAR       0x0900
#define VIDEOINT_SET_OVERSCAN   0x0b00
#define VIDEOINT_GET_MODE       0x0f00
#define VIDEOINT_SET_PALETTE    0x1000
#define VIDEOINT_CHARGEN        0x1100
#define VIDEOINT_ALT_SELECT     0x1200
#define VIDEOINT_VIDEO_DCC      0x1a00


/* Video Interrupt Routines */

extern unsigned short VideoInt( short, short, short, short );
#pragma aux VideoInt = \
        "push bp"   \
        "int 10h"   \
        "pop  bp"   \
    __parm __caller [__ax] [__bx] [__cx] [__dx] \
    __value         [__ax]

extern unsigned short VideoInt_bx( short, short, short, short );
#pragma aux VideoInt_bx = \
        "push bp"   \
        "int 10h"   \
        "pop  bp"   \
    __parm __caller [__ax] [__bx] [__cx] [__dx] \
    __value         [__bx]

extern unsigned short VideoInt_cx( short, short, short, short );
#pragma aux VideoInt_cx = \
        "push bp"   \
        "int 10h"   \
        "pop  bp"   \
    __parm __caller [__ax] [__bx] [__cx] [__dx] \
    __value         [__cx]

#define GetVideoMode()  ( VideoInt( VIDEOINT_GET_MODE, 0, 0, 0 ) & 0x7f )
#define EGA_Info()      ( VideoInt_bx( VIDEOINT_ALT_SELECT, 0x0010, 0, 0 ) )
#define EGA_Memory()    ( VideoInt_bx( VIDEOINT_ALT_SELECT, 0x0010, 0, 0 ) & 0xff )
