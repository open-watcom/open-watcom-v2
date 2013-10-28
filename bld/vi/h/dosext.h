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



#ifndef _DOSEXT_INCLUDED

struct ByteRegisters {
        unsigned char   al, ah;
        unsigned char   bl, bh;
        unsigned char   cl, ch;
        unsigned char   dl, dh;
};

struct WordRegisters {
        unsigned short  ax;
        unsigned short  bx;
        unsigned short  cx;
        unsigned short  dx;
        unsigned short  bp;
        unsigned short  si;
        unsigned short  di;
        unsigned short  ds;
        unsigned short  es;
        unsigned short  flags;
};

typedef union Registers {
        struct ByteRegisters    h;
        struct WordRegisters    x;
} Registers;

typedef struct InterruptData {
        short       interrupt_no;
        Registers   regs;
} InterruptData;

#define FLAG_CARRY          0x0001
#define FLAG_PARITY         0x0004
#define FLAG_AUX_CARRY      0x0010
#define FLAG_ZERO           0x0040
#define FLAG_SIGN           0x0080
#define FLAG_TRACE          0x0100
#define FLAG_INTERRUPT      0x0200
#define FLAG_DIRECTION      0x0400
#define FLAG_OVERFLOW       0x0800

void    InterruptCPU( void );

#ifndef __WATCOMC__
    #define MK_FP(__s, __o) ((void __far *) (((unsigned long)(__s) << 16) | (unsigned) (__o)))
#endif

#endif
