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


#ifndef _PRAGMAS_INCLUDED
#define _PRAGMAS_INCLUDED

#if defined(__OS2__) || defined(__QNX__) || defined(__NT__)
#define NO_INLINE
#endif

extern char In61( void );
extern void Out61( char );
extern void Out43( char );
extern void Out42( char );
extern U_INT DosMaxAlloc( void );
extern long DosGetFullPath( char *, char * );
extern void _FAR *DosGetVect( char );
extern void DosSetVect( char, void _FAR * );
extern void BIOSSetColorRegister( short, char, char, char );
extern void BIOSGetColorPalette( void _FAR * );
extern void BIOSSetBlinkAttr( void );
extern void BIOSSetNoBlinkAttr( void );
extern short BIOSTestKeyboard( void );
extern short BIOSGetKeyboard( char );
extern short BIOSKeyboardHit( char );
extern char BIOSGetRowCount( void );
extern unsigned long BIOSGetVideoMode( void );
extern short BIOSGetCursor( char );
extern void BIOSSetCursor( char, char, char );
extern void BIOSNewCursor( char, char );
extern long BIOSGetColorRegister( short );
extern char near *GetSP( void );
extern char near *GetBP( void );
extern void SetBP( char near * );
extern void SetSP( char near * );
extern char IsWindows( void );
extern char HasShareDOS( void );

#ifdef __AXP__
#else
#pragma aux IsWindows = \
        "mov    ax,1600h" \
        "int    2fh" \
        "cmp    al,0" \
        "je     notok" \
        "cmp    al,080h" \
        "je     notok" \
        "mov    al,1" \
        "jmp    done" \
        "notok:" \
        "mov    al,0" \
        "done:" \
        value [al];

#pragma aux HasShareDOS = \
        "mov    ax,1000h" \
        "int    2fh" \
        "cmp    al,0ffh" \
        "je     ok" \
        "mov    al,0" \
        "jmp    done" \
        "ok:" \
        "mov    al,1" \
        "done:" \
        value [al];

#ifdef __386__
#pragma aux GetSP = \
    "mov eax,esp" \
    value [eax];
#pragma aux GetBP = \
    "mov eax,ebp" \
    value [eax];
#pragma aux SetSP = \
    "mov esp,eax" \
    parm [eax] modify[esp];
#pragma aux SetBP = \
    "mov ebp,eax" \
    parm [eax];
#else
#pragma aux GetSP = \
    "mov ax,sp" \
    value [ax];
#pragma aux GetBP = \
    "mov ax,bp" \
    value [ax];
#pragma aux SetSP = \
    "mov sp,ax" \
    parm [ax] modify[sp];
#pragma aux SetBP = \
    "mov bp,ax" \
    parm [ax];
#endif

#if !defined(NO_INLINE)

#pragma aux In61 = 0xe4 0x61 value [al];
#pragma aux Out61 = 0xe6 0x61 parm [al];
#pragma aux Out43 = 0xe6 0x43 parm [al];
#pragma aux Out42 = 0xe6 0x42 parm [al];

#ifndef __CURSES__
#pragma aux BIOSGetKeyboard = \
         0xCD 0x16          /* int     016h */ \
         parm[ah] value[ax];

#pragma aux BIOSGetCursor = \
         0xB4 0x03          /* mov     ah,03h */\
         0xCD 0x10          /* int     010h */ \
         parm[bh] value [dx] modify[ax cx];

#pragma aux BIOSSetCursor = \
        0xB4 0x02          /* mov     ah,02h */\
        0xCD 0x10          /* int     010h */ \
        parm [bh] [dh] [dl] modify[ax];

#pragma aux BIOSNewCursor = \
        0xB4 0x01          /* mov     ah,01h */ \
        0xCD 0x10          /* int     010h */ \
        parm [ch] [cl] modify[ax cx];
#endif

#if !defined(__386__)
#pragma aux DosSetVect = \
        0xb4 0x25     /* mov    ah,25H */ \
        0xcd 0x21     /* int    21h */ \
        parm [al] [ds dx];

