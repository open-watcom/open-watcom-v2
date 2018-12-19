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


#include "pcscrnio.h"


extern void BIOSCharSet( unsigned char, unsigned char, unsigned, unsigned, unsigned, unsigned  );
#pragma aux BIOSCharSet = \
        "xchg bp,si"    \
        "mov  ah,11h"   \
        "xor  bl,bl"    \
        "int 10h"       \
        "xchg bp,si"    \
    __parm              [__al] [__bh] [__cx] [__dx] [__es] [__si] \
    __value             \
    __modify __exact    [__ah __bl]

/* get video save size */
extern unsigned _VidStateSize( unsigned );
#pragma aux _VidStateSize = \
        "mov  ax,1c00h" \
        "int 10h"       \
        "cmp  al,1ch"   \
        "je short L1"   \
        "xor  bx,bx"    \
    "L1:"               \
    __parm [__cx] \
    __value [__bx] \
    __modify __exact [__ax __bx]

/* save video state */
extern void _VidStateSave( unsigned, unsigned, unsigned );
#pragma aux _VidStateSave = \
        "mov  ax,1c01h" \
        "int 10h"       \
    __parm              [__cx] [__es] [__bx] \
    __value             \
    __modify __exact    [ax]

/* restore video state */
extern void _VidStateRestore( unsigned, unsigned, unsigned );
#pragma aux _VidStateRestore = \
        "mov  ax,1c02h" \
        "int 10h"       \
    __parm              [__cx] [__es] [__bx] \
    __value             \
    __modify __exact    [__ax]

extern unsigned MouseStateSize( void );
#pragma aux MouseStateSize = \
        "sub  bx,bx"    \
        "mov  ax,15h"   \
        "int 33h"       \
    __parm              [] \
    __value             [__bx] \
    __modify __exact    [__ax __bx]


extern void MouseStateSave( unsigned, unsigned, unsigned );
#pragma aux MouseStateSave =    \
        "mov  ax,16h"   \
        "int 33h"       \
    __parm              [__es] [__dx] [__bx] \
    __value             \
    __modify __exact    [__ax]


extern void MouseStateRestore( unsigned, unsigned, unsigned );
#pragma aux MouseStateRestore = \
        "mov  ax,17h"   \
        "int 33h"       \
    __parm              [__es] [__dx] [__bx] \
    __value             \
    __modify __exact    [__ax]

#if 0
extern void vertsync( void );
#pragma aux vertsync =  \
        "mov dx,3dah"   \
    "L1: in al,dx"      \
        "test al,8"     \
        "jz short L1"   \
    __parm              [] \
    __value             \
    __modify __exact    [__al __dx]
#endif

extern unsigned char    ActFontTbls;
