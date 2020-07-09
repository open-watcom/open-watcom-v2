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


#ifndef _INT10_H_INCLUDED
#define _INT10_H_INCLUDED

#if defined( __DOS__ ) || defined( __WINDOWS__ )

#define STR(...)            #__VA_ARGS__
#define INSTR(...)          STR(__VA_ARGS__)

#define VECTOR_VIDEO        0x10

#define _INT_10             0xcd VECTOR_VIDEO

/*
 * NOTE:
 *  some video BIOS implementations destroy BP/EBP registers on some BIOS functions
 *  to work properly with any video BIOS we preserve BP/EBP register for each INT 10h call
 *  by macro _INT_10_FN_xBP
 *  only function returning BP/EBP register must use _INT_10_FN macro
 */
#ifdef _M_I86
#define _INT_10_FN(n)       INSTR( mov ah, n ) _INT_10
#define _INT_10_FN_xBP(n)   "push bp" INSTR( mov ah, n ) _INT_10 "pop bp"
#else
#define _INT_10_FN(n)       INSTR( mov ah, n ) _INT_10
#define _INT_10_FN_xBP(n)   "push ebp" INSTR( mov ah, n ) _INT_10 "pop ebp"
#endif

#define DOUBLE_DOT_CHAR_PATTSET 0x12
#define COMPRESSED_CHAR_PATTSET 0x11
#define USER_CHAR_PATTSET       0

#define VIDMONOINDXREG      0x03B4
#define VIDCOLORINDXREG     0x03D4

#define EGA_CURSOR_OFF      0x1e00
#define NORM_CURSOR_OFF     0x2000
#define CGA_CURSOR_ON       0x0607
#define MONO_CURSOR_ON      0x0b0c

#define CURSOR_INVISIBLE    0x20

typedef union int10_cursor_pos {
    struct {
        unsigned char   col;
        unsigned char   row;
    } s;
    unsigned short      value;
} int10_cursor_pos;

typedef union int10_cursor_typ {
    struct {
        unsigned char   bot_line;
        unsigned char   top_line;
    } s;
    unsigned short      value;
} int10_cursor_typ;

typedef struct int10_cursor {
    int10_cursor_pos    pos;
    int10_cursor_typ    typ;
} int10_cursor;

typedef union int10_pixel_data {
    struct {
        unsigned char   ch;
        unsigned char   attr;
    } s;
    unsigned short      value;
} int10_pixel_data;

typedef struct int10_ega_info {
    unsigned char   mem;
    unsigned char   mono;
    unsigned char   switches;
    unsigned char   adapter_bits;
} int10_ega_info;

typedef struct int10_mode_info {
    unsigned char   mode;
    unsigned char   columns;
    unsigned char   dummy;
    unsigned char   page;
} int10_mode_info;

extern void _BIOSVideoSetMode( unsigned char mode );
#pragma aux _BIOSVideoSetMode = \
        _INT_10_FN_xBP( 0 )     \
    __parm              [__al] \
    __value             \
    __modify __exact    [__ax]

extern void _BIOSVideoSetCursorTyp( int10_cursor_typ );
#pragma aux _BIOSVideoSetCursorTyp = \
        _INT_10_FN_xBP( 1 )     \
    __parm              [__cx] \
    __value             \
    __modify __exact    [__ah]

extern void _BIOSVideoSetCursorPos( unsigned char page, int10_cursor_pos );
#pragma aux _BIOSVideoSetCursorPos = \
        _INT_10_FN_xBP( 2 )     \
    __parm              [__bh] [__dx] \
    __value             \
    __modify __exact    [__ah]

extern void _BIOSVideoSetCursorPosZero( unsigned char page );
#pragma aux _BIOSVideoSetCursorPosZero = \
        "xor    dx,dx"          \
        _INT_10_FN_xBP( 2 )     \
    __parm              [__bh] \
    __value             \
    __modify __exact    [__ah __dx]