#pragma aux DosGetVect = \
        0xb4 0x35     /* mov    ah,35H */ \
        0xcd 0x21     /* int    21h */ \
        0x89 0xd8     /* mov     ax,bx */ \
        0x8c 0xc2     /* mov     dx,es */ \
        parm [al] modify [es bx];

#ifndef __WINDOWS__
#pragma aux DosGetFullPath = \
        0xb4 0x60     /* mov    ah,60H */ \
        0xcd 0x21     /* int    21h */ \
        0x1b 0xd2     /* sbb    dx,dx */ \
        parm [ds si] [es di] value [dx ax];
#else
#pragma aux DosGetFullPath = \
        "push   ds" \
        "push   si" \
        "mov    ds,dx" \
        "mov    si,ax" \
        "mov    ah,060h" \
        "int    21h" \
        "sbb    dx,dx" \
        "pop    si" \
        "pop    ds" \
        parm [dx ax] [es di] value [dx ax];
#endif

#pragma aux DosMaxAlloc = \
        0x31 0xdb       /* xor bx,bx */ \
        0x4b            /* dec bx */ \
        0xb4 0x48       /* mov ah, 0x48 */ \
        0xcd 0x21       /* int    21h */ \
        value [bx] modify [ax];

#pragma aux BIOSSetColorRegister = \
        0xB8 0x10 0x10     /* mov     ax,01010h */ \
        0xCD 0x10          /* int     010h */ \
        parm [bx] [dh] [ch] [cl] modify[ax];

#pragma aux BIOSGetColorPalette = \
        0xB8 0x09 0x10     /* mov     ax,01009h */ \
        0xCD 0x10          /* int     010h */ \
        parm [es dx] modify[ax];

#pragma aux BIOSSetBlinkAttr = \
        0xB8 0x03 0x10    /* mov ax,01003h */ \
        0xB3 0x01         /* mov bl,1 */ \
        0xCD 0x10         /* int  010h */ \
        modify [ax bx];

#pragma aux BIOSSetNoBlinkAttr = \
        0xB8 0x03 0x10    /* mov ax,01003h */ \
        0xB3 0x00         /* mov bl,0 */ \
        0xCD 0x10         /* int  010h */ \
        modify [ax bx];

#pragma aux BIOSTestKeyboard = \
        0xB8 0xff 0x12     /* mov     ax,012ffh */ \
        0xCD 0x16          /* int     016h */ \
        value[ax];

#pragma aux BIOSKeyboardHit = \
        0xCD 0x16          /* int     016h */ \
        0x74 0x05          /* jz     foo1 */ \
        0xB8 0x01 0x00     /* mov     ax,1 */ \
        0xEB 0x03          /* jmp    short foo2 */ \
        0xB8 0x00 0x00     /* foo1:   mov     ax,0 */ \
        parm[ah] value[ax];

#pragma aux BIOSGetRowCount = \
        0xB8 0x30 0x11     /* mov     ax,01130h */ \
        0xB7 0x00          /* mov     bh,0 */ \
        0xB2 0x18          /* mov     dl,24 */ \
        0x55               /* push    bp - Bloodly BIOS scrams bp */ \
        0xCD 0x10          /* int     010h */ \
        0x5D               /* pop     bp */ \
        value [dl] modify[ax bx cx dx es];

#pragma aux BIOSGetVideoMode = \
        0xB4 0x0F          /* mov     ah,0fh */\
        0xCD 0x10          /* int     010h */ \
        value [bx ax] modify[bx];

#pragma aux BIOSGetColorRegister = \
        0xB8 0x15 0x10    /* mov ax,01015h */ \
        0xCD 0x10         /* int  010h */ \
        parm[bx] value[cx dx] modify [ax cx dx];
#else
#pragma aux DosMaxAlloc = \
        0x31 0xdb       /* xor ebx,ebx */ \
        0x4b            /* dec ebx */ \
        0xb4 0x48       /* mov ah, 0x48 */ \
        0xcd 0x21       /* int    21h */ \
        value [ebx] modify [eax];

