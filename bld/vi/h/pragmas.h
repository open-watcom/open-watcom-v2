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
* Description:  Low-level helper pragmas for vi.
*
****************************************************************************/


#ifndef _PRAGMAS_H_INCLUDED
#define _PRAGMAS_H_INCLUDED

#if defined( __DOS__ ) || defined( __WINDOWS__ )

extern unsigned char    In61( void );
#pragma aux In61 = 0xe4 0x61 __value [__al]

extern void             Out61( unsigned char );
#pragma aux Out61 = 0xe6 0x61 __parm [__al]

extern void             Out43( unsigned char );
#pragma aux Out43 = 0xe6 0x43 __parm [__al]

extern void             Out42( unsigned char );
#pragma aux Out42 = 0xe6 0x42 __parm [__al]

extern void (__interrupt _FAR *DosGetVect( char ))( void );
#ifdef _M_I86
#pragma aux DosGetVect = \
        "mov    ah,35h" \
        "int 21h"       \
        "mov    ax,bx"  \
        "mov    dx,es"  \
    __parm      [__al] \
    __value     [__dx __ax] \
    __modify    [__es __bx]
#else
#pragma aux DosGetVect =    \
        "push   es"         \
        "mov    ah,35h"     \
        "int 21h"           \
        "mov    eax,ebx"    \
        "mov    edx,es"     \
        "pop    es"         \
    __parm      [__al] \
    __value     [__dx __eax] \
    __modify    [__ebx]
#endif

extern void DosSetVect( char, void (__interrupt *)( void ) );
#ifdef _M_I86
#pragma aux DosSetVect = \
        "mov    ah,25h" \
        "int 21h"       \
    __parm      [__al] [__ds __dx] \
    __value     \
    __modify    [__ah]
#else
#pragma aux DosSetVect = \
        "push   ds"     \
        "push   fs"     \
        "pop    ds"     \
        "mov    ah,25h" \
        "int 21h"       \
        "pop    ds"     \
    __parm      [__al] [__fs __edx] \
    __value     \
    __modify    [__ah]
#endif

extern int DoSpawn( void *, void * );
#pragma aux DoSpawn = \
        "push   ds"     \
        "push   es"     \
        "push   si"     \
        "push   di"     \
        "mov    ds,dx"    /*  exe segment */ \
        "mov    dx,ax"    /*  exe offset */ \
        "mov    es,cx"    /*  parm block segment (offset in bx already) */ \
        "mov    ax,4b00h" /*  exec process */ \
        "int 21h"       \
        "jc short rcisright" \
        "mov    ax,4d00h" \
        "int 21h"       \
        "xor    ah,ah"  \
    "rcisright:"        \
        "pop    di"     \
        "pop    si"     \
        "pop    es"     \
        "pop    ds"     \
    __parm      [__dx __ax] [__cx __bx] \
    __value     [__ax] \
    __modify    []


extern int GetFcb( void *, void * );
#pragma aux GetFcb = \
        "push   ds"     \
        "push   es"     \
        "push   si"     \
        "push   di"     \
        "mov    ds,dx"    /*  exe segment */ \
        "mov    si,ax"    /*  exe offset */ \
        "mov    es,cx"    /*  parm block segment (offset in bx already) */ \
        "mov    di,bx"    \
        "mov    ax,2901h" /*  parse filename/get fcb */ \
        "int 21h"       \
        "pop    di"     \
        "pop    si"     \
        "pop    es"     \
        "pop    ds"     \
    __parm      [__dx __ax] [__cx __bx] \
    __value     [__ax] \
    __modify    []

extern unsigned DosMaxAlloc( void );
#ifdef _M_I86
#pragma aux DosMaxAlloc = \
        "xor    bx,bx"  \
        "dec    bx"     \
        "mov    ah,48h" \
        "int 21h"       \
    __parm      [] \
    __value     [__bx] \
    __modify    [__ax]
#else
#pragma aux DosMaxAlloc = \
        "xor    ebx,ebx"    \
        "dec    ebx"        \
        "mov    ah,48h"     \
        "int 21h"           \
    __parm      [] \
    __value     [__ebx] \
    __modify    [__eax]
#endif

extern unsigned short CheckRemovable( unsigned char );
#pragma aux CheckRemovable = \
        "mov  ax,4408h"     \
        "int 21h"           \
        "cmp  ax,0fh"       \
        "jne short ok"      \
        "xor  ax,ax"        \
        "jmp short done"    \
    "ok: inc  ax"           \
    "done:"                 \
    __parm      [__bl] \
    __value     [__ax] \
    __modify    []

#if defined( _M_IX86 ) && !defined( _M_I86 )
extern void LockMemory( void __far *, long size );
#pragma aux LockMemory = \
        "push es"       \
        "mov  ax,gs"    \
        "mov  es,ax"    \
        "mov  ax,252bh" \
        "mov  bh,5"     \
        "mov  bl,1"     \
        "int 21h"       \
        "pop  es"       \
    __parm      [__gs __ecx] [__edx] \
    __value     \
    __modify    [__bx]
#endif

#endif  /* defined( __DOS__ ) || defined( __WINDOWS__ ) */

#endif  /* _PRAGMAS_H_INCLUDED */
