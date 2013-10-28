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
* Description:  Low-level helper pragmas for vi.
*
****************************************************************************/


#ifndef _PRAGMAS_INCLUDED
#define _PRAGMAS_INCLUDED

#if defined( __WATCOMC__ ) && defined( __DOS__ ) && defined( _M_IX86 )

extern unsigned char    In61( void );
extern void             Out61( unsigned char );
extern void             Out43( unsigned char );
extern void             Out42( unsigned char );

extern void (__interrupt _FAR *DosGetVect( char ))( void );
extern void DosSetVect( char, void (__interrupt *)( void ) );

extern unsigned short _BIOSGetKeyboard( char );
extern unsigned short _BIOSKeyboardHit( char );

#pragma aux In61 = 0xe4 0x61 value [al];
#pragma aux Out61 = 0xe6 0x61 parm [al];
#pragma aux Out43 = 0xe6 0x43 parm [al];
#pragma aux Out42 = 0xe6 0x42 parm [al];

extern void JustAnInt28( void );
#pragma aux JustAnInt28 = "int 28h";

extern unsigned DosMaxAlloc( void );

#ifndef __CURSES__
#pragma aux _BIOSGetKeyboard = \
        "int  16h" \
    parm [ah] value [ax];

#pragma aux _BIOSKeyboardHit = \
        "int  16h" \
        "jz   L1" \
        "mov  ax, 1" \
        "jmp  short L2" \
        "L1: mov ax, 0" \
        "L2:" \
    parm [ah] value [ax];

#pragma aux BIOSGetCursor = \
        0xB4 0x03          /* mov     ah, 03h */\
        0xCD 0x10          /* int     010h */ \
    parm [bh] value [dx] modify [ax cx];

#pragma aux BIOSSetCursor = \
        0xB4 0x02          /* mov     ah, 02h */\
        0xCD 0x10          /* int     010h */ \
    parm [bh] [dh] [dl] modify [ax];

#pragma aux BIOSNewCursor = \
        0xB4 0x01          /* mov     ah,01h */ \
        0xCD 0x10          /* int     010h */ \
    parm [ch] [cl] modify [ax cx];
#endif

#if defined( _M_I86 )
#pragma aux DosSetVect = \
        "mov  ah, 25h" \
        "int  21h" \
    parm [al] [ds dx];

#pragma aux DosGetVect = \
        "mov  ah, 35h" \
        "int  21h" \
        "mov  ax, bx" \
        "mov  dx, es" \
    parm [al] modify [es bx];

#pragma aux DosMaxAlloc = \
        "xor bx, bx" \
        "dec bx" \
        "mov ah, 48h" \
        "int 21h" \
    value [bx] modify [ax];

#pragma aux BIOSSetColorRegister = \
        0xB8 0x10 0x10     /* mov     ax, 01010h */ \
        0xCD 0x10          /* int     010h */ \
    parm [bx] [dh] [ch] [cl] modify [ax];

#pragma aux BIOSGetColorPalette = \
        0xB8 0x09 0x10     /* mov     ax, 01009h */ \
        0xCD 0x10          /* int     010h */ \
    parm [es dx] modify [ax];

#pragma aux BIOSSetBlinkAttr = \
        0xB8 0x03 0x10    /* mov    ax, 01003h */ \
        0xB3 0x01         /* mov    bl, 1 */ \
        0xCD 0x10         /* int    010h */ \
    modify [ax bx];

#pragma aux BIOSSetNoBlinkAttr = \
        0xB8 0x03 0x10    /* mov    ax, 01003h */ \
        0xB3 0x00         /* mov    bl, 0 */ \
        0xCD 0x10         /* int    010h */ \
    modify [ax bx];

#pragma aux BIOSTestKeyboard = \
        0xB8 0xff 0x12     /* mov     ax, 012ffh */ \
        0xCD 0x16          /* int     016h */ \
    value[ax];

#pragma aux BIOSGetRowCount = \
        0xB8 0x30 0x11     /* mov     ax, 01130h */ \
        0xB7 0x00          /* mov     bh, 0 */ \
        0xB2 0x18          /* mov     dl, 24 */ \
        0x55               /* push    bp - Bloodly BIOS scrams bp */ \
        0xCD 0x10          /* int     010h */ \
        0x5D               /* pop     bp */ \
    value [dl] modify [ax bx cx dx es];