extern void DosSetVect( char, void _FAR * );
#pragma aux DosSetVect = \
        0x1e            /* push ds */ \
        0x0f 0xa0       /* push fs */ \
        0x1f            /* pop  ds */ \
        0xb4 0x25       /* mov    ah,25H */ \
        0xcd 0x21       /* int    21h */ \
        0x1f            /* pop  ds */ \
        parm [al] [fs edx];

#pragma aux DosGetVect = \
        0x06          /* push   es */ \
        0xb4 0x35     /* mov    ah,35H */ \
        0xcd 0x21     /* int    21h */ \
        0x89 0xd8     /* mov    eax,ebx */ \
        0x8c 0xc2     /* mov    dx,es */ \
        0x07          /* pop    es */ \
        parm [al] value[dx eax] modify [ebx];

#pragma aux DosGetFullPath = \
        0xb4 0x60     /* mov    ah,60H */ \
        0xcd 0x21     /* int    21h */ \
        parm [esi] [edi];

#pragma aux BIOSSetColorRegister = \
        0x66 0xB8 0x10 0x10     /* mov     ax,01010h */ \
        0xCD 0x10               /* int     010h */ \
        parm [bx] [dh] [ch] [cl] modify[ax];

#pragma aux BIOSGetColorPalette = \
        0x66 0xB8 0x09 0x10     /* mov     ax,01009h */ \
        0xCD 0x10          /* int     010h */ \
        parm [es dx] modify[ax];

#pragma aux BIOSSetBlinkAttr = \
        0x66 0xB8 0x03 0x10    /* mov ax,01003h */ \
        0xB3 0x01         /* mov bl,1 */ \
        0xCD 0x10         /* int  010h */ \
        modify [ax bx];

#pragma aux BIOSSetNoBlinkAttr = \
        0x66 0xB8 0x03 0x10    /* mov ax,01003h */ \
        0xB3 0x00         /* mov bl,0 */ \
        0xCD 0x10         /* int  010h */ \
        modify [ax bx];

#pragma aux BIOSTestKeyboard = \
        0x66 0xB8 0xff 0x12     /* mov     ax,012ffh */ \
        0xCD 0x16               /* int     016h */ \
        value[ax];

#pragma aux BIOSKeyboardHit = \
        0xCD 0x16               /* int     016h */ \
        0x74 0x06               /* jz     foo1 */ \
        0x66 0xB8 0x01 0x00     /* mov     ax,1 */ \
        0xEB 0x04               /* jmp    short foo2 */ \
        0x66 0xB8 0x00 0x00     /* foo1:   mov     ax,0 */ \
        parm[ah] value[ax];

#pragma aux BIOSGetRowCount = \
        0x66 0xB8 0x30 0x11     /* mov     ax,01130h */ \
        0xB7 0x00               /* mov     bh,0 */ \
        0xB2 0x18               /* mov     dl,24 */ \
        0x55                    /* push    bp - Bloodly BIOS scrams bp */ \
        0x06                    /* push    es - Bloodly BIOS scrams es */ \
        0xCD 0x10               /* int     010h */ \
        0x07                    /* pop     es */ \
        0x5D                    /* pop     bp */ \
        value [dl] modify[ax bx cx dx];

#pragma aux BIOSGetVideoMode = \
        0xB4 0x0F          /* mov     ah,0fh */\
        0xCD 0x10          /* int     010h */ \
        0xC1 0xE3 0x10     /* shl     ebx,16 */ \
        0x66 0x8B 0xD8     /* mov     bx,ax */ \
        value [ebx] modify[ax];

#pragma aux BIOSGetColorRegister = \
        0x66 0xB8 0x15 0x10    /* mov ax,01015h */ \
        0xCD 0x10         /* int  010h */ \
        0x66 0x8B 0xC1    /* mov     ax,cx */ \
        0xC1 0xE0 0x10    /* shr     eax,16 */ \
        0x66 0x8B 0xC2    /* mov     ax,dx */ \
        parm[bx] value[eax] modify [ax cx dx];

#endif
#endif
#endif
#endif
