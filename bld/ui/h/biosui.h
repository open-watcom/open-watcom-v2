/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2018 The Open Watcom Contributors. All Rights Reserved.
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

#ifndef _M_I86
#include "extender.h"
#endif


#if defined(__OSI__)
 #define _INT_10        "call __Int10"
 #define _INT_16        "call __Int16"
 #define _INT_21        "call __Int21"
 #define _INT_33        "call __Int33"
#else
 #define _INT_10        "int 10h"
 #define _INT_16        "int 16h"
 #define _INT_21        "int 21h"
 #define _INT_33        "int 33h"
#endif

#define STR(...)        #__VA_ARGS__
#define INSTR(...)      STR(__VA_ARGS__)

#ifdef _M_I86
#define REG_xBP         bp
#define REG_xSI         si
#else
#define REG_xBP         ebp
#define REG_xSI         esi
#endif

#define BIOS_VIDEO      0x10
#define BIOS_MOUSE      0x33

#define BIOS_CURR_VIDEO_MODE    0x49    /* byte */
#define BIOS_SCREEN_OFFSET      0x4e    /* word */
#define BIOS_SYSTEM_CLOCK       0x6c    /* dword */
#define BIOS_POINT_HEIGHT       0x85    /* byte */

#ifdef _M_I86
#define RealModeDataPtr( s, o ) MK_FP((s),(o))
#else
#define RealModeDataPtr( s, o ) EXTENDER_RM2PM( s, o )
#endif
#define RealModeData( s, o, t ) *(t __far *)RealModeDataPtr( s, o )

#define BIOSData( p, t )        RealModeData( 0x0040, p, t )
#define VIDEOData( s, p )       RealModeData( s, p, unsigned char )


#define MOUSE_DRIVER_OK     ((unsigned short)-1)

#define VIDMONOINDXREG      0x03B4
#define VIDCOLORINDXREG     0x03D4

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


extern void DOSIdleInterrupt( void );
#pragma aux DOSIdleInterrupt = "int 28h";

extern unsigned short MouseDrvReset( void );
#pragma aux MouseDrvReset = \
        "xor  ax,ax"        \
        _INT_33             \
    value [ax] modify [ax bx];

extern void MouseDrvCallRetState( unsigned short, md_stk_ptr );
#pragma aux MouseDrvCallRetState = \
        _INT_33                         \
        INSTR( mov ss:[REG_xSI+0],bx )  \
        INSTR( mov ss:[REG_xSI+2],cx )  \
        INSTR( mov ss:[REG_xSI+4],dx )  \
    parm [ax] [REG_xSI] modify [bx cx dx];

extern unsigned short MouseDrvCall1( unsigned short );
#pragma aux MouseDrvCall1 = \
        _INT_33             \
    parm [ax] value [ax] modify [cx dx];

extern unsigned short MouseDrvCall2( unsigned short, unsigned short, unsigned short, unsigned short );
#pragma aux MouseDrvCall2 = \
        _INT_33             \
    parm [ax] [bx] [cx] [dx] value [ax];

extern void MouseDrvCall3( unsigned short, unsigned short, unsigned short, unsigned short, unsigned short );
#pragma aux MouseDrvCall3 = \
        _INT_33             \
    parm [ax] [cx] [dx] [si] [di];

extern unsigned short MouseDrvCall4( unsigned short );
#pragma aux MouseDrvCall4 = \
        _INT_33             \
    parm [ax] modify [bx cx dx];

extern void BIOSSetMode( unsigned char );
#pragma aux BIOSSetMode = \
        "xor    ah,ah"      \
        _INT_10             \
    parm [al] modify [ah];

extern void BIOSSetCurTyp( unsigned char top_line, unsigned char bot_line );
#pragma aux BIOSSetCurTyp = \
        "mov    ah,1"           \
        _INT_10                 \
    parm [ch] [cl] modify [ah];

extern void BIOSSetCurPos( unsigned char row, unsigned char col, unsigned char page );
#pragma aux BIOSSetCurPos = \
        "mov    ah,2"       \
        _INT_10             \
    parm [dh] [dl] [bh] modify [ah];

extern struct cursor_pos  BIOSGetCurPos( unsigned char page );
#pragma aux BIOSGetCurPos = \
        "mov    ah,3"       \
        _INT_10             \
    parm [bh] value [dx] modify [ax cx];

extern unsigned short BIOSGetCurTyp( unsigned char page );
#pragma aux BIOSGetCurTyp = \
        "mov    ah,3"       \
        _INT_10             \
    parm [bh] value [cx] modify [ax dx];

extern PIXEL BIOSGetCharPixel( unsigned char page );
#pragma aux  BIOSGetCharPixel = \
        "mov    ah,8"           \
        _INT_10                 \
    parm [bh] value [ax];

extern void BIOSSetCharPixel( PIXEL char_attr, unsigned char page );
#pragma aux BIOSSetCharPixel = \
        "mov    bl,ah"          \
        "mov    cx,1"           \
        "mov    ah,9"           \
        _INT_10                 \
    parm [ax] [bh] modify [ah bl cx];

extern unsigned char BIOSGetPage( void );
#pragma aux BIOSGetPage = \
        "mov    ah,0fh"     \
        _INT_10             \
    value [bh] modify [ax];

extern unsigned char BIOSGetMode( void );
#pragma aux BIOSGetMode = \
        "mov    ah,0fh"     \
        _INT_10             \
    value [al] modify [ah bh];

extern unsigned char BIOSGetColumns( void );
#pragma aux BIOSGetColumns = \
        "mov    ah,0fh"     \
        _INT_10             \
    value [ah] modify [al bh];

extern unsigned char BIOSGetRows( void );
#pragma aux BIOSGetRows = \
        INSTR( push REG_xBP )\
        "push   es"         \
        "mov    ax,1130h"   \
        "xor    bh,bh"      \
        _INT_10             \
        "inc    dl"         \
        "pop    es"         \
        INSTR( pop REG_xBP )\
    value [dl] modify [ax bh cx];

extern struct ega_info BIOSEGAInfo( void );
#ifdef _M_I86
#pragma aux BIOSEGAInfo = \
        "mov    ah,12h"     \
        "mov    bx,0ff10h"  \
        _INT_10             \
        "mov    ax,bx"      \
        "mov    dx,cx"      \
    value [dx ax] modify [bx cx];
#else
#pragma aux BIOSEGAInfo = \
        "mov    ah,12h"     \
        "mov    bx,0ff10h"  \
        _INT_10             \
        "mov    eax,ecx"    \
        "shl    eax,10h"    \
        "or     ax,bx"      \
    value [eax] modify [bx cx];
#endif

extern unsigned short BIOSGetKeyboard( char );
#pragma aux  BIOSGetKeyboard = \
        _INT_16                 \
    parm [ah] value [ax];


extern unsigned char BIOSKeyboardHit( char );
#pragma aux BIOSKeyboardHit = \
        _INT_16                 \
        "jz short L1"           \
        "mov    al,1"           \
        "jmp short L2"          \
    "L1: xor    al,al"          \
    "L2:"                       \
    parm [ah] value [al];

extern unsigned short BIOSTestKeyboard( void );
#pragma aux BIOSTestKeyboard =  \
        "mov    ax,12ffh"       \
        _INT_16                 \
    value [ax];

extern unsigned short   Points;     /* Number of lines per char */

#if defined(__OSI__)
extern void         __Int10();
extern void         __Int16();
extern void         __Int21();
extern void         __Int33();
#endif

#endif // _BIOSUI_H_
