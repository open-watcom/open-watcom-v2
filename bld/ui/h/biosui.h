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
 #define _INT_33        "call __Int33"
#else
 #define _INT_10        "int 10h"
 #define _INT_16        "int 16h"
 #define _INT_21        "int 21h"
 #define _INT_33        "int 33h"
#endif

#define BIOS_VIDEO      0x10
#define BIOS_MOUSE      0x33

#define BIOS_PAGE           0x40
#define BIOS_SCREEN_OFFSET  0x4e
#define BIOS_SYSTEM_CLOCK   0x6c

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

extern void BIOSSetMode( unsigned char );
#pragma aux BIOSSetMode = \
        "xor    ah,ah"      \
        _INT_10             \
    parm [al] modify [ah];

extern void BIOSSetCurTyp( unsigned char top_line, unsigned char bot_line );
#pragma aux BIOSSetCurTyp = \
        "mov    ah,1"           \
        _INT_10                 \
    parm [ch] [cl] modify [ax cx];

extern void BIOSSetCurPos( unsigned char row, unsigned char col, unsigned char page );
#pragma aux BIOSSetCurPos = \
        "mov    ah,2"       \
        _INT_10             \
    parm [dh] [dl] [bh];

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

extern unsigned short BIOSGetCharAttr( unsigned char page );
#pragma aux  BIOSGetCharAttr = \
        "mov    ah,8"           \
        _INT_10                 \
    parm [bh] value [ax];

extern void BIOSSetCharAttr( unsigned short char_attr, unsigned char page );
#pragma aux BIOSSetCharAttr = \
        "mov    bl,ah"          \
        "mov    cx,1"           \
        "mov    ah,9"           \
        _INT_10                 \
    parm [ax] [bh] modify [bx cx];

extern unsigned char BIOSGetPage( void );
#pragma aux BIOSGetPage = \
        "mov    ah,0fh"     \
        _INT_10             \
    value [bh] modify [ax bx];

extern unsigned char BIOSGetMode( void );
#pragma aux BIOSGetMode = \
        "mov    ah,0fh"     \
        _INT_10             \
    value [al] modify [ax bx];

extern unsigned char BIOSGetColumns( void );
#pragma aux BIOSGetColumns = \
        "mov    ah,0fh"     \
        _INT_10             \
    value [ah] modify [ax bx];

extern unsigned char BIOSGetRows( void );
#ifdef _M_I86
#pragma aux BIOSGetRows = \
        "push   es"         \
        "push   bp"         \
        "mov    ax,1130h"   \
        "xor    bh,bh"      \
        _INT_10             \
        "inc    dl"         \
        "pop    bp"         \
        "pop    es"         \
    value [dl] modify [ax bx cx];
#else
#pragma aux BIOSGetRows = \
        "push   es"         \
        "push   ebp"        \
        "mov    ax,1130h"   \
        "xor    bh,bh"      \
        _INT_10             \
        "inc    dl"         \
        "pop    ebp"        \
        "pop    es"         \
    value [dl] modify [ax bx cx];
#endif

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


#if defined(__OSI__)
extern void         __Int10();
extern void         __Int16();
extern void         __Int21();
extern void         __Int33();
#endif

#endif // _BIOSUI_H_
