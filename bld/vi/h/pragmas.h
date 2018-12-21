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
extern unsigned char _BIOSKeyboardHit( char );

extern int DoSpawn( void *, void * );
extern int GetFcb( void *, void * );

#pragma aux In61 = 0xe4 0x61 __value [__al]
#pragma aux Out61 = 0xe6 0x61 __parm [__al]
#pragma aux Out43 = 0xe6 0x43 __parm [__al]
#pragma aux Out42 = 0xe6 0x42 __parm [__al]

extern unsigned DosMaxAlloc( void );

#pragma aux DoSpawn = \
        "push ds"   \
        "push es"   \
        "push si"   \
        "push di"   \
        "mov  ds,dx"    /*  exe segment */ \
        "mov  dx,ax"    /*  exe offset */ \
        "mov  es,cx"    /*  parm block segment (offset in bx already) */ \
        "mov  ax,4b00h" /*  exec process */ \
        "int 21h"   \
        "jc short rcisright" \
        "mov  ax,4d00h" \
        "int 21h"   \
        "xor  ah,ah" \
    "rcisright:"    \
        "pop  di"   \
        "pop  si"   \
        "pop  es"   \
        "pop  ds"   \
    __parm      [__dx __ax] [__cx __bx] \
    __value     [__ax] \
    __modify    []

#pragma aux GetFcb = \
        "push ds"   \
        "push es"   \
        "push si"   \
        "push di"   \
        "mov  ds,dx"    /*  exe segment */ \
        "mov  si,ax"    /*  exe offset */ \
        "mov  es,cx"    /*  parm block segment (offset in bx already) */ \
        "mov  di,bx"    \
        "mov  ax,2901h" /*  parse filename/get fcb */ \
        "int 21h"   \
        "pop  di"   \
        "pop  si"   \
        "pop  es"   \
        "pop  ds"   \
    __parm      [__dx __ax] [__cx __bx] \
    __value     [__ax] \
    __modify    []

#ifndef __CURSES__

#pragma aux _BIOSGetKeyboard = \
        "int 16h"   \
    __parm      [__ah] \
    __value     [__ax]

#pragma aux _BIOSKeyboardHit = \
        "int 16h"       \
        "jz short L1"   \
        "mov  al,1"     \
        "jmp short L2"  \
    "L1: xor  al,al"    \
    "L2:"               \
    __parm      [__ah] \
    __value     [__al]

#pragma aux BIOSGetCursor = \
        "mov  ah,3" \
        "int 10h"   \
    __parm      [__bh] \
    __value     [__dx] \
    __modify    [__ax __cx]

#pragma aux BIOSSetCursor = \
        "mov  ah,2" \
        "int 10h"   \
    __parm      [__bh] [__dh] [__dl] \
    __modify    [__ax]

#pragma aux BIOSNewCursor = \
        "mov  ah,1" \
        "int 10h"   \
    __parm      [__ch] [__cl] \
    __modify    [__ax __cx]

#endif

#if defined( _M_I86 )

#pragma aux DosSetVect = \
        "mov  ah,25h"   \
        "int 21h"       \
    __parm      [__al] [__ds __dx] \
    __value     \
    __modify    [__ah]

#pragma aux DosGetVect = \
        "mov  ah,35h"   \
        "int 21h"       \
        "mov  ax,bx"    \
        "mov  dx,es"    \
    __parm      [__al] \
    __value     [__dx __ax] \
    __modify    [__es __bx]

#pragma aux DosMaxAlloc = \
        "xor  bx,bx"    \
        "dec  bx"       \
        "mov  ah,48h"   \
        "int 21h"       \
    __parm      [] \
    __value     [__bx] \
    __modify    [__ax]

#pragma aux BIOSSetColorRegister = \
        "mov  ax,1010h" \
        "int 10h"       \
    __parm      [__bx] [__dh] [__ch] [__cl] \
    __modify    [__ax]

#pragma aux BIOSGetColorPalette = \
        "mov  ax,1009h" \
        "int 10h"       \
    __parm      [__es __dx] \
    __modify    [__ax]

