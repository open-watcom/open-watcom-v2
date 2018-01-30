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


#ifndef _BIOSUI_H_
#define _BIOSUI_H_

#ifdef __386__
#include "extender.h"
#endif

#ifdef _M_I86
#define FIRSTMEG(a,b) MK_FP((a),(b))
#else
#define FIRSTMEG(a,b) MK_FP( _ExtenderRealModeSelector, (((unsigned)(a)) << 4) + (b))
#endif

#if defined(__OSI__)
 #define _INT_10        "call __Int10"
 #define _INT_16        "call __Int16"
 #define _INT_21        "call __Int21"
#else
 #define _INT_10        "int 10h"
 #define _INT_16        "int 16h"
 #define _INT_21        "int 21h"
#endif

#define BIOS_VIDEO      0x10
#define BIOS_MOUSE      0x33

#define BIOS_PAGE           0x40
#define BIOS_SCREEN_OFFSET  0x4e
#define BIOS_SYSTEM_CLOCK   0x6c

#define MOUSE_DRIVER_OK     ((unsigned short)-1)

#define _INT_33         0xCD BIOS_MOUSE

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
    modify [ax bx];

extern void MouseDrvState( unsigned short, md_stk_ptr );
#ifdef _M_I86
#pragma aux MouseDrvState = \
        _INT_33             \
        "mov ss:[si+0],bx"  \
        "mov ss:[si+2],cx"  \
        "mov ss:[si+4],dx"  \
    parm [ax] [si] modify [bx cx dx];
#else
#pragma aux MouseDrvState = \
        _INT_33             \
        "mov ss:[esi+0],bx" \
        "mov ss:[esi+2],cx" \
        "mov ss:[esi+4],dx" \
    parm [ax] [esi] modify [bx cx dx];
#endif

extern unsigned short MouseDrvCall1( unsigned short );
#pragma aux MouseDrvCall1 = \
        _INT_33             \
    parm [ax] modify [cx dx];

extern unsigned short MouseDrvCall2( unsigned short, unsigned short, unsigned short, unsigned short );
#pragma aux MouseDrvCall2 = \
        _INT_33             \
    parm [ax] [bx] [cx] [dx];

extern void MouseDrvCall3( unsigned short, unsigned short, unsigned short, unsigned short, unsigned short );
#pragma aux MouseDrvCall3 = \
        _INT_33             \
    parm [ax] [cx] [dx] [si] [di];

extern unsigned char BIOSGetPage( void );
#pragma aux BIOSGetPage = \
        "push   bp"         \
        "mov    ah,0fh"     \
        _INT_10             \
        "pop    bp"         \
    parm caller [ax] value [bh] modify [bx];

extern void BIOSSetMode( unsigned );
#pragma aux BIOSSetMode = \
        "push   bp"         \
        "xor    ah,ah"      \
        _INT_10             \
        "pop    bp"         \
    parm caller [ax];

extern unsigned char BIOSGetMode( void );
#pragma aux BIOSGetMode = \
        "push   bp"         \
        "mov    ah,0fh"     \
        _INT_10             \
        "pop    bp"         \
    parm caller [ax] modify [bx];

extern unsigned char BIOSGetColumns( void );
#pragma aux BIOSGetColumns = \
        "push   bp"         \
        "mov    ah,0fh"     \
        _INT_10             \
        "pop    bp"         \
    parm caller [ax] value [ah] modify [bx];

extern unsigned char BIOSGetRows( void );
#pragma aux BIOSGetRows = \
        "push   es"         \
        "push   bp"         \
        "mov    ax,1130h"   \
        "xor    bh,bh"      \
        _INT_10             \
        "inc    dl"         \
        "pop    bp"         \
        "pop    es"         \
    parm value [dl] modify [ax bx cx];

extern struct ega_info BIOSEGAInfo( void );
#ifdef __386__
#pragma aux BIOSEGAInfo = \
        "push   ebp"        \
        "mov    ah,12h"     \
        "mov    bx,0ff10h"  \
        _INT_10             \
        "mov    eax,ecx"    \
        "shl    eax,10h"    \
        "or     ax,bx"      \
        "pop    ebp"        \
    parm modify [cx bx];
#else
#pragma aux BIOSEGAInfo = \
        "push   bp"         \
        "mov    ah,12h"     \
        "mov    bx,0ff10h"  \
        _INT_10             \
        "mov    ax,bx"      \
        "mov    dx,cx"      \
        "pop    bp"         \
    parm modify [cx bx];
#endif

extern unsigned char BIOSSumming( char );
#pragma aux BIOSSumming = \
        "push   bp"         \
        "mov    ah,12h"     \
        "mov    bl,33h"     \
        _INT_10             \
        "pop    bp"         \
    parm [al] value [al] modify [cx bx dx];

extern unsigned short BIOSGetKeyboard( char );
#pragma aux  BIOSGetKeyboard = \
        _INT_16                 \
    parm[ah] value[ax];


extern unsigned char BIOSKeyboardHit( char );
#pragma aux BIOSKeyboardHit = \
        _INT_16                 \
        "jz short L1"           \
        "mov    al,1"           \
        "jmp short L2"          \
    "L1: xor    al,al"          \
    "L2:"                       \
    parm[ah] value[al];

extern unsigned short BIOSTestKeyboard( void );
#pragma aux BIOSTestKeyboard =  \
        "mov    ax,12ffh"       \
        _INT_16                 \
    value [ax];

extern unsigned short BIOSGetCharAttr( unsigned char page );
#pragma aux  BIOSGetCharAttr = \
        "push   bp"             \
        "mov    ah,8"           \
        _INT_10                 \
        "pop    bp"             \
    parm caller [bh] value [ax];

extern void BIOSSetCharAttr( unsigned short char_attr, unsigned char page );
#pragma aux BIOSSetCharAttr = \
        "push   bp"             \
        "push   cx"             \
        "push   bx"             \
        "mov    bl,ah"          \
        "mov    cx,1"           \
        "mov    ah,9"           \
        _INT_10                 \
        "pop    bx"             \
        "pop    cx"             \
        "pop    bp"             \
    parm caller [ax] [bh];

extern struct cursor_pos  BIOSGetCurPos( unsigned char page );
#pragma aux BIOSGetCurPos = \
        "push   bp"         \
        "mov    ah,3"       \
        _INT_10             \
        "pop    bp"         \
    parm caller [bh] value [dx] modify [bx cx dx];

extern void BIOSSetCurPos( unsigned char row, unsigned char col, unsigned char page );
#pragma aux BIOSSetCurPos = \
        "push   bp"         \
        "mov    ah,2"       \
        _INT_10             \
        "pop    bp"         \
    parm caller [dh] [dl] [bh] modify [bx cx dx];

extern unsigned short BIOSGetCurTyp( unsigned char page );
#pragma aux BIOSGetCurTyp = \
        "push   bp"         \
        "mov    ah,3"       \
        _INT_10             \
        "pop    bp"         \
    parm caller [bh] value [cx] modify [bx cx dx];

extern void BIOSSetCurTyp( unsigned char top_line, unsigned char bot_line );
#pragma aux BIOSSetCurTyp = \
        "push   bp"             \
        "mov    ah,1"           \
        _INT_10                 \
        "pop    bp"             \
    parm caller [ch] [cl] modify [cx];


#if defined(__OSI__)
extern void         __Int10();
extern void         __Int16();
extern void         __Int21();
#endif

#endif // _BIOSUI_H_
