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
* Description:  Implementation of _control87().
*
****************************************************************************/


#include "variety.h"
#include <math.h>
#include <float.h>
#include "rtdata.h"

typedef unsigned short __based(__segname("_STACK"))     *cwp;

extern  void    __fstcw( cwp cw );
extern  void    __fldcw( cwp cw );

#if defined(__WINDOWS__) && !defined(__WINDOWS_386__)

extern void __far _fpmath( void );
#pragma aux _fpmath "__fpmath";

void __win87em_fldcw( unsigned int );
#pragma aux __win87em_fldcw = \
        "push   bx"                                     \
        "mov    bx, 4h"                                 \
        "call   far ptr _fpmath"                        \
        "pop    bx"                                     \
        parm [ax]

unsigned int __win87em_fstcw( void );
#pragma aux __win87em_fstcw = \
        "push   bx"                                     \
        "mov    bx, 5h"                                 \
        "call   far ptr _fpmath"                        \
        "pop    bx"                                     \
        value [ax]

#elif defined( __DOS_086__ )

extern unsigned char __dos87real;
#pragma aux __dos87real "*";

extern unsigned short __dos87emucall;
#pragma aux __dos87emucall "*";

void _WCI86NEAR __dos_emu_fldcw( cwp );
#pragma aux __dos_emu_fldcw "*" = \
        "mov    ax,3" \
        "call   __dos87emucall" \
        parm [bx];

void _WCI86NEAR __dos_emu_fstcw( cwp );
#pragma aux __dos_emu_fstcw "*" = \
        "mov    ax,4" \
        "call   __dos87emucall" \
        parm [bx];

#endif

#if defined(__386__)
#pragma aux __fstcw =       \
        "fstcw ss:[ecx]"    \
        parm caller [ecx];
#pragma aux __fldcw =       \
        "fldcw ss:[ecx]"    \
        parm caller [ecx];
#else
#pragma aux __fstcw =   \
        "xchg ax,bp"    \
        "fstcw [bp]"    \
        "fwait"         \
        "xchg ax,bp"    \
        parm caller [ax];
#pragma aux __fldcw =   \
        "xchg ax,bp"    \
        "fldcw [bp]"    \
        "xchg ax,bp"    \
        parm caller [ax];
#endif

_WCRTLINK unsigned _control87( unsigned new, unsigned mask )
/**********************************************************/
{
    unsigned short  control_word;

    control_word = 0;
    if( _RWD_8087 ) {
#if defined(__WINDOWS__) && !defined(__WINDOWS_386__)
        __fstcw( &control_word );
        control_word = __win87em_fstcw();
        if( mask != 0 ) {
            control_word = (control_word & ~mask) | (new & mask);
            __fldcw( &control_word );
            __win87em_fldcw(control_word);
        }
#elif defined( __DOS_086__ )
        if( __dos87real ) {
            __fstcw( &control_word );
            if( mask != 0 ) {
                control_word = (control_word & ~mask) | (new & mask);
                __fldcw( &control_word );
            }
        }
        if( __dos87emucall ) {
            __dos_emu_fstcw( &control_word );
            if( mask != 0 ) {
                control_word = (control_word & ~mask) | (new & mask);
                __dos_emu_fldcw( &control_word );
            }
        }
#else
        __fstcw( &control_word );
        if( mask != 0 ) {
            control_word = (control_word & ~mask) | (new & mask);
            __fldcw( &control_word );
        }
#endif
    }
    return( control_word );
}