#pragma aux BIOSSetBlinkAttr = \
        "mov  ax,1003h" \
        "mov  bl,1"     \
        "int 10h"       \
    __modify    [__ax __bx]

#pragma aux BIOSSetNoBlinkAttr = \
        "mov  ax,1003h" \
        "xor  bl,bl"    \
        "int 10h"       \
    __modify    [__ax __bx]

#pragma aux BIOSTestKeyboard = \
        "mov  ax,12ffh" \
        "int 16h"       \
    __value     [__ax]

#pragma aux BIOSGetRowCount = \
        "mov  ax,1130h" \
        "xor  bh,bh"    \
        "mov  dl,18h"   \
        "push bp" /* Bloodly BIOS scrams bp */ \
        "int 10h"       \
        "pop  bp"       \
    __value     [__dl] \
    __modify    [__ax __bx __cx __dx __es]

#pragma aux BIOSGetVideoMode = \
        "mov    ah,0fh" \
        "int    10h"    \
    __value     [__bx __ax] \
    __modify    [__bx]

#pragma aux BIOSGetColorRegister = \
        "mov    ax,1015h"   \
        "int    10h"        \
    __parm      [__bx] \
    __value     [__cx __dx] \
    __modify    [__ax __cx __dx]

#else

#pragma aux DosMaxAlloc = \
        "xor  ebx,ebx"  \
        "dec  ebx"      \
        "mov  ah,48h"   \
        "int 21h"       \
    __parm      [] \
    __value     [__ebx] \
    __modify    [__eax]

#pragma aux DosSetVect = \
        "push   ds"     \
        "push   fs"     \
        "pop    ds"     \
        "mov    ah,25h" \
        "int    21h"    \
        "pop    ds"     \
    __parm      [__al] [__fs __edx] \
    __value     \
    __modify    [__ah]

#pragma aux DosGetVect =    \
        "push   es"         \
        "mov    ah,35h"     \
        "int    21h"        \
        "mov    eax,ebx"    \
        "mov    edx,es"     \
        "pop    es"         \
    __parm      [__al] \
    __value     [__dx __eax] \
    __modify    [__ebx]

#pragma aux BIOSSetColorRegister = \
        "mov    ax,1010h"   \
        "int    10h"        \
    __parm      [__bx] [__dh] [__ch] [__cl] \
    __modify    [__ax]

#pragma aux BIOSGetColorPalette = \
        "push   es"         \
        "mov    es,ax"      \
        "mov    ax,1009h"   \
        "int    10h"        \
        "pop    es"         \
    __parm      [__ax __dx] \
    __modify    [__ax]

#pragma aux BIOSSetBlinkAttr = \
        "mov    ax,1003h"   \
        "mov    bl,1"       \
        "int    10h"        \
    __modify    [__ax __bx]

#pragma aux BIOSSetNoBlinkAttr = \
        "mov    ax,1003h"   \
        "xor    bl,bl"      \
        "int    10h"        \
    __modify    [__ax __bx]

#pragma aux BIOSTestKeyboard = \
        "mov    eax,12ffh"  \
        "int    16h"        \
    __value     [__eax]

#pragma aux BIOSGetRowCount = \
        "mov    ax,1130h"   \
        "xor    bh,bh"      \
        "mov    dl,18h"     \
        "push   bp" /* Bloodly BIOS scrams bp */ \
        "push   es" /* Bloodly BIOS scrams es */ \
        "int    10h"        \
        "pop    es"         \
        "pop    bp"         \
    __value     [__dl] \
    __modify    [__ax __bx __cx __dx]

#pragma aux BIOSGetVideoMode = \
        "mov    ah,0fh"     \
        "int    10h"        \
        "shl    ebx,16"     \
        "mov    bx,ax"      \
    __value     [__ebx] \
    __modify    [__ax]

#pragma aux BIOSGetColorRegister = \
        "mov    ax,1015h"   \
        "int    10h"        \
        "mov    ax,cx"      \
        "shr    eax,16"     \
        "mov    ax,dx"      \
    __parm      [__bx] \
    __value     [__eax] \
    __modify    [__ax __cx __dx]

#endif

#endif

#endif
