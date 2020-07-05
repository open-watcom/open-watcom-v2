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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#ifndef _BIOSUI_H_
#define _BIOSUI_H_

#include "extender.h"

#define _INT_10        "int 10h"
#define _INT_21        "int 21h"
#define _INT_33        "int 33h"

#define VECTOR_VIDEO    0x10
#define VECTOR_MOUSE    0x33

#define VIDMONOINDXREG      0x03B4
#define VIDCOLORINDXREG     0x03D4

#define MOUSE_DRIVER_OK     ((unsigned short)-1)

struct cursor_pos {
    unsigned char   col;
    unsigned char   row;
};

struct ega_info {
    unsigned char   mem;
    unsigned char   mono;
    unsigned char   switches;
    unsigned char   adapter_bits;
};

struct mouse_data {
    unsigned    bx;
    unsigned    cx;
    unsigned    dx;
};
typedef struct mouse_data __based( __segname( "_STACK" ) ) *md_stk_ptr;


extern unsigned short MouseDrvReset( void );
#pragma aux MouseDrvReset = \
        "xor  ax,ax"        \
        _INT_33             \
    __parm      [] \
    __value     [__ax] \
    __modify    [__ax __bx]

extern void MouseDrvCallRetState( unsigned short, md_stk_ptr );
#ifdef _M_I86
#pragma aux MouseDrvCallRetState = \
        _INT_33                 \
        "mov ss:[si+0],bx"      \
        "mov ss:[si+2],cx"      \
        "mov ss:[si+4],dx"      \
    __parm      [__ax] [__si] \
    __value     \
    __modify    [__bx __cx __dx]
#else
#pragma aux MouseDrvCallRetState = \
        _INT_33                 \
        "mov ss:[esi+0],bx"     \
        "mov ss:[esi+2],cx"     \
        "mov ss:[esi+4],dx"     \
    __parm      [__ax] [__esi] \
    __value     \
    __modify    [__bx __cx __dx]
#endif
extern unsigned short MouseDrvCall1( unsigned short );
#pragma aux MouseDrvCall1 = \
        _INT_33             \
    __parm      [__ax] \
    __value     [__ax] \
    __modify    [__cx __dx]

extern unsigned short MouseDrvCall2( unsigned short, unsigned short, unsigned short, unsigned short );
#pragma aux MouseDrvCall2 = \
        _INT_33             \
    __parm      [__ax] [__bx] [__cx] [__dx] \
    __value     [__ax] \
    __modify    []

extern void MouseDrvCall3( unsigned short, unsigned short, unsigned short, unsigned short, unsigned short );
#pragma aux MouseDrvCall3 = \
        _INT_33             \
    __parm      [__ax] [__cx] [__dx] [__si] [__di] \
    __value     \
    __modify    []

extern unsigned short MouseDrvCall4( unsigned short );
#pragma aux MouseDrvCall4 = \
        _INT_33             \
    __parm      [__ax] \
    __value     [__ax] \
    __modify    [__bx __cx __dx]

extern void BIOSSetMode( unsigned char );
#pragma aux BIOSSetMode = \
        "xor    ah,ah"      \
        _INT_10             \
    __parm      [__al] \
    __value     \
    __modify    [__ah]

extern void BIOSSetCursorTyp( unsigned char top_line, unsigned char bot_line );
#pragma aux BIOSSetCursorTyp = \
        "mov    ah,1"           \
        _INT_10                 \
    __parm      [__ch] [__cl] \
    __value     \
    __modify    [__ah]

extern void BIOSSetCursorPos( unsigned char row, unsigned char col, unsigned char page );
#pragma aux BIOSSetCursorPos = \
        "mov    ah,2"       \
        _INT_10             \
    __parm      [__dh] [__dl] [__bh] \
    __value     \
    __modify    [__ah]

extern struct cursor_pos  BIOSGetCursorPos( unsigned char page );
#pragma aux BIOSGetCursorPos = \
        "mov    ah,3"       \
        _INT_10             \
    __parm      [__bh] \
    __value     [__dx] \
    __modify    [__ax __cx]

extern unsigned short BIOSGetCursorTyp( unsigned char page );
#pragma aux BIOSGetCursorTyp = \
        "mov    ah,3"       \
        _INT_10             \
    __parm      [__bh] \
    __value     [__cx] \
    __modify    [__ax __dx]

extern PIXEL BIOSGetCharPixel( unsigned char page );
#pragma aux  BIOSGetCharPixel = \
        "mov    ah,8"           \
        _INT_10                 \
    __parm      [__bh] \
    __value     [__ax] \
    __modify    []

extern void BIOSSetCharPixel( PIXEL char_attr, unsigned char page );
#pragma aux BIOSSetCharPixel = \
        "mov    bl,ah"          \
        "mov    cx,1"           \
        "mov    ah,9"           \
        _INT_10                 \
    __parm      [__ax] [__bh] \
    __value     \
    __modify    [__ah __bl __cx]

extern unsigned char BIOSGetPage( void );
#pragma aux BIOSGetPage = \
        "mov    ah,0fh"     \
        _INT_10             \
    __parm      [] \
    __value     [__bh] \
    __modify    [__ax]

extern unsigned char BIOSGetMode( void );
#pragma aux BIOSGetMode = \
        "mov    ah,0fh"     \
        _INT_10             \
    __parm      [] \
    __value     [__al] \
    __modify    [__ah __bh]

extern unsigned char BIOSGetColumns( void );
#pragma aux BIOSGetColumns = \
        "mov    ah,0fh"     \
        _INT_10             \
    __parm      [] \
    __value     [__ah] \
    __modify    [__al __bh]

extern unsigned char BIOSGetRowCount( void );
#ifdef _M_I86
#pragma aux BIOSGetRowCount = \
        "mov    ax,1130h"   \
        "xor    bh,bh"      \
        _INT_10             \
        "inc    dl"         \
    __parm      [] \
    __value     [__dl] \
    __modify __exact    [__ax __bh __cx __es __bp]
#else
#pragma aux BIOSGetRowCount = \
        "mov    ax,1130h"   \
        "xor    bh,bh"      \
        _INT_10             \
        "inc    dl"         \
    __parm      [] \
    __value     [__dl] \
    __modify __exact    [__ax __bh __cx __es __ebp]
#endif

extern struct ega_info BIOSEGAInfo( void );
#ifdef _M_I86
#pragma aux BIOSEGAInfo = \
        "mov    ah,12h"     \
        "mov    bx,0ff10h"  \
        _INT_10             \
        "mov    ax,bx"      \
        "mov    dx,cx"      \
    __parm      [] \
    __value     [__dx __ax] \
    __modify    [__bx __cx]
#else
#pragma aux BIOSEGAInfo = \
        "mov    ah,12h"     \
        "mov    bx,0ff10h"  \
        _INT_10             \
        "mov    eax,ecx"    \
        "shl    eax,10h"    \
        "or     ax,bx"      \
    __parm      [] \
    __value     [__eax] \
    __modify    [__bx __cx]
#endif

#ifdef _M_I86
extern LP_PIXEL desqview_shadow_buffer( LP_PIXEL );
#pragma aux desqview_shadow_buffer = \
        "mov ah,0feh"   \
        _INT_10         \
    __parm      [__es __di] \
    __value     [__es __di] \
    __modify    [__ah]
#endif


#endif // _BIOSUI_H_
