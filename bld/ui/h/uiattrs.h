/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2020 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Text attribute definitions for ui library.
*
****************************************************************************/


#ifndef _UIATTRS_H_
#define _UIATTRS_H_

#define BRIGHT              0x08
#define UNDERLINE           0x01
#define BLINK               0x80
#define BLACK               0x00
#define WHITE               0x07

enum {
    FG_BLACK,
    FG_GREY1,
    FG_GREY5,
    FG_GREY8,
    FG_GREY2,
    FG_GREY3,
    FG_GREY6,
    FG_GREY11,
    FG_GREY4,
    FG_GREY7,
    FG_GREY12,
    FG_GREY13,
    FG_GREY9,
    FG_GREY10,
    FG_GREY14,
    FG_WHITE
};

enum {
    BG_BLACK,
    BG_GREY1,
    BG_GREY4,
    BG_GREY6,
    BG_GREY2,
    BG_GREY3,
    BG_GREY5,
    BG_WHITE
};

enum {
    C_BLACK,
    C_BLUE,
    C_GREEN,
    C_CYAN,
    C_RED,
    C_MAGENTA,
    C_BROWN,
    C_WHITE
};
#define C_BR_BLACK          (BRIGHT | C_BLACK)
#define C_BR_BLUE           (BRIGHT | C_BLUE)
#define C_BR_RED            (BRIGHT | C_RED)
#define C_BR_WHITE          (BRIGHT | C_WHITE)
#ifdef __UNIX__
  #define C_BR_WHITE_ON_WH  C_BROWN
#else
  #define C_BR_WHITE_ON_WH  C_BR_WHITE
#endif

enum {
    VGA_BLACK,
    VGA_GRAY,
    VGA_SILVER,
    VGA_NAVY,
    VGA_BLUE,
    VGA_AQUA,
    VGA_MAROON,
    VGA_RED,
    VGA_PURPLE,
    VGA_FUCHSIA,
    VGA_TEAL,
    VGA_GREEN,
    VGA_LIME,
    VGA_OLIVE,
    VGA_YELLOW,
    VGA_WHITE
};

#define BR_WHITE            (BRIGHT | WHITE)
#define BR_UNDERLINE        (BRIGHT | UNDERLINE)

#define _bg( a )            ((a) << 4)
#define _fg( a )            (a)
#define _attr( b, f )       (((b) << 4) | (f))

typedef struct a_colour {
    unsigned    red   : 6;
    unsigned    green : 6;
    unsigned    blue  : 6;
} a_colour;

#endif