extern int10_cursor _BIOSVideoGetCursor( unsigned char page );
#ifdef _M_I86
#pragma aux _BIOSVideoGetCursor = \
        _INT_10_FN_xBP( 3 )     \
    __parm              [__bh] \
    __value             [__cx __dx] \
    __modify __exact    [__ax __cx __dx]
#else
#pragma aux _BIOSVideoGetCursor = \
        _INT_10_FN_xBP( 3 )     \
        "shl    ecx,16"         \
        "mov    cx,dx"          \
    __parm              [__bh] \
    __value             [__ecx] \
    __modify __exact    [__ax __ecx __dx]
#endif

extern int10_cursor_pos _BIOSVideoGetCursorPos( unsigned char page );
#pragma aux _BIOSVideoGetCursorPos = \
        _INT_10_FN_xBP( 3 )     \
    __parm              [__bh] \
    __value             [__dx] \
    __modify __exact    [__ax __cx __dx]

extern int10_cursor_typ _BIOSVideoGetCursorTyp( unsigned char page );
#pragma aux _BIOSVideoGetCursorTyp = \
        _INT_10_FN_xBP( 3 )     \
    __parm              [__bh] \
    __value             [__cx] \
    __modify __exact    [__ax __cx __dx]

extern void _BIOSVideoSetPage( unsigned char page );
#pragma aux _BIOSVideoSetPage = \
        _INT_10_FN_xBP( 5 )     \
    __parm              [__al] \
    __value             \
    __modify __exact    [__ah]

extern void _BIOSVideoSetAttr( unsigned char attr );
#pragma aux _BIOSVideoSetAttr = \
        "xor  cx,cx"        \
        "mov  dx,3250h"     \
        "xor  al,al"        \
        _INT_10_FN_xBP( 6 )     \
    __parm              [__bh] \
    __value             \
    __modify __exact    [__ax __cx __dx]

extern unsigned char _BIOSVideoGetAttr( unsigned char page );
#pragma aux _BIOSVideoGetAttr = \
        _INT_10_FN_xBP( 8 )     \
    __parm              [__bh] \
    __value             [__ah] \
    __modify __exact    [__ax]

extern int10_pixel_data _BIOSVideoGetCharPixel( unsigned char page );
#pragma aux  _BIOSVideoGetCharPixel = \
        _INT_10_FN_xBP( 8 )     \
    __parm              [__bh] \
    __value             [__ax] \
    __modify __exact    [__ax]

extern void _BIOSVideoSetCharPixel( unsigned char page, int10_pixel_data );
#pragma aux _BIOSVideoSetCharPixel = \
        "mov    bl,ah"          \
        "mov    cx,1"           \
        _INT_10_FN_xBP( 9 )     \
    __parm              [__bh] [__ax] \
    __value             \
    __modify __exact    [__ah __bl __cx]

extern void _BIOSVideoRingBell( unsigned char page );
#ifdef _M_I86
#pragma aux _BIOSVideoRingBell = \
        "mov    al,7"           \
        _INT_10_FN_xBP( 0x0e )  \
    __parm              [__bh] \
    __value             \
    __modify __exact    [__ax]
#else
#pragma aux _BIOSVideoRingBell = \
        "mov    al,7"           \
        _INT_10_FN_xBP( 0x0e )  \
    __parm              [__bh] \
    __value             \
    __modify __exact    [__ax]
#endif

extern int10_mode_info _BIOSVideoGetModeInfo( void );
#ifdef _M_I86
#pragma aux _BIOSVideoGetModeInfo = \
        _INT_10_FN_xBP( 0x0f )  \
    __parm              [] \
    __value             [__bx __ax] \
    __modify __exact    [__ax __bx]
#else
#pragma aux _BIOSVideoGetModeInfo = \
        _INT_10_FN_xBP( 0x0f )  \
        "shl    ebx,16"         \
        "mov    bx,ax"          \
    __parm              [] \
    __value             [__ebx] \
    __modify __exact    [__ax __ebx]