#pragma aux BIOSGetVideoMode = \
        0xB4 0x0F          /* mov     ah, 0fh */\
        0xCD 0x10          /* int     010h */ \
    value [bx ax] modify [bx];

#pragma aux BIOSGetColorRegister = \
        0xB8 0x15 0x10    /* mov    ax, 01015h */ \
        0xCD 0x10         /* int    010h */ \
    parm [bx] value [cx dx] modify [ax cx dx];
#else
#pragma aux DosMaxAlloc = \
        "xor ebx, ebx" \
        "dec ebx" \
        "mov ah, 48h" \
        "int 21h" \
    value [ebx] modify [eax];

#pragma aux DosSetVect = \
        "push ds" \
        "push fs" \
        "pop  ds" \
        "mov  ah, 25h" \
        "int  21h" \
        "pop  ds" \
    parm [al] [fs edx];

#pragma aux DosGetVect = \
        "push es" \
        "mov  ah, 35h" \
        "int  21h" \
        "mov  eax, ebx" \
        "mov  edx, es" \
        "pop  es" \
    parm [al] value [dx eax] modify [ebx];

#pragma aux BIOSSetColorRegister = \
        0x66 0xB8 0x10 0x10     /* mov     ax, 01010h */ \
        0xCD 0x10               /* int     010h */ \
    parm [bx] [dh] [ch] [cl] modify [ax];

#pragma aux BIOSGetColorPalette = \
        0x06                    /* push es */ \
        0x8e 0xc0               /* mov  es, ax */ \
        0x66 0xB8 0x09 0x10     /* mov  ax, 01009h */ \
        0xCD 0x10               /* int  010h */ \
        0x07                    /* pop  es */ \
    parm [ax dx] modify [ax];

#pragma aux BIOSSetBlinkAttr = \
        0x66 0xB8 0x03 0x10     /* mov ax, 01003h */ \
        0xB3 0x01               /* mov bl, 1 */ \
        0xCD 0x10               /* int 010h */ \
    modify [ax bx];

#pragma aux BIOSSetNoBlinkAttr = \
        0x66 0xB8 0x03 0x10     /* mov ax, 01003h */ \
        0xB3 0x00               /* mov bl, 0 */ \
        0xCD 0x10               /* int 010h */ \
    modify [ax bx];

#pragma aux BIOSTestKeyboard = \
        0x66 0xB8 0xff 0x12     /* mov ax, 012ffh */ \
        0xCD 0x16               /* int 016h */ \
    value[ax];

#pragma aux BIOSGetRowCount = \
        0x66 0xB8 0x30 0x11     /* mov  ax, 01130h */ \
        0xB7 0x00               /* mov  bh, 0 */ \
        0xB2 0x18               /* mov  dl, 24 */ \
        0x55                    /* push bp - Bloodly BIOS scrams bp */ \
        0x06                    /* push es - Bloodly BIOS scrams es */ \
        0xCD 0x10               /* int  010h */ \
        0x07                    /* pop  es */ \
        0x5D                    /* pop  bp */ \
    value [dl] modify [ax bx cx dx];

#pragma aux BIOSGetVideoMode = \
        0xB4 0x0F          /* mov     ah, 0fh */\
        0xCD 0x10          /* int     010h */ \
        0xC1 0xE3 0x10     /* shl     ebx, 16 */ \
        0x66 0x8B 0xD8     /* mov     bx, ax */ \
    value [ebx] modify [ax];

#pragma aux BIOSGetColorRegister = \
        0x66 0xB8 0x15 0x10     /* mov  ax, 01015h */ \
        0xCD 0x10               /* int  010h */ \
        0x66 0x8B 0xC1          /* mov  ax, cx */ \
        0xC1 0xE0 0x10          /* shr  eax, 16 */ \
        0x66 0x8B 0xC2          /* mov  ax, dx */ \
    parm [bx] value [eax] modify [ax cx dx];

#endif

#endif

#endif