#endif

extern unsigned char _BIOSVideoGetMode( void );
#pragma aux _BIOSVideoGetMode = \
        _INT_10_FN_xBP( 0x0f )  \
    __parm              [] \
    __value             [__al] \
    __modify __exact    [__ax __bh]

extern unsigned char _BIOSVideoGetPage( void );
#pragma aux _BIOSVideoGetPage = \
        _INT_10_FN_xBP( 0x0f )  \
    __parm              [] \
    __value             [__bh] \
    __modify __exact    [__ax __bh]

extern unsigned char _BIOSVideoGetColumnCount( void );
#pragma aux _BIOSVideoGetColumnCount = \
        _INT_10_FN_xBP( 0x0f )  \
    __parm              [] \
    __value             [__ah] \
    __modify __exact    [__ax __bh]

extern void _BIOSVideoSetBlinkAttr( unsigned char on );
#pragma aux _BIOSVideoSetBlinkAttr = \
        "mov    al,3"           \
        "xor    bh,bh"          \
        _INT_10_FN_xBP( 0x10 )  \
    __parm              [__bl] \
    __value             \
    __modify __exact    [__ax __bh]

extern void _BIOSVideoGetColorPalette( void __far * );
#ifdef _M_I86
#pragma aux _BIOSVideoGetColorPalette = \
        "mov    al,9"           \
        _INT_10_FN_xBP( 0x10 )  \
    __parm              [__es __dx] \
    __value             \
    __modify __exact    [__ax]
#else
#pragma aux _BIOSVideoGetColorPalette = \
        "mov    al,9"           \
        _INT_10_FN_xBP( 0x10 )  \
    __parm              [__es __edx] \
    __value             \
    __modify __exact    [__ax]
#endif

extern void _BIOSVideoSetColorRegister( unsigned short reg, unsigned char r, unsigned char g, unsigned char b );
#pragma aux _BIOSVideoSetColorRegister = \
        "mov    al,10h"         \
        _INT_10_FN_xBP( 0x10 )  \
    __parm              [__bx] [__dh] [__ch] [__cl] \
    __value             \
    __modify __exact    [__ax]

extern unsigned long _BIOSVideoGetColorRegister( unsigned short reg );
#ifdef _M_I86
#pragma aux _BIOSVideoGetColorRegister = \
        "mov    al,15h"         \
        _INT_10_FN_xBP( 0x10 )  \
    __parm              [__bx] \
    __value             [__cx __dx] \
    __modify __exact    [__ax __cx __dx]
#else
#pragma aux _BIOSVideoGetColorRegister = \
        "mov    al,15h"         \
        _INT_10_FN_xBP( 0x10 )  \
        "shr    ecx,16"     \
        "mov    cx,dx"      \
    __parm              [__bx] \
    __value             [__ecx] \
    __modify __exact    [__ax __ecx __dx]
#endif

extern unsigned char _BIOSVideoGetRowCount( void );
#ifdef _M_I86
#pragma aux _BIOSVideoGetRowCount = \
        "xor    bh,bh"          \
        "mov    dl,24"          \
        "mov    al,30h"         \
        _INT_10_FN_xBP( 0x11 )  \
        "inc    dl"             \
    __parm              [] \
    __value             [__dl] \
    __modify __exact    [__ax __bh __cx __dl __es]
#else
#pragma aux _BIOSVideoGetRowCount = \
        "xor    bh,bh"          \
        "mov    dl,24"          \
        "mov    al,30h"         \
        _INT_10_FN_xBP( 0x11 )  \
        "inc    dl"             \
    __parm              [] \
    __value             [__dl] \
    __modify __exact    [__ax __bh __cx __dl __edi __es]    /* EDI workaround bug in DOS4G */
#endif

extern unsigned short _BIOSVideoGetPoints( void );
#ifdef _M_I86
#pragma aux _BIOSVideoGetPoints = \
        "xor    bh,bh"          \
        "mov    al,30h"         \
        _INT_10_FN_xBP( 0x11 )  \
    __parm              [] \
    __value             [__cx] \
    __modify __exact    [__ax __bh __cx __dl __es]
#else
#pragma aux _BIOSVideoGetPoints = \
        "xor  bh,bh"            \
        "mov  al,30h"           \
        _INT_10_FN_xBP( 0x11 )  \
    __parm              [] \
    __value             [__cx] \
    __modify __exact    [__ax __bh __cx __dl __edi __es]    /* EDI workaround bug in DOS4G */
#endif

extern void _BIOSVideoEGACharPattSet( unsigned char vidroutine );
#pragma aux _BIOSVideoEGACharPattSet = \
        "xor  bl,bl"            \
        _INT_10_FN_xBP( 0x11 )  \
    __parm              [__al] \
    __value             \
    __modify __exact    [__ah __bl]

extern void _BIOSVideoCharPattSet( unsigned char vidroutine, unsigned char bytesperchar, unsigned short patterncount, unsigned short charoffset, unsigned short table_rmseg, unsigned table_offset );
#ifdef _M_I86
#pragma aux _BIOSVideoCharPattSet = \
        "xchg   bp,si"          \
        "xor    bl,bl"          \
        _INT_10_FN_xBP( 0x11 )  \
        "xchg   bp,si"          \
    __parm              [__al] [__bh] [__cx] [__dx] [__es] [__si] \
    __value             \
    __modify __exact    [__ah __bl]
#else
#pragma aux _BIOSVideoCharPattSet = \
        "xchg   ebp,esi"        \
        "xor    bl,bl"          \
        _INT_10_FN_xBP( 0x11 )  \
        "xchg   ebp,esi"        \
    __parm              [__al] [__bh] [__cx] [__dx] [__es] [__esi] \
    __value             \
    __modify __exact    [__ah __bl]
#endif

extern int10_ega_info _BIOSVideoEGAInfo( void );
#ifdef _M_I86
#pragma aux _BIOSVideoEGAInfo = \
        "mov    bx,0ff10h"      \
        _INT_10_FN_xBP( 0x12 )  \
    __parm              [] \
    __value             [__cx __bx] \
    __modify __exact    [__ah __bx __cx]
#else
#pragma aux _BIOSVideoEGAInfo = \
        "mov  bx,0ff10h"        \
        _INT_10_FN_xBP( 0x12 )  \
        "shl  ecx,10h"          \
        "mov  cx,bx"            \
    __parm              [] \
    __value             [__ecx] \
    __modify __exact    [__ah __bx __ecx]
#endif

#ifdef _M_I86
extern unsigned _BIOSVideoDevCombCode( void );
#pragma aux _BIOSVideoDevCombCode = \
        "xor  al,al"            \
        _INT_10_FN_xBP( 0x1a )  \
        "cmp  al,1ah"           \
        "jz short L1"           \
        "sub  bx,bx"            \
    "L1:"                       \
    __parm              [] \
    __value             [__bx] \
    __modify __exact    [__ax __bx]
#endif

#ifdef _M_I86
extern void __far * _BIOSVideo_desqview_shadow_buffer( void __far * );
#pragma aux _BIOSVideo_desqview_shadow_buffer = \
        _INT_10_FN( 0xfe )      \
    __parm              [__es __di] \
    __value             [__es __di] \
    __modify __exact    [__ah __es __di]

extern void __far * _BIOSVideo_desqview_update( void __far * buff, unsigned size );
#pragma aux _BIOSVideo_desqview_update = \
        _INT_10_FN( 0xff )      \
    __parm              [__es __di] [__cx] \
    __value             [__es __di] \
    __modify __exact    [__ah __cx __es __di]
#endif

#endif  /* defined( __DOS__ ) || defined( __WINDOWS__ ) */

#endif  /* _INT10_H_INCLUDED */
